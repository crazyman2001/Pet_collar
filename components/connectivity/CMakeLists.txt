idf_component_register(
    SRCS "connectivity_manager.c" "wifi_manager.c"
    INCLUDE_DIRS "include"
    REQUIRES board state_machine esp_wifi esp_netif esp_event nvs_flash log
    PRIV_REQUIRES modem
)
