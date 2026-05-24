#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float avg_heart_rate;
    float min_heart_rate;
    float max_heart_rate;
    float avg_skin_temp;
    float max_skin_temp;
    float accel_x;
    float accel_y;
    float accel_z;
    bool motion_detected;
    float light_lux;
    uint32_t total_steps;
} sensor_snapshot_t;

esp_err_t sensor_manager_init(void);
esp_err_t sensor_manager_start_task(void);
esp_err_t sensor_manager_get_snapshot(sensor_snapshot_t *out);

#ifdef __cplusplus
}
#endif
