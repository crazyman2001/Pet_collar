idf_component_register(
    SRCS "mqtt_client.c"
    INCLUDE_DIRS "include"
    REQUIRES connectivity state_machine esp_event log mqtt
    PRIV_REQUIRES mbedtls
)
