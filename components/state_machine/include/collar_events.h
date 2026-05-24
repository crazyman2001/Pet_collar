/**
 * @file collar_events.h
 * @brief Events fed into the supervisor state machine (expandable enum)
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    COLLAR_EVT_NONE = 0,
    COLLAR_EVT_BOOT,
    COLLAR_EVT_TIMER_WAKE,
    COLLAR_EVT_SENSOR_BATCH_READY,
    COLLAR_EVT_WIFI_CONNECTED,
    COLLAR_EVT_WIFI_DISCONNECTED,
    COLLAR_EVT_WIFI_FAIL_THRESHOLD,
    COLLAR_EVT_PPP_CONNECTED,
    COLLAR_EVT_PPP_DISCONNECTED,
    COLLAR_EVT_MQTT_CONNECTED,
    COLLAR_EVT_MQTT_DISCONNECTED,
    COLLAR_EVT_GPS_FIX,
    COLLAR_EVT_GPS_TIMEOUT,
    COLLAR_EVT_EMERGENCY,
    COLLAR_EVT_EMERGENCY_CLEARED,
    COLLAR_EVT_CHARGER_ATTACHED,
    COLLAR_EVT_CHARGER_DETACHED,
    COLLAR_EVT_BATTERY_LOW,
    COLLAR_EVT_BATTERY_CRITICAL,
    COLLAR_EVT_CLOUD_CMD,
    COLLAR_EVT_OTA_REQUEST,
    COLLAR_EVT_SHUTDOWN,
    COLLAR_EVT_MAX
} collar_event_id_t;

typedef struct {
    collar_event_id_t id;
    int32_t param;
    void *user_data;
} collar_event_t;

#ifdef __cplusplus
}
#endif
