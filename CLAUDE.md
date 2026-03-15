The role of this file is to describe common mistakes and confusion points that agents might encounter as they work in
this project. If you ever encounter something in the project that surprises you, please alert the developer working with
you and indicate that this is the case in the CLAUDE.md file to help prevent future agents from having the same issue.

# CLAUDE.md - nd2-ieloop-sniffer

## Project Purpose

Reverse engineering the i-ELOOP regenerative braking system on a 2019 Mazda MX-5 ND2. The device:

1. Reads 6 analog signals from the DC-DC converter via the Arduino's ADC pins
2. Encodes those readings into a custom-ID CAN frame injected into the car's CAN bus
3. Records all CAN traffic (car frames + injected frame) for offline analysis to identify and decode the DC-DC
   converter's own CAN messages

## Hardware

- **MCU board:** Arduino UNO R4 WiFi (`uno_r4_wifi`) — Renesas RA6M5, Cortex-M4 @ 48 MHz
- **CAN transceiver:** SN65HVD230 (external, wired to the board's hardware CAN TX/RX pins)
- **ADC:** 14-bit (not the 10-bit of classic AVR Arduinos) — scale values accordingly

The UNO R4 WiFi has an ESP32-S3 as a WiFi/BLE coprocessor. The CAN controller is on the RA6M5, not the ESP32.
The `Arduino_CAN` library communicates with the RA6M5's built-in hardware CAN controller through the SN65HVD230
transceiver. This is **not** an MCP2515 SPI module (a common misconception in Arduino CAN examples).

## Build System

- **Tool:** PlatformIO (`platformio.ini`, environment `uno_r4_wifi`, platform `renesas-ra`, framework `arduino`)
- **Builds run from VS Code**, not the CLI — do not suggest `pio run` terminal commands
- **Serial monitor baud rate:** 115200
- **CAN bus speed:** 500 kbps (`CanBitRate::BR_500k`) — Mazda HS-CAN standard

## i-ELOOP Domain Knowledge

The i-ELOOP system has three main components: a variable-voltage generator (12–24.3 V), an electric double-layer
capacitor (~24.3 V max), and a DC-DC converter that steps the capacitor voltage down to 11–16 V for vehicle loads.

**Key voltage thresholds:**
- 24.3 V — maximum capacitor charge (9 cells × 2.7 V)
- 12–16 V — normal DC-DC converter output range
- 7.5 V — minimum capacitor voltage; below this the system enters bypass or pre-charge mode
- 131 °C — temperature threshold that triggers bypass mode in the voltage reduction circuit

**Operating modes** (each produces distinct CAN traffic and relay switching patterns):
- Regenerative braking — capacitor charges at up to 24.3 V during fuel-cut deceleration
- Conventional generation — 14 V generator output when capacitor is low
- Capacitor power supply — capacitor feeds vehicle loads through DC-DC converter
- Battery power supply — fallback when capacitor depleted
- i-stop mode — capacitor + battery share power during engine restart
- Bypass mode — generator feeds loads directly, DC-DC converter disabled (fault or overload ≥ 75 A)
- Pre-charge mode — slowly charges capacitor from battery or generator when voltage < 7.5 V

**DC-DC converter has 5 internal relays** whose switching state changes per mode — these are primary targets for CAN
frame identification during offline analysis.

**CAN bus:** The PCM controls the DC-DC converter via CAN signals (mode commands, relay control, excitation requests).
The DC-DC converter sends back status, current, temperature, and voltage feedback. These are the frames this project
aims to decode.
