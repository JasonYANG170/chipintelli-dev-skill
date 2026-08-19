<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI230X_datasheet V0.3_chs_20220902.pdf)

# 引脚描述

CI230X系列芯片引脚图如图P-1所示：

![CI230X系列芯片引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI230X%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-2.png)

图P-1 CI230X系列芯片引脚图

芯片各个引脚功能如下表描述：

表P-1 芯片引脚功能描述

| Pin Number | Pin name | Pin type | IO 5V-Tolerant | IO power-on default state | Description and alternate functions |
| --- | --- | --- | --- | --- | --- |
| 1 | RFIO | IO | - | - | WIFI RF transmitter/receiver |
| 2 | VDDA33 | P | - | - | 3.3V电源输入，外接4.7uf和0.1uf电容 |
| 3 | VDDA33 | P | - | - | 3.3V电源输入，外接4.7uf和0.1uf电容 |
| 4 | XOUT | O | - | - | 40MHz外部晶振管脚 XOUT |
| 5 | XIN | I | - | - | 40MHz外部晶振管脚 XIN |
| 6 | RSTn | I | - | - | 复位引脚 |
| 7 | EN | I | - | - | 使能引脚 |
| 8 | PE0 | IO | - | IN,PU | GPIO PE0 Note1 |
| 9 | PE1 | IO | - | IN,PU | GPIO PE1 Note1 |
| 10 | PE2 | IO | - | IN,PU | 1. GPIO PE2  2.WIFI firmware download TX Note1 |
| 11 | PE3 | IO | - | IN,PU | 1. GPIO PE3 2.WIFI firmware download RX Note1 |
| 12 | PE4 | IO | - | IN,PD | GPIO PE4 Note1 |
| 13 | PE6 | IO | - | IN,PD | GPIO PE6 |
| 14 | PE7 | IO | - | IN,PD | GPIO PE7 |
| 15 | PE9 | IO | - | IN,PU | 1. GPIO PE9 2. WIFI BOOTMODE1 Note1/Note2 |
| 16 | VDD33W | P | - | - | IO电源输入脚，推荐3.3V，外接 0.1uf 电容 |
| 17 | VDD11W | P | - | - | 1.1V LDO 输出管脚，外接 0.1uf 电容 |
| 18 | AIN3 | IO | - | IN,T+D | 1. 保留（上电默认状态） 2. GPIO PC3 3. IIC\_SDA 4. PWM1 5. PDM\_DAT 6. SAR ADC input channel 3 |
| 19 | AIN2 | IO | - | IN,T+U | 1. 保留（上电默认状态） 2. GPIO PC4（上电默认状态） 3. IIC\_SCL 4. PWM0 5. PDM\_CLK 6. SAR ADC input channel 2 |
| 20 | MICPL | I | - | - | Left Microphone P input |
| 21 | MICNL | I | - | - | Left Microphone N input |
| 22 | MICBIAS | O | - | - | Microphone bias output |
| 23 | MICNR | I | - | - | Right Microphone N input |
| 24 | MICPR | I | - | - | Right Microphone P input |
| 25 | VCM | O | - | - | VCM Output |
| 26 | AGND | P | - | - | Analog ground |
| 27 | HPOUTL | O | - | - | DAC output |
| 28 | AVDD | P | - | - | 3.3V 模拟 LDO 输出管脚，同时也是模拟 供电输入管脚，外接 4.7uf 电容 |
| 29 | VIN5V | P | - | - | 5V 电源输入，外接 4.7uf 电容 |
| 30 | VDD33 | P | - | - | 3.3V LDO 输出管脚，外接 4.7uf 电容 |
| 31 | VDD11 | P | - | - | 1.1V LDO 输出管脚，同时也是内核供电 输入管脚，外接 4.7uf 电容 |
| 32 | PA0 | IO | - | - | GPIO PA0 |
| 33 | PA1 | IO | - | - | GPIO PA1 |
| 34 | PD0 | IO | √ | IN,T+D | GPIO PD0 |
| 35 | PA2 | IO | √ | IN,T+D | 1. GPIO PA2（上电默认状态） 2. IIS\_SDI 3. IIC\_SDA 4. PWM0 |
| 36 | PA3 | IO | √ | IN,T+D | 1. GPIO PA3（上电默认状态） 2. IIS\_LRCLK 3. IIC\_SCL 4. PWM1 |
| 37 | PA4 | IO | √ | IN,T+U | 1. GPIO PA4（上电默认状态）/PG\_EN（根据上电时电平状态判断是否进行编程，高电平时启动编程功能）  2. IIS\_SDO  3. PWM2 |
| 38 | PA5 | IO | √ | IN,T+D | 1. GPIO PA5（上电默认状态）  2. IIS\_SCLK  3. PDM\_DAT  4. UART2\_TX  5. PWM3 |
| 39 | PA6 | IO | √ | IN,T+D | 1. GPIO PA6（上电默认状态）  2. IIS\_MCLK  3. PDM\_CLK  4. UART2\_RX  5. PWM4 |
| 40 | PA7 | IO | √ | IN,T+D | 1. GPIO PA7（上电默认状态）  2. PWM0 3. EXT\_INT[0] |
| 41 | PB0 | IO | √ | IN,T+D | 1. GPIO PB0（上电默认状态）  2. PWM1 3. EXT\_INT[1] |
| 42 | PB1 | IO | √ | IN,T+D | 1. GPIO PB1（上电默认状态）  2. PWM2  3. UART2\_TX |
| 43 | PB2 | IO | √ | IN,T+D | 1. GPIO PB2（上电默认状态）  2. PWM3  3. UART2\_RX |
| 44 | PB3 | IO | √ | IN,T+D | 1. GPIO PB3（上电默认状态）  2. PWM4  3. IIC\_SDA |
| 45 | PB4 | IO | √ | IN,T+D | 1. GPIO PB4（上电默认状态）  2. PWM5  3. IIC\_SCL |
| 46 | PB5 | IO | √ | IN,T+U | 1. GPIO PB5（上电默认状态）  2. UART0\_TX  3. IIC\_SDA  4. PWM1 |
| 47 | PB6 | IO | √ | IN,T+U | 1. GPIO PB6（上电默认状态）  2. UART0\_RX  3. IIC\_SCL  4. PWM2 |
| 48 | PF3 | IO | - | IN,PD | GPIO PF3 Note1 |
| 49 | PF4 | IO | - | IN,PU | GPIO PF4 Note1 |
| 50 | PF5 | IO | - | IN,PU | GPIO PF5 Note1 |
| 51 | PF6 | IO | - | IN,PU | GPIO PF6 Note1 |
| 52 | PF7 | IO | - | IN,PU | GPIO PF7 Note1 |
| 53 | PF8 | IO | - | IN,PU | GPIO PF8 Note1 |
| 54 | PF9 | IO | - | IN,PU | GPIO PF9 Note1 |
| 55 | VDDA33 | P | - | - | 3.3V电源输入，外接4.7uf和0.1uf电容 |
| 56 | VDDA33 | P | - | - |  |
| 57 | GND | P | - | - | GND |

***上表中 IO引脚的状态定义如下：***

I 输入

O 输出

IO 双向

P 电源和地

T+D 三态下拉

T+U 三态上拉

OUT 上电默认输出

IN 上电默认输入

**Note1：PE/PF口仅做WIFI下载调试接口，不建议用于其他用途。**

**Note2：芯片上电时管脚PE9（WIFI BOOTMODE1）的电平会作为WIFI升级模式判断信号，当上电判断为高时，WIFI部分直接从内部的Nor Flash启动。上电判断为低时，启动串口升级服务，这时可使用配套的升级工具对芯片内部WIFI部分Nor Flash进行编程。**

**Note3：芯片第57脚为芯片底面的散热焊盘，也是封装的接地焊盘，需要接地。**