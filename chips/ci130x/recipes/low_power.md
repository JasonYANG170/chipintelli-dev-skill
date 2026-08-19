# Recipe: Low Power Modes

> SDK: `CI130X_SDK_Offline_V2.1.14`
> Chips: CI1302, CI1303, CI1306, CI1312

---

## Overview

The CI130X supports several power modes to reduce current consumption. The `ci130x_lowpower` driver manages mode switching (normal, down-frequency, oscillator, sleep, deep-sleep), while the `ci130x_dpmu` driver provides fine-grained control over power management unit (PMU) settings including IO configuration, clock sources, LDO control, and wakeup source configuration.

---

## SDK

`CI130X_SDK_Offline_V2.1.14` - driver: `ci130x_chip_driver`

---

## Source Anchors

- `ci130x_sdk/driver/ci130x_chip_driver/inc/ci130x_lowpower.h` - power mode switching
- `ci130x_sdk/driver/ci130x_chip_driver/inc/ci130x_dpmu.h` - PMU configuration, IO, wakeup, LDO, clock

---

## API Usage

### Power Modes (ci130x_lowpower.h)

```c
typedef enum {
    POWER_MODE_ERR            = -1,
    POWER_MODE_NORMAL         = 0,   // Normal operation
    POWER_MODE_DOWN_FREQUENCY = 1,   // Reduced clock frequency
    POWER_MODE_OSC_FREQUENCY  = 2,   // Oscillator clock mode
    POWER_MODE_SLEEP          = 998, // Sleep mode
    POWER_MODE_DEEP_SLEEP     = 999, // Deep sleep mode
} power_mode_t;

// Register user callbacks for entering/exiting low power
void register_lowpower_user_fn(void *enter_lowpower_fn, void *exit_lowpower_fn);

// Switch power mode
power_mode_t power_mode_switch(power_mode_t power_mode);

// Get current power mode
power_mode_t get_curr_power_mode(void);
```

### Compile-Time Mode Enables

```c
#define POWER_MODE_DOWN_FREQUENCY_EN  1  // Enable down-frequency mode
#define POWER_MODE_OSC_FREQUENCY_EN    1  // Enable oscillator mode
#define POWER_MODE_SLEEP_EN            0  // Sleep mode (not used)
#define POWER_MODE_DEEP_SLEEP_EN       0  // Deep sleep (not used)
```

### DPMU Low Power Mode (ci130x_dpmu.h)

```c
typedef enum {
    DPMU_LOWPOWER_NO_MODE       = 0,
    DPMU_LOWPOWER_SLEEP_MODE    = 1,
    DPMU_LOWPOWER_DEEP_SLEEP_MODE = 2,
    DPMU_LOWPOWER_BOTH_MODE     = 3,
} Dpmu_Lowpower_Mode_t;

// Configure DPMU low power mode
void dpmu_set_low_power_mode(Dpmu_Lowpower_Mode_t mode);
```

### Wakeup Configuration (ci130x_dpmu.h)

```c
typedef enum {
    DPMU_WAKEUP_GPIO           = 0,  // AON GPIO wakeup
    DPMU_WAKEUP_IWDG           = 1,  // IWDG wakeup
    DPMU_WAKEUP_EFUSE          = 2,  // EFUSE controller
    DPMU_WAKEUP_TIMER01_GPWM01 = 3,  // Timer0/1, GPWM0/1
    DPMU_WAKEUP_IO_REG         = 4,  // DPMU IO registers
    DPMU_WAKEUP_PMU_RC_REG     = 5,  // PMU and RC registers
} Dpmu_Wakeup_Reset_Cfg_t;

// Enable/disable wakeup interrupt for a specific source
void dpmu_set_wakeup_int(int32_t wake_int_num, FunctionalState flag);

// Configure wakeup reset behavior
void dpmu_wakeup_reset_cfg(Dpmu_Wakeup_Reset_Cfg_t model, FunctionalState flag);

// Get wakeup state (which source caused wakeup)
uint32_t dpmu_get_wakeup_state(void);
```

### IO Configuration (ci130x_dpmu.h)

```c
typedef enum {
    DPMU_IO_PULL_DISABLE = 0,
    DPMU_IO_PULL_UP       = 1,
    DPMU_IO_PULL_DOWN     = 2,
} Dpmu_Io_Pull_t;

typedef enum {
    DPMU_IO_DIRECTION_INPUT  = 0,
    DPMU_IO_DIRECTION_OUTPUT = 1,
} Dpmu_Io_Direction_t;

void dpmu_set_io_reuse(PinPad_Name pin, IOResue_FUNCTION io_function);
void dpmu_set_adio_reuse(PinPad_Name pin, ADIOResue_MODE adio_mode);
void dpmu_set_io_pull(PinPad_Name pin, Dpmu_Io_Pull_t pull);
void dpmu_set_io_direction(PinPad_Name pin, Dpmu_Io_Direction_t dir);
void dpmu_set_io_open_drain(PinPad_Name pin, FunctionalState cmd);
void dpmu_set_io_slew_rate(PinPad_Name pin, Dpmu_Io_Slew_Rate_t slew_rate);
void dpmu_set_io_schmitt_trigger(PinPad_Name pin, Dpmu_Io_Schmitt_Trigger_t schmitt_trigger);
void dpmu_set_io_driver_strength(PinPad_Name pin, Dpmu_Io_Driver_Strength_t driver_strength);
```

### Clock and LDO Configuration (ci130x_dpmu.h)

```c
// SRC clock source selection
typedef enum {
    DPMU_SRC_USE_SYSTEM_DEFAULT = 0,
    DPMU_SRC_USE_INNER_RC       = 1,  // Internal RC oscillator
    DPMU_SRC_USE_OUTSIDE_OSC    = 3,  // External oscillator
} Dpmu_Src_Source_Sel_t;

void dpmu_set_src_source(Dpmu_Src_Source_Sel_t sel);
void dpmu_use_rc(void);

// RC frequency selection
typedef enum {
    DPMU_RC_FREQ_12d288M = 0,
    DPMU_RC_FREQ_2M,
    DPMU_RC_FREQ_4M,
    DPMU_RC_FREQ_8M,
    DPMU_RC_FREQ_16M,
    DPMU_RC_FREQ_24M,
    DPMU_RC_FREQ_32M,
    DPMU_RC_FREQ_64M,
} Dpmu_Rc_Freq_Sel_t;

void dpmu_rc_freq_sel(Dpmu_Rc_Freq_Sel_t sel);
void dpmu_set_rc_en(bool en);

// PLL configuration
void dpmu_pll_12d_config(uint32_t clk);
uint32_t dpmu_get_pll_frequency(void);

// LDO control
void dpmu_ldo1_lv_set(uint8_t lv);
void dpmu_ldo2_en(bool en);
void dpmu_ldo2_lv_set(uint8_t lv);
void dpmu_ldo3_en(bool en);
void dpmu_ldo3_lv_set(uint8_t lv);

// Crystal oscillator
void dpmu_osc_pad_for_gpio(FunctionalState en);  // ENABLE=GPIO, DISABLE=crystal

// Voltage detection
typedef enum {
    DPMU_VDT_LV_2_4V = 0,
    DPMU_VDT_LV_2_5V,
    DPMU_VDT_LV_2_6V,
    DPMU_VDT_LV_2_7V,
    DPMU_VDT_LV_2_8V,
    DPMU_VDT_LV_2_9V,
    DPMU_VDT_LV_3_0V,
    DPMU_VDT_LV_3_1V,
} Dpmu_Vdt_Lv_t;

void dpmu_vdt_lv_set(Dpmu_Vdt_Lv_t lv);
void dpmu_vdt_en(bool en);
```

### IWDG Halt During Sleep (ci130x_dpmu.h)

```c
void dpmu_set_iwdg_halt(void);    // Halt IWDG during sleep
void dpmu_clean_iwdg_halt(void);  // Resume IWDG after wakeup
```

---

## Usage Example

### Down-Frequency Mode (Reduced Power)

```c
#include "ci130x_lowpower.h"

// Enter down-frequency mode to save power during idle
void enter_lowpower_idle(void)
{
    power_mode_switch(POWER_MODE_DOWN_FREQUENCY);
}

// Return to normal mode
void exit_lowpower_idle(void)
{
    power_mode_switch(POWER_MODE_NORMAL);
}
```

### Register Low Power Callbacks

```c
// Called when entering low power mode
void my_enter_lowpower(void)
{
    // Save peripheral state, disable unused clocks, etc.
}

// Called when exiting low power mode
void my_exit_lowpower(void)
{
    // Restore peripheral state, re-enable clocks, etc.
}

void setup_lowpower_callbacks(void)
{
    register_lowpower_user_fn(my_enter_lowpower, my_exit_lowpower);
}
```

### Configure Wakeup Source for Sleep

```c
#include "ci130x_dpmu.h"
#include "ci130x_lowpower.h"

void enter_sleep_with_gpio_wakeup(void)
{
    // Enable GPIO wakeup
    dpmu_set_wakeup_int(DPMU_WAKEUP_GPIO, ENABLE);
    dpmu_wakeup_reset_cfg(DPMU_WAKEUP_GPIO, ENABLE);

    // Halt IWDG during sleep (optional)
    dpmu_set_iwdg_halt();

    // Set DPMU low power mode
    dpmu_set_low_power_mode(DPMU_LOWPOWER_SLEEP_MODE);

    // Enter sleep
    power_mode_switch(POWER_MODE_SLEEP);
}

// After wakeup, check wakeup source
void check_wakeup_source(void)
{
    uint32_t state = dpmu_get_wakeup_state();
    // state corresponds to Dpmu_Wakeup_Reset_Cfg_t values

    // Resume IWDG
    dpmu_clean_iwdg_halt();

    // Return to normal mode
    power_mode_switch(POWER_MODE_NORMAL);
}
```

### Switch Clock Source

```c
// Use internal RC oscillator (lower power, less accurate)
void use_internal_rc(void)
{
    dpmu_set_src_source(DPMU_SRC_USE_INNER_RC);
    dpmu_use_rc();
}

// Use external crystal oscillator (higher accuracy)
void use_external_osc(void)
{
    dpmu_set_src_source(DPMU_SRC_USE_OUTSIDE_OSC);
}
```

---

## Notes/Tips

- **Sleep and deep-sleep modes** (`POWER_MODE_SLEEP`, `POWER_MODE_DEEP_SLEEP`) are marked as "not used" (`POWER_MODE_SLEEP_EN=0`, `POWER_MODE_DEEP_SLEEP_EN=0`) in the default header. Enable them in `ci130x_lowpower.h` if needed.
- The SDK typically uses `POWER_MODE_DOWN_FREQUENCY` for idle power saving (e.g., during exit-wakeup timeout), which reduces the main clock frequency.
- `register_lowpower_user_fn()` lets you hook into the mode switch to save/restore peripheral state.
- After waking from sleep, always call `dpmu_clean_iwdg_halt()` if you halted the IWDG, and restore the clock source if changed.
- The AON timers (AON_TIMER0/1) and AON GPIO can wake the system from sleep mode.
- `dpmu_get_wakeup_state()` tells you which source triggered the wakeup, useful for debugging unexpected resets.
- For battery-powered applications, use the internal RC oscillator (`DPMU_SRC_USE_INNER_RC`) for lower power at the cost of timing accuracy.
- LDO3 can be disabled if not used (`dpmu_ldo3_en(false)`) to save power, as seen in the CI13XX SDK `main.c`.
