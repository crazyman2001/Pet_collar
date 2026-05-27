#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t wifi_manager_init(void);

/**
 * Boot-time Wi-Fi bring-up:
 * - Load SSID/password from NVS; connect in STA mode if present.
 * - If missing or connection fails → soft-AP + HTTP provisioning server.
 */
esp_err_t wifi_manager_start(void);

/** True when STA has obtained an IP (not provisioning AP). */
bool wifi_manager_is_sta_connected(void);

/** True when soft-AP + HTTP config server is active. */
bool wifi_manager_is_provisioning(void);

/** Alias for STA connected (used by connectivity layer). */
bool wifi_manager_is_connected(void);

int8_t wifi_manager_get_rssi(void);

#ifdef __cplusplus
}
#endif
