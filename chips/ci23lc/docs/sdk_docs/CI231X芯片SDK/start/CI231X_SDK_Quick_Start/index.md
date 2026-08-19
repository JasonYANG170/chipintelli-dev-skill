<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/CI231X_SDK_Quick_Start/ -->

# SDK Quick Start

---

## 1. 概述

本文为CI231X系列芯片软件开发包（SDK）快速开发手册，旨在方便开发人员快速开发代码。

---

## 2. 用户指南

### 2.1. 开发环境

以下是开发过程中需要的软件和硬件：

* IDE开发软件
* CI231X系列芯片 SDK
* 串口升级工具

#### 2.1.1. IDE开发软件

SDK中所有的应用可以通过通用的 vscode 工具编译以及使用，用户可以参照文档☞[《编译软件安装与使用》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)安装，或可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载。

#### 2.1.2. CI231X系列芯片SDK软件包

SDK包括示例工程，文档以及必要的工具，用户可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载。

#### 2.1.3. 串口升级工具

我司提供CI231X系列芯片所使用的串口升级工具PACK\_UPDATE\_TOOL.exe，用于烧录及升级固件使用。关于串口升级步骤详见第4章烧录。

### 2.2. 开发板介绍

以CI231X系列芯片标准开发板CI-E12GS02J为例介绍硬件，用户可以到 ☞[样品购买](http://mall.chipintelli.com/) 购买该开发板。

#### 2.2.1. 开发板实物图

型号：CI-E12GS02J（主芯片CI231X系列芯片）。

![开发板正面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI2312-%E6%A8%A1%E7%BB%84%E6%9D%BF1_%E6%AD%A3.png)

图2-1 开发板正面

![开发板背面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI2312-%E6%A8%A1%E7%BB%84%E6%9D%BF1_%E5%8F%8D.png)

图2-2 开发板背面

#### 2.2.2. 开发板简介

* CI-E12GS02J开发板硬件图如下。

![开发板](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI2312-%E6%A8%A1%E7%BB%84%E6%9D%BF1_%E7%AE%A1%E8%84%9A%E8%AF%B4%E6%98%8E.png)

* **1: 喇叭底座**。
* **2: 麦克风底座**。
* **3 : 电控管脚(PA2和PA3也可以配置为串口1)**。
* **4: 升级固件使用串口UART0\_TX和UART0\_RX**。
* **5: 输入电源5V、GND**。

---

## 3. 应用程序

目前针对CI231X系列芯片，已提供一套SDK：CI231X\_SDK(离线语音识别SDK）。

### 3.1. SDK整体架构介绍

| 文件名 | 作用 |
| --- | --- |
| components | 目录为组件，包括播放器、ASR识别、BLE和2.4G、按键、传感器、FreeRTOS操作系统等 |
| driver | 目录包括CI231X系列芯片底层驱动、板级支持配置 |
| libs | 目录包括lib文件 |
| projects | 目录包括示例工程 |
| startup | 目录包括CI231X系列芯片启动代码 |
| system | 目录包括系统相关代码、中断处理程序文件、平台相关配置 |
| tools | 工具目录，主要是合并、打包升级工具 |

### 3.2. CI231X\_SDK示例工程介绍

为了帮助开发者快速创建应用程序，CI231X\_SDK中已经创建了示例工程。通过学习示例工程，开发者可以很容易的熟悉SDK。

示例工程路径：CI231X\_SDK\projects\

| 工程文件 | 作用 |
| --- | --- |
| offline\_asr\_sample | CI231X系列芯片离线识别+BLE工程 |

---

## 4. 烧录

### 4.1. Images 相关

CI231X系列芯片应用要有5个images：asr.bin、dnn.bin、user\_code.bin、user\_file.bin和voice.bin

* asr.bin：语音模型；
* dnn.bin：声学模型；
* user\_code.bin：开发者开发的应用程序，通过vscode编译生成；
* user\_file.bin：开发者定义的命令词列表以及开发者其他的bin文件；
* voice.bin：播报音。

### 4.2. 固件合成工具

* 路径：SDK\projects\offline\_asr\_sample\firmware\
* 名称：合成分区bin文件.bat
* 作用： 调用本脚本后，会在firmware\asr、dnn、user\_file、voice各自目录下自动生成asr.bin、dnn.bin、user\_file.bin和voice.bin文件。

### 4.3. 烧录方式

使用串口升级工具PACK\_UPDATE\_TOOL.exe进行升级固件，以offline\_asr\_sample工程为例进行烧录。

(1) 采用USB转串口工具升级硬件连线方法：

![串口升级硬件连线](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-19.png)

图4-1 硬件连接图

* **1：电源(5V/GND)**
* **2：TX0和RX0连接到USB转串口工具的RXD和TXD**
* **3：USB转串口工具，连接到PC**

#### 步骤2：编译代码生成user\_code.bin 文件

（1）编译方法可以参照文档☞[《编译软件安装与使用》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)

（2）完成编译后，SDK\projects\offline\_asr\_sample\firmware\user\_code生成的应用代码user\_code.bin，如下图4-4 所示：

![user_code.bin 生成](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-5.png)

图4-2 user\_code.bin 生成

#### 步骤3：生成其他所需4个bin文件

**方法一：编译软件中直接合成**

（1）如下图步骤，运行（合成分区bin文件.bat）：

![编译软件中直接合成](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-77.png)

图4-3 编译软件中直接合成

（2）编译软件终端合成运行结果如下：

![运行结果](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-78.jpg)

图4-4 运行结果

**方法二：双击运行脚本（合成分区bin文件.bat）**

（1）进入SDK\projects\offline\_asr\_sample\firmware\下， 如下图4-5所示：

![firmware文件目录](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-6.png)

图4-5 firmware文件目录

（2）双击打开合成分区bin文件.bat，运行界面如图4-7所示，等待软件运行完成自动退出，调用脚本后，会在SDK\projects\offline\_asr\_sample\firmware\asr、dnn、user\_file、voice各自目录下自动生成asr.bin、dnn.bin、user\_file.bin和voice.bin文件。

![软件运行](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-67.png)

图4-6 合成分区结果

注意

* 10s处理完成后cmd.exe自动退出，如果没有自动退出，生成.bin文件失败，请检查asr、dnn、user\_file、voice下文件或文件名是否正确；

#### 步骤4：将5个bin文件打包成一个bin文件

方式一：如下图所示，在vscode编译环境中启动打包升级工具PACK\_UPDATE\_TOOL.exe

![启动升级工具](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-79.png)

图4-7 启动打包工具方法一

方式二：进入SDK\projects\offline\_asr\_sample\firmware\下， 如图4-6所示，双击打开（打包升级.bat），打开PACK\_UPDATE\_TOOL.exe

方式三：进入SDK\tools\，打开PACK\_UPDATE\_TOOL.exe，界面如下图4-8所示：

![串口升级工具选择界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-22.png)

图4-8 启动打包工具方法二

第一次打开软件时，这里会弹出提示框，界面如下图4-9所示，这里需选择CI130X。

![串口升级工具选择界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-8.png)

图4-9 串口升级工具选择界面

![串口升级工具主界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-9.png)

图4-10 串口升级工具主界面

* **CI231X系列芯片：当前是CI231X系列芯片使用模式；**
* **English：当前是中文显示，可点击切换成英文显示；**
* **固件打包：切换到固件打包界面；**
* **固件升级：切换到固件升级界面；**
* **按下“F1”查看帮助：关于升级工具的更多使用说明。**

点击串口升级工具主界面的（固件打包）按钮，界面如下图4-11所示：

![串口升级工具固件打包界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-10.png)

图4-11 串口升级工具固件打包界面

确认5个分区预留大小是否适合，例如user\_code.bin的实际大小为0x262A3字节，但是预留大小为0x23000字节。此时需要将user区的预留大小调整为0x27000字节，相应的可减少其他分区。voice.bin的实际大小为0XD80B9字节，voice预留大小为0x25000字节，此时将voice预留大小重新设置为0XD9000字节，相应的可减少其他分区。

注意

1.预留大小按照0x1000的整数倍进行配置。

点击（打包固件）按钮，出现固件已生成的窗口，表示固件制作完成。

点击确定后，在SDK\projects\offline\_asr\_sample\firmware目录下生成一个.bin文件。.bin文件的名字由打包界面软件名称和软件版本组成。如下图4-12所示的Firmware\_V200.bin：

![串口升级工具打包成功界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-11.jpg)

图4-12 串口升级工具打包成功界面

#### 步骤5：升级固件

完成上述步骤后。建议进行下面的确认：

1.确保硬件连线正确（详见步骤1）；

2.确保确保TYPE-C线或USB转串口工具连接到PC，PC能够识别到COM端口；

3.确保固件生成正确；

4.确保麦克风和喇叭正确连接开发板。

点击图4-10或图4-11界面的固件升级按钮，界面如下图4-13所示：

![串口升级工具升级界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-12.png)

图4-13 串口升级工具升级界面

* **1：选择步骤4生成的固件或者本司提供的固件；**
* **2：选择正确的COM端口；**
* **3：复位（可通过复位按钮，也可通过断电重启的方式）开发板，开发板自动开始升级。升级完成后显示update success。**

提示

更多固件制作信息可以访问 ☞[《命令词和固件制作指南》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/%E5%91%BD%E4%BB%A4%E8%AF%8D%E5%92%8C%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%E6%8C%87%E5%8D%97/)页面，或从图4-10起始页按“F1”查看帮助。

#### 步骤6：验证固件

烧录完成后，重启开发板，offline\_asr\_sample工程启动后会有语音播报。播报完成后，说唤醒词“智能管家”后，播报“你好”，表示升级成功。

**如果没有语音播报，请参考下面（第5章-调试），查找具体原因。**

---

## 5. 调试

有一种方式可以调试应用程序：

* 使用log机制跟踪代码的执行和数据。

### 5.1. Log机制

通过串口打印log的方式来追踪应用程序的执行情况。

#### 5.1.1. Log 输出管脚配置

UART0\_TX管脚是默认Log UART的输出引脚(也可将log功能重新配置其他引脚)。如果配置其他串口作为log输出，配置宏CONFIG\_CI\_LOG\_UART。按下图5-1修改或添加user\_config.h里的宏定义：

![offline_asr_sample工程配置Log输出配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-13.png)

图5-1 offline\_asr\_sample工程配置Log输出配置

建议用户在工程目录下的user\_config.h里配置，不要直接修改sdk\_default\_config.h里面的宏定义。

#### 5.1.2. Log 串口工具配置

PC使用串口工具配置如下，SDK默认串口0打印波特率为921600：

![PC串口工具配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/CI130X_SDK_Quick_Start-114.png)

图5-2 PC串口工具配置

注意

由于串口输出的数据有中文，建议使用支持UTF-8字体的串口工具。

推荐串口工具SecureCRT，有关SecureCRT更多信息请问：☞<https://www.vandyke.com/products/securecrt/>

#### 5.1.3. Log 打印通用接口

通用log打印接口：

mprintf(fmt, args…)，用法和printf相同。

#### 5.1.4. Log 打印封装接口

为了方便用户调试，SDK封装以下接口：

**表5-1** **打印API**

| Debug API | Funtion |
| --- | --- |
| ci\_logverbose(comlevel, message, args…) | 日志打印–详细 |
| ci\_logdebug(comlevel, message, args…) | 日志打印–调试 |
| ci\_loginfo(comlevel, message, args…) | 日志打印–信息 |
| ci\_logwarn(comlevel, message, args…) | 日志打印–警告 |
| ci\_logerr(comlevel, message, args…) | 日志打印–错误 |
| ci\_logassert(comlevel, message, args…) | 日志打印–断言 |

调试等级，表示log要打印的级别。定义了7种类别：

**表5-2** **调试等级**

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

#### 5.1.5. Log 打印长度配置

```
#define UART_LOG_BUFF_SIZE          512
```

默认打印长度512个字节，可配置宏UART\_LOG\_BUFF\_SIZE修改打印长度。