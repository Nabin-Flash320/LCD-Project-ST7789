#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>
#include <nvs_flash.h>
#include <string.h>

#include "driver_init.h"
#include "lvgl_demo_ui.h"

#define LCD_H_RES 320
#define LCD_V_RES 240
#define LCD_HOST SPI2_HOST
#define TAG "esp_lcd"

static void nvs_initialize();

void app_main(void)
{
    lcd_init();
    lvgl_init();
    touch_init();

    nvs_initialize();

    xTaskCreate(lvgl_task, "lvgl_task", 4096, NULL, 5, NULL);

    lv_button_test();
}

static void nvs_initialize()
{

    esp_err_t error = nvs_flash_init();
    if ((ESP_ERR_NVS_NO_FREE_PAGES == error) || (ESP_ERR_NVS_NEW_VERSION_FOUND == error))
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        error = nvs_flash_init();
    }
    return;
}
