#pragma once

#include "esp_err.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Max 5 historical rows/min on re-entry (integration spec §4.9) */
#define STORAGE_FLUSH_RATE_PER_MIN 5

esp_err_t storage_buffer_init(void);
esp_err_t storage_buffer_append(const char *csv, size_t len);
esp_err_t storage_buffer_flush_next(char *buf, size_t buf_len, size_t *out_len);
size_t storage_buffer_pending_count(void);

#ifdef __cplusplus
}
#endif
