# Recipe: UART Communication

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> Header: `ci130x_uart.h`, `ci130x_scu.h`, `ci130x_dpmu.h`

> Applies to: CI1301, CI1302, CI1303, and CI1306 unless the recipe states narrower support; confirm exact chip, board, and voice/connectivity feature set before coding.
> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/uart_comm.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

CI130X has 3 UART ports (UART0, UART1, UART2). They are used for:
- **Log output** (`CONFIG_CI_LOG_UART`)
- **Voice module protocol** (`UART_PROTOCOL_NUMBER`)
- **Custom serial communication**

### UART Pin Mapping

| UART | TX Pin Options | RX Pin Options |
|------|---------------|----------------|
| UART0 | PB5 (2nd func) | PB6 (2nd func) |
| UART1 | PA7 (2nd func) / PB7 (2nd func) | PB0 (2nd func) / PC0 (2nd func) |
| UART2 | PA5 (4th func) / PB1 (2nd func) / PB6 (4th func) | PA6 (4th func) / PB2 (2nd func) |

---

## Log UART Configuration

The log UART is configured in `user_config.h`:

```c
#define CONFIG_CI_LOG_UART  HAL_UART0_BASE  // Use UART0 for log output
```

### Log Initialization

Log UART is initialized automatically in `platform_init()` (in `main.c`):

```c
static int platform_init(void)
{
    #if CONFIG_CI_LOG_UART
    ci_log_init();  // Initialize log module
    #if COMMAND_LINE_CONSOLE_EN
    vUARTCommandConsoleStart(256, 4);  // Start CLI console
    #endif
    #endif
    return 0;
}
```

### Log Macros

```c
#include "ci_log.h"

ci_loginfo(LOG_USER, "Hello World\n");
ci_logdebug(CI_LOG_DEBUG, "Debug value: %d\n", value);
ci_logerr(LOG_USER, "Error: %s\n", msg);
mprintf("Direct printf\n");
```

---

## Voice Module Protocol UART

The voice module protocol UART is configured in `user_config.h`:

```c
#define MSG_COM_USE_UART_EN         1
#define UART_PROTOCOL_NUMBER        (HAL_UART2_BASE)
#define UART_PROTOCOL_BAUDRATE      (UART_BaudRate9600)
#define UART_PROTOCOL_VER           2   // 1=legacy, 2=current, 255=platform
```

### Protocol Initialization

In `user_msg_deal.c` `userapp_initial()`:

```c
void userapp_initial(void)
{
    #if MSG_COM_USE_UART_EN
    #if (UART_PROTOCOL_VER == 1)
    uart_communicate_init();  // Legacy protocol v1
    #elif (UART_PROTOCOL_VER == 2)
    vmup_communicate_init();  // Current protocol v2
    #elif (UART_PROTOCOL_VER == 255)
    // Platform-generated protocol: manual UART init
    UARTInterruptConfig((UART_TypeDef *)UART_PROTOCOL_NUMBER, UART_PROTOCOL_BAUDRATE);
    #endif
    #endif
}
```

### Receiving Protocol Messages

Protocol messages arrive as system messages:

```c
uint32_t deal_userdef_msg(sys_msg_t *msg)
{
    switch(msg->msg_type)
    {
    #if MSG_COM_USE_UART_EN
    case SYS_MSG_TYPE_COM:
    {
        #if ((UART_PROTOCOL_VER == 1) || (UART_PROTOCOL_VER == 2))
        sys_msg_com_data_t *com_rev_data;
        com_rev_data = &(msg->msg_data.com_data);
        userapp_deal_com_msg(com_rev_data);
        #endif
        break;
    }
    #endif
    default:
        break;
    }
    return ret;
}
```

---

## Custom UART Configuration

### DMA Mode UART (recommended for most uses)

```c
#include "ci130x_uart.h"
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"

void custom_uart_init(void)
{
    UART_TypeDef *uart = (UART_TypeDef*)HAL_UART1_BASE;

    // Step 1: Enable clock and reset
    scu_set_device_gate(HAL_UART1_BASE, ENABLE);
    scu_set_device_reset(HAL_UART1_BASE);
    scu_set_device_reset_release(HAL_UART1_BASE);

    // Step 2: Configure pin mux for UART1 on PB7(TX)/PB0(RX)
    dpmu_set_io_reuse(PB7, SECOND_FUNCTION);  // UART1_TX
    dpmu_set_io_reuse(PB0, SECOND_FUNCTION);  // UART1_RX

    // Step 3: Configure UART in DMA mode
    UARTDMAConfig(uart, UART_BaudRate115200);

    // Step 4: Enable UART
    UART_EN(uart, ENABLE);
}
```

### Interrupt Mode UART

```c
void custom_uart_interrupt_init(void)
{
    UART_TypeDef *uart = (UART_TypeDef*)HAL_UART1_BASE;

    scu_set_device_gate(HAL_UART1_BASE, ENABLE);
    scu_set_device_reset(HAL_UART1_BASE);
    scu_set_device_reset_release(HAL_UART1_BASE);

    dpmu_set_io_reuse(PB7, SECOND_FUNCTION);
    dpmu_set_io_reuse(PB0, SECOND_FUNCTION);

    // Configure with interrupt mode
    UARTInterruptConfig(uart, UART_BaudRate115200);

    // Enable specific interrupts
    UART_IntMaskConfig(uart, UART_RXInt, ENABLE);       // Receive interrupt
    UART_IntMaskConfig(uart, UART_RXTimeoutInt, ENABLE); // Timeout interrupt

    UART_EN(uart, ENABLE);
}
```

### Polling Mode UART

```c
void custom_uart_polling_init(void)
{
    UART_TypeDef *uart = (UART_TypeDef*)HAL_UART1_BASE;

    scu_set_device_gate(HAL_UART1_BASE, ENABLE);
    scu_set_device_reset(HAL_UART1_BASE);
    scu_set_device_reset_release(HAL_UART1_BASE);

    dpmu_set_io_reuse(PB7, SECOND_FUNCTION);
    dpmu_set_io_reuse(PB0, SECOND_FUNCTION);

    // Configure in polling mode
    UARTPollingConfig(uart, UART_BaudRate115200);
    UART_EN(uart, ENABLE);
}

// Send data via polling
void uart_send_string(UART_TypeDef *uart, const char *str)
{
    while (*str)
    {
        UartPollingSenddata(uart, *str++);
    }
    UartPollingSenddone(uart);  // Wait for transmission complete
}

// Receive data via polling
char uart_receive_char(UART_TypeDef *uart)
{
    return UartPollingReceiveData(uart);
}
```

---

## Advanced UART Configuration

### Custom Line Control

```c
UART_TypeDef *uart = (UART_TypeDef*)HAL_UART1_BASE;

// 8 data bits, 1 stop bit, no parity (most common)
UART_LCRConfig(uart, UART_WordLength_8b, UART_StopBits_1, UART_Parity_No);

// 8 data bits, 1 stop bit, even parity
UART_LCRConfig(uart, UART_WordLength_8b, UART_StopBits_1, UART_Parity_Even);

// 7 data bits, 2 stop bits, odd parity
UART_LCRConfig(uart, UART_WordLength_7b, UART_StopBits_2, UART_Parity_Odd);
```

### FIFO Configuration

```c
// Set RX FIFO trigger level (interrupt when FIFO reaches this level)
UART_RXFIFOConfig(uart, UART_FIFOLevel1_4);  // Trigger at 4 words

// Set TX FIFO trigger level
UART_TXFIFOConfig(uart, UART_FIFOLevel1_2);  // Trigger at 8 words

// Clear FIFOs
UART_FIFOClear(uart);
```

### Receive Timeout

```c
// Set receive timeout (in bit times)
UART_TimeoutConfig(uart, 16);  // Timeout after 16 bit periods of inactivity
```

### Open Drain Mode

For interfacing with 5V logic (requires external pull-up to 5V):

```c
// In user_config.h:
#define UART1_PAD_OPENDRAIN_MODE_EN  1  // Enable open-drain for UART1

// Or configure in code:
dpmu_set_io_open_drain(PB7, ENABLE);  // TX pin open-drain
```

---

## Baud Rate Auto-Calibration

When using internal RC oscillator, baud rate may drift. Enable auto-calibration:

```c
// In user_config.h:
#if (USE_EXTERNAL_CRYSTAL_OSC == 0)
#define UART_BAUDRATE_CALIBRATE         1
#define BAUDRATE_SYNC_PERIOD            300000  // Sync period in ms
#define BAUDRATE_FAST_SYNC_PERIOD       5000    // Retry period after failure
#define BAUD_CALIBRATE_MAX_WAIT_TIME    400     // ACK timeout in ms
#endif
```

---

## Available Baud Rates

| Enum | Baud Rate |
|------|-----------|
| `UART_BaudRate2400` | 2400 |
| `UART_BaudRate4800` | 4800 |
| `UART_BaudRate9600` | 9600 |
| `UART_BaudRate19200` | 19200 |
| `UART_BaudRate38400` | 38400 |
| `UART_BaudRate57600` | 57600 |
| `UART_BaudRate115200` | 115200 |
| `UART_BaudRate230400` | 230400 |
| `UART_BaudRate380400` | 380400 |
| `UART_BaudRate460800` | 460800 |
| `UART_BaudRate921600` | 921600 |
| `UART_BaudRate1M` | 1000000 |
| `UART_BaudRate2M` | 2000000 |
| `UART_BaudRate3M` | 3000000 |

---

## Common Issues

| Issue | Cause | Fix |
|-------|-------|-----|
| Garbled output | Wrong baud rate | Match baud rate on both ends |
| No output | Clock gate not enabled | Call `scu_set_device_gate()` before config |
| Log and protocol conflict | Same UART for both | Use different UARTs for log and protocol |
| Data loss at high speed | FIFO overflow | Use DMA mode, increase FIFO trigger level |
| Baud rate drift | Internal RC inaccuracy | Enable `UART_BAUDRATE_CALIBRATE` or use external crystal |
| 5V interface damage | Push-pull to 5V | Enable open-drain mode with external pull-up |
