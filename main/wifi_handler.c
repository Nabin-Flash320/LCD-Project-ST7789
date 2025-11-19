
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "wifi_handler.h"
#include "device_event_handler.h"

#define EXAMPLE_ESP_MAXIMUM_RETRY 5
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;
static esp_netif_t *sta_netif = NULL;

static char *wifi_sta_ssid = "NABIN_DHFiberNet";
static char *wifi_sta_pass = "Sainikian@112233";
static int s_retry_num = 0;
static bool wifi_connected = false;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGE(__FILE__, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGE(__FILE__, "connect to the AP fail");
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE)
    {
        ESP_LOGE(__FILE__, "WIFI_EVENT_SCAN_DONE");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGE(__FILE__, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

esp_err_t wifi_sta_connect()
{
    esp_err_t ret = ESP_OK;
    s_wifi_event_group = xEventGroupCreate();

    sta_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    strncpy((char *)wifi_config.sta.ssid, wifi_sta_ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, wifi_sta_pass, sizeof(wifi_config.sta.password));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGE(__FILE__, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    char connected_msg[128];
    memset(connected_msg, 0, 64);
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGE(__FILE__, "connected to ap SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
        snprintf(connected_msg, 128, "WiFi connected to: %s", wifi_sta_ssid);
        device_event_post(DEVICE_EVENT_SET_MESSAGE_EVENT, connected_msg, strlen(connected_msg));
        wifi_connected = true;
        ret = ESP_OK;
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGE(__FILE__, "Failed to connect to SSID:%s, password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
        // ESP_ERROR_CHECK(esp_wifi_disconnect());
        // esp_netif_destroy_default_wifi(sta_netif);
        // ESP_ERROR_CHECK(esp_wifi_stop());
        // ESP_ERROR_CHECK(esp_wifi_deinit());
        snprintf(connected_msg, 128, "Unable to connect to: %s", wifi_sta_ssid);
        device_event_post(DEVICE_EVENT_SET_MESSAGE_EVENT, connected_msg, strlen(connected_msg));
        wifi_connected = false;
        ret = ESP_FAIL;
    }
    else
    {
        ESP_LOGE(__FILE__, "UNEXPECTED EVENT");
        ret = ESP_FAIL;
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);

    device_event_post(DEVICE_EVENT_WIFI_CONNECTED_EVENT, &ret, sizeof(int));
    return ret;
}

void wifi_sta_scan_aps(void *params)
{
    wifi_scan_config_t scan_config = {
        .ssid = NULL, // Scan all SSIDs
        .bssid = NULL,
        .channel = 0,        // Scan all channels
        .show_hidden = true, // Show hidden networks
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active.min = 100,
        .scan_time.active.max = 300,
    };

    int count = 5;
    while (count-- != 0)
    {
        ESP_LOGI(__FILE__, "Starting WiFi scan...(count: %d)", count);
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true)); // Block until scan completes

        uint16_t number = 20;
        wifi_ap_record_t ap_info[20];
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
        ESP_LOGI(__FILE__, "Found %d networks", number);

        char *actual_record = (char *)malloc(32 * number);
        if (!actual_record)
        {
            continue;
        }

        for (int i = 0; i < number; i++)
        {
            printf("[%d] SSID: %s(len: %d) | RSSI: %d | Auth: %d\n",
                   i + 1, ap_info[i].ssid, strlen((const char *)ap_info[i].ssid), ap_info[i].rssi, ap_info[i].authmode);
        }

        device_event_post(DEVICE_EVENT_WIFI_AP_DETAILS_EVENT, ap_info, number);
        ESP_LOGI(__FILE__, "Scan complete, sleeping for 5 seconds...");
        vTaskDelay(pdMS_TO_TICKS(3000)); // Delay for 5 seconds before next scan
    }

    vTaskDelete(NULL);
}

bool is_wifi_connected()
{
    return wifi_connected;
}
