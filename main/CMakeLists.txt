idf_component_register(
    SRCS "app_main.c" "app_telemetry.c"
    INCLUDE_DIRS "include"
    REQUIRES nvs_flash board state_machine connectivity cloud_mqtt gps modem sensors power storage telemetry esp_system
)
