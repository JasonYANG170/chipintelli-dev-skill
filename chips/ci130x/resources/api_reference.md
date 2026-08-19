# CI130X Peripheral API Reference

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> Header source: `driver/ci130x_chip_driver/inc/`  
> All functions use C linkage (`extern "C"`). Include the corresponding `ci130x_<peripheral>.h` header.

---

## Table of Contents

- [SCU (System Control Unit)](#scu-system-control-unit)
- [DPMU (Power Management Unit)](#dpmu-power-management-unit)
- [GPIO](#gpio)
- [UART](#uart)
- [I2C](#i2c)
- [IIS (I2S Audio)](#iis-i2s-audio)
- [IISDMA](#iisdma)
- [ADC](#adc)
- [PWM](#pwm)
- [Timer](#timer)
- [DMA](#dma)
- [SPIFlash](#spiflash)
- [DTRFlash (QSPI Controller)](#dtrflash-qspi-controller)
- [Watchdog (IWDG)](#watchdog-iwdg)
- [Window Watchdog (TWDG)](#window-watchdog-twdg)
- [Codec (Inner CODEC)](#codec-inner-codec)
- [ALC (Auxiliary ALC)](#alc-auxiliary-alc)
- [PDM](#pdm)
- [Mailbox](#mailbox)
- [LowPower](#lowpower)
- [Cache](#cache)

---

## SCU (System Control Unit)

**Header**: `ci130x_scu.h`

### Key Defines

| Define | Value | Description |
|--------|-------|-------------|
| `MAIN_FREQUENCY` | 240000000 (external crystal) / 200000000 (internal RC) | Main PLL frequency |
| `SRC_FREQUENCY_NORMAL` | 12288000 | SRC clock frequency (normal mode) |
| `SRC_FREQUENCY_LOWPOWER` | 12288000 | SRC clock frequency (low-power mode) |

### Enums

#### `Nmi_Irq_t` - NMI interrupt source
| Value | Name | Description |
|-------|------|-------------|
| 0x1 | `NMI_IRQ_IWGD` | IWDG NMI |
| 0x2 | `NMI_IRQ_WWGD` | Window watchdog NMI |
| 0x3 | `NMI_IRQ_EXT0` | External interrupt 0 |
| 0x4 | `NMI_IRQ_EXT1` | External interrupt 1 |
| 0x5 | `NMI_IRQ_TIMER0` | Timer0 NMI |
| 0x6 | `NMI_IRQ_TIMER1` | Timer1 NMI |
| 0x7-0x9 | `NMI_IRQ_UART0/1/2` | UART NMI |
| 0xA-0xC | `NMI_IRQ_PA/PB/PC` | GPIO NMI |
| 0xE | `NMI_IRQ_V11_OK` | Voltage OK NMI |
| 0xF | `NMI_IRQ_ADC` | ADC NMI |

#### `Sys_Clk_Gate_t` - System clock gate selection
| Value | Name |
|-------|------|
| 0 | `SLEEPING_GATE` |
| 1 | `SLEEPDEEP_GATE` |
| 2 | `CPU_CORECLK_GATE` |
| 3 | `STCLK_GATE` |
| 4-10 | `SRAM0_GATE` .. `SRAM6_GATE` |
| 11 | `ROM_GATE` |

#### `PinPad_Name` - Pin pad enumeration
Complete pad list: `PA0`..`PA7`, `PB0`..`PB7`, `PC0`..`PC5`, `PD0`..`PD5`, plus special pads: `BOOT_SEL_0_PAD`, `SPI0_CS_PAD`, `SPI0_D0_PAD`..`SPI0_D3_PAD`, `SPI0_CLK_PAD`, `KEY_RSTN_PAD`, `TEST_EN_PAD`.

Each pad has up to 6 functions (1st through 6th). See the comment block in `ci130x_scu.h` for the full pinmux table.

#### `IOResue_FUNCTION` - Pin function select
| Value | Name |
|-------|------|
| 0 | `FIRST_FUNCTION` |
| 1 | `SECOND_FUNCTION` |
| 2 | `THIRD_FUNCTION` |
| 3 | `FORTH_FUNCTION` |
| 4 | `FIFTH_FUNCTION` |
| 5 | `SIXTH_FUNCTION` |

### Functions

| Signature | Description |
|-----------|-------------|
| `void scu_unlock_system_config(void)` | Unlock system control registers for writing |
| `void scu_unlock_reset_config(void)` | Unlock reset-related registers |
| `void scu_unlock_clk_config(void)` | Unlock clock-related registers |
| `void scu_lock_system_config(void)` | Lock system control registers |
| `void scu_lock_clk_config(void)` | Lock clock-related registers |
| `void scu_lock_reset_config(void)` | Lock reset registers |
| `int32_t scu_set_device_gate(uint32_t device_base, int32_t gate)` | Enable/disable peripheral clock (ENABLE/DISABLE) |
| `int32_t scu_set_device_reset(uint32_t device_base)` | Assert peripheral reset |
| `int32_t scu_set_device_reset_release(uint32_t device_base)` | Release peripheral reset |
| `int32_t scu_get_system_reset_state(void)` | Get system reset state (RETURN_OK=normal power-on, PARA_ERROR=abnormal) |
| `int32_t scu_set_ext_wakeup_int(Ext_Num num, FunctionalState cmd)` | Configure external wakeup interrupt |
| `int32_t scu_clear_ext_int_state(Ext_Num num)` | Clear external interrupt state |
| `void scu_set_ext_filter_config(Ext_Num num, FunctionalState cmd, uint32_t param)` | Configure external interrupt filter |
| `int8_t scu_para_en_disable(uint32_t device_base)` | Disable peripheral clock gate (para) |
| `int8_t scu_para_en_enable(uint32_t device_base)` | Enable peripheral clock gate (para) |
| `int32_t scu_set_div_parameter(uint32_t device_base, uint32_t div_num)` | Set clock divider parameter for a peripheral |
| `void scu_spiflash_no_boot_set(void)` | Set SPIFlash to non-boot mode |
| `void scu_run_in_flash(void)` | Configure system to run from flash |
| `void scu_run_not_in_flash(void)` | Configure system to run from RAM |
| `int32_t scu_set_system_clk_gate(Sys_Clk_Gate_t base, FunctionalState gate)` | Gate/ungate system clock (SRAM, ROM, etc.) |
| `void scu_set_wwdg_halt(void)` | Halt window watchdog |
| `void scu_clean_wwdg_halt(void)` | Clear window watchdog halt |
| `void scu_set_dma_mode(DmaInt_Sel_t channel)` | Select DMA interrupt channel |
| `void scu_sel_dtrflash_clk(Dtr_Clk_Sel_t clk)` | Select DTRFlash clock source |
| `void scu_wait_pll_lock_state(void)` | Wait for PLL to lock |
| `void scu_iis_src_config(IIS_Src_Config_t *config, IIS_Mclk_Source_t src)` | Configure IIS SRC clock source |
| `void scu_iis_mclk_Config(IIS_Mclk_Config_t *config)` | Configure IIS MCLK |
| `void scu_iis_pad_mclk_config(IIS_Mclk_Source_t src, IIS_Clk_Mode_t mode)` | Configure IIS MCLK pad output/input |
| `void scu_iis_codec_mclk_config(Codec_Channel_t channel, IIS_Mclk_Source_t src)` | Configure codec MCLK source |
| `void scu_iis_pdm_mclk_config(IIS_Mclk_Source_t src)` | Configure PDM MCLK source |
| `void scu_iis_clk_config(IISNumx device, IIS_Clk_Source_t clk_source)` | Configure IIS SCK/LRCK source |
| `void scu_iis_pad_clk_config(IIS_Clk_Source_t clk_source, IIS_Clk_Mode_t mode)` | Configure IIS SCK/LRCK pad output/input |
| `void scu_iis_codec_dac_data_config(Codec_Dac_Data_Sel_t src)` | Select codec DAC data source |
| `void scu_iis_pad_data_config(Pad_IIS_Data_Sel_t src)` | Select pad IIS output data source |
| `void iis_clk_config(IIS_Clk_ConfigTypedef* config)` | Full IIS clock configuration |
| `void scu_rc_trim_en(bool en)` | Enable/disable RC trim |
| `void scu_rc_trim_state_clear(void)` | Clear RC trim state |
| `void scu_nmi_irq_cfg(Nmi_Irq_t irq)` | Configure NMI interrupt source |
| `uint8_t scu_get_rc_trim_state(void)` | Get RC trim state |
| `uint8_t scu_get_rc_trim_c_value(void)` | Get RC trim C value |
| `uint8_t scu_get_rc_trim_f_value(void)` | Get RC trim F value |
| `void dsu_init(unsigned int addr)` | Initialize DSU (debug support unit) |

---

## DPMU (Power Management Unit)

**Header**: `ci130x_dpmu.h`

### Key Enums

#### `Dpmu_Lowpower_Mode_t`
| Value | Name | Description |
|-------|------|-------------|
| 0 | `DPMU_LOWPOWER_NO_MODE` | No low-power mode |
| 1 | `DPMU_LOWPOWER_SLEEP_MODE` | Sleep mode |
| 2 | `DPMU_LOWPOWER_DEEP_SLEEP_MODE` | Deep sleep mode |
| 3 | `DPMU_LOWPOWER_BOTH_MODE` | Both sleep and deep sleep |

#### `Dpmu_Io_Pull_t`
| Value | Name |
|-------|------|
| 0 | `DPMU_IO_PULL_DISABLE` |
| 1 | `DPMU_IO_PULL_UP` |
| 2 | `DPMU_IO_PULL_DOWN` |

#### `Dpmu_Src_Source_Sel_t`
| Value | Name |
|-------|------|
| 0 | `DPMU_SRC_USE_SYSTEM_DEFAULT` |
| 1 | `DPMU_SRC_USE_INNER_RC` |
| 3 | `DPMU_SRC_USE_OUTSIDE_OSC` |

### Functions

| Signature | Description |
|-----------|-------------|
| `void dpmu_unlock_cfg_config(void)` | Unlock DPMU config registers |
| `void dpmu_lock_cfg_config(void)` | Lock DPMU config registers |
| `void dpmu_set_io_reuse(PinPad_Name pin, IOResue_FUNCTION io_function)` | Set pin multiplex function |
| `void dpmu_set_adio_reuse(PinPad_Name pin, ADIOResue_MODE adio_mode)` | Set pin digital/analog mode |
| `void dpmu_set_io_open_drain(PinPad_Name pin, FunctionalState cmd)` | Enable/disable open-drain on pin |
| `void dpmu_set_io_pull(PinPad_Name pin, Dpmu_Io_Pull_t pull)` | Set pin pull-up/down |
| `void dpmu_set_io_direction(PinPad_Name pin, Dpmu_Io_Direction_t dir)` | Set pin direction (input/output) |
| `void dpmu_set_io_slew_rate(PinPad_Name pin, Dpmu_Io_Slew_Rate_t slew_rate)` | Set pin slew rate |
| `void dpmu_set_io_schmitt_trigger(PinPad_Name pin, Dpmu_Io_Schmitt_Trigger_t schmitt_trigger)` | Set pin trigger mode |
| `void dpmu_set_io_driver_strength(PinPad_Name pin, Dpmu_Io_Driver_Strength_t driver_strength)` | Set pin drive strength (0-3) |
| `void dpmu_osc_pad_for_gpio(FunctionalState en)` | Enable GPIO function on OSC pads (PA0/PA1) |
| `void dpmu_set_low_power_mode(Dpmu_Lowpower_Mode_t mode)` | Set low-power mode |
| `void dpmu_set_wakeup_int(int32_t wake_int_num, FunctionalState flag)` | Enable/disable wakeup interrupt |
| `void dpmu_pll_12d_config(uint32_t clk)` | Configure PLL 12.288 MHz clock |
| `void dpmu_iwdg_reset_none_config(void)` | Configure IWDG reset: no reset |
| `void dpmu_iwdg_reset_system_config(void)` | Configure IWDG reset: system reset |
| `void dpmu_iwdg_reset_bus_config(void)` | Configure IWDG reset: bus reset |
| `void dpmu_twdg_reset_none_config(void)` | Configure TWDG reset: no reset |
| `void dpmu_twdg_reset_sysytem_config(void)` | Configure TWDG reset: system reset |
| `void dpmu_twdg_reset_bus_config(void)` | Configure TWDG reset: bus reset |
| `void dpmu_software_reset_none_config(void)` | Configure software reset: no reset |
| `void dpmu_software_reset_system_config(void)` | Configure software reset: system reset |
| `void dpmu_software_reset_bus_config(void)` | Configure software reset: bus reset |
| `void dpmu_set_src_source(Dpmu_Src_Source_Sel_t sel)` | Select SRC clock source |
| `void dpmu_pmu_trim_en(bool en)` | Enable/disable PMU trim |
| `void dpmu_set_pmu_trim_value(uint8_t val)` | Set PMU trim value |
| `uint8_t dpmu_get_pmu_trim_value(void)` | Get PMU trim value |
| `void dpmu_vdt_lv_set(Dpmu_Vdt_Lv_t lv)` | Set low-voltage detection level (2.4V-3.1V) |
| `void dpmu_vdt_en(bool en)` | Enable/disable voltage detection |
| `void dpmu_ldo2_en(bool en)` | Enable/disable LDO2 |
| `void dpmu_ldo3_en(bool en)` | Enable/disable LDO3 |
| `void dpmu_ldo1_lv_set(uint8_t lv)` | Set LDO1 voltage level |
| `void dpmu_ldo2_lv_set(uint8_t lv)` | Set LDO2 voltage level |
| `void dpmu_ldo3_lv_set(uint8_t lv)` | Set LDO3 voltage level |
| `void dpmu_config_update_en(Dpmu_Update_En_Num_t num)` | Trigger PMU config update |
| `void dpmu_rc_freq_sel(Dpmu_Rc_Freq_Sel_t sel)` | Select RC frequency (12.288M/2M/4M/8M/16M/24M/32M/64M) |
| `uint32_t dpmu_get_pll_frequency(void)` | Get actual PLL frequency |
| `uint32_t dpmu_get_wakeup_state(void)` | Get wakeup state |
| `void dpmu_wakeup_reset_cfg(Dpmu_Wakeup_Reset_Cfg_t model, FunctionalState flag)` | Configure wakeup reset |
| `void dpmu_use_rc(void)` | Switch to internal RC clock |

---

## GPIO

**Header**: `ci130x_gpio.h`

### Enums

#### `gpio_base_t` - GPIO controller
| Value | Name |
|-------|------|
| `HAL_PA_BASE` | `PA` |
| `HAL_PB_BASE` | `PB` |
| `HAL_PC_BASE` | `PC` |
| `HAL_PD_BASE` | `PD` |

#### `gpio_pin_t` - Pin selection (bitmask, can OR multiple)
| Value | Name |
|-------|------|
| `0x1 << 0` | `pin_0` |
| `0x1 << 1` | `pin_1` |
| `0x1 << 2` | `pin_2` |
| `0x1 << 3` | `pin_3` |
| `0x1 << 4` | `pin_4` |
| `0x1 << 5` | `pin_5` |
| `0x1 << 6` | `pin_6` |
| `0x1 << 7` | `pin_7` |
| `0xFF` | `pin_all` |

#### `gpio_trigger_t` - Interrupt trigger mode
| Value | Name |
|-------|------|
| 1 | `high_level_trigger` |
| 2 | `low_level_trigger` |
| 3 | `up_edges_trigger` |
| 4 | `down_edges_trigger` |
| 5 | `both_edges_trigger` |

### Typedefs

| Type | Description |
|------|-------------|
| `gpio_info_t` | Struct: `{ gpio_base_t base; gpio_pin_t pin; }` |
| `gpio_irq_callback_t` | `void (*)(void)` - GPIO IRQ callback function |
| `gpio_irq_callback_list_t` | Linked list node for callback registration |

### Functions (multi-pin: operate one or more pins)

| Signature | Description |
|-----------|-------------|
| `void gpio_set_output_mode(gpio_base_t gpio, gpio_pin_t pins)` | Set pin(s) to output mode |
| `void gpio_set_input_mode(gpio_base_t gpio, gpio_pin_t pins)` | Set pin(s) to input mode |
| `uint8_t gpio_get_direction_status(gpio_base_t gpio, gpio_pin_t pins)` | Get direction status of pin(s) |
| `void gpio_irq_mask(gpio_base_t gpio, gpio_pin_t pins)` | Mask interrupt for pin(s) |
| `void gpio_irq_unmask(gpio_base_t gpio, gpio_pin_t pins)` | Unmask interrupt for pin(s) |
| `void gpio_irq_trigger_config(gpio_base_t gpio, gpio_pin_t pins, gpio_trigger_t trigger)` | Configure interrupt trigger mode |
| `void gpio_set_output_high_level(gpio_base_t gpio, gpio_pin_t pins)` | Set pin(s) output high |
| `void gpio_set_output_low_level(gpio_base_t gpio, gpio_pin_t pins)` | Set pin(s) output low |
| `uint8_t gpio_get_input_level(gpio_base_t gpio, gpio_pin_t pins)` | Get input level of pin(s) |

### Functions (single-pin: operate one pin at a time)

| Signature | Description |
|-----------|-------------|
| `uint8_t gpio_get_direction_status_single(gpio_base_t gpio, gpio_pin_t pins)` | Get direction of a single pin |
| `uint8_t gpio_get_irq_raw_status_single(gpio_base_t gpio, gpio_pin_t pins)` | Get raw interrupt status of a single pin |
| `uint8_t gpio_get_irq_mask_status_single(gpio_base_t gpio, gpio_pin_t pins)` | Get masked interrupt status of a single pin |
| `void gpio_clear_irq_single(gpio_base_t gpio, gpio_pin_t pins)` | Clear interrupt flag for a single pin |
| `void gpio_set_output_level_single(gpio_base_t gpio, gpio_pin_t pins, uint8_t level)` | Set output level (0/1) for a single pin |
| `uint8_t gpio_get_input_level_single(gpio_base_t gpio, gpio_pin_t pins)` | Get input level of a single pin |

### Interrupt registration

| Signature | Description |
|-----------|-------------|
| `void registe_gpio_callback(gpio_base_t base, gpio_irq_callback_list_t *gpio_irq_callback_node)` | Register GPIO interrupt callback |
| `void PA_IRQHandler(void)` | PA interrupt handler |
| `void PB_IRQHandler(void)` | PB interrupt handler |
| `void AON_PC_IRQHandler(void)` | AON PC interrupt handler |

---

## UART

**Header**: `ci130x_uart.h`

### Key Defines

| Define | Value | Description |
|--------|-------|-------------|
| `UART0_DMA_ADDR` | `0x61000000` | UART0 DMA address |
| `UART1_DMA_ADDR` | `0x62000000` | UART1 DMA address |
| `UART2_DMA_ADDR` | `0x63000000` | UART2 DMA address |

### Enums

#### `UART_BaudRate`
| Name | Value |
|------|-------|
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

#### `UART_WordLength`
| Name | Value |
|------|-------|
| `UART_WordLength_5b` | 0 |
| `UART_WordLength_6b` | 1 |
| `UART_WordLength_7b` | 2 |
| `UART_WordLength_8b` | 3 |

#### `UART_StopBits`
| Name | Value |
|------|-------|
| `UART_StopBits_1` | 0 |
| `UART_StopBits_1_5` | 1 |
| `UART_StopBits_2` | 2 |

#### `UART_Parity`
| Name | Value |
|------|-------|
| `UART_Parity_No` | 0x0 |
| `UART_Parity_Odd` | 0x01 |
| `UART_Parity_Even` | 0x03 |

#### `UART_IntMask`
| Name | Value | Description |
|------|-------|-------------|
| `UART_RXInt` | 4 | Receive interrupt |
| `UART_TXInt` | 5 | Transmit interrupt |
| `UART_RXTimeoutInt` | 6 | Receive timeout interrupt |
| `UART_AllInt` | 12 | All interrupts |

#### `UART_TXRXDMA`
| Name | Value |
|------|-------|
| `UART_RXDMA` | 0 |
| `UART_TXDMA` | 1 |

### Functions

| Signature | Description |
|-----------|-------------|
| `void UartPollingSenddata(UART_TypeDef* UARTx, char ch)` | Send a single byte via polling |
| `char UartPollingReceiveData(UART_TypeDef* UARTx)` | Receive a single byte via polling |
| `void UARTPollingConfig(UART_TypeDef* UARTx, UART_BaudRate uartbaudrate)` | Configure UART in polling mode |
| `void UART_IntMaskConfig(UART_TypeDef* UARTx, UART_IntMask intmask, FunctionalState cmd)` | Mask/unmask UART interrupt |
| `void UARTInterruptConfig(UART_TypeDef* UARTx, UART_BaudRate bd)` | Configure UART in interrupt mode |
| `void UARTDMAConfig(UART_TypeDef* UARTx, UART_BaudRate uartbaudrate)` | Configure UART in DMA mode |
| `int UART_MaskIntState(UART_TypeDef* UARTx, UART_IntMask intmask)` | Get masked interrupt state |
| `void UART_IntClear(UART_TypeDef* UARTx, UART_IntMask intmask)` | Clear interrupt flag |
| `unsigned char UART_RXDATA(UART_TypeDef* UARTx)` | Read received data byte |
| `int UART_ERRORSTATE(UART_TypeDef* UARTx, UART_ERRORFLAG uarterrorflag)` | Get error state |
| `void UART_TXDATAConfig(UART_TypeDef* UARTx, unsigned int val)` | Write transmit data |
| `int UART_FLAGSTAT(UART_TypeDef* UARTx, UART_FLAGStatus uartflag)` | Get flag status |
| `int UART_BAUDRATEConfig(UART_TypeDef* UARTx, UART_BaudRate uartbaudrate)` | Set baud rate |
| `void UART_FIFOClear(UART_TypeDef* UARTx)` | Clear TX/RX FIFO |
| `int UART_LCRConfig(UART_TypeDef* UARTx, UART_WordLength wordlength, UART_StopBits uartstopbits, UART_Parity uartparity)` | Configure line control (data bits, stop bits, parity) |
| `int UART_TXFIFOByteWordConfig(UART_TypeDef* UARTx, UART_ByteWord uarttxfifobit)` | Set TX FIFO byte/word mode |
| `void UART_EN(UART_TypeDef* UARTx, FunctionalState cmd)` | Enable/disable UART |
| `void UART_CRConfig(UART_TypeDef* UARTx, UART_CRBitCtrl crbitctrl, FunctionalState cmd)` | Configure control register bits |
| `void UART_RXFIFOConfig(UART_TypeDef* UARTx, UART_FIFOLevel fifoleve)` | Set RX FIFO trigger level |
| `void UART_TXFIFOConfig(UART_TypeDef* UARTx, UART_FIFOLevel fifoleve)` | Set TX FIFO trigger level |
| `int UART_RawIntState(UART_TypeDef* UARTx, UART_IntMask intmask)` | Get raw interrupt state |
| `void UART_TXRXDMAConfig(UART_TypeDef* UARTx, UART_TXRXDMA uartdma)` | Enable/disable TX/RX DMA |
| `void UART_TimeoutConfig(UART_TypeDef* UARTx, unsigned short time)` | Configure receive timeout |
| `void UartPollingSenddone(UART_TypeDef* UARTx)` | Wait for polling send to complete |
| `void UART_DMAByteWordConfig(UART_TypeDef* UARTx, FunctionalState cmd)` | Configure DMA byte/word mode |
| `void UartSetCLKBaseBaudrate(UART_TypeDef *UARTx, UART_BaudRate uartbaudrate)` | Set clock base baud rate |

---

## I2C

**Header**: `ci130x_iic.h`

### Enums

#### `iic_base_t`
| Name | Value |
|------|-------|
| `IIC0` | `HAL_IIC0_BASE` |
| `IIC_NULL` | 0 |

#### `IIC_TimeOut`
| Name | Value |
|------|-------|
| `LONG_TIME_OUT` | 0x5FFFFF |
| `SHORT_TIME_OUT` | 0xFFFF |

### Typedefs

| Type | Description |
|------|-------------|
| `master_send_cb_t` | `bool (*)(char* data, IIC_SendStateType state, IIC_AckType previous_ack)` |
| `master_recv_cb_t` | `bool (*)(char data, bool stop)` |
| `slave_send_cb_t` | `bool (*)(char* data, IIC_SendStateType state, IIC_AckType previous_ack)` |
| `slave_recv_cb_t` | `bool (*)(char data, bool stop)` |
| `multi_transmission_msg` | Struct: `{ char *buf; int size; iic_multi_transmission_type flag; union { int read_size; int write_size; }; }` |

### Functions (Common API)

| Signature | Description |
|-----------|-------------|
| `void iic_polling_init(iic_base_t base, uint32_t speed, uint32_t slaveaddr, IIC_TimeOut timeout)` | Initialize I2C in polling mode |
| `int32_t iic_master_polling_send(iic_base_t base, uint16_t addr, const char *buf, int32_t count, uint8_t *last_ack_flag)` | Master send data via polling |
| `int32_t iic_master_polling_recv(iic_base_t base, uint16_t addr, char *buf, int32_t count)` | Master receive data via polling |
| `int32_t iic_master_multi_transmission(iic_base_t base, uint16_t addr, multi_transmission_msg *msg, int msg_count)` | Master multi-message transmission (write-then-read) |

### Functions (Interrupt API)

| Signature | Description |
|-----------|-------------|
| `int32_t iic_slave_polling_send(iic_base_t base, const char *buf, int32_t count, uint8_t *last_ack_flag)` | Slave send via polling |
| `int32_t iic_slave_polling_recv(iic_base_t base, char *buf, int32_t count)` | Slave receive via polling |
| `void iic_interrupt_init(iic_base_t base, uint32_t speed, uint32_t slaveaddr, IIC_TimeOut timeout)` | Initialize I2C in interrupt mode |
| `int32_t iic_master_interrupt_send(iic_base_t base, uint16_t addr, master_send_cb_t master_send_cb)` | Master send via interrupt |
| `int32_t iic_master_interrupt_recv(iic_base_t base, uint16_t addr, master_recv_cb_t master_recv_cb)` | Master receive via interrupt |
| `int32_t iic_slave_interrupt_send(iic_base_t base, slave_send_cb_t slave_send_cb)` | Slave send via interrupt |
| `int32_t iic_slave_interrupt_recv(iic_base_t base, slave_recv_cb_t slave_recv_cb)` | Slave receive via interrupt |
| `void IIC_IRQHandler(iic_base_t base)` | I2C interrupt handler |

### Functions (Legacy compatible API)

| Signature | Description |
|-----------|-------------|
| `int32_t i2c_master_only_send(char slave_ic_address, const char *buf, int32_t count)` | Master send only (legacy) |
| `int32_t i2c_master_send_recv(char slave_ic_address, char *buf, int32_t send_len, int32_t rev_len)` | Master send then receive (legacy) |
| `int32_t i2c_master_only_recv(char slave_ic_address, char *buf, int32_t rev_len)` | Master receive only (legacy) |

---

## IIS (I2S Audio)

**Header**: `ci130x_iis.h`

### Enums

#### `iis_base_t`
| Name | Value |
|------|-------|
| `IIS0` | `HAL_IIS0_BASE` |
| `IIS1` | `HAL_IIS1_BASE` |
| `IIS2` | `HAL_IIS2_BASE` |

#### `iis_data_width_t`
| Name | Value |
|------|-------|
| `IIS_DW_16BIT` | 0 |
| `IIS_DW_24BIT` | 1 |
| `IIS_DW_32BIT` | 2 |
| `IIS_DW_20BIT` | 3 |

#### `iis_data_format_t`
| Name | Value |
|------|-------|
| `IIS_DF_IIS` | 0 (standard I2S) |
| `IIS_DF_MSB` | 1 (left-justified) |
| `IIS_DF_LSB` | 2 (right-justified) |

#### `iis_sound_channel_t`
| Name | Value |
|------|-------|
| `IIS_SC_STEREO` | 0 (stereo) |
| `IIS_SC_MONO` | 1 (mono) |

### Typedefs

| Type | Description |
|------|-------------|
| `iis_tx_config_t` | TX init struct: sck_lrck, cha, txfifo_trig, txch_dw, txch_copy, tx_df, tx_sc, tx_merge, tx_swap |
| `iis_rx_config_t` | RX init struct: sck_lrck, cha, rxfifo_trig, rxch_dw, rx_df, rx_sc, rx_merge, rx_swap |
| `IIS_DMA_TXInit_Typedef` | IISDMA TX init struct |
| `IIS_DMA_RXInit_Typedef` | IISDMA RX init struct |

### Functions

| Signature | Description |
|-----------|-------------|
| `void iis_init(void)` | Initialize IIS module |
| `void iis_tx_enable(uint32_t iis_base, iis_tx_channal_t cha, FunctionalState cmd)` | Enable/disable TX channel |
| `void iis_tx_l_mute(uint32_t iis_base, iis_tx_channal_t cha, FunctionalState cmd)` | Mute/unmute TX left channel |
| `void iis_tx_r_mute(uint32_t iis_base, iis_tx_channal_t cha, FunctionalState cmd)` | Mute/unmute TX right channel |
| `void iis_tx_chk(uint32_t iis_base, iis_tx_channal_t cha, FunctionalState cmd)` | Enable/disable TX check |
| `void iis_tx_config(uint32_t iis_base, iis_tx_config_p tx_cfg)` | Configure TX channel |
| `void iis_tx_same(uint32_t iis_base, FunctionalState cmd)` | Enable/disable TX same data on both channels |
| `void iis_rx_enable(uint32_t iis_base, iis_rx_channal_t cha, FunctionalState cmd)` | Enable/disable RX channel |
| `void iis_rx_mute(uint32_t iis_base, iis_rx_channal_t cha, FunctionalState cmd)` | Mute/unmute RX channel |
| `void iis_rx_chk(uint32_t iis_base, iis_rx_channal_t cha, FunctionalState cmd)` | Enable/disable RX check |
| `void iis_rx_dma_chk(uint32_t iis_base, iis_rx_channal_t cha, FunctionalState cmd)` | Enable/disable RX DMA check |
| `void iis_rx_config(uint32_t iis_base, iis_rx_config_p rx_cfg)` | Configure RX channel |
| `void iis_rx_cha_merge(uint32_t iis_base, uint8_t enable_rx0, uint8_t enable_rx1, uint8_t enable_rx2)` | Merge RX channels |
| `void iis_int_handler(uint32_t iis_base)` | IIS interrupt handler |
| `void IISx_TXDMA_Init(IIS_DMA_TXInit_Typedef* IISDMA_Str)` | Initialize IIS TX DMA |
| `void IISx_RXDMA_Init(IIS_DMA_RXInit_Typedef* IISDMA_Str)` | Initialize IIS RX DMA |

---

## IISDMA

**Header**: `ci130x_iisdma.h`

### Enums

#### `IISDMA_TXRX_ENx`
| Name | Value |
|------|-------|
| `IISxDMA_TX_EN` | 1 |
| `IISxDMA_RX_EN` | 0 |

#### `IISDMAChax`
| Name | Value |
|------|-------|
| `IISDMACha0` | 0 |
| `IISDMACha1` | 1 |
| `IISDMACha2` | 2 |

### Functions

| Signature | Description |
|-----------|-------------|
| `void IISDMA_DMICModeConfig(IISDMA_TypeDef* IISDMAx, FunctionalState cmd)` | Configure DMIC mode |
| `void IISDMA_ChannelENConfig(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, IISDMA_TXRX_ENx iisdma_txrx_sel, FunctionalState cmd)` | Enable/disable IISDMA channel |
| `void IISDMA_ADDRRollBackINT(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, IISDMA_TXRX_ENx iisdma_txrx_sel, FunctionalState cmd)` | Enable/disable address rollback interrupt |
| `void IISDMA_ChannelIntENConfig(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, IISDMA_TXRX_ENx iisdma_txrx_sel, FunctionalState cmd)` | Enable/disable channel interrupt |
| `void IISDMA_EN(IISDMA_TypeDef* IISDMAx, FunctionalState cmd)` | Enable/disable IISDMA |
| `void IISxDMA_RADDR(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, unsigned int rxaddr)` | Set RX address |
| `void IISxDMA_RNUM(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, IISDMA_RXxInterrupt iisrxtointerrupt, IISDMA_RXTXxRollbackADDR rollbacktimes, IISDMA_TXRXSingleSIZEx rxsinglesize)` | Set RX transfer parameters |
| `void IISxDMA_TADDR0(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, unsigned int txaddr0)` | Set TX address 0 |
| `void IISxDMA_TNUM0(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, IISDMA_RXTXxRollbackADDR rollbackaddr, IISDMA_TXRXSingleSIZEx txsinglesize)` | Set TX0 transfer parameters |
| `void IISxDMA_TADDR1(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, unsigned int txaddr1)` | Set TX address 1 |
| `void IISxDMA_TNUM1(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, IISDMA_RXTXxRollbackADDR rollbackaddr, IISDMA_TXRXSingleSIZEx txsinglesize)` | Set TX1 transfer parameters |
| `void IISDMA_PriorityConfig(IISDMA_TypeDef* IISDMAx, IISDMA_Priorityx iisdma_priority)` | Set IISDMA channel priority |
| `void IISDMA_INT_All_Clear(IISDMA_TypeDef* IISDMAx)` | Clear all IISDMA interrupts |
| `int IISDMA_ADDRRollBackSTATE(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx, IISDMA_TXRX_ENx iisdma_txrx_sel)` | Get address rollback state |
| `int CHECK_IISDMA_DATABUSBUSY(IISDMA_TypeDef* IISDMAx)` | Check if IISDMA data bus is busy |
| `void iisdma_config(iisdma_config_p config)` | Full IISDMA configuration from struct |
| `uint32_t Get_IISxDMA_RADDR(IISDMA_TypeDef* IISDMAx, IISDMAChax iisx)` | Get current RX address |

---

## ADC

**Header**: `ci130x_adc.h`

### Enums

#### `adc_channelx_t`
| Name | Value |
|------|-------|
| `ADC_CHANNEL_0` | 0 |
| `ADC_CHANNEL_1` | 1 |
| `ADC_CHANNEL_2` | 2 |
| `ADC_CHANNEL_3` | 3 |
| `ADC_CHANNEL_4` | 4 |
| `ADC_CHANNEL_5` | 5 |
| `ADC_CHANNEL_MAX` | 6 |

#### `adc_int_mode_t`
| Name | Value | Description |
|------|-------|-------------|
| `ADC_INT_MODE_TRANS_END` | 0 | Interrupt on each conversion complete |
| `ADC_INT_MODE_VALUE_NOT_MEET` | 1 | Interrupt on abnormal value |

### Functions

| Signature | Description |
|-----------|-------------|
| `void adc_int_clear(adc_channelx_t channel)` | Clear ADC interrupt flag |
| `void adc_soc_soft_ctrl(FunctionalState cmd)` | Software control of ADC start-of-conversion |
| `int8_t adc_get_vol_value(adc_channelx_t cha, float* vol_val)` | Get voltage value for channel |
| `void adc_poweron(void)` | Power on ADC |
| `void adc_reset(void)` | Reset ADC |
| `void adc_signal_mode(adc_channelx_t cha)` | Set single-channel mode |
| `void adc_series_mode(adc_channelx_t cha)` | Set series mode |
| `void adc_cycle_mode(adc_channelx_t cha, uint16_t cycle)` | Set cycle mode with period |
| `void adc_caculate_mode(void)` | Set calculate mode |
| `uint32_t adc_get_result(adc_channelx_t channel)` | Get raw ADC conversion result |
| `void adc_calibrate(FunctionalState cmd)` | Enable/disable ADC calibration |
| `void adc_wait_int(adc_channelx_t cha)` | Wait for ADC interrupt (extern) |
| `void ADC_irqhandle(void)` | ADC interrupt handler |

---

## PWM

**Header**: `ci130x_pwm.h`

### Enums

#### `pwm_base_t`
| Name | Value |
|------|-------|
| `PWM0` | `HAL_PWM0_BASE` |
| `PWM1` | `HAL_PWM1_BASE` |
| `PWM2` | `HAL_PWM2_BASE` |
| `PWM3` | `HAL_PWM3_BASE` |
| `PWM4` | `HAL_PWM4_BASE` |
| `PWM5` | `HAL_PWM5_BASE` |

### Typedefs

#### `pwm_init_t`
| Field | Type | Description |
|-------|------|-------------|
| `clk_sel` | `unsigned int` | Clock source: 0=PCLK, 1=SRC clock |
| `freq` | `unsigned int` | PWM frequency in Hz |
| `duty` | `unsigned int` | PWM duty cycle |
| `duty_max` | `unsigned int` | Maximum duty value |

### Functions

| Signature | Description |
|-----------|-------------|
| `void pwm_init(pwm_base_t base, pwm_init_t init)` | Initialize PWM with config struct |
| `void pwm_start(pwm_base_t base)` | Start PWM output |
| `void pwm_stop(pwm_base_t base)` | Stop PWM output |
| `void pwm_set_duty(pwm_base_t base, unsigned int duty, unsigned int duty_max)` | Set PWM duty cycle |
| `void pwm_set_restart_md(pwm_base_t base, uint8_t cmd)` | Set PWM restart mode |

---

## Timer

**Header**: `ci130x_timer.h`

### Enums

#### `timer_base_t`
| Name | Value | Description |
|------|-------|-------------|
| `TIMER0` | `HAL_TIMER0_BASE` | Timer0 |
| `TIMER1` | `HAL_TIMER1_BASE` | Timer1 |
| `TIMER2` | `HAL_TIMER2_BASE` | Timer2 |
| `TIMER3` | `HAL_TIMER3_BASE` | Timer3 |
| `AON_TIMER0` | `HAL_PWM4_BASE` | Always-on Timer0 |
| `AON_TIMER1` | `HAL_PWM5_BASE` | Always-on Timer1 |

#### `timer_count_mode_t`
| Name | Value | Description |
|------|-------|-------------|
| `timer_count_mode_single` | 0 | Single cycle (one-shot) |
| `timer_count_mode_auto` | 1 | Auto reload |
| `timer_count_mode_free` | 2 | Free running |
| `timer_count_mode_event` | 3 | Event count |

#### `timer_clock_div_t`
| Name | Value |
|------|-------|
| `timer_clk_div_0` | 0 (no division) |
| `timer_clk_div_2` | 1 |
| `timer_clk_div_4` | 2 |
| `timer_clk_div_16` | 3 |

### Typedefs

#### `timer_init_t`
| Field | Type |
|-------|------|
| `mode` | `timer_count_mode_t` |
| `div` | `timer_clock_div_t` |
| `width` | `timer_iqr_width_t` |
| `count` | `unsigned int` |

### Functions

| Signature | Description |
|-----------|-------------|
| `void timer_init(timer_base_t base, timer_init_t init)` | Initialize timer with config struct |
| `void timer_set_mode(timer_base_t base, timer_count_mode_t mode)` | Set timer count mode |
| `void timer_start(timer_base_t base)` | Start timer |
| `void timer_stop(timer_base_t base)` | Stop timer |
| `void timer_event_start(timer_base_t base)` | Start event count mode |
| `void timer_set_count(timer_base_t base, unsigned int count)` | Set timer count value |
| `void timer_get_count(timer_base_t base, unsigned int* count)` | Get current timer count |
| `void timer_cascade_set(timer_base_t base, unsigned int count)` | Set timer cascade |
| `void timer_clear_irq(timer_base_t base)` | Clear timer interrupt flag |

---

## DMA

**Header**: `ci130x_dma.h`

### Key Defines

| Define | Value | Description |
|--------|-------|-------------|
| `GDMA_SDRAM_ADDR` | `0x70000000UL` | SDRAM base |
| `GDMA_CSRAM_ADDR` | `0x1FFF8000UL` | CSRAM base (32KB) |
| `GDMA_SRAM0_ADDR` | `0x1FFE8000UL` | SRAM0 base (64KB) |
| `GDMA_SRAM1_ADDR` | `0x20000000UL` | SRAM1 base (64KB) |

### Enums

#### `DMACChannelx`
| Name | Value |
|------|-------|
| `DMACChannel0` | 0 |
| `DMACChannel1` | 1 |
| ... | ... |
| `DMACChannel7` | 7 |
| `DMACChannelALL` | 9 |

#### `DMAC_FLOWCTRL`
| Name | Value | Description |
|------|-------|-------------|
| `M2M_DMA` | 0 | Memory to memory |
| `M2P_DMA` | 1 | Memory to peripheral |
| `P2M_DMA` | 2 | Peripheral to memory |
| `SP2DP_DMA` | 3 | Src-periph to dst-periph (DMA ctrl) |

#### `DMAC_Peripherals`
| Name | Value |
|------|-------|
| `DMAC_Peripherals_SPI0` | 0 |
| `DMAC_Peripherals_UART0_RX` | 4 |
| `DMAC_Peripherals_UART0_TX` | 5 |
| `DMAC_Peripherals_UART1_RX` | 6 |
| `DMAC_Peripherals_UART1_TX` | 7 |
| `DMAC_Peripherals_UART2_RX` | 8 |
| `DMAC_Peripherals_UART2_TX` | 9 |

### Typedefs

| Type | Description |
|------|-------------|
| `dma_callback_func_ptr_t` | `void (*)(void)` - DMA completion callback |
| `dma_config_t` | Struct: flowctrl, burstsize, transferwidth, srcaddr, destaddr, transfersize |
| `LLI_Control` | Linked list item control struct |

### Functions

| Signature | Description |
|-----------|-------------|
| `void clear_dma_translate_flag(DMACChannelx dmachannel)` | Clear DMA transfer flag |
| `int wait_dma_translate_flag(DMACChannelx dmachannel, uint32_t timeout)` | Wait for DMA transfer complete (with timeout) |
| `void dma_with_os_int(void)` | Enable DMA with OS interrupt |
| `void dma_without_os_int(void)` | Disable DMA OS interrupt |
| `void dma_int_event_group_init(void)` | Initialize DMA interrupt event group |
| `void dma_irq_handler(void)` | DMA interrupt handler |
| `int DMAC_IntStatus(DMACChannelx dmachannel)` | Get DMA interrupt status |
| `int DMAC_IntTCStatus(DMACChannelx dmachannel)` | Get terminal count interrupt status |
| `void DMAC_IntTCClear(DMACChannelx dmachannel)` | Clear terminal count interrupt |
| `int DMAC_IntErrorStatus(DMACChannelx dmachannel)` | Get error interrupt status |
| `void DMAC_IntErrorClear(DMACChannelx dmachannel)` | Clear error interrupt |
| `void DMAC_Config(DMAC_AHBMasterx dmamaster, ENDIANMODE endianmode)` | Configure DMA controller |
| `void DMAC_EN(FunctionalState cmd)` | Enable/disable DMA controller |
| `void DMAC_ChannelSoureAddr(DMACChannelx dmachannel, unsigned int addr)` | Set channel source address |
| `void DMAC_ChannelDestAddr(DMACChannelx dmachannel, unsigned int addr)` | Set channel destination address |
| `void DMAC_ChannelTCInt(DMACChannelx dmachannel, FunctionalState cmd)` | Enable/disable terminal count interrupt |
| `void DMAC_ChannelTransferSize(DMACChannelx dmachannel, unsigned short size)` | Set channel transfer size |
| `unsigned int DMAC_ChannelCurrentTransferSize(DMACChannelx dmachannel)` | Get current transfer count |
| `void DMAC_ChannelDisable(DMACChannelx dmachannel)` | Disable DMA channel |
| `void DMAC_ChannelEnable(DMACChannelx dmachannel)` | Enable DMA channel |
| `void DMAC_ChannelConfig(DMACChannelx dmachannel, char destperiph, char srcperiph, DMAC_FLOWCTRL flowctrl)` | Configure channel peripheral mapping |
| `void DMAC_M2MConfig(DMACChannelx dmachannel, unsigned int srcaddr, unsigned int destaddr, unsigned int bytesize, DMAC_AHBMasterx master)` | Configure memory-to-memory transfer |
| `void DMAC_M2P_P2MConfig(DMACChannelx dmachannel, DMAC_Peripherals periph, DMAC_FLOWCTRL flowctrl, unsigned int srcaddr, unsigned int destaddr, unsigned int bytesize)` | Configure memory-to-peripheral or peripheral-to-memory transfer |
| `void DMAC_M2P_P2M_advance_config(DMACChannelx dmachannel, DMAC_Peripherals periph, DMAC_FLOWCTRL flowctrl, unsigned int srcaddr, unsigned int destaddr, unsigned int bytesize, TRANSFERWIDTHx datawidth, BURSTSIZEx burstsize, DMAC_AHBMasterx master)` | Advanced M2P/P2M configuration with width and burst size |
| `void DMAC_P2PConfig(DMACChannelx dmachannel, DMAC_Peripherals srcperiph, DMAC_Peripherals destperiph, unsigned int bytesize)` | Configure peripheral-to-peripheral transfer |
| `void spic_dma_config(DMACChannelx channel, dma_config_t* config)` | Configure SPIFlash DMA |

---

## SPIFlash

**Header**: `ci130x_spiflash.h`

### Enums

#### `spic_base_t`
| Name | Value |
|------|-------|
| `QSPI0` | `HAL_DTRFLASH_BASE` |

#### `spic_cmd_code_t` (selected commands)
| Name | Value | Description |
|------|-------|-------------|
| `SPIC_CMD_CODE_WRITE_ENABLE` | 0x06 | Write Enable |
| `SPIC_CMD_CODE_READJEDECID` | 0x9F | Read JEDEC ID |
| `SPIC_CMD_CODE_READSTATUSREG1` | 0x05 | Read Status Register 1 |
| `SPIC_CMD_CODE_SECTORERASE4K` | 0x20 | 4KB Sector Erase |
| `SPIC_CMD_CODE_BLOCKERASE64K` | 0xd8 | 64KB Block Erase |
| `SPIC_CMD_CODE_PAGEPROGRAM` | 0x02 | Page Program |
| `SPIC_CMD_CODE_READDATA` | 0x03 | Read Data |
| `SPIC_CMD_CODE_FASTREAD` | 0x0b | Fast Read |
| `SPIC_CMD_CODE_CHIPERASE` | 0xc7 | Chip Erase |

### Functions

| Signature | Description |
|-----------|-------------|
| `int32_t flash_init(spic_base_t spic)` | Initialize SPIFlash |
| `int32_t spic_read_unique_id(spic_base_t spic, uint8_t* unique)` | Read flash unique ID |
| `int32_t spic_read_jedec_id(spic_base_t spic, uint8_t* jedec)` | Read JEDEC ID |
| `int32_t spic_erase_security_reg(spic_base_t spic, spic_security_reg_t reg)` | Erase security register |
| `int32_t spic_write_security_reg(spic_base_t spic, spic_security_reg_t reg, uint32_t buf, uint32_t addr, uint32_t size)` | Write security register |
| `int32_t spic_read_security_reg(spic_base_t spic, spic_security_reg_t reg, uint32_t buf, uint32_t addr, uint32_t size)` | Read security register |
| `int32_t spic_security_reg_lock(spic_base_t spic, spic_security_reg_t reg)` | Lock security register |
| `int32_t flash_erase(spic_base_t spic, uint32_t addr, uint32_t size)` | Erase flash region (addr, size in bytes) |
| `int32_t flash_write(spic_base_t spic, uint32_t addr, uint32_t buf, uint32_t size)` | Write data to flash (addr, buf, size) |
| `int32_t flash_read(spic_base_t spic, uint32_t buf, uint32_t addr, uint32_t size)` | Read data from flash (buf, addr, size) |
| `int32_t dnn_mode_config(spic_base_t spic, uint32_t start_addr, uint32_t size)` | Configure DNN mode region |
| `int32_t flash_dnn_mode(spic_base_t spic, FunctionalState cmd)` | Enable/disable DNN mode |
| `uint32_t flash_check_mode(spic_base_t spic)` | Check current flash mode |
| `int32_t spic_quad_mode(spic_base_t spic)` | Enable quad mode |
| `int32_t spic_erase(spic_base_t spic, spic_cmd_code_t code, uint32_t addr)` | Erase with specific command |
| `int32_t spic_protect(spic_base_t spic, FunctionalState cmd)` | Enable/disable flash protection |
| `int32_t flash_is_dnn_mode(spic_base_t spic)` | Check if DNN mode is active |
| `int32_t spic_reset(spic_base_t spic)` | Reset SPIFlash |
| `int32_t spic_xipconfig(spic_base_t spic)` | Configure XIP (execute-in-place) mode |
| `int32_t flash_clk_div_init(spic_base_t spic)` | Initialize flash clock divider |

---

## DTRFlash (QSPI Controller)

**Header**: `ci130x_dtrflash.h`

### Enums

#### `flash_clk_div_t`
| Name | Value |
|------|-------|
| `FLASH_CLK_DIV_2` | 0 |
| `FLASH_CLK_DIV_4` | 1 |
| `FLASH_CLK_DIV_6` | 2 |
| `FLASH_CLK_DIV_8` | 3 |

#### `md_sel_t` - Transfer mode selection
| Name | Value | Description |
|------|-------|-------------|
| `MD_SEL_LINE_1` | 0 | Single-line mode |
| `MD_SEL_LINE_4` | 1 | Quad-line mode |
| `MD_SEL_LINE_8` | 2 | Octal-line mode |
| `MD_SEL_DTR_LINE_1` | 8 | DTR single-line |
| `MD_SEL_DTR_LINE_4` | 9 | DTR quad-line |

### Functions

| Signature | Description |
|-----------|-------------|
| `int32_t spic_cmd(uint32_t spic_base, spic_base_config_p spic_base_config)` | Send SPI command |
| `int32_t spic_read_by_cpu(uint32_t spic_base, spic_base_config_p spic_base_config, uint8_t* read_data, uint32_t read_len)` | Read via CPU |
| `int32_t spic_write_by_cpu(uint32_t spic_base, spic_base_config_p spic_base_config, uint8_t* write_data, uint32_t write_len)` | Write via CPU |
| `int32_t spic_read_by_dma(uint32_t spic_base, spic_base_config_p spic_base_config, uint8_t* read_data, uint32_t read_len)` | Read via DMA |
| `int32_t spic_write_by_dma(uint32_t spic_base, spic_base_config_p spic_base_config, uint8_t* write_data, uint32_t write_len)` | Write via DMA |
| `int32_t spic_read_xip(uint32_t spic_base, spic_base_config_p spic_base_config, uint8_t* read_data, uint32_t read_len)` | Read via XIP |
| `int32_t spic_init(uint32_t spic_base, spic_init_p init)` | Initialize QSPI controller |
| `int32_t spic_clk_phase_set(uint32_t spic_base, uint32_t tx_shift, uint32_t tx_nege_en, uint32_t rx_shift, uint32_t rx_nege_en)` | Set clock phase |
| `void spic_hardware_reset(uint32_t spic_base, uint8_t enable)` | Hardware reset |
| `int32_t spic_xip_config(uint32_t spic_base, spic_base_config_p spic_base_config)` | Configure XIP mode |
| `void spic_prefetch_en(uint32_t spic_base, bool en)` | Enable/disable prefetch |
| `void spic_change_clk(uint32_t spic_base, flash_clk_div_t clk)` | Change flash clock divider |

---

## Watchdog (IWDG)

**Header**: `ci130x_iwdg.h`

### Enums

#### `iwdg_base_t`
| Name | Value |
|------|-------|
| `IWDG` | `HAL_IWDG_BASE` |

#### `iwdg_irqen_t`
| Name | Value |
|------|-------|
| `iwdg_irqen_enable` | 1 |
| `iwdg_irqen_disable` | 0 |

#### `iwdg_resen_t`
| Name | Value |
|------|-------|
| `iwdg_resen_enable` | 1 |
| `iwdg_resen_disable` | 0 |

### Typedefs

#### `iwdg_init_t`
| Field | Type |
|-------|------|
| `count` | `unsigned int` |
| `irq` | `iwdg_irqen_t` |
| `res` | `iwdg_resen_t` |

### Functions

| Signature | Description |
|-----------|-------------|
| `void iwdg_init(iwdg_base_t base, iwdg_init_t init)` | Initialize IWDG |
| `void iwdg_open(iwdg_base_t base)` | Enable IWDG |
| `void iwdg_close(iwdg_base_t base)` | Disable IWDG |
| `void iwdg_feed(iwdg_base_t base)` | Feed watchdog (clear counter) |
| `void iwdg_irqhander(void)` | IWDG interrupt handler |

---

## Window Watchdog (TWDG)

**Header**: `ci130x_TimerWdt.h`

### Typedefs

#### `TWDG_InitTypeDef`
| Field | Type | Description |
|-------|------|-------------|
| `scale` | `unsigned int` | Divider (0 or 1 = no division) |
| `lower` | `unsigned int` | Lower window bound |
| `upper` | `unsigned int` | Upper window bound |
| `mode` | `unsigned int` | Mode: `TWDG_NORMAL_MODE` or `TWDG_PRE_WARNING` |

### Functions

| Signature | Description |
|-----------|-------------|
| `void TWDG_init(TWDG_InitTypeDef *initStruct)` | Initialize window watchdog |
| `void TWDG_disable(void)` | Disable window watchdog |
| `unsigned int TWDG_counter(void)` | Get current counter value |
| `void TWDG_service(void)` | Feed window watchdog |
| `void TWDG_IRQHandler(void)` | TWDG interrupt handler |
| `void TWDG_test(int mode, int ms_windowLowper, int ms_windowUpper)` | Test function |

---

## Codec (Inner CODEC)

**Header**: `ci130x_codec.h`

### Key Enums

#### `inner_codec_mode_t`
| Name | Value |
|------|-------|
| `INNER_CODEC_MODE_MASTER` | 3 |
| `INNER_CODEC_MODE_SLAVE` | 0 |

#### `inner_codec_input_mode_t`
| Name | Value |
|------|-------|
| `INNER_CODEC_INPUT_MODE_DIFF` | 1 (differential) |
| `INNER_CODEC_INPUT_MODE_SINGGLE_ENDED` | 2 (single-ended) |

#### `inner_codec_mic_amplify_t`
| Name | Value |
|------|-------|
| `INNER_CODEC_MIC_AMP_0dB` | 0 |
| `INNER_CODEC_MIC_AMP_6dB` | 1 |
| `INNER_CODEC_MIC_AMP_9dB` | 2 |
| `INNER_CODEC_MIC_AMP_12dB` | 3 |
| `INNER_CODEC_MIC_AMP_16dB` | 4 |
| `INNER_CODEC_MIC_AMP_20dB` | 5 |

#### `inner_codec_samplerate_t`
| Name | Value |
|------|-------|
| `INNER_CODEC_SAMPLERATE_96K` | 0 |
| `INNER_CODEC_SAMPLERATE_48K` | 1 |
| `INNER_CODEC_SAMPLERATE_44_1K` | 2 |
| `INNER_CODEC_SAMPLERATE_32K` | 3 |
| `INNER_CODEC_SAMPLERATE_24K` | 4 |
| `INNER_CODEC_SAMPLERATE_16K` | 5 |
| `INNER_CODEC_SAMPLERATE_12K` | 6 |
| `INNER_CODEC_SAMPLERATE_8K` | 7 |

### Typedefs

#### `inner_codec_adc_config_t`
| Field | Type |
|-------|------|
| `codec_adc_input_mode_l` | `inner_codec_input_mode_t` |
| `codec_adc_input_mode_r` | `inner_codec_input_mode_t` |
| `codec_adc_mic_amp_l` | `inner_codec_mic_amplify_t` |
| `codec_adc_mic_amp_r` | `inner_codec_mic_amplify_t` |
| `pga_gain_l` | `float` |
| `pga_gain_r` | `float` |
| `dig_gain_l` | `float` |
| `dig_gain_r` | `float` |

#### `inner_codec_alc_config_t`
| Field | Type |
|-------|------|
| `holdtime` | `inner_codec_alc_hold_time_t` |
| `decaytime` | `inner_codec_alc_decay_time_t` |
| `attacktime` | `inner_codec_alc_attack_time_t` |
| `max_pga_gain` | `inner_codec_alc_pga_max_gain_t` |
| `min_pga_gain` | `inner_codec_alc_pga_min_gain_t` |
| `samplerate` | `inner_codec_samplerate_t` |
| `alcmode` | `inner_codec_alc_mode_t` |
| `pga_gain` | `float` |
| `max_level` | `inner_codec_alc_level_t` |
| `min_level` | `inner_codec_alc_level_t` |

### Functions

| Signature | Description |
|-----------|-------------|
| `void inner_codec_up_ibas_adc(void)` | Power up ADC bias current |
| `void inner_codec_up_ibas_dac(void)` | Power up DAC bias current |
| `void inner_codec_reset(void)` | Reset inner codec |
| `void inner_codec_power_up(inner_codec_current_t current)` | Power up codec with specified current |
| `void inner_codec_power_off(void)` | Power off codec |
| `void inner_codec_hp_filter_config(inner_cedoc_gate_t gate, inner_codec_highpass_cut_off_t Hz)` | Configure high-pass filter |
| `void inner_codec_adc_enable(inner_codec_adc_config_t *ADC_Config)` | Enable ADC with config |
| `void inner_codec_adc_disable(inner_codec_cha_sel_t cha, inner_cedoc_gate_t EN)` | Disable ADC channel |
| `void inner_codec_dac_enable(bool is_first_enable)` | Enable DAC |
| `void inner_codec_dac_disable(void)` | Disable DAC |
| `void inner_codec_alc_disable(inner_codec_cha_sel_t cha)` | Disable ALC for channel |
| `void inner_codec_adc_mode_set(inner_codec_mode_t mode, inner_codec_frame_1_2len_t frame_Len, inner_codec_valid_word_len_t word_len, inner_codec_i2s_data_famat_t data_fram)` | Set ADC mode |
| `void inner_codec_dac_mode_set(inner_codec_mode_t mode, inner_codec_frame_1_2len_t frame_Len, inner_codec_valid_word_len_t word_len, inner_codec_i2s_data_famat_t data_fram)` | Set DAC mode |
| `void inner_codec_left_alc_pro_mode_config(inner_codec_alc_config_t* ALC_Type)` | Configure left ALC (pro mode) |
| `void inner_codec_right_alc_pro_mode_config(inner_codec_alc_config_t* ALC_Type)` | Configure right ALC (pro mode) |
| `void inner_codec_left_alc_enable(inner_cedoc_gate_t gate, inner_codec_use_alc_control_pgagain_t is_alc_ctr_pga)` | Enable left ALC |
| `void inner_codec_right_alc_enable(inner_cedoc_gate_t gate, inner_codec_use_alc_control_pgagain_t is_alc_ctr_pga)` | Enable right ALC |
| `void inner_codec_micbias_set(inner_codec_micbias_t bias)` | Set MIC bias voltage |

---

## ALC (Auxiliary ALC)

**Header**: `ci130x_alc.h`

### Functions

| Signature | Description |
|-----------|-------------|
| `void alc_interrupt_handler(ALC_TypeDef* alc)` | ALC interrupt handler |
| `void alc_aux_left_config(ALC_TypeDef* alc, alc_aux_config_t* ALC_Type)` | Configure left ALC auxiliary |
| `void alc_aux_right_config(ALC_TypeDef* alc, alc_aux_config_t* ALC_Type)` | Configure right ALC auxiliary |
| `void alc_aux_intterupt_config(ALC_TypeDef* alc, alc_aux_int_t* ALC_INT_Type, alc_aux_cha_t cha)` | Configure ALC interrupts |
| `void alc_aux_globle_enable(ALC_TypeDef* alc, FunctionalState cmd)` | Enable/disable ALC global |
| `void alc_aux_globle_config(ALC_TypeDef* alc, alc_aux_globle_config_t* ALC_Glb_Type)` | Configure ALC global settings |
| `void alc_aux_left_cha_en(ALC_TypeDef* alc, FunctionalState cmd)` | Enable/disable left ALC channel |
| `void alc_aux_right_cha_en(ALC_TypeDef* alc, FunctionalState cmd)` | Enable/disable right ALC channel |

---

## PDM

**Header**: `ci130x_pdm.h`

> PDM API mirrors the inner CODEC API with `pdm_` prefix instead of `inner_codec_`. Key types: `pdm_adc_config_t`, `pdm_alc_config_t`, `pdm_mic_amplify_t`, `pdm_samplerate_t`, etc.

---

## Mailbox

**Header**: `ci130x_mailbox.h`

### Enums

#### `mailbox_cmd_t`
| Name | Value |
|------|-------|
| `MAILBOX_POWERUP_CMD` | 0 |
| `MAILBOX_RPMSG_CMD` | 1 |
| `MAILBOX_NUCLEAR_CMD` | 2 |
| `MAILBOX_UNKNOWN_CMD` | 0xFFFFFFFF |

### Typedefs

| Type | Description |
|------|-------------|
| `mailbox_irq_cmd_cb_t` | `void (*)(uint32_t data0, uint32_t data1)` |

### Functions

| Signature | Description |
|-----------|-------------|
| `void mailbox_preinit(void)` | Pre-initialize mailbox |
| `uint32_t mailbox_init(mailbox_irq_cmd_cb_t callback_func)` | Initialize mailbox with callback |
| `void mailbox_deinit(void)` | Deinitialize mailbox |
| `uint32_t host_mail_send_msg(uint32_t data0, uint32_t data1, mailbox_cmd_t cmd)` | Send message from host core |
| `uint32_t host_mail_rev_msg(uint32_t *data0, uint32_t *data1, mailbox_cmd_t *cmd)` | Receive message on host core |
| `void mailboxboot_sync(void)` | Synchronize host and nuclear cores at boot |
| `uint32_t mailbox_send_msg(uint32_t data0, uint32_t data1, mailbox_cmd_t cmd)` | Send mailbox message |

---

## LowPower

**Header**: `ci130x_lowpower.h`

### Enums

#### `power_mode_t`
| Name | Value | Description |
|------|-------|-------------|
| `POWER_MODE_ERR` | -1 | Error |
| `POWER_MODE_NORMAL` | 0 | Normal mode |
| `POWER_MODE_DOWN_FREQUENCY` | 1 | Frequency reduction mode |
| `POWER_MODE_OSC_FREQUENCY` | 2 | Crystal oscillator mode |
| `POWER_MODE_SLEEP` | 998 | Sleep mode |
| `POWER_MODE_DEEP_SLEEP` | 999 | Deep sleep mode |

### Functions

| Signature | Description |
|-----------|-------------|
| `void register_lowpower_user_fn(void *enter_lowpower_fn, void *exit_lowpower_fn)` | Register user low-power enter/exit callbacks |
| `power_mode_t power_mode_switch(power_mode_t power_mode)` | Switch power mode |
| `power_mode_t get_curr_power_mode(void)` | Get current power mode |

---

## Cache

**Header**: `ci130x_cache.h`

### Key Defines

| Define | Value | Description |
|--------|-------|-------------|
| `ICACHE_TCM_S` | `0x1ffa8000` | ICACHE TCM start address |
| `ICACHE_TCM_E` | `0x1ffaffff` | ICACHE TCM end address |
| `ICACHE_TCM_A` | `0x1fbb0000` | ICACHE TCM actual address |

### Enums

#### `cache_base_t`
| Name | Value |
|------|-------|
| `ICACHE` | `HAL_ICACHE_BASE` |
| `SCACHE` | `HAL_SCACHE_BASE` |

### Functions

| Signature | Description |
|-----------|-------------|
| `void cache_enable_auto(cache_base_t base, unsigned int start, unsigned int end)` | Enable auto cache for address range |
| `void cache_disable_auto(cache_base_t base)` | Disable auto cache |
| `void get_hit_miss(cache_base_t base, unsigned int* hit, unsigned int* miss)` | Get cache hit/miss counters |
| `void cache_alias_mode(cache_base_t base, unsigned int start, unsigned int end, unsigned int alias)` | Configure cache alias mode |
| `void i_cache_tcm_enable(unsigned int flash_user_offset)` | Enable I-cache TCM |
| `void s_cache_tcm_enable(void)` | Enable S-cache TCM |
| `void show_cache(void)` | Print cache status |
