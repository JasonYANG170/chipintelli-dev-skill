---
name: chipintelli-dev-skill
description: >-
  Use when developing, configuring, building, debugging, or reviewing firmware for
  Chipintelli (启英泰伦) CI110X, CI112X, CI130X, CI13LC, CI13XX, CI230X, or CI23LC
  voice MCUs. Routes work to source-grounded SDK recipes and API references for ASR,
  audio algorithms, BLE/Wi-Fi combinations, IR control, OTA, and peripheral drivers.
version: 1.1.0
author: JasonYANG170
license: MIT
platforms: [windows]
metadata:
  hermes:
    tags: [embedded, chipintelli, riscv, voice, asr, ble, wifi, firmware]
    related_skills: [systematic-debugging, test-driven-development, requesting-code-review]
---

# chipintelli-dev-skill

启英泰伦 (Chipintelli) 语音 MCU 全生态统一开发技能。覆盖 7 大芯片系列、20+ SDK 版本，
所有 recipe、API 参考、陷阱与示例工程均基于本地 `docs/` 文档与真实 SDK 源码接地。

## Directory Structure

```
chipintelli-dev-skill/
  SKILL.md                         # This file - unified routing and guidance
  AGENTS.md                        # Code conventions, build workflow, checklists
  README.md                        # Installation and usage guide
  chips/
    ci110x/                        # CI1102/CI1103 (1st gen, RISC-V Nuclei)
      recipes/                     # Scenario guides
      resources/                   # API ref, pitfalls, memory layout, examples
    ci112x/                        # CI1122 (1st gen host, RISC-V Nuclei)
      recipes/
      resources/
    ci130x/                        # CI1301/CI1302/CI1303/CI1306 (2nd gen, RISC-V Nuclei)
      recipes/
      resources/
    ci13lc/                        # CI1311/CI1312/CI1316x/CI1324x/CI1332x (3rd gen LC, RISC-V)
      recipes/
      resources/
    ci13xx/                        # CI13XX unified SDK (3rd gen, covers CI130x+CI13LC)
      recipes/
      resources/
    ci230x/                        # CI2305/CI2306 (Wi-Fi+BLE combo, ARM Cortex-M4 LN882H)
      recipes/
      resources/
    ci23lc/                        # CI23LC BLE SDK (3rd gen + BLE, RISC-V)
      recipes/
      resources/
  resources/
    chip_index.md                  # Chip family quick reference
    sdk_index.md                   # All SDK versions mapped to chip families
    recipe_index.md                # Cross-chip recipe total index
    glossary.md                    # Terms (ASR, AEC, BF, DOA, CWSL, VPR, etc.)
```

## Chip Family Routing

**Step 1: Identify the target chip. Step 2: Navigate to `chips/<family>/`. Step 3: Read the recipe, then copy the example.**

| Chip Family | Directory | Architecture | Core | Header | Build Tool | Key Features |
|---|---|---|---|---|---|---|
| CI110X (CI1102/CI1103) | `chips/ci110x/` | RISC-V (Nuclei N201) | rv32imafc | `ci110x_system.h` | riscv-nuclei-elf-gcc + Make | 1st gen offline ASR, codec, IIS, UART |
| CI112X (CI1122) | `chips/ci112x/` | RISC-V (Nuclei N201) | rv32imafc | `ci112x_system.h` | riscv-nuclei-elf-gcc + Make | 1st gen host MCU, USB, more flash |
| CI130X (CI1301/CI1302/CI1303/CI1306) | `chips/ci130x/` | RISC-V (Nuclei N300) | rv32imafc | `ci130x_system.h` | riscv-nuclei-elf-gcc + Make | 2nd gen offline ASR, dual-core, DPMU |
| CI13LC (CI1311/CI1312/CI1316x/CI1324x/CI1332x) | `chips/ci13lc/` | RISC-V (Nuclei N300) | rv32imafc | `ci13lc.h` / `ci1316x.h` / `ci1324x.h` / `ci1332x.h` | riscv-nuclei-elf-gcc + Make | 3rd gen low-cost, single/dual mic, CWSL, NN enc |
| CI13XX (unified) | `chips/ci13xx/` | RISC-V (Nuclei N300) | rv32imafc | `ci13xx_system.h` | riscv-nuclei-elf-gcc + Make | 3rd gen unified SDK (CI130x + CI13LC combined) |
| CI230X (CI2305/CI2306) | `chips/ci230x/` | ARM Cortex-M4 | LN882H | `ln882h.h` | CMake + GCC-ARM / Keil | Wi-Fi + BLE combo, online+offline voice |
| CI23LC (CI2312/CI23242) | `chips/ci23lc/` | RISC-V (Nuclei N300) | rv32imafc | `ci13lc.h` | riscv-nuclei-elf-gcc + Make | 3rd gen + BLE, voice + BLE broadcast |

---

## SDK Version Routing

The local SDK collection contains 20+ SDK versions. See `resources/sdk_index.md` for the complete mapping and selection decision tree. **Rule:** Always use the latest SDK version available for your chip.

---

## Universal Core Principles

These apply to **all** Chipintelli chip families:

1. **Never guess APIs** -- Check `chips/<family>/resources/api_reference.md` first; no matching doc = does not exist. Cross-reference with `docs/软件开发/` documentation.
2. **Dual-core architecture (CI130X/CI13XX)** -- These chips have a host core (N300) and a nuclear core (DSP). Communication via `nuclear_com` / `mailbox`. ASR runs on nuclear core; user code runs on host core.
3. **SCU clock gate before peripheral use** -- Call `scu_set_device_gate()` to enable clock, then `scu_set_device_reset()` / `scu_set_device_reset_release()` to reset peripheral before init.
4. **Board configuration via `user_config.h`** -- Select board type (`USE_CI_D06GT01D_BOARD`), chip type (`CI_CHIP_TYPE`), mic mode (`MIC_DIFF_SINGLE`), UART config, ASR config, player config, algorithm config all in one file.
5. **`source_file.prj` controls build** -- The Lua-based build system reads `source_file.prj` to generate Makefile. Add source files there, not in Makefile directly.
6. **FreeRTOS is the RTOS** -- All SDKs use FreeRTOS. Tasks created with `xTaskCreate()`. Semaphores, queues, timers all from FreeRTOS.
7. **ASR message flow** -- ASR results arrive via `sys_msg_queue` -> `sys_msg_deal_task` -> `user_msg_deal()`. Register callbacks in `user_msg_deal.c`.
8. **Flash layout is chip-dependent** -- Different chips have different flash sizes (2MB/4MB). The firmware pack tool (`PACK_UPDATE_TOOL.exe`) creates the final image with ASR model + voice files + user code.
9. **Copy closest example, then modify** -- Never write from scratch; copy the recommended sample from `projects/` and adapt `user_config.h` + `user_msg_deal.c`.
10. **`ci-tool` VS Code extension** -- Install `ci-tool-1.1.2.vsix` from `tools/` for project selection, build, and debug integration.

---

## Voice Recognition Specific Principles

1. **ASR model files** -- Located in `projects/<sample>/firmware/asr/` and `firmware/dnn/`. Generated by the voice AI platform (https://aiplatform.chipintelli.com).
2. **Voice prompt files** -- Located in `projects/<sample>/firmware/voice/`. MP3 or prompt format.
3. **Wakeup + command word model** -- `USE_SEPARATE_WAKEUP_EN=1` uses separate wakeup model. `DEFAULT_MODEL_GROUP_ID` selects startup model (0=command, 1=wakeup).
4. **Exit wakeup timeout** -- `EXIT_WAKEUP_TIME` (ms) controls how long to stay in command mode after wakeup before returning to wakeup-only mode.
5. **Confidence tuning** -- `DEFAULT_CONFIDENCE`, `DEFAULT_CNT`, `ADAPTIVE_THRESHOLD` adjust recognition sensitivity.
6. **Audio algorithm pipeline** -- AEC -> Denoise -> Beamforming -> DOA -> ASR. Enable via `USE_AEC_MODULE`, `USE_DENOISE_MODULE`, `USE_ALC_AUTO_SWITCH_MODULE` in `user_config.h`.
7. **Player initialization** -- `audio_play_init()` creates the player task. Decoders (MP3/prompt/ADPCM/M4A/FLAC) enabled via `USE_PROMPT_DECODER`, `USE_MP3_DECODER` etc.
8. **Serial protocol** -- `MSG_COM_USE_UART_EN=1` enables the voice module UART protocol. Protocol version via `UART_PROTOCOL_VER` (1=legacy, 2=current, 255=platform-generated).

---

## BLE-Specific Principles (CI23LC / CI1302 BLE)

1. **BLE SDK structure** -- `app_ble/` contains BLE application logic; `components/ci_ble/` contains BLE stack.
2. **BLE message handling** -- `ble_adv_msg_deal.c` handles BLE advertising and connection events. Application demos in `app_ble/demo/` (fan, heater, air conditioner, RGB, etc.).
3. **BLE + ASR combo** -- The BLE SDK runs ASR and BLE concurrently. BLE messages and ASR messages both route through `sys_msg_queue`.
4. **`cias_demo_config.h`** -- BLE demo configuration: select which demo (fan/heater/AC/RGB/tea table/water heater) to activate.

---

## Wi-Fi Combo Principles (CI230X)

1. **Dual chip architecture** -- CI230X = CI13xx (voice, RISC-V) + LN882H (Wi-Fi/BLE, ARM Cortex-M4). Communication via SDIO.
2. **CMake build system** -- CI230X SDK uses CMake (not Make). Build: `cmake -B build-ci230x-wifi-sdk-combo-release && cmake --build build-ci230x-wifi-sdk-combo-release`.
3. **Flash partition** -- Defined in `cfg/flash_partition_cfg.json` and `flash_partition_table.h`. Contains boot + app + OTA + voice model partitions.
4. **OTA** -- `components/fota/` handles firmware-over-the-air. Audio-side OTA agent in `doc/audio端ota升级代理程序`.
5. **JFlash tool** -- Use `tools/JFlash/JFlash.exe` with `ln882h.jflash` config for LN882H flashing.

---

## Scenario Quick Reference

See `resources/recipe_index.md` for the complete cross-chip recipe routing table with `.md` extensions. Below is a summary of available recipes:

### Project Setup

Every chip family has a `recipes/new_project.md`. See the chip family routing table above.

### Voice Recognition

| Scenario | Recipe Path |
|---|---|
| **Offline ASR** | `chips/ci130x/recipes/offline_asr.md` (also ci13lc) |
| **Wakeup + command** | `chips/ci130x/recipes/offline_asr.md` (covers wakeup config) |
| **CWSL (self-learning)** | `chips/ci13lc/recipes/cwsl.md` (also ci23lc) |
| **Multi-intent ASR** | `chips/ci13lc/recipes/multi_intents.md` |
| **Natural language (NL) ASR** | `chips/ci13lc/recipes/nl_asr.md` |
| **Two-mic beamforming** | `chips/ci130x/recipes/two_mic.md` |
| **Sound event detection** | `chips/ci13xx/recipes/sound_event.md` |
| **Offline ASR (CI13XX)** | `chips/ci13xx/recipes/offline_asr.md` |
| **LLM AIoT** | `chips/ci13xx/recipes/llm_aiot.md` |
| **VPR (voiceprint)** | `chips/ci13xx/recipes/vpr.md` |

### Audio Algorithms

| Scenario | Recipe Path |
|---|---|
| **AEC (echo cancel)** | `chips/ci130x/recipes/aec.md` |
| **Beamforming / DOA** | `chips/ci130x/recipes/two_mic.md` |
| **Other algorithms** | See `chips/ci130x/resources/api_reference.md` for denoise, dereverb, AGC, DRC, EQ headers |

### Connectivity

| Scenario | Recipe Path |
|---|---|
| **BLE voice combo** | `chips/ci23lc/recipes/ble_voice.md` |
| **BLE broadcast** | `chips/ci23lc/recipes/ble_broadcast.md` |
| **BLE + mini-program** | `chips/ci23lc/recipes/ble_miniprogram.md` |
| **Wi-Fi + voice** | `chips/ci230x/recipes/wifi_voice.md` |
| **IR control** | `chips/ci130x/recipes/ir_control.md` (also ci13lc, ci13xx) |
| **UART communication** | `chips/ci130x/recipes/uart_comm.md` |
| **OTA** | `chips/ci130x/recipes/ota.md` (also ci13lc, ci13xx, ci230x) |

### Peripherals

For GPIO, UART, I2C, IIS, ADC, and DMA, see `chips/<family>/resources/api_reference.md` for complete function signatures. Dedicated recipes available:

| Scenario | Recipe Path |
|---|---|
| **GPIO** | `chips/ci130x/recipes/gpio_control.md` |
| **UART** | `chips/ci130x/recipes/uart_comm.md` |
| **PWM** | `chips/ci130x/recipes/pwm_output.md` |
| **Timer** | `chips/ci130x/recipes/timer.md` |
| **SPI Flash** | `chips/ci130x/recipes/spiflash.md` |
| **Watchdog** | `chips/ci130x/recipes/watchdog.md` |
| **Flash control** | `chips/ci130x/recipes/flash_control.md` |
| **Low power** | `chips/ci130x/recipes/low_power.md` |
| **Codec (audio)** | `chips/ci130x/recipes/codec.md` |
| **Audio player** | `chips/ci130x/recipes/audio_player.md` |
| **Audio player (CI13LC)** | `chips/ci13lc/recipes/audio_player.md` |

### TTS / VPR

| Scenario | Recipe Path |
|---|---|
| **TTS** | `chips/ci13xx/recipes/tts.md` |
| **VPR** | `chips/ci13xx/recipes/vpr.md` |

---

## Critical Pitfalls

### RISC-V Chips (CI110X/CI112X/CI130X/CI13LC/CI13XX/CI23LC)

| # | Pitfall | Fix |
|---|---------|-----|
| 1 | SCU clock gate not enabled | Call `scu_set_device_gate(DEVICE_BASE, ENABLE)` before peripheral init |
| 2 | Device reset not performed | Call `scu_set_device_reset()` then `scu_set_device_reset_release()` after clock gate |
| 3 | OS-dependent driver called before RTOS start | QSPIFlash, DMA, I2C, SPI drivers use FreeRTOS API. Init them in `vTaskVariablesInit`, NOT before scheduler start. If pre-RTOS init needed: set `CONFIG_DIRVER_BUF_USED_FREEHEAP_EN=0` and `DRIVER_OS_API=0` |
| 4 | Wrong chip type in `user_config.h` | `CI_CHIP_TYPE` must match actual chip (1302/1306/1312/13242/13322 etc.) |
| 5 | Board file mismatch | `BOARD_PORT_FILE` must match the physical board. Wrong board file = wrong pin config |
| 6 | Mic mode mismatch | `MIC_DIFF_SINGLE` must match hardware: 0=differential, 1=single-end (MICN_L to GND) |
| 7 | Log UART and protocol UART conflict | `CONFIG_CI_LOG_UART` and `UART_PROTOCOL_NUMBER` must use different UART ports |
| 8 | PLL frequency mismatch | `get_ipcore_clk()` must match `MAIN_FREQUENCY` within 10MHz. If not, PLL config error |
| 9 | Flash write without erase | SPIFlash erases in 4KB sectors. Always erase before write |
| 10 | `source_file.prj` not updated | Adding a .c file to the project requires adding it to `source_file.prj`, not Makefile |
| 11 | LTO enabled but debug needed | Set `RELEASE=TRUE` for release (LTO on), omit for debug (LTO on but with -g). For no LTO: remove `LTO_OPTION = -flto` |
| 12 | Dual-core sync missed | `mailboxboot_sync()` must be called after `nuclear_com_init()` to sync host and nuclear cores |
| 13 | ASR model not matching chip | ASR model in `firmware/asr/` must be generated for the correct chip type |
| 14 | Heap size insufficient | `SYS_HEAP_SIZE` in linker script (.lds) must accommodate all tasks + buffers. Check with `xPortGetFreeHeapSize()` |
| 15 | Watchdog not fed | IWDG initialized in `platform_init()`. Long tasks must yield or feed watchdog |

### ARM Cortex-M4 (CI230X / LN882H)

| # | Pitfall | Fix |
|---|---------|-----|
| 1 | Wrong toolchain | LN882H uses ARM GCC (arm-none-eabi-gcc), NOT riscv-nuclei-elf-gcc |
| 2 | CMake build directory | Must build in separate directory: `cmake -B build-ci230x-wifi-sdk-combo-release` |
| 3 | Flash partition mismatch | `flash_partition_cfg.json` must match `flash_partition_table.h`. Changing one requires updating both |
| 4 | SDIO communication failure | CI13xx <-> LN882H via SDIO. Check SDIO pins and clock config |
| 5 | JFlash configuration | Use `tools/JFlash/ln882h.jflash`, not generic config |

---

## API References and Guides

Each chip family has documentation under `chips/<family>/resources/`:

| Document | Description | Available For |
|---|---|---|
| `api_reference.md` | Complete peripheral API function signatures | CI130X, CI13LC, CI230X, CI23LC |
| `pitfalls.md` | Detailed error scenarios and fixes | CI130X, CI13LC, CI230X, CI23LC |
| `memory_layout.md` | Flash/RAM layout, linker script reference | CI130X, CI13LC, CI230X |
| `example_list.md` | Index of all example projects | All families |
| `build_system.md` | Make/Lua build system walkthrough | CI130X |

---

## Execution Workflow

| Step | Name | Description |
|------|------|-------------|
| 1 | **Identify chip** | Determine the exact chip variant (e.g., CI1306, CI13242, CI23242). Navigate to `chips/<family>/`. |
| 2 | **Select SDK** | Choose the appropriate SDK version from `resources/sdk_index.md`. Prefer latest. |
| 3 | **Read recipe** | Find the matching scenario in `chips/<family>/recipes/`. Read it first. |
| 4 | **Check API** | For APIs not in recipes, check `chips/<family>/resources/api_reference.md`. |
| 5 | **Check pitfalls** | Review `chips/<family>/resources/pitfalls.md` before coding. |
| 6 | **Validate** | Verify all API signatures, header includes, pin assignments, `user_config.h` settings. |
| 7 | **Confirm** | Present plan: includes, pin config, init sequence, `user_config.h` changes, build commands. |
| 8 | **Execute** | **New project:** copy closest sample from `projects/`, then modify `user_config.h` + `user_msg_deal.c`. **Existing:** edit in place. |
| 9 | **Build** | RISC-V: `make -j` in `project_file/` dir with `PATH` including `tools/build-tools/bin` + compiler path. ARM: `cmake --build`. |
| 10 | **Flash** | Use `ci-tool-kit.exe` or `code_program.exe` for CI13xx. Use `JFlash.exe` for LN882H. |
| 11 | **Debug** | UART log output (`CONFIG_CI_LOG_UART`). OpenOCD + GDB for JTAG debug. `SEGGER_SYSVIEW` for RTOS analysis. |

---

## Build Commands

### RISC-V Chips (CI110X/CI112X/CI130X/CI13LC/CI13XX/CI23LC)

```bash
# Set up environment
export PATH="$SDK_ROOT/tools/build-tools/bin:$GCC_ROOT/gcc_fix_raissrc/bin:$PATH"
export SDK_PATH="$SDK_ROOT"

# Build (in project_file/ directory)
cd projects/<sample>/project_file
make -j4

# Clean
make clean
```

Compiler: `riscv-nuclei-elf-gcc` (GCC 9.2.0, rv32imafc, ilp32f)
Linker: `riscv-nuclei-elf-g++`
OpenOCD: `openocd.exe` from `riscv-nuclei-elf-gcc-9.2.0/openocd/bin/`

### ARM Cortex-M4 (CI230X / LN882H)

```bash
# CMake build
cd CI230X_wifi_combo_sdk_release_v1.1.1
cmake -B build-ci230x-wifi-sdk-combo-release
cmake --build build-ci230x-wifi-sdk-combo-release

# Or use Keil: open .uvprojx, Build (F7), Flash (F8)
```

---

## Failure Strategies

| Situation | Action |
|---|---|
| API does not exist in resources | Stop immediately, inform user. Check `docs/软件开发/` for documentation. |
| Chip family unclear | Ask user for exact chip part number |
| SDK version unclear | Default to latest version for the chip family |
| Pin conflict detected | Check board file in `driver/boards/`. Suggest alternate board config or custom board file. |
| ASR not recognizing | Check model files in `firmware/asr/`, verify `DEFAULT_MODEL_GROUP_ID`, check mic config |
| Audio not playing | Check `AUDIO_PLAYER_ENABLE`, decoder flags, voice files in `firmware/voice/`, PA control |
| Flash write fails | Verify flash size matches chip, check erase before write, check `flash_control` init |
| Build fails | Check `source_file.prj` is valid, check `SDK_PATH` env, check compiler in PATH |
| Dual-core sync failure | Verify `mailboxboot_sync()` called, check `nuclear_com_init()` |
| BLE not advertising | Check `ci_ble` component, verify BLE stack init, check `ble_adv_msg_deal.c` |
| Wi-Fi not connecting | Check `components/wifi/`, verify SDIO communication, check partition table |
