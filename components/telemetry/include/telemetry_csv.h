#pragma once

/** Max CSV frame size per integration spec §4.4 */
#define TELEMETRY_CSV_MAX_BYTES 512

/**
 * Positional CSV index order (integration spec §4.4):
 * timestamp, is_emergency, window_seconds, avg_hr, min_hr, max_hr,
 * avg_spo2, min_spo2, max_spo2, avg_ambient_temp, avg_object_temp, max_object_temp,
 * accel_x, accel_y, accel_z, motion_detected, light_level, total_steps,
 * battery_voltage, battery_percentage, latitude, longitude, gps_fix_time_sec,
 * temp_dht22, humidity, heat_index, rssi
 */
#include "esp_err.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t telemetry_build_csv(char *buf, size_t buf_len, size_t *out_len);

#ifdef __cplusplus
}
#endif
