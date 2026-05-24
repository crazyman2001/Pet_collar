#include "collar_tasks.h"

#include "board_init.h"
#include "collar_supervisor.h"
#include "connectivity_manager.h"
#include "gps_l89.h"
#include "modem_manager.h"
#include "mqtt_client.h"
#include "power_manager.h"
#include "sensor_manager.h"
#include "storage_buffer.h"

#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "app";

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "KtinosCare dog collar firmware boot");

    ESP_ERROR_CHECK(board_init());
    ESP_ERROR_CHECK(modem_manager_init());
    ESP_ERROR_CHECK(gps_l89_init());
    ESP_ERROR_CHECK(power_manager_init());
    ESP_ERROR_CHECK(sensor_manager_init());
    ESP_ERROR_CHECK(storage_buffer_init());
    ESP_ERROR_CHECK(collar_mqtt_init());

    ESP_ERROR_CHECK(collar_supervisor_start());
    ESP_ERROR_CHECK(connectivity_manager_start());
    ESP_ERROR_CHECK(collar_mqtt_start_task());
    ESP_ERROR_CHECK(sensor_manager_start_task());
    ESP_ERROR_CHECK(power_manager_start_task());
    app_telemetry_start();

    ESP_LOGI(TAG, "All tasks started — Wi-Fi primary, LTE PPP + L89 GPS fallback");
}
