#include "collar_tasks.h"
#include "collar_state_machine.h"
#include "gps_l89.h"
#include "mqtt_client.h"
#include "storage_buffer.h"
#include "telemetry_csv.h"

#include "connectivity_manager.h"
#include "modem_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "telemetry";
static collar_state_t s_last_state = COLLAR_STATE_OFF;

static void on_state_change(collar_state_t state)
{
    if (state == COLLAR_STATE_GNSS_ACQUISITION && s_last_state != COLLAR_STATE_GNSS_ACQUISITION) {
        gps_l89_start_task();
    }
    if (state == COLLAR_STATE_WIFI_CONNECTED && s_last_state != COLLAR_STATE_WIFI_CONNECTED) {
        modem_manager_power_off();
    }
    s_last_state = state;
}

static void telemetry_task(void *arg)
{
    (void)arg;
    char csv[TELEMETRY_CSV_MAX_BYTES];

    for (;;) {
        const collar_state_t state = collar_state_machine_get_state();
        on_state_change(state);

        size_t len = 0;
        if (telemetry_build_csv(csv, sizeof(csv), &len) != ESP_OK) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (connectivity_manager_has_ip() && collar_mqtt_is_connected()) {
            while (storage_buffer_pending_count() > 0) {
                size_t hist_len = 0;
                if (storage_buffer_flush_next(csv, sizeof(csv), &hist_len) == ESP_OK) {
                    collar_mqtt_publish_telemetry(csv, hist_len);
                    vTaskDelay(pdMS_TO_TICKS(60000 / STORAGE_FLUSH_RATE_PER_MIN));
                } else {
                    break;
                }
            }
            collar_mqtt_publish_telemetry(csv, len);
        } else {
            storage_buffer_append(csv, len);
        }

        const uint32_t sleep_sec = collar_state_machine_get_report_interval_sec();
        vTaskDelay(pdMS_TO_TICKS(sleep_sec * 1000U));
    }
}

void app_telemetry_start(void)
{
    xTaskCreatePinnedToCore(telemetry_task, "telemetry", 6144, NULL, 4, NULL, 1);
}
