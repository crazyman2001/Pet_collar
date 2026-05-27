#pragma once

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Start captive-style HTTP server on the AP interface (192.168.4.1). */
esp_err_t wifi_prov_http_start(void);

/** Stop HTTP server (when leaving provisioning mode). */
void wifi_prov_http_stop(void);

bool wifi_prov_http_is_running(void);

#ifdef __cplusplus
}
#endif
