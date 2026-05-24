/**
 * @file collar_tasks.h
 * @brief FreeRTOS task map for the dog collar firmware
 *
 * | Task           | Core | Priority | Responsibility                          |
 * |----------------|------|----------|-----------------------------------------|
 * | supervisor     | 0    | 5        | State machine event loop                |
 * | connectivity   | 1    | 6        | Wi-Fi first, PPP fallback policy        |
 * | mqtt           | 1    | 5        | mTLS MQTT session, publish/subscribe    |
 * | sensors        | 0    | 4        | I2C vitals + IMU + ALS sampling         |
 * | gps_l89        | 1    | 4        | L89HA NMEA parse (Wi-Fi down only)      |
 * | power          | 0    | 3        | Battery ADC, threshold events           |
 * | telemetry      | 1    | 4        | CSV build, buffer flush, uplink         |
 *
 * Modem power/PPP runs inside connectivity task via modem_manager (no dedicated task yet).
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void app_telemetry_start(void);

#ifdef __cplusplus
}
#endif
