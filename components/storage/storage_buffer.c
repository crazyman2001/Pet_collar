#include "storage_buffer.h"

#include "esp_log.h"
#include <string.h>

static const char *TAG = "storage";
static size_t s_pending;

esp_err_t storage_buffer_init(void)
{
    /* TODO: mount SPIFFS/LittleFS on `storage` partition, circular queue for 7-day retention */
    ESP_LOGI(TAG, "Offline buffer initialized (stub)");
    s_pending = 0;
    return ESP_OK;
}

esp_err_t storage_buffer_append(const char *csv, size_t len)
{
    if (!csv || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    (void)len;
    s_pending++;
    ESP_LOGD(TAG, "Buffered row, pending=%u", (unsigned)s_pending);
    return ESP_OK;
}

esp_err_t storage_buffer_flush_next(char *buf, size_t buf_len, size_t *out_len)
{
    if (!buf || s_pending == 0) {
        return ESP_ERR_NOT_FOUND;
    }
    /* Stub — pop oldest row from flash queue */
    strncpy(buf, "0,0,900,,,,,,,,,,,,,,,,,,,,,,,,,", buf_len - 1);
    buf[buf_len - 1] = '\0';
    if (out_len) {
        *out_len = strlen(buf);
    }
    s_pending--;
    return ESP_OK;
}

size_t storage_buffer_pending_count(void)
{
    return s_pending;
}
