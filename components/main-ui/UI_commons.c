
#include "lvgl.h"

#include "main_ui.h"

DEFINE_OBJECT(primary);
DEFINE_OBJECT(keyboard);
DEFINE_OBJECT(text_area);

static void common_ui_start_keyboard(lv_keyboard_mode_t mode);

void common_ui_alphanumeric_keyboard_start()
{
    common_ui_start_keyboard(LV_KEYBOARD_MODE_TEXT_LOWER);
}

void common_ui_numeric_keyboard_start()
{
    common_ui_start_keyboard(LV_KEYBOARD_MODE_NUMBER);
}

static void common_ui_start_keyboard(lv_keyboard_mode_t mode)
{
    object_primary = main_ui_get_main_menu_object();
    LV_ASSERT(object_primary);

    object_keyboard = lv_keyboard_create(object_primary);
    LV_ASSERT(object_keyboard);

    lv_keyboard_set_mode(object_keyboard, mode);

    /*Create a text area. The keyboard will write here*/
    object_text_area = lv_textarea_create(object_primary);
    lv_obj_align(object_text_area, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_size(object_text_area, lv_pct(90), 80);
    lv_obj_add_state(object_text_area, LV_STATE_FOCUSED);

    lv_keyboard_set_textarea(object_keyboard, object_text_area);
    lv_obj_add_event_cb(object_keyboard, event_handler_keyboard_events, LV_EVENT_CANCEL, NULL);
}
