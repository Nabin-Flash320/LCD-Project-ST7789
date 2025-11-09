
#include "esp_event.h"

#include "device_event_handler.h"

static esp_event_loop_handle_t loop_handle = NULL;

ESP_EVENT_DEFINE_BASE(DEVICE_EVENT);

int device_event_initialize(esp_event_handler_t handler)
{
    if (!handler)
    {
        return -1;
    }

    esp_event_loop_args_t loop_args = {
        .queue_size = 25,
        .task_name = "div-loop",
        .task_priority = 2,
        .task_stack_size = 4096,
        .task_core_id = 1,
    };

    ESP_ERROR_CHECK(esp_event_loop_create(&loop_args, &loop_handle));
    ESP_ERROR_CHECK(esp_event_handler_register_with(loop_handle, DEVICE_EVENT, ESP_EVENT_ANY_ID, handler, NULL));
    return 0;
}

int device_event_post(int32_t event_id, void* args, size_t data_size)
{
    if (!loop_handle)
    {
        return -1;
    }

    struct s_event_data data = {
        .data = args,
        .data_len = data_size,
    };

    ESP_ERROR_CHECK(
        esp_event_post_to(loop_handle, DEVICE_EVENT, event_id, &data, data_size, 2000 / portTICK_PERIOD_MS));
    return 0;
}
