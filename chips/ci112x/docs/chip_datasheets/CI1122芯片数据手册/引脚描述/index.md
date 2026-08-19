<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1122%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1122芯片数据手册.pdf)

# 引脚描述

CI1122芯片引脚图如图2所示：

![CI1122芯片引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1122%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI1122%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-2.png)

图2 CI1122芯片引脚图

芯片各个引脚功能如下表描述：

表2 芯片引脚功能描述

| Pin Number | Pin name | Pin type | IO driving capability | IO power-on default state | Alternate functions |
| --- | --- | --- | --- | --- | --- |
| 1 | XIN | I | - | - | 12.288MHZ crystal oscillator interface |
| 2 | PLL\_AVDD12 | P | - | - | PLL 1.2V power supply |
| 3 | PLL\_AVSS12 | P | - | - | PLL ground |
| 4 | AIN0 | IO | 4mA | IN,T+D | Default:ADC0 Input Can be configured as Digital functions pin. 1.GPIO0\_0 2.PWM0 Output |
| 5 | AIN1 | IO | 4mA | IN,T+D | Default:ADC1 Input Can be configured as Digital functions pin. 1.GPIO0\_1 2.PWM1 Output |
| 6 | AIN2 | IO | 4mA | IN,T+D | Default:ADC2 Input Can be configured as Digital functions pin. 1.GPIO0\_2 2.PWM2 Output |
| 7 | AIN3 | IO | 4mA | IN,T+D | Default:ADC3 Input Can be configured as Digital functions pin. 1.GPIO0\_3 2.PWM3 Output |
| 8 | VCC33 | P | - | - | 3.3V power supply |
| 9 | VCC12 | P | - | - | 1.2V Core power supply |
| 10 | GPIO0\_4 | IO | 4mA | IN,T+D | 1.GPIO0\_4 2.PWM2 Output |
| 11 | GPIO0\_5 | IO | 4mA | IN,T+U | 1.Reserved 2.GPIO0\_5 3.UART1\_TX |
| 12 | GPIO0\_6 | IO | 4mA | IN,T+D | 1.Reserved 2.GPIO0\_6 3.UART1\_RX |
| 13 | GPIO0\_7 | IO | 4mA | IN,T+U | 1.Reserved 2.GPIO0\_7 3.PWM3 |
| 14 | GPIO1\_0 | IO | 4mA | IN,T+D | 1.Reserved 2.GPIO1\_0 3.PWM4 |
| 15 | PWM0 | IO | 4mA | IN,T+D | 1.GPIO1\_1 2.PWM0 Output 3.EXT\_INT[0] |
| 16 | PWM1 | IO | 4mA | IN,T+D | 1.GPIO1\_2 2.PWM1 Output 3.EXT\_INT[1] |
| 17 | PWM2 | IO | 4mA | IN,T+D | 1.GPIO1\_3 2.PWM2 Output |
| 18 | PWM3 | IO | 4mA | IN,T+D | 1.GPIO1\_4 2.PWM3 Output |
| 19 | PWM4 | IO | 4mA | IN,T+D | 1.GPIO1\_5 2.PWM4 Output |
| 20 | PWM5 | IO | 4mA | IN,T+D | 1.GPIO1\_6 2.PWM5 Output |
| 21 | VCC33 | P | - | - | 3.3V power supply |
| 22 | UART0\_RX | IO | 4mA | IN,T+U | 1.GPIO1\_7 2.UART0\_RX: Receive channel of UART0 |
| 23 | UART0\_TX | IO | 4mA | IN,T+U | 1.GPIO2\_0 2.UART0\_TX: Transmit channel of UART0 |
| 24 | TEST\_EN | I | - | - | Internal pull-down 0—functional mode 1—TEST mode |
| 25 | KEY\_RSTn | I | - | - | External reset input. Pull this pin low to reset device to initial state. Has internal weak pull-up. |
| 26 | UART1\_TX | IO | 4mA | IN,T+U | 1.GPIO2\_1 2.UART1\_TX: Transmit channel of UART1 |
| 27 | UART1\_RX | IO | 4mA | IN,T+U | 1.GPIO2\_2 2.UART1\_RX: Receive channel of UART1 |
| 28 | VCC33 | P | - | - | 3.3V power supply |
| 29 | VCC12 | P | - | - | 1.2V Core power supply |
| 30 | IIS1\_SDI | IO | 4mA | IN,T+D | 1.GPIO2\_3 2.IIS1\_SDI: Serial Data Input for IIS1 interface |
| 31 | IIS1\_LRCLK | IO | 4mA | IN,T+D | 1.GPIO2\_4 2.IIS1\_LRCLK: IIS1 interface LRCLK clock |
| 32 | IIS1\_SDO | IO | 4mA | IN,T+D | 1.GPIO2\_5 2.IIS1\_SDO: Serial Data Output for IIS1 interface |
| 33 | IIS1\_SCLK | IO | 4mA | IN,T+D | 1.GPIO2\_6 2.IIS1\_SCLK: Serial Clock for IIS1 interface |
| 34 | IIS1\_MCLK | IO | 4mA | IN,T+D | 1.GPIO2\_7(UART\_UPDATE\_EN) At start-up, this pin is used to select one of two functional modes: 1—Start serial port upgrade service and program 0—Start directly from Flash 2.IIS1\_MCLK:Master Clock for IIS1 reference |
| 35 | IIC0\_SCL | IO | 4mA | IN,T+U | 1.GPIO3\_0 2.IIC0\_SCL: IIC0 Serial Clock 3.UART1\_TX |
| 36 | IIC0\_SDA | IO | 4mA | IN,T+U | 1.GPIO3\_1 2.IIC0\_SDA: IIC0 Serial Data 3.UART1\_RX |
| 37 | MICN | I | - | - | Microphone N input |
| 38 | MICP | I | - | - | Microphone P input |
| 39 | MICBIAS | O | - | - | Microphone bias output |
| 40 | AGND | P | - | - | Analog ground |
| 41 | AVDD | P | - | - | 3.3V analog supply |
| 42 | AGND | P | - | - | Analog ground |
| 43 | HPOUT | O | - | - | DAC output |
| 44 | AVDD | P | - | - | 3.3V analog supply |
| 45 | VCM\_ADC | O | - | - | ADC VCM output |
| 46 | VCM\_DAC | O |  | - | DAC VCM output |
| 47 | GPIO3\_2 | IO | 4mA | IN,T+D | 1.GPIO3\_2 2.PWM4 Output |
| 48 | XOUT | O | - | - | 12.288MHZ crystal oscillator interface |

***上表中 IO引脚的状态定义如下：***

I 输入

O 输出

IO 双向

P 电源或地

T+D 三态正下拉

T+U 三态正上拉

OUT 上电默认为输出模式

IN 上电默认为输入模式