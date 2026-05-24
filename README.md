# KtinosCare Dog Health Collar — ESP-IDF Firmware

Firmware for the smart dog health monitoring collar (ESP32-WROOM), aligned with:

- `Dog_collar_SRS_V1.1` — functional requirements and system states
- `final_3.pdf` — hardware schematic (PET_COLLAR_1)
- `KtinosCare_Full_Architecture_Integration_Specification_v4` — MQTT/mTLS, CSV telemetry, cloud integration

## Connectivity policy

| Priority | Link | When |
|----------|------|------|
| 1 | **Wi-Fi** | Saved AP available; primary TCP/IP for MQTT/HTTPS |
| 2 | **4G PPP** | Wi-Fi unavailable after handshake threshold; A7670C modem powered via load switch |
| GPS | **L89HA** | Active when Wi-Fi is lost (standalone GNSS UART); powered down when Wi-Fi is connected |

Cellular modem and GPS are **not** used while Wi-Fi is connected (SRS FR-10a).

## Architecture documentation

Detailed design and code-level implementation guides are in **[docs/](./docs/)**:

| Document | Description |
|----------|-------------|
| [docs/ARCHITECTURE.md](./docs/ARCHITECTURE.md) | System context, layers, boot flow, data paths |
| [docs/STATE_MACHINE.md](./docs/STATE_MACHINE.md) | States, events, transitions |
| [docs/RTOS_AND_TASKS.md](./docs/RTOS_AND_TASKS.md) | FreeRTOS tasks, cores, IPC |
| [docs/COMPONENT_IMPLEMENTATION.md](./docs/COMPONENT_IMPLEMENTATION.md) | Per-module APIs and source walkthrough |
| [docs/CONNECTIVITY_AND_CLOUD.md](./docs/CONNECTIVITY_AND_CLOUD.md) | Wi-Fi, PPP, GPS, MQTT, CSV, buffering |
| [docs/HARDWARE_PLATFORM.md](./docs/HARDWARE_PLATFORM.md) | Pin map, sensors, power |

System states: `components/state_machine/include/collar_states.h` (SRS §5).

## Build

Requires [ESP-IDF v5.x](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/).

```bash
cd prj_code
idf.py set-target esp32
idf.py build
idf.py -p COMx flash monitor
```

Copy `sdkconfig.defaults` overrides or run `idf.py menuconfig` for Wi-Fi credentials (`Example Configuration` → set via `collar_config` NVS at runtime).

## Configuration

- **Board pins**: `components/board/include/board_pins.h` (verify against `design/final_3.pdf`)
- **Cloud**: `components/telemetry/include/telemetry_csv.h`, `components/cloud_mqtt/include/mqtt_topics.h`
- **Kconfig**: `idf.py menuconfig` → `KtinosCare Collar Configuration`

## Project layout

```
prj_code/
├── main/                 app entry, task creation
├── components/
│   ├── board/            GPIO / UART / I2C map
│   ├── state_machine/    states, events, supervisor
│   ├── connectivity/     Wi-Fi manager, link policy
│   ├── modem/            A7670 power, PPP netif
│   ├── gps/              L89HA driver
│   ├── cloud_mqtt/         mTLS client, backoff
│   ├── telemetry/        CSV frame builder
│   ├── sensors/          sensor aggregation
│   ├── power/            battery + sleep
│   └── storage/          flash queue for blackout
```

## License

Proprietary — Bhumi Embedded Solutions / KtinosCare.
