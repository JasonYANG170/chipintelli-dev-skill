# CI130X Development Pitfalls

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> Each pitfall includes wrong vs. correct code examples.

---

## Table of Contents

1. [SCU Clock Gate Not Enabled](#1-scu-clock-gate-not-enabled)
2. [Device Reset Not Performed](#2-device-reset-not-performed)
3. [OS-Dependent Driver Called Before RTOS Start](#3-os-dependent-driver-called-before-rtos-start)
4. [Wrong Chip Type in user_config.h](#4-wrong-chip-type-in-user_configh)
5. [Board File Mismatch](#5-board-file-mismatch)
6. [Mic Mode Mismatch](#6-mic-mode-mismatch)
7. [Log UART and Protocol UART Conflict](#7-log-uart-and-protocol-uart-conflict)
8. [PLL Frequency Mismatch](#8-pll-frequency-mismatch)
9. [Flash Write Without Erase](#9-flash-write-without-erase)
10. [source_file.prj Not Updated](#10-source_fileprj-not-updated)
11. [LTO Enabled But Debug Needed](#11-lto-enabled-but-debug-needed)
12. [Dual-Core Sync Missed](#12-dual-core-sync-missed)
13. [ASR Model Not Matching Chip](#13-asr-model-not-matching-chip)
14. [Heap Size Insufficient](#14-heap-size-insufficient)
15. [Watchdog Not Fed](#15-watchdog-not-fed)
16. [GPIO Interrupt Callback Not Registered](#16-gpio-interrupt-callback-not-registered)
17. [Flash Erase Size Not Aligned to 4KB](#17-flash-erase-size-not-aligned-to-4kb)
18. [Volume Range Exceeds Hardware Limits](#18-volume-range-exceeds-hardware-limits)
19. [ASR Recognition During Audio Playback (No AEC)](#19-asr-recognition-during-audio-playback-no-aec)
20. [Wakeup/Exit Model Switch Race Condition](#20-wakeupexit-model-switch-race-condition)

---

## 1. SCU Clock Gate Not Enabled

**Problem**: Peripheral does not respond. Registers read as 0 or the peripheral silently fails to initialize.

**Root Cause**: The SCU (System Control Unit) gates the clock to each peripheral by default. Without enabling the clock, register writes have no effect.

### Wrong

```c
// Clock gate NOT enabled - peripheral will not work
UARTDMAConfig((UART_TypeDef*)HAL_UART2_BASE, UART_BaudRate9600);
UART_EN((UART_TypeDef*)HAL_UART2_BASE, ENABLE);
```

### Correct

```c
// Step 1: Enable clock gate
scu_set_device_gate(HAL_UART2_BASE, ENABLE);
// Step 2: Reset peripheral
scu_set_device_reset(HAL_UART2_BASE);
scu_set_device_reset_release(HAL_UART2_BASE);
// Step 3: Now configure the peripheral
UARTDMAConfig((UART_TypeDef*)HAL_UART2_BASE, UART_BaudRate9600);
UART_EN((UART_TypeDef*)HAL_UART2_BASE, ENABLE);
```

**Rule**: Always call `scu_set_device_gate(DEVICE_BASE, ENABLE)` before any peripheral register access.

---

## 2. Device Reset Not Performed

**Problem**: Peripheral is in an undefined state after power-on, behaving erratically.

**Root Cause**: After enabling the clock, the peripheral must be reset to known initial state. Skipping reset leaves stale register values.

### Wrong

```c
scu_set_device_gate(HAL_GDMA_BASE, ENABLE);
// Missing reset sequence - DMA may be in undefined state
DMAC_Config(DMAC_AHBMaster1, LittleENDIANMODE);
```

### Correct

```c
scu_set_device_gate(HAL_GDMA_BASE, ENABLE);
scu_set_device_reset(HAL_GDMA_BASE);          // Assert reset
scu_set_device_reset_release(HAL_GDMA_BASE);  // Release reset
// Now DMA is in known initial state
DMAC_Config(DMAC_AHBMaster1, LittleENDIANMODE);
```

**Rule**: Always pair `scu_set_device_reset()` with `scu_set_device_reset_release()` after enabling the clock gate.

---

## 3. OS-Dependent Driver Called Before RTOS Start

**Problem**: System hangs during boot, or interrupts get permanently masked.

**Root Cause**: Drivers for QSPIFlash, DMA, I2C, and SPI use FreeRTOS API internally (semaphores, event groups). Calling them before `vTaskStartScheduler()` causes the OS primitives to fail, leaving interrupts disabled.

### Wrong

```c
int main(void)
{
    hardware_default_init();

    // WRONG: flash_init uses FreeRTOS semaphores, but scheduler not started yet
    flash_init(QSPI0);

    platform_init();
    vTaskStartScheduler();
    while(1);
}
```

### Correct

```c
int main(void)
{
    hardware_default_init();
    platform_init();
    welcome();

    xTaskCreate(task_init, "init task", 280, NULL, 4, NULL);
    vTaskStartScheduler();
    while(1);
}

static void task_init(void *p_arg)
{
    // CORRECT: Now RTOS is running, flash_init can use semaphores safely
    flash_control_inner_port_init();
    ci_flash_data_info_init(DEFAULT_MODEL_GROUP_ID);
    // ...
    vTaskDelete(NULL);
}
```

**Alternative**: If you must init before RTOS, set these macros to 0:
```c
#define CONFIG_DIRVER_BUF_USED_FREEHEAP_EN  0
#define DRIVER_OS_API                       0
```

---

## 4. Wrong Chip Type in user_config.h

**Problem**: Flash operations fail, wrong memory layout, ASR model load errors.

**Root Cause**: `CI_CHIP_TYPE` determines flash size (2MB vs 4MB), package, and available peripherals.

### Wrong

```c
// Using CI1306 (4MB flash) config but actual chip is CI1302 (2MB flash)
#define USE_CI_D06GT01D_BOARD       1   // Wrong board for CI1302
// This sets CI_CHIP_TYPE to 1306, but the chip only has 2MB flash
```

### Correct

```c
// Match the actual chip on your board
#define USE_CI_D02GS01J_BOARD       1   // CI1302, 2MB flash, SSOP24
// CI_CHIP_TYPE will be set to 1302 automatically
```

**Chip type reference**:

| Board | Chip Type | Flash | Package |
|-------|-----------|-------|---------|
| CI-D02GS01J | 1302 | 2MB | SSOP24 |
| CI-D02GS02S | 1302 | 2MB | SSOP24 |
| CI-D12GS01J | 1312 | 2MB | SSOP16 |
| CI-D06GT01D | 1306 | 4MB | QFN40 |

---

## 5. Board File Mismatch

**Problem**: GPIO pins don't work, UART output is garbage, codec doesn't record.

**Root Cause**: The board file (`BOARD_PORT_FILE`) configures pin multiplexing, pad settings, and peripheral routing. A wrong board file applies wrong pinmux.

### Wrong

```c
// Board file doesn't match physical hardware
#define USE_CI_D02GS01J_BOARD       1   // Using terminal module config
#define BOARD_PORT_FILE             "CI-D02GS01J.c"
// But actual hardware is CI-D06GT01D dev board with different pinout
```

### Correct

```c
// Verify physical board, then set correct config
#define USE_CI_D06GT01D_BOARD       1
#define BOARD_PORT_FILE             "CI-D06GT01D.c"
```

**How to verify**: Check the silk screen on your board, or check `driver/boards/` for available board files.

---

## 6. Mic Mode Mismatch

**Problem**: No audio input, very low recording volume, or excessive noise.

**Root Cause**: `MIC_DIFF_SINGLE` must match the physical microphone circuit. Differential mode uses both MICP and MICN pins; single-end mode ties MICN to GND.

### Wrong

```c
// Hardware uses single-end mic (MICN_L tied to GND) but config is differential
#define MIC_DIFF_SINGLE             0   // 0 = differential, but hardware is single-end
```

### Correct

```c
// Match hardware: single-end mic
#define MIC_DIFF_SINGLE             1   // 1 = single-end mode
```

**How to check**: Measure MICN_L pin. If connected to GND, use single-end mode (1). If connected to the negative terminal of the mic, use differential mode (0).

---

## 7. Log UART and Protocol UART Conflict

**Problem**: System asserts at boot: `"Log uart and protocol uart confict!"`. Protocol messages corrupt log output.

**Root Cause**: Both the log system and the voice module protocol use the same UART.

### Wrong

```c
#define CONFIG_CI_LOG_UART          HAL_UART0_BASE  // Log on UART0
#define UART_PROTOCOL_NUMBER        (HAL_UART0_BASE) // Protocol ALSO on UART0 - CONFLICT!
```

### Correct

```c
#define CONFIG_CI_LOG_UART          HAL_UART0_BASE  // Log on UART0
#define UART_PROTOCOL_NUMBER        (HAL_UART2_BASE) // Protocol on UART2 - no conflict
#define UART_PROTOCOL_BAUDRATE      (UART_BaudRate9600)
```

---

## 8. PLL Frequency Mismatch

**Problem**: System hangs at boot with `"PLL config err!"` message.

**Root Cause**: The actual PLL frequency doesn't match `MAIN_FREQUENCY`. This happens when clock source (internal RC vs external crystal) is misconfigured.

### Wrong

```c
// Config says external crystal but board has no external crystal
#define USE_EXTERNAL_CRYSTAL_OSC    1  // No crystal on board!
// MAIN_FREQUENCY becomes 240MHz, but actual frequency from RC is ~200MHz
// Boot check: abs(200000000 - 240000000) > 10000000 => HANG
```

### Correct

```c
// Use internal RC if no external crystal
#define USE_EXTERNAL_CRYSTAL_OSC    0  // Use internal RC
// MAIN_FREQUENCY becomes 200MHz, matching actual frequency

// For CI1312/CI1311, external crystal is not supported:
#if ((CI_CHIP_TYPE == 1312) || (CI_CHIP_TYPE == 1311))
#define USE_EXTERNAL_CRYSTAL_OSC    0  // Must use internal RC
#endif
```

---

## 9. Flash Write Without Erase

**Problem**: Flash write succeeds but data is corrupted. Subsequent reads return wrong values.

**Root Cause**: SPIFlash can only change bits from 1 to 0 during write. Erasing sets all bits to 1. Writing without erase causes OR-ing of old and new data.

### Wrong

```c
// Write without erase - data will be corrupted
uint32_t buf[16] = {0x12345678};
flash_write(QSPI0, 0x100000, (uint32_t)buf, sizeof(buf));
```

### Correct

```c
uint32_t buf[16] = {0x12345678};

// Step 1: Erase the sector (4KB minimum)
flash_erase(QSPI0, 0x100000, 4096);

// Step 2: Write data
flash_write(QSPI0, 0x100000, (uint32_t)buf, sizeof(buf));

// Step 3: Read back to verify
uint32_t read_buf[16];
flash_read(QSPI0, (uint32_t)read_buf, 0x100000, sizeof(read_buf));
```

**Note**: Erase granularity is 4KB sectors (`SPIC_CMD_CODE_SECTORERASE4K`), 32KB blocks, or 64KB blocks.

---

## 10. source_file.prj Not Updated

**Problem**: New .c file is not compiled. Build succeeds but functions are undefined at link time.

**Root Cause**: The Make + Lua build system reads `source_file.prj` to generate `build/source_file.mk`. Adding files to the Makefile directly has no effect.

### Wrong

```makefile
# Adding source files directly in Makefile - WILL NOT WORK
C_SRCS += my_custom_code.c
```

### Correct

```
# In source_file.prj, add a new line:
source-file: projects/offline_asr_pro_sample/src/my_custom_code.c
```

Then rebuild:
```bash
make clean
make -j4
```

The Lua script (`generate_makefile.lua`) parses `source_file.prj` and generates the compilation rules in `build/source_file.mk`.

---

## 11. LTO Enabled But Debug Needed

**Problem**: Debugger shows optimized-out variables, cannot set breakpoints, stack traces are incomplete.

**Root Cause**: LTO (Link-Time Optimization) is enabled by default via `LTO_OPTION = -flto` in the Makefile.

### Wrong

```makefile
# In Makefile - LTO is always on, can't debug properly
LTO_OPTION = -flto
```

### Correct

For debugging, remove LTO:
```makefile
# In project Makefile, comment out or empty the LTO option:
# LTO_OPTION = -flto
LTO_OPTION =
```

For release builds, keep LTO for smaller code size:
```makefile
LTO_OPTION = -flto
```

Or use the `RELEASE` flag:
```bash
make RELEASE=TRUE -j4  # Release build (no -g debug info)
make -j4                # Debug build (with -g, but LTO still on)
```

---

## 12. Dual-Core Sync Missed

**Problem**: ASR doesn't start, nuclear core (DSP) doesn't respond, system hangs.

**Root Cause**: CI130X has a dual-core architecture (host N300 + nuclear DSP). `mailboxboot_sync()` must be called after `nuclear_com_init()` to synchronize both cores.

### Wrong

```c
static void task_init(void *p_arg)
{
    cm_init();
    // Missing nuclear_com_init() and mailboxboot_sync()
    // Nuclear core never starts, ASR will not work

    ci_flash_data_info_init(DEFAULT_MODEL_GROUP_ID);
    audio_play_init();
}
```

### Correct

```c
static void task_init(void *p_arg)
{
    cm_init();

    // Step 1: Initialize dual-core communication
    nuclear_com_init();

    // Step 2: Initialize inter-core communication channels
    decoder_port_inner_rpmsg_init();
    flash_control_inner_port_init();
    dnn_nuclear_com_outside_port_init();
    asr_top_nuclear_com_outside_port_init();
    vad_fe_nuclear_com_outside_port_init();
    flash_manage_nuclear_com_outside_port_init();
    codec_manage_inner_port_init();

    // Step 3: Synchronize cores - MUST be called
    mailboxboot_sync();

    // Step 4: Now safe to init ASR and flash data
    ci_flash_data_info_init(DEFAULT_MODEL_GROUP_ID);
    audio_play_init();
}
```

---

## 13. ASR Model Not Matching Chip

**Problem**: ASR recognition fails completely, or system crashes when loading model.

**Root Cause**: ASR model files in `firmware/asr/` and `firmware/dnn/` are chip-specific. A model generated for CI1306 will not work on CI1302.

### Wrong

```
firmware/asr/
  [0]asr_chinese_CI1306_V00874_cmd.dat    # CI1306 model
  [1]asr_chinese_CI1306_V00874_wake.dat   # CI1306 model

# But user_config.h says:
#define CI_CHIP_TYPE 1302    # CI1302 chip!
```

### Correct

```
# Generate model for the correct chip on https://aiplatform.chipintelli.com
# Then place correct model files:
firmware/asr/
  [0]asr_chinese_CI1302_Vxxxxx_cmd.dat    # CI1302 model
  [1]asr_chinese_CI1302_Vxxxxx_wake.dat   # CI1302 model

# And ensure user_config.h matches:
#define CI_CHIP_TYPE 1302
```

---

## 14. Heap Size Insufficient

**Problem**: `xTaskCreate()` fails, malloc returns NULL, random crashes under load.

**Root Cause**: `SYS_HEAP_SIZE` in the linker script (`ci130x.lds`) is too small for all FreeRTOS tasks and buffers.

### Wrong

```lds
/* In ci130x.lds - heap too small for complex application */
SYS_HEAP_SIZE = (1024 * 30);  /* 30KB - too small */
```

### Correct

```lds
/* Increase heap size */
SYS_HEAP_SIZE = (1024 * 60);  /* 60KB - default for offline_asr_pro_sample */
```

**Verify at runtime**:
```c
// Check free heap in main loop
mprintf("system heap free:%dKB\n", xPortGetFreeHeapSize()/1024);
mprintf("system heap min free:%dKB\n", xPortGetMinimumEverFreeHeapSize()/1024);
mprintf("asr heap min free:%dKB\n", get_heap_bytes_remaining_size()/1024);
```

If `xPortGetMinimumEverFreeHeapSize()` is below 2KB, increase `SYS_HEAP_SIZE`.

---

## 15. Watchdog Not Fed

**Problem**: System resets unexpectedly after ~2 seconds of operation.

**Root Cause**: IWDG is initialized in `platform_init()` with a 2-second timeout. Long-running tasks that don't yield will trigger the watchdog.

### Wrong

```c
// Long blocking operation without yielding
void my_long_task(void *p_arg)
{
    while (1)
    {
        process_large_data();  // Takes 5+ seconds
        // No vTaskDelay, no iwdg_feed - watchdog resets system!
    }
}
```

### Correct

```c
void my_long_task(void *p_arg)
{
    while (1)
    {
        // Process in chunks, yielding between chunks
        for (int i = 0; i < chunk_count; i++)
        {
            process_chunk(i);
            vTaskDelay(pdMS_TO_TICKS(10));  // Yield to other tasks
        }
    }
}
```

**Or feed the watchdog manually** (not recommended for regular tasks):
```c
iwdg_feed(IWDG);  // Only use in critical sections
```

**Watchdog config** (from `main.c`):
```c
iwdg_init_t init;
init.irq = iwdg_irqen_enable;
init.res = iwdg_resen_enable;
init.count = ((get_src_clk()/0x10)*3);  // ~2 seconds
iwdg_init(IWDG, init);
iwdg_open(IWDG);
```

---

## 16. GPIO Interrupt Callback Not Registered

**Problem**: GPIO interrupt fires but user callback never executes.

**Root Cause**: The callback must be registered via `registe_gpio_callback()` with a properly linked list node.

### Wrong

```c
// Configured interrupt but no callback registered
gpio_set_input_mode(PA, pin_3);
gpio_irq_trigger_config(PA, pin_3, both_edges_trigger);
gpio_irq_unmask(PA, pin_3);
// Missing callback registration - interrupt fires but nothing happens
```

### Correct

```c
// Define callback
static void my_gpio_callback(void)
{
    if (gpio_get_irq_raw_status_single(PA, pin_3))
    {
        gpio_clear_irq_single(PA, pin_3);
        // Handle interrupt
    }
}

// Define callback node
static gpio_irq_callback_list_t my_callback_node = {
    .gpio_irq_callback = my_gpio_callback,
    .next = NULL,
};

// Configure and register
gpio_set_input_mode(PA, pin_3);
gpio_irq_trigger_config(PA, pin_3, both_edges_trigger);
gpio_irq_unmask(PA, pin_3);
registe_gpio_callback(PA, &my_callback_node);
```

---

## 17. Flash Erase Size Not Aligned to 4KB

**Problem**: `flash_erase()` fails or erases more data than expected.

**Root Cause**: SPIFlash erase operates on 4KB sector boundaries. Passing a non-aligned size causes unpredictable behavior.

### Wrong

```c
// Erasing 100 bytes - not aligned to 4KB sector
flash_erase(QSPI0, 0x100100, 100);
flash_write(QSPI0, 0x100100, (uint32_t)buf, 100);
```

### Correct

```c
// Always erase full 4KB sectors
#define FLASH_SECTOR_SIZE 4096

uint32_t addr = 0x100100;
uint32_t size = 100;

// Align address down to sector boundary
uint32_t erase_addr = addr & ~(FLASH_SECTOR_SIZE - 1);
// Calculate number of sectors to erase
uint32_t erase_size = ((addr + size - erase_addr) + FLASH_SECTOR_SIZE - 1) & ~(FLASH_SECTOR_SIZE - 1);

flash_erase(QSPI0, erase_addr, erase_size);
flash_write(QSPI0, addr, (uint32_t)buf, size);
```

---

## 18. Volume Range Exceeds Hardware Limits

**Problem**: Audio distortion at high volume, or no sound at low volume.

**Root Cause**: `VOLUME_MAX` and `VOLUME_MIN` must match hardware capabilities. The `vol_set()` function maps these to a hardware gain range.

### Wrong

```c
#define VOLUME_MAX    20   // Too high - hardware only supports 0-7
#define VOLUME_MIN    0    // 0 maps to silence
```

### Correct

```c
#define VOLUME_MAX    7    // Hardware maximum
#define VOLUME_MIN    1    // Minimum audible volume
#define VOLUME_DEFAULT 5   // Default startup volume
```

**Volume mapping** (from `system_msg_deal.c`):
```c
uint8_t vol_set(char vol)
{
    if (vol <= VOLUME_MAX && vol >= VOLUME_MIN)
    {
        audio_play_set_vol_gain(67 * vol / VOLUME_MAX + 7);
        cinv_item_write(NVDATA_ID_VOLUME, sizeof(vol), &vol);
    }
    return vol;
}
```

---

## 19. ASR Recognition During Audio Playback (No AEC)

**Problem**: ASR false triggers or fails to recognize during prompt playback. The speaker's audio feeds back into the microphone.

**Root Cause**: Without AEC (Acoustic Echo Cancellation), the microphone picks up the prompt audio from the speaker, confusing the ASR engine.

### Wrong

```c
// No AEC enabled, ASR runs during playback
#define USE_AEC_MODULE              0
// Result: prompt audio causes false recognition or masks real commands
```

### Correct

```c
// Enable AEC to cancel echo from speaker
#define USE_AEC_MODULE              1
#define IF_JUST_CLOSE_HPOUT_WHILE_NO_PLAY   1
#define HOST_CODEC_CHA_NUM          2

// Configure AEC in ci_ssp_config.c:
const aec_config_t aec_config = {
    .mic_channel_num = 1,
    .ref_channel_num = 1,
    .aec_control_mode = COMPUTE_REF_AMPL_MODE,
    .aec_gain = 1.0f,
    .aec_enable_threshold = 6000.0f,
    .aec_mic_div_ref_thr = 0.05f,
    .nlp_flag = 2,
    .aggr_mode = 1,
    .fft_size = 256,
    .alc_off_codec_adc_gain_mic = 20,
    .alc_off_codec_adc_gain_ref = 0,
};
```

**Alternative** (simpler but less responsive): Pause ASR during playback:
```c
pause_asr(1, 1);  // Mute mic and pause ASR
play_prompt(...);  // Play prompt
// Resume after playback done callback
```

---

## 20. Wakeup/Exit Model Switch Race Condition

**Problem**: System gets stuck in wrong model state, or ASR stops responding after wakeup/exit cycle.

**Root Cause**: The wakeup/exit flow involves model switching, prompt playback, and state updates across multiple asynchronous paths. Race conditions can occur if these are not serialized through the system message queue.

### Wrong

```c
// Directly switching models from arbitrary tasks - causes race condition
void my_task(void *p_arg)
{
    // WRONG: Don't call model switch directly from other tasks
    cmd_info_change_cur_model_group(0);  // Race with system message task!
}
```

### Correct

```c
// Send a message to the system message task to handle model switch
void my_task(void *p_arg)
{
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD;
    send_msg_to_sys_task(&send_msg, NULL);
    // The system message task handles this serially
}
```

**Key mechanism**: The SDK uses `ignore_exit_wakeup` and `ignore_asr_msg` counters to handle concurrent wakeup/exit events. These are managed exclusively in `system_msg_deal.c`'s `UserTaskManageProcess()`. Never bypass this by calling model switch functions directly.
