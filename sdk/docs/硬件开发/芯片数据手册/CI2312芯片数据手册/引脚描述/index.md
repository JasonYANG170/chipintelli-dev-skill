<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI2311&CI2312芯片数据手册.zip)

# 引脚描述

CI231X系列芯片引脚图如图P-1所示：

![CI2312芯片引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI2312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-2.png)

图P-1 芯片引脚图

芯片各个引脚功能如下表描述：

表P-1 芯片引脚功能描述

| Pin Number | Pin name | Pin type | IO 5V-Tolerant | IO power-on default state | Description and alternate functions |
| --- | --- | --- | --- | --- | --- |
| 1 | AVDD | P | - | - | 3.3V 模拟 LDO 输出管脚，同时也是模拟 供电输入管脚，外接 4.7uF 电容 |
| 2 | VIN5V | P | - |  | VIN5V 是 PMU 电源输入引脚。正常工作 输入电压范围为 3.6V-5.5V。外部连接一 个 4.7uf 输入电容器。该引脚的最大输入 电压为 6.5V。请注意该引脚需要添加过压 和浪涌保护装置，例如 TVS 和 4.7 欧姆电 阻，以防止浪涌冲击 |
| 3 | VDD33 | P | - | - | 3.3V LDO 输出管脚，外接 4.7uF 电容 |
| 4 | VDD11 | P | - | - | 1.1V LDO 输出管脚，同时也是内核供电 输入管脚，外接 4.7uF 电容 |
| 5 | GND | P | - | - | Ground PAD |
| 6 | PA2 | IO | √ | IN,T+D | 1. GPIO PA2（上电默认状态） 2. IIS\_SDI 3. IIC\_SDA 4. UART1\_TX 5. PWM0 |
| 7 | PA3 | IO | √ | IN,T+D | 1. GPIO PA3（上电默认状态） 2. IIS\_LRCLK 3. IIC\_SCL 4. UART1\_RX1 5. PWM1 |
| 8 | PA4 | IO | √ | IN,T+U | 1. GPIO PA4（上电默认状态）/PG\_EN（根据上电时电平状态判断是否进行编程，高电平时启动编程功能） 2. IIS\_SDO 3. PWM2 |
| 9 | XTALN | I | - | - | 外部晶振管脚正极 |
| 10 | XTALP | I | - | - | 外部晶振管脚负极 |
| 11 | RF | - | - | - | RF天线 |
| 12 | GND | P | - | - | Ground PAD |
| 13 | VDD\_RF | P | - |  | VDD\_RF 是蓝牙电源输入引脚。输入电压为 3.3V。外部连接一 个 4.7uf 输入电容器。 |
| 14 | PB5 | IO | √ | IN,T+U | 1. GPIO PB5（上电默认状态） 2. UART0\_TX 3. IIC\_SDA 4. PWM1 |
| 15 | PB6 | IO | √ | IN,T+U | 1. GPIO PB6（上电默认状态） 2. UART0\_RX 3. IIC\_SCL 4. PWM2 |
| 16 | AIN3 | IO | - | IN,T+U | 1. 保留（上电默认状态）  2. GPIO PC3  3. PWM1  4.PDM\_DAT  5. SAR ADC input channel 3 |
| 17 | AIN2 | IO | - | IN,T+U | 1. 保留（上电默认状态）  2. GPIO PC4  3. PWM0  4. PDM\_CLK  5.SAR ADC input channel 2 |
| 18 | HPOUT | O | - | - | DAC output |
| 19 | MICPR | I | - | - | Right Microphone P input |
| 20 | MICNL | I | - | - | Left Microphone N input |
| 21 | MICPL | I | - | - | Left Microphone P input |
| 22 | MICBIAS | O | - | - | Microphone bias output |
| 23 | VCM | O | - | - | VCM Output |
| 24 | AGND | P | - | - | Analog ground |

***上表中 IO引脚的状态定义如下：***

I 输入

O 输出

IO 双向

P 电源或地

T+D 三态正下拉

T+U 三态正上拉

OUT 上电默认为输出模式

IN 上电默认为输入模式

所有IO支持驱动能力可配，上下拉电阻可配。

**Note1：PA4（PG\_EN）引脚根据上电时电平状态判断是否进行编程，高电平时启动编程功能。**