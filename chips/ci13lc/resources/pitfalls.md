# CI13LC Pitfalls and Common Errors

> SDK: `CI13LC_SDK_V2.0.15` / `CI13LC_SDK_2.1.5`
> Chips: CI13080, CI13081, CI13082, CI13160, CI13160P, CI13161, CI13161P, CI13162, CI13240, CI13241, CI13242, CI13320, CI13321, CI13322, CI13322S, CI13642

---

## 1. Chip Variant Selection -- `CI_CHIP_TYPE` Must Match Actual Silicon

CI13LC covers 16+ chip variants in one SDK. The `CI_CHIP_TYPE` macro in `user_config.h` **must** match the physical chip on your board. The build system uses this to select the correct chip header (`ci13080.h`, `ci13242.h`, `ci13322.h`, etc.) which defines `FLASH_SIZE` and the `PinPad_Name` enum.

```c
// user_config.h -- WRONG: chip is CI13242 but type says CI13322
#define CI_CHIP_TYPE  13322  // Wrong! Will use ci1332x.h pin map

// CORRECT:
#define CI_CHIP_TYPE  13242  // Uses ci1324x.h pin map, FLASH_SIZE=2MB
```

**Symptom**: Pin assignments silently wrong, flash operations fail or corrupt data, `check_chip_name()` logs an error at boot.

**Fix**: Always verify `CI_CHIP_TYPE` matches the chip marking. The SDK `main.c` calls `check_chip_name()` which compares `CI_CHIP_TYPE` against `BD_SURPPORT_CHIP_NAME` from the board file.

### Chip Variant Table

| CI_CHIP_TYPE | Header | Flash Size | Pin Family | Typical Package |
|---|---|---|---|---|
| 13080 | `ci13080.h` -> `ci1308x.h` | 512KB | ci1308x (10 pads) | SOP8 |
| 13081 | `ci13081.h` -> `ci1308x.h` | 1MB | ci1308x (10 pads) | SOP8 |
| 13082 | `ci13082.h` -> `ci1308x.h` | 2MB | ci1308x (10 pads) | SOP8 |
| 13160 | `ci13160.h` -> `ci1316x.h` | 512KB | ci1316x (13 pads) | SOP14 |
| 13160P | `ci13160P.h` -> `ci1316x.h` | 512KB | ci1316x (13 pads) | SOP14 |
| 13161 | `ci13161.h` -> `ci1316x.h` | 1MB | ci1316x (13 pads) | SOP14 |
| 13161P | `ci13161P.h` -> `ci1316x.h` | 1MB | ci1316x (13 pads) | SOP14 |
| 13162 | `ci13162.h` -> `ci1316x.h` | 2MB | ci1316x (13 pads) | SOP14 |
| 13240 | `ci13240.h` -> `ci1324x.h` | 512KB | ci1324x (19 pads) | SOP16 |
| 13241 | `ci13241.h` -> `ci1324x.h` | 1MB | ci1324x (19 pads) | SOP16 |
| 13242 | `ci13242.h` -> `ci1324x.h` | 2MB | ci1324x (19 pads) | SOP16 |
| 13320 | `ci13320.h` -> `ci1332x.h` | 512KB | ci1332x (27 pads) | QFN32 |
| 13321 | `ci13321.h` -> `ci1332x.h` | 1MB | ci1332x (27 pads) | QFN32 |
| 13322 | `ci13322.h` -> `ci1332x.h` | 2MB | ci1332x (27 pads) | QFN32 |
| 13322S | `ci13322S.h` -> `ci1332x.h` | 1MB | ci1332x (27 pads) | QFN32 |
| 13642 | `ci13642.h` -> `ci1332x.h` | 2MB | ci1332x (27 pads) | QFN32 |

---

## 2. Flash Size Differences -- Model and Voice Files Must Fit

Flash sizes vary dramatically across CI13LC variants:

| Size | Chips | Impact |
|---|---|---|
| 512KB | CI13080, CI13160, CI13160P, CI13240, CI13320 | Very limited -- small ASR models only, no MP3 voice prompts |
| 1MB | CI13081, CI13161, CI13161P, CI13241, CI13321, CI13322S | Moderate -- standard ASR + prompt voice |
| 2MB | CI13082, CI13162, CI13242, CI13322, CI13642 | Full -- ASR + MP3 voice + CWSL + multi-intent |

**Pitfall**: A firmware built for CI13322 (2MB) will not fit on CI13320 (512KB). The pack tool will fail or the image will be truncated.

**Fix**: Check `FLASH_SIZE` in the chip header. If developing for multiple flash sizes, use the smallest target for testing. Disable `USE_MP3_DECODER` and `AUDIO_PLAY_SUPPT_MP3_PROMPT` for 512KB chips.

---

## 3. Pin Count Differences -- Same Function, Different Pin

The same peripheral function may be on different pins across chip families:

| Function | ci1308x | ci1316x | ci1324x | ci1332x |
|---|---|---|---|---|
| UART0_TX | PB5 | PB5 | PB5 | PB5 |
| UART0_RX | PB6 | PB6 | PB6 | PB6 |
| UART1_TX | -- | PA2 | PA2 | PA2 |
| UART2_TX | PC1 | -- | PC1 | PC1 |
| IIC0_SDA | PB5 | PA2 | PA2 | PA2 |
| IIC0_SCL | PB6 | PA3 | PA3 | PA3 |
| PWM0 | PC0 | PA2 | PA2 | PA2 |
| IIS0_SDI | -- | PA2 | PA2 | PA2 |
| EXT_INT0 | -- | -- | -- | PA7 |
| EXT_INT1 | -- | -- | -- | PB0 |

**Pitfall**: Code written for CI13322 (27 pads) using PA7 for external interrupt will fail to compile or run on CI13162 (13 pads) where PA7 does not exist.

**Fix**: Always use the `PinPad_Name` enum from the correct chip family header. The compiler will catch invalid pin references.

---

## 4. CI13LC vs CI130X Header Confusion

CI13LC SDK provides **both** `ci13lc_*.h` and `ci_*.h` headers (without the `13lc` prefix). The `ci130x_*.h` headers do **not** exist in CI13LC SDK.

**Pitfall**: Copying CI130X code that includes `ci130x_gpio.h` will fail. The function names are identical, but the headers are different.

```c
// WRONG (CI130X style):
#include "ci130x_gpio.h"
#include "ci130x_uart.h"

// CORRECT (CI13LC style):
#include "ci_gpio.h"      // or "ci13lc_gpio.h"
#include "ci_uart.h"      // or "ci13lc_uart.h"
```

---

## 5. Clock Source Selection -- Internal RC vs External Crystal

CI13LC chips can use either internal RC oscillator or external crystal. The choice affects UART baud rate accuracy.

```c
// For CI1312/CI1311 (no external crystal pads):
#define USE_EXTERNAL_CRYSTAL_OSC  0  // Must use internal RC

// For CI13322/CI13242 (has PA0/PA1 crystal pads):
#define USE_EXTERNAL_CRYSTAL_OSC  1  // Use external crystal
```

**Pitfall**: Using internal RC without enabling baud rate auto-calibration causes UART communication errors.

**Fix**: When `USE_EXTERNAL_CRYSTAL_OSC=0`, enable:
```c
#define UART_BAUDRATE_CALIBRATE  1
#define BAUDRATE_SYNC_PERIOD     300000  // ms
```

---

## 6. SCU Clock Gate -- Must Enable Before Any Peripheral Access

Every peripheral must have its clock enabled via `scu_set_device_gate()` before any register access. Additionally, DPMU IO reuse and direction must be configured.

```c
// WRONG: Access GPIO without enabling clock
gpio_set_output_high_level(PA, pin_2);  // Hangs or no effect

// CORRECT:
scu_set_device_gate(HAL_PA_BASE, ENABLE);
dpmu_set_io_reuse(PA2, FIRST_FUNCTION);
dpmu_set_io_direction(PA2, DPMU_IO_DIRECTION_OUTPUT);
gpio_set_output_mode(PA, pin_2);
gpio_set_output_high_level(PA, pin_2);
```

---

## 7. OS-Dependent Drivers -- Do Not Call Before Scheduler Start

QSPIFlash, DMA, I2C, and SPI drivers use FreeRTOS API (semaphores, event groups). Calling them before `vTaskStartScheduler()` will hang or corrupt state.

**Pitfall**: Initializing I2C or flash in `hardware_default_init()` (before FreeRTOS starts).

**Fix**: Move driver init to `task_init()` or `vTaskVariablesInit()`. If pre-RTOS init is absolutely needed:
```c
#define CONFIG_DIRVER_BUF_USED_FREEHEAP_EN  0
#define DRIVER_OS_API                        0
```

---

## 8. CWSL and Multi-Intent Are Mutually Exclusive

The CWSL (Command Word Self-Learning) and multi-intent features cannot be used simultaneously.

```c
// WRONG:
#define USE_CWSL      1
#define MULTI_INTENTS 3  // Error! CWSL + multi-intent not supported
```

The CI13LC SDK provides a combined `cwsl_multi_intents_sample` that uses CWSL for wakeup words only and multi-intent for command words. But standard CWSL for command words + multi-intent is not supported.

---

## 9. NL ASR (Natural Language) Requires `MULTI_INTENTS=1`

The `nl_asr_sample` sets `MULTI_INTENTS 1` with a comment "must be 1, cannot modify". Natural language ASR builds on the multi-intent framework. Setting it to 0 will break NL ASR.

---

## 10. CWSL Template Storage -- Flash Space Reservation

CWSL stores learned templates in flash via the `ci_nvdm` (non-volatile data management) component. The `CICWSL_TOTAL_TEMPLATE` macro reserves flash space at build time.

```c
#define CICWSL_TOTAL_TEMPLATE  10  // Reserves space for 10 templates
```

**Pitfall**: Setting this too high wastes flash; too low means learning fails after N words. The limit is 1000 command word nodes + 10 learning combinations.

---

## 11. ASR Model Must Match Chip Type

ASR model files in `firmware/asr/` are generated by the voice AI platform for a specific chip type. A model generated for CI1306 (CI130X) will not work correctly on CI13322 (CI13LC), even though both are Nuclei N300 RISC-V.

**Fix**: Regenerate models from https://aiplatform.chipintelli.com for the correct CI13LC chip type.

---

## 12. `ASR_FE_REDUCE_MEM` -- Memory Optimization Flag

```c
#define ASR_FE_REDUCE_MEM  1  // Saves ~15KB memory
```

This flag enables a memory-optimized ASR front-end. The `offline_asr_sample` defaults to 0, but `cwsl_sample`, `nl_asr_sample`, and `multi_intents_asr_sample` default to 1 (they need the extra memory). When porting between samples, ensure this flag is set correctly.

---

## 13. Log UART and Protocol UART Conflict

```c
// WRONG: Same UART for log and protocol
#define CONFIG_CI_LOG_UART       HAL_UART0_BASE
#define UART_PROTOCOL_NUMBER     HAL_UART0_BASE  // Conflict!

// CORRECT: Different UARTs
#define CONFIG_CI_LOG_UART       HAL_UART0_BASE
#define UART_PROTOCOL_NUMBER     HAL_UART2_BASE
```

The SDK asserts this at runtime: `CI_ASSERT(0,"Log uart and protocol uart confict!\n")`.

---

## 14. IIS1 Is Connected to Inner CODEC

On CI13LC, IIS1 is internally connected to the inner CODEC (ADC/DAC). IIS0 is the external IIS interface. Using IIS1 for external audio I/O requires reconfiguring the SCU clock routing, which conflicts with the inner CODEC.

**Pitfall**: Trying to use IIS1 for external I2S codec while the inner CODEC is active.

---

## 15. `BOARD_CONFIG_FILE` Must Match Physical Board

```c
#define BOARD_CONFIG_FILE  "CI-F32XGT01D-V10.h"
```

The board config file defines mic bias, PA control pin, codec settings, and pad configuration. Using the wrong board file causes silent failures (no audio, wrong pin mux).

---

## 16. Watchdog Default Configuration

CI13LC SDK uses `iwdg_config_reset(HAL_IWDG_BASE)` for default watchdog configuration. This enables the watchdog with reset on timeout. Long-running operations (flash erase, ASR model loading) must yield or the watchdog will reset the chip.

```c
// In platform_init():
iwdg_config_reset(HAL_IWDG_BASE);  // Default watchdog with reset
```

---

## 17. EPWM vs PWM -- Different Modules

CI13LC has both standard PWM (`ci13lc_pwm.h`) and Enhanced PWM (`ci13lc_epwm.h`). They are separate hardware modules:
- **PWM0-PWM3**: Simple PWM with frequency/duty control. Base: `0x40014000-0x40017000`.
- **EPWM**: Single module with dead-band, trip-zone, ADC trigger. Base: `0x40007000`.

Do not confuse `pwm_init()` with `epwm_init()` -- they configure different hardware.

---

## 18. Main Frequency Must Match PLL Configuration

```c
#define MAIN_FREQUENCY  210000000  // 210MHz
```

At boot, `welcome()` checks `get_ipcore_clk()` against `MAIN_FREQUENCY` with 10MHz tolerance. If mismatched, the code enters an infinite loop:
```c
if (abs(((int)get_ipcore_clk()) - ((int)MAIN_FREQUENCY)) > 10000000) {
    ci_logerr(LOG_USER,"PLL config err!\n");
    while(1);
}
```

**Fix**: Ensure `dpmu_pll_config()` in `SystemInit()` produces the correct frequency matching `MAIN_FREQUENCY`.
