# Chip Family Quick Reference

## Architecture Overview

| Family | Chips | Core | Arch | Flash | SRAM | Key Features |
|---|---|---|---|---|---|---|
| CI110X | CI1102, CI1103 | Nuclei N201 | rv32imafc | 2MB | ~512KB | 1st gen offline ASR |
| CI112X | CI1122 | Nuclei N201 | rv32imafc | 2MB | ~512KB | 1st gen host MCU, USB |
| CI130X | CI1301, CI1302, CI1303, CI1306 | Nuclei N300 | rv32imafc | 2-4MB | ~512KB | 2nd gen dual-core ASR |
| CI13LC | CI1311, CI1312, CI13161, CI13162, CI13241, CI13242, CI13320, CI13321, CI13322, CI13642 | Nuclei N300 | rv32imafc | 2-4MB | ~512KB | 3rd gen low-cost, CWSL, NL ASR |
| CI13XX | CI130X + CI13LC unified | Nuclei N300 | rv32imafc | 2-4MB | ~512KB | 3rd gen unified SDK |
| CI230X | CI2305, CI2306 | Cortex-M4 (LN882H) + Nuclei N300 | ARM + RISC-V | 4MB+ | ~512KB | Wi-Fi + BLE + voice combo |
| CI23LC | CI2312, CI23242 | Nuclei N300 | rv32imafc | 2-4MB | ~512KB | 3rd gen + BLE |

## Chip Selection Guide

### By Application

| Application | Recommended Chip | Recommended SDK |
|---|---|---|
| Basic offline voice control | CI1302, CI13242 | CI130X_SDK_Offline or CI13LC_SDK_V2.0.15 |
| Voice control + IR | CI1306, CI13242 | CI130X_SDK_Offline_IR or CI13LC_SDK_IR |
| Dual-mic beamforming | CI1306 | CI130X_SDK_TwoMic |
| Advanced algorithm (AEC/BF/DOA) | CI1306 | CI130X_SDK_ALG or CI13XX_SDK_ASR_ALG |
| Command word self-learning | CI13242, CI13322 | CI13LC_SDK_V2.0.15 (cwsl_sample) |
| Natural language ASR | CI13242, CI13322 | CI13LC_SDK_V2.0.15 (nl_asr_sample) |
| LLM AIoT (online+offline) | CI1306, CI13322 | CI130X_SDK_LLM_AIOT or CI13XX_SDK_LLM_AIOT |
| Voice + BLE | CI23242 | CI23LC_SDK_BLE_V1.3.13 |
| Voice + BLE + mini-program | CI23242 | CI23LC_SDK_BLE_V1.3.13 |
| Voice + Wi-Fi | CI2305, CI2306 | CI230X_wifi_combo_sdk |
| Sound event detection | CI13322 | CI13XX_SDK_ASR_ALG |
| Voiceprint recognition | CI1306 | CI13XX_SDK_LLM_AIOT |
| Deep denoise + ASR | CI1303 | CI130X_SDK_ALG_PRO |
| TTS | CI1306 | CI13XX_SDK_LLM_AIOT |

### By Pin Count

| Chip | Package | Flash | Notes |
|---|---|---|---|
| CI1302 | SSOP24 | 2MB | Minimal pins, basic ASR |
| CI1306 | QFN40 | 4MB | Full features, dual-mic |
| CI1312 | SSOP16 | 2MB | Ultra-low-cost |
| CI13242 | QFN32 | 4MB | 3rd gen, CWSL |
| CI13322 | QFN56 | 4MB | Max pins, full features |
| CI23242 | QFN32 | 4MB | BLE + voice |
| CI2305 | - | - | Wi-Fi combo |
| CI2306 | - | - | Wi-Fi combo (enhanced) |

## Module (Board) Selection

| Module | Chip | Flash | Form Factor | Use Case |
|---|---|---|---|---|
| CI-D02GS01J | CI1302 | 2MB | 端子模块 | Basic voice module |
| CI-D02GS02S | CI1302 | 2MB | SMT module | Basic voice module (SMT) |
| CI-D06GT01D | CI1306 | 4MB | 开发板 | Development board |
| CI-D06GT01J | CI1306 | 4MB | Module | Full-featured module |
| CI-D0XGS07J-BT | CI1306 | - | Module | With Bluetooth |
| CI-F16XGS02J | CI13242 | - | Module | 3rd gen module |
| CI-F24XGS01J | CI13242 | - | Module | 3rd gen module |
| CI-F24XGS01S | CI13242 | - | SMT module | 3rd gen SMT |
| CI-F322GS01S | CI13322 | - | SMT module | Full-pin SMT |
| CI-E0XGT02S | CI2305/CI2306 | - | Module | Wi-Fi combo module |
| CI-E0XGT03S | CI2305/CI2306 | - | Module | Wi-Fi combo module (enhanced) |
| CI-G16XGS02J | CI2312 | - | Module | BLE module |
| CI-G24XGS02J | CI23242 | - | Module | BLE + voice module |

## Dual-Core Architecture (CI130X/CI13XX)

```
+------------------+     +------------------+
|   Host Core      |     |  Nuclear Core    |
|   (Nuclei N300)  |     |  (DSP)           |
|                  |     |                  |
|  - FreeRTOS      |<--->|  - ASR engine    |
|  - User code     | mb |  - VAD            |
|  - Player        |    |  - Algorithm     |
|  - BLE/Wi-Fi     |    |  - DNN           |
|  - UART/SPI/I2C  |     |                  |
+------------------+     +------------------+
         |
    +----+----+
    | SPIFlash|
    | (shared)|
    +---------+
```

Communication: `nuclear_com_init()` -> `mailboxboot_sync()` -> `REMOTE_CALL()` for cross-core calls.

## Toolchain Reference

| Toolchain | Path | Used By |
|---|---|---|
| riscv-nuclei-elf-gcc 9.2.0 | `riscv-nuclei-elf-gcc-9.2.0/gcc_fix_raissrc/bin/` | CI110X, CI112X, CI130X, CI13LC, CI13XX, CI23LC |
| OpenOCD (Nuclei) | `riscv-nuclei-elf-gcc-9.2.0/openocd/bin/` | RISC-V debug |
| GCC-ARM | (system-installed) | CI230X (LN882H) |
| Keil MDK | (system-installed) | CI230X (LN882H, optional) |
| ci-tool VS Code | `tools/ci-tool-1.1.2.vsix` | Project management, build |
| ci-tool-kit | `tools/ci-tool-kit.exe` | Flashing CI13xx |
| PACK_UPDATE_TOOL | `tools/PACK_UPDATE_TOOL.exe` | Firmware packing |
| JFlash | `CI230X.../tools/JFlash/JFlash.exe` | Flashing LN882H |
