# KtinosCare Collar — Architecture Documentation

Technical documentation for the `ktinos_collar` ESP-IDF firmware in `prj_code/`.

## Source references

| Document | Location | Role |
|----------|----------|------|
| Software Requirements Specification v1.1 | `Dog_collar_SRS_V1.1.pdf` | Functional requirements, system states (§5), data-push policies (DP-1..4) |
| Hardware schematic PET_COLLAR_1 | `design/final_3.pdf` | ESP32, A7670C, L89HA, sensors, power rails |
| Cloud integration contract v4 | `doc/KtinosCare_Full_Architecture_Integration_Specification_v4.pdf` | MQTT/mTLS, CSV schema, topics, buffering, OTA |

## Documentation index

| # | Document | Contents |
|---|----------|----------|
| 1 | [ARCHITECTURE.md](./ARCHITECTURE.md) | System context, layered design, boot flow, component map, extension points |
| 2 | [STATE_MACHINE.md](./STATE_MACHINE.md) | States, events, transitions, reporting intervals, implementation in `collar_state_machine.c` |
| 3 | [RTOS_AND_TASKS.md](./RTOS_AND_TASKS.md) | FreeRTOS tasks, cores, priorities, inter-task communication |
| 4 | [COMPONENT_IMPLEMENTATION.md](./COMPONENT_IMPLEMENTATION.md) | Per-module API, source files, stubs vs. complete logic |
| 5 | [CONNECTIVITY_AND_CLOUD.md](./CONNECTIVITY_AND_CLOUD.md) | Wi-Fi, LTE PPP, GPS policy, MQTT, CSV telemetry, offline buffer |
| 6 | [HARDWARE_PLATFORM.md](./HARDWARE_PLATFORM.md) | Pin map, buses, sensors, power subsystems |

## Quick navigation (code)

| Concern | Primary files |
|---------|----------------|
| Application entry | `main/app_main.c` |
| Telemetry uplink loop | `main/app_telemetry.c` |
| State machine | `components/state_machine/collar_state_machine.c` |
| Wi-Fi / failover | `components/connectivity/connectivity_manager.c` |
| Board pins | `components/board/include/board_pins.h` |
| CSV payload | `components/telemetry/telemetry_csv.c` |
