[简体中文](README.md) | [English](README_en.md)

# chipintelli-dev-skill

A unified AI skill for the Chipintelli voice MCU ecosystem, covering 7 major chip families and 20+ SDK versions.
All recipes, API references, pitfalls, and sample projects are grounded in local `docs/` documentation (1,128 pages from document.chipintelli.com) and actual SDK source code.

Supported chips: CI1102/CI1103, CI1122, CI1301/CI1302/CI1303/CI1306, CI1311/CI1312/CI13161/CI13162/CI13241/CI13242/CI13322, CI2305/CI2306, CI2312/CI23242.

## Features

- Based on 20+ Chipintelli SDK real source code and 1128 pages of official documentation
- Covered chip series: CI110X (1st generation), CI112X (1st generation main control), CI130X (2nd generation), CI13LC (3rd generation low cost), CI13XX (3rd generation unified), CI230X (Wi-Fi+BLE), CI23LC (3rd generation+BLE)
- Covers core functions: offline ASR, wakeup + command words, CWSL self-learning, multi-intent recognition, natural speaking, dual microphone beams, sound source localization, acoustic event detection, LLM AIoT, VPR voiceprint
- Covered audio algorithms: AEC echo cancellation, noise reduction, beamforming, DOA, dereverberation, AGC/DRC/EQ, deep noise reduction
- Coverage connection capabilities: BLE voice, BLE broadcast, Bluetooth applet, Wi-Fi+voice, infrared control, serial port protocol, OTA
- Covers peripheral drivers: GPIO/UART/I2C/IIS/ADC/PWM/Timer/DMA/SPIFlash/Watchdog/Low Power Consumption/Flash Control/Codec
- Each chip family includes recipes (scenario guides), API quick references, memory layouts, pitfalls, and example indexes
- RISC-V (Nuclei N300) + ARM Cortex-M4 (LN882H) dual architecture support
- Build toolchains: riscv-nuclei-elf-gcc 9.2.0 + Make/CMake + GCC-ARM

## Chip series list

| Series | Representative Chip | Architecture | Core Features | Typical Applications |
|---|---|---|---|---|
| CI110X | CI1102, CI1103 | RISC-V Nuclei N201 | 1st generation offline speech recognition | Basic voice control |
| CI112X | CI1122 | RISC-V Nuclei N201 | 1st generation main control MCU | USB Dongle, air conditioning remote control |
| CI130X | CI1302, CI1306 | RISC-V Nuclei N300 | 2nd generation offline ASR, dual core | Voice module, light control, fan |
| CI13LC | CI1312, CI13242, CI13322 | RISC-V Nuclei N300 | 3rd generation, low cost, single/dual microphone | Ceiling lights, heaters, tea dispensers |
| CI13XX | Unified CI130X + CI13LC | RISC-V Nuclei N300 | 3rd generation unified SDK | General development across the product range |
| CI230X | CI2305, CI2306 | ARM Cortex-M4 (LN882H) | Wi-Fi + BLE + Voice | Offline Voice AIoT |
| CI23LC | CI2312, CI23242 | RISC-V Nuclei N300 | 3rd generation + BLE | Voice + Bluetooth applet |

## Installation instructions

### 1. Copy the skill to the Hermes skill directory

```bash
# Hermes Agent (Windows)
cp -r D:/启英泰伦/chipintelli-dev-skill ~/AppData/Local/hermes/skills/

# Claude Code: ~/.claude/skills 或 .claude/skills
# OpenCode: ~/.config/opencode/skills 或 .opencode/skills
```

### 2. Verify loading

```bash
# Hermes Agent
skill_view(name='chipintelli-dev-skill')
# 预期: readiness_status: available
```

## Working principle

| Steps | Name | Description |
|------|------|------|
| 1 | Locate the chip | Determine the target chip model and navigate to `chips/<family>/` |
| 2 | Select SDK | Select the appropriate SDK version according to the chip and application scenario |
| 3 | Read a recipe | Find a matching scenario in `chips/<family>/recipes/*.md` and implement it step by step |
| 4 | Check APIs | Consult `chips/<family>/resources/api_reference.md` or verify the actual header files |
| 5 | Verification | Verify `user_config.h` configuration, initialization sequence, pin assignment, model file |
| 6 | Confirm | Give the user the implementation plan (configuration, pins, entry, build command) |
| 7 | Execute | Copy the latest example project and modify `user_config.h` + `user_msg_deal.c` |
| 8 | Build | RISC-V: `make -j` / ARM: `cmake --build` |
| 9 | Programming | `ci-tool-kit.exe` (CI13xx) / `JFlash.exe` (LN882H) |
| 10 | Debugging | UART Log + OpenOCD/GDB |

## General principles

- **Dual-core architecture**: CI130X/CI13XX has a host core (N300) and a nuclear core (DSP), communicating through `nuclear_com`/`mailbox`
- **SCU clock gating**: Enable the clock with `scu_set_device_gate()` before using peripherals
- **OS dependent driver**: QSPIFlash/DMA/I2C/SPI driver uses FreeRTOS API and needs to be initialized after RTOS starts
- **`user_config.h` unified configuration**: board level, chip model, microphone mode, serial port, ASR, player, and algorithm are all in this file
- **`source_file.prj` Control Compilation**: Add source files in this file, not Makefile
- **Grounding First**: All content is based on real SDK source code and official documents, no fabricated APIs
- **Model file matching**: The ASR model must match the chip model and is generated on the speech AI platform

## Local resources

- Document Center: `docs/` (1128 pages, source document.chipintelli.com)
- Compiler: `riscv-nuclei-elf-gcc-9.2.0/` (GCC 9.2.0 + OpenOCD)
- SDK collection: 20+ SDK directories, covering all chip series and application scenarios
- Chip index: `resources/chip_index.md`
- SDK Index: `resources/sdk_index.md`
- Recipe summary: `resources/recipe_index.md`
- Glossary: `resources/glossary.md`
- Chipintelli official website: https://www.chipintelli.com
- Voice AI platform: https://aiplatform.chipintelli.com

## License

SDK content remains copyrighted by Chipintelli. This aggregate skill's metadata is licensed under MIT.
