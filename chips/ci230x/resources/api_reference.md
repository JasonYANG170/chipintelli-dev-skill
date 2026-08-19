# CI230X API Reference (LN882H HAL)

> **SDK**: `CI230X_wifi_combo_sdk_release_v1.1.1`
> **Chips**: CI2305, CI2306 (LN882H Wi-Fi/BLE + CI13xx voice)
> **Architecture**: ARM Cortex-M4 (LN882H)
> **Driver Path**: `mcu/driver_ln882h/hal/`

The CI230X is a dual-chip solution: CI13xx (RISC-V, voice/ASR) + LN882H (ARM Cortex-M4, Wi-Fi/BLE). This document covers the **LN882H HAL driver APIs**. For CI13xx voice chip APIs, see `chips/ci13lc/resources/api_reference.md`.

---

## Common Types (`hal_common.h`)

```c
typedef enum {
    HAL_DISABLE = 0u,
    HAL_ENABLE  = 1u,
} hal_en_t;

typedef enum {
    HAL_OK      = 0u,
    HAL_ERROR   = 1u,
    HAL_BUSY    = 2u,
    HAL_TIMEOUT = 3u,
} hal_status_t;

#define hal_assert(expr)  // Enabled when HAL_ASSERT_EN is defined
```

**Include pattern**:
```c
#include "ln882h.h"
#include "hal/hal_common.h"
#include "hal/hal_gpio.h"
#include "hal/hal_uart.h"
// etc.
```

---

## 1. GPIO (`hal_gpio.h`)

### Types

```c
typedef enum {
    GPIO_PULL_UP = 0, GPIO_PULL_DOWN = 1, GPIO_PULL_NONE = 2, GPIO_PULL_UP_AND_PULL_DOWN = 3,
} gpio_pull_t;

typedef enum {
    GPIO_NORMAL_SPEED = 0, GPIO_HIGH_SPEED = 1,
} gpio_speed_t;

typedef enum {
    GPIO_MODE_DIGITAL = 0, GPIO_MODE_ANALOG = 1, GPIO_MODE_AFIO = 2,
} gpio_mode_t;

typedef enum {
    GPIO_INPUT = 0, GPIO_OUTPUT = 1,
} gpio_direction_t;

typedef enum {
    GPIO_INT_RISING = 0, GPIO_INT_FALLING = 1, GPIO_INT_RISING_FALLING = 2,
} gpio_int_type_t;

typedef enum {
    GPIO_PIN_0 = 0x0001, GPIO_PIN_1 = 0x0002, /* ... */ GPIO_PIN_15 = 0x8000, GPIO_PIN_All = 0xFFFF,
} gpio_pin_t;

// Alternate functions
typedef enum {
    I2C0_SCL=0, I2C0_SDA=1,
    UART0_TX=2, UART0_RX=3, UART0_RTS=4, UART0_CTS=5,
    UART1_TX=6, UART1_RX=7, UART2_TX=8, UART2_RX=9,
    TIMER0_PWM=10, TIMER1_PWM=11, TIMER2_PWM=12, TIMER3_PWM=13,
    ADV_TIMER_PWM0=14, /* ... */ ADV_TIMER_PWM11=25,
    SPI0_CLK=26, SPI0_CSN=27, SPI0_MOSI=28, SPI0_MISO=29,
    SPI1_CLK=30, SPI1_CSN=31, SPI1_MOSI=32, SPI1_MISO=33,
    WS2811_OUT=34, CLK_TEST0=35, CLK_TEST1=36, TX_IND=37, RX_IND=38, WIFI_BLE_IND=39,
} afio_function_t;

typedef struct {
    gpio_pin_t       pin;
    gpio_pull_t      pull;
    gpio_speed_t     speed;
    gpio_mode_t      mode;
    gpio_direction_t dir;
} gpio_init_t_def;
```

### Functions

```c
void     hal_gpio_init(uint32_t gpio_base, gpio_init_t_def *gpio_init);
void     hal_gpio_deinit(uint32_t gpio_base);
void     hal_gpio_pin_pull_set(uint32_t gpio_base, gpio_pin_t pin, gpio_pull_t pull);
void     hal_gpio_pin_speed_set(uint32_t gpio_base, gpio_pin_t pin, gpio_speed_t speed);
void     hal_gpio_pin_mode_set(uint32_t gpio_base, gpio_pin_t pin, gpio_mode_t mode);
void     hal_gpio_pin_direction_set(uint32_t gpio_base, gpio_pin_t pin, gpio_direction_t dir);

uint16_t hal_gpio_port_input_read(uint32_t gpio_base);
uint16_t hal_gpio_port_output_read(uint32_t gpio_base);
uint8_t  hal_gpio_pin_input_read(uint32_t gpio_base, gpio_pin_t pin);
uint8_t  hal_gpio_pin_output_read(uint32_t gpio_base, gpio_pin_t pin);
uint8_t  hal_gpio_pin_read(uint32_t gpio_base, gpio_pin_t pin);
void     hal_gpio_port_output_write(uint32_t gpio_base, uint16_t port_val);
void     hal_gpio_pin_set(uint32_t gpio_base, gpio_pin_t pin);
void     hal_gpio_pin_reset(uint32_t gpio_base, gpio_pin_t pin);
void     hal_gpio_pin_toggle(uint32_t gpio_base, gpio_pin_t pin);

void     hal_gpio_pin_it_en(uint32_t gpio_base, gpio_pin_t pin, hal_en_t en);
void     hal_gpio_pin_it_cfg(uint32_t gpio_base, gpio_pin_t pin, gpio_int_type_t type);
uint8_t  hal_gpio_pin_get_it_flag(uint32_t gpio_base, gpio_pin_t pin);
void     hal_gpio_pin_clr_it_flag(uint32_t gpio_base, gpio_pin_t pin);

void     hal_gpio_pin_afio_select(uint32_t gpio_base, gpio_pin_t pin, afio_function_t fun);
void     hal_gpio_pin_afio_en(uint32_t gpio_base, gpio_pin_t pin, hal_en_t en);
```

**GPIO bases**: `GPIOA_BASE`, `GPIOB_BASE`. GPIOA supports AFIO on pins 0-12, GPIOB on pins 3-9.

---

## 2. UART (`hal_uart.h`)

### Types

```c
typedef enum { UART_WORD_LEN_8 = 0, UART_WORD_LEN_9 = 1, } uart_word_len_enum;
typedef enum { UART_OVER_SAMPL_16 = 0, UART_OVER_SAMPL_8 = 1, } uart_over_sampl_enum;
typedef enum { UART_PARITY_EVEN = 0, UART_PARITY_ODD = 1, UART_PARITY_NONE = 2, } uart_parity_enum;
typedef enum { UART_STOP_BITS_1=0, UART_STOP_BITS_0_5=1, UART_STOP_BITS_2=2, UART_STOP_BITS_1_5=3, } uart_stop_bits_enum;
typedef enum { UART_HW_FLOW_CTRL_NONE=0, UART_HW_FLOW_CTRL_RTS=0x100, UART_HW_FLOW_CTRL_CTS=0x200, UART_HW_FLOW_CTRL_RTS_CTS=0x300, } uart_hw_flow_ctrl_enum;

typedef struct {
    uint32_t baudrate;
    uint16_t word_len;
    uint16_t stop_bits;
    uint16_t parity;
    uint16_t over_sampl;
} uart_init_t_def;
```

### Functions

```c
void     hal_uart_init(uint32_t uart_base, uart_init_t_def *uart_init);
void     hal_uart_deinit(uint32_t uart_base);
void     hal_uart_send_data(uint32_t uart_base, uint16_t data);
uint16_t hal_uart_recv_data(uint32_t uart_base);

void     hal_uart_rx_mode_en(uint32_t uart_base, hal_en_t en);
void     hal_uart_tx_mode_en(uint32_t uart_base, hal_en_t en);
void     hal_uart_hardware_flow_rts_en(uint32_t uart_base, hal_en_t en);
void     hal_uart_hardware_flow_cts_en(uint32_t uart_base, hal_en_t en);
void     hal_uart_en(uint32_t uart_base, hal_en_t en);
void     hal_uart_dma_en(uint32_t uart_base, uart_dma_req_enum dma_req, hal_en_t en);
void     hal_uart_baudrate_set(uint32_t uart_base, uint32_t baudrate);

void     hal_uart_it_en(uint32_t uart_base, uart_it_en_enum it_en);
void     hal_uart_it_disable(uint32_t uart_base, uart_it_en_enum it_en);
void     hal_uart_it_flag_clear(uint32_t uart_base, uart_it_flag_clear_enum it_flag_clear);
uint8_t  hal_uart_flag_get(uint32_t uart_base, uart_flag_enum flag);
uint8_t  hal_uart_it_en_status_get(uint32_t uart_base, uart_it_en_enum it_en);
void     hal_uart_clr_over_run_err(uint32_t uart_base);

uint8_t  hal_uart_tx_fifo_level_get(uint32_t uart_base);
uint8_t  hal_uart_rx_fifo_level_get(uint32_t uart_base);
void     hal_uart_rx_fifo_it_trig_level_set(uint32_t uart_base, uint8_t level);
```

**UART bases**: `UART0_BASE`, `UART1_BASE`, `UART2_BASE`.

---

## 3. I2C (`hal_i2c.h`)

### Types

```c
typedef enum { I2C_ADD_7BIT_MODE = 0, I2C_ADD_10BIT_MODE = 1, } i2c_add_mode_t;
typedef enum { I2C_SM_MODE = 0, I2C_FM_MODE = 1, } i2c_master_mode_sel_t;
typedef enum { I2C_ACK_DIS = 0, I2C_ACK_EN = 1, } i2c_ack_en_t;

typedef struct {
    uint8_t                  i2c_peripheral_clock_freq;
    i2c_sw_rst_t             i2c_sw_rst;
    i2c_smbus_alert_model_t  i2c_smbus_alert_model;
    i2c_pec_t                i2c_pec;
    i2c_poc_t                i2c_poc;
    i2c_ack_en_t             i2c_ack_en;
    i2c_clock_stretch_dis_t  i2c_clock_stretch_dis;
    i2c_general_call_en_t    i2c_general_call_en;
    i2c_pec_en_t             i2c_pec_en;
    i2c_arp_en_t             i2c_arp_en;
    i2c_smbus_type_t         i2c_smbus_type;
    i2c_mode_t               i2c_mode;
    i2c_add_mode_t           i2c_add_mode;
    i2c_master_mode_sel_t    i2c_master_mode_sel;
    i2c_fm_mode_duty_cycle_t i2c_fm_mode_duty_cycle;
    uint16_t                 i2c_ccr;      // SCL clock divider (1-2047)
    uint8_t                  i2c_trise;    // Max rise time (1-31)
} i2c_init_t_def;
```

### Functions

```c
void     hal_i2c_init(uint32_t i2c_x_base, i2c_init_t_def *i2c_init);
void     hal_i2c_deinit(void);
void     hal_i2c_set_peripheral_clock_freq(uint32_t i2c_x_base, uint32_t peripheral_clock_freq);
void     hal_i2c_en(uint32_t i2c_x_base, hal_en_t en);
void     hal_i2c_dma_en(uint32_t i2c_x_base, hal_en_t en);
void     hal_i2c_ack_en(uint32_t i2c_x_base, hal_en_t en);

// Master operations
void     hal_i2c_master_reset(uint32_t i2c_x_base);
uint8_t  hal_i2c_master_start(uint32_t i2c_x_base, uint32_t timeout);
void     hal_i2c_master_stop(uint32_t i2c_x_base);
void     hal_i2c_master_send_data(uint32_t i2c_x_base, uint8_t data);
uint8_t  hal_i2c_master_recv_data(uint32_t i2c_x_base);
uint8_t  hal_i2c_master_wait_addr(uint32_t i2c_x_base, uint32_t timeout);
uint8_t  hal_i2c_master_wait_add10(uint32_t i2c_x_base, uint32_t timeout);

// Slave operations
void     hal_i2c_slave_set_add_mode(uint32_t i2c_x_base, i2c_add_mode_t add_mode);
void     hal_i2c_slave_set_add1(uint32_t i2c_x_base, uint16_t add);
void     hal_i2c_slave_set_add2(uint32_t i2c_x_base, uint16_t add);
uint8_t  hal_i2c_slave_wait_addr(uint32_t i2c_x_base, uint32_t timeout);

// Universal
uint8_t  hal_i2c_wait_txe(uint32_t i2c_x_base, uint32_t timeout);
uint8_t  hal_i2c_wait_rxne(uint32_t i2c_x_base, uint32_t timeout);
uint8_t  hal_i2c_wait_btf(uint32_t i2c_x_base, uint32_t timeout);
uint8_t  hal_i2c_wait_bus_idle(uint32_t i2c_x_base, uint32_t timeout);
void     hal_i2c_clear_sr(uint32_t i2c_x_base);

// Interrupt
void     hal_i2c_it_cfg(uint32_t i2c_x_base, i2c_it_flag_t i2c_it_flag, hal_en_t en);
uint8_t  hal_i2c_get_it_flag(uint32_t i2c_x_base, i2c_it_flag_t i2c_it_flag);
void     hal_i2c_clr_it_flag(uint32_t i2c_x_base, i2c_it_flag_t i2c_it_flag);
uint8_t  hal_i2c_get_status_flag(uint32_t i2c_x_base, i2c_status_flag_t i2c_status_flag);
void     hal_i2c_clr_status_flag(uint32_t i2c_x_base, i2c_status_flag_t i2c_status_flag);
```

**I2C base**: `I2C_BASE` (single I2C controller).

---

## 4. SPI (`hal_spi.h`)

### Types

```c
typedef enum { SPI_DIRECTION_2LINES_FULLDUPLEX=0, SPI_DIRECTION_2LINES_RXONLY=1, SPI_DIRECTION_1LINE_RX=2, SPI_DIRECTION_1LINE_TX=3, } spi_direction_t;
typedef enum { SPI_MODE_SLAVE=0, SPI_MODE_MASTER=1, } spi_mode_t;
typedef enum { SPI_DATASIZE_16B=0, SPI_DATASIZE_8B=1, } spi_data_size_t;
typedef enum { SPI_CPOL_LOW=0, SPI_CPOL_HIGH=1, } spi_cpol_t;
typedef enum { SPI_CPHA_1EDGE=0, SPI_CPHA_2EDGE=1, } spi_cpha_t;
typedef enum { SPI_NSS_HARD=0, SPI_NSS_SOFT=1, } spi_nss_model_t;
typedef enum { SPI_BAUDRATEPRESCALER_2=0x01, /* ... */ SPI_BAUDRATEPRESCALER_256=0x08, } spi_baud_rate_precaler_t;
typedef enum { SPI_FIRST_BIT_MSB=0, SPI_FIRST_BIT_LSB=1, } spi_first_bit_t;

typedef struct {
    spi_direction_t          spi_direction;
    spi_mode_t               spi_mode;
    spi_data_size_t          spi_data_size;
    spi_cpol_t               spi_cpol;
    spi_cpha_t               spi_cpha;
    spi_nss_model_t          spi_nss_mode;
    spi_baud_rate_precaler_t spi_baud_rate_prescaler;
    spi_first_bit_t          spi_first_bit;
    uint16_t                 spi_crc_polynomial;
} spi_init_type_def;
```

### Functions

```c
void     hal_spi_init(uint32_t spi_x_base, spi_init_type_def *spi_init);
void     hal_spi_deinit(uint32_t spi_x_base);
void     hal_spi_set_nss(uint32_t spi_x_base, spi_nss_model_t software_config);
void     hal_spi_ssoe_en(uint32_t spi_x_base, hal_en_t en);
void     hal_spi_set_data_size(uint32_t spi_x_base, spi_data_size_t data_size);
void     hal_spi_set_bidirectional_line(uint32_t spi_x_base, spi_direction_t spi_direction);
void     hal_spi_en(uint32_t spi_x_base, hal_en_t en);
void     hal_spi_dma_en(uint32_t spi_x_base, spi_dma_en_t spi_dma_en, hal_en_t en);

void     hal_spi_send_data(uint32_t spi_x_base, uint16_t data);
uint16_t hal_spi_recv_data(uint32_t spi_x_base);
uint8_t  hal_spi_wait_txe(uint32_t spi_x_base, uint32_t timeout);
uint8_t  hal_spi_wait_rxne(uint32_t spi_x_base, uint32_t timeout);
uint8_t  hal_spi_wait_bus_idle(uint32_t spi_x_base, uint32_t timeout);

// CRC
void     hal_spi_transmit_crc(uint32_t spi_x_base);
void     hal_spi_calculate_crc_en(uint32_t spi_x_base, hal_en_t en);
uint16_t hal_spi_get_crc(uint32_t spi_x_base, spi_crc_model_t spi_crc_model);

// Interrupt
void     hal_spi_it_cfg(uint32_t spi_x_base, spi_it_flag_t spi_it, hal_en_t en);
uint8_t  hal_spi_get_status_flag(uint32_t spi_x_base, spi_status_flag_t spi_status_flag);
void     hal_spi_clr_status_flag(uint32_t spi_x_base, spi_status_flag_t spi_status_flag);
uint8_t  hal_spi_get_it_flag(uint32_t spi_x_base, spi_it_flag_t spi_it_flag);
void     hal_spi_clr_it_flag(uint32_t spi_x_base, spi_it_flag_t spi_it_flag);
```

**SPI bases**: `SPI0_BASE`, `SPI1_BASE`.

---

## 5. ADC (`hal_adc.h`)

### Types

```c
typedef enum { ADC_CH0=1<<0, ADC_CH1=1<<1, /* ... */ ADC_CH7=1<<7, } adc_ch_t;
typedef enum { ADC_CONV_MODE_SINGLE=0, ADC_CONV_MODE_CONTINUE=1, } adc_conv_mode_t;
typedef enum { ADC_DATA_ALIGN_RIGHT=0, ADC_DATA_ALIGN_LEFT=1, } adc_data_align_mode_t;
typedef enum { ADC_VREF_0_8_V=0, ADC_VREF_0_8_5_V=1, ADC_VREF_0_9_5_V=2, ADC_VREF_1_0_5_V=3, } adc_vref_set_t;
typedef enum { ADC_OVER_SAMPLING_RATIO_X2=0, X4=1, X5=2, X16=3, X32=4, X64=5, } adc_ov_smp_ratio_t;

typedef struct {
    adc_ch_t                adc_awd_ch;
    adc_awd_sgl_t           adc_awd_sgl;
    adc_auto_off_mode_t     adc_auto_off_mode;
    adc_wait_conv_mode_en_t adc_wait_conv_mode_en;
    adc_conv_mode_t         adc_conv_mode;
    adc_data_align_mode_t   adc_data_align_mode;
    /* ... trigger config, oversampling, watchdog thresholds ... */
    adc_ch_t                adc_ch;
    uint8_t                 adc_presc;    // prescaler = (n+1)*2
    adc_vref_set_t          adc_vref_set;
} adc_init_t_def;
```

### Functions

```c
void     hal_adc_init(uint32_t adc_base, adc_init_t_def *adc_init);
void     hal_adc_deinit(void);
void     hal_adc_dma_en(uint32_t adc_base, hal_en_t en);
void     hal_adc_awd_en(uint32_t adc_base, hal_en_t en);
void     hal_adc_en(uint32_t adc_base, hal_en_t en);

void     hal_adc_start_conv(uint32_t adc_base);
void     hal_adc_stop_conv(uint32_t adc_base);
uint8_t  hal_adc_get_conv_status(uint32_t adc_base, adc_ch_t ch);
void     hal_adc_clr_conv_status(uint32_t adc_base, adc_ch_t ch);
void     hal_adc_spe_sw_start(uint32_t adc_base);
uint16_t hal_adc_get_data(uint32_t adc_base, adc_ch_t ch);

void     hal_adc_it_cfg(uint32_t adc_base, adc_it_flag_t adc_it_flag, hal_en_t en);
uint8_t  hal_adc_get_it_flag(uint32_t adc_base, adc_it_flag_t adc_it_flag);
void     hal_adc_clr_it_flag(uint32_t adc_base, adc_it_flag_t adc_it_flag);
```

**ADC base**: `ADC_BASE`. 8 channels (CH0 = internal temperature sensor). Note: CH0 is internal temperature, CH1-CH7 are external.

---

## 6. Flash (`hal_flash.h`)

### Constants

```c
#define FLASH_PAGE_SIZE         (256)
#define FALSH_SIZE_4K           (4 *1024)
#define FALSH_SIZE_BLOCK_32K    (32*1024)
#define FALSH_SIZE_BLOCK_64K    (64*1024)
#define FALSH_SIZE_MAX          (4 *1024*1024)  // 4MB max
#define FLASH_SECURITY_SIZE_MAX (4*256)
```

### Functions

```c
void     hal_flash_init(void);
void     hal_flash_deinit(void);
uint8_t  hal_flash_read_by_cache(uint32_t offset, uint32_t length, uint8_t *buffer);
uint8_t  hal_flash_read(uint32_t offset, uint32_t length, uint8_t *buffer);
uint8_t  hal_flash_program(uint32_t offset, uint32_t length, uint8_t *buffer);
void     hal_flash_erase(uint32_t offset, uint32_t length);  // Must be 4K aligned!
void     hal_flash_chip_erase(void);
uint32_t hal_flash_read_id(void);
uint16_t hal_flash_read_device_id(void);

void     hal_flash_erase_type(uint32_t offset, flash_erase_type_t type);
uint8_t  hal_flash_read_sr1(void);
uint8_t  hal_flash_read_sr2(void);
void     hal_flash_quad_mode_enable(uint8_t enable);
void     hal_flash_operation_wait(void);

// Security area (OTP)
void     hal_flash_security_area_erase(uint32_t offset);
uint8_t  hal_flash_security_area_program(uint32_t offset, uint32_t len, uint8_t *buf);
void     hal_flash_security_area_read(uint32_t offset, uint32_t len, uint8_t *buf);

// Suspend/Resume
void     hal_flash_program_erase_suspend(void);
void     hal_flash_program_erase_resume(void);
```

**Erase types**:
```c
typedef enum { ERASE_SECTOR_4KB, ERASE_BLOCK_32KB, ERASE_BLOCK_64KB, ERASE_CHIP, } flash_erase_type_t;
```

---

## 7. DMA (`hal_dma.h`)

### Channel Mapping

```
DMA_CH_1 -> QSPI_TX, QSPI_RX
DMA_CH_2 -> WS2811_OUT
DMA_CH_3 -> SPI1_RX, SPI0_RX
DMA_CH_4 -> SPI0_TX, SPI1_TX
DMA_CH_5 -> UART0_RX, UART1_RX, UART2_RX
DMA_CH_6 -> UART0_TX, UART1_TX, UART2_TX
DMA_CH_7 -> I2C0, ADC
```

### Types

```c
typedef enum { DMA_READ_FORM_P=0, DMA_READ_FORM_MEM=1, } dma_dir_t;
typedef enum { DMA_PRI_LEV_LOW=0, DMA_PRI_LEV_MEDIUM=1, DMA_PRI_LEV_HIGH=2, } dma_pri_lev_t;
typedef enum { DMA_MEM_SIZE_8_BIT=0, DMA_MEM_SIZE_16_BIT=1, DMA_MEM_SIZE_32_BIT=2, } dma_mem_size_t;
typedef enum { DMA_MEM_INC_DIS=0, DMA_MEM_INC_EN=1, } dma_mem_inc_en_t;
typedef enum { DMA_CIRC_MODE_DIS=0, DMA_CIRC_MODE_EN=1, } dma_circ_mode_en_t;

typedef struct {
    dma_mem_to_mem_en_t dma_mem_to_mem_en;
    dma_pri_lev_t       dma_pri_lev;
    dma_mem_size_t      dma_mem_size;
    dma_p_size_t        dma_p_size;
    dma_mem_inc_en_t    dma_mem_inc_en;
    dma_p_inc_en_t      dma_p_inc_en;
    dma_circ_mode_en_t  dma_circ_mode_en;
    dma_dir_t           dma_dir;
    uint16_t            dma_data_num;
    uint32_t            dma_mem_addr;
    uint32_t            dma_p_addr;
} dma_init_t_def;
```

### Functions

```c
void     hal_dma_init(uint32_t dma_x_base, dma_init_t_def *dma_init);
void     hal_dma_deinit(void);
void     hal_dma_en(uint32_t dma_x_base, hal_en_t en);
void     hal_dma_set_dir(uint32_t dma_x_base, dma_dir_t dma_dir);
void     hal_dma_set_mem_addr(uint32_t dma_x_base, uint32_t dma_mem_addr);
void     hal_dma_set_p_addr(uint32_t dma_x_base, uint32_t dma_p_addr);
void     hal_dma_set_data_num(uint32_t dma_x_base, uint16_t dma_data_num);
void     hal_dma_set_mem_size(uint32_t dma_x_base, dma_mem_size_t dma_mem_size);
void     hal_dma_set_p_size(uint32_t dma_x_base, dma_p_size_t dma_p_size);
void     hal_dma_set_pri_lev(uint32_t dma_x_base, dma_pri_lev_t dma_pri_lev);
void     hal_dma_set_mem_inc_en(uint32_t dma_x_base, dma_mem_inc_en_t dma_mem_inc_en);
void     hal_dma_set_p_inc_en(uint32_t dma_x_base, dma_p_inc_en_t dma_p_inc_en);
uint16_t hal_dma_get_data_num(uint32_t dma_x_base);

void     hal_dma_it_cfg(uint32_t dma_x_base, dma_it_flag_t dma_it_flag, hal_en_t en);
uint8_t  hal_dma_get_it_flag(uint32_t dma_x_base, dma_it_flag_t dma_it_flag);
void     hal_dma_clr_it_flag(uint32_t dma_x_base, dma_it_flag_t dma_it_flag);
```

---

## 8. Clock (`hal_clock.h`)

### Types

```c
typedef enum { CLK_SRC_XTAL=0, CLK_SRC_PLL=1, } clk_src_t;
typedef enum { CLK_PLL_CLK_NO_MUL=2, CLK_PLL_CLK_1_MUL=2, CLK_PLL_CLK_1_5_MUL=3, /* ... */ CLK_PLL_CLK_4_MUL=8, } clk_pllclk_mul_t;
typedef enum { CLK_PCLK0_NO_DIV=1, /* ... */ CLK_PCLK0_16_DIV=16, } clk_pclk0_div_t;
typedef enum { CLK_HCLK_NO_DIV=1, /* ... */ CLK_HCLK_16_DIV=16, } clk_hclk_div_t;

typedef struct {
    clk_src_t        clk_src;
    clk_pllclk_mul_t clk_pllclk_mul;
    clk_pclk0_div_t  clk_pclk0_div;
    clk_hclk_div_t   clk_hclk_div;
} clock_init_t;
```

### Functions

```c
void      hal_clock_init(clock_init_t *clock_init);
void      hal_clock_select_clk_src(clk_src_t clk_src);
void      hal_clock_set_pll_clk_mul(clk_pllclk_mul_t clk_pllclk_mul);
void      hal_clock_set_apb0_clk_div(clk_pclk0_div_t clk_pclk0_div);
void      hal_clock_set_ahb_clk_div(clk_hclk_div_t clk_hclk_div);
clk_src_t hal_clock_get_clk_src(void);
uint32_t  hal_clock_get_core_clk(void);
uint32_t  hal_clock_get_apb0_clk(void);
uint32_t  hal_clock_get_ahb_clk(void);
```

**Clock formula**:
- With PLL: `Core = XTAL * PLL_MUL`, `AHB = Core / HCLK_DIV`, `APB = AHB / PCLK0_DIV`
- Without PLL: `Core = XTAL`, `AHB = XTAL / HCLK_DIV`, `APB = AHB / PCLK0_DIV`

---

## 9. External Interrupt (`hal_ext.h`)

### Types

```c
typedef enum {
    EXT_INT_SENSE_0=0,  // PA0
    EXT_INT_SENSE_1=1,  // PA1
    EXT_INT_SENSE_2=2,  // PA2
    EXT_INT_SENSE_3=3,  // PA3
    EXT_INT_SENSE_4=4,  // PA5
    EXT_INT_SENSE_5=5,  // PA6
    EXT_INT_SENSE_6=6,  // PA7
    EXT_INT_SENSE_7=7,  // PB9
} ext_int_sense_t;

typedef enum {
    EXT_INT_HIGH_LEVEL=0, EXT_INT_LOW_LEVEL=1, EXT_INT_POSEDEG=2, EXT_INT_NEGEDGE=3,
} ext_trig_mode_t;
```

### Functions

```c
void     hal_ext_init(ext_int_sense_t ext_int_sense, ext_trig_mode_t ext_trig_mode, hal_en_t en);
void     hal_ext_deinit(void);
uint8_t  hal_ext_get_it_flag(ext_it_flag_t ext_it_flag);
uint8_t  hal_ext_get_raw_it_flag(ext_it_raw_flag_t ext_raw_it_flag);
void     hal_ext_clr_it_flag(ext_it_flag_t ext_it_flag);
```

---

## 10. Interrupt (`hal_interrupt.h`)

```c
void set_interrupt_priority(void);
void switch_global_interrupt(hal_en_t enable);
```

---

## 11. Timer (`hal_timer.h`)

### Types

```c
typedef enum { TIM_FREE_RUNNING_MODE=0, TIM_USER_DEF_CNT_MODE=1, } tim_mode_t;

typedef struct {
    tim_mode_t  tim_mode;
    uint32_t    tim_load_value;
    uint32_t    tim_load2_value;
    uint8_t     tim_div;  // 0=no div, 1=div2, 2=div3, ...
} tim_init_t_def;
```

### Functions

```c
void     hal_tim_init(uint32_t tim_x_base, tim_init_t_def *tim_init);
void     hal_tim_deinit(void);
void     hal_tim_en(uint32_t tim_x_base, hal_en_t en);
void     hal_tim_set_load_value(uint32_t tim_x_base, uint32_t value);
void     hal_tim_set_load2_value(uint32_t tim_x_base, uint32_t value);
uint32_t hal_tim_get_current_cnt_value(uint32_t tim_x_base);
void     hal_tim_pwm_en(uint32_t tim_x_base, hal_en_t en);
uint8_t  hal_tim_get_div(uint32_t tim_x_base);
uint32_t hal_tim_get_load_value(uint32_t tim_x_base);
uint32_t hal_tim_get_load2_value(uint32_t tim_x_base);

void     hal_tim_it_cfg(uint32_t tim_x_base, tim_it_flag_t tim_it_flag, hal_en_t en);
uint8_t  hal_tim_get_it_flag(uint32_t tim_x_base, tim_it_flag_t tim_it_flag);
void     hal_tim_clr_it_flag(uint32_t tim_x_base, tim_it_flag_t tim_it_flag);
```

**Timer bases**: `TIMER0_BASE`, `TIMER1_BASE`, `TIMER2_BASE`, `TIMER3_BASE`.
**Warning**: `TIMER3_BASE` is used by the WiFi library -- do not use it in application code.

---

## 12. Other HAL Modules

| Module | Header | Key Functions |
|--------|--------|--------------|
| Advanced Timer | `hal_adv_timer.h` | PWM output with advanced features |
| Watchdog | `hal_wdt.h` | Watchdog timer init and feed |
| RTC | `hal_rtc.h` | Real-time clock |
| QSPI | `hal_qspi.h` | QSPI flash controller (used by hal_flash) |
| Cache | `hal_cache.h` | Instruction/data cache control |
| TRNG | `hal_trng.h` | True random number generator |
| AES | `hal_aes.h` | Hardware AES encryption |
| EFuse | `hal_efuse.h` | eFuse read/write |
| I2S | `hal_i2s.h` | I2S audio interface |
| WS2811 | `hal_ws2811.h` | WS2811 LED driver |
| SDIO (device) | `hal_sdio_device.h` | SDIO device mode (for CI13xx communication) |
| SDIO (room) | `hal_sdio_room.h` | SDIO communication room |
| Misc | `hal_misc.h` | Miscellaneous hardware control |
| IT | `hal_it.h` | Interrupt handlers |
