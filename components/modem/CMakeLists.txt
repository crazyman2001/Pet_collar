idf_component_register(
    SRCS "modem_manager.c"
    INCLUDE_DIRS "include"
    REQUIRES board driver esp_netif log
)
