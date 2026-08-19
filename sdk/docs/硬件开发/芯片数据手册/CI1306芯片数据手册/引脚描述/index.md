<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1306_datasheet V1.5_chs_20251205.pdf)

# 引脚描述

CI1306芯片引脚图如图P-1所示：

![CI1306芯片引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI1306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-2.png)

图P-1 CI1306芯片引脚图

芯片各个引脚功能如下表描述：

表P-1 芯片引脚功能描述

| 管脚号 | 管脚名称 | 类型 | IO 5V-耐压 | IO上电默 认状态 | 管脚复用和功能描述 |
| --- | --- | --- | --- | --- | --- |
| 1 | VDD11 | P | - | - | 1.1V LDO 输出管脚，同时也是内核供电 输入管脚，外接 4.7uF电容 |
| 2 | XIN | I | - | - | 1. 外部晶振管脚 XIN（上电默认状态，正 常应用无需外接晶振）  2. GPIO PA0  3. PWM5 |
| 3 | XOUT | O | - | - | 1. 外部晶振管脚 XOUT（上电默认状态， 正常应用无需外接晶振）  2.GPIO PA1 |
| 4 | PD0 | IO | √ | IN,T+D | GPIO PD0 |
| 5 | PD1 | IO | √ | IN,T+D | GPIO PD1 |
| 6 | PA2 | IO | √ | IN,T+D | 1. GPIO PA2（上电默认状态） 2. IIS\_SDI 3. IIC\_SDA 4. UART1\_TX 5. PWM0 |
| 7 | PA3 | IO | √ | IN,T+D | 1. GPIO PA3（上电默认状态） 2. IIS\_LRCLK 3. IIC\_SCL 4. UART1\_RX1 5. PWM1 |
| 8 | PA4 | IO | √ | IN,T+U | 1. GPIO PA4（上电默认状态）/PG\_EN（根据上电时电平状态判断是否进行编程，高电平时启动编程功能） 2. IIS\_SDO 3. PWM2 |
| 9 | PA5 | IO | √ | IN,T+D | 1. GPIO PA5（上电默认状态） 2. IIS\_SCLK 3. PDM\_DAT 4. UART2\_TX 5. PWM3 |
| 10 | PA6 | IO | √ | IN,T+D | 1. GPIO PA6（上电默认状态） 2. IIS\_MCLK 3. PDM\_CLK 4. UART2\_RX 5. PWM4 |
| 11 | PA7 | IO | √ | IN,T+D | 1. GPIO PA7（上电默认状态） 2. PWM0 3. UART1\_TX 4. EXT\_INT[0] |
| 12 | PB0 | IO | √ | IN,T+D | 1. GPIO PB0（上电默认状态） 2. PWM1 3. UART1\_RX 4. EXT\_INT[1] |
| 13 | PB1 | IO | √ | IN,T+D | 1. GPIO PB1（上电默认状态） 2. PWM2 3. UART2\_TX |
| 14 | PB2 | IO | √ | IN,T+D | 1. GPIO PB2（上电默认状态） 2. PWM3 3. UART2\_RX |
| 15 | PB3 | IO | √ | IN,T+D | 1. GPIO PB3（上电默认状态） 2. PWM4 3. IIC\_SDA |
| 16 | PB4 | IO | √ | IN,T+D | 1. GPIO PB4（上电默认状态） 2. PWM5 3. IIC\_SCL |
| 17 | PB5 | IO | √ | IN,T+U | 1. GPIO PB5（上电默认状态） 2. UART0\_TX 3. IIC\_SDA 4. PWM1 |
| 18 | PB6 | IO | √ | IN,T+U | 1. GPIO PB6（上电默认状态） 2. UART0\_RX 3. IIC\_SCL 4. PWM2 |
| 19 | PB7 | IO | √ | IN,T+U | 1. GPIO PB7（上电默认状态） 2. UART1\_TX 3. IIC\_SDA 4. PWM3 |
| 20 | PC0 | IO | √ | IN,T+U | 1. GPIO PC0（上电默认状态） 2. UART1\_RX 3. IIC\_SCL 4. PWM4 |
| 21 | PD3 | IO | - | IN,T+D | GPIO PD3 |
| 22 | PD4 | IO | - | IN,T+D | GPIO PD4 |
| 23 | TEST\_EN | I | - | - | 测试引脚 Note1 |
| 24 | RSTn | I | - | - | 复位引脚 Note2 |
| 25 | AIN5 | IO | - | IN,T+D | 1. 保留（上电默认状态）  2. GPIO PC1  3. UART2\_TX  4. PWM3  5. PDM\_DAT  6. SAR ADC input channel 5 |
| 26 | AIN4 | IO | - | IN,T+U | 1. 保留（上电默认状态）  2. GPIO PC2  3. UART2\_RX  4. PWM2  5. PDM\_CLK  6. SAR ADC input channel 4 |
| 27 | AIN3 | IO | - | IN,T+D | 1. 保留（上电默认状态）  2. GPIO PC3  3. IIC\_SDA  4. PWM1  5. PDM\_DAT  6. SAR ADC input channel 3 |
| 28 | AIN2 | IO | - | IN,T+U | 1. 保留（上电默认状态） 2. GPIO PC4  3. IIC\_SCL  4. PWM0  5. PDM\_CLK  6. SAR ADC input channel 2 |
| 29 | PC5 | IO | √ | IN,T+D | GPIO PC5/BOOT\_SEL （根据上电时电平状态判断系统启动的入口设备，低电平时从 Flash 启动，高电平时从 SRAM 启动） |
| 30 | MICPL | I | - | - | Left Microphone P input |
| 31 | MICNL | I | - | - | Left Microphone N input |
| 32 | MICBIAS | O | - | - | Microphone bias output |
| 33 | MICNR | I | - | - | Right Microphone N input |
| 34 | MICPR | I | - | - | Right Microphone P input |
| 35 | VCM | O | - | - | VCM Output |
| 36 | AGND | P | - | - | Analog ground |
| 37 | HPOUT | O | - | - | DAC output |
| 38 | AVDD | P | - | - | 3.3V 模拟 LDO 输出管脚，同时也是模拟 供电输入管脚，外接 4.7uF 电容 |
| 39 | VIN5V | P | - | - | VIN5V 是 PMU 电源输入引脚。正常工作 输入电压范围为 3.6V-5.5V。外部连接一 个 4.7uf 输入电容器。该引脚的最大输入 电压为 6.5V。请注意该引脚需要添加过压 和浪涌保护装置，例如 TVS 和 4.7 欧姆电 阻，以防止浪涌冲击 |
| 40 | VDD33 | P | - | - | F3.3V LDO 输出管脚，外接 4.7uF 电容 |
| 41 | GND | P | - | - | 芯片底部焊盘接地 Note3 |

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

**Note2：TEST\_EN是使能测试功能引脚，内部有下拉，当上电时检测该引脚输入电平，为低电平时芯片正常启动，为高电平时进入测试模式。**

**Note3：RSTn是外部复位输入引脚，拉低时间大于100uS时使能芯片复位。**

**Note4：BOOT\_SEL引脚根据上电时电平状态判断系统启动的入口设备，低电平时从Flash启动，高电平时从SRAM启动。**

**Note5：QFN40封装底部有散热焊盘，使用时需要接到地。**