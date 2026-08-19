# Chipintelli Chip Matrix

Use this matrix to route Chipintelli voice MCU work before choosing SDK recipes or examples.

| Chip or Family | Directory | Architecture/Core | Typical Header | Build Flow | Main Evidence |
|---|---|---|---|---|---|
| CI1102, CI1103 | `chips/ci110x/` | RISC-V Nuclei N201 | `ci110x_system.h` | Make + `riscv-nuclei-elf-gcc` | First-generation offline ASR examples |
| CI1122 | `chips/ci112x/` | RISC-V Nuclei N201 | `ci112x_system.h` | Make + `riscv-nuclei-elf-gcc` | Host MCU and USB examples |
| CI1301, CI1302, CI1303, CI1306 | `chips/ci130x/` | RISC-V Nuclei N300 | `ci130x_system.h` | Lua `source_file.prj` + Make | Offline ASR, dual-core, DPMU, peripheral examples |
| CI1311, CI1312, CI1316x, CI1324x, CI1332x | `chips/ci13lc/` | RISC-V Nuclei N300 | `ci13lc.h`, chip-specific headers | Lua `source_file.prj` + Make | Low-cost voice, CWSL, single/dual mic examples |
| CI13XX unified SDK | `chips/ci13xx/` | RISC-V Nuclei N300 | `ci13xx_system.h` | SDK-specific Make flow | Unified CI130x/CI13LC feature examples |
| CI2305, CI2306 | `chips/ci230x/` | ARM Cortex-M4 / LN882H combo | `ln882h.h` | CMake, GCC-ARM or Keil | Wi-Fi/BLE combo, online/offline voice, OTA |
| CI2312, CI23242 | `chips/ci23lc/` | RISC-V Nuclei N300 | `ci13lc.h` | Lua `source_file.prj` + Make | Voice + BLE broadcast/combo examples |

## Routing Rules

- Existing project SDK version wins. Do not migrate SDKs unless the user asks.
- For a new project, choose the newest SDK that explicitly supports the exact chip, board, voice model flow, and connectivity requirements.
- CI1312 routes through CI13LC unless a selected unified CI13XX SDK proves otherwise.
- CI230X is ARM/LN882H based; do not reuse RISC-V startup, interrupt, or toolchain assumptions.
- ASR assets, voice prompts, flash packing, `user_config.h`, and `source_file.prj` are part of the implementation, not optional extras.
