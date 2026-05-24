#pragma once

/**
 * MQTT topic namespace (KtinosCare integration spec §4.6)
 * Replace {device_uid} at runtime from NVS provisioning.
 */
#define MQTT_TOPIC_TELEMETRY_FMT   "ktinoskare/device/%s/telemetry"
#define MQTT_TOPIC_ALERTS_FMT      "ktinoskare/device/%s/alerts"
#define MQTT_TOPIC_CMD_FMT         "ktinoskare/device/%s/cmd"
#define MQTT_TOPIC_STATUS_FMT      "ktinoskare/device/%s/status"

#define MQTT_BROKER_PORT           8883
#define MQTT_KEEPALIVE_SEC         240
