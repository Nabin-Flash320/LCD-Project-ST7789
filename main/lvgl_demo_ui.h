#ifndef __LVGL_EXAMPLE_H__
#define __LVGL_EXAMPLE_H__

#include "core/lv_obj.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_style_gen.h"
#include "display/lv_display.h"
#include "esp_rom_sys.h"
#include "lvgl.h"
#include "misc/lv_area.h"
#include "misc/lv_color.h"
#include "widgets/label/lv_label.h"

static void create_button_xy(int32_t pos_x, int32_t pos_y, int32_t size_x, int32_t size_y, lv_event_cb_t cb);
static void btn_event_cb(lv_event_t* e);

void lv_button_test(void)
{
    const int cols = 4;
    const int rows = 6;
    const int gap_x = 5;
    const int gap_y = 5;

    const int btn_w = (240 - (gap_x * (cols - 1))) / cols; // 56
    const int btn_h = (360 - (gap_y * (rows - 1))) / rows; // 55

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            int x = col * (btn_w + gap_x);
            int y = row * (btn_h + gap_y);
            create_button_xy(x, y, btn_w, btn_h, btn_event_cb);
        }
    }
}

static void create_button_xy(int32_t pos_x, int32_t pos_y, int32_t size_x, int32_t size_y, lv_event_cb_t cb)
{

    lv_obj_t* btn = lv_button_create(lv_screen_active()); /*Add a button the current screen*/
    lv_obj_set_size(btn, size_x, size_y);                 /*Set its size*/
    lv_obj_add_event_cb(btn, cb, LV_EVENT_ALL, NULL);     /*Assign a callback to the button*/
    lv_obj_set_pos(btn, pos_x, pos_y);
    lv_obj_set_style_bg_color(btn, lv_color_make(255, 255, 255), LV_PART_MAIN);

    lv_area_t coord;
    lv_obj_get_coords(btn, &coord);
}


static void btn_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target_obj(e);

    if (code == LV_EVENT_PRESSED)
    {
        // Change to grey when pressed
        lv_obj_set_style_bg_color(btn, lv_color_make(150, 150, 150), LV_PART_MAIN);
    }
    else if (code == LV_EVENT_RELEASED || code == LV_EVENT_CLICKED)
    {
        // Revert when released
        lv_obj_set_style_bg_color(btn, lv_color_make(255, 255, 255), LV_PART_MAIN);
        lv_obj_t* btn_text = lv_label_create(btn);
        lv_obj_set_style_text_color(btn_text, (lv_color_t) {0, 0, 0}, LV_STATE_DEFAULT);
    }
}
#endif
