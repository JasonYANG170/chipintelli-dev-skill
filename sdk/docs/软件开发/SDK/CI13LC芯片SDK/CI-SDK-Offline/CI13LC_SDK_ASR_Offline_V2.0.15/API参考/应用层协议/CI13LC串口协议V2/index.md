<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AEV2/ -->

# CI13LC系列芯片串口协议V2

---

## 1. 概述

本文档为我司标准语音模块串口协议设计说明书，方便快速开发代码和评审，维护人员了解代码框架。

### 1.1. 功能

* 完整传输包，包含：头尾，长度，校验，消息类型，消息序列号；
* 支持变长命令，方便扩展；
* 消息类型(命令，通知，回复)；
* 命令消息，可配置，回复ACK。通知消息无ACK；
* 消息格式将与bootloader升级的相同，通过header来与bootloader协议区分；
* 增加统一的语义ID，6字节。工具需要生成对应的头文件，方便平台制作固件使用,更多语义ID说明详见☞[《CI13LC系列芯片语义ID文档说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/CI13LC%E8%AF%AD%E4%B9%89ID%E6%96%87%E6%A1%A3%E8%AF%B4%E6%98%8E/) ；
* 默认波特率使用9600。默认串口使用UART1（该串口最快为1M）；
* 支持的命令：查询协议版本号，查询系统版本号，设置音量（音量分级在user\_config.h中定义），播放本地播报音，复位命令，获取UNIQUEID，命令字尽量兼容CI1006上的标准板协议。

### 1.2. 性能

* 接收采用中断方式处理，输出采用polling方式处理，所以标准板上的不适合大数据量传输；
* 代码实现方面，为了减少不必要的malloc/free，用了copy的方式，所以此协议不适合数据量太大的长命令。

---

## 2. 协议简介

### 2.1. 串口命令格式概述

说明：该协议内非单字节数据，低字节在先，高字节在后。

![串口命令格式](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-1.png)

图2-1 串口命令格式

### 2.2. 命令详细描述

#### 2.2.1. 错误回复

说明：当发送错误时使用该回复

ACK:

![当发送错误时回复](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-2.png)

图2-2 当发送错误时回复

#### 2.2.2. 语音识别结果

说明：语音模块在进入唤醒状态后，识别到结果后发送该消息（包含唤醒词）。

CMD:

![识别到结果后发送该消息](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-3.png)

图2-3 识别到结果后发送该消息

ACK包格式如下，但暂未实现该功能和重传机制。

![ACK包格式](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-4.png)

图2-4 ACK包格式

#### 2.2.3. 播放本地播报音

说明：用于播放保存在语音模块固件内的播报音。

CMD:

![播放播报音cmd](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-5.png)

图2-5 播放播报音CMD

ACK :

![串口命令格式](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-6.png)

图2-6 播放播报音ACK

#### 2.2.4. 获取UNIQUEID

说明：用于查询flash 的uniqueID，注意，此功能需要flash器件的支持。

CMD:

![查询flash的uniqueID-CMD](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-7.png)

图2-7 查询flash的uniqueID-CMD

ACK:

![查询flash的uniqueID-ACK](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-8.png)

图2-8 查询flash的uniqueID-ACK

#### 2.2.5. 获取版本号

说明：用于查询语音模块中的固件使用的SDK的版本号。

CMD:

![查询SDK的版本号-CMD](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-9.png)

图2-9 查询SDK的版本号-CMD

ACK:

![查询SDK的版本号-ACK](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-10.png)

图2-10 查询SDK的版本号-ACK

#### 2.2.6. 复位模块

说明：用于复位语音模块。

CMD:

![串口命令格式](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-11.png)

图2-11 复位语音模块-CMD

ACK:

![串口命令格式](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-12.png)

图2-12 复位语音模块-ACK

#### 2.2.7. 设置功能

说明：用于设置模块和本串口协议的一些功能选项。

CMD:

![设置功能选项-CMD](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-13.png)

图2-13 设置功能选项-CMD

ACK:

![设置功能选项-ACK](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-14.png)

图2-14 设置功能选项-ACK

#### 2.2.8. 状态通知

说明：用于发送语音模块状态改变的通知。

CMD:

![语音模块状态改变的通知-CMD](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-15.png)

图2-15 语音模块状态改变的通知-CMD

#### 2.2.9. 进入OTA模式

说明：用于设置语音模块进入OTA模式。

CMD:

![进入OTA模式-CMD](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-16.png)

图2-16 进入OTA模式-CMD

ACK:

![进入OTA模式-ACK](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-17.png)

图2-17 进入OTA模式-ACK

---

## 3. 串口协议配置

串口通讯的具体配置:

![串口通讯的具体配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-18.png)

图2-18 串口通讯的具体配置

建议开发者在自己工程路径下进行以下配置，不要修改sdk\_default\_config.h文件里面配置。下面以sample\_light工程为例，修改串口通讯配置。打开SDK\projects\sample\_light\user\_config.h。

```
#define MSG_COM_USE_UART_EN                  1
#define UART_PROTOCOL_NUMBER            (HAL_UART1_BASE)
#define UART_PROTOCOL_BAUDRATE          (UART_BaudRate9600)
```

## 4. 串口协议使用说明

CI13LC系列芯片串口协议V2，有以下特点:

* 完整传输包，包含：头尾，长度，校验，消息类型，消息序列号。
* 支持变长命令，方便扩展。
* 消息类型(命令，通知，回复)。
* 命令消息，可配置，回复ACK。通知消息无ACK。
* 消息格式将与bootloader升级的相同，通过header来与bootloader协议区分。
* 默认波特率使用9600。默认串口使用UART1（该串口最快为1M）。
* 支持的命令：查询协议版本号，查询系统版本号，设置音量（音量分级在user\_config.h中定义），播放本地播报音，复位命令等，具体协议格式如下图所示：

举例说明1：

* A5 FC 07 00 A0 91 18 01 55 E0 01 00 00 1B 9B 02 FB解析如下，
* A5 FC：head
* 07 00:有效数据为7byte
* A0 ：这是命令词信息
* 91 ：命令号码为0x91（本次数据内容为命令词数据）
* 18 ：包序号，本串口第0x08次外发数据，该数值不断累加
* 01 55 E0 01 00 00：当前命令词的唯一数据
* 1B ：命令词阈值
* 9B 02：累加和
* FB：结尾数据

举例说明2：

* A5 FC 02 00 A3 9A 17 00 B1 05 02 FB 解析如下：
* A5 FC ：head
* 02 00：有效数据2byte
* A3 ：当前为通知数据
* 9A ：命令号码为0X9A（本次数据内容为语音模组内容改变）
* 17 ：本串口第0x07次外发数据，该数值不断累加
* 00 B1：有效数据。（本数据表示进入唤醒状态）
* 05 02：累加和
* FB：结尾数据

串口协议数据参考图：

![串口协议数据参考图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-19.png)

图2-19 串口协议数据参考图

在实际应用中，用户采用该串口协议可以方便地将设备的电控板和CI13LC系列芯片（模块）进行对接，我司出厂的模块可指定烧录对应的串口协议固件，用户将传统设备开发升级为语音设备时，可以只增加电控板主控芯片处理串口协议的代码，而不用开发CI13LC系列芯片或模块端的代码，减少了对语音芯片开发的学习时间，提升方案开发的效率。

以晾衣杆为例，当用户在电路上将电控板的串口和CI13LC系列芯片的串口连接后，烧录了带串口协议固件的CI13LC系列芯片接收到对应的语音指令后，CI13LC系列芯片的通讯串口会发出如下图所示的协议数据。如用户发出“打开吹风”的命令词，CI13LC系列芯片进行语音识别，识别到后会在串口发出如下图第三行的串口协议数据，晾衣机的电控板主控芯片通过串口接收到该数据后，处理识别发现是“打开吹风”命令词发出的协议，可以控制电机等设备执行打开吹风的动作。

![串口协议固件实例](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/img/CI13LC%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE-20.png)

图2-20 串口协议固件实例

注意事项：

1.修改UART\_PROTOCOL\_NUMBER时，请确认该串口是否已被其他外设或代码占用，SDK默认使用UART0 作为Log (关于更多Log 详见☞[《SDK Quick Start文档》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/) ci13lc\_Quit\_Start文档)输出端口；

2.串口通讯默认其他参数：数据位：8位，停止位：1位，无奇偶数校验。电控板的主控芯片的串口也要按照该参数进行配置。如果需要修改，请在对应的UART初始化接口（UARTInterruptConfig、UARTPollingConfig、UARTDMAConfig）里修改UART\_LCRConfig函数的调用参数，详见☞[《通用异步收发传输器文档》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/uart/)。