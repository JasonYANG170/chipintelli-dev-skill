<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13162P%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI13162P_datasheet V1.0_chs_20241121.pdf)

# 引脚图和功能描述

## 引脚图

![CI13162P芯片引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13162P%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI13162P%E8%8A%AF%E7%89%87%E5%BC%95%E8%84%9A%E5%AE%9A%E4%B9%89.png)

图P-1 管脚顺序及定义图

## 管脚描述

表P-1 管脚描述

| 管脚号 | 管脚名称 | 类型 | 是否支持5V电平 | 上电默认状态 | 管脚功能 |
| --- | --- | --- | --- | --- | --- |
| 1 | IN- | IO | - | - | ● 功放反向输入 |
| 2 | SPKN | IO | - | - | ● 功放N端输出 |
| 3 | SPKP | IO | - | - | ● 功放P端输出 |
| 4 | VIN5V | P | - | - | ● 供电电压输入，供电电压范围3.6V～5.5V *Note1* |
| 5 | VDD11 | P | - | - | ● LDO-1.1V 输出 ● 内核1.1V供电输入 *Note1* |
| 6 | GND | P | - | - | Ground |
| 7 | PA2 | IO | √ | IN,T+D | ● GPIO PA2（上电默认状态） ● IIS\_SDI ● IIC\_SDA ● UART1\_TX ● PWM0 ● PWMP |
| 8 | PA3 | IO | √ | IN,T+D | ● GPIO PA3（上电默认状态） ● IIS\_LRCLK ● IIC\_SCL ●UART1\_RX1 ● PWM1 ●PWMN |
| 9 | PB5 | IO | √ | IN,T+U | ● GPIO PB5（上电默认状态） ● UART0\_TX ● IIC\_SDA ● PWM1 ●PWMP |
| 10 | PB6 | IO | √ | IN,T+U | ● GPIO PB6（上电默认状态） ● UART0\_RX ● IIC\_SCL ● PWM2 ●PWMN |
| 11 | VCM | O | - | - | ● VCM Output  ● PGEN  *Note2* |
| 12 | MICP | I | - | - | Microphone P input |
| 13 | MICBIAS | O | - | - | Microphone bias output |
| 14 | HPOUT | O | - | - | DAC output |
| 15 | AVDD | P | - | - | ● 内部LDO-3.3V输出 ● 内部模拟电路3.3V供电输入 *Note1* |
| 16 | VREF/IN+ | IO | - | - | ● 功放电压基准端 |

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
| PA2 | PA2 | - | IIC\_SDA | UART1\_TX | PWM0 | PWMP | - |
| PA3 | PA3 | - | IIC\_SCL | UART1\_RX | PWM1 | PWMN | - |
| PB5 | PB5 | UART0\_TX | IIC\_SDA | PWM1 | PWMP | - | - |
| PB6 | PB6 | UART0\_RX | IIC\_SCL | PWM2 | PWMN | - | - |