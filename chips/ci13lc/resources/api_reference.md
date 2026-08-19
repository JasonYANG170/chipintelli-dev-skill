# CI13LC Peripheral API Reference

> SDK: `CI13LC_SDK_V2.0.15` (also applies to V2.1.5)
> Headers: `driver/ci13lc_chip_driver/inc/ci13lc_*.h`
> Architecture: RISC-V Nuclei N300 (rv32imafc)

## Naming Convention: CI13LC vs CI130X

CI13LC is the 3rd-generation low-cost family. The driver headers use a dual naming scheme:

| CI130X header | CI13LC header (alias) | Notes |
|---|---|---|
| `ci130x_gpio.h` | `ci13lc_gpio.h` / `ci_gpio.h` | Both exist; `ci13lc_*.h` is the primary |
| `ci130x_uart.h` | `ci13lc_uart.h` / `ci_uart.h` | Same API, different include guard |
| `ci130x_scu.h` | `ci13lc_scu.h` / `ci_scu.h` | SCU has expanded IIS clock config |
| `ci130x_dpmu.h` | `ci13lc_dpmu.h` / `ci_dpmu.h` | DPMU adds PVDC, wakeup reset cfg |
| `ci130x_spiflash.h` | `ci13lc_spiflash.h` / `ci_spiflash.h` | Same flash API |
| `ci130x_iwdg.h` | `ci13lc_iwdg.h` / `ci_iwdg.h` | Same watchdog API |
| `ci130x_dma.h` | `ci13lc_dma.h` / `ci_dma.h` | Same DMA controller |
| `ci130x_iis.h` | `ci13lc_iis.h` / `ci_iis.h` | IIS clock routing via SCU is new |
| `ci130x_iic.h` | `ci13lc_iic.h` / `ci_iic.h` | Same I2C API |
| `ci130x_pwm.h` | `ci13lc_pwm.h` / `ci_pwm.h` | Same PWM API |
| `ci130x_timer.h` | `ci13lc_timer.h` / `ci_timer.h` | Same timer API |
| `ci130x_codec.h` | `ci13lc_codec.h` / `ci_codec.h` | Inner codec, same register layout |
| N/A | `ci13lc_epwm.h` | **New**: Enhanced PWM (EPWM) with dead-band, trip zone |
| N/A | `ci13lc_pvdc.h` | **New**: Programmable Voltage Detector |
| N/A | `ci13lc_lowpower.h` | Low-power mode management |
| N/A | `ci13lc_cache.h` | Cache control (ICACHE/SCACHE) |

**Key difference**: CI13LC function names are **not** prefixed with `ci13lc_`. The function names are the same as CI130X (e.g., `gpio_set_output_mode`, `UARTDMAConfig`, `flash_init`). Only the **header file names** and **include guards** differ. This means CI130X user code can often be ported by just changing `#include "ci130x_gpio.h"` to `#include "ci13lc_gpio.h"` or `#include "ci_gpio.h"`.

## Chip Variant Headers

CI13LC uses a layered header system. The top-level `ci13lc.h` defines all register bases, IRQ numbers, and peripheral typedefs. Per-pin-count-family headers define the `PinPad_Name` enum:

| Header | Covers Chips | Pin Pad Count |
|---|---|---|
| `ci1308x.h` | CI13080, CI13081, CI13082 | Minimal (10 pads) |
| `ci1316x.h` | CI13160, CI13160P, CI13161, CI13161P, CI13162 | Small (13 pads) |
| `ci1324x.h` | CI13240, CI13241, CI13242 | Medium (19 pads) |
| `ci1332x.h` | CI13320, CI13321, CI13322, CI13322S | Full (27 pads) |
| `ci13642.h` | CI13642 | Full + extra (same as ci1332x layout) |

Per-chip headers (`ci13080.h`, `ci13160.h`, `ci13242.h`, `ci13322.h`, etc.) only define `FLASH_SIZE` and `#include` the family header. The `CI_CHIP_TYPE` macro in `user_config.h` selects which chip header is included by the build system.

---

## GPIO (ci13lc_gpio.h)

### Types

```c
typedef enum { PA = HAL_PA_BASE, PB = HAL_PB_BASE, PC = HAL_PC_BASE } gpio_base_t;
typedef enum { pin_0, pin_1, pin_2, pin_3, pin_4, pin_5, pin_6, pin_7, pin_all } gpio_pin_t;
typedef enum {
    high_level_trigger = 1, low_level_trigger = 2,
    up_edges_trigger = 3, down_edges_trigger = 4, both_edges_trigger = 5
} gpio_trigger_t;
typedef struct { gpio_base_t base; gpio_pin_t pin; } gpio_info_t;
typedef void (*gpio_irq_callback_t)(void);
typedef struct gpio_irq_callback_list_s {
    gpio_irq_callback_t gpio_irq_callback;
    struct gpio_irq_callback_list_s *next;
} gpio_irq_callback_list_t;
```

### Multi-pin API (operate one or more pins simultaneously)

```c
void gpio_set_output_mode(gpio_base_t gpio, gpio_pin_t pins);
void gpio_set_input_mode(gpio_base_t gpio, gpio_pin_t pins);
uint8_t gpio_get_direction_status(gpio_base_t gpio, gpio_pin_t pins);
void gpio_irq_mask(gpio_base_t gpio, gpio_pin_t pins);
void gpio_irq_unmask(gpio_base_t gpio, gpio_pin_t pins);
void gpio_irq_trigger_config(gpio_base_t gpio, gpio_pin_t pins, gpio_trigger_t trigger);
void gpio_set_output_high_level(gpio_base_t gpio, gpio_pin_t pins);
void gpio_set_output_low_level(gpio_base_t gpio, gpio_pin_t pins);
uint8_t gpio_get_input_level(gpio_base_t gpio, gpio_pin_t pins);
```

### Single-pin API

```c
uint8_t gpio_get_direction_status_single(gpio_base_t gpio, gpio_pin_t pins);
uint8_t gpio_get_irq_raw_status_single(gpio_base_t gpio, gpio_pin_t pins);
uint8_t gpio_get_irq_mask_status_single(gpio_base_t gpio, gpio_pin_t pins);
void gpio_clear_irq_single(gpio_base_t gpio, gpio_pin_t pins);
void gpio_set_output_level_single(gpio_base_t gpio, gpio_pin_t pins, uint8_t level);
uint8_t gpio_get_input_level_single(gpio_base_t gpio, gpio_pin_t pins);
```

### Interrupt Registration

```c
void registe_gpio_callback(gpio_base_t base, gpio_irq_callback_list_t *gpio_irq_callback_node);
void PA_IRQHandler(void);
void PB_IRQHandler(void);
void PC_IRQHandler(void);
```

### Usage Pattern

```c
#include "ci_gpio.h"
#include "ci_scu.h"

/* Configure PA2 as output */
scu_set_device_gate(HAL_PA_BASE, ENABLE);
dpmu_set_io_reuse(PA2, FIRST_FUNCTION);
dpmu_set_io_direction(PA2, DPMU_IO_DIRECTION_OUTPUT);
gpio_set_output_mode(PA, pin_2);
gpio_set_output_high_level(PA, pin_2);

/* Configure PA4 as input with falling-edge interrupt */
dpmu_set_io_reuse(PA4, FIRST_FUNCTION);
dpmu_set_io_direction(PA4, DPMU_IO_DIRECTION_INPUT);
gpio_set_input_mode(PA, pin_4);
gpio_irq_trigger_config(PA, pin_4, down_edges_trigger);
gpio_irq_unmask(PA, pin_4);

static gpio_irq_callback_list_t my_cb_node;
static void my_gpio_isr(void) { /* handle interrupt */ }
registe_gpio_callback(PA, &my_cb_node);
my_cb_node.gpio_irq_callback = my_gpio_isr;
```

---

## UART (ci13lc_uart.h)

### Types

```c
typedef enum {
    UART_BaudRate2400, UART_BaudRate4800, UART_BaudRate9600, UART_BaudRate19200,
    UART_BaudRate38400, UART_BaudRate57600, UART_BaudRate115200, UART_BaudRate230400,
    UART_BaudRate380400, UART_BaudRate460800, UART_BaudRate921600,
    UART_BaudRate1M, UART_BaudRate2M, UART_BaudRate3M
} UART_BaudRate;

typedef enum { UART_WordLength_5b, UART_WordLength_6b, UART_WordLength_7b, UART_WordLength_8b } UART_WordLength;
typedef enum { UART_StopBits_1, UART_StopBits_1_5, UART_StopBits_2 } UART_StopBits;
typedef enum { UART_Parity_No, UART_Parity_Odd, UART_Parity_Even } UART_Parity;
typedef enum { UART_Byte, UART_Word } UART_ByteWord;
typedef enum { UART_RXDMA, UART_TXDMA } UART_TXRXDMA;
typedef enum { UART_DmaWidth_Byte, UART_DmaWidth_Halfword, UART_DmaWidth_Word } UART_DmaWidth;
```

### Key Functions

```c
/* Polling mode */
void UARTPollingConfig(UART_TypeDef* UARTx, UART_BaudRate uartbaudrate);
void UartPollingSenddata(UART_TypeDef* UARTx, char ch);
char UartPollingReceiveData(UART_TypeDef* UARTx);
void UartPollingSenddone(UART_TypeDef* UARTx);

/* DMA mode (preferred for protocol UART) */
void UARTDMAConfig(UART_TypeDef* UARTx, UART_BaudRate uartbaudrate);
void UARTDMAElseConfig(UART_TypeDef* UARTx, UART_BaudRate uartbaudrate, UART_DmaWidth width_type);
void UART_TXRXDMAConfig(UART_TypeDef* UARTx, UART_TXRXDMA uartdma);
void UART_DMAByteWordConfig(UART_TypeDef* UARTx, FunctionalState cmd);

/* Interrupt mode */
void UARTInterruptConfig(UART_TypeDef* UARTx, UART_BaudRate bd);
void UART_IntMaskConfig(UART_TypeDef* UARTx, UART_IntMask intmask, FunctionalState cmd);
void UART_IntClear(UART_TypeDef* UARTx, UART_IntMask intmask);
int UART_RawIntState(UART_TypeDef* UARTx, UART_IntMask intmask);
int UART_MaskIntState(UART_TypeDef* UARTx, UART_IntMask intmask);

/* Low-level config */
int UART_BAUDRATEConfig(UART_TypeDef* UARTx, UART_BaudRate uartbaudrate);
int UART_LCRConfig(UART_TypeDef* UARTx, UART_WordLength wordlength, UART_StopBits uartstopbits, UART_Parity uartparity);
void UART_FIFOClear(UART_TypeDef* UARTx);
void UART_EN(UART_TypeDef* UARTx, FunctionalState cmd);
void UART_CRConfig(UART_TypeDef* UARTx, UART_CRBitCtrl crbitctrl, FunctionalState cmd);
void UART_RXFIFOConfig(UART_TypeDef* UARTx, UART_FIFOLevel fifoleve);
void UART_TXFIFOConfig(UART_TypeDef* UARTx, UART_FIFOLevel fifoleve);
void UART_TimeoutConfig(UART_TypeDef* UARTx, unsigned short time);
void UartSetCLKBase(UART_TypeDef *UARTx);
void UART_NO_STOP_EN(UART_TypeDef *UARTx, FunctionalState cmd);

/* Baud rate auto-detection (for internal RC clock source) */
UART_BaudStatus UartBaudStatusRead(UART_TypeDef *UARTx);
void UartBaudSampleRateSet(UART_TypeDef *UARTx, UART_BaudSampleRate sample);
void UartBaudIntClear(UART_TypeDef *UARTx, UART_BaudInt status);
uint32_t UartBaudIntStatus(UART_TypeDef *UARTx, UART_BaudInt status);
void UartBaudIntMask(UART_TypeDef *UARTx, UART_BaudInt status, FunctionalState en);
void UartBaudCheckEnable(UART_TypeDef *UARTx, FunctionalState en);
```

### DMA Addresses

```c
#define UART0_DMA_ADDR 0x61000000
#define UART1_DMA_ADDR 0x62000000
#define UART2_DMA_ADDR 0x63000000
```

### Usage Pattern

```c
#include "ci_uart.h"
#include "ci_scu.h"

/* Protocol UART (DMA mode) */
UART_TypeDef *uart = (UART_TypeDef*)UART_PROTOCOL_NUMBER;
scu_set_device_gate((uint32_t)uart, ENABLE);
pad_config_for_uart(uart);  /* board.c helper */
UARTDMAConfig(uart, UART_PROTOCOL_BAUDRATE);
```

---

## I2C (ci13lc_iic.h)

### Types

```c
typedef enum { IIC0 = HAL_IIC0_BASE, IIC_NULL = 0 } iic_base_t;
typedef enum { LONG_TIME_OUT = 0x5FFFFF, SHORT_TIME_OUT = 0xFFFF } IIC_TimeOut;
typedef enum { IIC_ACKTYPE_ERR = -1, IIC_ACKTYPE_ACK = 0, IIC_ACKTYPE_NACK = 1 } IIC_AckType;
typedef enum { IIC_M_WRITE = 0, IIC_M_READ = 1 } iic_multi_transmission_type;
typedef struct {
    char *buf; int size;
    iic_multi_transmission_type flag;
    union { int read_size; int write_size; };
} multi_transmission_msg;
```

### Polling API (commonly used)

```c
void iic_polling_init(iic_base_t base, uint32_t speed, uint32_t slaveaddr, IIC_TimeOut timeout);
int32_t iic_master_polling_send(iic_base_t base, uint16_t addr, const char *buf, int32_t count, uint8_t *last_ack_flag);
int32_t iic_master_polling_recv(iic_base_t base, uint16_t addr, char *buf, int32_t count);
int32_t iic_master_multi_transmission(iic_base_t base, uint16_t addr, multi_transmission_msg *msg, int msg_count);
```

### Interrupt API (enable `IIC_INTERFACE_ELSE=1`)

```c
void iic_interrupt_init(iic_base_t base, uint32_t speed, uint32_t slaveaddr, IIC_TimeOut timeout);
int32_t iic_master_interrupt_send(iic_base_t base, uint16_t addr, master_send_cb_t master_send_cb);
int32_t iic_master_interrupt_recv(iic_base_t base, uint16_t addr, master_recv_cb_t master_recv_cb);
int32_t iic_slave_interrupt_send(iic_base_t base, slave_send_cb_t slave_send_cb);
int32_t iic_slave_interrupt_recv(iic_base_t base, slave_recv_cb_t slave_recv_cb);
void IIC_IRQHandler(iic_base_t base);
```

### Legacy Compatibility API

```c
int32_t i2c_master_only_send(char slave_ic_address, const char *buf, int32_t count);
int32_t i2c_master_send_recv(char slave_ic_address, char *buf, int32_t send_len, int32_t rev_len);
int32_t i2c_master_only_recv(char slave_ic_address, char *buf, int32_t rev_len);
```

---

## IIS (ci13lc_iis.h)

### Types

```c
typedef enum { IIS0 = HAL_IIS0_BASE, IIS1 = HAL_IIS1_BASE } iis_base_t;
typedef enum { IIS_DW_16BIT, IIS_DW_24BIT, IIS_DW_32BIT, IIS_DW_20BIT } iis_data_width_t;
typedef enum { IIS_DF_IIS, IIS_DF_MSB, IIS_DF_LSB } iis_data_format_t;
typedef enum { IIS_SC_STEREO, IIS_SC_MONO } iis_sound_channel_t;
typedef enum { IIS_TX_CHANNAL_TX0, IIS_TX_CHANNAL_TX1 } iis_tx_channal_t;
typedef enum { IIS_RX_CHANNAL_RX0, IIS_RX_CHANNAL_RX1, IIS_RX_CHANNAL_RX2 } iis_rx_channal_t;
```

### Functions

```c
void iis_tx_enable(uint32_t iis_base, iis_tx_channal_t cha, FunctionalState cmd);
void iis_tx_l_mute(uint32_t iis_base, iis_tx_channal_t cha, FunctionalState cmd);
void iis_tx_r_mute(uint32_t iis_base, iis_tx_channal_t cha, FunctionalState cmd);
void iis_tx_chk(uint32_t iis_base, iis_tx_channal_t cha, FunctionalState cmd);
void iis_tx_config(uint32_t iis_base, iis_tx_config_p tx_cfg);
void iis_tx_same(uint32_t iis_base, FunctionalState cmd);

void iis_rx_enable(uint32_t iis_base, iis_rx_channal_t cha, FunctionalState cmd);
void iis_rx_mute(uint32_t iis_base, iis_rx_channal_t cha, FunctionalState cmd);
void iis_rx_chk(uint32_t iis_base, iis_rx_channal_t cha, FunctionalState cmd);
void iis_rx_dma_chk(uint32_t iis_base, iis_rx_channal_t cha, FunctionalState cmd);
void iis_rx_config(uint32_t iis_base, iis_rx_config_p rx_cfg);
void iis_rx_cha_merge(uint32_t iis_base, uint8_t enable_rx0, uint8_t enable_rx1, uint8_t enable_rx2);

void iis_int_handler(uint32_t iis_base);
void IISx_TXDMA_Init(IIS_DMA_TXInit_Typedef* IISDMA_Str);
void IISx_RXDMA_Init(IIS_DMA_RXInit_Typedef* IISDMA_Str);
```

### IIS Clock Configuration (via SCU)

CI13LC introduces a flexible IIS clock routing system through the SCU. Key types and functions from `ci13lc_scu.h`:

```c
typedef struct {
    IIS_Src_Source_t source;   /* IPCORE, EXT_OSC, INTER_RC, PAD_IN */
    uint32_t source_div;
} IIS_Src_Config_t;

typedef struct {
    IIS_Mclk_t mclk;           /* MCLK0 or MCLK1 */
    IIS_Mclk_Source_t src;     /* SRC0, SRC1, PAD_IN */
    IIS_Mclk_Fs_t fs;          /* 128, 192, 256, 384 */
    IIS_Sck_Lrck_Wid_t sck_lrck; /* 32 or 64 */
} IIS_Mclk_Config_t;

typedef struct {
    IISNumx device_select;       /* IISNum0, IISNum1_RX, IISNum1_TX */
    IIS_Mode_Sel_t model_sel;    /* IIS_SLAVE or IIS_MASTER */
    IIS_Src_Config_t src_cfg;
    IIS_Mclk_Config_t mclk_cfg;
    IIS_Clk_Source_t clk_cfg;
    IIS_Mclk_Mode_t mclk_mode;
    IIS_SckLrck_Mode_t clk_mode;
} IIS_Clk_ConfigTypedef;

void scu_iis_src_config(IIS_Src_Config_t *config, IIS_Mclk_Source_t src);
void scu_iis_mclk_config(IIS_Mclk_Config_t *config);
void scu_iis_pad_mclk_config(IIS_Mclk_Source_t src, IIS_Clk_Mode_t mode);
void scu_iis_codec_mclk_config(Codec_Channel_t channel, IIS_Mclk_Source_t src);
void scu_iis_clk_config(IISNumx device, IIS_Clk_Source_t clk_source);
void scu_iis_pad_clk_config(IIS_Clk_Source_t clk_source, IIS_Clk_Mode_t mode);
void scu_iis_codec_dac_data_config(Codec_Dac_Data_Sel_t src);
void scu_iis_pad_data_config(Pad_IIS_Data_Sel_t src);
void iis_clk_config(IIS_Clk_ConfigTypedef* config);
```

> **Note**: IIS1 is connected to the inner CODEC by default. IIS0 is the external IIS interface on PAD. The SCU clock routing determines which clock sources feed MCLK, SCK, and LRCK.

---

## ADC / PVDC (ci13lc_pvdc.h)

CI13LC does not have a general-purpose ADC. It has a **Programmable Voltage Detector (PVDC)** for power monitoring.

### Types

```c
typedef enum { PVDC_INTR_BELOW_LOW, PVDC_INTR_BETWEEN_LOW_HIGH, PVDC_INTR_ABOVE_HIGH } pvdc_intr_t;
typedef enum { PVDC_ANALOG_BELOW_CURRENT, PVDC_ANALOG_ABOVE_CURRENT } pvdc_result_t;
typedef enum {
    PVDC_VOL_2_4, PVDC_VOL_2_5, PVDC_VOL_2_6, PVDC_VOL_2_7,
    PVDC_VOL_2_8, PVDC_VOL_2_9, PVDC_VOL_3_0, PVDC_VOL_3_1
} pvdc_vol_t;
```

### Functions

```c
void pvdc_reg_unlock(void);
void pvdc_reg_lock(void);
uint8_t pvdc_raw_irq_status(pvdc_intr_t intr);
uint8_t pvdc_mask_irq_status(pvdc_intr_t intr);
void pvdc_clear_irq(pvdc_intr_t intr);
void pvdc_irq_mask(pvdc_intr_t intr, FunctionalState en);
pvdc_result_t pvdc_get_irq_pvd_result(void);
pvdc_vol_t pvdc_get_irq_vol(void);
void pvdc_enable(FunctionalState en);
void pvdc_set_vol_high_threshold(pvdc_vol_t threshold);
void pvdc_set_vol_low_threshold(pvdc_vol_t threshold);
pvdc_result_t pvdc_get_current_pvd_result(void);
pvdc_vol_t pvdc_get_current_vol(void);
void pvdc_scan_interval_time(uint16_t time);
void pvdc_max_wait_times(uint16_t times);
```

---

## PWM (ci13lc_pwm.h)

### Types

```c
typedef enum { PWM0 = HAL_PWM0_BASE, PWM1 = HAL_PWM1_BASE, PWM2 = HAL_PWM2_BASE, PWM3 = HAL_PWM3_BASE } pwm_base_t;
typedef enum { PWM_LEVEL_LOW, PWM_LEVEL_HIGH } pwm_level_t;
typedef enum { pwm_clk_pclk, pwm_clk_exit } pwm_clk_sel_t;
typedef struct {
    pwm_clk_sel_t clk_sel;
    unsigned int freq;      /* Hz */
    unsigned int duty;
    unsigned int duty_max;
} pwm_init_t;
```

### Functions

```c
void pwm_init(pwm_base_t base, pwm_init_t init);
void pwm_start(pwm_base_t base);
void pwm_stop(pwm_base_t base);
void pwm_set_duty(pwm_base_t base, unsigned int duty, unsigned int duty_max);
void pwm_set_restart_md(pwm_base_t base, uint8_t cmd);
void pwm_set_stop_level(pwm_base_t base, pwm_level_t level);
```

---

## EPWM (ci13lc_epwm.h) -- Enhanced PWM

CI13LC adds an **Enhanced PWM (EPWM)** module with dead-band generation, trip-zone protection, and ADC SOC trigger. This is useful for motor control and power electronics.

### Key Types

```c
typedef enum { EPWM_CTRMODE_INC, EPWM_CTRMODE_DEC, EPWM_CTRMODE_INC_DEC, EPWM_CTRMODE_STOP } EPWM_CTRMODEx;
typedef enum { EPWM_CLKDIV1, EPWM_CLKDIV2, EPWM_CLKDIV4, EPWM_CLKDIV8, EPWM_CLKDIV16, EPWM_CLKDIV32, EPWM_CLKDIV64, EPWM_CLKDIV128 } EPWM_CLKDIVx;
typedef enum { EPWM_OUT_NONE, EPWM_OUT_LOW, EPWM_OUT_HIGH, EPWM_OUT_EDG } EPWM_OUTx;

typedef struct {
    unsigned short TBPRD, CMPA, CMPB, CPR1, CPR2, DBRED, DBFED, TBPHS;
    epwm_tbctl_init_t TBCTL;
    epwm_cmpctrl_init_t CMPCTL;
    epwm_aqctlx_init_t AQCTLA;
    epwm_aqctlx_init_t AQCTLB;
    epwm_aqsfrc_init_t AQSFRC;
    epwm_aqcsfrc_init_t AQCSFRC;
    epwm_dbctl_init_t DBCTL;
    epwm_tzsel_init_t TZSEL;
    epwm_tzctl_init_t TZCTL;
    epwm_etsel_init_t ETSEL;
    epwm_etps_init_t ETPS;
    epwm_etfrc_init_t ETFRC;
} epwm_init_t;
```

### Functions

```c
void epwm_init(EPWM_TypeDef* epwmx, epwm_init_t* EPWMInit_Struct);
void epwm_start(EPWM_TypeDef* EPWMx);
void epwm_stop(EPWM_TypeDef* EPWMx);
void epwm_tbctl_config(EPWM_TypeDef* EPWMx, epwm_tbctl_init_t* tbctl_init);
void epwm_tbprd_config(EPWM_TypeDef* EPWMx, unsigned short tbprd);
void epwm_cmpa_config(EPWM_TypeDef* epwmx, unsigned short cmpaval);
void epwm_cmpb_config(EPWM_TypeDef* EPWMx, unsigned short cmpbval);
void epwm_aqctla_config(EPWM_TypeDef* EPWMx, epwm_aqctlx_init_t* aqctla_init);
void epwm_aqctlb_config(EPWM_TypeDef* EPWMx, epwm_aqctlx_init_t* aqctlb_init);
void epwm_cmpctrl_config(EPWM_TypeDef* EPWMx, epwm_cmpctrl_init_t* cmpctrl_init);
void epwm_dbctl_config(EPWM_TypeDef* EPWMx, epwm_dbctl_init_t* dbctl_init);
void epwm_dbred_config(EPWM_TypeDef* EPWMx, unsigned short dbred);
void epwm_dbfed_config(EPWM_TypeDef* EPWMx, unsigned short dbfed);
void epwm_tzsel_config(EPWM_TypeDef* EPWMx, epwm_tzsel_init_t* tzsel_init);
void epwm_tzctl_config(EPWM_TypeDef* EPWMx, epwm_tzctl_init_t* tzctl_init);
void epwm_etsel_config(EPWM_TypeDef* EPWMx, epwm_etsel_init_t* etsel_init);
void epwm_etsel_interrupt_enable(EPWM_TypeDef* EPWMx, FunctionalState cmd);
void epwm_etps_config(EPWM_TypeDef* EPWMx, epwm_etps_init_t* etps_init);
void epwm_soc_config(EPWM_TypeDef* epwmx, EPWM_SOCx socx, EPWM_SOCABSELx socsel);
```

---

## Timer (ci13lc_timer.h)

### Types

```c
typedef enum { TIMER0 = HAL_TIMER0_BASE, TIMER1 = HAL_TIMER1_BASE } timer_base_t;
typedef enum { timer_count_mode_single, timer_count_mode_auto, timer_count_mode_free, timer_count_mode_event } timer_count_mode_t;
typedef enum { timer_clk_div_0, timer_clk_div_2, timer_clk_div_4, timer_clk_div_16 } timer_clock_div_t;
typedef enum { timer_iqr_width_f, timer_iqr_width_2, timer_iqr_width_4, timer_iqr_width_8 } timer_iqr_width_t;
typedef enum { timer_clk_pclk, timer_clk_exit } timer_clk_sel_t;
typedef struct {
    timer_count_mode_t mode;
    timer_clock_div_t div;
    timer_iqr_width_t width;
    unsigned int count;
    timer_clk_sel_t clk_sel;
} timer_init_t;

#define TIMER_S_COUNT  (get_apb_clk())
#define TIMER_MS_COUNT (get_apb_clk() / 1000)
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

## DMA (ci13lc_dma.h)

### Types

```c
typedef enum { DMACChannel0, DMACChannel1, DMACChannel2, DMACChannel3, DMACChannel4, DMACChannel5, DMACChannel6, DMACChannel7, DMACChannelALL } DMACChannelx;
typedef enum { DMAC_AHBMaster1, DMAC_AHBMaster2 } DMAC_AHBMasterx;
typedef enum { INCREMENT, NOINCREMENT } INCREMENTx;
typedef enum { TRANSFERWIDTH_8b, TRANSFERWIDTH_16b, TRANSFERWIDTH_32b } TRANSFERWIDTHx;
typedef enum { BURSTSIZE1, BURSTSIZE4, BURSTSIZE8, BURSTSIZE16, BURSTSIZE32, BURSTSIZE64, BURSTSIZE128, BURSTSIZE256 } BURSTSIZEx;
typedef enum { M2M_DMA, M2P_DMA, P2M_DMA, SP2DP_DMA, SP2DP_DP, M2P_P, P2M_P, SP2DP_SP } DMAC_FLOWCTRL;
typedef enum {
    DMAC_Peripherals_SPI0 = 0,
    DMAC_Peripherals_UART0_RX = 4, DMAC_Peripherals_UART0_TX = 5,
    DMAC_Peripherals_UART1_RX = 6, DMAC_Peripherals_UART1_TX = 7,
    DMAC_Peripherals_UART2_RX = 8, DMAC_Peripherals_UART2_TX = 9
} DMAC_Peripherals;
```

### Key Functions

```c
void DMAC_Config(DMAC_AHBMasterx dmamaster, ENDIANMODE endianmode);
void DMAC_EN(FunctionalState cmd);
void DMAC_M2MConfig(DMACChannelx dmachannel, unsigned int srcaddr, unsigned int destaddr, unsigned int bytesize, DMAC_AHBMasterx master);
void DMAC_M2P_P2MConfig(DMACChannelx dmachannel, DMAC_Peripherals periph, DMAC_FLOWCTRL flowctrl, unsigned int srcaddr, unsigned int destaddr, unsigned int bytesize);
void DMAC_M2P_P2M_advance_config(DMACChannelx dmachannel, DMAC_Peripherals periph, DMAC_FLOWCTRL flowctrl, unsigned int srcaddr, unsigned int destaddr, unsigned int bytesize, TRANSFERWIDTHx datawidth, BURSTSIZEx burstsize, DMAC_AHBMasterx master);
void DMAC_P2PConfig(DMACChannelx dmachannel, DMAC_Peripherals srcperiph, DMAC_Peripherals destperiph, unsigned int bytesize);
void DMAC_ChannelEnable(DMACChannelx dmachannel);
void DMAC_ChannelDisable(DMACChannelx dmachannel);
int DMAC_IntTCStatus(DMACChannelx dmachannel);
void DMAC_IntTCClear(DMACChannelx dmachannel);
void DMAC_ChannelTCInt(DMACChannelx dmachannel, FunctionalState cmd);
void clear_dma_translate_flag(DMACChannelx dmachannel);
int wait_dma_translate_flag(DMACChannelx dmachannel, uint32_t timeout);
void dma_with_os_int(void);
void dma_without_os_int(void);
```

### GDMA Memory Regions

```c
#define GDMA_SDRAM_ADDR   0x70000000UL  /* 16MB external SDRAM */
#define GDMA_CSRAM_ADDR   0x1FFF8000UL  /* 32KB CSRAM */
#define GDMA_SRAM0_ADDR   0x1FFE8000UL  /* 64KB SRAM0 */
#define GDMA_SRAM1_ADDR   0x20000000UL  /* 64KB SRAM1 */
#define GDMA_PCMRAM_ADDR  0x20020000UL  /* 16KB PCM RAM */
#define GDMA_FFTRAM_ADDR  0x200FF800UL  /* 2KB FFT RAM */
```

---

## SPIFlash (ci13lc_spiflash.h)

### Types

```c
typedef enum { QSPI0 = HAL_DTRFLASH_BASE } spic_base_t;
typedef enum {
    SPIC_CMD_CODE_WRITE_ENABLE = 0x06,
    SPIC_CMD_CODE_READJEDECID = 0x9F,
    SPIC_CMD_CODE_SECTORERASE4K = 0x20,
    SPIC_CMD_CODE_BLOCKERASE64K = 0xd8,
    SPIC_CMD_CODE_CHIPERASE = 0xc7,
    SPIC_CMD_CODE_PAGEPROGRAM = 0x02,
    SPIC_CMD_CODE_READDATA = 0x03,
    SPIC_CMD_CODE_FASTREAD = 0x0b,
    SPIC_CMD_CODE_POWERDOWN = 0xb9,
    SPIC_CMD_CODE_RELEASEPOWERDOWN = 0xab,
    /* ... many more commands ... */
} spic_cmd_code_t;
typedef enum { SPIC_SECURITY_REG1, SPIC_SECURITY_REG2, SPIC_SECURITY_REG3 } spic_security_reg_t;
```

### Functions

```c
int32_t flash_init(spic_base_t spic);
int32_t spic_read_unique_id(spic_base_t spic, uint8_t* unique);
int32_t spic_read_jedec_id(spic_base_t spic, uint8_t* jedec);
int32_t flash_erase(spic_base_t spic, uint32_t addr, uint32_t size);
int32_t flash_write(spic_base_t spic, uint32_t addr, uint32_t buf, uint32_t size);
int32_t flash_read(spic_base_t spic, uint32_t buf, uint32_t addr, uint32_t size);
int32_t dnn_mode_config(spic_base_t spic, uint32_t start_addr, uint32_t size);
int32_t flash_dnn_mode(spic_base_t spic, FunctionalState cmd);
uint32_t flash_check_mode(spic_base_t spic);
int32_t spic_quad_mode(spic_base_t spic);
int32_t spic_erase(spic_base_t spic, spic_cmd_code_t code, uint32_t addr);
int32_t spic_protect(spic_base_t spic, FunctionalState cmd);
int32_t spic_reset(spic_base_t spic);
int32_t spic_xipconfig(spic_base_t spic);

/* Security register API (OTP-like) */
int32_t spic_erase_security_reg(spic_base_t spic, spic_security_reg_t reg);
int32_t spic_write_security_reg(spic_base_t spic, spic_security_reg_t reg, uint32_t buf, uint32_t addr, uint32_t size);
int32_t spic_read_security_reg(spic_base_t spic, spic_security_reg_t reg, uint32_t buf, uint32_t addr, uint32_t size);
int32_t spic_security_reg_lock(spic_base_t spic, spic_security_reg_t reg);
```

---

## Watchdog / IWDG (ci13lc_iwdg.h)

### Types

```c
typedef enum { IWDG = HAL_IWDG_BASE } iwdg_base_t;
typedef enum { iwdg_irqen_enable, iwdg_irqen_disable } iwdg_irqen_t;
typedef enum { iwdg_resen_enable, iwdg_resen_disable } iwdg_resen_t;
typedef struct { unsigned int count; iwdg_irqen_t irq; iwdg_resen_t res; } iwdg_init_t;

#define IWDG_S_COUNT  (get_src_clk() / 16)
#define IWDG_MS_COUNT (IWDG_S_COUNT / 1000)
```

### Functions

```c
void iwdg_init(iwdg_base_t base, iwdg_init_t init);
void iwdg_open(iwdg_base_t base);
void iwdg_close(iwdg_base_t base);
void iwdg_feed(iwdg_base_t base);
void iwdg_irqhander(void);
```

> The SDK also provides `iwdg_config_reset(HAL_IWDG_BASE)` in system code for default watchdog configuration with reset enabled.

---

## SCU -- System Control Unit (ci13lc_scu.h)

### Register Lock/Unlock

```c
void scu_unlock_system_config(void);
void scu_unlock_reset_config(void);
void scu_unlock_clk_config(void);
void scu_lock_system_config(void);
void scu_lock_clk_config(void);
void scu_lock_reset_config(void);
```

### Device Clock and Reset

```c
int32_t scu_set_device_gate(uint32_t device_base, int32_t gate);       /* ENABLE/DISABLE clock */
int32_t scu_set_device_reset(uint32_t device_base);                    /* Assert reset */
int32_t scu_set_device_reset_release(uint32_t device_base);            /* Release reset */
int32_t scu_get_system_reset_state(void);                              /* RETURN_OK=power-on, PARA_ERROR=abnormal */
int32_t scu_set_system_clk_gate(Sys_Clk_Gate_t base, FunctionalState gate);
int32_t scu_set_div_parameter(uint32_t device_base, uint32_t div_num);
void scu_sel_dtrflash_clk(Dtr_Clk_Sel_t clk);
void scu_wait_pll_lock_state(void);
void scu_spiflash_no_boot_set(void);
void scu_run_in_flash(void);
void scu_run_not_in_flash(void);
```

### Wakeup and NMI

```c
int32_t scu_set_ext_wakeup_int(Wakeup_Mask_t num, FunctionalState cmd);
int32_t scu_clear_ext_int_state(Wakeup_Mask_t num);
void scu_set_ext_filter_config(Ext_Num num, FunctionalState cmd, uint32_t param);
void scu_nmi_irq_cfg(Nmi_Irq_t irq);
void dsu_init(void);  /* Dual-core sync unit init */
```

### IIS Clock Routing (see IIS section above)

---

## DPMU -- Digital Power Management Unit (ci13lc_dpmu.h)

### IO Configuration

```c
void dpmu_unlock_cfg_config(void);
void dpmu_lock_cfg_config(void);
void dpmu_set_io_reuse(PinPad_Name pin, IOResue_FUNCTION io_function);        /* Select pin mux function */
void dpmu_set_adio_reuse(PinPad_Name pin, ADIOResue_MODE adio_mode);           /* Digital or analog mode */
void dpmu_set_io_open_drain(PinPad_Name pin, FunctionalState cmd);
void dpmu_set_io_pull(PinPad_Name pin, Dpmu_Io_Pull_t pull);                   /* Pull-up/down/disable */
void dpmu_set_io_direction(PinPad_Name pin, Dpmu_Io_Direction_t dir);          /* Input or output */
void dpmu_set_io_slew_rate(PinPad_Name pin, Dpmu_Io_Slew_Rate_t slew_rate);
void dpmu_set_io_schmitt_trigger(PinPad_Name pin, Dpmu_Io_Schmitt_Trigger_t schmitt_trigger);
void dpmu_set_io_driver_strength(PinPad_Name pin, Dpmu_Io_Driver_Strength_t driver_strength);
void dpmu_osc_pad_for_gpio(FunctionalState en);   /* Configure PA0/PA1 as GPIO (vs crystal oscillator) */
```

### Clock and PLL

```c
void dpmu_pll_config(uint32_t in_clk, uint32_t out_clk);
void dpmu_pll_12d_config(uint32_t clk);
uint32_t dpmu_get_pll_frequency(void);
void dpmu_set_src_source(Dpmu_Src_Source_Sel_t sel);           /* Inner RC or external OSC */
void dpmu_sys_clk_sel_cfg(Dpmu_Sys_Clk_Sel_t sel);             /* SRC or PLL */
void dpmu_test_clk_sel(Dpmu_Test_Clk_Sel_t src);
void dpmu_iwdg_clk_sel(Dpmu_Iwdg_Clk_Sel_t src);
void dpmu_use_rc(void);
```

### Reset Control

```c
void dpmu_clean_reset_state(void);
void dpmu_iwdg_reset_none_config(void);
void dpmu_iwdg_reset_system_config(void);
void dpmu_iwdg_reset_bus_config(void);
void dpmu_software_reset_none_config(void);
void dpmu_software_reset_system_config(void);
void dpmu_software_reset_bus_config(void);
void dpmu_core_reset_none_config(void);
void dpmu_core_reset_system_config(void);
void dpmu_core_reset_bus_config(void);
```

### Low Power

```c
void dpmu_set_low_power_mode(Dpmu_Lowpower_Mode_t mode);       /* SLEEP, DEEP_SLEEP, BOTH */
void dpmu_set_wakeup_int(Dpmu_Wakeup_SRC_t wake_int_num, FunctionalState flag);
void dpmu_set_vdt_mask(bool en);
uint32_t dpmu_get_wakeup_state(void);
void dpmu_wakeup_reset_cfg(Dpmu_Wakeup_Reset_Cfg_t model, FunctionalState flag);
void dpmu_set_iwdg_halt(void);
void dpmu_clean_iwdg_halt(void);
```

### LDO and RC

```c
void dpmu_ldo1_lv_set(uint8_t lv);
void dpmu_ldo3_lv_set(uint8_t lv);
void dpmu_ldo3_en(bool en);
void dpmu_enter_lowpower_ldo1_lv(uint8_t lv);
void dpmu_enter_lowpower_ldo3_lv(uint8_t lv);
void dpmu_enter_lowpower_ldo3_en(bool en);
void dpmu_exit_lowpower_ldo1_lv(uint8_t lv);
void dpmu_exit_lowpower_ldo3_lv(uint8_t lv);
void dpmu_exit_lowpower_ldo3_en(bool en);
void dpmu_set_pmu_update_en(Dpmu_Update_En_t num);
void dpmu_set_ldo_mask(bool en);
void dpmu_set_rc_trim_c_value(uint8_t val);
void dpmu_set_rc_trim_f_value(uint8_t val);
void dpmu_set_rc_en(bool en);
void dpmu_set_rc_update_cfg(void);
void dpmu_osc_pad_cfg_fma(uint8_t num);
void dpmu_osc_pad_cfg_en(Dpmu_Xtal_Mode_t mode, FunctionalState cmd);
```

---

## Inner CODEC (ci13lc_codec.h)

The inner CODEC provides ADC (microphone input) and DAC (audio output) with programmable gain, ALC (Automatic Level Control), and high-pass filtering.

### Key Types

```c
typedef enum { INNER_CODEC_MODE_MASTER = 3, INNER_CODEC_MODE_SLAVE = 0 } inner_codec_mode_t;
typedef enum { INNER_CODEC_INPUT_MODE_DIFF = 1, INNER_CODEC_INPUT_MODE_SINGGLE_ENDED = 2 } inner_codec_input_mode_t;
typedef enum {
    INNER_CODEC_MIC_AMP_0dB, INNER_CODEC_MIC_AMP_6dB, INNER_CODEC_MIC_AMP_9dB,
    INNER_CODEC_MIC_AMP_12dB, INNER_CODEC_MIC_AMP_16dB, INNER_CODEC_MIC_AMP_20dB
} inner_codec_mic_amplify_t;
typedef enum {
    INNER_CODEC_SAMPLERATE_96K, INNER_CODEC_SAMPLERATE_48K, INNER_CODEC_SAMPLERATE_44_1K,
    INNER_CODEC_SAMPLERATE_32K, INNER_CODEC_SAMPLERATE_24K, INNER_CODEC_SAMPLERATE_16K,
    INNER_CODEC_SAMPLERATE_12K, INNER_CODEC_SAMPLERATE_8K
} inner_codec_samplerate_t;

typedef struct {
    inner_codec_input_mode_t codec_adc_input_mode_l;
    inner_codec_input_mode_t codec_adc_input_mode_r;
    inner_codec_mic_amplify_t codec_adc_mic_amp_l;
    inner_codec_mic_amplify_t codec_adc_mic_amp_r;
    float pga_gain_l;
    float pga_gain_r;
    float dig_gain_l;
    float dig_gain_r;
} inner_codec_adc_config_t;
```

> The CODEC is typically configured through `codec_manager` component (`cm_init()`) rather than direct register access. See `components/codec_manager/codec_manager.c` and `board.c` for board-specific CODEC initialization.

---

## Peripheral Base Addresses (from ci13lc.h)

```c
#define HAL_SCU_BASE        0x40000000
#define HAL_GDMA_BASE       0x40001000
#define HAL_IISDMA0_BASE    0x40003000
#define HAL_DTRFLASH_BASE   0x40004000
#define HAL_NPU_BASE        0x40006000
#define HAL_EPWM_BASE       0x40007000
#define HAL_IIC0_BASE       0x40011000
#define HAL_CODEC_BASE      0x40013000
#define HAL_PWM0_BASE       0x40014000
#define HAL_PWM1_BASE       0x40015000
#define HAL_PWM2_BASE       0x40016000
#define HAL_PWM3_BASE       0x40017000
#define HAL_TIMER0_BASE     0x40018000
#define HAL_TIMER1_BASE     0x40019000
#define HAL_PA_BASE         0x40020000
#define HAL_PB_BASE         0x40021000
#define HAL_UART0_BASE      0x40022000
#define HAL_UART1_BASE      0x40023000
#define HAL_UART2_BASE      0x40024000
#define HAL_IIS0_BASE       0x40025000
#define HAL_IIS1_BASE       0x40026000
#define HAL_DPMU_BASE       0x40030000
#define HAL_PC_BASE         0x40031000
#define HAL_IWDG_BASE       0x40032000
#define HAL_EFUSE_BASE      0x40033000
#define HAL_PVDC_BASE       0x40034000

/* FIFO bases (DMA access) */
#define SPI0FIFO_BASE       0x60000000
#define UART0FIFO_BASE      0x61000000
#define UART1FIFO_BASE      0x62000000
#define UART2FIFO_BASE      0x63000000
```

## IRQ Numbers (from ci13lc.h)

```c
typedef enum {
    SCU_IRQn   = 20,  NPU_IRQn  = 21,  EPWM_IRQn = 22,
    DMA_IRQn   = 23,  TIMER0_IRQn = 24, TIMER1_IRQn = 25,
    IIC0_IRQn  = 28,  PA_IRQn  = 29,  PB_IRQn  = 30,
    UART0_IRQn = 31,  UART1_IRQn = 32, UART2_IRQn = 33,
    IIS0_IRQn  = 34,  IIS1_IRQn = 35,  IIS_DMA_IRQn = 37,
    ALC_TIMEOUT_IRQn = 38, DTR_IRQn = 40,
    V11_OK_IRQn = 41, VDT_IRQn = 42,
    EXT0_IRQn = 43,   EXT1_IRQn = 44,  IWDG_IRQn = 45,
    PVDC_IRQn = 47,   EFUSE_IRQn = 48, PC_IRQn = 49,
} IRQn_Type;
```
