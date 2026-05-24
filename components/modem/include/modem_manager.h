#pragma once

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t modem_manager_init(void);
esp_err_t modem_manager_power_on(void);
esp_err_t modem_manager_power_off(void);
esp_err_t modem_manager_start_ppp(void);
esp_err_t modem_manager_stop_ppp(void);
bool modem_manager_is_ppp_up(void);

#ifdef __cplusplus
}
#endif
