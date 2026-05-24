#include "mqtt_client.h"
#include "mqtt_topics.h"

#include "collar_events.h"
#include "collar_state_machine.h"
#include "connectivity_manager.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "mqtt";
static bool s_connected;

static void mqtt_task(void *arg)
{
    (void)arg;
    for (;;) {
        if (connectivity_manager_has_ip()) {
            /*
             * TODO: esp_mqtt_client with mTLS client cert from `certs` partition,
             * broker URI from NVS, exponential backoff + jitter (2s .. 15min cap).
             */
            if (!s_connected) {
                s_connected = true;
                collar_event_t evt = { .id = COLLAR_EVT_MQTT_CONNECTED };
                collar_state_machine_post_event(&evt, 0);
                ESP_LOGI(TAG, "MQTT session stub (port %d, keepalive %ds)",
                         MQTT_BROKER_PORT, MQTT_KEEPALIVE_SEC);
            }
        } else if (s_connected) {
            s_connected = false;
            collar_event_t evt = { .id = COLLAR_EVT_MQTT_DISCONNECTED };
            collar_state_machine_post_event(&evt, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

esp_err_t collar_mqtt_init(void)
{
    return ESP_OK;
}

esp_err_t collar_mqtt_start_task(void)
{
    const BaseType_t ok = xTaskCreatePinnedToCore(mqtt_task, "mqtt", 8192, NULL, 5, NULL, 1);
    return ok == pdPASS ? ESP_OK : ESP_FAIL;
}

esp_err_t collar_mqtt_publish_telemetry(const char *csv, size_t len)
{
    if (!s_connected || !csv) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGD(TAG, "telemetry %u bytes", (unsigned)len);
    (void)csv;
    return ESP_OK;
}

esp_err_t collar_mqtt_publish_alert(const char *json, size_t len)
{
    if (!s_connected || !json) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGI(TAG, "alert %u bytes", (unsigned)len);
    (void)json;
    return ESP_OK;
}

bool collar_mqtt_is_connected(void)
{
    return s_connected;
}
