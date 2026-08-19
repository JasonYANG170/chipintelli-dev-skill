<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/tuya_iot/ -->

# CI230X系列芯片-涂鸦IOT方案介绍

## 1. 方案介绍

### 1.1 功能概述

启英泰伦为离线+IOT方案专业定制的CI2305芯片，该芯片具有高性能，高扩展性，低成本的特点，集成了离线ASR+WIFI+蓝牙BLE功能，用户可以利用该芯片快速开发离线+IOT方案；结合离线语音识别快，高识别率的优点，配合云端和手机APP（小程序）在线控制功能，给用户带来全新的端云融合的产品体验，方案SDK使用启英泰伦发布的 ci230x\_aiot\_offlineASR\_sdk\_release 和 ci230x\_aiot\_wifi\_combo\_sdk\_release ，下面将基于CI2305芯片+涂鸦IOT云来介绍方案。

### 1.2 方案框图

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/TUYA_IOT_%E6%A1%86%E6%9E%B6%E5%9B%BE.png)

图 1-1

---

### 1.3 离线+IOT方案有如下功能和特性

* 设备支持本地识别功能和本地控制功能，当本地识别指令后可以控制设备，播报语音，指令上传到云端同步设备状态。
* 设备支持云端远程控制功能，远程查询设备状态，在线对设备进行OTA升级；同时设备端也可以接收云端其它自定义功能，如闹钟，定时控制等功能，设备端支持在线获取云端红外码库的功能，云端丰富的红外码库支持市面上绝大部分红外设备（如空调，洗衣机，风扇等）。
* 设备设置支持蓝牙BLE配网。
* 设备支持涂鸦APP进行控制，同时设备可以和云端，小程序端进行状态实时同步。

### 1.4 离线+涂鸦IOT红外遥控器方案

本方案为红外遥控器的离线+IOT方案（用户也可根据需求应用在其他产品上，如红外灯控，风扇），方案集成单MIC/双MIC+AEC回声消除算法、盲源分离算法，dereverb 去混响算法，beamforming 波束形成算法，Denoise算法，算法可以根据需求进行配置和定制，具有良好的抗噪性能，demo中已提供的本地基础命令词如下（用户可自定义100+命令词）：

## 2. 离线+涂鸦IOT方案快速开发

### 2.1 涂鸦IOT云平台相关操作

* #### 注册登录涂鸦智能全球化智能平台

登录 ☞ [涂鸦智能全球化智能平台](https://auth.tuya.com/?from=http://iot.tuya.com/purchase/store?page=1&tag=&q=)，点击左下角 **立即注册**，注册一个涂鸦云开放平台开发者账号。

![腾讯云开发者](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E6%B6%82%E9%B8%A6%E4%BA%91%E5%B9%B3%E5%8F%B0%E6%8E%A5%E5%85%A5-1.png)

图 2-1

* #### 根据需求创建产品，涂鸦平台支持的产品类型很齐全，下面以创建插座为例来进行说明

![腾讯云开发者](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E6%B6%82%E9%B8%A6%E4%BA%91%E5%B9%B3%E5%8F%B0%E6%8E%A5%E5%85%A5-2.png)

图 2-2

![腾讯云开发者](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E6%B6%82%E9%B8%A6%E4%BA%91%E5%B9%B3%E5%8F%B0%E6%8E%A5%E5%85%A5-3.png)

图 2-3

![腾讯云开发者](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E6%B6%82%E9%B8%A6%E4%BA%91%E5%B9%B3%E5%8F%B0%E6%8E%A5%E5%85%A5-4.png)

图 2-4

![腾讯云开发者](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E6%B6%82%E9%B8%A6%E4%BA%91%E5%B9%B3%E5%8F%B0%E6%8E%A5%E5%85%A5-5.png)

图 2-5

* #### 定义DP点，涂鸦平台对设备功能通过dp的方式来管理，可以使用平台支持的标准功能，也支持自定义功能

![腾讯云开发者](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E6%B6%82%E9%B8%A6%E4%BA%91%E5%B9%B3%E5%8F%B0%E6%8E%A5%E5%85%A5-6.png)

图 2-6

![腾讯云开发者](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E6%B6%82%E9%B8%A6%E4%BA%91%E5%B9%B3%E5%8F%B0%E6%8E%A5%E5%85%A5-7.png)

图 2-7

![腾讯云开发者](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E6%B6%82%E9%B8%A6%E4%BA%91%E5%B9%B3%E5%8F%B0%E6%8E%A5%E5%85%A5-8.png)

图 2-8

* #### 涂鸦IOT相关项目开发说明：

​ 涂鸦平台接入需要涂鸦商务以及FAE做支持，如需开发涂鸦项目，请联系启英泰伦商务协调相关资源，涂鸦开发接入文档☞ [涂鸦智能全球化智能平台](https://developer.tuya.com/cn/docs/iot-device-dev/sdk-architecture?id=Kam0td6gat9n3)。

### 2.2 使能涂鸦IOT云

编译配置：开启涂鸦云使能 ：project\ci230x\_aiot\_wifi\_combo\_sdk\_release\CMakeLists.txt

```
set(CIAS_IOT_CLOUD_HUAWEI           0)     #华为云使能
set(CIAS_IOT_CLOUD_XIAOMI_ENABLE    0)     #小米云使能
set(CIAS_IOT_CLOUD_CI_ENABLE        0)     #启英云使能
set(CIAS_IOT_TENCENT_ENABLE         0)     #腾讯IOT云使能
set(CIAS_IOT_TVS_ENABLE             0)     #腾讯TVS使能
set(CIAS_IOT_TUYA_ENABLE            1)     #涂鸦云使能
set(CIAS_IOT_CLOUD_ALI_ENABLE       0)     #阿里云使能
```

### 2.3 调用涂鸦IOT云的入口函数

project\ci230x\_aiot\_wifi\_combo\_sdk\_release\app\usr\_tuya\main.c

```
  //Creat usr app task.
  if( creat_usr_app_task() != CIAS_OK)
  {
    LOG(LOG_LVL_ERROR, "creat_usr_app_task call error\r\n");
    return CIAS_FAIL;

  }
```

### 2.4 修改鉴权文件和产品ID

project\ci230x\_aiot\_wifi\_combo\_sdk\_release\app\cias\_aiot\_wifi\cias\_cloud\cloud\_tuya\apps\include\tuya\_device\_auth.h

```
#define PRODUCT_ID   "k8j4zo1061upmkbl"
#define PROD_UUID    "tuya544a3243ca246fa3"    
#define PROD_AUTH_KEY  "AQo131PftCynG1uRTLTVpJXEpu4z9AX0"
```

## 3. 主要功能API说明

### 3.1 涂鸦应用入口文件tuya\_main.c，关键接口函数调用流程

* main()->user\_main()->pre\_app\_init()->tuya\_iot\_init()->app\_init()->pre\_deivce\_init()->device\_init()

### 3.2 核心接口说明

* ###### pre\_app\_init()-用于系统初始的前置准备工作，此时涂鸦Device OS尚未启动
* ###### app\_init()-用于应用初始化，此时涂鸦Device OS已经初始化
* ###### pre\_device\_init()-设备功能启动前置准备工作，此时设备功能尚未启动
* ###### device\_init()-设备功能启动，完成之后设备进入正常工作状态,设备鉴权连接云端都在该函数内。

## 4. 涂鸦红外遥控器Demo说明

### 4.1 涂鸦红外遥控器Demo固件：

​ 涂鸦红外遥控器Demo固件地址如下：☞[涂鸦IOT-智能红外遥控器Demo固件.rar](https://aiplatform.chipintelli.com/attachment)

​ 用户拿到开发板如果没有预烧录固件，可以下载固件进行体验，涂鸦IOT云平台接入需要烧录鉴权文件，用户没有鉴权文件可以联系我司商务。鉴权文件具体烧录方法请参考：☞[开发环境搭建与使用-11章节](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)

### 4.2 Demo使用参考视频：☞[Demo使用视频](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88/%E6%96%B9%E6%A1%8803%EF%BC%9A%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%26%E6%B6%82%E9%B8%A6APP%20IoT%E6%96%B9%E6%A1%88/)

### 4.3 使用启英泰伦IOT开发板（注意：跳线帽按图中短接）

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/tuya_iot-2.png)

图 4-1 IOT开发板

### 4.4 设备配网与红外码库下载

本方案配网采用涂鸦智能APP蓝牙进行配网，首先添加设备，需唤醒设备（智能管家）并喊话”开始配网”，使设备进入配网状态，后续添加红外遥控设备并下载对应码库即可，使用文档可参考 ☞[涂鸦红外遥控器使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-DEMO/%E6%B6%82%E9%B8%A6%E7%BA%A2%E5%A4%96%E9%81%A5%E6%8E%A7%E5%99%A8%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E6%B6%82%E9%B8%A6%E7%BA%A2%E5%A4%96%E9%81%A5%E6%8E%A7%E5%99%A8%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)

### 4.5 涂鸦APP在线控制设备

涂鸦APP成功添加红外遥控设备后有专属的控制面板，并且离线语音控制和在线APP控制设备的状态能快速同步，空调控制面板如下图

![3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/tuya_iot-3.png)

图 4-2 控制面板

### 4.6 本地语音离线控制设备

本方案离线本地识别基于启英泰伦三代语音芯片，已提供的本地基础命令词如下（用户可自定义200+命令词）

| 唤醒词 | 智能管家 |  |  |  |  |
| --- | --- | --- | --- | --- | --- |
| **配网指令** | **开始配网** | **我要配网** | **关闭配网** |  |  |
| **命令词** | 打开空调 | 开空调 | 关闭空调 | 关空调 | 高速风 |
| 空调高速风 | 中速风 | 空调中速风 | 低速风 | 空调低速风 | 自动风 |
| 自动风速 | 十六度 | 十七度 | 十八度 | 十九度 | 二十度 |
| 二十一度 | 二十二度 | 二十三度 | 二十四度 | 二十五度 | 二十六度 |
| 二十七度 | 二十八度 | 二十九度 | 三十度 | 打开制冷模式 | 制冷模式 |
| 打开制热模式 | 制热模式 | 打开加热模式 | 加热模式 | 打开送风模式 | 送风模式 |
| 打开除湿模式 | 除湿模式 | 打开自动模式 | 自动模式 | 升高温度 | 降低温度 |
| 增大风速 | 增加风速 | 减小风速 | 减少风速 | 定时三十分钟 | 定时一小时 |
| 定时二小时 | 定时两小时 | 定时三小时 | 定时四小时 | 定时五小时 | 定时六小时 |
| 定时七小时 | 定时八小时 | 定时九小时 | 定时十小时 | 定时十一小时 | 定时十二小时 |
|  |  |  |  |  |  |