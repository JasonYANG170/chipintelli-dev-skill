# IR Remote Control with CI130X

## Overview

The CI130X SDK IR variant (`CI130X_SDK_Offline_IR_V2.0.10`) adds infrared remote control capabilities to the voice recognition system.

## SDK

Use `CI130X_SDK_Offline_IR_V2.0.10`.

## Features

- **IR Learn**: Learn IR codes from existing remote controls
- **IR Send**: Transmit IR codes to control appliances (AC, TV, etc.)
- **Voice-triggered IR**: Use voice commands to send IR codes
- **IR code storage**: Store learned codes in flash

## Configuration

IR control is enabled by default in the IR SDK variant. The IR driver header is at `components/ir_remote_driver/ir_remote_driver.h`.

### IR Hardware Pins (from `ir_remote_driver.h` defaults)

| Function | Pin | Config |
|----------|-----|--------|
| IR output (PWM) | PA2 | PWM0, 5th function |
| IR receive (GPIO) | PA4 | 1st function |
| IR timer | TIMER0 | TIMER0_IRQn |

### IR Driver API (from `ir_remote_driver.h`)

```c
#include "ir_remote_driver.h"

// Initialize IR hardware (call once in userapp_initial)
int32_t ir_hw_init(void);

// Set IR level code storage address in flash
int32_t set_ir_level_code_addr(uint32_t addr, uint32_t size);

// Send: load IR code from flash and transmit
void ir_send_init(void);
int32_t send_ir_code_start(uint32_t count);

// Receive: start learning mode with timeout
void ir_receive_start(int time_out);
int32_t check_ir_receive(void);
void ir_receive_end(void);

// State queries
int32_t check_ir_busy_state(void);
uint32_t get_receive_level_count(void);

// Event callback registration
void registe_ir_remote_callback(ir_remote_event_callback_t ir_callback);
void unregiste_ir_remote_callback(void);
```

### IR Event States (from `ir_remote_driver.h`)

```c
typedef enum {
    IR_IDEL = 0,               // Idle
    IR_SEND_START,             // Send started
    IR_SEND_END,               // Send ended
    IR_RECEIVE_START,          // Receive started
    IR_RECEIVE_END,            // Receive ended
    IR_EVENT_ERR = -1,         // Error
    IR_SEND_DATA_ERR = -2,     // Send data error
    IR_RECEIVE_SHORT_ERR = -3, // Receive too short
} IrRemoteEvent;
```

## Voice Integration

IR commands are triggered by voice through the ASR message handler. In `user_msg_deal.c`:

```c
#include "ir_remote_driver.h"

// Example: send IR code when voice command recognized
uint32_t deal_asr_msg_by_cmd_id(sys_msg_asr_data_t *asr_msg,
                                 cmd_handle_t cmd_handle, uint16_t cmd_id)
{
    switch (cmd_id)
    {
    case 100: // "Turn on AC" -> send AC IR code
        // Load IR code from flash and send
        ir_send_init();
        send_ir_code_start(0);  // index 0 = first stored IR code
        break;
    // ... more cases
    }
    return 0;
}
```

For the IR learning flow, use `ir_receive_start()` to enter learning mode, then check status via `check_ir_receive()` or via a registered callback (`registe_ir_remote_callback`).

## Typical Application Flow

1. User says "Learn IR code" -> Device enters IR learning mode
2. User points original remote at device and presses button
3. Device learns and stores the IR code
4. User says "Turn on AC" -> Device sends learned AC IR code

## Hardware Requirements

- IR LED (transmitter)
- IR receiver (e.g., HS0038 or equivalent)
- Connected to CI130X GPIO pins (defined in board file)
