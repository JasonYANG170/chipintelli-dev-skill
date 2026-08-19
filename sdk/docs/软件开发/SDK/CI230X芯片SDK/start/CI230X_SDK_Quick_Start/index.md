<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/CI230X_SDK_Quick_Start/ -->

# SDK Quick Start

---

## 1. 概述

本文为C230X软件开发包（SDK）快速开发手册，旨在方便开发人员快速开发代码。

---

## 2. 用户指南

### 2.1. 开发环境

以下是开发过程中需要的软件和硬件：

* IDE开发软件
* CI230X系列芯片 SDK
* 串口升级工具

#### 2.1.1. IDE开发软件

SDK中所有的应用可以通过通用的 vscode 工具编译以及使用，用户可以参照文档☞[《开发环境搭建与使用》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)安装，或可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载。

#### 2.1.2. C230X SDK软件包

SDK包括示例工程，文档以及必要的工具，用户可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载。

#### 2.1.3. 串口升级工具

我司提供C230X芯片所使用的串口升级工具（启英泰伦语音离线开发工具），用于烧录及升级固件使用。关于串口升级步骤详见☞[《开发环境搭建与使用》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)第6章节语音固件烧录和第10章节WIFI固件烧录。

### 2.2. 开发板介绍

以C230X标准开发板CI-E05GT02S\_MB开发板套件为例介绍硬件，用户可以到 ☞[样品购买](http://mall.chipintelli.com/) 购买该开发板，开发板套件说明请参考:☞[《CI-E05GT02S\_MB开发板套件说明》](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI-E05GT02S_MB%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/)

---

## 3. SDK-Part1语音部分介绍

目前针对CI230X系列芯片，已提供两套SDK：（离线+IOT）SDK和(离在线+IOT) SDK

### 3.1. SDK整体架构

| 文件名 | 作用 |
| --- | --- |
| components | 目录为组件，包括播放器、ASR识别、按键、传感器、FreeRTOS操作系统等 |
| driver | 目录包括CI230X系列芯片芯片底层驱动、板级支持配置 |
| libs | 目录包括lib文件 |
| projects/cias\_aiot\_audio | 目录包括和WiFi通信接口、通信协议、speex压缩、消息处理接口、用户配置文件、（红外、灯控、空调demo）等 |
| startup | 目录包括CI13XX系列芯片启动代码 |
| system | 目录包括系统相关代码、中断处理程序文件、平台相关配置 |
| tools | 工具目录，主要是合并、打包升级工具 |

### 3.2. CI230X\_SDK 语音工程介绍

为了帮助开发者快速创建应用程序，CI230X\_SDK中已经创建了语音工程。通过学习该工程，开发者可以很容易的熟悉SDK。

语音工程路径：CI230X\_SDK\projects\

| 工程文件 | 作用 |
| --- | --- |
| cias\_aiot\_audio | CI230X系列芯片 (离线+在线)工程 |

### 3.3. Images 相关

CI230X列芯片 语音固件应用要有5个images：asr.bin、dnn.bin、user\_code.bin、user\_file.bin和voice.bin

* asr.bin：语音模型；
* dnn.bin：声学模型；
* user\_code.bin：开发者开发的应用程序，通过vscode编译生成；
* user\_file.bin：开发者定义的命令词列表以及开发者其他的bin文件；
* voice.bin：播报音。

### 3.4. 固件合成工具

* 路径：SDK\projects\cias\_aiot\_audio\firmware\
* 名称：合成分区bin文件.bat
* 作用： 调用本脚本后，会在firmware\asr、dnn、user\_file、voice各自目录下自动生成asr.bin、dnn.bin、user\_file.bin和voice.bin文件。

### 3.5. 烧录方式

使用串口升级工具- 启英泰伦语音离线开发工具，以cias\_aiot\_audio工程为例进行烧录。

烧录方式请参考☞[《开发环境搭建与使用》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)第4和第6章节

---

## 3.6. 调试

有一种方式可以调试应用程序：

* 使用log机制跟踪代码的执行和数据。

### 3.6.1. Log机制

通过串口打印log的方式来追踪应用程序的执行情况。

#### Log 输出管脚配置:

UART0\_TX管脚是默认Log UART的输出引脚(也可将log功能重新配置其他引脚)。如果配置其他串口作为log输出，配置宏CONFIG\_CI\_LOG\_UART。按下图5-1修改或添加user\_config.h里的宏定义：

![cias_aiot_audio工程配置Log输出配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/CI230X_%E6%97%A5%E5%BF%97%E4%B8%B2%E5%8F%A3.png)

图3-1 cias\_aiot\_audio工程配置Log输出配置

建议用户在工程目录下的user\_config.h里配置，不要直接修改sdk\_default\_config.h里面的宏定义。

### 3.6.2. Log 串口工具配置

PC使用串口工具配置如下，SDK默认串口0打印波特率为921600：

![PC串口工具配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/CI230X_PC%E6%97%A5%E5%BF%97%E4%B8%B2%E5%8F%A3.png)

图3-2 PC串口工具配置

注意

由于串口输出的数据有中文，建议使用支持UTF-8字体的串口工具。

推荐串口工具SecureCRT，有关SecureCRT更多信息请问：☞<https://www.vandyke.com/products/securecrt/>

### 3.6.3. Log 打印通用接口

通用log打印接口：

mprintf(fmt, args…)，用法和printf相同。

### 3.6.4. Log 打印封装接口

为了方便用户调试，SDK封装以下接口：

**表3-1** **打印API**

| Debug API | Funtion |
| --- | --- |
| ci\_logverbose(comlevel, message, args…) | 日志打印–详细 |
| ci\_logdebug(comlevel, message, args…) | 日志打印–调试 |
| ci\_loginfo(comlevel, message, args…) | 日志打印–信息 |
| ci\_logwarn(comlevel, message, args…) | 日志打印–警告 |
| ci\_logerr(comlevel, message, args…) | 日志打印–错误 |
| ci\_logassert(comlevel, message, args…) | 日志打印–断言 |

调试等级，表示log要打印的级别。定义了7种类别：

**表3-2** **调试等级**

| Debug Level | Usage Scenario |
| --- | --- |
| #define CI\_LOG\_VERBOSE | 全部打印 |
| #define CI\_LOG\_DEBUG | 调试、信息、警告，错误、断言 |
| #define CI\_LOG\_INFO | 信息、警告，错误、断言 |
| #define CI\_LOG\_WARN | 警告、错误、断言 |
| #define CI\_LOG\_ERROR | 错误、断言 |
| #define CI\_LOG\_ASSERT | 断言 |
| #define CI\_LOG\_NONE | 不打印 |

注意

使用表5-1封装接口，必须将CONFIG\_CI\_LOG\_EN定义为1。

### 3.6.5. Log 打印长度配置

```
#define UART_LOG_BUFF_SIZE          512
```

默认打印长度512个字节，可配置宏UART\_LOG\_BUFF\_SIZE修改打印长度。

## 4.SDK-Part2 WIFI部分介绍

### 4.1. SDK整体架构

| 文件名 | 作用 |
| --- | --- |
| components | 目录为组件，包括BLE协议适配、OTA、C库、系统AT指令集、网络组件、加密校验算、FreeRTOS操作系统等 |
| doc | 目录为文档目录，包含ota升级说明、系统demo开发使用等 |
| lib | 目录包括lib文件 |
| mcu | 目录为ARM控制器软件接口标准、寄存器控制、HAL驱动适配、系统资源配置、内存map等 |
| project/ci230x-wifi-sdk/combo/app | 目录包括ate接口、ble用户接口、用户任务适配，用户任务入口等 |
| project/ci230x-wifi-sdk/combo/bsp | 目录包括adc测试接口、串口硬件映射配置接口 |
| project/ci230x-wifi-sdk/combo/cfg | 目录包括flash分区文件、系统配置文件等 |
| project/ci230x-wifi-sdk/combo/gcc | 目录包括gcc编译器、cmake编译器 |
| project/ci230x-wifi-sdk/combo/startup | 目录包括CI230X系列芯片 WIFI启动代码 |
| tools | 工具目录，主要Jflash工具、python脚本工具 |

### 4.2. CI230X\_SDK WIFI用户工程目录介绍

为了帮助开发者快速熟悉SDK，在构建WIFI SDK时已经对SDK进行了解耦，用户只需要重点关注和用户开发者紧密结合的文件夹：project/ci230x-wifi-sdk/combo/app/cias\_aiot\_wifi

cias\_aiot\_wifi目录说明：

| 文件名 | 作用 |
| --- | --- |
| cias\_adapter | 用户适配接口目录 |
| cias\_algorithm | 算法接口目录(speex算法) |
| cias\_auth | 用户鉴权认证目录 |
| cias\_ble\_port | 用户ble适配目录 |
| cias\_cloud | 云端接入目录，包括（阿里IOT-飞燕、涂鸦IOT、海信IOT、腾讯IOT、腾讯云小微） |
| cias\_common | 公共接口目录（包括系统管理接口、循环队列）等 |
| cias\_kernel | 用户适配系统方法接口(包括事件管理、信号量、任务管理、定时器，互斥信号)等 |
| cias\_log | 日志定义目录 |
| cias\_media | 语音数据处理目录 |
| cias\_msg\_handle | 消息处理目录 |
| cias\_net | 网络接口目录(包括http、ap)等 |
| cias\_ota | ota适配目录(WiFi固件升级、语音固件透传升级) |
| cias\_protocol | 协议目录 |
| cias\_system | 系统接口目录 |
| cias\_test | 接口测试目录 |
| cias\_user\_cfg | 用户配置目录、该目录用户重点关注 |
| cias\_wifi\_port | WiFi适配接口目录 |

### 4.3. Images 相关

CI230X系列芯片 WIFI固件应用编译后会生成两个镜像文件flashimage.bin、flashimage-ota-xz-vx.x.bin

* flashimage.bin：系统固件、用户烧录该文件到flash；
* flashimage-ota-xz-vx.x.bin：ota升级固件，ota使用；

固件生成目录sdk/build-ci230x-wifi-sdk-combo-release/bin目录：

![cias_aiot_audio工程配置Log输出配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/CI230X_WIFI%E7%94%9F%E6%88%90%E5%9B%BA%E4%BB%B6.png)

图4-1 WIFI编译固件

### 4.4. 烧录方式

使用串口升级工具- 启英泰伦语音离线开发工具，烧录方式请参考☞[《开发环境搭建与使用》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)第9和第10章节

### 4.5. 调试

有一种方式可以调试应用程序：

* 使用log机制跟踪代码的执行和数据。

### 4.5.1. Log机制

通过串口打印log的方式来追踪应用程序的执行情况。

#### Log 输出管脚配置:

UART0\_TX管脚是默认Log UART的输出引脚(也可将log功能重新配置其他引脚)。如果配置其他串口作为log输出，或者串口0想映射到其他管脚，修改serial\_hw.c中串口硬件管脚映射即可，如图4-2

![cias_aiot_audio工程配置Log输出配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/CI230X_WIFI%E6%97%A5%E5%BF%97%E4%B8%B2%E5%8F%A3%E4%BF%AE%E6%94%B9.png)

图4-2 WIFI日志串口配置

#### 4.5.2. Log 打印接口

通用log打印接口：

log\_printf(fmt, args…)，用法和printf相同。

为了方便用户调试，SDK封装以下接口：LOG(level, …)，用于分级打印；使用实例：LOG(LOG\_LVL\_ERROR,“%s”, “error info.\r\n”);

#### 表4-1：调试等级表

| Debug API | Function |
| --- | --- |
| #define LOG\_LVL\_EMERG | *最高级别-严重* |
| #define LOG\_LVL\_ERROR | 错误、断言 |
| #define LOG\_LVL\_WARN | 警告、错误、断言 |
| #define LOG\_LVL\_INFO | 信息、警告，错误、断言 |
| #define LOG\_LVL\_DEBUG | 调试、信息、警告，错误、断言 |
| #define LOG\_LVL\_TRACE | 不打印 |

#### 4.5.3.Log 打印长度配置：wrap\_stdio.c文件，默认打印最大日志输出512字节

```
#define WRAP_STDOUT_BUF_SIZE   (512)     //LOG打印长度
```