#include "gps_l89.h"
#include "board_pins.h"
#include "collar_events.h"
#include "collar_state_machine.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "gps_l89";
static gps_fix_t s_fix;
static bool s_running;

static esp_err_t gps_uart_init(void)
{
    const uart_config_t uart_cfg = {
        .baud_rate = BOARD_GPS_UART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(BOARD_UART_GPS, 1024, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(BOARD_UART_GPS, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(
        BOARD_UART_GPS, BOARD_GPS_TX_GPIO, BOARD_GPS_RX_GPIO,
        UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    return ESP_OK;
}

static bool parse_gga(const char *line, gps_fix_t *fix)
{
    /* Minimal $GNGGA / $GPGGA parser stub */
    if (strncmp(line, "$GNGGA", 6) != 0 && strncmp(line, "$GPGGA", 6) != 0) {
        return false;
    }
    int fix_quality = 0;
    double lat = 0, lon = 0;
    if (sscanf(line, "%*[^,],%*[^,],%*[^,],%*[^,],%d,%lf,%*[^,],%lf", &fix_quality, &lat, &lon) >= 3 &&
        fix_quality > 0) {
        fix->valid = true;
        fix->latitude = lat;
        fix->longitude = lon;
        return true;
    }
    return false;
}

static void gps_task(void *arg)
{
    (void)arg;
    uint8_t buf[256];
    char line[128];
    size_t line_len = 0;
    const TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(BOARD_GPS_FIX_TIMEOUT_MS);

    gpio_set_level(BOARD_GPS_RST_GPIO, 1);

    while (s_running && xTaskGetTickCount() < deadline) {
        if (!collar_state_machine_gps_allowed()) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        int n = uart_read_bytes(BOARD_UART_GPS, buf, sizeof(buf) - 1, pdMS_TO_TICKS(500));
        if (n <= 0) {
            continue;
        }
        buf[n] = '\0';
        for (int i = 0; i < n; ++i) {
            if (buf[i] == '\n' || buf[i] == '\r') {
                if (line_len > 0) {
                    line[line_len] = '\0';
                    if (parse_gga(line, &s_fix)) {
                        s_fix.fix_time_sec = (uint32_t)(xTaskGetTickCount() / configTICK_RATE_HZ);
                        collar_event_t evt = { .id = COLLAR_EVT_GPS_FIX };
                        collar_state_machine_post_event(&evt, 0);
                        ESP_LOGI(TAG, "Fix lat=%.5f lon=%.5f", s_fix.latitude, s_fix.longitude);
                        vTaskDelete(NULL);
                        return;
                    }
                    line_len = 0;
                }
            } else if (line_len < sizeof(line) - 1) {
                line[line_len++] = (char)buf[i];
            }
        }
    }

    collar_event_t timeout = { .id = COLLAR_EVT_GPS_TIMEOUT };
    collar_state_machine_post_event(&timeout, 0);
    ESP_LOGW(TAG, "GNSS fix timeout");
    vTaskDelete(NULL);
}

esp_err_t gps_l89_init(void)
{
    memset(&s_fix, 0, sizeof(s_fix));
    return gps_uart_init();
}

esp_err_t gps_l89_start_task(void)
{
    if (!collar_state_machine_gps_allowed()) {
        return ESP_ERR_INVALID_STATE;
    }
    s_running = true;
    const BaseType_t ok = xTaskCreatePinnedToCore(gps_task, "gps_l89", 4096, NULL, 4, NULL, 1);
    return ok == pdPASS ? ESP_OK : ESP_FAIL;
}

esp_err_t gps_l89_get_last_fix(gps_fix_t *out)
{
    if (!out) {
        return ESP_ERR_INVALID_ARG;
    }
    *out = s_fix;
    return s_fix.valid ? ESP_OK : ESP_ERR_NOT_FOUND;
}
