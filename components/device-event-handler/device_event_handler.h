
#ifndef __DEVICE_EVENT_HANDLER_H__
#define __DEVICE_EVENT_HANDLER_H__

#include "esp_event.h"

enum
{
    DEVICE_EVENT_WIFI_CONNECTED_EVENT,
    DEVICE_EVENT_SET_MESSAGE_EVENT,
    DEVICE_EVENT_WIFI_AP_DETAILS_EVENT,
    DEVICE_EVENT_MAX,
};

struct s_event_data
{
    void* data;
    size_t data_len;
};

ESP_EVENT_DECLARE_BASE(DEVICE_EVENT);

int device_event_initialize(esp_event_handler_t handler);
int device_event_post(int32_t event_id, void* args, size_t data_size);

#endif // __DEVICE_EVENT_HANDLER_H__
