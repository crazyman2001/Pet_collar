#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t board_init(void);

/** Shared I2C master bus for sensors (valid after board_init). */
i2c_master_bus_handle_t board_get_i2c_bus(void);

#ifdef __cplusplus
}
#endif
