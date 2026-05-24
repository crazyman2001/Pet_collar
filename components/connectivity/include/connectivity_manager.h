#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Wi-Fi-first link manager; starts connectivity FreeRTOS task. */
esp_err_t connectivity_manager_start(void);

/** True when TCP/IP is available (Wi-Fi STA or PPP). */
bool connectivity_manager_has_ip(void);

/** Current RSSI (Wi-Fi) or 0 if on PPP. */
int8_t connectivity_manager_get_rssi(void);

#ifdef __cplusplus
}
#endif
