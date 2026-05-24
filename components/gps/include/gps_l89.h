#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool valid;
    double latitude;
    double longitude;
    uint32_t fix_time_sec;
} gps_fix_t;

esp_err_t gps_l89_init(void);
esp_err_t gps_l89_start_task(void);
esp_err_t gps_l89_get_last_fix(gps_fix_t *out);

#ifdef __cplusplus
}
#endif
