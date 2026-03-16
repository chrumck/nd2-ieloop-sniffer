# Mazda MX-5 ND2 i-Eloop Sniffer

## Project Purpose

Reverse engineering the i-ELOOP regenerative braking system on a 2019 Mazda MX-5 ND2. The device:

1. Reads 6 analog signals from the DC-DC converter via the Arduino's ADC pins
2. Encodes those readings into a custom-ID CAN frame injected into the car's CAN bus

The entire CAN traffic including the custom frame will be recorded during vehicle's operation and used for identifying
and decoding the frames related to i-ELOOP.

## i-ELOOP System Overview

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

**DC-DC converter has 5 internal relays** whose switching state changes per mode.

**CAN bus:** It is not clear at this point how PCM exactly controls the DC-DC converter (and vice-versa), whether is via
CAN signals or by two additional wires directly connecting PCM to The DC-DC converter (terminals 1L and 1D on the DC-DC
converter).

## Sniffer Hardware

- **MCU board:** Arduino UNO R4 WiFi (`uno_r4_wifi`) — Renesas RA6M5, Cortex-M4 @ 48 MHz
- **CAN transceiver:** SN65HVD230 (external, wired to the board's hardware CAN TX/RX pins)
- **ADC:** 14-bit (not the 10-bit of classic AVR Arduinos) — scale values accordingly

The UNO R4 WiFi has an ESP32-S3 as a WiFi/BLE coprocessor. The CAN controller is on the RA6M5, not the ESP32. The
`Arduino_CAN` library communicates with the RA6M5's built-in hardware CAN controller through the SN65HVD230 transceiver.
This is **not** an MCP2515 SPI module (a common misconception in Arduino CAN examples).

## Signal Conditioning Circuit and ADC Channel Mapping

The Arduino's 6 ADC channels are split into two groups with different front-end circuits.

**Channels 0–3 — LM324N op-amp buffer follower + high-impedance voltage divider:**

- Voltage divider: 4.7 MΩ (top) / 680 kΩ (bottom), 1% precision
- Divider ratio: 680 / (4700 + 680) ≈ 0.1264 → scale factor ≈ ×7.91
- Total input impedance ≈ 5.38 MΩ (needed for terminals that require high-impedance probing)
- The LM324N is powered from Arduino Vin (~12V), dropped to ~10V via a Schottky diode for supply stability; output
  ceiling ≈ 8.5V (V⁺ − 1.5V), so the op-amp does **not** act as overvoltage protection for the 3.3V ADC — the divider
  ratio itself is the protection mechanism
- Maximum measurable input: ~3.3V × 7.91 ≈ 26.1V (covers the 24.3V capacitor range with margin)

**Channels 4–5 — Resistive divider only (no op-amp):**

- Voltage divider: 10 kΩ (top) / 1 kΩ (bottom), 1% precision
- Divider ratio: 1/11 → scale factor ×11
- Total input impedance = 11 kΩ
- Maximum measurable input: 3.3V × 11 = 36.3V

**ADC scaling (Arduino UNO R4 WiFi):**

- ADC is 14-bit (0–16383 counts), reference = 3.3V
- `V_at_pin = (raw_count / 16383.0) × 3.3`
- `V_actual = V_at_pin × scale_factor` (7.91 for ch 0–3, 11.0 for ch 4–5)

**Channel-to-terminal mapping:**

| ADC ch | DC-DC terminal | Expected signal                                                      | Notes                                   |
| ------ | -------------- | -------------------------------------------------------------------- | --------------------------------------- |
| 0      | 1L             | Unknown — PCM/DCDC direct comm                                       | Scope before connecting; may be digital |
| 1      | 1D             | Unknown — PCM/DCDC direct comm                                       | Scope before connecting; may be digital |
| 2      | 1A             | Supercapacitor pack thermistor output                                | High-impedance probing required         |
| 3      | 1K             | Voltage sense at mid-point of supercapacitor pack                    | High-impedance probing required         |
| 4      | 2A             | Total supercapacitor pack voltage (0–24.3 V)                         | Lower impedance signal                  |
| 5      | 1H             | Battery voltage at current sensor input — see wiring diagram 0140-1b | Lower impedance signal                  |

## Custom CAN Frame (ID 0x7F0)

Transmitted at 50 Hz. 6 ADC channels are sampled at 10-bit resolution (raw 14-bit reading right-shifted by 4, range
0–1023) and packed big-endian into an 8-byte payload with 4 spare zero bits at the end.

| Byte | Bits 7–0 contents       |
| ---- | ----------------------- |
| 0    | ch0[9:2]                |
| 1    | ch0[1:0] ‖ ch1[9:4]    |
| 2    | ch1[3:0] ‖ ch2[9:6]    |
| 3    | ch2[5:0] ‖ ch3[9:8]    |
| 4    | ch3[7:0]               |
| 5    | ch4[9:2]                |
| 6    | ch4[1:0] ‖ ch5[9:4]    |
| 7    | ch5[3:0] ‖ 0000         |

**Decoding a channel value `v` (0–1023) back to actual voltage:**

`V_actual = (v / 1023.0) × 3.3 × scale_factor`

where `scale_factor` = 7.91 for ch0–3, 11.0 for ch4–5.

**Terminals 1L and 1D — unknown, pre-investigation required:** Before connecting the sniffer to the car, terminals 1L
and 1D must be inspected with an oscilloscope to determine whether the signal is analog or digital. The PCM inspection
document shows PCM terminals 2C and 2AK as "DC-DC converter (i-ELOOP) control" with values of 0 or B+, which does not
give any meaningful insight. If the signals are digital, further investigation will be needed to establish the
communication protocol used by Mazda and a way devised to record this communication along with analog readouts.
