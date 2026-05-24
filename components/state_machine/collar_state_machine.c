#include "collar_state_machine.h"

#include "esp_log.h"
#include "freertos/queue.h"

static const char *TAG = "state";

static collar_state_machine_config_t s_cfg;
static collar_state_t s_state = COLLAR_STATE_DEEP_SLEEP;
static collar_link_t s_link = COLLAR_LINK_NONE;
static uint32_t s_report_interval_sec = COLLAR_REPORT_NORMAL_15M;
static bool s_emergency;
static QueueHandle_t s_evt_queue;

static collar_state_t transition(collar_state_t next)
{
    if (next == s_state) {
        return s_state;
    }
    collar_state_t prev = s_state;
    if (s_cfg.on_exit) {
        s_cfg.on_exit(prev, next);
    }
    s_state = next;
    ESP_LOGI(TAG, "%s -> %s", collar_state_name(prev), collar_state_name(next));
    if (s_cfg.on_enter) {
        s_cfg.on_enter(prev, next);
    }
    return s_state;
}

static void handle_event(const collar_event_t *evt)
{
    switch (evt->id) {
    case COLLAR_EVT_BOOT:
        transition(COLLAR_STATE_SENSOR_SAMPLING);
        break;

    case COLLAR_EVT_TIMER_WAKE:
        if (s_state == COLLAR_STATE_DEEP_SLEEP) {
            transition(COLLAR_STATE_SENSOR_SAMPLING);
        }
        break;

    case COLLAR_EVT_SENSOR_BATCH_READY:
        if (!s_emergency && s_state != COLLAR_STATE_EMERGENCY_ALARM) {
            /* Remain in sampling until connectivity layer promotes link state */
        }
        break;

    case COLLAR_EVT_WIFI_CONNECTED:
        s_link = COLLAR_LINK_WIFI;
        transition(COLLAR_STATE_WIFI_CONNECTED);
        break;

    case COLLAR_EVT_WIFI_DISCONNECTED:
        if (s_link == COLLAR_LINK_WIFI) {
            s_link = COLLAR_LINK_NONE;
        }
        if (s_state == COLLAR_STATE_WIFI_CONNECTED) {
            transition(COLLAR_STATE_SENSOR_SAMPLING);
        }
        break;

    case COLLAR_EVT_WIFI_FAIL_THRESHOLD:
        if (s_link != COLLAR_LINK_WIFI) {
            transition(COLLAR_STATE_LTE_TRANSMISSION);
        }
        break;

    case COLLAR_EVT_PPP_CONNECTED:
        s_link = COLLAR_LINK_PPP;
        if (s_state != COLLAR_STATE_EMERGENCY_ALARM) {
            transition(COLLAR_STATE_LTE_TRANSMISSION);
        }
        break;

    case COLLAR_EVT_PPP_DISCONNECTED:
        if (s_link == COLLAR_LINK_PPP) {
            s_link = COLLAR_LINK_NONE;
        }
        break;

    case COLLAR_EVT_MQTT_CONNECTED:
        break;

    case COLLAR_EVT_MQTT_DISCONNECTED:
        break;

    case COLLAR_EVT_GPS_FIX:
    case COLLAR_EVT_GPS_TIMEOUT:
        if (s_state == COLLAR_STATE_GNSS_ACQUISITION) {
            transition(COLLAR_STATE_SENSOR_SAMPLING);
        }
        break;

    case COLLAR_EVT_EMERGENCY:
        s_emergency = true;
        s_report_interval_sec = COLLAR_REPORT_ANOMALY_5M;
        transition(COLLAR_STATE_EMERGENCY_ALARM);
        break;

    case COLLAR_EVT_EMERGENCY_CLEARED:
        s_emergency = false;
        s_report_interval_sec = COLLAR_REPORT_NORMAL_15M;
        transition(COLLAR_STATE_SENSOR_SAMPLING);
        break;

    case COLLAR_EVT_CHARGER_ATTACHED:
        transition(COLLAR_STATE_CHARGING);
        break;

    case COLLAR_EVT_CHARGER_DETACHED:
        transition(COLLAR_STATE_SENSOR_SAMPLING);
        break;

    case COLLAR_EVT_BATTERY_CRITICAL:
        /* High-power ops restricted except alarms (SRS FR-18) */
        break;

    case COLLAR_EVT_CLOUD_CMD:
        if (evt->param > 0) {
            s_report_interval_sec = (uint32_t)evt->param;
        }
        break;

    case COLLAR_EVT_SHUTDOWN:
        transition(COLLAR_STATE_OFF);
        break;

    default:
        break;
    }

    /* GNSS when Wi-Fi is down (user + schematic: L89HA) */
    if (evt->id == COLLAR_EVT_WIFI_DISCONNECTED || evt->id == COLLAR_EVT_WIFI_FAIL_THRESHOLD) {
        if (s_link != COLLAR_LINK_WIFI && !s_emergency) {
            transition(COLLAR_STATE_GNSS_ACQUISITION);
        }
    }
}

const char *collar_state_name(collar_state_t state)
{
    static const char *names[] = {
        "OFF", "DEEP_SLEEP", "SENSOR_SAMPLING", "WIFI_CONNECTED", "LTE_TRANSMISSION",
        "GNSS_ACQUISITION", "EMERGENCY_ALARM", "CHARGING",
    };
    if (state >= COLLAR_STATE_MAX) {
        return "UNKNOWN";
    }
    return names[state];
}

esp_err_t collar_state_machine_init(const collar_state_machine_config_t *cfg)
{
    s_cfg = *cfg;
    s_evt_queue = xQueueCreate(16, sizeof(collar_event_t));
    if (!s_evt_queue) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t collar_state_machine_post_event(const collar_event_t *evt, TickType_t wait_ticks)
{
    if (!s_evt_queue || !evt) {
        return ESP_ERR_INVALID_STATE;
    }
    return xQueueSend(s_evt_queue, evt, wait_ticks) == pdTRUE ? ESP_OK : ESP_ERR_TIMEOUT;
}

esp_err_t collar_state_machine_process(TickType_t wait_ticks)
{
    collar_event_t evt;
    if (xQueueReceive(s_evt_queue, &evt, wait_ticks) == pdTRUE) {
        handle_event(&evt);
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
}

collar_state_t collar_state_machine_get_state(void)
{
    return s_state;
}

collar_link_t collar_state_machine_get_link(void)
{
    return s_link;
}

void collar_state_machine_set_link(collar_link_t link)
{
    s_link = link;
}

uint32_t collar_state_machine_get_report_interval_sec(void)
{
    return s_report_interval_sec;
}

void collar_state_machine_set_report_interval_sec(uint32_t sec)
{
    s_report_interval_sec = sec;
}

bool collar_state_machine_is_emergency(void)
{
    return s_emergency;
}

bool collar_state_machine_gps_allowed(void)
{
    return s_link != COLLAR_LINK_WIFI;
}

bool collar_state_machine_modem_allowed(void)
{
    return s_link != COLLAR_LINK_WIFI;
}
