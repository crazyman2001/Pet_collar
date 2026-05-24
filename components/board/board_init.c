#include "board_init.h"
#include "board_pins.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "board";
static i2c_master_bus_handle_t s_i2c_bus;

i2c_master_bus_handle_t board_get_i2c_bus(void)
{
    return s_i2c_bus;
}

esp_err_t board_init(void)
{
    const gpio_config_t outputs = {
        .pin_bit_mask = (1ULL << BOARD_MODEM_PWR_GPIO) | (1ULL << BOARD_MODEM_RST_GPIO) |
                        (1ULL << BOARD_MODEM_POWER_HOLD) | (1ULL << BOARD_GPS_RST_GPIO) |
                        (1ULL << BOARD_VBAT_ADC_EN_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&outputs));

    gpio_set_level(BOARD_MODEM_PWR_GPIO, 0);
    gpio_set_level(BOARD_MODEM_RST_GPIO, 1);
    gpio_set_level(BOARD_MODEM_POWER_HOLD, 0);
    gpio_set_level(BOARD_GPS_RST_GPIO, 1);
    gpio_set_level(BOARD_VBAT_ADC_EN_GPIO, 0);

    const gpio_config_t inputs = {
        .pin_bit_mask = (1ULL << BOARD_LIGHT_INT_GPIO) | (1ULL << BOARD_MEMS_INT_GPIO) |
                        (1ULL << BOARD_RESP_INT_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&inputs));

    const i2c_master_bus_config_t bus_cfg = {
        .i2c_port = BOARD_I2C_PORT,
        .sda_io_num = BOARD_I2C_SDA_GPIO,
        .scl_io_num = BOARD_I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &s_i2c_bus));

    ESP_LOGI(TAG, "Board GPIO and I2C initialized");
    return ESP_OK;
}
