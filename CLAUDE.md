The role of this file is to describe common mistakes and confusion points that agents might encounter as they work in
this project. If you ever encounter something in the project that surprises you, please alert the developer working with
you and indicate that this is the case in the CLAUDE.md file to help prevent future agents from having the same issue.

# CLAUDE.md - nd2-ieloop-sniffer

Read @README.md to understand the project purpose and its wiring details.

**IMPORTANT — terminal naming collision:** The docs folder contains `inspection_BCM.pdf` and `inspection_PCM.pdf`. Both
modules use the same terminal naming convention (1A, 1K, 1L, 1H, 2A, etc.) for their own connectors, but the signal
definitions are completely different. For example, BCM terminal 1K = body ground, BCM terminal 1L = body ground, BCM
terminal 1H = map light power supply — none of these have anything to do with the DC-DC converter terminals of the same
names. Always verify which ECU's connector is being referenced.

## Build System

- **Tool:** PlatformIO (`platformio.ini`, environment `uno_r4_wifi`, platform `renesas-ra`, framework `arduino`)
- **Builds run from VS Code**, not the CLI — do not suggest `pio run` terminal commands
- **Serial monitor baud rate:** 115200
- **CAN bus speed:** 500 kbps (`CanBitRate::BR_500k`) — Mazda HS-CAN standard
