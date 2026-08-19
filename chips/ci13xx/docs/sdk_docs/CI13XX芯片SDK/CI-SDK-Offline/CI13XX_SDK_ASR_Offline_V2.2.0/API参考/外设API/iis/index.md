<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/iis/ -->

# 音频数字传输总线(IIS)

---

## 1、简介

![IIS系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/IIS%E7%B3%BB%E7%BB%9F%E7%BB%93%E6%9E%84%E5%9B%BE-1.png)

IIS系统结构图

* IIS是用于数字音频设备之间传输音频数据的通信接口，实现对外部16/20/24/32bit立体声数字音频信号编解码电路的信号传输功能。
* CI13XX支持3路IIS：IIS0~IIS2，其中，IIS0为通用IIS，支持TX发送和RX接收功能，IIS1为芯片内部专用IIS，支持TX发射和RX接收功能，TX单元专用于给CI13XX芯片的内部CODEC DAC使用，RX单元专用于给CI13XX芯片的内部CODEC ADC使用；IIS2为专用IIS，仅支持RX接收功能，专用于连接芯片内部PDM模块。
* CI13XX有IIS专用的DMA控制器，用于内存memory与IIS之间的数据传输，所有IIS共用一个DMA控制器IISDMA0，每个IIS使用IISDMA0的不同通道。

## 2、特性

* IIS接口由MCLK、SCK、LRCLK、SDI、SDO这些信号线组成；
* MCLK：主时钟，一般是音频采样率(LRCLK的频率)的128/192/256/384倍；
* SCK：串行位时钟，每个SCK周期传输1bit数据；
* LRCLK：帧时钟，用于切换左右声道的数据；
* IIS格式下LRCLK为0表示当前数据帧是左声道数据，为1表示当前数据帧是右声道数据；
* 左/右对齐格式下LRCLK为0表示当前数据帧是右声道数据，为1表示当前数据帧是左声道数据；
* SDI/SDO：串行数据输入/输出，用于传输音频数据；
* LRCLK与SCK的比例可配置为1:32或1:64；发送通道通过API（iis\_tx\_config）配置，接收通道通过API（iis\_rx\_config）配置；
* 收发数据格式可配置为IIS格式、左对齐格式和右对齐格式；发送通道通过API（iis\_tx\_config）配置，接收通道通过API（iis\_rx\_config）配置；
* 收发数据位数可配置为16bit、20bit、24bit、32bit；发送通道通过API（iis\_tx\_config）配置，接收通道通过API（iis\_rx\_config）配置；
* 使用单声道模式时，支持声道数据拷贝功能，发送时单声道数据同时发送到左右两个声道，接收时左右两个声道合并成单声道的数据；
* 采样数据位宽为16bit时，支持声道数据合并功能，具体功能原理见下图；

![IIS系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iis-5.png)

* 接收和发送数据时左右声道数据支持对调，接收的左右通道数据对调通过API(iis\_rx\_config)配置，发送的左右通道数据对调通过API(iis\_tx\_config)配置；
* 接收和发送数据通道完全独立；
* 支持静音模式，接收时通过API(iis\_rx\_mute)配置，发送时通过API(iis\_tx\_r\_mute、iis\_tx\_l\_mute)配置；
* 支持声道数配置，接收时通过API(iis\_rx\_config)配置单声道或双声道，发送时通过API(iis\_tx\_config)配置单声道或双声道；

## 3、IIS时序

### 3.1、时钟计算

* 假设采样频率是16K（LRCK），MCLK是256倍，量化位数是16bit，SCK/LRCK = 64
* MCLK = 16K \* 256 = 4096 K = 4M
* BCLK = 64 \* 16K = 1M
* 示波器查看引脚时钟验证正确性

### 3.2、时序图

* （1）、IIS格式

![IIS系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iis-2.png)

* （2）、左对齐格式

![IIS系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iis-3.png)

* （3）、右对齐格式

![IIS系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iis-4.png)

## 4、API列表

| 函数名 | 描述 |
| --- | --- |
| iis\_rx\_config | IIS接收模式初始化 |
| iis\_rx\_mute | IIS接收模式，静音 |
| iis\_tx\_config | IIS发送模式初始化 |
| iis\_tx\_r\_mute | IIS发送模式，右声道静音 |
| iis\_tx\_l\_mute | IIS发送模式，左声道静音 |
| iis\_rx\_enable | IIS接收使能 |
| iis\_tx\_enable | IIS发送使能 |

## 5、使用示例

IIS&IISDMA使用示例请查阅☞[《音频系统管理文档》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/Speech-API/%E9%9F%B3%E9%A2%91%E7%B3%BB%E7%BB%9F%E7%AE%A1%E7%90%86/)。

## 6、API 参考