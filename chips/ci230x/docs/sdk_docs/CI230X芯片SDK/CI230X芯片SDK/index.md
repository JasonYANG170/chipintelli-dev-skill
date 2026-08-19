<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X%E8%8A%AF%E7%89%87SDK/ -->

# SDK概述

---

## 1. 概述

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/img/ci230x_sdk_%E6%A1%86%E6%9E%B6.png)

图 1-1

* CI230X系列芯片SDK分为语音部分和WIFI部分，两部分SDK均使用FreeRTOS系统：

**Part1**：语音部分SDK分为IOT应用SDK(**ci230x\_aiot\_offlineASR\_sdk\_release**)和离在线SDK(**ci230x\_aiot\_offlineASR&onlineASR\_sdk\_release**)，IOT SDK只适用于(离线识别+云端IOT)应用，该SDK适用CI2305/CI2306芯片开发；离在线SDK适用于(离线识别+在线识别)应用，该SDK只能用CI2306芯片开发。

**Part2**：WIFI部分IOT和离在线统一为一套SDK(**ci230x\_aiot\_offlineASR\_sdk\_release**)，通过宏来控制具体使能纯IOT功能还是离在线功能。

CI230X系列芯片SDK支持对芯片wifi部分和语音部分单独进行OTA升级，SDK均使用vscode来进行开发，SDK可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载最新版本进行开发。

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/img/SDK%E4%B8%8B%E8%BD%BD.png)

## 2. SDK开发流程

在进行SDK开发之前，需要确认开发的是离在线方案，还是离线+IOT方案，方案选型确定以后再下载对应的SDK，开发流程参考图2-1；开发视频请参考☞[CI230X系列芯片开发指导视频](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/)

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/img/CI230X%E5%BC%80%E5%8F%91%E8%A7%86%E5%B1%8F%E6%95%99%E7%A8%8B.png)

**公有云接入**：用户如果接入腾讯IOT类的公有云，深度开发语音SDK即可，定制语音部分应用；启英泰伦已对WIFI部分接入公有IOT云完成定制并编译出固件；用户只需烧录WIFI固件和云端接入鉴权文件，再配合开发的语音固件即可完成IOT和离在线方案的定制。以腾讯IOT接入为例，用户可以烧录标注固件: ☞[腾讯IOT-CJSON透传WIFI固件.zip](https://aiplatform.chipintelli.com/attachment)

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/img/%E8%85%BE%E8%AE%AFIOT-CJSON%E6%A0%87%E5%87%86%E5%9B%BA%E4%BB%B6.png)

**私有云接入**：用户如果需要接入私有云，需要对WIFI SDK 进行熟悉，主要是网络部分mqtt接口，http下载数据接口，以及用户部分代码部分进行熟悉后接入私有云，再配合开发的语音固件即可完成IOT和离在线方案的定制。

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/img/CI230X_sdk%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B.png)

## 3. 芯片框图

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/img/CI230X_chip_platform.png)

图 3-1

## 4. SDK版本介绍

### 4.1 **ci230x\_aiot\_offlineASR\_sdk\_release**

主要针对离线+IOT的应用场景，例如语音插座，红外遥控器，灯控等。

**SDK支持的音频前端算法有：**

* ASR 语音识别，☞[《语音识别使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* Denoise 降噪，☞[《语音降噪使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* AEC 回声消除，☞[《回声消除使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* DOA 波达方向估计，☞[《DOA使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/DOA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* Dereverb 去混响，使用说明待完善补充…
* Beamforming 波束形成，使用说明待完善补充…
* BSS 盲源分离，使用说明待完善补充…

### 4.2 **ci230x\_aiot\_offlineASR&onlineASR\_sdk\_release**

主要针对离线识别+IOT+在线识别的应用场景，例如智能音响，智能空调等。

**SDK支持的音频前端算法有：**

* ASR 语音识别，☞[《语音识别使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* AEC 回声消除，☞[《回声消除使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* BSS 盲源分离，使用说明待完善补充…

### 4.3 **ci230x\_aiot\_offlineASR\_sdk\_release**

WIFI SDK支持WIFI+BLE蓝牙功能，BLE蓝牙只能用于配网功能，不能进行音频数据传输。

**WIFI SDK 框架图：**

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/img/SDK%E6%A1%86%E6%9E%B6%E5%9B%BE.png)

图 4-1