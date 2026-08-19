<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13082V%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI13082V_datasheet V1.1_chs_20260707.pdf)

# 引脚图和功能描述

## 引脚图

![CI13082V芯片引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13082V%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI13082V%E5%BC%95%E8%84%9A%E5%AE%9A%E4%B9%89.png)

图P-1 管脚顺序及定义图

## 管脚描述

表P-1 管脚描述

| 管脚号 | 管脚名称 | 类型 | 是否支持5V电平 | 上电默认状态 | 管脚功能 |
| --- | --- | --- | --- | --- | --- |
| 1 | VDD33 | P | - | - | ● 供电电压输入，供电电压范围3.0V～3.6V *Note1* |
| 2 | VDD11 | P | - | - | ● 1.1V LDO输出管脚，同时也是内核供电输入管脚，外接4.7uF电容 |
| 3 | GND | P | - | - | 接地管脚 |
| 4 | PB5 | IO | √ | IN,T+U | ● GPIO PB5（上电默认状态） ● UART0\_TX ● IIC\_SDA ● PWM1 ● PWMP |
| 5 | PB6 | IO | √ | IN,T+U | ● GPIO PB6（上电默认状态） ● UART0\_RX ● IIC\_SCL ● PWM2 ● PWMN |
| 6 | HPOUT | IO | - | IN,T+D | ● DAC output ● PC0 ● - ● - ● PWM0 ● PGEN *Note2* |
| 7 | MICP | I | - | - | ● Microphone P input |
| 8 | VCM | P | - | - | ● VCM电压、外接4.7uf电容 |

**Note1：管脚需外接4.7uF电容**  
**Note2：上电时该管脚为高电平时，系统将进入编程模式**

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
| PB5 | PB5 | UART0\_TX | IIC\_SDA | PWM1 | PWMP | - | - |
| PB6 | PB6 | UART0\_RX | IIC\_SCL | PWM2 | PWMN | - | - |
| PC1 | - | PC1 | TX2 | PWM3 | - | - | - |
| PC0 *Note3* | PC0 | - | - | PWM0 | - | - | PGEN |

**Note3：HPOUT与PC0（PGEN）管脚复用，内部默认下拉，上电后软件可配置其功能。当上电时系统检测到该管脚为高电平、且UART0接口上有固件升级信号，则自动进入升级模式，此时可通过升级工具对芯片内部的 Flash进行编程。若此时系统未检测到UART0接口上有固件升级信号、或检测到PC0管脚的电压为低电平，都将进入正常工作模式。**