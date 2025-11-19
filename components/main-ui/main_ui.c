
#include "lvgl.h"

#include "main_ui.h"
#include "driver_init.h"

DEFINE_OBJECT(parent);
DEFINE_OBJECT(utility_bar);
DEFINE_OBJECT(wifi_setting);
DEFINE_OBJECT(wifi_setting_label);
DEFINE_OBJECT(bluetooth_setting);
DEFINE_OBJECT(bluetooth_setting_label);
DEFINE_OBJECT(scrolling_label);
DEFINE_OBJECT(main_display);

DEFINE_STYLE(parent);
DEFINE_STYLE(utility_bar);
DEFINE_STYLE(wifi_setting);
DEFINE_STYLE(wifi_setting_label);
DEFINE_STYLE(bluetooth_setting);
DEFINE_STYLE(bluetooth_setting_label);
DEFINE_STYLE(scrolling_label);
DEFINE_STYLE(main_display);

static int main_ui_initialize_utility_bar(lv_obj_t *parent);
static int main_ui_initialize_main_display(lv_obj_t *parent);
static int main_ui_initialize_wifi_setting(lv_obj_t *parent);
static int main_ui_initialize_bluetooth_setting(lv_obj_t *parent);
static int main_ui_scrolling_label(lv_obj_t *parent);

void main_ui_initialize()
{
    object_parent = lv_scr_act();
    LV_ASSERT(object_parent);
    lv_obj_set_layout(object_parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(object_parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(object_parent, 5, LV_PART_MAIN);

    main_ui_initialize_utility_bar(object_parent);
    main_ui_initialize_wifi_scan_result_widget(object_parent);

    MALLOC_STYLE(style_parent);
    LV_ASSERT(style_parent);
    lv_style_init(style_parent);
    lv_style_set_bg_color(style_parent, COLOR_LIGHT_GRAY());
    lv_obj_add_style(object_parent, style_parent, LV_PART_MAIN);
}

void main_ui_set_message(const char *message)
{
    lv_label_set_text(object_scrolling_label, message);
}

void main_ui_set_wifi_status(bool connected)
{
    lv_style_set_opa(style_wifi_setting, connected ? 255 : 100);
    if (connected)
    {
        lv_style_set_bg_color(style_wifi_setting, lv_color_make(0, 255, 0));
    }

    lv_obj_refresh_style(object_wifi_setting, LV_PART_MAIN, LV_STYLE_PROP_ANY);
}

static int main_ui_initialize_utility_bar(lv_obj_t *parent)
{
    if (NULL == parent)
    {
        return -1;
    }

    object_utility_bar = lv_obj_create(parent);
    LV_ASSERT(object_utility_bar);
    lv_obj_set_layout(object_utility_bar, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(object_utility_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(object_utility_bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_size(object_utility_bar, LV_HOR_RES, 34);

    MALLOC_STYLE(style_utility_bar);
    LV_ASSERT(style_utility_bar);
    lv_style_init(style_utility_bar);
    lv_style_set_border_width(style_utility_bar, 2);
    lv_style_set_pad_all(style_utility_bar, 0);
    lv_style_set_pad_left(style_utility_bar, 2);
    lv_style_set_pad_right(style_utility_bar, 2);
    lv_style_set_border_color(style_utility_bar, COLOR_SLATE_GRAY());

    main_ui_initialize_wifi_setting(object_utility_bar);
    main_ui_initialize_bluetooth_setting(object_utility_bar);
    main_ui_scrolling_label(object_utility_bar);
    lv_obj_add_style(object_utility_bar, style_utility_bar, LV_PART_MAIN);
    return 0;
}

static int main_ui_initialize_main_display(lv_obj_t *parent)
{
    if (NULL == parent)
    {
        return -1;
    }

    object_main_display = lv_obj_create(parent);
    LV_ASSERT(object_main_display);
    lv_obj_set_size(object_main_display, LCD_H_RES, LCD_V_RES);

    MALLOC_STYLE(style_main_display);
    LV_ASSERT(style_main_display);
    lv_style_init(style_main_display);
    lv_style_set_border_width(style_main_display, 2);
    lv_style_set_pad_all(style_main_display, 0);
    lv_style_set_pad_left(style_main_display, 2);
    lv_style_set_pad_right(style_main_display, 2);
    lv_style_set_border_color(style_main_display, COLOR_SLATE_GRAY());

    lv_obj_add_style(object_main_display, style_main_display, LV_PART_MAIN);
    return 0;
}

static int main_ui_initialize_wifi_setting(lv_obj_t *parent)
{
    if (NULL == parent)
    {
        return -1;
    }

    object_wifi_setting = lv_button_create(parent);
    LV_ASSERT(object_wifi_setting);
    object_wifi_setting_label = lv_label_create(object_wifi_setting);
    LV_ASSERT(object_wifi_setting_label);
    lv_label_set_text(object_wifi_setting_label, LV_SYMBOL_WIFI);

    MALLOC_STYLE(style_wifi_setting_label);
    LV_ASSERT(style_wifi_setting_label);
    lv_style_init(style_wifi_setting_label);
    lv_style_set_size(style_wifi_setting_label, 15, 15);
    lv_style_set_text_color(style_wifi_setting_label, lv_color_black());
    lv_obj_add_style(object_wifi_setting_label, style_wifi_setting_label, LV_PART_MAIN);

    MALLOC_STYLE(style_wifi_setting);
    LV_ASSERT(style_wifi_setting);
    lv_style_init(style_wifi_setting);
    lv_style_set_width(style_wifi_setting, 24);
    lv_style_set_height(style_wifi_setting, 24);
    lv_style_set_pad_all(style_wifi_setting, 0);
    lv_style_set_pad_left(style_wifi_setting, 3);
    lv_style_set_pad_top(style_wifi_setting, 3);
    lv_style_set_pad_right(style_wifi_setting, 2);
    lv_style_set_text_color(style_wifi_setting, lv_color_white());
    lv_style_set_text_align(style_wifi_setting, LV_TEXT_ALIGN_CENTER);
    lv_style_set_shadow_width(style_wifi_setting, 0);
    lv_style_set_bg_color(style_wifi_setting, lv_color_white());
    lv_style_set_border_color(style_wifi_setting, lv_color_black());
    lv_style_set_border_width(style_wifi_setting, 1);
    lv_style_set_opa(style_wifi_setting, 100);
    lv_obj_add_style(object_wifi_setting, style_wifi_setting, LV_PART_MAIN);
    return 0;
}

static int main_ui_initialize_bluetooth_setting(lv_obj_t *parent)
{
    if (NULL == parent)
    {
        return -1;
    }

    object_bluetooth_setting = lv_button_create(parent);
    LV_ASSERT(object_bluetooth_setting);
    object_bluetooth_setting_label = lv_label_create(object_bluetooth_setting);
    LV_ASSERT(object_bluetooth_setting_label);
    lv_label_set_text(object_bluetooth_setting_label, LV_SYMBOL_BLUETOOTH);

    MALLOC_STYLE(style_bluetooth_setting_label);
    LV_ASSERT(style_bluetooth_setting_label);
    lv_style_init(style_bluetooth_setting_label);
    lv_style_set_size(style_bluetooth_setting_label, 16, 16);
    lv_style_set_text_color(style_bluetooth_setting_label, lv_color_black());
    lv_obj_add_style(object_bluetooth_setting_label, style_bluetooth_setting_label, LV_PART_MAIN);

    MALLOC_STYLE(style_bluetooth_setting);
    LV_ASSERT(style_bluetooth_setting);
    lv_style_init(style_bluetooth_setting);
    lv_style_set_width(style_bluetooth_setting, 24);
    lv_style_set_height(style_bluetooth_setting, 24);
    lv_style_set_pad_all(style_bluetooth_setting, 0);
    lv_style_set_pad_left(style_bluetooth_setting, 3);
    lv_style_set_pad_top(style_bluetooth_setting, 3);
    lv_style_set_pad_right(style_bluetooth_setting, 2);
    lv_style_set_text_color(style_bluetooth_setting, lv_color_white());
    lv_style_set_text_align(style_bluetooth_setting, LV_TEXT_ALIGN_CENTER);
    lv_style_set_shadow_width(style_bluetooth_setting, 0);
    lv_style_set_bg_color(style_bluetooth_setting, lv_color_white());
    lv_style_set_border_color(style_bluetooth_setting, lv_color_black());
    lv_style_set_border_width(style_bluetooth_setting, 1);
    lv_style_set_opa(style_bluetooth_setting, 100);
    lv_obj_add_style(object_bluetooth_setting, style_bluetooth_setting, LV_PART_MAIN);
    return 0;
}

static int main_ui_scrolling_label(lv_obj_t *parent)
{
    if (!parent)
    {
        return -1;
    }

    object_scrolling_label = lv_label_create(parent);
    LV_ASSERT(object_scrolling_label);
    lv_label_set_long_mode(object_scrolling_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(object_scrolling_label, 165);
    lv_obj_set_height(object_scrolling_label, 24);
    lv_obj_align(object_scrolling_label, LV_ALIGN_CENTER, 0, 40);
    lv_obj_set_style_radius(object_scrolling_label, 5, 0);
    lv_obj_set_style_text_align(object_scrolling_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(object_scrolling_label, " ");

    MALLOC_STYLE(style_scrolling_label);
    LV_ASSERT(style_scrolling_label);
    lv_style_init(style_scrolling_label);
    lv_style_set_border_width(style_scrolling_label, 1);
    lv_style_set_text_color(style_scrolling_label, lv_color_black());
    lv_style_set_pad_top(style_scrolling_label, 2);
    lv_obj_add_style(object_scrolling_label, style_scrolling_label, LV_PART_MAIN);
    return 0;
}
