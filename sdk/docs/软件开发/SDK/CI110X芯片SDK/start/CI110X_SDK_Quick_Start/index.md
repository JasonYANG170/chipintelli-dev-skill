<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/CI110X_SDK_Quick_Start/ -->

# SDK Quick Start

---

## 1. 概述

本文为CI110X系列芯片软件开发包（SDK）快速开发手册，旨在方便开发人员快速开发代码。

---

## 2. 用户指南

### 2.1. 开发环境

以下是开发过程中需要的软件和硬件：

* IDE开发软件
* 调试工具
* CI110X系列芯片SDK软件包
* 串口升级工具

#### 2.1.1. IDE开发软件

SDK中所有的应用可以通过IAR IDE开发软件编译以及使用，版本为8.30.1，有关该软件的更多信息请访问: ☞ <https://www.iar.com/iar-embedded-workbench/>， 用户可以自行联系IAR公司或其经销商采购该软件并使用。

#### 2.1.2. 调试工具

SDK开发时采用J-Link进行调试，硬件建议使用V9以上版本。有关该工具的更多信息请访问: ☞ <https://www.segger.com> ，用户可以自行下载相关的软件驱动和文档，安装J-Link配套的软件。特别注意使用前一定要正确安装设备驱动，才能搭配IDE开发软件使用J-Link进行调试。关于IDE开发软件和J-Link配合调试的使用说明请访问☞ <https://www.segger.com> 进行查看。

#### 2.1.3. CI110X系列芯片SDK软件包

SDK包括示例工程，文档以及必要的工具，用户可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料中下载。

#### 2.1.4. 串口升级工具

我司提供CI110X系列芯片所使用的串口升级工具PACK\_UPDATE\_TOOL.exe，用于烧录及升级固件使用。关于串口升级步骤详见第4章烧录。

### 2.2. 开发板介绍

CI110X系列芯片标准开发板由一块底板和一块子板组成，下面以单麦GS01S开发板套件（B02GS01S）为例介绍硬件，用户可以到 ☞[样品购买](http://mall.chipintelli.com/) 购买该套件。

#### 2.2.1. 开发板子板

子板型号：CI-B02GS01S（主芯片CI1102）。

![子板正面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-2.jpg)

图2-2 子板正面

![子板背面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-3.jpg)

图2-3 子板背面

#### 2.2.2. 开发板底板

底板型号：CI-B02-MB-V10。

![底板正面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-4.jpg)

图2-4 底板正面

![底板背面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-5.jpg)

图2-5 底板背面

#### 2.2.3. 整体开发板

* 单麦GS01S开发板套件（B02GS01S）整体硬件图如下。

![整体开发板](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-6.png)

* **1: 输入电源5V/GND**。
* **2: 模式选择，支持调试模式、升级模式、正常模式**。

短接SEL和EN —> **调试模式**：通过J-link下载代码到内存中运行，但是不会更新Flash里面的代码。

短接PG和EN —> **升级模式**：通过本司的串口升级工具（PACK\_UPDATE\_TOOL.exe），更新Flash里面的固件。

不进行任何短接 —> **正常模式**：不能通过J-link下载代码到内存，不能进行串口升级。如果Flash里固件正确，系统正常启动。

注意

短接SEL和EN或者PG和EN，必须重新上电，才会起作用。如果短接PG和EN后，1s内没有使用串口升级工具进行升级，系统仍然会正常启动。

* **3 : 升级固件使用串口UART0\_TX和UART0\_RX**。

处于升级模式时，TX0、RX0用于串口升级使用，升级完成进入系统后，TX0用于Log（Log详见第5章）输出。

* **4: 麦克风底座**。
* **5: 喇叭底座**。
* **6: JLINK连接端口,TCK、TMS、GND**。

---

## 3. 应用程序

目前公司提供了3套SDK，分别是CI110X\_SDK\_ASR\_Offline(离线语音识别SDK）、CI110X\_SDK\_ALG\_Application(算法SDK)、CI110X\_SDK\_Combine\_Cloud(离在线语音识别SDK)

### 3.1. SDK整体架构介绍

| 文件名 | 作用 |
| --- | --- |
| components | 目录为组件，包括播放器、ASR识别、按键、传感器、FreeRTOS操作系统等 |
| doc | 文档目录 |
| driver | 目录包括CI110X系列芯片底层驱动 |
| platform | 目录包括平台相关配置 |
| sample | 目录包括示例工程 |
| startup | 目录包括CI110X系列芯片启动代码 |
| system | 目录包括系统相关代码、中断处理程序文件 |
| tools | 工具目录，主要是合并、打包升级工具 |

### 3.2. CI110X\_SDK\_ASR\_Offline示例工程介绍

为了帮助开发者快速创建应用程序，CI110X\_SDK\_ASR\_Offline中已经创建一些的示例工程。通过学习示例工程，开发者可以很容易的熟悉SDK。

示例工程路径：CI110X\_SDK\_ASR\_Offline\sample\internal\

| 工程文件 | 作用 |
| --- | --- |
| sample\_1102 | CI1102 离线识别工程 |
| sample\_1103 | CI1103 离线识别工程 |

### 3.3. CI110X\_SDK\_ALG\_Application示例工程介绍

示例工程路径：CI110X\_SDK\_ALG\_Application\sample\internal\

| 工程文件 | 作用 |
| --- | --- |
| sample\_common | CI1103 离线多种算法工程 |
| sample\_cwsl | CI1103 离线命令词自学习工程 |

### 3.4. CI110X\_SDK\_Combine\_Cloud示例工程介绍

示例工程路径：CI110X\_SDK\_Combine\_Cloud\sample\internal\

| 工程文件 | 作用 |
| --- | --- |
| sample\_media | CI1103 离在线工程，串口传输音频 |
| sample\_i2s\_interface | CI1103 离在线工程，IIS传输音频 |

---

## 4. 烧录

### 4.1. Images 相关

CI110X系列芯片应用要有5个images:asr.bin、dnn.bin、user.bin、user\_file.bin和voice.bin

* asr.bin：语音模型
* dnn.bin：声学模型
* user.bin：开发者开发的应用程序
* user\_file.bin：开发者定义的命令词列表以及开发者其他的bin文件
* voice.bin：播报音

### 4.2. 固件合成工具

* 路径：SDK\sample\internal\sample\_xxx\firmware\
* 名称：合成分区bin文件.bat
* 作用： 调用本脚本后，会在firmware\asr、dnn、user\_file、voice各自目录下自动生成asr.bin、dnn.bin、user\_file.bin和voice.bin文件。

### 4.3. 烧录方式

使用串口升级工具进行升级固件，以sample\_1102工程为例进行烧录代码。

#### 步骤1：串口升级硬件连线

![串口升级硬件连线](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-7.jpg)

* **1：电源(5V/GND)**
* **2：短接PG和EN**
* **3：TX0和RX0连接到USB转串口工具的RXD和TXD**
* **4：USB转串口工具，连接到PC**

#### 步骤2：编译代码生成user.bin文件

进入SDK\sample\internal\sample\_1102\project\下， 如下图4-1所示：

![编译代码](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-8.jpg)

图4-1 project目录

首先使用IAR（详见2.1 开发环境）打开ci110\_project.eww文件，然后clean工程，如下图4-2所示：

![编译代码](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-9.jpg)

图4-2 clean工程

最后make(编译)工程，入下图4-3所示：

![编译代码](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-10.jpg)

图4-3 编译工程

完成编译后，

SDK\sample\internal\sample\_1102\firmware\user\_code生成的应用代码user.bin，如下图4-4 所示：

![完成编译](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-11.jpg)

图4-4 user.bin生成

#### 步骤3：生成其他所需4个bin文件

进入SDK\sample\internal\sample\_1102\firmware\下， 如下图4-5所示：

![生成其他所需4个bin文件](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-12.jpg)

图4-5 firmware文件目录

打开合成分区bin文件.bat

![打开合成分区bin文件](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-13.jpg)

10s处理完成后cmd.exe自动退出，如果没有自动退出，生成.bin文件失败，请检查asr、dnn、user\_file、voice下文件或文件名是否正确。

#### 步骤4：将5个bin文件打包成一个bin文件

方式一：进入SDK\sample\internal\sample\_1102\firmware\下， 如图4-5所示，打开打包升级.bat

方式二：进入SDK\tools\，打开PACK\_UPDATE\_TOOL.exe

界面如下图4-6所示：

![串口升级工具主界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-14.png)

图4-6 串口升级工具主界面

点击固件打包，界面如下图4-7所示：

![串口升级工具打包界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-15.png)

图4-7 串口升级工具打包界面

确认5个分区预留大小是否适合，例如user.bin的实际大小为0x262A3字节，但是预留大小为0x23000字节。此时需要将user区的预留大小调整为0x27000字节。voice.bin的实际大小为0XD80B9字节，voice预留大小为0x25000字节，此时将voice预留大小重新设置为0XD9000字节。

注意

1.预留大小按照0x1000的整数倍进行配置。
2.配置预留大小后需要刷新地址。

点击打包固件，出现固件已生成的窗口，表示固件制作完成。如下图4-8所示：

![串口升级工具打包完成界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-16.png)

图4-8 串口升级工具打包完成界面

点击OK后，在SDK\sample\internal\sample\_1102\firmware目录下生成一个.bin文件。.bin文件的名字由打包界面软件名称和软件版本组成。如下图4-9所示：

![固件生成](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-17.png)

图4-9 固件生成

#### 步骤5：升级固件

完成上述步骤后。建议进行下面的确认：

1.确保连线正确（详见步骤1）

2.确保进入升级模式（详见2.2模块介绍）

3.确保USB转串口连接到PC，PC能够识别到COM端口

4.确保固件生成正确

5.确保麦克风和喇叭正确插入开发板

点击图4-9界面的固件升级按钮。界面如下图4-10所示：

![串口升级工具升级界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-18.png)

图4-10 串口升级工具升级界面

选择步骤4生成的固件或者本司提供的固件，如下图4-11所示：

![选择固件](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-19.png)

图4-11 选择固件

选择正确的COM端口，如下图4-12所示：

![选择COM端口](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-20.jpg)

图4-12 选择COM端口

复位（可通过断电重启的方式）开发板，开发板自动开始升级。升级完成后显示update successful界面如下图4-13所示。关闭串口升级工具。

![升级完成界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-21.png)

图4-13 升级完成界面

提示

更多固件制作信息可以访问[命令词和固件制作指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/%E5%91%BD%E4%BB%A4%E8%AF%8D%E5%92%8C%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%E6%8C%87%E5%8D%97/)页面

#### 步骤6：验证固件

烧录完成后，系统会自动重启，sample\_1102工程启动后会有语音播报。播报完成后，说唤醒词“智能管家”后，播报“你好”，表示升级成功。

**如果没有语音播报，请参考下面（第5章-调试），查找具体原因。**

---

## 5. 调试

有两种方式可以调试应用程序：

* 使用log 机制跟踪代码的执行和数据。
* 使用IAR 和 Jlink调试代码，增加/删除breakpoints（断点）以及访问/追踪memory等。

### 5.1. Log机制

通过串口打印log的方式来追踪应用程序的执行情况。

#### 5.1.1. Log 输出管脚配置

UART0\_TX管脚是默认Log UART的输出引脚(也可将log功能重新配置其他引脚)。如果配置其他串口作为log输出，配置宏CONFIG\_CI\_LOG\_UART。下图5-1以sample\_1102工程（关于工程详见第三章）为例，使用UART2\_TX作为Log输出：

![sample_light工程配置Log输出配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-22.jpg)

图5-1 sample\_1102工程配置Log输出配置

建议用户将该宏定义到工程目录下的user\_config.h，不要直接修改sdk\_default\_config.h里面的宏定义。

#### 5.1.2 Log 串口工具配置

PC使用串口工具配置如下：

![串口工具配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/SDK_Quick_Start-23.gif)

注意

由于串口输出的数据有中文，建议使用支持UTF-8字体的串口工具。

推荐串口工具SecureCRT，有关SecureCRT更多信息请问：☞ <https://www.vandyke.com/products/securecrt/>

#### 5.1.3 Log 打印通用接口

通用log打印接口：

mprintf(fmt, args…)，用法和printf相同。

#### 5.1.4 Log 打印封装接口

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

#### 5.1.5 Log 打印长度配置

```
#define UART_LOG_BUFF_SIZE          512
```

默认打印长度512个字节，可配置宏UART\_LOG\_BUFF\_SIZE修改打印长度。

### 5.2. IDE工具 和 Jlink调试使用方法

IAR工具的调试使用方法请详见其使用手册，有关IAR的更多信息请访问 <https://www.iar.com/iar-embedded-workbench/>

Jlink调试使用手册，请查看 ☞[《JLINK 调试指南》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/JLINK%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97/)