
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

void event_handler_bluetooth_setting(lv_event_t *event)
{
    const char *code_name = lv_event_code_get_name(lv_event_get_code(event));

    ESP_LOGE(TAG, "Bluetooth setting event code %s", code_name);
}
