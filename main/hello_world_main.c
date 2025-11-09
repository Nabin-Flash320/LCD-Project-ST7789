
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>
#include <nvs_flash.h>
#include <string.h>

#include "driver_init.h"

#include "device_event_handler.h"
#include "driver_init.h"
#include "main_ui.h"
#include "wifi_handler.h"

static void device_event_handler_callback(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                                          void* event_data);

static void app_main_UI_starter(void* params)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGE(__FILE__, "Initializing NVS flash");
    esp_err_t error = nvs_flash_init();
    if ((ESP_ERR_NVS_NO_FREE_PAGES == error) || (ESP_ERR_NVS_NEW_VERSION_FOUND == error))
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        error = nvs_flash_init();
    }

    main_ui_initialize();

    while (1)
    {
        uint32_t time_till_next = lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(time_till_next > 0 ? time_till_next : 5));
    }

    vTaskDelete(NULL);
}

static void device_event_handler_callback(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                                          void* event_data)
{
    if (DEVICE_EVENT == event_base)
    {
        struct s_event_data* data = (struct s_event_data*) event_data;
        {
            switch (event_id)
            {
            case DEVICE_EVENT_WIFI_CONNECTED_EVENT:
            {
                main_ui_set_wifi_status(*(int*) data->data == 0 ? true : false);
                xTaskCreate(wifi_sta_scan_aps, "wifi-scanner", 4096, NULL, 2, NULL);
                break;
            }
            case DEVICE_EVENT_SET_MESSAGE_EVENT:
            {
                main_ui_set_message((char*) data->data);
                break;
            }
            case DEVICE_EVENT_WIFI_AP_DETAILS_EVENT:
            {
                main_ui_set_wifi_scan_result((char*) data->data, data->data_len);
                break;
            }
            default:
            {
                ESP_LOGE(__FILE__, "Unhandled event id(%ld)", event_id);
                break;
            }
            }
        }
    }
}

void app_main()
{
    lcd_driver_init();
    lvgl_init();
    touch_driver_init();
    touch_init();

    device_event_initialize(device_event_handler_callback);
    xTaskCreate(app_main_UI_starter, "ui-starter", 4096 * 2, NULL, 3, NULL);
    wifi_sta_connect();
}
