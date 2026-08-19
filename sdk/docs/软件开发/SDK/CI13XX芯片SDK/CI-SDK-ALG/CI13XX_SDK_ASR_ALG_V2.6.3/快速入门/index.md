<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/ -->

# 快速入门

## **概述**

本文旨在帮助开发者采用CI13XX系列语音AI芯片☞[离线语音算法SDK（CI-SDK-ASR-ALG）](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/)来快速上手开发一个离线语音识别+语音算法的固件。本文内容包括：1）CI-IDE （启英泰伦集成开发环境）的搭建和SDK的下载；2）利用已搭建的IDE环境和SDK，结合启英泰伦的☞[语音AI平台](https://aiplatform.chipintelli.com/)进行一个语音识别固件代码的开发；3）利用 IDE及SDK 所提供的工具进行代码编辑、编译链接、固件生成与下载调试，然后配合 HDT （硬件工具链）可实现固件下载到开发板，以及板极通信验证、日志 LOG 打印与音频数据分析。完成从环境搭建到工程编译再到上板验证的语音识别全流程开发。

备注

* **离线语音算法算法 SDK （语音算法软件开发包，ALG ASR Software Development Kit）** 支持启英泰伦单双麦离线语音识别+语音算法。是用于提供开发启英泰伦CI13XX系列AI语音芯片所使用的软件库和源代码的工具包，并提供了丰富的API(Application Programming Interface,应用程序编程接口)和PACK\_UPDATAE\_TOOL固件打包升级工具等；启英泰伦官方针对不同系列芯片发布了各自系列的☞[离线语音算法SDK（CI-SDK-ASR-ALG）](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/)版本。
* **IDE（启英泰伦集成开发环境，Integrated Development Environment ）** 是用于提供程序开发环境的应用程序，一般包括代码编辑器、编译器、调试器和图形用户界面等工具。开发者需要通过官方推荐的集成开发环境 (IDE) 对启英泰伦软件开发包（语音算法SDK）进行编辑、编译、链接、调试等操作。
* **AI开发平台（启英泰伦集人工智能开发平台，Chipintelli AI Platform）** 是我司为用户提供的一整套基于我司智能语音芯片方案的功能开发及管理平台，平台网址为：☞<https://aiplatform.chipintelli.com/>。该平台包含了功能开发、开发资料、提交工单、帮助文档、样品采购等功能。开发者须先完成注册。
* **HDT（硬件开发工具链，Hardware Development Toolchain）** 是一套用于设计、开发、调试和验证的硬件工具集合，涵盖嵌入式系统开发的整个流程。

## **准备工作**

本节以 CI1303 作为示例，帮助开发者快速搭建开发环境。

下图为CI13XX系列语音AI芯片开发 IDE 搭建与程序上板示意图：

![CI 芯片开发 IDE 搭建与程序上板示意图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/img/index-1.png)

### **HDT（硬件开发工具链）准备**

1. **CI1303** 开发板套件（☞[CI1303 开发板套件说明](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI1301%26CI1302%26CI303%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/)）

备注

* CI1303 开发板包含：CI-D02GS02S模块 + CI-B02-MB开发板底板
* CI1303 开发板套件包含：CI-D03GS02S模块 + CI-B02-MB开发板底板 + 麦克 + 喇叭 + 串口调试工具 + 采音板 + 杜邦线 + USB 数据线
* 在☞[启英商城](http://mall.chipintelli.com)选购开发板时，若选择 “开发板套件”，其已包含全套 **硬件开发工具链** （串口调试工具、采音板）与 **硬件连接线** （杜邦线、USB数据线）。若开发者已持有一套开发套件，后续在更换芯片模块时，仅需单独采购其他型号模块，无需再次重复购置开发套件

2. **USB 数据线** （USB A 转 USB Type-C，购买开发板套件时配套有USB 数据线，无需单独购买；若需单独购买请点击☞[USB数据线获取](http://mall.chipintelli.com/chip?product_category=18&brd=1)）
3. **串口调试工具** （用于接收开发板打印调试信息或验证串口通信协议，购买开发板套件时配套有串口调试工具，无需单独购买；若需单独购买请点击☞[串口调试工具获取](http://mall.chipintelli.com/chip?product_category=18&brd=1)）
4. 电脑（Windows 10 及以上系统）

### **离线语音SDK下载**

SDK的下载有两种获取方式，方式一是通过启英泰伦 **语音AI平台** 中的”语音识别固件及SDK开发”功能板块，在完成无代码固件开发流程配置后同步勾选“SDK下载选项”并最终输出SDK包；方式二则是直接点击官方链接下载具体版本的标准SDK，配合文档中心的[API 参考](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/)用于深度编程开发。

* 方式一：通过AI开发平台进行深度定制SDK或直接开发和生成语音识别固件，操作步骤请点击[语音AI平台产品开发流程指引](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B%E6%8C%87%E5%BC%95/) ；
* 方式二：直接点击下载 ☞[CI13XX\_SDK\_ASR\_ALG\_V2.6.3](https://www.chipintelli.com/zh-cn/page/152.html)。

备注

**方式二** 需要开发者具有一定的嵌入式编程基础。

### **IDE搭建准备**

开发者在IDE（集成开发环境）上使用 离线语音算法SDK，请下载并安装以下软件：

1. ☞ [**Visual Studio Code**](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E8%B5%84%E6%BA%90/Visual%20Studio%20Code/) 轻量级代码编辑器；
2. ☞ [**GCC编译工具链**](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E8%B5%84%E6%BA%90/gcc/) 编译链接工具，用于编译 CI13XX 系列芯片工程代码；
3. **CI TOOL** 插件安装到轻量级代码编辑器(VS Code)上使用，该插件提供了运行 GCC工具链 等功能的脚本；

备注

**CI TOOL** 插件包含在 **离线语音算法SDK** 软件开发包的tools目录中，无需单独下载

## **安装**

为安装所需软件，启英泰伦提供了以下方法。

**IDE 搭建**

启英泰伦集成开发环境由以下几部分组合而成：

* VSCode + GCC工具链 + CI TOOL + PACK UPDATE TOOL

请点击☞[IDE 搭建与使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)，按照详细步骤操作。

**HDT 安装**

启英泰伦硬件开发工具链由以下几部分组合而成，安装步骤请点击备注中的使用说明：

* 串口调试工具 + 采音板

备注

* ☞请点击查看[UART串口调试工具使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E8%B5%84%E6%BA%90/UART%E4%B8%B2%E5%8F%A3%E8%B0%83%E8%AF%95%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* ☞请点击下载[采音板操作说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E8%B5%84%E6%BA%90/%E9%87%87%E9%9F%B3%E6%9D%BF%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* USB数据线和杜邦线等硬件连接线不算作启英泰伦HDT（硬件开发工具链）

## **编译第一个语音识别固件**

方式一：基于AI开发平台进行无代码开发，请点击[语音AI平台产品开发流程指引](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B%E6%8C%87%E5%BC%95/)；

提示

☞开发者可点击前往[视频教程](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E7%AF%87/%E8%BD%AF%E4%BB%B6%E7%AF%8706%EF%BC%9A%E5%B9%B3%E5%8F%B05%E5%88%86%E9%92%9F%E5%AE%8C%E6%88%90%E5%8D%95%E9%BA%A6%E7%A6%BB%E7%BA%BF%E5%9B%BA%E4%BB%B6%EF%BC%88%E7%AC%AC%E4%B8%80%E8%AE%B2%EF%BC%9A%E5%88%B6%E4%BD%9C%E5%AE%9A%E5%88%B6%E5%8D%8F%E8%AE%AE%E7%9A%84%E5%9B%BA%E4%BB%B6%EF%BC%89/)观看详细操作步骤

方式二：基于离线语音SDK编程开发

1. 当开发者已经搭建好IDE（集成开发环境 ），请按照☞[SDK 快速开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/)编译第一个工程；
2. 在第一个工程成功运行在开发板上后，开发者要根据具体项目需求更改唤醒词和命令词时，则需按照☞[命令词和固件制作指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/%E5%91%BD%E4%BB%A4%E8%AF%8D%E5%92%8C%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%E6%8C%87%E5%8D%97/)中的步骤进行作词操作；
3. 打包成最终可烧录下载到开发板中的.bin固件，则需按照☞[SDK 快速开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/)中的“将5个bin文件打包成一个bin文件”等具体步骤进行操作。

提示

☞开发者可点击前往[视频教程](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E7%AF%87/%E8%BD%AF%E4%BB%B6%E7%AF%8712%EF%BC%9A%E5%9F%BA%E4%BA%8ESDK%E5%BC%80%E5%8F%91%E5%9B%BA%E4%BB%B6%EF%BC%88%E7%AC%AC%E4%B8%80%E8%AE%B2%EF%BC%9A%E5%9F%BA%E4%BA%8ESDK%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%EF%BC%89/)观看详细操作步骤

## **固件下载和体验测试**

1. 开发套件搭建：按照链接☞[SDK 快速开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/)中的“硬件连线”步骤，将开发板、麦克、喇叭、电脑等按连接说明连接好；
2. 固件下载：点击☞[SDK 快速开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/)中的“升级固件”步骤，将语音识别固件下载到开发板中；
3. 如何体验测试：点击☞[SDK 快速开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/)中的“验证固件”步骤，让开发者知道开机播报音、唤醒词、命令词、唤醒时间，唤醒持续时间 等概念，体验离线语音算法功能的魅力。