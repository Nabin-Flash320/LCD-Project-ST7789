
#include "lvgl.h"
#include "esp_log.h"

#include "main_ui.h"

#define TAG "BLE-Tile"

DEFINE_OBJECT(primary);
DEFINE_OBJECT(ble_tile);

DEFINE_STYLE(ble_tile);

void tile_ui_create_ble_tile()
{
    object_primary = main_ui_get_main_menu_object();
    LV_ASSERT(object_primary);

    object_ble_tile = lv_obj_create(object_primary);
    LV_ASSERT(object_ble_tile);
    lv_obj_set_grid_cell(object_ble_tile, LV_GRID_ALIGN_STRETCH, 0, 4, LV_GRID_ALIGN_STRETCH, 2, 2);

    MALLOC_STYLE(style_ble_tile);
    LV_ASSERT(style_ble_tile);
    lv_style_init(style_ble_tile);
    lv_style_set_border_width(style_ble_tile, 1);
    lv_style_set_border_color(style_ble_tile, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_ble_tile, style_ble_tile, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(object_ble_tile);
    lv_label_set_text(label, LV_SYMBOL_BLUETOOTH);
}
