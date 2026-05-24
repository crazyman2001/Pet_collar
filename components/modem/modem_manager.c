#include "modem_manager.h"
#include "board_pins.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "modem";
static bool s_powered;
static bool s_ppp_up;

static esp_err_t modem_uart_init(void)
{
    const uart_config_t uart_cfg = {
        .baud_rate = BOARD_MODEM_UART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(BOARD_UART_MODEM, 2048, 2048, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(BOARD_UART_MODEM, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(
        BOARD_UART_MODEM, BOARD_MODEM_TX_GPIO, BOARD_MODEM_RX_GPIO,
        UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    return ESP_OK;
}

esp_err_t modem_manager_init(void)
{
    return modem_uart_init();
}

esp_err_t modem_manager_power_on(void)
{
    if (s_powered) {
        return ESP_OK;
    }
    gpio_set_level(BOARD_MODEM_PWR_GPIO, 1);
    gpio_set_level(BOARD_MODEM_POWER_HOLD, 1);
    gpio_set_level(BOARD_MODEM_RST_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(BOARD_MODEM_RST_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(BOARD_MODEM_BOOT_MS));
    s_powered = true;
    ESP_LOGI(TAG, "A7670 powered on");
    return ESP_OK;
}

esp_err_t modem_manager_power_off(void)
{
    if (!s_powered) {
        return ESP_OK;
    }
    modem_manager_stop_ppp();
    gpio_set_level(BOARD_MODEM_POWER_HOLD, 0);
    gpio_set_level(BOARD_MODEM_PWR_GPIO, 0);
    s_powered = false;
    ESP_LOGI(TAG, "A7670 powered off");
    return ESP_OK;
}

esp_err_t modem_manager_start_ppp(void)
{
    ESP_ERROR_CHECK(modem_manager_power_on());

    /*
     * TODO: AT dial-up + esp_netif PPP attach (esp_modem component or custom):
     *   AT+CGDCONT=1,"IP","<apn>"
     *   ATD*99***1#
     * Register PPP netif as default route when Wi-Fi is down.
     */
    ESP_LOGI(TAG, "PPP session stub — integrate esp_modem / lwIP PPP");
    s_ppp_up = true;
    return ESP_OK;
}

esp_err_t modem_manager_stop_ppp(void)
{
    s_ppp_up = false;
    return ESP_OK;
}

bool modem_manager_is_ppp_up(void)
{
    return s_ppp_up;
}
