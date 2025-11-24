
#include "lvgl.h"
#include "esp_log.h"

#include "main_ui.h"

#define TAG "WiFi-Tile"

DEFINE_OBJECT(primary);
DEFINE_OBJECT(wifi_tile);

DEFINE_STYLE(wifi_tile);

void tile_ui_create_wifi_tile()
{
    object_primary = main_ui_get_main_menu_object();
    LV_ASSERT(object_primary);

    object_wifi_tile = lv_obj_create(object_primary);
    LV_ASSERT(object_wifi_tile);
    lv_obj_set_grid_cell(object_wifi_tile, LV_GRID_ALIGN_STRETCH, 0, 4, LV_GRID_ALIGN_STRETCH, 0, 2);

    MALLOC_STYLE(style_wifi_tile);
    LV_ASSERT(style_wifi_tile);
    lv_style_init(style_wifi_tile);
    lv_style_set_border_width(style_wifi_tile, 1);
    lv_style_set_border_color(style_wifi_tile, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_wifi_tile, style_wifi_tile, LV_PART_MAIN);


    lv_obj_t *label = lv_label_create(object_wifi_tile);
    lv_label_set_text(label, LV_SYMBOL_WIFI);
}
