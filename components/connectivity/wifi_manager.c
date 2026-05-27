#include "wifi_manager.h"
#include "wifi_credentials.h"
#include "wifi_prov_http.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"
#include <string.h>

static const char *TAG = "wifi";

#define WIFI_CONNECTED_BIT   BIT0
#define WIFI_FAIL_BIT        BIT1

static EventGroupHandle_t s_wifi_events;
static bool s_sta_got_ip;
static bool s_provisioning;
static bool s_initial_connect;
static uint8_t s_sta_retry_count;

static void start_provisioning_ap(void);
static void start_sta_with_credentials(const char *ssid, const char *pass);

static void wifi_event_handler(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg;
    (void)data;

    if (s_provisioning) {
        return;
    }

    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        s_sta_got_ip = false;
        if (s_initial_connect) {
            if (s_sta_retry_count < CONFIG_COLLAR_STA_RETRY_MAX) {
                s_sta_retry_count++;
                ESP_LOGW(TAG, "STA connect retry %u/%u", s_sta_retry_count,
                         CONFIG_COLLAR_STA_RETRY_MAX);
                esp_wifi_connect();
            } else {
                ESP_LOGE(TAG, "STA connect failed — entering provisioning AP");
                xEventGroupSetBits(s_wifi_events, WIFI_FAIL_BIT);
            }
        } else {
            ESP_LOGW(TAG, "Disconnected — retrying");
            esp_wifi_connect();
        }
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        s_sta_got_ip = true;
        s_initial_connect = false;
        s_sta_retry_count = 0;
        xEventGroupSetBits(s_wifi_events, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Connected to AP, got IP");
    }
}

static void start_provisioning_ap(void)
{
    s_provisioning = true;
    s_sta_got_ip = false;
    s_initial_connect = false;

    esp_wifi_stop();
    wifi_prov_http_stop();

    wifi_config_t ap_cfg = { 0 };
    strncpy((char *)ap_cfg.ap.ssid, CONFIG_COLLAR_PROV_AP_SSID, sizeof(ap_cfg.ap.ssid) - 1);
    ap_cfg.ap.ssid_len = strlen(CONFIG_COLLAR_PROV_AP_SSID);
    ap_cfg.ap.channel = CONFIG_COLLAR_PROV_AP_CHANNEL;
    ap_cfg.ap.max_connection = 4;

    if (strlen(CONFIG_COLLAR_PROV_AP_PASSWORD) > 0) {
        strncpy((char *)ap_cfg.ap.password, CONFIG_COLLAR_PROV_AP_PASSWORD,
                sizeof(ap_cfg.ap.password) - 1);
        ap_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
    } else {
        ap_cfg.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(wifi_prov_http_start());

    ESP_LOGI(TAG, "Provisioning AP active — SSID: %s", CONFIG_COLLAR_PROV_AP_SSID);
    ESP_LOGI(TAG, "Open http://192.168.4.1/ to configure Wi-Fi");
}

static void start_sta_with_credentials(const char *ssid, const char *pass)
{
    s_provisioning = false;
    s_sta_got_ip = false;
    s_initial_connect = true;
    s_sta_retry_count = 0;
    xEventGroupClearBits(s_wifi_events, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);

    wifi_prov_http_stop();

    wifi_config_t sta_cfg = { 0 };
    strncpy((char *)sta_cfg.sta.ssid, ssid, sizeof(sta_cfg.sta.ssid) - 1);
    if (pass) {
        strncpy((char *)sta_cfg.sta.password, pass, sizeof(sta_cfg.sta.password) - 1);
    }
    sta_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    esp_wifi_stop();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Connecting to saved SSID: %s", ssid);
}

esp_err_t wifi_manager_init(void)
{
    if (!s_wifi_events) {
        s_wifi_events = xEventGroupCreate();
        if (!s_wifi_events) {
            return ESP_ERR_NO_MEM;
        }
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t inst;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &inst));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &inst));

    return ESP_OK;
}

esp_err_t wifi_manager_start(void)
{
    char ssid[WIFI_CRED_MAX_SSID_LEN + 1] = { 0 };
    char pass[WIFI_CRED_MAX_PASS_LEN + 1] = { 0 };

    if (wifi_credentials_load(ssid, sizeof(ssid), pass, sizeof(pass)) == ESP_OK) {
        start_sta_with_credentials(ssid, pass);

        const TickType_t timeout_ticks =
            pdMS_TO_TICKS(CONFIG_COLLAR_STA_CONNECT_TIMEOUT_SEC * 1000);
        EventBits_t bits = xEventGroupWaitBits(
            s_wifi_events,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdTRUE,
            pdFALSE,
            timeout_ticks);

        if (bits & WIFI_CONNECTED_BIT) {
            return ESP_OK;
        }

        ESP_LOGW(TAG, "Could not connect to saved network — starting provisioning AP");
        start_provisioning_ap();
        return ESP_OK;
    }

    ESP_LOGW(TAG, "No Wi-Fi credentials in flash — starting provisioning AP");
    start_provisioning_ap();
    return ESP_OK;
}

bool wifi_manager_is_sta_connected(void)
{
    return s_sta_got_ip && !s_provisioning;
}

bool wifi_manager_is_provisioning(void)
{
    return s_provisioning;
}

bool wifi_manager_is_connected(void)
{
    return wifi_manager_is_sta_connected();
}

int8_t wifi_manager_get_rssi(void)
{
    if (!wifi_manager_is_sta_connected()) {
        return 0;
    }
    wifi_ap_record_t ap;
    if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
        return ap.rssi;
    }
    return 0;
}
