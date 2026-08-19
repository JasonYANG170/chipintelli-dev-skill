<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/iis/ -->

# 音频数字传输总线(IIS)

---

## 1、IIS简介

![IIS系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/IIS%E7%B3%BB%E7%BB%9F%E7%BB%93%E6%9E%84%E5%9B%BE-1.png)

IIS系统结构图

CI13LC系列芯片有3路IIS，这些IIS有通用或者特殊用途。IIS0为通用IIS，有TX和RX的功能，可外接其他CODEC芯片；IIS1有TX功能，专门给CI13LC系列芯片的内部CODEC DAC使用，管脚不外接；IIS1有RX功能，专门给CI13LC系列芯片的内部CODEC ADC使用，管脚不外接。所有IIS公用一组IISDMA0，对应的IISDMAChax通道不同。

同时CI13LC系列芯片为IIS设计了专用IISDMA，专用于内存与IIS之间的数据传输。通过IISDMA不仅可以产生传输完成中断，还可以产生地址回卷中断，还可以直接与VAD模块联系起来。

---

## 2、IIS时序

### 2.1、基础简介

* 通常IIS接口由MCLK、SCK、LRCK、SDI、SDO这几根线组成
* 位时钟BCLK（又名SCK，对应每一位（bit）数据，都有一个脉冲，SCK是同时用于左、右声道数据交替传输的时钟）
* BCLK / LRCK的比值一般是32或64
* 量化位数是相对于单声道来说的，常见的位数有16bit，24bit，32bit，20bit
* 采样频率（即LRCK，用于切换左右声道的数据。一般，1表示左声道，0表示右声道）
* MCLK（主时钟，也名过采样率，一般是采样频率的128、或256、或384或512倍）

### 2.2、时钟计算

* 假设采样频率是16K（LRCK），MCLK是256倍，量化位数是16bit，SCK/LRCK = 64
* MCLK = 16K \* 256 = 4096 K = 4M
* BCLK = 64 \* 16K = 1M
* 示波器查看引脚时钟验证正确性

### 2.3、时序图

* （1）、IIS格式

![IIS系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iis-2.png)

* （2）、左对齐格式

![IIS系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iis-3.png)

* （3）、右对齐格式

![IIS系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iis-4.png)

## 3、API

| 函数名 | 描述 |
| --- | --- |
| iis\_rx\_config | IIS接收模式初始化 |
| iis\_rx\_mute | IIS接收模式，静音 |
| iis\_tx\_config | IIS发送模式初始化 |
| iis\_tx\_r\_mute | IIS发送模式，右声道静音 |
| iis\_tx\_l\_mute | IIS发送模式，左声道静音 |
| iis\_rx\_enable | IIS接收使能 |
| iis\_tx\_enable | IIS发送使能 |

## 使用示例

IIS&IISDMA使用示例请查阅☞[《音频系统管理》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/%E9%9F%B3%E9%A2%91%E7%B3%BB%E7%BB%9F%E7%AE%A1%E7%90%86/)。

## 4、API 参考