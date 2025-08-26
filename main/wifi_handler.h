

#ifndef __WIFI_HANDLER_H__
#define __WIFI_HANDLER_H__

#include "esp_err.h"

esp_err_t wifi_sta_connect();
void wifi_sta_scan_aps(void *params);

#endif // __WIFI_HANDLER_H__

