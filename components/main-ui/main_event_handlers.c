
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

void event_handler_keyboard_events(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    const char *code_name = lv_event_code_get_name(code);
    ESP_LOGE(TAG, "Event code %s", code_name);
}

void event_handler_switch_events(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    const char *code_name = lv_event_code_get_name(code);

    lv_obj_t *object = lv_event_get_current_target(event);
    assert(object);

    s_common_ui_callback_user_data_t *switch_user_data = lv_event_get_user_data(event);
    assert(switch_user_data);

    ESP_LOGE(TAG, "Switch event %s for %s", code_name, switch_user_data->name);
    switch_user_data->messages.switch_state = lv_obj_has_state(object, LV_STATE_CHECKED) ? true : false;
    if (switch_user_data->callback)
    {
        switch_user_data->callback();
    }
}

void event_handler_bluetooth_setting(lv_event_t *event)
{
    const char *code_name = lv_event_code_get_name(lv_event_get_code(event));
    ESP_LOGE(TAG, "Bluetooth setting event code %s", code_name);
}
