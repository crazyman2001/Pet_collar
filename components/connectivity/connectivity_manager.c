#include "connectivity_manager.h"
#include "wifi_manager.h"

#include "board_pins.h"
#include "collar_events.h"
#include "collar_state_machine.h"
#include "modem_manager.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "conn";
static bool s_has_ip;
static int64_t s_wifi_down_since_us;

static void notify_wifi_up(void)
{
    s_has_ip = true;
    s_wifi_down_since_us = 0;
    collar_event_t evt = { .id = COLLAR_EVT_WIFI_CONNECTED };
    collar_state_machine_post_event(&evt, 0);
    modem_manager_power_off();
}

static void notify_wifi_down(void)
{
    s_has_ip = false;
    if (s_wifi_down_since_us == 0) {
        s_wifi_down_since_us = esp_timer_get_time();
    }
    collar_event_t evt = { .id = COLLAR_EVT_WIFI_DISCONNECTED };
    collar_state_machine_post_event(&evt, 0);
}

static void connectivity_task(void *arg)
{
    (void)arg;
    ESP_ERROR_CHECK(wifi_manager_init());
    ESP_ERROR_CHECK(wifi_manager_start());

    for (;;) {
        if (wifi_manager_is_connected()) {
            if (!s_has_ip) {
                notify_wifi_up();
            }
        } else {
            if (s_has_ip) {
                notify_wifi_down();
            }

            if (s_wifi_down_since_us > 0) {
                int64_t down_ms = (esp_timer_get_time() - s_wifi_down_since_us) / 1000;
                if (down_ms >= BOARD_WIFI_FAIL_THRESHOLD_MS &&
                    collar_state_machine_modem_allowed()) {
                    ESP_LOGW(TAG, "Wi-Fi down %lld ms — starting LTE PPP", (long long)down_ms);
                    collar_event_t thr = { .id = COLLAR_EVT_WIFI_FAIL_THRESHOLD };
                    collar_state_machine_post_event(&thr, 0);
                    if (modem_manager_start_ppp() == ESP_OK) {
                        s_has_ip = true;
                        collar_event_t ppp = { .id = COLLAR_EVT_PPP_CONNECTED };
                        collar_state_machine_post_event(&ppp, 0);
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

esp_err_t connectivity_manager_start(void)
{
    const BaseType_t ok = xTaskCreatePinnedToCore(
        connectivity_task, "connectivity", 6144, NULL, 6, NULL, 1);
    return ok == pdPASS ? ESP_OK : ESP_FAIL;
}

bool connectivity_manager_has_ip(void)
{
    return s_has_ip;
}

int8_t connectivity_manager_get_rssi(void)
{
    return wifi_manager_get_rssi();
}
