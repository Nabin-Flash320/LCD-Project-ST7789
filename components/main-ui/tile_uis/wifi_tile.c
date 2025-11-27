
#include "lvgl.h"
#include "esp_log.h"

#include "main_ui.h"

#define TAG "WiFi-Tile"

DEFINE_OBJECT(primary);
DEFINE_OBJECT(wifi_tile);
DEFINE_OBJECT(wifi_icon);
DEFINE_OBJECT(wifi_title);
DEFINE_OBJECT(wifi_enable_button);
DEFINE_OBJECT(wifi_ssid_display);
DEFINE_OBJECT(wifi_open_button);

DEFINE_STYLE(wifi_tile);
DEFINE_STYLE(wifi_icon);
DEFINE_STYLE(wifi_title);
DEFINE_STYLE(wifi_enable_button);
DEFINE_STYLE(wifi_ssid_display);
DEFINE_STYLE(wifi_open_button);

static void create_wifi_icon_cell();
static void create_wifi_title_cell();
static void create_wifi_enable_button_cell();
static void create_wifi_ssid_display_cell();
static void create_wifi_open_wifi_config_cell();

static void wifi_enable_button_clicked();

static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

LV_IMAGE_DECLARE(wifi_turned_off);
LV_IMAGE_DECLARE(wifi_fill_strength);

static s_common_ui_callback_user_data_t wifi_enable_button_data = {
    .callback = wifi_enable_button_clicked,
    .name = "Enable Wifi",
    .messages.switch_state = false,
};

void tile_ui_create_wifi_tile()
{
    object_primary = main_ui_get_main_menu_object();
    LV_ASSERT(object_primary);

    object_wifi_tile = lv_obj_create(object_primary);
    LV_ASSERT(object_wifi_tile);
    lv_obj_set_grid_cell(object_wifi_tile, LV_GRID_ALIGN_STRETCH, 0, 4, LV_GRID_ALIGN_STRETCH, 0, 2);

    // Tile also has grid layout
    lv_obj_set_layout(object_wifi_tile, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_row_dsc_array(object_wifi_tile, row_dsc, 0);
    lv_obj_set_style_grid_column_dsc_array(object_wifi_tile, col_dsc, 0);

    MALLOC_STYLE(style_wifi_tile);
    LV_ASSERT(style_wifi_tile);
    lv_style_init(style_wifi_tile);
    lv_style_set_border_width(style_wifi_tile, 1);
    lv_style_set_border_color(style_wifi_tile, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_wifi_tile, style_wifi_tile, LV_PART_MAIN);

    create_wifi_icon_cell();
    create_wifi_title_cell();
    create_wifi_enable_button_cell();
    create_wifi_ssid_display_cell();
    create_wifi_open_wifi_config_cell();
}

static void create_wifi_icon_cell()
{
    LV_ASSERT(object_wifi_tile);

    object_wifi_icon = lv_image_create(object_wifi_tile);
    LV_ASSERT(object_wifi_icon);
    lv_image_set_src(object_wifi_icon, &wifi_turned_off);
    lv_obj_set_style_image_recolor(object_wifi_icon, lv_color_hex(0x00AADD), LV_STATE_DEFAULT);
    lv_obj_set_grid_cell(object_wifi_icon, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 2);

    MALLOC_STYLE(style_wifi_icon);
    LV_ASSERT(style_wifi_icon);
    lv_style_init(style_wifi_icon);
    lv_style_set_border_width(style_wifi_icon, 1);
    lv_style_set_radius(style_wifi_icon, 8);
    lv_style_set_border_color(style_wifi_icon, COLOR_MAKE(0xC8, 0xC8, 0xC8));

    lv_obj_add_style(object_wifi_icon, style_wifi_icon, LV_PART_MAIN);
}

static void create_wifi_title_cell()
{
    LV_ASSERT(object_wifi_tile);

    object_wifi_title = lv_label_create(object_wifi_tile);
    LV_ASSERT(object_wifi_title);
    lv_obj_set_grid_cell(object_wifi_title, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    MALLOC_STYLE(style_wifi_title);
    LV_ASSERT(style_wifi_title);
    lv_style_init(style_wifi_title);
    lv_style_set_border_width(style_wifi_title, 0);
    lv_style_set_border_color(style_wifi_title, COLOR_MAKE(0, 0, 0));
    lv_style_set_pad_all(style_wifi_title, 0);
    lv_style_set_align(style_wifi_title, LV_ALIGN_LEFT_MID);

    lv_obj_add_style(object_wifi_title, style_wifi_title, LV_PART_MAIN);

    lv_label_set_text(object_wifi_title, "WiFi");
}

static void create_wifi_enable_button_cell()
{
    LV_ASSERT(object_wifi_tile);

    object_wifi_enable_button = lv_switch_create(object_wifi_tile);
    LV_ASSERT(object_wifi_enable_button);
    lv_obj_set_grid_cell(object_wifi_enable_button, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_add_event_cb(object_wifi_enable_button, event_handler_switch_events, LV_EVENT_VALUE_CHANGED, &wifi_enable_button_data);

    MALLOC_STYLE(style_wifi_enable_button);
    LV_ASSERT(style_wifi_enable_button);
    lv_style_init(style_wifi_enable_button);
    lv_style_set_border_width(style_wifi_enable_button, 0);
    lv_style_set_border_color(style_wifi_enable_button, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_wifi_enable_button, style_wifi_enable_button, LV_PART_MAIN);
}

static void create_wifi_ssid_display_cell()
{
    LV_ASSERT(object_wifi_tile);

    object_wifi_ssid_display = lv_label_create(object_wifi_tile);
    LV_ASSERT(object_wifi_ssid_display);
    lv_obj_set_grid_cell(object_wifi_ssid_display, LV_GRID_ALIGN_STRETCH, 1, 3, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_label_set_text(object_wifi_ssid_display, "NABIN_NTFiberNet");
    lv_label_set_long_mode(object_wifi_ssid_display, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(object_wifi_ssid_display, LV_TEXT_ALIGN_CENTER, 0);

    MALLOC_STYLE(style_wifi_ssid_display);
    LV_ASSERT(style_wifi_ssid_display);
    lv_style_init(style_wifi_ssid_display);
    lv_style_set_border_width(style_wifi_ssid_display, 0);
    lv_style_set_border_color(style_wifi_ssid_display, COLOR_MAKE(0, 0, 0));

    lv_obj_add_style(object_wifi_ssid_display, style_wifi_ssid_display, LV_PART_MAIN);
}

static void create_wifi_open_wifi_config_cell()
{
    LV_ASSERT(object_wifi_tile);

    object_wifi_open_button = lv_button_create(object_wifi_tile);
    LV_ASSERT(object_wifi_open_button);
    lv_obj_set_grid_cell(object_wifi_open_button, LV_GRID_ALIGN_STRETCH, 0, 4, LV_GRID_ALIGN_STRETCH, 2, 2);

    MALLOC_STYLE(style_wifi_open_button);
    LV_ASSERT(style_wifi_open_button);
    lv_style_init(style_wifi_open_button);
    lv_style_set_border_width(style_wifi_open_button, 1);
    lv_style_set_border_color(style_wifi_open_button, COLOR_MAKE(0, 0, 0));
    lv_style_set_shadow_width(style_wifi_open_button, 0);

    lv_obj_add_style(object_wifi_open_button, style_wifi_open_button, LV_PART_MAIN);

    lv_obj_t *button_label = lv_label_create(object_wifi_open_button);
    LV_ASSERT(button_label);

    lv_label_set_text(button_label, "Open Setting");
    lv_obj_center(button_label);

    lv_obj_set_state(object_wifi_open_button, LV_STATE_DISABLED, 1);
}

static void wifi_enable_button_clicked()
{
    if (wifi_enable_button_data.messages.switch_state)
    {
        main_ui_set_message("WiFi turned on");
        lv_image_set_src(object_wifi_icon, &wifi_fill_strength);
        lv_style_set_border_color(style_wifi_icon, COLOR_MAKE(0, 0, 0));
        lv_obj_set_state(object_wifi_open_button, LV_STATE_DISABLED, 0);
    }
    else
    {
        main_ui_set_message("WiFi turned off");
        lv_image_set_src(object_wifi_icon, &wifi_turned_off);
        lv_style_set_border_color(style_wifi_icon, COLOR_MAKE(0xC8, 0xC8, 0xC8));
        lv_obj_set_state(object_wifi_open_button, LV_STATE_DISABLED, 1);
    }
}