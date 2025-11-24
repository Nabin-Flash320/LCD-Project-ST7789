
#include "lvgl.h"
#include "lv_types.h"
#include "lv_obj_class_private.h"

#include "w_tiles.h"

typedef struct
{
    lv_obj_t *container; // Main container that is the tile itself
    lv_obj_t *icon_container; // Container for ICON
    lv_obj_t *icon; // Object for icon itself
    lv_obj_t *title; // Title of the tile WiFi, Bluetooth, BLE, etc.
    lv_obj_t *button; // Toogle button for turning on or off
    lv_obj_t *message; // Message to show on the device
    lv_obj_t *list; // Open actual UI
} widget_tiles_t;

const lv_obj_class_t widget_tile_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_DPI_DEF * 2,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(widget_tiles_t),
    .name = "tile_widgets",
};

lv_obj_t *widget_tile_create(lv_obj_t *parent)
{
    LV_LOG_INFO("Begin");

    if(NULL == parent)
    {
        return NULL;
    }

    lv_obj_t *object = lv_obj_class_create_obj(&widget_tile_class, parent);
    LV_ASSERT_MALLOC(object);
    lv_obj_class_init_obj(object);

    widget_tiles_t* tile = (widget_tiles_t*)object;
    return object;

}

