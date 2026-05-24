idf_component_register(
    SRCS "gps_l89.c"
    INCLUDE_DIRS "include"
    REQUIRES board state_machine driver log
)
