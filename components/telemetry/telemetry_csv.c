#include "telemetry_csv.h"

#include "collar_state_machine.h"
#include "connectivity_manager.h"
#include "gps_l89.h"
#include "power_manager.h"
#include "sensor_manager.h"

#include "esp_timer.h"
#include <stdio.h>
#include <time.h>

esp_err_t telemetry_build_csv(char *buf, size_t buf_len, size_t *out_len)
{
    if (!buf || buf_len < TELEMETRY_CSV_MAX_BYTES) {
        return ESP_ERR_INVALID_ARG;
    }

    sensor_snapshot_t snap = { 0 };
    sensor_manager_get_snapshot(&snap);

    gps_fix_t fix = { 0 };
    gps_l89_get_last_fix(&fix);

    power_status_t pwr = { 0 };
    power_manager_get_status(&pwr);

    const time_t now = time(NULL);
    const uint32_t ts = (now > 0) ? (uint32_t)now : (uint32_t)(esp_timer_get_time() / 1000000ULL);
    const int is_emergency = collar_state_machine_is_emergency() ? 1 : 0;
    const uint32_t window = collar_state_machine_get_report_interval_sec();
    const int8_t rssi = connectivity_manager_get_rssi();

    int n = snprintf(
        buf, buf_len,
        "%lu,%d,%lu,"
        "%.1f,%.1f,%.1f,"       /* heart rate avg/min/max */
        ",,,"                   /* spo2 placeholders */
        ",%.1f,%.1f,"           /* ambient / object temp */
        ",%.2f,%.2f,%.2f,%d,"    /* accel + motion */
        "%.0f,%lu,"             /* light, steps */
        "%.2f,%d,"              /* battery */
        "%.6f,%.6f,%lu,"        /* lat, lon, fix time */
        ",,,%d",                /* dht22 placeholders + rssi */
        (unsigned long)ts, is_emergency, (unsigned long)window,
        snap.avg_heart_rate, snap.min_heart_rate, snap.max_heart_rate,
        snap.avg_skin_temp, snap.max_skin_temp,
        snap.accel_x, snap.accel_y, snap.accel_z, snap.motion_detected ? 1 : 0,
        snap.light_lux, (unsigned long)snap.total_steps,
        pwr.voltage_v, pwr.percent,
        fix.valid ? fix.latitude : 0.0,
        fix.valid ? fix.longitude : 0.0,
        (unsigned long)fix.fix_time_sec,
        (int)rssi);

    if (n < 0 || (size_t)n >= buf_len) {
        return ESP_ERR_NO_MEM;
    }
    if (out_len) {
        *out_len = (size_t)n;
    }
    return ESP_OK;
}
