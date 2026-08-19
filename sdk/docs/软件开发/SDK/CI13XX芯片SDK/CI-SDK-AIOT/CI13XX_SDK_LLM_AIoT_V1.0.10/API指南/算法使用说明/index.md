<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# CI13XX离在线大模型对话SDK\_V1.0.10开发流程

## 1. 概述

**离在线大模型对话SDK(CI13XX\_SDK\_LLM\_AIoT\_V1.0.10) 相对于算法SDK新增功能有：录音音频数据上传(压缩/非压缩)、SPEEX和OPUS编解码、播放网络音频数据(MP3/PCM)、本地VAD端点检测；由于增加功能加大了内存和算力消耗，故在算法SDK版本上做了功能和组合裁剪，当前只支持下面的算法组合**:

| 算法名称 | 算法说明 |
| --- | --- |
| USE\_AI\_DOA\_AEC\_DENOISE\_NN | 双麦声源定位+回声消除+深度降噪(需外挂codec) |
| USE\_AEC\_DENOISE\_NN | 回声消除+深度降噪 |
| USE\_CWSL\_AEC\_DENOISE\_NN | 自学习+回声消除+深度降噪(需外挂codec) |

## 2. 算法功能组合说明:

| 开启算法 | 说明 |
| --- | --- |
| ASR+声源定位+回声消除+NN降噪 | 同时开启识别加声源定位+回声消除+深度降噪 |
| ASR+回声消除+NN降噪 | 同时开启识别+回声消除+深度降噪 |
| ASR+自学习+回声消除+NN降噪 | 同时开启识别加自学习+回声消除+深度降噪 |

注意

除了上表算法组合功能，不支持其他组合，请勿随意组合算法功能，否则会出现sdk编译异常或者运行故障。

## 3. 算法功能使用说明

**3.1 在CI13XX\_SDK\_LLM\_AIoT\_Vx.x.x\project\_file\makefile文件定义了CI\_ALG\_TYPE变量，通过修改该变量的值，选择使用对应的算法功能，makefile中会自动根据CI\_ALG\_TYPE的配置去定义和使能对应的宏，用户不需要再去重复定义使能相关宏参数；CI\_ALG\_TYPE默认等于USE\_AEC\_DENOISE\_NN。例如：应用中如果需要用到自学习+回声消除+降噪，CI\_ALG\_TYPE配置如下图：**
![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/CI_ALG_TYPE.png)

**3.2 CI\_ALG\_TYPE变量和算法功能对应关系表如下：**

| CI\_ALG\_TYPE | 算法功能说明 |
| --- | --- |
| USE\_AI\_DOA\_AEC\_DENOISE\_NN | 开启识别+声源定位+回声消除+NN降噪 |
| USE\_AEC\_DENOISE\_NN | 开启识别+回声消除+NN降噪 |
| USE\_CWSL\_AEC\_DENOISE\_NN | 开启识别+自学习+回声消除+NN降噪 |

## 4.模型ID定义：

**开启算法功能需使用不同的前端算法模型，各个算法模型对应ID如下表：**

| 模型ID | 模型类型 |
| --- | --- |
| 60003 | NN深度降噪模型 |
| 60004 | DOA声源定位模型 |

## 5. SDK开发包下载:

**5.1 注册并登录AI开发平台：[https://aiplatform.chipintelli.com](https://aiplatform.chipintelli.com/)**

**5.2 获取算法SDK CI13XX\_SDK\_ASR\_ALG\_VXX的软件开发包：<https://aiplatform.chipintelli.com/attachment>， (*若有新版本，请使用最新版本的SDK*),如下图：**
![SDK下载](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/aiot_sdk%E4%B8%8B%E8%BD%BD.png)

## 6. WiFi/4G芯片和启英语音芯片对接离在线方案:

**6.1 上传和播放音频参数+交互协议+指令说明：**
请采参考离在线大模型对话SDK对接协议说明文档☞[启英泰伦离在线大模型对话SDK对接参数及协议说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/pdf/%E5%90%AF%E8%8B%B1%E6%B3%B0%E4%BC%A6%E7%A6%BB%E5%9C%A8%E7%BA%BF%E5%BA%94%E7%94%A8%E5%AF%B9%E6%8E%A5%E8%AF%B4%E6%98%8E-V2.3.pdf)

**6.2 方案对接说明：**
离在线大模型对话SDK前端方案说明文档☞[启英泰伦离在线大模型对话对接方案说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/pdf/%E5%90%AF%E8%8B%B1%E6%B3%B0%E4%BC%A6%E7%A6%BB%E5%9C%A8%E7%BA%BF%E5%A4%A7%E6%A8%A1%E5%9E%8B%E6%96%B9%E6%A1%88%E8%AF%B4%E6%98%8E-V1.13.pdf)