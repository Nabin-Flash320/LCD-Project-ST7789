
#include "lvgl.h"
#include "esp_log.h"

#include "main_ui.h"

#define TAG "bluetooth-Tile"

DEFINE_OBJECT(primary);
DEFINE_OBJECT(bluetooth_tile);
DEFINE_OBJECT(bluetooth_icon);
DEFINE_OBJECT(bluetooth_title);
DEFINE_OBJECT(bluetooth_enable_button);
DEFINE_OBJECT(bluetooth_ssid_display);
DEFINE_OBJECT(bluetooth_open_button);

DEFINE_STYLE(bluetooth_tile);
DEFINE_STYLE(bluetooth_icon);
DEFINE_STYLE(bluetooth_title);
DEFINE_STYLE(bluetooth_enable_button);
DEFINE_STYLE(bluetooth_ssid_display);
DEFINE_STYLE(bluetooth_open_button);

static void create_bluetooth_icon_cell();
static void create_bluetooth_title_cell();
static void create_bluetooth_enable_button_cell();
static void create_bluetooth_connected_device_cell();
static void create_bluetooth_open_bluetooth_config_cell();

static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

LV_IMAGE_DECLARE(bluetooth_on);

void tile_ui_create_bluetooth_tile()
{
    object_primary = main_ui_get_main_menu_object();
    LV_ASSERT(object_primary);

    object_bluetooth_tile = lv_obj_create(object_primary);
    LV_ASSERT(object_bluetooth_tile);
    lv_obj_set_grid_cell(object_bluetooth_tile, LV_GRID_ALIGN_STRETCH, 0, 4, LV_GRID_ALIGN_STRETCH, 2, 2);

    // Tile also has grid layout
    lv_obj_set_layout(object_bluetooth_tile, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_row_dsc_array(object_bluetooth_tile, row_dsc, 0);
    lv_obj_set_style_grid_column_dsc_array(object_bluetooth_tile, col_dsc, 0);

    MALLOC_STYLE(style_bluetooth_tile);
    LV_ASSERT(style_bluetooth_tile);
    lv_style_init(style_bluetooth_tile);
    lv_style_set_border_width(style_bluetooth_tile, 1);
    lv_style_set_border_color(style_bluetooth_tile, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_bluetooth_tile, style_bluetooth_tile, LV_PART_MAIN);

    create_bluetooth_icon_cell();
    create_bluetooth_title_cell();
    create_bluetooth_enable_button_cell();
    create_bluetooth_connected_device_cell();
    create_bluetooth_open_bluetooth_config_cell();
}

static void create_bluetooth_icon_cell()
{
    LV_ASSERT(object_bluetooth_tile);

    object_bluetooth_icon = lv_image_create(object_bluetooth_tile);
    LV_ASSERT(object_bluetooth_icon);
    lv_image_set_src(object_bluetooth_icon, &bluetooth_on);
    lv_obj_set_style_image_recolor(object_bluetooth_icon, lv_color_hex(0x00AADD), LV_STATE_DEFAULT);
    lv_obj_set_grid_cell(object_bluetooth_icon, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 2);

    MALLOC_STYLE(style_bluetooth_icon);
    LV_ASSERT(style_bluetooth_icon);
    lv_style_init(style_bluetooth_icon);
    lv_style_set_border_width(style_bluetooth_icon, 1);
    lv_style_set_radius(style_bluetooth_icon, 8);
    lv_style_set_border_color(style_bluetooth_icon, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_bluetooth_icon, style_bluetooth_icon, LV_PART_MAIN);
}

static void create_bluetooth_title_cell()
{
    LV_ASSERT(object_bluetooth_tile);

    object_bluetooth_title = lv_label_create(object_bluetooth_tile);
    LV_ASSERT(object_bluetooth_title);
    lv_obj_set_grid_cell(object_bluetooth_title, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    MALLOC_STYLE(style_bluetooth_title);
    LV_ASSERT(style_bluetooth_title);
    lv_style_init(style_bluetooth_title);
    lv_style_set_border_width(style_bluetooth_title, 0);
    lv_style_set_border_color(style_bluetooth_title, COLOR_MAKE(0, 0, 0));
    lv_style_set_pad_all(style_bluetooth_title, 0);
    lv_style_set_align(style_bluetooth_title, LV_ALIGN_LEFT_MID);

    lv_obj_add_style(object_bluetooth_title, style_bluetooth_title, LV_PART_MAIN);

    lv_label_set_text(object_bluetooth_title, "BLE");
}

static void create_bluetooth_enable_button_cell()
{
    LV_ASSERT(object_bluetooth_tile);

    object_bluetooth_enable_button = lv_switch_create(object_bluetooth_tile);
    LV_ASSERT(object_bluetooth_enable_button);
    lv_obj_set_grid_cell(object_bluetooth_enable_button, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    MALLOC_STYLE(style_bluetooth_enable_button);
    LV_ASSERT(style_bluetooth_enable_button);
    lv_style_init(style_bluetooth_enable_button);
    lv_style_set_border_width(style_bluetooth_enable_button, 0);
    lv_style_set_border_color(style_bluetooth_enable_button, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_bluetooth_enable_button, style_bluetooth_enable_button, LV_PART_MAIN);
}

static void create_bluetooth_connected_device_cell()
{
    LV_ASSERT(object_bluetooth_tile);

    object_bluetooth_ssid_display = lv_label_create(object_bluetooth_tile);
    LV_ASSERT(object_bluetooth_ssid_display);
    lv_obj_set_grid_cell(object_bluetooth_ssid_display, LV_GRID_ALIGN_STRETCH, 1, 3, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_label_set_text(object_bluetooth_ssid_display, "NABINs_BLEDevice");
    lv_label_set_long_mode(object_bluetooth_ssid_display, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(object_bluetooth_ssid_display, LV_TEXT_ALIGN_CENTER, 0);

    MALLOC_STYLE(style_bluetooth_ssid_display);
    LV_ASSERT(style_bluetooth_ssid_display);
    lv_style_init(style_bluetooth_ssid_display);
    lv_style_set_border_width(style_bluetooth_ssid_display, 0);
    lv_style_set_border_color(style_bluetooth_ssid_display, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_bluetooth_ssid_display, style_bluetooth_ssid_display, LV_PART_MAIN);
}

static void create_bluetooth_open_bluetooth_config_cell()
{
    LV_ASSERT(object_bluetooth_tile);

    object_bluetooth_open_button = lv_button_create(object_bluetooth_tile);
    LV_ASSERT(object_bluetooth_open_button);
    lv_obj_set_grid_cell(object_bluetooth_open_button, LV_GRID_ALIGN_STRETCH, 0, 4, LV_GRID_ALIGN_STRETCH, 2, 2);

    MALLOC_STYLE(style_bluetooth_open_button);
    LV_ASSERT(style_bluetooth_open_button);
    lv_style_init(style_bluetooth_open_button);
    lv_style_set_border_width(style_bluetooth_open_button, 1);
    lv_style_set_border_color(style_bluetooth_open_button, COLOR_MAKE(0, 0, 0));
    lv_style_set_shadow_width(style_bluetooth_open_button, 0);

    lv_obj_add_style(object_bluetooth_open_button, style_bluetooth_open_button, LV_PART_MAIN);

    lv_obj_t *button_label = lv_label_create(object_bluetooth_open_button);
    LV_ASSERT(button_label);

    lv_label_set_text(button_label, "Open Setting");
    lv_obj_center(button_label);
}