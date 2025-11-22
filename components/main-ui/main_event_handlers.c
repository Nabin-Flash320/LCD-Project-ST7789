
#include "lvgl.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "wifi_handler.h"
#include "main_ui.h"

#define TAG "MainEventHandler"

void event_handler_wifi_setting(lv_event_t *event)
{
    wifi_ui_start();
}

void event_handler_wifi_list_button_click(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);

    if (LV_EVENT_CLICKED == code)
    {
        lv_obj_t *object = (lv_obj_t *)lv_event_get_target(event);
        LV_ASSERT(object);

        lv_obj_t *wifi_button_list = (lv_obj_t *)lv_event_get_user_data(event);
        LV_ASSERT(wifi_button_list);

        const char *data = lv_list_get_button_text(wifi_button_list, object);
        wifi_ui_connect_wifi(data);
    }
}

void event_handler_bluetooth_setting(lv_event_t *event)
{
    const char *code_name = lv_event_code_get_name(lv_event_get_code(event));

    ESP_LOGE(TAG, "Bluetooth setting event code %s", code_name);
}
