/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_types.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_panel_ops.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lvgl.h"
#include "touch_xpt2046.h"

#include "device_event_handler.h"
#include "wifi_handler.h"
#include "main_ui.h"

#define LCD_H_RES 320
#define LCD_V_RES 240

static void touch_driver_read(lv_indev_t *drv, lv_indev_data_t *data); // This is not used, remove it later
static void screen_touch_event_handler(lv_event_t *event);
static void lcd_flush_callback(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void lcd_wait_callback(lv_display_t *disp);
static void lcd_lvgl_timer_callback(void *args);
static void device_event_handler_callback(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

static spi_bus_config_t lcd_st7789_buscfg = {
    .sclk_io_num = 14,
    .mosi_io_num = 13,
    .miso_io_num = 12,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t), // transfer 80 lines of pixels (assume pixel is RGB565) at most in one SPI transaction
};

static spi_bus_config_t touch_xpt2056_buscfg = {
    .sclk_io_num = 25,
    .mosi_io_num = 32,
    .miso_io_num = 39,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 32,
};

static esp_lcd_panel_io_spi_config_t io_config = {
    .dc_gpio_num = 2,
    .cs_gpio_num = 15,
    .pclk_hz = 20 * 1000 * 1000,
    .lcd_cmd_bits = 8,
    .lcd_param_bits = 8,
    .spi_mode = 0,
    .trans_queue_depth = 10,
};

static esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = -1,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
    .bits_per_pixel = 16,
};

const esp_timer_create_args_t lvgl_timer_tick = {
    .callback = lcd_lvgl_timer_callback,
    .name = "lgvl_tick",
};

static esp_lcd_panel_io_handle_t lcd_io_handle = NULL;
static esp_lcd_panel_handle_t lcd_panel_handle = NULL;

static esp_timer_handle_t lcd_lvgl_timer_handle;
static SemaphoreHandle_t lvgl_semaphore_handle = NULL;

static esp_lcd_touch_handle_t tp = NULL;
static esp_lcd_panel_io_handle_t tp_io_handle = NULL;
static esp_lcd_panel_io_spi_config_t tp_io_config = ESP_LCD_TOUCH_IO_SPI_XPT2046_CONFIG(33);

lv_indev_t *touch_screen_device = NULL;

void touch_interrupt_cb(esp_lcd_touch_t *touch_panel)
{
    uint16_t x[1], y[1], strength[1];
    uint8_t count = 0;

    ESP_ERROR_CHECK(esp_lcd_touch_read_data(tp));
    if (esp_lcd_touch_get_coordinates(touch_panel, x, y, strength, &count, 1))
    {
        esp_rom_printf("Touch event detected!(x: %ld, y: %ld)(strength: %d)\n", x[0], y[0], strength[0]);
        lv_indev_send_event(touch_screen_device, LV_EVENT_PRESSED, NULL);
    }
}

static esp_lcd_touch_config_t tp_cfg = {
    .x_max = LCD_V_RES,
    .y_max = LCD_H_RES,
    .rst_gpio_num = -1,
    .int_gpio_num = 36,
    .interrupt_callback = touch_interrupt_cb, // Touch interrupt callback
    .flags = {
        .swap_xy = 0,
        .mirror_x = 0,
        .mirror_y = 0,
    },
};

static void app_main_UI_starter(void *params)
{

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGE(__FILE__, "Initializing NVS flash");
    esp_err_t error = nvs_flash_init();
    if ((ESP_ERR_NVS_NO_FREE_PAGES == error) || (ESP_ERR_NVS_NEW_VERSION_FOUND == error))
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        error = nvs_flash_init();
    }

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &lcd_st7789_buscfg, SPI_DMA_CH_AUTO)); // Enable the DMA feature
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &lcd_io_handle));
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(lcd_io_handle, &panel_config, &lcd_panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_init(lcd_panel_handle));
    esp_lcd_panel_swap_xy(lcd_panel_handle, true);
    esp_lcd_panel_mirror(lcd_panel_handle, true, false);
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd_panel_handle, true));

    // Turning on the backlight
    ESP_ERROR_CHECK(gpio_set_direction(21, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(21, 1));

    // ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &touch_xpt2056_buscfg, SPI_DMA_CH_AUTO));
    // ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI3_HOST, &tp_io_config, &tp_io_handle));
    // ESP_ERROR_CHECK(esp_lcd_touch_new_spi_xpt2046(tp_io_handle, &tp_cfg, &tp));

    lvgl_semaphore_handle = xSemaphoreCreateBinary();
    lv_init();

    lv_display_t *display = lv_display_create(LCD_H_RES, LCD_V_RES);
    size_t display_buffer_size = LCD_H_RES * (240 / 10) * sizeof(lv_color16_t);

    void *buffer_1 = heap_caps_calloc(sizeof(lv_color16_t), display_buffer_size, MALLOC_CAP_DMA);
    assert(buffer_1);
    void *buffer_2 = heap_caps_calloc(sizeof(lv_color16_t), display_buffer_size, MALLOC_CAP_DMA);
    assert(buffer_2);

    lv_display_set_buffers(display, buffer_1, buffer_2, display_buffer_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_user_data(display, lcd_panel_handle);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(display, lcd_flush_callback);
    lv_display_set_flush_wait_cb(display, lcd_wait_callback);

    // touch_screen_device = lv_indev_create();
    // lv_indev_set_type(touch_screen_device, LV_INDEV_TYPE_POINTER);
    // lv_indev_add_event_cb(touch_screen_device, screen_touch_event_handler, LV_EVENT_PRESSED, NULL);
    // lv_indev_set_read_cb(touch_screen_device, touch_driver_read);

    ESP_ERROR_CHECK(esp_timer_create(&lvgl_timer_tick, &lcd_lvgl_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lcd_lvgl_timer_handle, 1 * 1000));

    xSemaphoreGive(lvgl_semaphore_handle);

    main_ui_initialize();

    while (1)
    {
        if (pdTRUE == xSemaphoreTake(lvgl_semaphore_handle, portMAX_DELAY))
        {
            lv_timer_handler();
            xSemaphoreGive(lvgl_semaphore_handle);
        }

        vTaskDelay(1);
    }

    vTaskDelete(NULL);
}

void app_main()
{
    device_event_initialize(device_event_handler_callback);
    xTaskCreate(app_main_UI_starter, "ui-starter", 4096 * 2, NULL, 3, NULL);
    wifi_sta_connect();
}

static void device_event_handler_callback(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (DEVICE_EVENT == event_base)
    {
        struct s_event_data *data = (struct s_event_data *)event_data;
        if (pdTRUE == xSemaphoreTake(lvgl_semaphore_handle, portMAX_DELAY))
        {
            switch (event_id)
            {
            case DEVICE_EVENT_WIFI_CONNECTED_EVENT:
            {
                main_ui_set_wifi_status(*(int *)data->data == 0 ? true : false);
                xTaskCreate(wifi_sta_scan_aps, "wifi-scanner", 4096, NULL, 2, NULL);
                break;
            }
            case DEVICE_EVENT_SET_MESSAGE_EVENT:
            {
                main_ui_set_message((char *)data->data);
                break;
            }
            case DEVICE_EVENT_WIFI_AP_DETAILS_EVENT:
            {
                main_ui_set_wifi_scan_result((char *)data->data, data->data_len);
                break;
            }
            default:
            {
                ESP_LOGE(__FILE__, "Unhandled event id(%ld)", event_id);
                break;
            }
            }

            xSemaphoreGive(lvgl_semaphore_handle);
        }
    }
}

static void screen_touch_event_handler(lv_event_t *event)
{
    esp_rom_printf("Hello world!\n");
}

static void touch_driver_read(lv_indev_t *drv, lv_indev_data_t *data)
{
    uint16_t x[1];
    uint16_t y[1];
    printf("Hello world!\n");

    uint16_t strength[1];
    uint8_t count = 0;

    // Update touch point data.
    ESP_ERROR_CHECK(esp_lcd_touch_read_data(tp));

    data->state = LV_INDEV_STATE_REL;

    if (esp_lcd_touch_get_coordinates(tp, x, y, strength, &count, 1))
    {
        data->point.x = x[0];
        data->point.y = y[0];
        data->state = LV_INDEV_STATE_PR;
        printf("Touched: (x: %ld and y: %ld) strength: %d\n", data->point.x, data->point.y, strength[0]);
    }

    data->continue_reading = false;
}

static void lcd_flush_callback(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // because SPI LCD is big-endian, we need to swap the RGB bytes order
    lv_draw_sw_rgb565_swap(px_map, (offsetx2 + 1 - offsetx1) * (offsety2 + 1 - offsety1));
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
}

static void lcd_wait_callback(lv_display_t *disp)
{
    vTaskDelay(1 / portTICK_PERIOD_MS);
}

static void lcd_lvgl_timer_callback(void *args)
{
    lv_tick_inc(1);
}