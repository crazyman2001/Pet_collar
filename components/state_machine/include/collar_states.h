/**
 * @file collar_states.h
 * @brief System states and modes (SRS §5, integration spec lifecycle)
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** High-level operational states */
typedef enum {
    COLLAR_STATE_OFF = 0,           /**< Shipping / storage — no telemetry */
    COLLAR_STATE_DEEP_SLEEP,        /**< Idle, minimal wake */
    COLLAR_STATE_SENSOR_SAMPLING,   /**< Periodic vitals + IMU */
    COLLAR_STATE_WIFI_CONNECTED,    /**< Wi-Fi up, MQTT active, GPS/modem off */
    COLLAR_STATE_LTE_TRANSMISSION,  /**< PPP over A7670, MQTT/flush */
    COLLAR_STATE_GNSS_ACQUISITION,  /**< L89HA fix (Wi-Fi down) */
    COLLAR_STATE_EMERGENCY_ALARM,   /**< Overrides intervals — immediate uplink */
    COLLAR_STATE_CHARGING,          /**< USB present — reporting may continue */
    COLLAR_STATE_MAX
} collar_state_t;

/** Connectivity sub-state (orthogonal to main state) */
typedef enum {
    COLLAR_LINK_NONE = 0,
    COLLAR_LINK_WIFI,
    COLLAR_LINK_PPP,
} collar_link_t;

/** Activity classification for adaptive push (SRS DP-1..DP-4) */
typedef enum {
    COLLAR_ACTIVITY_UNKNOWN = 0,
    COLLAR_ACTIVITY_RESTING,
    COLLAR_ACTIVITY_WALKING,
    COLLAR_ACTIVITY_RUNNING,
} collar_activity_t;

/** Reporting profile */
typedef enum {
    COLLAR_REPORT_NORMAL_15M = 900,
    COLLAR_REPORT_REST_30M = 1800,
    COLLAR_REPORT_SLEEP_1H = 3600,
    COLLAR_REPORT_ANOMALY_5M = 300,
} collar_report_interval_sec_t;

const char *collar_state_name(collar_state_t state);

#ifdef __cplusplus
}
#endif
