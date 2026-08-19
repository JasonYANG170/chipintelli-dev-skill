# Recipe: Watchdog (IWDG)

> SDK: `CI130X_SDK_Offline_V2.1.14`
> Chips: CI1302, CI1303, CI1306, CI1312

> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/watchdog.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

The CI130X includes an Independent Watchdog (IWDG) that can reset the system if the software fails to feed it within the configured timeout. The IWDG runs on a dedicated clock (SRC clock / 16) and continues running in low-power modes.

---

## SDK

`CI130X_SDK_Offline_V2.1.14` - driver: `ci130x_chip_driver`

---

## Source Anchors

- `ci130x_sdk/driver/ci130x_chip_driver/inc/ci130x_iwdg.h`
- `ci130x_sdk/driver/ci130x_chip_driver/inc/ci130x_dpmu.h` (for reset configuration)

---

## API Usage

### Controller

```c
typedef enum {
    IWDG = HAL_IWDG_BASE,
} iwdg_base_t;
```

### Interrupt Enable

```c
typedef enum {
    iwdg_irqen_enable  = 1,  // Enable IWDG interrupt
    iwdg_irqen_disable = 0,  // Disable IWDG interrupt
} iwdg_irqen_t;
```

### Reset Enable

```c
typedef enum {
    iwdg_resen_enable  = 1,  // Enable IWDG system reset
    iwdg_resen_disable = 0,  // Disable IWDG system reset
} iwdg_resen_t;
```

### Configuration Structure

```c
typedef struct {
    unsigned int  count;  // Count value (timeout = count * 16 / src_clk)
    iwdg_irqen_t  irq;    // Interrupt enable
    iwdg_resen_t  res;    // Reset enable
} iwdg_init_t;
```

### Functions

```c
// Initialize IWDG with configuration
void iwdg_init(iwdg_base_t base, iwdg_init_t init);

// Open (start) the IWDG
void iwdg_open(iwdg_base_t base);

// Close (stop) the IWDG
void iwdg_close(iwdg_base_t base);

// Feed the IWDG (reset the counter)
void iwdg_feed(iwdg_base_t base);

// IWDG interrupt handler
void iwdg_irqhander(void);
```

### DPMU Reset Configuration (from ci130x_dpmu.h)

```c
// Configure IWDG reset behavior
void dpmu_iwdg_reset_system_config(void);  // IWDG reset triggers system reset
void dpmu_iwdg_reset_bus_config(void);     // IWDG reset triggers bus reset
void dpmu_iwdg_reset_none_config(void);    // IWDG reset disabled

// Halt IWDG during low-power modes
void dpmu_set_iwdg_halt(void);    // Halt IWDG in sleep/deep-sleep
void dpmu_clean_iwdg_halt(void); // Resume IWDG after sleep
```

---

## Usage Example

### Standard Watchdog Setup

```c
#include "ci130x_iwdg.h"
#include "ci130x_dpmu.h"
#include "ci130x_scu.h"

void watchdog_init(void)
{
    // Enable IWDG clock gate
    scu_set_device_gate(IWDG, ENABLE);

    // Configure IWDG to trigger system reset on timeout
    dpmu_iwdg_reset_system_config();

    iwdg_init_t init;
    init.irq   = iwdg_irqen_enable;    // Enable interrupt (fires before reset)
    init.res   = iwdg_resen_enable;   // Enable system reset
    // Timeout = count * 16 / src_clk
    // e.g., src_clk = 12288000 Hz => count = (12288000/16) * 3 = 2304000 => 3s
    init.count = ((get_src_clk() / 0x10) * 3);

    iwdg_init(IWDG, init);
    iwdg_open(IWDG);
}
```

### Feed in Main Loop

```c
// In your main task or FreeRTOS task loop:
void user_task_loop(void)
{
    while (1)
    {
        // ... do work ...

        // Feed the watchdog to prevent reset
        iwdg_feed(IWDG);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

### IWDG Interrupt Handler

```c
#include "ci130x_core_eclic.h"

// Register in ci130x_it.c or user code
__attribute__((interrupt())) void IWDG_IRQHandler(void)
{
    // This fires shortly before the reset triggers
    // Can log critical state or attempt recovery
    // After this, the system will reset if not fed
    iwdg_irqhander();
}
```

### Pattern from CI13XX SDK main.c

```c
// As seen in CI13XX projects (same driver API):
iwdg_init_t init;
init.irq = iwdg_irqen_enable;
init.res = iwdg_resen_enable;
init.count = ((get_src_clk() / 0x10) * 3);  // ~3 seconds
scu_set_device_gate(IWDG, ENABLE);
dpmu_iwdg_reset_system_config();
iwdg_init(IWDG, init);
iwdg_open(IWDG);
```

---

## Notes/Tips

- The IWDG clock is derived from SRC clock divided by 16. Timeout = `count * 16 / src_clk` seconds.
- With `irq=enable`, the interrupt fires at roughly 50% of the timeout to give the system a chance to respond before the actual reset.
- Call `iwdg_feed(IWDG)` regularly from your main loop. A good practice is to feed every 1/3 of the timeout period.
- `dpmu_set_iwdg_halt()` can pause the IWDG during deep sleep; call `dpmu_clean_iwdg_halt()` when waking up.
- If the IWDG triggers a system reset, the system reboots. Check the reset cause in `dpmu_get_wakeup_state()` if you need to distinguish IWDG reset from power-on.
- Do not feed the IWDG from a high-priority ISR that may be starved; feed from a reliable task.
- The IWDG cannot be stopped once opened (hardware limitation); `iwdg_close()` may not fully disable it on all chip revisions.
