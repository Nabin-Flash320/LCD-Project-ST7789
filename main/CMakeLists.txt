idf_component_register(
    SRCS "wifi_handler.c" "hello_world_main.c"
    INCLUDE_DIRS ""
    REQUIRES lvgl spi_flash esp_lcd esp_wifi esp_netif esp_event nvs_flash touch_xpt2046 Main-UI device-event-handler
)
