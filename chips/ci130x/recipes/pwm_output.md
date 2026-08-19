# Recipe: PWM Output

> SDK: `CI130X_SDK_Offline_V2.1.14`
> Chips: CI1302, CI1303, CI1306, CI1312

---

## Overview

The CI130X provides 6 independent PWM controllers (PWM0-PWM5). Each can generate a configurable frequency and duty cycle waveform. PWM4 and PWM5 are shared with the AON (Always-On) timer module.

---

## SDK

`CI130X_SDK_Offline_V2.1.14` - driver: `ci130x_chip_driver`

---

## Source Anchors

- `ci130x_sdk/driver/ci130x_chip_driver/inc/ci130x_pwm.h`

---

## API Usage

### Controller Selection

```c
typedef enum {
    PWM0 = HAL_PWM0_BASE,
    PWM1 = HAL_PWM1_BASE,
    PWM2 = HAL_PWM2_BASE,
    PWM3 = HAL_PWM3_BASE,
    PWM4 = HAL_PWM4_BASE,
    PWM5 = HAL_PWM5_BASE,
} pwm_base_t;
```

### Configuration Structure

```c
typedef struct {
    unsigned int clk_sel;   // 0:PCLK, 1:SRC clock
    unsigned int freq;      // PWM frequency in Hz
    unsigned int duty;      // PWM duty cycle
    unsigned int duty_max;  // Maximum duty value
} pwm_init_t;
```

### Functions

```c
// Initialize PWM with given parameters
void pwm_init(pwm_base_t base, pwm_init_t init);

// Start PWM output
void pwm_start(pwm_base_t base);

// Stop PWM output
void pwm_stop(pwm_base_t base);

// Set duty cycle dynamically
void pwm_set_duty(pwm_base_t base, unsigned int duty, unsigned int duty_max);

// Set restart mode (1=restart counter on duty update)
void pwm_set_restart_md(pwm_base_t base, uint8_t cmd);
```

---

## Usage Example

```c
#include "ci130x_pwm.h"
#include "ci130x_scu.h"

// Initialize PWM0 for a 1kHz signal at 50% duty
void pwm_demo_init(void)
{
    // Enable PWM0 clock gate
    scu_set_device_gate(PWM0, ENABLE);

    pwm_init_t init;
    init.clk_sel  = 0;       // Use PCLK
    init.freq     = 1000;    // 1 kHz
    init.duty     = 50;      // 50% duty
    init.duty_max = 100;     // Duty range 0-100

    pwm_init(PWM0, init);
    pwm_start(PWM0);
}

// Change duty cycle at runtime (e.g., for LED dimming)
void pwm_set_brightness(uint8_t percent)
{
    if (percent > 100) percent = 100;
    pwm_set_duty(PWM0, percent, 100);
}

// Stop PWM output
void pwm_demo_stop(void)
{
    pwm_stop(PWM0);
}
```

### Servo Control Example

```c
// RC servo: 50Hz, 1ms-2ms pulse (5%-10% of 20ms period)
void servo_set_angle(pwm_base_t base, uint8_t angle)
{
    // angle: 0-180 degrees
    // Map to duty: 5% (0 deg) to 10% (180 deg)
    unsigned int duty = 500 + (angle * 555) / 180;  // 5000-10000 range
    pwm_set_duty(base, duty, 10000);
}

void servo_init(pwm_base_t base)
{
    scu_set_device_gate(base, ENABLE);
    pwm_init_t init = {
        .clk_sel  = 0,
        .freq     = 50,       // 50 Hz for servo
        .duty     = 7500,     // ~7.5% = center (1.5ms)
        .duty_max = 10000,
    };
    pwm_init(base, init);
    pwm_start(base);
}
```

---

## Notes/Tips

- Call `scu_set_device_gate(PWMx, ENABLE)` before using any PWM controller.
- PWM4 and PWM5 overlap with AON_TIMER0 and AON_TIMER1; do not use both simultaneously.
- `duty_max` sets the resolution of duty control. Use 100 for percentage or 1000 for finer control.
- For IR carrier signals, PWM is typically configured at 38kHz with 33% duty.
- `pwm_set_restart_md(base, 1)` ensures the counter restarts immediately when duty is updated, useful for smooth transitions.
