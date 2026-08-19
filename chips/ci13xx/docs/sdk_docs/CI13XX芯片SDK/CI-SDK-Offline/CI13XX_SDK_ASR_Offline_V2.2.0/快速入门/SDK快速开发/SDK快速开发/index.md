<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/ -->

# SDK 快速开发

---

## 1. 概述

本文为CI13XX系列芯片离线语音软件开发包（CI13XX\_SDK\_ASR\_Offline\_Vx.x.x）的快速开发指南，旨在方便开发者基于该SDK快速熟悉开发调试步骤。SDK下载地址为 ☞[SDK下载链接](https://aiplatform.chipintelli.com/attachment)。

---

## 2. 用户指南

### 2.1. 软件开发环境介绍

#### 2.1.1. 启英泰伦集成开发环境

开发者参照☞《[IDE 搭建与使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)》完成启英泰伦集成开发环境的搭建，即可在搭建好的集成开发环境上使用 **离线语音识别SDK** 。

#### 2.1.2. CI13XX 系列离线语音识别SDK

示例工程、技术文档及配套工具包均包含在SDK中。请访问 ☞[启英泰伦语音 AI 平台](https://aiplatform.chipintelli.com/attachment) 下载最新版SDK。在SDK中包含有打包升级工具（PACK\_UPDATE\_TOOL.exe），该软件工具用于 CI13XX 系列芯片的打包固件、固件升级和打印调试等，操作步骤请参考本文第4章（固件升级）。

### 2.2. 硬件开发板介绍

下面以 **CI1303开发板** 为例介绍硬件，该开发板可以到 ☞[样品购买](http://mall.chipintelli.com/chip2?product_id=74&brd=1) 链接中购买。其余开发板的使用详见开发板使用指南。

| 开发板套件 | 支持芯片 | 购买链接 | 使用指南 |
| --- | --- | --- | --- |
| CI1312开发板套件 | CI1312、CI1311 | 补充中… | 补充中… |
| CI1306开发板套件 | CI1306 | ☞[样品购买](http://mall.chipintelli.com/chip2?product_id=74&brd=1) | ☞[开发板使用指南](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI1306%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/) |
| CI1303开发板套件 | CI1303、CI1302、CI1301 | ☞[样品购买](http://mall.chipintelli.com/chip2?product_id=87&brd=1) | ☞[开发板使用指南](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI1301%26CI1302%26CI303%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/) |
| CI1302开发板套件 | CI1302、CI1301 | ☞[样品购买](http://mall.chipintelli.com/chip2?product_id=86&brd=1) | ☞[开发板使用指南](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI1301%26CI1302%26CI303%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/) |

#### 2.2.1. 开发板实物图

CI1303开发板：CI-D03GS02S模块x1 + CI-B02-MB底板x1（芯片型号：CI1303）。

![开发板正面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-15.png)

图2-1 CI1303开发板正面

#### 2.2.2. 开发板主要接口介绍

* CI1303开发板主要接口图如下图所示。

![开发板](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-17.png)

图2-3 开发板主要接口示意图

* **1：USB Type-C接口 —— 提供 5 V 供电，并支持固件升级和打印调试；**
* **2：USB Type-C 5 V 供电按钮开关；**
* **3：UART0 TX / RX 排针 —— 用于固件升级和打印调试；（需要使用☞[UART串口调试工具](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%B5%84%E6%BA%90/UART%E4%B8%B2%E5%8F%A3%E8%B0%83%E8%AF%95%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)）**
* **4：麦克风底座；**
* **5：喇叭底座；**

---

## 3. 应用程序

目前针对开发CI13XX系列芯片的应用程序，我司已提供一套离线语音识别SDK（CI13XX\_SDK\_ASR\_Offline\_Vx.x.x）用于代码开发相关操作，下面将对该SDK进行介绍。

### 3.1. SDK整体架构介绍

| 文件名 / 目录 | 作用 |
| --- | --- |
| components | 组件：播放器、ASR 识别、按键、传感器、FreeRTOS 等 |
| driver | CI13XX 系列芯片底层驱动、板级支持配置 |
| libs | 预编译静态/动态库文件 |
| projects | 示例工程 |
| startup | CI13XX 系列芯片启动代码 |
| system | 系统相关代码、中断处理程序、平台配置 |
| tools | 工具：images 合成、固件打包升级工具等 |

### 3.2. CI13XX\_SDK\_ASR\_Offline\_Vx.x.x示例工程介绍

为便于快速上手，CI13XX\_SDK\_ASR\_Offline\_Vx.x.x已内置示例工程。通过编译、运行这些示例，开发者可迅速了解 SDK 的架构与使用方法。

示例工程路径：CI13XX\_SDK\_ASR\_Offline\_Vx.x.x\projects\

| 工程文件 | 作用 |
| --- | --- |
| offline\_asr\_pro\_sample | 离线语音识别 Pro 版示例工程 |

### 3.3. SDK新文件添加与配置

说明

CI13XX系列芯片的SDK中，如果用户增加了新的源代码文件，以“offline\_asr\_pro\_sample”工程为例，则必须找到工程文件SDK\projects\offline\_asr\_pro\_sample\project\_file\source\_file.prj，参照下列示例添加文件到工程编译。

#### 3.3.1. 添加.h头文件

* 找到//header file path.注释所在位置，参照示例填写.h头文件所在相对路径

```
//header file path.
include-path: driver/ci130x_chip_driver/inc
include-path: driver/boards
```

#### 3.3.2. 添加.c源文件

* 找到//source files.注释所在位置，参照示例填写.c源文件所在相对路径

```
//source files.
source-file: driver/ci130x_chip_driver/src/ci130x_core_eclic.c 
source-file: driver/ci130x_chip_driver/src/ci130x_core_timer.c
```

#### 3.3.3. 添加.a库文件

* 参照示例填写.a库文件所在相对路径

```
library-file: libs/libnewlib_port.a 
library-file: libs/libasr.a
```

---

## 4. 固件升级

### 4.1 .bin文件说明

CI13XX 应用固件由 5 个 bin文件组成：

| Image 文件 | 功能说明 |
| --- | --- |
| `asr.bin` | 语言模型 |
| `dnn.bin` | 声学模型 |
| `user_code.bin` | 应用程序，由SDK编译生成 |
| `user_file.bin` | 开发者定义的命令词列表以及其他的bin文件 |
| `voice.bin` | 播报音 |

### 4.2 .bin文件生成脚本

* 路径：`CI13XX_SDK_ASR_Offline_Vx.x.x\projects\offline_asr_pro_sample\firmware\`
* 脚本：`合成分区bin文件.bat`
* 功能：执行脚本后，系统将在 `firmware\\asr`、`dnn`、`user_file`、`voice` 子目录中自动生成 `asr.bin`、`dnn.bin`、`user_file.bin`、`voice.bin` 四个分区文件。

### 4.3 固件打包升级流程

下面将以 `offline_asr_pro_sample` 工程为例，介绍如何使用串口升级工具 `PACK_UPDATE_TOOL.exe` 完成固件升级。

#### 步骤 1：硬件连线

开发板提供两种常用固件升级连线方式(方式A或方式B二选一)，硬件连接选择可根据实际环境灵活调整：

* **方式 A：USB Type-C 直连** —— 利用板载 USB 转串口控制器，单根数据线即可完成供电、串口通讯及固件升级，操作简便；

![串口升级硬件连线1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-18.png)

图4-1 硬件连接图一

* **1：USB Type-C 数据线，连接至 PC，参见上图标号 1**

---

* **方式 B：UART串口调试工具** —— 在无 USB Type-C 数据线时，可使用UART串口调试工具进行供电，以及通过排针连接芯片和UART串口调试工具的 TX/RX 引脚完成固件升级。

![串口升级硬件连线2](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-19.png)

图4-2 硬件连接图二

* **1：5 V 供电 (VCC/GND)，UART0\_TX → 转串口工具 RXD，UART0\_RX → 转串口工具 TXD，参见上图标号 1**
* **2：UART串口调试工具 —— 通过 USB 口连接至 PC**

---

#### 步骤 2：编译生成 `user_code.bin`

1. 参照☞[《IDE 搭建与使用》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)文档完成环境配置并执行编译。

注意

1、首次编译 SDK 代码前，请根据目标硬件，在板级配置文件中选定对应的开发板或模块。具体操作请参阅☞[板级配置说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/%E6%9D%BF%E7%BA%A7%E9%85%8D%E7%BD%AE%E8%AF%B4%E6%98%8E/)。

2、SDK代码中需将对应所使用开发板的板级配置为“1”，其余配置为“0”，如下图：

![user_code.bin 生成](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-7.png)

2. 编译完成后，在 `projects\offline_asr_pro_sample\firmware\user_code` 目录下生成 `user_code.bin`（见图 4-3）。

![user_code.bin 生成](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-5.png)

图4-3 user\_code.bin 生成

---

#### 步骤 3：生成其余四个 bin 文件

以下提供两种生成方式（选其一即可）：

**方式 1：在 VS Code 中直接生成**

1. 在 VS Code 中运行 `合成分区bin文件.bat`（见图 4-4）。

![VS Code 终端中直接合成](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-77.png)

图4-4 VS Code 终端中直接合成

2. 在VS Code 终端中显示的运行结果如图 4-5 所示。

![运行结果](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-78.jpg)

图4-5 运行结果

**方式 2：双击脚本文件**

1. 进入 `projects\offline_asr_pro_sample\firmware` 目录（图 4-6）。

![firmware文件目录](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-6.png)

图4-6 firmware文件目录

2. 双击 `合成分区bin文件.bat`，脚本开始运行后将会出现如图4-7所示的界面，脚本执行完毕后将在 `asr`、`dnn`、`user_file`、`voice` 子目录中自动生成对应的 bin 文件。

![软件运行](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-67.png)

图4-7 合成分区bin文件脚本运行界面

注意

**处理完成后cmd.exe自动退出，如果没有自动退出，生成.bin文件失败，请检查asr、dnn、user\_file、voice下文件或文件名是否正确；**

---

#### 步骤 4：合并五个 bin 文件

启动 `PACK_UPDATE_TOOL.exe` 软件，推荐两种方式（任选其一即可）：

**方式 1：在 VS Code 中启动**

1. 在 VS Code 中启动打包升级工具 `PACK_UPDATE_TOOL.exe`（见图 4-8）。

![启动升级工具](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-79.png)

图4-8 VS Code 中启动打包工具

**方式 2：双击批处理脚本**

1. 进入 `projects\offline_asr_pro_sample\firmware` 目录，如图4-6所示。
2. 双击 `打包升级.bat`，脚本将自动调用 `PACK_UPDATE_TOOL.exe`。

---

首次启动 `PACK_UPDATE_TOOL.exe` 时，会弹出芯片系列选择框（图 4-10）。请选择 “CI13XX 系列” “CI1303”芯片后继续。

![串口升级工具选择界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-8.png)

图4-10 串口升级工具选择界面

![串口升级工具主界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-9.png)

图4-11 串口升级工具主界面

串口升级工具主界面介绍：
- **CI13XX 系列芯片** —— 当前是CI13XX系列芯片使用模式
- **CI1303** —— 当前选择的芯片型号是CI1303
- **English** —— 切换界面语言（中 / 英）
- **固件打包** —— 进入固件打包界面
- **固件升级** —— 进入固件升级界面
- **F1** —— 打开帮助文档

点击串口升级工具主界面的（固件打包）按钮，界面如下图4-12所示：

![串口升级工具固件打包界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-10.png)

图4-12 串口升级工具固件打包界面

根据各 bin 文件实际大小检查并调整五个分区的“预留大小”。示例：
- `user_code.bin` 如果实际大小 0x232A3 字节，原预留 0x23000，应改为 ≥ 0x24000
- `voice.bin` 如果实际大小 0xD80B9 字节，原预留 0xF0000，应下调至接近且不小于实际值，例如 0xD9000，以避免浪费 Flash 空间。

> **注意：** 预留大小必须以 0x1000 为步进。

单击“打包固件”按钮后，系统会弹出“固件已生成”提示框。点击“确定”后，固件便生成完毕，并保存在 `projects\\offline_asr_pro_sample\\firmware` 目录下，文件名由软件名称与版本号自动拼接，例如 `Firmware_V200.bin`（图 4-13）。

![串口升级工具打包成功界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-11.png)

图4-13 串口升级工具打包成功界面

---

#### 步骤 5：升级固件

升级前，请先确认以下事项：

1. 确认硬件连线正确（参见步骤 1）；
2. 确认 USB Type-C 数据线或UART串口调试工具已连接至 PC，且 PC 已识别对应 COM 端口；
3. 确认固件成功生成；
4. 确认麦克风和喇叭已正确连接至开发板。

点击图4-10或图4-11界面的固件升级按钮，界面如下图4-14所示：

![串口升级工具升级界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-12.png)

图4-14 串口升级工具升级界面

1. 选择步骤 4 生成的固件文件，或启英泰伦官方提供的固件；
2. 选择正确的 COM 端口；
3. 复位开发板（“按供电按钮开关断电重启”或“有带复位按键的开发板型号可按复位键”，图2-3所示），升级工具将自动开始升级；完成后状态栏提示 `Update successful`。

> **提示：** 更多固件制作信息请参阅 ☞[《命令词和固件制作指南》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/%E5%91%BD%E4%BB%A4%E8%AF%8D%E5%92%8C%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%E6%8C%87%E5%8D%97/)，或在图4-10起始页按 **F1** 查看帮助。

---

#### 步骤 6：验证固件

固件升级完成后，请给开发板重新上电或按复位键。`offline_asr_pro_sample` 工程启动后将播放启动提示音。提示音结束后，说唤醒词“智能管家”应得到响应并播报“你好”，即表示升级成功。若想更换命令词/播报音，请参考☞[语音固件开发指南](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B%E6%8C%87%E5%BC%95/)

> **提示：若未听到语音播报，请参阅第 5 章“调试”排查原因。**

---

## 5. SDK调试

SDK 内置基于 **Log** 的调试机制，可跟踪代码执行流程及关键数据。

### 5.1. Log机制

开发者可通过串口输出 **Log** 信息，追踪应用程序运行状态。

#### 5.1.1. Log 输出管脚配置

默认情况下，`UART0_TX` 为 Log 串口的输出引脚。若需改用其他串口，请在 `user_config.h` 中配置宏 `CONFIG_CI_LOG_UART`（图 5-1）。

![offline_asr_pro_sample工程配置Log输出配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-13.png)

图5-1 offline\_asr\_pro\_sample工程配置Log输出管脚

建议仅在工程路径下的 `user_config.h` 中修改配置，避免直接修改 `sdk_default_config.h`。

#### 5.1.2. Log 串口工具配置

PC 端串口工具配置示例如下。端口号根据系统为UART串口调试工具分配的COM端口进行选择，波特率设置为921600，“日志输出”选项前打勾：

![PC串口工具配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/img/CI130X_SDK_Quick_Start-114.png)

图5-2 PC串口工具配置

> **注意：** 串口输出包含中文，请使用支持 UTF-8 字体的串口工具。

推荐使用 [SecureCRT](https://www.vandyke.com/products/securecrt/) 等专业串口工具。

#### 5.1.3. Log 打印通用接口

通用 Log 打印接口：

```
mprintf(fmt, args...); // 用法与 printf 相同
```

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

> **注意：** 若使用表 5-1 所示封装接口，需在配置中将 `CONFIG_CI_LOG_EN` 设为 **1**。

#### 5.1.5. Log 打印长度配置

```
#define UART_LOG_BUFF_SIZE          512
```

默认打印长度 512 字节，可通过宏 `UART_LOG_BUFF_SIZE` 调整。

---

## 6. 结语与支持

至此，您已完成 CI13XX 系列 SDK 的环境搭建、固件升级及基本调试流程。后续可根据业务需求，在示例工程基础上进行二次开发，或参考组件源码快速集成更多功能。