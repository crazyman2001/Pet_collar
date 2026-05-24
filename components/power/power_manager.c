#include "power_manager.h"
#include "board_pins.h"

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "power";
static adc_oneshot_unit_handle_t s_adc;
static power_status_t s_status = { .state = BATTERY_STATE_NORMAL, .percent = 80 };

static float read_battery_voltage(void)
{
    gpio_set_level(BOARD_VBAT_ADC_EN_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
    int raw = 0;
    adc_oneshot_read(s_adc, ADC_CHANNEL_7, &raw);
    gpio_set_level(BOARD_VBAT_ADC_EN_GPIO, 0);
    /* Divider: adjust R26/R27 from schematic */
    const float v = (raw / 4095.0f) * 3.3f * 2.2f;
    return v;
}

static battery_state_t classify(float v)
{
    if (v >= 4.10f) {
        return BATTERY_STATE_FULL;
    }
    if (v >= 3.70f) {
        return BATTERY_STATE_NORMAL;
    }
    if (v >= 3.50f) {
        return BATTERY_STATE_LOW;
    }
    return BATTERY_STATE_CRITICAL;
}

static void power_task(void *arg)
{
    (void)arg;
    for (;;) {
        s_status.voltage_v = read_battery_voltage();
        s_status.state = classify(s_status.voltage_v);
        if (s_status.voltage_v >= 4.15f) {
            s_status.percent = 100;
        } else if (s_status.voltage_v <= 3.40f) {
            s_status.percent = 5;
        } else {
            s_status.percent = (int)((s_status.voltage_v - 3.40f) / (4.15f - 3.40f) * 100.0f);
        }
        ESP_LOGD(TAG, "VBAT %.2fV (%d%%)", s_status.voltage_v, s_status.percent);
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

esp_err_t power_manager_init(void)
{
    adc_oneshot_unit_init_cfg_t init = { .unit_id = ADC_UNIT_1 };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init, &s_adc));
    adc_oneshot_chan_cfg_t chan = { .bitwidth = ADC_BITWIDTH_12, .atten = ADC_ATTEN_DB_12 };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_adc, ADC_CHANNEL_7, &chan));
    return ESP_OK;
}

esp_err_t power_manager_start_task(void)
{
    const BaseType_t ok = xTaskCreatePinnedToCore(power_task, "power", 3072, NULL, 3, NULL, 0);
    return ok == pdPASS ? ESP_OK : ESP_FAIL;
}

esp_err_t power_manager_get_status(power_status_t *out)
{
    if (!out) {
        return ESP_ERR_INVALID_ARG;
    }
    *out = s_status;
    return ESP_OK;
}
