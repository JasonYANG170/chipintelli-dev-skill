<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI23161%26CI23162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI23161&CI23162芯片数据手册.zip)

# 引脚图和功能描述

## 引脚图

![CI23161&CI23162芯片引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI23161%26CI23162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI23161%26CI23162%E8%8A%AF%E7%89%87%E5%BC%95%E8%84%9A%E5%AE%9A%E4%B9%89.png)

图P-1 管脚顺序及定义图

## 管脚描述

表P-1 管脚描述

| 管脚号 | 管脚名称 | 类型 | 是否支持5V电平 | 上电默认状态 | 管脚功能 |
| --- | --- | --- | --- | --- | --- |
| 1 | AVDD | P | - | - | ● 内部LDO-3.3V输出 ● 内部模拟电路3.3V供电输入 *Note1* |
| 2 | VIN5V | P | - | - | ● 供电电压输入，供电电压范围3.6V～5.5V *Note1* |
| 3 | VDD11 | P | - | - | ● LDO-1.1V 输出 ● 内核1.1V供电输入 *Note1* |
| 4 | GND1 | P | - | - | Ground |
| 5 | VDDRF | P | - |  | ●RF电源输入 |
| 6 | XOUT | IO | - | - | ● 晶振输出 *Note6* |
| 7 | XIN | IO | - | - | ● 晶振输入 *Note6* |
| 8 | GND2 | P | - | - | ● Ground |
| 9 | RF | IO | - |  | ● RF天线 |
| 10 | PB5 | IO | √ | IN,T+U | ● GPIO PB5（上电默认状态） ● UART0\_TX ● IIC\_SDA ● PWM1 ●PWMP |
| 11 | PB6 | IO | √ | IN,T+U | ● GPIO PB6（上电默认状态） ● UART0\_RX ● IIC\_SCL ● PWM2 ●PWMN |
| 12 | PC4 | IO | - | IN,T+U | ●保留（上电默认状态）G  ●PC4 ●SCL ●PWM0 |
| 13 | HPOUT/PGEN | O | - | - | ●DAC output ●PGEN *Note2* |
| 14 | MICP | I | - | - | Microphone P input |
| 15 | MICBIAS | O | - | - | Microphone bias output |
| 16 | VCM | O | - | - | VCM Output |

备注

Note1：管脚需外接4.7uF电容。

Note2：上电时该管脚为高电平，系统将进入编程模式。

Note6： Pin6 XOUT 及 Pin7 XIN 为蓝牙晶体输入输出脚，仅为蓝牙提供时钟；语音时钟由芯片内置的 RC 振荡器提供。

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
| PB5 | PB5 | UART0\_TX | IIC\_SDA | PWM1 | PWMP | - | - |
| PB6 | PB6 | UART0\_RX | IIC\_SCL | PWM2 | PWMN | - | - |
| PC4 | - | PC4 | SCL | PWM0 | - | - | ICE |