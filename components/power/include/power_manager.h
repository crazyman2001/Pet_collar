#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BATTERY_STATE_FULL = 0,
    BATTERY_STATE_NORMAL,
    BATTERY_STATE_LOW,
    BATTERY_STATE_CRITICAL,
} battery_state_t;

typedef struct {
    float voltage_v;
    int percent;
    battery_state_t state;
    bool charging;
} power_status_t;

esp_err_t power_manager_init(void);
esp_err_t power_manager_start_task(void);
esp_err_t power_manager_get_status(power_status_t *out);

#ifdef __cplusplus
}
#endif
