# Recipe: Timer

> SDK: `CI130X_SDK_Offline_V2.1.14`
> Chips: CI1302, CI1303, CI1306, CI1312

> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/timer.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

The CI130X provides 4 standard timers (TIMER0-TIMER3) and 2 always-on timers (AON_TIMER0, AON_TIMER1). Timers support single-cycle, auto-reload, free-running, and event-counting modes with configurable clock dividers.

---

## SDK

`CI130X_SDK_Offline_V2.1.14` - driver: `ci130x_chip_driver`

---

## Source Anchors

- `ci130x_sdk/driver/ci130x_chip_driver/inc/ci130x_timer.h`

---

## API Usage

### Controller Selection

```c
typedef enum {
    TIMER0     = HAL_TIMER0_BASE,
    TIMER1     = HAL_TIMER1_BASE,
    TIMER2     = HAL_TIMER2_BASE,
    TIMER3     = HAL_TIMER3_BASE,
    AON_TIMER0 = HAL_PWM4_BASE,   // Shared with PWM4
    AON_TIMER1 = HAL_PWM5_BASE,   // Shared with PWM5
} timer_base_t;
```

### Count Modes

```c
typedef enum {
    timer_count_mode_single = 0,  // Single cycle (one-shot)
    timer_count_mode_auto   = 1,  // Auto reload (periodic)
    timer_count_mode_free   = 2,  // Free running
    timer_count_mode_event  = 3,  // Event counting
} timer_count_mode_t;
```

### Clock Divider

```c
typedef enum {
    timer_clk_div_0  = 0,  // No division
    timer_clk_div_2  = 1,  // /2
    timer_clk_div_4  = 2,  // /4
    timer_clk_div_16 = 3,  // /16
} timer_clock_div_t;
```

### IRQ Signal Width

```c
typedef enum {
    timer_iqr_width_f = 0,  // Cleared by TIMER_CFG1[CT]
    timer_iqr_width_2 = 1,   // 2 clock cycles
    timer_iqr_width_4 = 2,   // 4 clock cycles
    timer_iqr_width_8 = 3,   // 8 clock cycles
} timer_iqr_width_t;
```

### Configuration Structure

```c
typedef struct {
    timer_count_mode_t mode;   // Count mode
    timer_clock_div_t  div;    // Clock divider
    timer_iqr_width_t  width;  // IRQ signal width
    unsigned int       count;  // Count value
} timer_init_t;
```

### Functions

```c
void timer_init(timer_base_t base, timer_init_t init);
void timer_set_mode(timer_base_t base, timer_count_mode_t mode);
void timer_start(timer_base_t base);
void timer_stop(timer_base_t base);
void timer_event_start(timer_base_t base);
void timer_set_count(timer_base_t base, unsigned int count);
void timer_get_count(timer_base_t base, unsigned int* count);
void timer_cascade_set(timer_base_t base, unsigned int count);
void timer_clear_irq(timer_base_t base);
```

---

## Usage Example

### Periodic Timer with Interrupt

```c
#include "ci130x_timer.h"
#include "ci130x_scu.h"
#include "ci130x_core_eclic.h"

// Timer IRQ handler (defined in ci130x_it.c or user code)
__attribute__((interrupt())) void TIMER0_IRQHandler(void)
{
    // Clear interrupt flag
    timer_clear_irq(TIMER0);
    // Perform periodic task
    // ...
}

void timer_periodic_init(void)
{
    // Enable TIMER0 clock
    scu_set_device_gate(TIMER0, ENABLE);

    timer_init_t init;
    init.mode  = timer_count_mode_auto;  // Periodic
    init.div   = timer_clk_div_16;       // /16 divider
    init.width = timer_iqr_width_4;      // 4-cycle IRQ pulse
    // Count value: src_clk / div / desired_freq
    // e.g., src_clk=12.288MHz, div=16, freq=1000Hz => count=768
    init.count = 768;

    timer_init(TIMER0, init);

    // Register interrupt
    eclic_irq_register(TIMER0_IRQn, TIMER0_IRQHandler);
    eclic_irq_enable(TIMER0_IRQn);

    timer_start(TIMER0);
}
```

### One-Shot Timer

```c
void timer_oneshot_init(uint32_t count_val)
{
    scu_set_device_gate(TIMER1, ENABLE);

    timer_init_t init;
    init.mode  = timer_count_mode_single;  // One-shot
    init.div   = timer_clk_div_0;           // No division
    init.width = timer_iqr_width_4;
    init.count = count_val;

    timer_init(TIMER1, init);
    timer_start(TIMER1);
}
```

### Read Current Count

```c
void timer_measure_elapsed(uint32_t *count)
{
    timer_get_count(TIMER2, count);
    // Elapsed time = count / (src_clk / div)
}
```

---

## Notes/Tips

- Call `scu_set_device_gate(TIMERx, ENABLE)` before using any timer.
- AON_TIMER0/1 overlap with PWM4/PWM5; cannot use both simultaneously.
- For periodic interrupts, use `timer_count_mode_auto`; the timer reloads `count` automatically after each expiry.
- Always call `timer_clear_irq(base)` inside the IRQ handler to clear the pending interrupt.
- Timer count value = `src_clk / divider / target_frequency`.
- The IR remote driver uses TIMER0 by default (see `ir_remote_driver.h`).
