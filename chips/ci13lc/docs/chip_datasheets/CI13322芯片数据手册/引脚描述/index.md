<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13322%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI13322_datasheet V1.2_chs_20250725.pdf)

# 引脚图和功能描述

## 引脚图

![CI13322芯片引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13322%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI13322%E5%BC%95%E8%84%9A%E5%AE%9A%E4%B9%89.png)

图P-1 管脚顺序及定义图

## 管脚描述

表P-1 管脚描述

| 管脚号 | 管脚名称 | 类型 | 是否支持5V电平 | 上电默认状态 | 管脚功能 |
| --- | --- | --- | --- | --- | --- |
| 1 | VDD11 | P | - | - | 1.1V LDO输出管脚，同时也是内核供电输入管脚，外接4.7uF电容 |
| 2 | XIN | I | - | - | ●XIN（上电默认状态) ●GPIO PA0 ●PWM2 |
| 3 | XOUT | O | - | - | ●XOUT（上电默认状态） ●GPIO PA1 |
| 4 | SRC—sel | I | - | IN, T+U | ● 时钟选择，默认NC |
| 5 | BOOT0 | I | - | IN, T+U | ● BOOT模式选择，默认NC |
| 6 | PA2 | IO | √ | IN,T+D | ● GPIO PA2（上电默认状态） ● IIS\_SDI ● IIC\_SDA ● UART1\_TX ● PWM0 ● PWMP |
| 7 | PA3 | IO | √ | IN,T+D | ● GPIO PA3（上电默认状态） ● IIS\_LRCLK ● IIC\_SCL ●UART1\_RX1 ● PWM1 ●PWMN |
| 8 | PA4 | IO | √ | IN,T+U | ● GPIO PA4（上电默认状态）/PG\_EN（根据上电时电平状态判断是否进行编程，高电平时启动编程功能） ● IIS\_SDO ● - ● -  ● PWM2  ● PWMP |
| 9 | PA5 | IO | √ | IN,T+D | ● GPIO PA5（上电默认状态） ● IIS\_SCLK ● -  ● UART2\_TX ● PWM3 ● PWMN0 |
| 10 | PA6 | IO | √ | IN,T+D | ● GPIO PA6（上电默认状态） ● IIS\_MCLK ● -  ● UART2\_RX ● PWM0 |
| 11 | PA7 | IO | √ | IN,T+D | ● GPIO PA7（上电默认状态） ● PWM0 ● TX1  ●INT0 |
| 12 | PB0 | IO | √ | IN,T+D | ● GPIO PB0（上电默认状态） ● PWM1 ● RX1  ●INT1 |
| 13 | PB1 | IO | √ | IN,T+D | ● GPIO PB1（上电默认状态） ● PWM2 ● TX2  ●PWMP |
| 14 | PB2 | IO | √ | IN,T+D | ● GPIO PB2（上电默认状态） ● PWM3 ● RX2  ●PWMP |
| 15 | PB5 | IO | √ | IN,T+U | ● GPIO PB5（上电默认状态） ● UART0\_TX ● IIC\_SDA ● PWM1 ●PWMP |
| 16 | PB6 | IO | √ | IN,T+U | ● GPIO PB6（上电默认状态） ● UART0\_RX ● IIC\_SCL ● PWM2 ●PWMN |
| 17 | TEST\_EN | I | - | - | ●测试使能管脚，默认NC |
| 18 | RSTN | I | - | - | ●复位管脚，低电平复位，默认NC |
| 19 | PC4 | IO | - | IN,T+U | ●保留（上电默认状态）G  ●PC4 ●SCL ●PWM0 |
| 20 | PC1 | IO | - | IN,T+D | ●保留（上电默认状态）G  ●PC1 ●TX2 ●PWM3 |
| 21 | PC2 | IO | - | IN,T+U | ●保留（上电默认状态）G  ●PC2 ●RX2 ●PWM2 |
| 22 | PC3 | IO | - | IN,T+U | ●保留（上电默认状态）G  ●PC3 ●SDA ●PWM1 |
| 23 | PC5 | IO | - | IN,T+U | ●保留（上电默认状态）G  ●PC5 ●BOOT1 |
| 24 | MICN | I | - | - | Microphone N input |
| 25 | MICP | I | - | - | Microphone P input |
| 26 | MICBIAS | O | - | - | Microphone bias output |
| 27 | VCM | O | - | - | VCM Output |
| 28 | AGND | P | - | - | Analog ground |
| 29 | HPOUT | O | - | - | DAC output |
| 30 | AVDD | P | - | - | ● 内部LDO-3.3V输出 ● 内部模拟电路3.3V供电输入 *Note1* |
| 31 | VIN5V | P | - | - | ● 供电电压输入，供电电压范围3.6V～5.5V *Note1* |
| 32 | PB7 | IO | - | IN,T+U | ●GPIO PB7 |
| 33 | GND | P | - | - | 接地管脚 |

**Note1：管脚需外接4.7uF电容**  
**Note2：上电时该管脚为高电平，系统将进入编程模式**

## 符号定义

I 输入

O 输出

IO 双向

P 电源或地

T+D 三态正下拉

T+U 三态正上拉

OUT 上电默认为输出模式

IN 上电默认为输入模式

所有IO支持驱动能力可配，上下拉电阻可配。

## 复用功能

表P-2 IO复用功能

| Pin Name | Function1 | Function2 | Function3 | Function4 | Function5 | Function6 | Specific Function |
| --- | --- | --- | --- | --- | --- | --- | --- |
| PA0 | PA0 | PWM2 | - | - | - | - | XIN |
| PA1 |  |  |  |  |  |  | XOUT |
| PA2 | PA2 | - | IIC\_SDA | UART1\_TX | PWM0 | PWMP | - |
| PA3 | PA3 | - | IIC\_SCL | UART1\_RX | PWM1 | PWMN | - |
| PA4 | PA4 | - | - | - | PWM2 | - | PG\_EN *Note3* |
| PA5 | PA5 | SCLK |  | TX2 | PWM3 | PWMN |  |
| PA6 | PA6 | MCLK |  | RX2 | PWM0 |  |  |
| PA7 | PA7 | PWM0 | TX1 | INT0 |  |  |  |
| PB0 | PB0 | PWM1 | RX1 | INT1 |  |  |  |
| PB1 | PB1 | PWM2 | TX2 | PWMP |  |  |  |
| PB2 | PB2 | PWM3 | RX2 | PWMN |  |  |  |
| PB5 | PB5 | UART0\_TX | IIC\_SDA | PWM1 | PWMP | - | - |
| PB6 | PB6 | UART0\_RX | IIC\_SCL | PWM2 | PWMN | - | - |
| PC4 | - | PC4 | SCL | PWM0 |  |  |  |
| PC1 | - | PC1 | TX2 | PWM3 |  |  |  |
| PC2 | - | PC2 | RX2 | PWM2 |  |  |  |
| PC3 | - | PC3 | SDA | PWM1 |  |  |  |

**Note3：PA4（PG\_EN）管脚内部默认上拉，当上电时系统检测到该管脚为高电平、且UART0接口上有固件升级信号，则自动进入升级模式，此时可通过升级工具对芯片内部的 Flash进行编程。若此时系统未检测到UART0接口上有固件升级信号、或检测到PA4管脚的电压为低电平，都将进入正常工作模式。**