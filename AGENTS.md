# AGENTS.md - Supplementary Agent Guide

> Core rules, mapping table, pitfalls, recipes index, and execution workflow are all in `SKILL.md`.
> This file covers **only** conventions and tooling guidance not present in `SKILL.md`. Do not duplicate content.

## Project Context

**Language**: C -- **Target**: Chipintelli voice MCU (RISC-V Nuclei / ARM Cortex-M4) -- **Toolchain**: riscv-nuclei-elf-gcc 9.2.0 + Make (RISC-V) or CMake + GCC-ARM (CI230X)

## Architecture Quick Reference

| Architecture | Chips | Header | Build Tool | Linker Script |
|---|---|---|---|---|
| RISC-V (Nuclei N201) | CI1102, CI1103, CI1122 | `ci110x_system.h`, `ci112x_system.h` | riscv-nuclei-elf-gcc + Make | `*.lds` (INCLUDE common.lds) |
| RISC-V (Nuclei N300) | CI1301/CI1302/CI1303/CI1306 | `ci130x_system.h` | riscv-nuclei-elf-gcc + Make | `ci130x.lds` (INCLUDE common.lds) |
| RISC-V (Nuclei N300) | CI1311/CI1312/CI1316x/CI1324x/CI1332x | `ci13lc.h`, `ci1316x.h`, `ci1324x.h`, `ci1332x.h` | riscv-nuclei-elf-gcc + Make | `ci13lc.lds` |
| RISC-V (Nuclei N300) | CI13XX unified | `ci13xx_system.h` | riscv-nuclei-elf-gcc + Make | `ci13xx.lds` |
| ARM Cortex-M4 | CI2305, CI2306 (LN882H) | `ln882h.h` | CMake + GCC-ARM / Keil | `ln882h.ld` / `ln882h.sct` |
| RISC-V (Nuclei N300) | CI2312, CI23242 (BLE) | `ci13lc.h` | riscv-nuclei-elf-gcc + Make | `ci13lc.lds` |

## Compiler Flags (RISC-V)

```
-march=rv32imafc -mabi=ilp32f -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mfdiv
-Os -fshort-enums -fsigned-char -ffunction-sections -fdata-sections -fno-common
-fno-delete-null-pointer-checks -fno-unroll-loops -std=gnu11
```

Key defines:
- `CI_CONFIG_FILE="user_config.h"` -- user configuration file
- `CORE_ID=0` -- host core identifier
- `PROJECT_NAME="<name>"` -- project name (auto from directory or `PROJECT_NAME` env)
- `ASR_CODE_VERSION=2` -- ASR algorithm version

## Code Generation Conventions

### File Naming
- Driver source: `ci<chip>_<peripheral>.c` (e.g., `ci130x_gpio.c`, `ci13lc_uart.c`)
- Driver header: `ci<chip>_<peripheral>.h`
- Board files: `CI-<module>.c` (e.g., `CI-D06GT01D.c`, `CI-D02GS01J.c`)
- User code: `user_config.h`, `user_msg_deal.c`, `user_msg_deal.h`, `system_msg_deal.c`
- ASR config: `ci_ssp_config.c` (voice front-end signal processing config)
- Linker script: `ci<chip>.lds` (includes `common.lds`)

### Include Patterns

**CI130X projects:**
```c
#include <stdio.h>
#include <malloc.h>
#include "FreeRTOS.h"
#include "task.h"
#include "sdk_default_config.h"    // pulls in user_config.h via CI_CONFIG_FILE
#include "ci130x_core_eclic.h"
#include "ci130x_spiflash.h"
#include "ci130x_gpio.h"
#include "ci130x_uart.h"
#include "ci130x_dpmu.h"
#include "ci130x_mailbox.h"
#include "board.h"
#include "audio_play_api.h"
#include "asr_api.h"
#include "ci_log.h"
#include "status_share.h"
#include "system_msg_deal.h"
```

**CI13LC projects:**
```c
#include "sdk_default_config.h"
#include "ci13lc.h"                // or ci1316x.h, ci1324x.h, ci1332x.h
#include "ci13lc_gpio.h"
#include "ci13lc_uart.h"
#include "board.h"
#include "asr_api.h"
```

**CI230X (LN882H) projects:**
```c
#include "ln882h.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "wifi_api.h"
// ARM CMSIS headers
#include "core_cm4.h"
```

### Standard Project Structure (CI130X/CI13LC/CI13XX)
```
SDK_ROOT/
  ci130x_sdk/                     # or ci13lc_sdk, ci13xx_sdk
    .vscode/
      settings.json               # ci-tool config: activated_project, compiler path
      tasks.json                  # VS Code build tasks (clean, compile)
    components/                   # SDK components (alg, asr, player, freertos, ...)
    driver/
      ci130x_chip_driver/         # chip driver (inc/ + src/)
      boards/                     # board configurations (CI-D06GT01D.c, etc.)
      third_device_driver/        # external sensor drivers
    libs/                         # pre-compiled libraries (.a files)
    startup/                      # ci130x_init.c, ci130x_startup.S, ci130x_vtable.S
    system/                       # ci130x_system.c, ci130x_it.c, platform_config.c
    utils/                        # common_head.mk, common_flags.mk, common_tail.mk, common.lds
    tools/                        # ci-tool-kit, PACK_UPDATE_TOOL, generate_makefile.lua
    projects/
      <sample_name>/
        firmware/
          asr/                    # ASR model files
          dnn/                    # DNN model files
          user_file/              # user data files
          voice/                  # voice prompt files (MP3/prompt)
        project_file/
          Makefile                # project Makefile (includes common_head.mk)
          source_file.prj         # source file list (parsed by generate_makefile.lua)
        src/
          main.c                  # main entry point
          ci130x.lds              # project linker script
          user_config.h           # user configuration (board, chip, mic, UART, ASR, player)
          system_msg_deal.c       # system message processing task
          system_msg_deal.h
          user_msg_deal.c         # user message handling (ASR results)
          user_msg_deal.h
          system_hook.c           # system hooks
          system_hook.h
          ci_ssp_config.c         # voice signal processing config
```

### Standard Main Entry Pattern (CI130X)

```c
int main(void)
{
    /* 1. Hardware init: clocks, interrupts, platform */
    hardware_default_init();

    /* 2. Print SDK welcome */
    welcome();

    /* 3. Start FreeRTOS scheduler */
    extern int main_blinky(void);
    main_blinky();

    while(1);
    return 0;
}

/* FreeRTOS entry task */
static void task_init(void *p_arg)
{
    /* 4. Initialize dual-core communication */
    dsu_init();
    cm_init();
    nuclear_com_init();
    mailboxboot_sync();

    /* 5. Register audio codec and signal processing */
    audio_in_codec_registe();
    set_ssp_registe(&audio_capture, &ci_ssp, ...);

    /* 6. Initialize flash and model data */
    ci_flash_data_info_init(DEFAULT_MODEL_GROUP_ID);

    /* 7. Initialize player */
    audio_play_init();

    /* 8. Initialize system message and user tasks */
    sys_msg_task_initial();
    xTaskCreate(UserTaskManageProcess, "UserTaskManageProcess", 480, NULL, 4, NULL);

    /* 9. Delete self - FreeRTOS scheduler takes over */
    vTaskDelete(NULL);
}
```

### ASR Result Handling Pattern

```c
/* In user_msg_deal.c */
void user_msg_deal_task(void)
{
    /* sys_msg_t arrives from system_msg_deal_task */
    /* MSG_ASR_RESULT: ASR recognition result */
    /* MSG_KWS_RESULT: keyword spotting result */
    /* MSG_WAKEUP: wakeup event */
    /* MSG_EXIT_WAKEUP: exit wakeup timeout */

    switch (msg_type) {
    case MSG_ASR_RESULT:
        /* Get semantic ID from msg_data */
        uint16_t semantic_id = msg.data.asr_id;
        /* Map semantic ID to action */
        handle_asr_result(semantic_id);
        break;
    case MSG_WAKEUP:
        /* Play wakeup prompt if PLAY_ENTER_WAKEUP_EN */
        break;
    }
}
```

### GPIO Control Pattern

```c
/* Configure GPIO as output */
scu_set_device_gate(GPIOA_BASE, ENABLE);
gpio_set_output_mode(PA, pin_0);
gpio_set_output_high_level(PA, pin_0);

/* Configure GPIO as input with interrupt */
gpio_set_input_mode(PA, pin_1);
gpio_irq_trigger_config(PA, pin_1, both_edges_trigger);
gpio_irq_unmask(PA, pin_1);
registe_gpio_callback(PA, &my_callback_node);
```

### UART Configuration Pattern

```c
/* Protocol UART */
UART_TypeDef *uart = (UART_TypeDef*)UART_PROTOCOL_NUMBER;
scu_set_device_gate(uart, ENABLE);
pad_config_for_uart(uart);
UARTDMAConfig(uart, UART_PROTOCOL_BAUDRATE);
```

### Interrupt Handler Pattern (RISC-V Nuclei)

```c
/* In ci130x_it.c - ECLIC interrupt handlers */
__attribute__((interrupt())) void GPIOA_IRQHandler(void)
{
    /* Check and clear interrupt */
    if (gpio_get_irq_raw_status_single(PA, pin_0)) {
        gpio_clear_irq_single(PA, pin_0);
        /* Handle interrupt */
    }
}
```

### Debug Output Convention

```c
/* CI_LOG system */
#include "ci_log.h"
ci_loginfo(LOG_USER, "message: %d\n", value);
ci_logdebug(CI_LOG_DEBUG, "debug: 0x%x\n", addr);

/* mprintf for direct UART output */
mprintf("value = %d\n", value);

/* Log level controlled by CONFIG_CI_LOG_UART in user_config.h */
```

Debug UART: configured by `CONFIG_CI_LOG_UART` (typically `HAL_UART0_BASE`), baudrate set in `ci_log_init()`.

## Build Workflow

### RISC-V (CI13xx series)

| Step | Action |
|---|---|
| 1 | Install `ci-tool-1.1.2.vsix` VS Code extension from `tools/` |
| 2 | Open SDK root folder in VS Code |
| 3 | Set `ci-tool.activated_project` in `.vscode/settings.json` |
| 4 | Set `ci-tool.COMPILER_PATH` to `riscv-nuclei-elf-gcc-9.2.0/gcc_fix_raissrc/bin` |
| 5 | `Ctrl+Shift+B` -> `compile` task, or terminal: `make -j` in `project_file/` |
| 6 | Use `ci-tool-kit.exe` or `code_program.exe` to flash |
| 7 | Debug: OpenOCD + `riscv-nuclei-elf-gdb` via JLink/CMSIS-DAP |

### ARM (CI230X)

| Step | Action |
|---|---|
| 1 | Install Keil MDK or GCC-ARM toolchain |
| 2 | `cmake -B build-ci230x-wifi-sdk-combo-release` |
| 3 | `cmake --build build-ci230x-wifi-sdk-combo-release` |
| 4 | Flash LN882H via `tools/JFlash/JFlash.exe` |
| 5 | Flash CI13xx via `ci-tool-kit.exe` |

## source_file.prj Format

The `source_file.prj` file is the source of truth for what gets compiled. Format:

```
//config
define-macro: MACRO_NAME=value

//compile options
build-config: BUILD_VAR=value

//source files (relative to SDK root)
source-file: startup/ci130x_init.c
source-file: components/freertos/croutine.c
source-file: projects/<sample>/src/main.c

//include paths
include-path: components/
include-path: driver/ci130x_chip_driver/inc
```

**To add a new source file:** Add a `source-file:` line in `source_file.prj`. The Lua script (`generate_makefile.lua`) parses this and generates `build/source_file.mk`.

## Firmware Packing

After compilation, firmware is packed using `tools/PACK_UPDATE_TOOL.exe`:

1. Compile project -> generates `.elf` and `.bin` in `build/`
2. Place ASR model in `firmware/asr/`, DNN model in `firmware/dnn/`
3. Place voice prompts in `firmware/voice/`
4. Place user data in `firmware/user_file/`
5. Run `PACK_UPDATE_TOOL.exe` to create final firmware image
6. Flash via `ci-tool-kit.exe` or `code_program.exe`

## Code Generation Checklist

### RISC-V Project Setup
- [ ] `user_config.h`: correct `CI_CHIP_TYPE`, `BOARD_PORT_FILE`, `MIC_DIFF_SINGLE`
- [ ] `user_config.h`: correct `CONFIG_CI_LOG_UART` and `UART_PROTOCOL_NUMBER` (different UARTs)
- [ ] `user_config.h`: correct `USE_EXTERNAL_CRYSTAL_OSC` (0 for CI1312/CI1311, 1 for others)
- [ ] `user_config.h`: ASR config (`USE_SEPARATE_WAKEUP_EN`, `DEFAULT_MODEL_GROUP_ID`, `EXIT_WAKEUP_TIME`)
- [ ] `user_config.h`: player config (`AUDIO_PLAYER_ENABLE`, decoder flags, volume range)
- [ ] `user_config.h`: algorithm config (`USE_AEC_MODULE`, `USE_DENOISE_MODULE`, `USE_ALC_AUTO_SWITCH_MODULE`)
- [ ] `source_file.prj`: all new source files listed
- [ ] `ci130x.lds`: `SYS_HEAP_SIZE` adequate for all tasks
- [ ] SCU clock gate enabled for all used peripherals
- [ ] Board file matches physical board

### ASR Integration
- [ ] ASR model files present in `firmware/asr/` and `firmware/dnn/`
- [ ] Voice prompt files present in `firmware/voice/`
- [ ] `ci_ssp_config.c`: correct voice signal processing parameters
- [ ] `user_msg_deal.c`: handles `MSG_ASR_RESULT`, `MSG_WAKEUP`, `MSG_EXIT_WAKEUP`
- [ ] `VAD_SENSITIVITY` tuned for application
- [ ] `DEFAULT_CONFIDENCE` and `DEFAULT_CNT` set appropriately

### BLE Integration (CI23LC)
- [ ] `components/ci_ble/` included in build
- [ ] `app_ble/ble_adv_msg_deal.c`: advertising data and intervals configured
- [ ] `cias_demo_config.h`: correct demo selected
- [ ] BLE and ASR message routing verified

### Wi-Fi Combo (CI230X)
- [ ] `flash_partition_cfg.json` matches `flash_partition_table.h`
- [ ] SDIO communication between CI13xx and LN882H verified
- [ ] Wi-Fi credentials configured
- [ ] OTA partition size adequate

## Do Not Modify

- `components/` - SDK components (read-only, use as library)
- `driver/` - Chip driver source (read-only reference)
- `libs/` - Pre-compiled libraries
- `startup/` - Startup assembly (do not modify unless absolutely necessary)
- `system/ci130x_system.c` - System init (modify `user_config.h` instead)
- `SKILL.md` front matter - Skill metadata
