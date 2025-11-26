
#ifndef __MAIN_UI_H__
#define __MAIN_UI_H__

#include "lvgl.h"

#define DEFINE_OBJECT(postfix) static lv_obj_t *object_##postfix = NULL
#define DEFINE_STYLE(postfix) static lv_style_t *style_##postfix = NULL

#define MALLOC_STYLE(var) var = lv_malloc(sizeof(lv_style_t))
#define COLOR_MAKE(r, g, b) lv_color_make(r, g, b)

#define COLOR_SLATE_GRAY() COLOR_MAKE(112, 128, 144)
#define COLOR_LIGHT_GRAY() COLOR_MAKE(211, 211, 211)

typedef void (*func_common_ui_callback_t)();

typedef struct
{

    func_common_ui_callback_t callback;
    union
    {
        char list_button_selected[64];
        char keyboard_message[64];
    } messages;

} s_common_ui_callback_user_data_t;

void main_ui_initialize();
void main_ui_set_message(const char *message);
void main_ui_set_wifi_status(bool connected);

lv_obj_t *main_ui_get_main_menu_object();

// Functions for event handlers
void event_handler_wifi_setting(lv_event_t *event);
void event_handler_wifi_list_button_click(lv_event_t *event);
void event_handler_keyboard_events(lv_event_t *event);
void event_handler_bluetooth_setting(lv_event_t *event);

// Functions for WiFi UI
void wifi_ui_start();
void wifi_ui_connect_wifi(const char *SSID);

// Functions for common UIs like keyboards, message boxes, etc.
void common_ui_alphanumeric_keyboard_start();
void common_ui_numeric_keyboard_start();

// Tile uis
void tile_ui_create_wifi_tile();
void tile_ui_create_bluetooth_tile();
void tile_ui_create_other_tile();

#endif // __MAIN_UI_H__
