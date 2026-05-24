#pragma once

#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t collar_mqtt_init(void);
esp_err_t collar_mqtt_start_task(void);
esp_err_t collar_mqtt_publish_telemetry(const char *csv, size_t len);
esp_err_t collar_mqtt_publish_alert(const char *json, size_t len);
bool collar_mqtt_is_connected(void);

#ifdef __cplusplus
}
#endif
