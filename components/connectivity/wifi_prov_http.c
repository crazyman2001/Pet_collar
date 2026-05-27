#include "wifi_prov_http.h"
#include "wifi_credentials.h"

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "wifi_prov";
static httpd_handle_t s_server;

static const char *PROV_HTML =
    "<!DOCTYPE html><html><head>"
    "<meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
    "<title>KtinosCare Collar Wi-Fi</title>"
  "<style>body{font-family:sans-serif;max-width:420px;margin:2rem auto;padding:0 1rem}"
    "input{width:100%;padding:8px;margin:6px 0 12px;box-sizing:border-box}"
    "button{width:100%;padding:12px;background:#2563eb;color:#fff;border:0;border-radius:6px}"
    "</style></head><body>"
    "<h2>Wi-Fi Setup</h2>"
    "<p>Enter your home network credentials. The collar will reboot and connect.</p>"
    "<form method=\"POST\" action=\"/configure\">"
    "<label>SSID</label><input name=\"ssid\" maxlength=\"32\" required>"
    "<label>Password</label><input name=\"password\" type=\"password\" maxlength=\"64\">"
    "<button type=\"submit\">Save &amp; Connect</button>"
    "</form></body></html>";

static char *url_decode_inplace(char *s)
{
    char *src = s;
    char *dst = s;
    while (*src) {
        if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else if (*src == '%' && src[1] && src[2]) {
            char hex[3] = { src[1], src[2], 0 };
            *dst++ = (char)strtol(hex, NULL, 16);
            src += 3;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
    return s;
}

static bool form_get_value(const char *body, const char *key, char *out, size_t out_len)
{
    char pattern[32];
    snprintf(pattern, sizeof(pattern), "%s=", key);
    const char *start = strstr(body, pattern);
    if (!start) {
        return false;
    }
    start += strlen(pattern);
    const char *end = strchr(start, '&');
    size_t len = end ? (size_t)(end - start) : strlen(start);
    if (len >= out_len) {
        len = out_len - 1;
    }
    memcpy(out, start, len);
    out[len] = '\0';
    url_decode_inplace(out);
    return out[0] != '\0' || strcmp(key, "password") == 0;
}

static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, PROV_HTML, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t configure_post_handler(httpd_req_t *req)
{
    if (req->content_len <= 0 || req->content_len > 256) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid body");
        return ESP_FAIL;
    }

    char body[257];
    int received = 0;
    while (received < req->content_len && received < (int)sizeof(body) - 1) {
        int r = httpd_req_recv(req, body + received, sizeof(body) - 1 - received);
        if (r <= 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Receive failed");
            return ESP_FAIL;
        }
        received += r;
    }
    body[received] = '\0';

    char ssid[WIFI_CRED_MAX_SSID_LEN + 1] = { 0 };
    char pass[WIFI_CRED_MAX_PASS_LEN + 1] = { 0 };

    if (!form_get_value(body, "ssid", ssid, sizeof(ssid))) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "SSID required");
        return ESP_FAIL;
    }
    form_get_value(body, "password", pass, sizeof(pass));

    esp_err_t err = wifi_credentials_save(ssid, pass);
    if (err != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "NVS save failed");
        return ESP_FAIL;
    }

    const char *ok =
        "<!DOCTYPE html><html><body><h2>Saved</h2>"
        "<p>Rebooting to connect to your Wi-Fi...</p></body></html>";
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, ok, HTTPD_RESP_USE_STRLEN);

    vTaskDelay(pdMS_TO_TICKS(1500));
    esp_restart();
    return ESP_OK;
}

esp_err_t wifi_prov_http_start(void)
{
    if (s_server) {
        return ESP_OK;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.lru_purge_enable = true;
    config.max_uri_handlers = 8;

    esp_err_t err = httpd_start(&s_server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start failed: %s", esp_err_to_name(err));
        return err;
    }

    httpd_uri_t root = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler,
    };
    httpd_uri_t configure = {
        .uri = "/configure",
        .method = HTTP_POST,
        .handler = configure_post_handler,
    };
    httpd_register_uri_handler(s_server, &root);
    httpd_register_uri_handler(s_server, &configure);

    ESP_LOGI(TAG, "Provisioning HTTP server on http://192.168.4.1/");
    return ESP_OK;
}

void wifi_prov_http_stop(void)
{
    if (s_server) {
        httpd_stop(s_server);
        s_server = NULL;
        ESP_LOGI(TAG, "Provisioning HTTP server stopped");
    }
}

bool wifi_prov_http_is_running(void)
{
    return s_server != NULL;
}
