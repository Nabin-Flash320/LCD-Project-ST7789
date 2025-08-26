

#include "lvgl.h"
#include "esp_wifi.h"

#include "main_ui.h"

DEFINE_OBJECT(wifi_scan_result);

DEFINE_STYLE(wifi_scan_result);

void main_ui_initialize_wifi_scan_result_widget(lv_obj_t *parent)
{
    if (NULL == parent)
    {
        return;
    }

    object_wifi_scan_result = lv_table_create(parent);
    LV_ASSERT(object_wifi_scan_result);
    lv_obj_set_size(object_wifi_scan_result, 320, 200);
    lv_table_set_column_width(object_wifi_scan_result, 0, 315);

    MALLOC_STYLE(style_wifi_scan_result);
    LV_ASSERT(style_wifi_scan_result);
    lv_style_init(style_wifi_scan_result);
    lv_style_set_border_width(style_wifi_scan_result, 2);
    lv_style_set_border_color(style_wifi_scan_result, COLOR_SLATE_GRAY());

    lv_obj_add_style(object_wifi_scan_result, style_wifi_scan_result, LV_PART_MAIN);
}

void main_ui_set_wifi_scan_result(void *wifi_scan_result, size_t len)
{
    if (!wifi_scan_result || len == 0)
    {
        return;
    }

    wifi_ap_record_t *wifi_record = (wifi_ap_record_t *)wifi_scan_result;
    LV_ASSERT(object_wifi_scan_result);
    int col_idx = 0;
    for (int i = 0; i < len; i++)
    {
        if (0 != lv_strlen((const char *)wifi_record[i].ssid))
        {
            printf("HERE(ssid: %s)\n", wifi_record[i].ssid);
            lv_table_set_cell_value(object_wifi_scan_result, col_idx, 0, (const char *)wifi_record[i].ssid);
            col_idx++;
        }
    }
}
