idf_component_register(
    SRCS "telemetry_csv.c"
    INCLUDE_DIRS "include"
    REQUIRES gps sensors power connectivity
)
