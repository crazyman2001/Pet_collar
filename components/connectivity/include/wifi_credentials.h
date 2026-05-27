#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** NVS namespace for persisted STA credentials. */
#define WIFI_CRED_NVS_NAMESPACE   "wifi_cfg"
#define WIFI_CRED_MAX_SSID_LEN    32
#define WIFI_CRED_MAX_PASS_LEN    64

/** True if non-empty SSID is stored in NVS. */
bool wifi_credentials_is_stored(void);

/** Load SSID/password from flash. Returns ESP_ERR_NOT_FOUND if missing/empty. */
esp_err_t wifi_credentials_load(char *ssid, size_t ssid_len, char *pass, size_t pass_len);

/** Save credentials and commit to NVS. */
esp_err_t wifi_credentials_save(const char *ssid, const char *pass);

/** Erase stored credentials. */
esp_err_t wifi_credentials_clear(void);

#ifdef __cplusplus
}
#endif
