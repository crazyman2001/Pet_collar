#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t wifi_manager_init(void);
esp_err_t wifi_manager_start(void);
bool wifi_manager_is_connected(void);
int8_t wifi_manager_get_rssi(void);

#ifdef __cplusplus
}
#endif
