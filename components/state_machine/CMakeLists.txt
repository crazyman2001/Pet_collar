idf_component_register(
    SRCS "collar_state_machine.c" "collar_supervisor.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos esp_timer log
)
