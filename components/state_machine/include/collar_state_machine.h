#pragma once

#include "collar_events.h"
#include "collar_states.h"

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*collar_state_enter_cb_t)(collar_state_t prev, collar_state_t next);
typedef void (*collar_state_exit_cb_t)(collar_state_t prev, collar_state_t next);

typedef struct {
    collar_state_enter_cb_t on_enter;
    collar_state_exit_cb_t on_exit;
} collar_state_machine_config_t;

esp_err_t collar_state_machine_init(const collar_state_machine_config_t *cfg);
esp_err_t collar_state_machine_post_event(const collar_event_t *evt, TickType_t wait_ticks);
/** Called from supervisor task — drains event queue and runs transitions. */
esp_err_t collar_state_machine_process(TickType_t wait_ticks);
collar_state_t collar_state_machine_get_state(void);
collar_link_t collar_state_machine_get_link(void);
void collar_state_machine_set_link(collar_link_t link);
uint32_t collar_state_machine_get_report_interval_sec(void);
void collar_state_machine_set_report_interval_sec(uint32_t sec);
bool collar_state_machine_is_emergency(void);
bool collar_state_machine_gps_allowed(void);
bool collar_state_machine_modem_allowed(void);

#ifdef __cplusplus
}
#endif
