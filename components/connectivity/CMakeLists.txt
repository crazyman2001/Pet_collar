idf_component_register(
    SRCS "wifi_prov_http.c" "wifi_credentials.c" "connectivity_manager.c" "wifi_manager.c"
    INCLUDE_DIRS "include"
    REQUIRES board state_machine esp_wifi esp_netif esp_event esp_http_server nvs_flash log
    PRIV_REQUIRES modem
)
