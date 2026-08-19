<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/CI231X%E8%8A%AF%E7%89%87SDK/ -->

# SDK概述

---

## 1. 概述

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/img/CI231X-SDK%E6%A1%86%E6%9E%B6.png)

图 1-1

CI231X系列芯片SDK使用FreeRTOS系统，SDK包含如下功能：

* 离线语音识别功能
* 蓝牙BLE功能
* 2.4G 透传功能

**注意：使用CI231X系列芯片开发产品时，BLE和2.4G支持动态切换。**

SDK可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载最新版本进行开发。

## 2. SDK版本介绍

### 2.1. CI231X\_SDK

用户可以使用该SDK进行离线语音识别+蓝牙BLE应用开发，如：离线语音识别+蓝牙BLE灯控产品、离线语音识别+蓝牙BLE风扇产品等；也可以使用该SDK进行离线语音识别+RF 2.4G透传产品开发，如：2.4G 遥控器等，支持的音频前端算法有：

* ASR 语音识别，☞[《语音识别使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* 命令词自学习功能，☞[《离线命令词自学习使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/%E7%A6%BB%E7%BA%BF%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* Denoise 降噪，☞[《语音降噪使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* Aec 回声消除，☞[《回声消除使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)