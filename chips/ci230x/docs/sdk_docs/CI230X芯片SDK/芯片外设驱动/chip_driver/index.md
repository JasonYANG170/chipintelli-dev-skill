<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E8%8A%AF%E7%89%87%E5%A4%96%E8%AE%BE%E9%A9%B1%E5%8A%A8/chip_driver/ -->

# CI230X系列芯片驱动

---

## 1. CI230X系列芯片驱动概述

CI230X系列芯片驱动分“语音部分”和“WIFI部分”，“语音部分”驱动请查看[语音部分外设驱动开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/driver/ci230x_driver/gpio/)；“WIFI部分”和“语音部分”通过UART通信，通信速率最大支持2Mbits/s，接下来将对“WIFI部分”驱动做详细描述。

---

## 2. 串行外设驱动（SPI)

* SPI初始化和使能API接口

```
void    hal_spi_init(uint32_t spi_x_base,spi_init_type_def* spi_init);
void    hal_spi_deinit(uint32_t spi_x_base);
void    hal_spi_en(uint32_t spi_x_base,hal_en_t en)
```

* SPI收发数据API接口

```
void     hal_spi_send_data(uint32_t spi_x_base,uint16_t data);
uint16_t hal_spi_recv_data(uint32_t spi_x_base);
```

---

## 3. 高级定时器（ADV\_Timer）

定时器是一个通过可编程预分频器驱动的16位自动装载计数器构成。

高级定时器主要特征：

* 16位向上、向下、向上/下自动装载计数器
* 6位可编程预分频器，计数器时钟频率的分频系数为0～63之间的任意数值，0为不分频
* 12位可编程死区时间的互补输出
* 六个独立通道
* 支持PWM输出比较、输入捕获以及PWM触发ADC采样功能

高级定时器初始化和配置API接口

```
void    hal_adv_tim_init(uint32_t adv_tim_x_base,adv_tim_init_t_def* adv_tim_init);      
void    hal_adv_tim_deinit(void);
```

高级定时器数据获取API接口

```
 uint16_t   hal_adv_tim_get_count(uint32_t adv_tim_x_base);
```

---

## 4. 基本定时器（Timer）

定时器是一个通过可编程预分频器驱动的24位自动装载计数器构成。

定时器主要特征：

* 8位可编程(可以实时修改)预分频器，计数器时钟频率的分频系数为0～255之间的任意数值 ，0为不分频。
* 四个完全独立的Timer
* 可以实时修改PWM占空比（但是只有CNT值为0的时候才能加载最新的LOAD值）
* 24位的向下计数器

定时器初始化API接口

```
void        hal_tim_init(uint32_t tim_x_base,tim_init_t_def* tim_init);
void        hal_tim_deinit(void);
void        hal_tim_en(uint32_t tim_x_base,hal_en_t en);
void        hal_tim_set_load_value(uint32_t tim_x_base,uint32_t value);
uint32_t    hal_tim_get_current_cnt_value(uint32_t tim_x_base);
```

定时器中断初始化API接口

```
void    hal_tim_it_cfg(uint32_t tim_x_base,tim_it_flag_t tim_it_flag,hal_en_t en);
uint8_t hal_tim_get_it_flag(uint32_t tim_x_base,tim_it_flag_t tim_it_flag);
void    hal_tim_clr_it_flag(uint32_t tim_x_base,tim_it_flag_t tim_it_flag);
```

## 5. 看门狗（WDT）

WDT初始化和配置API接口

```
void    hal_wdt_init(uint32_t wdt_base,wdt_init_t_def *wdt_init);
void    hal_wdt_deinit(void);
void    hal_wdt_en(uint32_t wdt_base,hal_en_t en);
void    hal_wdt_cnt_restart(uint32_t wdt_base);
```

## 6. 时钟控制（CLOCK）

时钟控制器初始化API接口

```
 void   hal_clock_init(clock_init_t *clock_init);
```

## 7. 实时时钟（RTC）

实时时钟初始化和配置API接口

```
void      hal_rtc_init(uint32_t rtc_base,rtc_init_t_def* rtc_init);
void      hal_rtc_deinit(``void``);
void      hal_rtc_en(uint32_t rtc_base,hal_en_t en);
uint32_t  hal_rtc_get_cnt(uint32_t rtc_base);
```

实时时钟中断API接口

```
void     hal_rtc_it_cfg(uint32_t rtc_base,rtc_it_flag_t rtc_it_flag,hal_en_t en);
uint8_t  hal_rtc_get_it_flag(uint32_t rtc_base,rtc_it_flag_t rtc_it_flag);
void     hal_rtc_clr_it_flag(uint32_t rtc_base,rtc_it_flag_t rtc_it_flag);
```

## 8. 通用异步收发器（UART）

通用异步收发器API接口

```
void       hal_uart_init(uint32_t uart_base, uart_init_t_def* uart_init);
void       hal_uart_deinit(uint32_t uart_base);
void       hal_uart_send_data(uint32_t uart_base, uint16_t data);
uint16_t   hal_uart_recv_data(uint32_t uart_base);
void     hal_uart_en(uint32_t uart_base, hal_en_t en);
void     hal_uart_baudrate_set(uint32_t uart_base, uint32_t baudrate);
void     hal_uart_it_en(uint32_t uart_base, uart_it_en_enum it_en);
void     hal_uart_it_disable(uint32_t uart_base, uart_it_en_enum it_en);
```

## 9. AES模块

密码学中的高级加密标准（Advanced Encryption Standard，AES），又称Rijndael加密法，是美国联
邦政府采用的一种区块加密标准

AES主要特征：

* 支持ECB和CBC加密
* 支持128bit、192bit、256bit长度的秘钥
* 支持一次加密128bit长度的明文
* 支持加密完成中断
* 支持busy标志

AES初始化和配置API接口

```
void     hal_uart_init(uint32_t uart_base, uart_init_t_def* uart_init);
void     hal_uart_deinit(uint32_t uart_base);
void     hal_uart_send_data(uint32_t uart_base, uint16_t data);
uint16_t   hal_uart_recv_data(uint32_t uart_base);
void       hal_uart_en(uint32_t uart_base, hal_en_t en);
void       hal_uart_baudrate_set(uint32_t uart_base, uint32_t baudrate);
void       hal_uart_it_en(uint32_t uart_base, uart_it_en_enum it_en);
void       hal_uart_it_disable(uint32_t uart_base, uart_it_en_enum it_en);
```

## 10. 通用异步收发器（UART）

通用异步收发器初始化，配置，收发数据API接口

```
void     hal_uart_init(uint32_t uart_base, uart_init_t_def* uart_init);
void     hal_uart_deinit(uint32_t uart_base);
void     hal_uart_send_data(uint32_t uart_base, uint16_t data);
uint16_t   hal_uart_recv_data(uint32_t uart_base);
void     hal_uart_en(uint32_t uart_base, hal_en_t en);
void     hal_uart_baudrate_set(uint32_t uart_base, uint32_t baudrate);
void     hal_uart_it_en(uint32_t uart_base, uart_it_en_enum it_en);
void     hal_uart_it_disable(uint32_t uart_base, uart_it_en_enum it_en);
```

## 11. DMA控制器（DMA）

DMA控制器初始化和配置API接口

```
void    hal_dma_init(uint32_t dma_x_base,dma_init_t_def* dma_init);  
void    hal_dma_deinit(``void``);
void    hal_dma_en(uint32_t dma_x_base,hal_en_t en);
void    hal_dma_set_dir(uint32_t dma_x_base,dma_dir_t dma_dir);    
void    hal_dma_set_mem_addr(uint32_t dma_x_base,uint32_t dma_mem_addr);    
void    hal_dma_set_p_addr(uint32_t dma_x_base,uint32_t dma_p_addr);
void    hal_dma_set_data_num(uint32_t dma_x_base,uint16_t dma_data_num);   
void    hal_dma_set_mem_size(uint32_t dma_x_base,dma_mem_size_t dma_mem_size);    
void    hal_dma_set_p_size(uint32_t dma_x_base,dma_p_size_t dma_p_size);    
void    hal_dma_set_pri_lev(uint32_t dma_x_base,dma_pri_lev_t dma_pri_lev); 
void    hal_dma_set_mem_inc_en(uint32_t dma_x_base,dma_mem_inc_en_t dma_mem_inc_en);
void    hal_dma_set_p_inc_en(uint32_t dma_x_base,dma_p_inc_en_t dma_p_inc_en); 
uint16_t  hal_dma_get_data_num(uint32_t dma_x_base);
```

DMA中断API接口

```
void      hal_dma_it_cfg(uint32_t dma_x_base,dma_it_flag_t dma_it_flag,hal_en_t en);
uint8_t   hal_dma_get_it_flag(uint32_t dma_x_base,dma_it_flag_t dma_it_flag);
void      hal_dma_clr_it_flag(uint32_t dma_x_base,dma_it_flag_t dma_it_flag);
uint8_t   hal_dma_get_status_flag(uint32_t dma_x_base,dma_status_flag_t dma_status_flag);
void      hal_dma_clr_status_flag(uint32_t dma_x_base,dma_status_flag_t dma_status_flag);
```

## 12. EXT中断

EX中断初始化API接口

```
void    hal_ext_init(ext_int_sense_t ext_int_sense,ext_trig_mode_t ext_trig_mode,hal_en_t en);
void    hal_ext_deinit(void);
```

## 13. FLASH控制(4K对齐)

FLASH初始化、配置、擦除、读写API接口

```
void    hal_flash_init(void);
void    hal_flash_deinit(void);
uint8_t hal_flash_read_by_cache(uint32_t offset, uint32_t length, uint8_t *buffer);
uint8_t hal_flash_read(uint32_t offset, uint32_t length, uint8_t *buffer);
uint8_t hal_flash_program(uint32_t offset, uint32_t length, uint8_t *buffer);
void    hal_flash_erase(uint32_t offset, uint32_t length);
```

## 14. GPIO和EXT中断(最大翻转速率40M)

GPIO和中断配置API接口

```
void      hal_gpio_init(uint32_t gpio_base, gpio_init_t_def *gpio_init);
uint16_t  hal_gpio_port_output_read(uint32_t gpio_base);
void      hal_gpio_pin_set(uint32_t gpio_base, gpio_pin_t pin);
void      hal_gpio_pin_reset(uint32_t gpio_base, gpio_pin_t pin);
```

## 15. I2C接口

I2C接口配置流程

1. 首先配置IIC的引脚，通过引脚功能复用I2C SCL和I2C SDA。
2. 设置 **IIC\_CR2** 中的 **freq** ，设置外设时钟，范围：0 - 63，单位Mhz,推荐使用默认值4。
3. 设置 **IIC\_CR1** 中的 **pe** 置零，先失能IIC总线，为了设置IIC的时钟。
4. 设置 **IIC\_CCR** 中的 **fs** 设置IIC为1，设置IIC为快速模式（0为标准模式）。
5. 设置 **IIC\_CCR** 中的 **duty** ，这个寄存器决定着快速模式下，T\_high和T\_low的比例关系。0： T\_low/T\_high = 2； 1: T\_low/T\_high = 16/9（使用说明请看 **IIC\_CCR** 中的 **ccr** 说明）。
6. 设置 **IIC\_CCR** 中的 **ccr** ，设置总线频率。T = T\_high + T\_low。
7. 设置 **IIC\_TRISE** 中的 **trise** ，这个寄存器决定了IIC的SCL和SDA的电平上升时间。上升时间 = trise \* Tpclk，推荐设置为0xF。
8. 设置 **IIC\_CCR1** 中的 **smbus** 寄存器为0，设成IIC模式（1为SMBus模式）。
9. 设置 **IIC\_CCR1** 中的 **pe** 为1，使能IIC外设。
10. 开始配置 **I2C\_CR1** 中的 **swrst** ,复位IIC总线。先置位1，然后再置为0.

```
标准模式下：
T_high = CCR * Tpclk，
T_low = 2 * CCR * Tpclk ,
T = CCR * Tpclk + 2 * CCR * Tpclk
快速模式下：
duty = 0时：
T_high = CCR * Tpclk
T_low = 2 × CCR * Tpclk
duty = 1时：
T_high = 9 * CCR * Tpclk
T_low = 16 * CCR * Tpclk
时钟配置举例：
快速模式下：400k SCL周期为2.5us，假设外设工作时钟为40MHZ 所以Tpckl = 1/40M = 0.025us
如果duty = 0;
fs = 1;
Thigh = CCR × TPCLK1
Tlow = 2 × CCR × TPCLK1
Thigh + Tlow = 2.5us
所以 3 * CCR × TPCLK1 = 2.5us CCR = 33
如果duty = 1;
fs = 1;
Thigh = 9 * CCR × TPCLK1
Tlow = 16 × CCR × TPCLK1
Thigh + Tlow = 2.5us
所以 3 * CCR × TPCLK1 = 2.5us CCR = 4
duty -> I2C_CCR->duty
fs -> I2C_CCR->fs
Thigh -> I2C_CLK高电平时间
Tlow -> I2C_CLK低电平时间
TPCLK1 -> 外设时钟
CCR -> I2C_CCR->ccr
```

I2C初始化、配置、收发数据API接口

```
void            hal_i2c_init(uint32_t i2c_x_base,i2c_init_t_def* i2c_init);             
void            hal_i2c_deinit(void);
void            hal_i2c_set_peripheral_clock_freq(uint32_t i2c_x_base, uint32_t peripheral_clock_freq);
void            hal_i2c_en(uint32_t i2c_x_base,hal_en_t en);
void            hal_i2c_master_reset(uint32_t i2c_x_base);
uint8_t         hal_i2c_master_start(uint32_t i2c_x_base,uint32_t timeout);
void            hal_i2c_master_stop(uint32_t i2c_x_base);
void            hal_i2c_master_send_data(uint32_t i2c_x_base,uint8_t data);
uint8_t         hal_i2c_master_recv_data(uint32_t i2c_x_base);
uint8_t         hal_i2c_master_wait_addr(uint32_t i2c_x_base,uint32_t timeout);
```

## 16. I2S接口

I2S接口特性：

* 只支持从机模式，I2S时钟由主机提供
* 全双工模式双声道立体声
* 可配置的数据位宽，支持12、16、20、24和32位宽的数据
* 支持飞利浦I2S标准
* I2S不支持DMA，且只有一路
* I2S测试时一定要注意先开主机再开从机，否则初始化从机的时候，主机可能会误收数据
* I2S作为从机接收的时候，可能会丢掉帧头帧尾的数据

I2S初始化和配置API接口

```
void        hal_i2s_init(uint32_t i2s_base,i2s_init_t_def* i2s_init);  
void        hal_i2s_deinit(void);
void        hal_i2s_rx_en(uint32_t i2s_base,hal_en_t en);          
void        hal_i2s_tx_en(uint32_t i2s_base,hal_en_t en);          
void        hal_i2s_en(uint32_t i2s_base,hal_en_t en);
void        hal_i2s_send_data(uint32_t i2s_base,uint32_t left_data,uint32_t right_data);
void        hal_i2s_recv_data(uint32_t i2s_base,uint32_t *left_data,uint32_t *right_data);
```

I2S中断API接口

```
void        hal_i2s_it_cfg(uint32_t i2s_base,i2s_it_flag_t i2s_it_flag,hal_en_t en);
uint8_t     hal_i2s_get_it_flag(uint32_t i2s_base,i2s_it_flag_t i2s_it_flag);
void        hal_i2s_clr_it_flag(uint32_t i2s_base,i2s_it_flag_t i2s_it_flag);
```

## 17. SDIO接口（SDIO）

SDIO接口特性：

* SDIO是一个通过可编程预分频器驱动的24位自动装载计数器构成。 SDIO调试时，CMD、D0、D1、D2、D3必须接上拉电阻，对于LN8XXX芯片，我们必须选择高速模式，否则会出现数据异常的问题。
* 最大速度40Mhz，推荐速度25Mhz(实测最大到22Mhz,注意线材连接)
* 只能做从机使用

SDIO初始化、配置，收发数据API接口

```
void      hal_sdio_device_init(sdio_init_t *sdio_init);
void      hal_sdio_device_deinit(void);
void      hal_sdio_device_clear_busy(void);
void      hal_sdio_device_set_busy(void);
uint32_t* hal_sdio_device_get_recv_buf_addr(void);
void      hal_sdio_device_set_recv_buf_addr(uint8_t * addr);
uint16_t  hal_sdio_device_get_recv_buf_size(void);
uint32_t* hal_sdio_device_get_send_buf_addr(void);
void      hal_sdio_device_set_send_buf_addr(uint8_t *addr);
void      hal_sdio_device_set_send_buf_size(uint32_t size);
void      hal_sdio_device_trig_host_data1_int(void);
void      hal_sdio_device_trig_host_s2m1_int(void);
uint8_t*  hal_sdio_device_cis_func_get(sdio_dev_func_num_t fn);
uint8_t   hal_sdio_device_cis_fn_set(sdio_dev_func_num_t fn,uint32_t offset,uint8_t value);
void      hal_sdio_device_cis_init(void);
void      hal_sdio_device_it_cfg(uint32_t sdio_dev_int_flag,hal_en_t en);
uint8_t   hal_sdio_device_it_get_flag(sdio_dev_int_flag_t sdio_dev_int_flag);
void      hal_sdio_device_it_clr_flag(sdio_dev_int_flag_t sdio_dev_int_flag);
```

## 18. WS2811控制接口

WS2811接口特性：

* WS2811控制接口专门用于控制WS2811系列LED控制芯片（或支持WS2811芯片协议的其它芯片）而专门研发的外设
* 支持DMA传输
* 支持自定义波特率（7位宽度）
* 支持一路中断(DR为空时触发)

```
对于WS2811来说，最重要的就是T0L,T1L,T0H,T1H的时间，下面简单叙述下这几个值如何设置：
首先我们需要知道的是，我们已经人为的固定了T0H和T0L的比例为 1 ：4 ,T1H和T1L的比例为 4 : 1,(根据WS2811手册这个比例是成立的)
而这里所说的波特率就是单个0(T0)或单个1(T1)的时间，所以T0 = T0H + T0L, T1 = T1H + T1L,
根据WS2811手册和T0H,T0L,T1H,T1L之间的比例，一般我们认为(以下数据为实测值，可以直接使 用):
T0 = 1125 ns,T0L = 900ns,T0H = 225ns T1 = 1125 ns,T1L = 225ns,T1H = 900ns 而 T0 = T1 = (br + 1) * 5 * (1 / pclk)
其中br为WS2811 BR寄存器，pclk为外设时钟，FPGA上pclk = 40M,所以 br = 8;
```

WS2811初始化和配置API接口

```
void   hal_ws2811_init(uint32_t ws2811_base, ws2811_init_t_def *ws2811_init);
void   hal_ws2811_deinit(void);
void   hal_ws2811_en(uint32_t ws2811_base,hal_en_t en);
void   hal_ws2811_set_data(uint32_t ws2811_base,uint8_t value);
void   hal_ws2811_dma_en(uint32_t ws2811_base,hal_en_t en);
uint8_thal_ws2811_get_status_flag(uint32_t ws2811_base,ws2811_status_flag_t ws2811_status_flag);
```

WS2811中断API接口

```
void   hal_ws2811_it_cfg(uint32_t ws2811_base,ws2811_it_flag_t ws2811_it_flag ,hal_en_t en);
uint8_thal_ws2811_get_it_flag(uint32_t ws2811_base,ws2811_it_flag_t ws2811_it_flag);
```

## 19. 模数转换器（ADC）

ADC接口特性：

* 支持6通道轮询采样
* 支持单次和连续转换模式
* 支持DMA
* 支持转换完成中断
* 12bit分辨率

粗测电压值对应ADC原始值关系：

| 0 | 32 | 0.4 | 480 | 1.1 | 1248 | 3.0 | 3360 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 0.01 | 32 | 0.51 | 608 | 1.5 | 1696 | 3.1 | 3488 |
| 0.11 | 160 | 0.8 | 928 | 2.0 | 2272 | 3.2 | 3616 |
| 0.33 | 416 | 1 | 1120 | 2.5 | 2848 | 3.3 | 3744 |

ADC初始化和配置API接口

```
void        hal_adc_init(uint32_t adc_base,adc_init_t_def* adc_init);
void        hal_adc_deinit(void);
void        hal_adc_en(uint32_t adc_base,hal_en_t en);
void        hal_adc_start_conv(uint32_t adc_base);
void        hal_adc_stop_conv(uint32_t adc_base);
uint8_t     hal_adc_get_conv_status(uint32_t adc_base, adc_ch_t ch);
void        hal_adc_clr_conv_status(uint32_t adc_base, adc_ch_t ch);
void        hal_adc_spe_sw_start(uint32_t adc_base);
uint16_t    hal_adc_get_data(uint32_t adc_base,adc_ch_t ch);
```