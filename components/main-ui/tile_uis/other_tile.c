
#include "lvgl.h"
#include "esp_log.h"

#include "main_ui.h"

#define TAG "Other-Tile"

DEFINE_OBJECT(primary);
DEFINE_OBJECT(other_tile);

DEFINE_STYLE(other_tile);

void tile_ui_create_other_tile()
{
    object_primary = main_ui_get_main_menu_object();
    LV_ASSERT(object_primary);

    object_other_tile = lv_obj_create(object_primary);
    LV_ASSERT(object_other_tile);
    lv_obj_set_grid_cell(object_other_tile, LV_GRID_ALIGN_STRETCH, 0, 4, LV_GRID_ALIGN_STRETCH, 4, 2);

    MALLOC_STYLE(style_other_tile);
    LV_ASSERT(style_other_tile);
    lv_style_init(style_other_tile);
    lv_style_set_border_width(style_other_tile, 1);
    lv_style_set_border_color(style_other_tile, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_other_tile, style_other_tile, LV_PART_MAIN);
}
