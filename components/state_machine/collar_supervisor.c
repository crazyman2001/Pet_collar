#include "collar_supervisor.h"
#include "collar_state_machine.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "supervisor";

static void on_state_enter(collar_state_t prev, collar_state_t next)
{
    (void)prev;
    ESP_LOGI(TAG, "Entered %s", collar_state_name(next));
}

static void supervisor_task(void *arg)
{
    (void)arg;
    collar_event_t boot = { .id = COLLAR_EVT_BOOT };
    collar_state_machine_post_event(&boot, portMAX_DELAY);

    for (;;) {
        collar_state_machine_process(pdMS_TO_TICKS(500));
    }
}

esp_err_t collar_supervisor_start(void)
{
    const collar_state_machine_config_t cfg = {
        .on_enter = on_state_enter,
        .on_exit = NULL,
    };
    ESP_ERROR_CHECK(collar_state_machine_init(&cfg));

    const BaseType_t ok = xTaskCreatePinnedToCore(
        supervisor_task, "supervisor", 4096, NULL, 5, NULL, 0);
    return ok == pdPASS ? ESP_OK : ESP_FAIL;
}
