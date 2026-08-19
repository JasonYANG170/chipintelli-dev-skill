<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI-B03GT02S模块数据手册.pdf)

# CI-B03GT02S模块数据手册

## 模块介绍

### 概述

本模块是针对低成本离线语音应用方案开发的一款通用、便携、低功耗高性能的语音识别模块，型号包括为：CI-B03GT02S，CI-B03GT02S主芯片为CI1103，支持本地300条命令词以下的离线语音识别。

![模块框图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-1.png)

图1 模块框图

该模块具有以下特点：

模块体积小巧，长宽为27mm×27mm，工作电压为4.5V-5.5V，板载一颗8MB NorFlash、伴音功放、LDO以及DCDC，外围电路设计简单。模块支持双麦克风输入，外围连接上喇叭和麦克风即可正常工作。模块采用单面贴装，通过插针将主芯片大部分功能IO口引出，预留了3路UART接口，1组IIS接口，1组IIC接口，6路的PWM接口，调试和升级都非常方便。

* 模块主芯片支持离线神经网络计算，支持双麦克风降噪增强，回声消除，360度全方位拾音，可抑制环境噪音，保证嘈杂环境中语音识别的准确性。使用本模块进行离线语音识别不依赖网络，时延小，性能高，可实现97%以上的高识别率，10米超远距离识别，响应时间最快达到0.2S。
* 模块可以应用到有能耗等级要求的产品和电池类产品中，运行功耗≤150mW。
* 高可靠性，模块选材均选用工业级器件，通过了包括双85高温高湿测试在内的多项可靠性实验，可应用在对可靠性要求极高的家电产品中。

模块可以和配套的功能底板使用，更多功能底板的详细信息，请点击：☞[CI-B03GT02S-MB开发板套件说明](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI-B03GT02S-MB%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/)

### 模块主芯片介绍

CI1103芯片是一颗专用于语音处理的人工智能芯片，可广泛应用于家电、家居、照明、音箱、玩具、穿戴设备、汽车等产品领域，实现语音交互及控制。CI1103芯片内置自主研发的脑神经网络处理器BNPU，支持本地大词汇量语音识别和声纹识别，和内置的CPU核结合可以做各类智能语音方案应用。

了解CI1103芯片更多的详细信息，请点击以下链接:

☞[CI1103芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1103%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/)

### 模块应用场景

该模块可用作语音识别前端+客户硬件主控板方案组合使用，也可以作为灯具、玩具等方案的单芯片主控模块。应用时在模块底板上添加麦克风和喇叭插座或者焊点，并通过外部接入的5V电源进行供电。

![模块作为语音识别前端应用示意图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-2.png)

图2 模块作为语音识别前端应用示意图

CI-B03GT02S模块支持300条以内的离线语音识别命令词，可以用来做空调、洗衣机、中控等要求较多命令词的产品，还可以通过CI1103的计算能力，支持本地命令词自学习等扩展功能，增强产品的体验感。

![模块可应用的产品](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-3.png)

图3 模块可应用的产品

## 模块规格

### 模块实物图

![模块实物和芯片对应位置图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-4-1.png)

![模块实物和芯片对应位置图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-4-2.png)

图4 模块实物和芯片对应位置图

模块实物如图4所示，语音识别模块为单面贴装，主要IC为CI1103、8MB的NorFlash、伴音功放等。语音指令从麦克风输入，经语音识别IC进行语音识别、指令处理后，将反馈播报音发送至伴音功放，驱动喇叭播放声音。伴音功放最大驱动功率为1.5W@8Ω。模块内部通过一路Codec实现了回声消除，正常运行下回声有效抑制的信噪比最大范围为-10dB到-15dB。

### 模块尺寸图

![模块尺寸图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-5-1.png)

![模块尺寸图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-5-2.png)

图5 模块尺寸图

如图5所示，模块形状为正方形，尺寸为27mm\*27mm，PCB板厚为1.0±0.1mm，模块高度为3.1±0.1mm，用户可根据此尺寸设计结构。

### 模块硬件接口定义

![模块引脚图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-6.png)

图6 模块引脚图

CI-B03GT02S的引脚位置如上图所示。

1. 四线双麦克风接口，请在底板设计麦克风插座或者焊点，考虑麦克风线路上增加ESD器件，为保障好的语音识别效果，建议采用灵敏度为-32±3dB，信噪比≥65dB的麦克风，请点击 ☞[参考麦克风器件](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%A4%96%E5%9B%B4%E5%99%A8%E4%BB%B6%E5%85%BC%E5%AE%B9%E5%88%97%E8%A1%A8/#micphone_list) 了解更多信息；
2. 双线单喇叭接口，请在底板设计喇叭插座或者焊点，为保障好的语音播报效果，建议采用带腔体的喇叭，请点击 ☞[参考喇叭器件](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%A4%96%E5%9B%B4%E5%99%A8%E4%BB%B6%E5%85%BC%E5%AE%B9%E5%88%97%E8%A1%A8/#speaker_list) 了解更多信息；
3. UART0接口可用于模块固件升级，底板上请设计对应的插针方便后续升级；UART1接口与主控MCU通讯，PWM信号主要应用于灯控和红外控制信号的收发，引脚顺序请参考图5和图6。本模块的所有UART接口均可配置为GPIO口。
   模块全部对外引脚的功能描述如表1所示：

表1 模块引脚对照表

| 管脚号 | 管脚名称 | I/O类型 | IO驱动能力 | IO上电默认状态 | 功能定义 |
| --- | --- | --- | --- | --- | --- |
| 1 | UART0\_TX | IO | 4mA | IN，T+U | 1.GPIO[0] 2.UART0\_TX:Transmit channel of UART0 |
| 2 | UART0\_RX | IO | 4mA | IN，T+U | 1.GPIO[1] 2.UART0\_RX:Receive channel of UART0 |
| 3 | 3.3V | P | - | - | 3.3V电源输出 |
| 4 | GND | P | - | - | 地信号 |
| 5 | I2C0\_SDA | IO | 4mA | IN，T+U | 1.GPIO[2] 2.I2C0\_SDA:I2C0 Serial Data 3.SDC0\_D1:SDC0 interface data 1 4.SPI2\_CS:SPI2 interface chip selection signal |
| 6 | I2C0\_SCL | IO | 4mA | IN，T+U | 1.GPIO[3] 2.I2C0\_SCL:I2C0 Serial Clock 3.SDC0\_D0:SDC0 interface data 0 4.SPI2\_D1:SPI2 interface data 1 |
| 7 | PWM0 | IO | 4mA | IN，T+D | 1.GPIO[15] 2.PWM0 Output 3.SDC\_CARD\_DETECT\_N |
| 8 | PWM1 | IO | 4mA | IN，T+D | 1.GPIO[16] 2.PWM1 Output |
| 9 | PWM2 | IO | 4mA | IN，T+D | 1.GPIO[17] 2.PWM2 Output |
| 10 | PWM3 | IO | 4mA | IN，T+D | 1.GPIO[18] 2.PWM3 Output 3.IPM\_CS |
| 11 | PWM4 | IO | 4mA | IN，T+D | 1.GPIO[19] 2.PWM4 Output 3.I2C1\_SDA:I2C1 Serial Data |
| 12 | PWM5 | IO | 4mA | IN，T+D | 1.GPIO[20] 2.PWM5 Output 3.I2C1\_SCL:I2C1 Serial Clock |
| 13 | JTAG\_TCK | IO | 4mA | IN，T+D | 1.TCK:Serial Wire Debug port clock pin，Has intemal weak pull-down 2.GPIO[21] |
| 14 | JTAG\_TMS | IO | 4mA | IN，T+U | 1.TMS:Serial Wire Debug port data pin，Has intemal weak pull-up 2.GPIO[22] |
| 15 | UART1\_TX | IO | 4mA | IN，T+U | 1.GPIO[23]， 2.UART1\_TX:Transmit channel of UART1 3.I2C1\_SDA:I2C1 Serial Data |
| 16 | UART1\_RX | IO | 4mA | IN，T+U | 1.GPIO[24] 2.UART1\_RX:Receive channel of UART1 3.I2C1\_SCL:I2C1 Serial Clock |
| 17 | UART2\_TX | IO | 4mA | IN，T+U | 1.GPIO[25]， 2.UART2\_TX:Transmit channel of UART2 3.I2C1\_SDA:I2C1 Serial Data |
| 18 | UART2\_RX | IO | 4mA | IN，T+U | 1.GPIO[26] 2.UART1\_RX:Receive channel of UART2 3.I2C1\_SCL:I2C1 Serial Clock 4.EXT\_INT[1] |
| 19 | GND | P | - | - | 地信号 |
| 20 | I2S1\_SDI | IO | 4mA | OUT，T+D | 1.GPIO[27] 2.I2S1\_SDI:Serial Data Input for I2S1 interface 3.SPII\_CS:SPI1 interface chip selection signal |
| 21 | I2S1\_LRCLK | IO | 4mA | IN，T+D | 1.GPIO[28] 2.I2S1\_LRCLK:I2S1 LRCLK clock 3.SPI1\_DIN:SPI1 interface data input |
| 22 | I2S1\_SDO | IO | 4mA | IN，T+D | 1.GPIO[29] (BOOT\_SEL[1]) At startup，Boot1 option bit is used to select one of two modes: 0—Normal functional model 1—Debug mode 2.I2S1\_SDO:Serial Data Output for I2S1 interface |
| 23 | I2S1\_SCLK | IO | 4mA | IN，T+D | 1.GPIO[30] 2.I2S1\_SCLK:Serial Clock for I2S1 interface 3.SPI1\_DOUT:Transmit channel of SPI1 |
| 24 | I2S1\_MCLK (PG\_EN) | IO | 4mA | IN，T+D | 1.GPIO[31] (UART\_UPDATE\_EN) At startup，this pin is used to select one of two functional modes: 1—Start serial port upgrade service and program 0—Start directly from Flash 2.I2S1\_MCLK:Master Clock for I2S1 reference 3.SPI1\_CLK:Serial Clock for SPI1 interface |
| 25 | MICL+ | - | - | - | 麦克风输入 |
| 26 | AGND | P | - | - | 模拟地信号 |
| 27 | MICL+ | - | - | - | 麦克风输入 |
| 28 | AGND | P | - | - | 模拟地信号 |
| 29 | HP\_L | O | - | O | Left DAC channel output |
| 30 | GND | P | - | - | 地信号 |
| 31 | SKP- | - | - | - | 喇叭输出 |
| 32 | SKP+ | - | - | - | 喇叭输出 |
| 33 | GND | P | - | - | 地信号 |
| 34 | 5V | P | - | - | 5V电源输入 |
| 35 | ADC1 | IO | - | IN，T+D | 1.GPIO[36] 2.ADC1 Input 3.PWM1 4.I2C0\_SDA |
| 35 | ADC0 | IO | - | IN，T+D | 1.GPIO[37] 2.ADC0 Input 3.PWM0 4.I2C0\_SDA 4.I2C0\_SCL |

上表中的标注符号释义如下：

I：input

O：output

IO：bidirectional

P：power or ground

T+D：tristate plus pull-down

T+U：tristate plus pull-up

OUT：power-on defaults to output mode

IN：power-on defaults to input mode

### 模块电气特性参数

表2 模块引脚对照表

| 参数 | 条件 | 最小值 | 典型值 | 最大值 | 单位 | 备注 |
| --- | --- | --- | --- | --- | --- | --- |
| 模块供电电压 | / | 4.5 | 5 | 5.5 | V | NOTE1 |
| 模块播音状态电流（正常音量） | 4欧3W喇叭 | / | 70 | / | mA | NOTE2 |
| 模块工作电流 | / | / | 40 | / | mA | NOTE3 |
| 安静环境下监听状态电流 | 5V供电 | / | 32 | / | mA | / |
| 芯片IO接口电压 | / | 3 | 3.3 | 3.6 | V | / |
| 模块UART接口电压 | / | 3 | 3.3 | 3.6 | V | / |

***NOTE1：5V为模块典型供电电压，输入超过5.5V电压会损坏模块。***

***NOTE2：模块播音状态下最大电流能达到250mA，按照两倍余量原则上需要为模块提供一组驱动能力为500mA的电源供电。***

***NOTE3：典型值测试时为静音状态。最大值测试时为识别并播音状态。***

### 模块温湿度参数

CI-B03GT02S模块的温湿度参数如表3所示。

表3 温湿度参数表

| 参数 | 最小值 | 典型值 | 最大值 | 单位 | 备注 |
| --- | --- | --- | --- | --- | --- |
| 模块工作环境温度 | -20 | 25 | 85 | °C | / |
| 模块存储环境温度 | -20 | 25 | 100 | °C | / |
| 模块存储湿度 | 0% | / | 5% | RH | / |
| 焊接温度 | / | 220 | 245 | 5°C | / |

## 模块应用

### 模块底板设计参考

使用该模块需要设计调试底板或上位机主板，调试底板的主要作用是用于承载本模块、给模块板提供电源、放置麦克风和喇叭插座、与主控通讯电路以及其它功能电路的转接。 模块供电输入端需放置一个100uF的电容和5V稳压管，喇叭和麦克风插座附近添加ESD器件，PG\_EN管脚添加一个跳线和4.7K的上拉电阻，方便通过跳线进入升级模式。 底板上需要设计UART0外接的排针，以方便模块升级。参考设计如下图所示。

![模块底板参考设计](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-7-1.png)

![模块底板参考设计](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-7-2.png)

图7 模块底板参考设计

### 模块上电及启动

使用该模块时，将模块安装于底板或上位机主板上，接上喇叭和麦克风，模块的5V电源上电后模块即可启动，上电后若无异常喇叭会播报开机提示音，此时UART口会有打印信息输出，用户可将该UART口用USB串口调试助手等工具接到电脑上，在电脑的串口调试窗口查看打印信息，出现如图8所示的打印信息则表明模块启动正常。注意模块的UART接口为3.3V电平高速串口，若与5V电平的系统对接需要进行电平转换。

输入模块的5V电源会经过模块上的LDO和DCDC芯片降压为3.3V和1.2V，供主芯片使用。模块上的功放芯片采用5V电源供电。5V供电需保证500mA的额定供电电流，要求电源稳定，纹波在300mV以内。

![模块启动后的一个参考打印信息](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-8.png)

图8 模块启动后的一个参考打印信息

### 模块默认命令词

如果为用户量产模块，一般出厂前会烧录用户指定的命令词条固件。如果客户未指定，模块会自带默认固件，该固件带默认命令词，供用户测试使用，其命令词如下图所示：

![模块默认命令词](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-9.png)

图9 模块默认命令词

### 模块默认串口通讯协议

本模块默认的固件中支持串口通讯协议，用于和上位机的通讯使用。该串口协议可扩展，有以下特点:

* 完整传输包，包含：头尾，长度，校验，消息类型，消息序列号。
* 支持变长命令，方便扩展。
* 消息类型(命令，通知，回复)。
* 命令消息，可配置，回复ACK。通知消息无ACK。
* 消息格式将与bootloader升级的相同，通过header来与bootloader协议区分。
* 默认波特率使用9600。
* 注意：模块预留的UART0接口，UART0接口默认为打印输出接口。如需UART0作为上述串口协议接口，必须修改代码，修改方式可参照☞[CI110X系列芯片SDK](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/CI110X%E8%8A%AF%E7%89%87SDK/) 的串口协议部分文档实现。
* 支持的命令：查询协议版本号，查询系统版本号，设置音量（音量分级在user\_config.h中定义），播放本地播报音，复位命令等，具体协议格式如下图所示：

![串口协议格式](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-10.png)

图10 串口协议格式

**举例说明1：**

A5 FC 07 00 A0 91 18 01 55 E0 01 00 00 1B 9B 02 FB解析如下，

A5 FC：head

07 00:有效数据为7byte

A0 ：这是命令词信息

91 ：命令号码为0x91（本次数据内容为命令词数据）

18 ：包序号，本串口第0x08次外发数据，该数值不断累加

01 55 E0 01 00 00：当前命令词的唯一数据

1B ：命令词阈值

9B 02：累加和

FB：结尾数据

备注：如果应用中仅关注命令词和阈值，则只关注蓝色部分的7个有效数据就可以了。

**举例说明2：**

A5 FC 02 00 A3 9A 17 00 B1 05 02 FB 解析如下：

A5 FC ：head

02 00：有效数据2byte

A3 ：当前为通知数据

9A ：命令号码为0X9A（本次数据内容为语音模块内容改变）

17 ：本串口第0x07次外发数据，该数值不断累加

00 B1：有效数据。（本数据表示进入唤醒状态）

05 02：累加和

FB：结尾数据

备注：该数据为通知数据，用户根据情况选择使用该信息。

更多的内容解析数据可以可参照☞[CI110X系列芯片SDK](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/CI110X%E8%8A%AF%E7%89%87SDK/) 中的串口协议部分。下图为一个协议数据参考截图：

![串口协议数据参考截图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-11.png)

图11 串口协议数据参考截图

### 软件开发

如果模块自带的默认固件无法满足用户需求，用户可以自行进行软件开发，修改该模块的命令词、播报音、串口通讯协议以及其他功能。

软件开发流程主要包含以下几个步骤：

* SDK开发包资料下载
* 模型制作（语言模型+声学模型）
* 语音合成
* 命令词信息表与音频文件关联
* 固件打包

详细开发流程请点击☞[CI110X系列芯片SDK](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/CI110X%E8%8A%AF%E7%89%87SDK/) 了解。

### 固件烧录

#### 烧录前准备工作

用户要烧录模块前，需要准备以下物品：

* 待烧录的模块
* USB转串口工具
* 固件烧录工具（pack\_update\_tool.exe）
* 固件信息（\*.bin格式的文件）
* 1.25mm间距-4P的麦克风
* 2.0mm间距喇叭
* 杜邦线若干
* 跳线帽

![固件烧录需准备的物品](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-12.png)

图12 固件烧录需准备的物品

#### 硬件连接并烧录

以上图的USB转串口工具为例，在烧录前需要先将USB转串口的电源、地、串口收发引脚分别和模块对应的引脚连接起来，（注意USB转串口的RXD和TXD分别对应模块的UART0\_TX和UART0\_RX），这里以配套的底板作相应说明，连接方式如下图所示，接着，短接PG\_EN和旁边的引脚3V3（模块以及底板附有丝印，可根据丝印提示查找相关的位置）。模块烧录的接线图如下图所示。

![模块和USB转串口工具连接图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-13.png)

图13 模块和USB转串口工具连接图

打开固件烧录工具（该工具可以在SDK开发包中CI110X\_SDK\tools目录找到PACK\_UPDATE\_TOOL.exe），根据芯片选择对应的型号，点击固件升级按钮，选择制作好的固件文件，并找到对应的电脑分配给USB转串口工具的串口端口号。准备工作就绪后，通过瞬间上电时短接PG\_EN和旁边的引脚（拉高PG\_EN管脚的电平）进入编程模式，此时可下载固件。如果遇到USB转串口工具在电脑上无法识别，请在电脑装上相应的驱动。

![固件烧录工具示意图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-14.png)

图14 固件烧录工具示意图

#### 烧录后功能测试

固件烧录结束后，建议对模块进行功能测试，以验证烧录固件是否成功。功能测试前待测模块需首先连接麦克风以、喇叭，通电确认是否能有上电播报音，并用唤醒词和命令词测试是否能正常唤醒和识别，如果均能正常工作，则模块功能正常，烧录成功；否则，烧录失败，需进一步探其原因。

### 使用中可能出现的问题和解决方法

本章节列举了一些模块使用中可能遇到的问题和对应的解决方法。

* 模块不能烧录并更新固件。
  出现上述问题后，请检查以下操作点：
  1. 模块上电前是否已拉高PG\_EN管脚；
  2. 串口管脚是否接对，TX和RX是否有接反，电脑端USB转串口工具驱动是否正常，PC端烧录工具是否选择了正确的串口号；
  3. 如以上两点检查无误后，模块还不能烧录，需要使用万用表测量模块供电电压5V、3.3V、1.2V是否正确，用示波器测量晶体是否起振，频率是否为12.288Mhz。各个硬件测量点参考下图。如果发现有电压或晶体出问题，考虑为模块硬件故障，请更换模块或针对模块硬件进行维修。如上述检查均无问题，请联系我司技术支持人员获取帮助。

![模块测试点分布图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-15.png)

图15 模块测试点分布图

* 模块烧录完成，上电后无播报。
  出现上述问题后，请检查以下操作点：
  1. 确认烧录固件是否与板子匹配；
  2. 确认喇叭正确接好，供电正常；
     采用示波器测量主芯片的语音输出测试点。无输出需检查请固件是否正确，有输出需检查模块上的功放期间是否焊接出现异常，如果功放有异常可更换后再测试。测量点见下图。如上述检查均无问题，请联系我司技术支持人员获取帮助。

![模块语音输出测量点](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-16.png)

图16 模块语音输出测量点

* 模块烧录完成，上电后有播报但是不识别命令词：
  1. 检查麦克风和插口的连接是否完好；
  2. 检查麦克风正负极方向是否与模块板上标示一致，没有插反；
  3. 使用万用表测量主芯片对应的MICBIAS管脚是否为2.8V左右电压，使用示波器测量麦克风输入管脚是否有输入语音波形（示波器每格电压调整为100mv档位），若信号正常需考虑固件是否正确，若信号异常需观察板子硬件是否有物理损伤。测量点见下图。如上述检查均无问题，请联系我司技术支持人员获取帮助。

![模块麦克风信号测试点](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-17.png)

图17 模块麦克风信号测试点

### 其它应用注意事项

* 因为CI110X系列芯片ESD等级较高并且模块设计为方便用户扩展，所以模块没有设计ESD器件，对与ESD很高要求的产品可以在底板添加ESD器件，添加位置为麦克风、喇叭、电源插座位置。建议用户在检验、焊接生产过程中佩戴防静电手环或防静电手套、指套，保障产品的质量可靠性。
* 使用时注意麦克风、喇叭、电源串口不能接错，注意防止背面测试点短路。
* 用户可采用USB转串口工具对开发的软件进行调试，调试时需在SDK软件中相应位置加上串口打印命令，编译后生成固件并烧录，就可以进行调试验证。
* 此模块板的所有IO均为3.3V电平，如果需要接5V或2.5V逻辑电平，必须加转接电路。
* BOOT\_SEL为启动模式选择，FLASH\_PG\_EN为升级选择，调试使用时可焊接排针，使用跳线帽分别与中间排针短接使用。
* 模块底板或上位机主板设计时，模块5V电源输入处需放置容值不小于100uF的电容和5V的稳压管，麦克风走线尽量短；麦克风走线要注意屏蔽，SPK走线尽量短而粗，走线区域不得有其它走线跨跃。
* 控制底板翘曲程度不大于0.5%，防止模块焊接不良。

## 生产指南、存储和包装订购信息

### 生产存储指南

1. 启英泰伦邮票口封装模块必须采用SMT贴片机器贴片（采用插针方式除外），并且拆开包装后须于24小时内完成贴片，否则要重新抽真空包装。
2. 启英泰伦邮票口封装模块存储条件如下：

* 包装袋储存在 < 40 °C/90%RH 的非冷凝大气环境中。
* 真空防潮袋内置有湿度指示卡如下图：

![湿度指示卡](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-18.png)

图18 湿度指示卡

3. 启英泰伦出厂的模块需要烘烤，湿度指示卡及烘烤的几种情况如下所述：

* 拆封时如果湿度指示卡读值30%、40%、50%色环均为蓝色，需要对模块进行持续烘烤2小时
* 拆封时如果湿度指示卡读取到30%色环变为粉色，需要对模块进行持续烘烤4小时
* 拆封时如果湿度指示卡读取到30%、40%色环变为粉色，需要对模块进行持续烘烤6小时
* 拆封时如果湿度指示卡读取到30%、40%、50%色环变为粉色，需要对模块进行持续烘烤12小时

4. 烘烤参数如下：

* 烘烤温度：125±5℃
* 报警温度设定：130℃
* 自然条件下冷却＜36℃后，即可进行SMT贴片
* 干燥次数：1次
* 若烘烤后超过12小时没有焊接，请再次进行烘烤

5. 如果拆封时间超过3个月，禁止使用SMT工艺焊接此批次模块，因为此PCB为沉金工艺，超过3个月后焊盘氧化严重，SMT贴片时极有可能导致虚焊、漏焊。
6. SMT贴片前，请对模块进行ESD（静电放电、静电释放）保护。操作过程请佩戴静电手套和静电手环。
7. 为了确保回流焊合格率，贴片请全部产品进行目测、AOI 检测，以确保炉温控制、器件吸附方式、摆放方式的正确性。

### 推荐炉温曲线

![推荐炉温曲线](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI110X%E7%B3%BB%E5%88%97/img/CI-B03GT02S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-19.png)

图19 推荐炉温曲线

### 包装订购信息

表4 模块批量订购及包装信息

| 产品型号 | 包装方式 | 每个托盘装模块数量 | 每包装模块数量 | 每箱装模块数量 |
| --- | --- | --- | --- | --- |
| CI-B03GT02S | 托盘+静电袋+纸箱 | 50pcs | 10个托盘共500pcs | 2000pcs |

## 采购和技术支持

用户如果要采购我司产品样品，请点击 ☞[样品购买](http://mall.chipintelli.com/) ，也可以点击 ☞[样品和批量采购](https://document.chipintelli.com/%E6%A0%B7%E5%93%81%E5%92%8C%E6%89%B9%E9%87%8F%E9%87%87%E8%B4%AD/) 获取更多信息。

如果您想获取技术支持，请登录☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com) 。