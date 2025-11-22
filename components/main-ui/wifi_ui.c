
#include "esp_log.h"
#include "lvgl.h"
#include "esp_wifi_types.h"

#include "main_ui.h"
#include "wifi_handler.h"

#define TAG "WiFiEvenHandler"

static void wifi_scan_result_handler(uint16_t scan_number, wifi_ap_record_t *record);
static void wifi_list_timer_cb(lv_timer_t *timer);

DEFINE_OBJECT(primary);
DEFINE_OBJECT(wifi_scan_list);
DEFINE_OBJECT(message_box);

static lv_timer_t *wifi_list_timer = NULL;
static uint8_t wifi_list_timoeut_counter = 60;

void wifi_ui_start()
{
    object_primary = main_ui_get_main_menu_object();
    LV_ASSERT(object_primary);

    wifi_list_timer = lv_timer_create(wifi_list_timer_cb, 1000, NULL);

    object_message_box = lv_msgbox_create(object_primary);
    LV_ASSERT(object_message_box);
    lv_msgbox_add_title(object_message_box, "WiFi");
    lv_msgbox_add_text(object_message_box, "WiFi Scanning...");

    wifi_handler_start_scan(wifi_scan_result_handler);
}

void wifi_ui_connect_wifi(const char *SSID)
{
    wifi_list_timoeut_counter = 60;

    assert(SSID);
    
    ESP_LOGE(TAG, "WiFi button clicked(%s)", SSID);
}

static void wifi_scan_result_handler(uint16_t scan_number, wifi_ap_record_t *record)
{
    wifi_list_timoeut_counter = 60;

    lv_lock();

    lv_obj_delete(object_message_box);

    object_wifi_scan_list = lv_list_create(object_primary);
    lv_obj_set_size(object_wifi_scan_list, lv_obj_get_width(object_primary), lv_obj_get_height(object_primary));

    lv_obj_t *button;
    for (int i = 0; i < scan_number; i++)
    {
        ESP_LOGE(TAG, "SSID: %s(rssi: %d)(authmode: %d)", record[i].ssid, record[i].rssi, record[i].authmode);

        button = lv_list_add_button(object_wifi_scan_list, NULL, (const char *)record[i].ssid);
        lv_obj_add_event_cb(button, event_handler_wifi_list_button_click, LV_EVENT_CLICKED, object_wifi_scan_list);
    }

    lv_unlock();
}

static void wifi_list_timer_cb(lv_timer_t *timer)
{
    lv_lock();
    if (wifi_list_timoeut_counter-- == 0)
    {
        ESP_LOGE(TAG, "Timeout occured, exiting from wifi setting");
        lv_obj_del(object_wifi_scan_list);
        lv_timer_del(wifi_list_timer);
        wifi_list_timoeut_counter = 60;
    }
    else
    {
        ESP_LOGE(TAG, "Timeout at: %d", wifi_list_timoeut_counter);
    }

    lv_unlock();
}
