# Recipe: GPIO Control

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> Header: `ci130x_gpio.h`, `ci130x_scu.h`, `ci130x_dpmu.h`

> Applies to: CI1301, CI1302, CI1303, and CI1306 unless the recipe states narrower support; confirm exact chip, board, and voice/connectivity feature set before coding.
> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/gpio_control.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

CI130X has 4 GPIO ports (PA, PB, PC, PD) with up to 8 pins each. Pins are shared with other peripheral functions via pin multiplexing configured through DPMU.

### GPIO Port Map

| Port | Pins | Notes |
|------|------|-------|
| PA | PA0-PA7 | PA0/PA1 shared with OSC; PA2-PA6 shared with IIS0/IIC0/UART/PWM |
| PB | PB0-PB7 | Shared with PWM/UART/IIC |
| PC | PC0-PC5 | PC1-PC4 shared with ADC (AIN2-AIN5) |
| PD | PD0-PD5 | General purpose |

---

## Pin Configuration

Before using a pin as GPIO, configure its multiplex function to GPIO (FIRST_FUNCTION) and set it to digital mode:

```c
#include "ci130x_gpio.h"
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"

// Configure PA2 as GPIO
dpmu_set_io_reuse(PA2, FIRST_FUNCTION);  // Select GPIO function
dpmu_set_adio_reuse(PA2, DIGITAL_MODE);  // Digital mode (not analog)
```

---

## GPIO Output

### Basic Output Control

```c
// Step 1: Enable GPIO clock and reset
scu_set_device_gate(HAL_GPIOA_BASE, ENABLE);
scu_set_device_reset(HAL_GPIOA_BASE);
scu_set_device_reset_release(HAL_GPIOA_BASE);

// Step 2: Configure pin as GPIO
dpmu_set_io_reuse(PA2, FIRST_FUNCTION);
dpmu_set_adio_reuse(PA2, DIGITAL_MODE);

// Step 3: Set as output mode
gpio_set_output_mode(PA, pin_2);

// Step 4: Control output level
gpio_set_output_high_level(PA, pin_2);  // Set high
gpio_set_output_low_level(PA, pin_2);   // Set low

// Or use single-pin API with level parameter
gpio_set_output_level_single(PA, pin_2, 1);  // High
gpio_set_output_level_single(PA, pin_2, 0);  // Low
```

### Toggle Example

```c
void led_toggle(void)
{
    static uint8_t state = 0;
    if (state)
    {
        gpio_set_output_high_level(PA, pin_2);
        state = 0;
    }
    else
    {
        gpio_set_output_low_level(PA, pin_2);
        state = 1;
    }
}
```

### Multiple Pins Simultaneously

```c
// Set PA0 and PA1 as output
gpio_set_output_mode(PA, pin_0 | pin_1);

// Set both high at once
gpio_set_output_high_level(PA, pin_0 | pin_1);
```

---

## GPIO Input

### Basic Input Read

```c
// Configure pin
scu_set_device_gate(HAL_GPIOA_BASE, ENABLE);
scu_set_device_reset(HAL_GPIOA_BASE);
scu_set_device_reset_release(HAL_GPIOA_BASE);

dpmu_set_io_reuse(PA3, FIRST_FUNCTION);
dpmu_set_adio_reuse(PA3, DIGITAL_MODE);

// Set as input
gpio_set_input_mode(PA, pin_3);

// Optional: Enable pull-up
dpmu_set_io_pull(PA3, DPMU_IO_PULL_UP);

// Read input level
uint8_t level = gpio_get_input_level_single(PA, pin_3);
if (level)
{
    // Pin is high
}
else
{
    // Pin is low
}
```

### Polling Example

```c
// Poll a button on PA3 (active low with pull-up)
bool is_button_pressed(void)
{
    return (gpio_get_input_level_single(PA, pin_3) == 0);
}

void button_poll_task(void *p_arg)
{
    while (1)
    {
        if (is_button_pressed())
        {
            vTaskDelay(pdMS_TO_TICKS(20));  // Debounce
            if (is_button_pressed())
            {
                // Button confirmed pressed
                handle_button_press();
                while (is_button_pressed())
                {
                    vTaskDelay(pdMS_TO_TICKS(10));  // Wait for release
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

---

## GPIO Interrupt

### Interrupt Setup

```c
#include "ci130x_gpio.h"

// Define callback function
static void my_gpio_isr_callback(void)
{
    // Check which pin triggered the interrupt
    if (gpio_get_irq_raw_status_single(PA, pin_3))
    {
        // Clear interrupt flag
        gpio_clear_irq_single(PA, pin_3);

        // Handle interrupt
        // NOTE: This runs in interrupt context - keep it short!
        // Use xQueueSendFromISR or xTimerStartFromISR to defer work
    }
}

// Define callback node (must persist in memory)
static gpio_irq_callback_list_t my_callback_node = {
    .gpio_irq_callback = my_gpio_isr_callback,
    .next = NULL,
};

void gpio_interrupt_init(void)
{
    // Enable GPIO clock
    scu_set_device_gate(HAL_GPIOA_BASE, ENABLE);
    scu_set_device_reset(HAL_GPIOA_BASE);
    scu_set_device_reset_release(HAL_GPIOA_BASE);

    // Configure pin
    dpmu_set_io_reuse(PA3, FIRST_FUNCTION);
    dpmu_set_adio_reuse(PA3, DIGITAL_MODE);

    // Set as input
    gpio_set_input_mode(PA, pin_3);

    // Enable pull-up (for active-low button)
    dpmu_set_io_pull(PA3, DPMU_IO_PULL_UP);

    // Configure trigger mode
    gpio_irq_trigger_config(PA, pin_3, both_edges_trigger);
    // Other options: high_level_trigger, low_level_trigger,
    //                up_edges_trigger, down_edges_trigger

    // Unmask interrupt
    gpio_irq_unmask(PA, pin_3);

    // Register callback
    registe_gpio_callback(PA, &my_callback_node);
}
```

### Deferred Processing from ISR

```c
// Use a timer to defer processing from ISR to task context
static TimerHandle_t debounce_timer = NULL;

static void my_gpio_isr_callback(void)
{
    if (gpio_get_irq_raw_status_single(PA, pin_3))
    {
        gpio_clear_irq_single(PA, pin_3);
        // Start debounce timer from ISR
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerStartFromISR(debounce_timer, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void debounce_timer_callback(TimerHandle_t xTimer)
{
    // Now in timer task context - safe to do real work
    uint8_t level = gpio_get_input_level_single(PA, pin_3);
    if (level == 0)
    {
        // Button pressed (active low)
        send_msg_to_sys_task(&button_msg, NULL);
    }
}

void gpio_interrupt_init(void)
{
    // Create debounce timer
    debounce_timer = xTimerCreate("debounce", pdMS_TO_TICKS(20),
                                  pdFALSE, 0, debounce_timer_callback);

    // ... rest of init (same as above)
}
```

---

## Pin Multiplexing Reference

Key pin function assignments (from `ci130x_scu.h` `PinPad_Name` enum):

| Pad | 1st Func | 2nd Func | 3rd Func | 4th Func | 5th Func |
|-----|----------|----------|----------|----------|----------|
| PA2 | PA_2 | IIS0_SDI | IIC0_SDA | UART1_TX | PWM0 |
| PA3 | PA_3 | IIS0_LRCLK | IIC0_SCL | UART1_RX | PWM1 |
| PA4 | PA_4 | IIS0_SDO | - | - | PWM2 |
| PA5 | PA_5 | IIS0_SCLK | PDM_DAT | UART2_TX | PWM3 |
| PA6 | PA_6 | IIS0_MCLK | PDM_CLK | UART2_RX | PWM4 |
| PA7 | PA_7 | PWM0 | UART1_TX | EXT_INT0 | - |
| PB0 | PB_0 | PWM1 | UART1_RX | EXT_INT1 | - |
| PB5 | PB_5 | UART0_TX | IIC0_SDA | PWM1 | - |
| PB6 | PB_6 | UART0_RX | IIC0_SCL | PWM2 | UART2_TX |
| PB7 | PB_7 | UART1_TX | IIC0_SDA | PWM3 | PDM_DAT |
| PC0 | PC_0 | UART1_RX | IIC0_SCL | PWM4 | PDM_CLK |
| PC1 | PC_1 | - | UART2_TX | PWM3 | PDM_DAT |
| PC2 | PC_2 | - | UART2_RX | PWM2 | PDM_CLK |
| PC3 | PC_3 | IIC0_SDA | PWM1 | PDM_DAT | - |
| PC4 | PC_4 | IIC0_SCL | PWM0 | PDM_CLK | - |

> **Note**: PA0 and PA1 are shared with the oscillator. Use `dpmu_osc_pad_for_gpio(ENABLE)` to use them as GPIO.

---

## GPIO in user_msg_deal.c

The recommended place for GPIO initialization is `userapp_initial()` in `user_msg_deal.c`:

```c
void userapp_initial(void)
{
    #if MSG_COM_USE_UART_EN
    vmup_communicate_init();
    #endif

    ///tag-gpio-init
    // LED output
    scu_set_device_gate(HAL_GPIOA_BASE, ENABLE);
    scu_set_device_reset(HAL_GPIOA_BASE);
    scu_set_device_reset_release(HAL_GPIOA_BASE);

    dpmu_set_io_reuse(PA2, FIRST_FUNCTION);
    dpmu_set_adio_reuse(PA2, DIGITAL_MODE);
    gpio_set_output_mode(PA, pin_2);
    gpio_set_output_low_level(PA, pin_2);  // LED off initially

    // Button input with interrupt
    dpmu_set_io_reuse(PA3, FIRST_FUNCTION);
    dpmu_set_adio_reuse(PA3, DIGITAL_MODE);
    gpio_set_input_mode(PA, pin_3);
    dpmu_set_io_pull(PA3, DPMU_IO_PULL_UP);
    gpio_irq_trigger_config(PA, pin_3, down_edges_trigger);
    gpio_irq_unmask(PA, pin_3);
    registe_gpio_callback(PA, &my_callback_node);
}
```

---

## Drive Strength and Slew Rate

For high-speed or high-drive applications:

```c
// Set drive strength (0=weakest, 3=strongest)
dpmu_set_io_driver_strength(PA2, DPMU_IO_DRIVER_STRENGTH_3);

// Set slew rate
dpmu_set_io_slew_rate(PA2, DPMU_IO_SLEW_RATE_FAST);

// Enable Schmitt trigger for noisy inputs
dpmu_set_io_schmitt_trigger(PA3, DPMU_IO_SCHMITT_TRIGGER_ENABLE);
```
