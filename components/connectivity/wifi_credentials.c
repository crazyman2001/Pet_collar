#include "wifi_credentials.h"

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <string.h>

static const char *TAG = "wifi_cred";

static bool ssid_valid(const char *ssid)
{
    return ssid && ssid[0] != '\0' && strlen(ssid) <= WIFI_CRED_MAX_SSID_LEN;
}

bool wifi_credentials_is_stored(void)
{
    char ssid[WIFI_CRED_MAX_SSID_LEN + 1];
    char pass[WIFI_CRED_MAX_PASS_LEN + 1];
    return wifi_credentials_load(ssid, sizeof(ssid), pass, sizeof(pass)) == ESP_OK;
}

esp_err_t wifi_credentials_load(char *ssid, size_t ssid_len, char *pass, size_t pass_len)
{
    if (!ssid || ssid_len < 2) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(WIFI_CRED_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t required = ssid_len;
    err = nvs_get_str(handle, "ssid", ssid, &required);
    if (err == ESP_OK && pass && pass_len > 0) {
        required = pass_len;
        esp_err_t perr = nvs_get_str(handle, "pass", pass, &required);
        if (perr == ESP_ERR_NVS_NOT_FOUND) {
            pass[0] = '\0';
        } else if (perr != ESP_OK) {
            nvs_close(handle);
            return perr;
        }
    } else if (err == ESP_OK && pass) {
        pass[0] = '\0';
    }

    nvs_close(handle);

    if (err != ESP_OK) {
        return err;
    }
    if (!ssid_valid(ssid)) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t wifi_credentials_save(const char *ssid, const char *pass)
{
    if (!ssid_valid(ssid)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (pass && strlen(pass) > WIFI_CRED_MAX_PASS_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(WIFI_CRED_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_str(handle, "ssid", ssid);
    if (err == ESP_OK) {
        err = nvs_set_str(handle, "pass", pass ? pass : "");
    }
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Saved Wi-Fi credentials for SSID '%s'", ssid);
    }
    return err;
}

esp_err_t wifi_credentials_clear(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(WIFI_CRED_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }
    nvs_erase_key(handle, "ssid");
    nvs_erase_key(handle, "pass");
    err = nvs_commit(handle);
    nvs_close(handle);
    return err;
}
