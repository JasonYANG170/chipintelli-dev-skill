# Recipe: IR Remote Control (CI13XX)

> SDK: `CI13XX_SDK_ASR_ALG_V2.7.12`
> Chips: CI1306, CI1311, CI1312, CI1316, CI1324, CI1332, CI2312

> Evidence: `chips/ci13xx/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13xx/recipes/ir_control.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

The CI13XX SDK includes an IR (infrared) remote driver component (`ir_remote_driver`) for sending and receiving IR remote control codes. The driver uses PWM for IR carrier generation and a GPIO input with timer-based measurement for IR code reception. This is commonly used in voice-controlled appliances (e.g., air conditioners) where voice commands trigger IR code transmission.

---

## SDK

`CI13XX_SDK_ASR_ALG_V2.7.12` - component: `ir_remote_driver`

---

## Source Anchors

- `CI13XX_SDK_ASR_ALG_V2.7.12/components/ir_remote_driver/ir_remote_driver.h` - IR driver API and pin configuration
- `CI13XX_SDK_ASR_ALG_V2.7.12/components/ir_remote_driver/ir_remote_driver.c` - implementation

---

## API Usage

### IR Pin Configuration Structures

```c
// IR output (send) pin info
typedef struct {
    PinPad_Name PinName;        // Pin name (e.g., PA2)
    gpio_base_t GpioBase;       // GPIO base (e.g., PA)
    gpio_pin_t  PinNum;         // Pin number (e.g., pin_2)
    IOResue_FUNCTION PwmFun;   // PWM function for carrier
    IOResue_FUNCTION IoFun;    // GPIO function
    pwm_base_t PwmBase;        // PWM controller (e.g., PWM0)
} stIrOutIoInfo;

// IR receive pin info
typedef struct {
    PinPad_Name PinName;
    gpio_base_t GpioBase;
    gpio_pin_t  PinNum;
    IOResue_FUNCTION IoFun;    // GPIO function
    IRQn_Type   GpioIRQ;       // GPIO interrupt number
} stIrRevIoInfo;

// IR timer info
typedef struct {
    timer_base_t  ir_use_timer;      // Timer controller (e.g., TIMER0)
    IRQn_Type     ir_use_timer_IRQ;   // Timer interrupt number
} stIrTimerInfo;

// Complete IR pin configuration
typedef struct {
    stIrOutIoInfo  outPin;    // Send pin config
    stIrRevIoInfo  revPin;    // Receive pin config
    stIrTimerInfo  irTimer;   // Timer config
} stIrPinInfo;
```

### IR Event and State

```c
// IR events
typedef enum {
    IR_IDEL = 0,               // Idle
    IR_SEND_START,             // Send started
    IR_SEND_END,               // Send ended
    IR_RECEIVE_START,          // Receive started
    IR_RECEIVE_END,            // Receive ended
    IR_EVENT_ERR        = -1,  // Error
    IR_SEND_DATA_ERR    = -2,  // Send data error
    IR_RECEIVE_SHORT_ERR = -3, // Receive data too short
} IrRemoteEvent;

// IR state
typedef struct {
    bool is_busy;              // Busy flag
    IrRemoteEvent event;       // Current event
} IrRemoteState;

// Callback type
typedef void (*ir_remote_event_callback_t)(IrRemoteState *state);
```

### Default Pin Configuration

```c
// Output (send) pin: PA2 with PWM0
#define IR_OUT_PWM_PIN_NAME       PA2
#define IR_OUT_GPIO_PIN_BASE      PA
#define IR_OUT_PWM_NAME           PWM0
#define IR_OUT_PWM_PIN_NUMBER     pin_2
#define IR_OUT_PWM_FUNCTION       FIFTH_FUNCTION
#define IR_OUT_GPIO_FUNCTION      FIRST_FUNCTION

// Input (receive) pin: PA4
#define IR_REV_IO_PIN_NAME        PA4
#define IR_REV_IO_PIN_BASE        PA
#define IR_REV_IO_PIN_NUMBER      pin_4
#define IR_REV_IO_IRQ             PA_IRQn
#define IR_REV_IO_FUNCTION        FIRST_FUNCTION

// Timer: TIMER0
#define IR_USED_TIMER_NAME        TIMER0
#define IR_USED_TIMER_IRQ         TIMER0_IRQn
```

### Initialization Functions

```c
// Initialize IR hardware (PWM, GPIO, timer, interrupts)
int32_t ir_hw_init(void);

// Set custom IR pin configuration
int32_t ir_setPinInfo(stIrPinInfo* pIrPinInfo);

// Set IR level code data address (from flash)
// addr: flash address of IR code data
// size: data size
int32_t set_ir_level_code_addr(uint32_t addr, uint32_t size);

// Set IR level code for testing
int32_t set_ir_level_code_addr_for_test(uint32_t addr, uint32_t size);
```

### Send Functions

```c
// Initialize IR send (configures PWM for 38kHz carrier)
void ir_send_init(void);

// Start sending IR code
// count: IR code index to send
int32_t send_ir_code_start(uint32_t count);
```

### Receive Functions

```c
// Start IR receive with timeout
// time_out: timeout value
void ir_receive_start(int time_out);

// Check if IR receive is complete
// Returns: 1=received, 0=not yet
int32_t check_ir_receive(void);

// End IR receive
void ir_receive_end(void);
```

### State Query Functions

```c
// Get IR level code address
uint16_t *get_ir_level_code_addr(void);

// Get IR driver buffer
uint16_t * get_ir_driver_buf(void);

// Check if IR is busy (sending or receiving)
int32_t check_ir_busy_state(void);

// Get received level count
uint32_t get_receive_level_count(void);

// Set received level count
void set_receive_level_count(uint32_t level_cnt);
```

### Carrier Configuration

```c
// Set odd or even level carrier PWM wave
// odd_even: 0=even, 1=odd
int32_t set_odd_even_carry_pwm_wave(int odd_even);
```

### Callback Registration

```c
// Register IR event callback
void registe_ir_remote_callback(ir_remote_event_callback_t ir_callback);

// Unregister IR event callback
void unregiste_ir_remote_callback(void);
```

---

## Usage Example

### Initialize IR Driver

```c
#include "ir_remote_driver.h"

void ir_init(void)
{
    // Initialize with default pin configuration
    ir_hw_init();

    // Set IR code data from flash
    // IR code data is stored in flash, typically at a known address
    // Refer to external/firmware_Reference/ir/ for IR data files
    set_ir_level_code_addr(IR_DATA_FLASH_ADDR, IR_DATA_SIZE);
}
```

### Send IR Code (e.g., from Voice Command)

```c
// Callback for IR events
void ir_event_callback(IrRemoteState *state)
{
    switch (state->event)
    {
        case IR_SEND_END:
            // IR code sent successfully
            break;
        case IR_SEND_DATA_ERR:
            // Error sending IR code
            break;
        case IR_RECEIVE_END:
            // IR code received
            break;
        default:
            break;
    }
}

void ir_setup_callback(void)
{
    registe_ir_remote_callback(ir_event_callback);
}

// Send IR code by index (e.g., power on/off for AC)
void send_ir_by_voice_command(uint32_t ir_code_index)
{
    // Check if IR is busy
    if (check_ir_busy_state())
    {
        return;  // Busy, skip
    }

    // Start sending IR code
    send_ir_code_start(ir_code_index);
}
```

### Receive IR Code

```c
void ir_receive_demo(void)
{
    // Start receiving with timeout
    ir_receive_start(1000);  // timeout in ms

    // Poll for completion
    while (!check_ir_receive())
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Get received level count
    uint32_t level_count = get_receive_level_count();

    // Process received IR data
    // ...

    // End receive
    ir_receive_end();
}
```

### Integration with ASR (Voice -> IR)

```c
// In user_msg_deal.c - deal_asr_msg_by_cmd_id():
uint32_t deal_asr_msg_by_cmd_id(sys_msg_asr_data_t *asr_msg,
                                 cmd_handle_t cmd_handle,
                                 uint16_t cmd_id)
{
    uint32_t ret = 1;

    switch (cmd_id)
    {
        case 2:  // "Turn on AC"
            send_ir_by_voice_command(0);  // IR code index 0: power on
            break;

        case 3:  // "Turn off AC"
            send_ir_by_voice_command(1);  // IR code index 1: power off
            break;

        case 10: // "Cooling mode"
            send_ir_by_voice_command(5);  // IR code index 5: cooling
            break;

        default:
            ret = 0;
            break;
    }

    return ret;
}
```

### Custom Pin Configuration

```c
void ir_custom_pin_init(void)
{
    stIrPinInfo pin_info;

    // Output pin: PA2, PWM0, fifth function
    pin_info.outPin.PinName  = PA2;
    pin_info.outPin.GpioBase = PA;
    pin_info.outPin.PinNum   = pin_2;
    pin_info.outPin.PwmFun   = FIFTH_FUNCTION;
    pin_info.outPin.IoFun    = FIRST_FUNCTION;
    pin_info.outPin.PwmBase  = PWM0;

    // Receive pin: PA4, first function
    pin_info.revPin.PinName  = PA4;
    pin_info.revPin.GpioBase = PA;
    pin_info.revPin.PinNum   = pin_4;
    pin_info.revPin.IoFun    = FIRST_FUNCTION;
    pin_info.revPin.GpioIRQ  = PA_IRQn;

    // Timer: TIMER0
    pin_info.irTimer.ir_use_timer     = TIMER0;
    pin_info.irTimer.ir_use_timer_IRQ = TIMER0_IRQn;

    // Apply custom configuration
    ir_setPinInfo(&pin_info);
    ir_hw_init();
}
```

---

## Notes/Tips

- **Enable IR in user_config.h**: Set `USE_IR_ENABLE=1` to enable the IR feature.
- **IR data files**: IR code data is stored in flash as binary files. Refer to `external/firmware_Reference/ir/firmware/user_file/[0]ir_data_*.bin` for sample data.
- **Packaging**: When using IR, uncheck "auto-run after upgrade" in the packaging tool to avoid re-flashing IR data.
- **PWM carrier**: The IR send uses PWM at 38kHz (standard IR remote frequency) with 33% duty cycle. The PWM is configured by `ir_send_init()`.
- **Timer conflict**: The IR driver uses TIMER0 by default. If TIMER0 is used elsewhere, change `IR_USED_TIMER_NAME` and update the pin info.
- **GPIO conflict**: The IR receive pin (PA4) uses the GPIOA interrupt. Other GPIOA interrupts may interfere; ensure proper interrupt handling.
- **IR code library**: The IR code data format stores timing levels (mark/space durations) as uint16_t values. The driver buffer (`get_ir_driver_buf()`) holds the parsed levels.
- **Busy state**: Always check `check_ir_busy_state()` before starting a new send or receive operation.
- **Multiple IR codes**: Use `send_ir_code_start(index)` with different indices to send various IR codes (power, mode, temperature, etc.).
- **Project reference**: See `projects/offline_asr_alg_pro_sample/app/app_ir/` for a complete IR application example including air conditioner control.
