#include "sensor_manager.h"
#include "collar_events.h"
#include "collar_state_machine.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "sensors";
static sensor_snapshot_t s_snap;

static void sensor_task(void *arg)
{
    (void)arg;
    for (;;) {
        const uint32_t interval_ms = collar_state_machine_get_report_interval_sec() * 1000U;

        /*
         * TODO: MAX30102 (PPG), LSM6DSOX (IMU), OPT3001 (ALS), thermistor ADC.
         * Adaptive sampling per activity state (SRS FR-5).
         */
        s_snap.avg_heart_rate = 80.0f;
        s_snap.min_heart_rate = 72.0f;
        s_snap.max_heart_rate = 88.0f;
        s_snap.avg_skin_temp = 38.2f;
        s_snap.max_skin_temp = 38.5f;
        s_snap.accel_x = 0.01f;
        s_snap.accel_y = 0.02f;
        s_snap.accel_z = 1.0f;
        s_snap.motion_detected = false;
        s_snap.light_lux = 120.0f;
        s_snap.total_steps = 0;

        collar_event_t evt = { .id = COLLAR_EVT_SENSOR_BATCH_READY };
        collar_state_machine_post_event(&evt, 0);

        vTaskDelay(pdMS_TO_TICKS(interval_ms));
    }
}

esp_err_t sensor_manager_init(void)
{
    return ESP_OK;
}

esp_err_t sensor_manager_start_task(void)
{
    const BaseType_t ok = xTaskCreatePinnedToCore(sensor_task, "sensors", 6144, NULL, 4, NULL, 0);
    return ok == pdPASS ? ESP_OK : ESP_FAIL;
}

esp_err_t sensor_manager_get_snapshot(sensor_snapshot_t *out)
{
    if (!out) {
        return ESP_ERR_INVALID_ARG;
    }
    *out = s_snap;
    return ESP_OK;
}
