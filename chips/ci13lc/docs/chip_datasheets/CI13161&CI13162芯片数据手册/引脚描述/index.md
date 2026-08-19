<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13161%26CI13162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI13161&CI13162芯片数据手册.zip)

# 引脚图和功能描述

## 引脚图

![CI13161&CI13162芯片引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13161%26CI13162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI13161%26CI13162%E8%8A%AF%E7%89%87%E5%BC%95%E8%84%9A%E5%AE%9A%E4%B9%89.png)

图P-1 管脚顺序及定义图

## 管脚描述

表P-1 管脚描述

| 管脚号 | 管脚名称 | 类型 | 是否支持5V电平 | 上电默认状态 | 管脚功能 |
| --- | --- | --- | --- | --- | --- |
| 1 | AVDD | P | - | - | ● 内部LDO-3.3V输出 ● 内部模拟电路3.3V供电输入 *Note1* |
| 2 | VIN5V | P | - | - | ● 供电电压输入，供电电压范围3.6V～5.5V *Note1* |
| 3 | PA0 | IO | - | - | ● XIN（上电默认状态） ● GPIO PA0 ● PWM2 |
| 4 | VDD11 | P | - | - | ● LDO-1.1V 输出 ● 内核1.1V供电输入 *Note1* |
| 5 | GND | P | - | - | Ground |
| 6 | PA2 | IO | √ | IN,T+D | ● GPIO PA2（上电默认状态） ● IIC\_SDA ● UART1\_TX ● PWM0 ● PWMP |
| 7 | PA3 | IO | √ | IN,T+D | ● GPIO PA3（上电默认状态） ● IIC\_SCL ● UART1\_RX1 ● PWM1 ● PWMN |
| 8 | PA4 | IO | √ | IN,T+U | ● GPIO PA4（上电默认状态）/PG\_EN （*Note2*） ● PWM2 |
| 9 | PB5 | IO | √ | IN,T+U | ● GPIO PB5（上电默认状态） ● UART0\_TX ● IIC\_SDA ● PWM1 ● PWMP |
| 10 | PB6 | IO | √ | IN,T+U | ● GPIO PB6（上电默认状态） ● UART0\_RX ● IIC\_SCL ● PWM2 ● PWMN |
| 11 | VCM | O | - | - | VCM Output |
| 12 | MICN | I | - | - | Microphone N input |
| 13 | MICP | I | - | - | Microphone P input |
| 14 | MICBIAS | O | - | - | Microphone bias output |
| 15 | AGND | P | - | - | Analog ground |
| 16 | HPOUT | O | - | - | DAC output |

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

| Pin Number | Function1 | Function2 | Function3 | Function4 | Function5 | Function6 | Specific Function |
| --- | --- | --- | --- | --- | --- | --- | --- |
| PA0 | PA0 | PWM2 | - | - | - | - | XIN |
| PA2 | PA2 | - | IIC\_SDA | UART1\_TX | PWM0 | PWMP | - |
| PA3 | PA3 | - | IIC\_SCL | UART1\_RX | PWM1 | PWMN | - |
| PA4 | PA4 | - | - | - | PWM2 | - | PG\_EN *Note3* |
| PB5 | PB5 | UART0\_TX | IIC\_SDA | PWM1 | PWMP | - | - |
| PB6 | PB6 | UART0\_RX | IIC\_SCL | PWM2 | PWMN | - | - |

**Note3：PA4（PG\_EN）管脚内部默认上拉，当上电时系统检测到该管脚为高电平、且UART0接口上有固件升级信号，则自动进入升级模式，此时可通过升级工具对芯片内部的 Flash进行编程。若此时系统未检测到UART0接口上有固件升级信号、或检测到PA4管脚的电压为低电平，都将进入正常工作模式。**