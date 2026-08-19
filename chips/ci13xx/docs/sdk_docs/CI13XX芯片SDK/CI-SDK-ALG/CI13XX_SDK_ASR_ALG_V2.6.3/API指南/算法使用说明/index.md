<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# CI13XX算法SDK2.6.3开发流程

---

## 1. 概述

**CI13XX\_SDK\_ASR\_ALG\_V2.6.3版本主要包含了以下算法功能和算法**:

| 算法名称 | 算法说明 |
| --- | --- |
| ASR | 单双mic麦语音识别，固定词条或者自然说 |
| VPR | 基于特定人的声纹识别 |
| WMAN\_VPR | 男女声纹检测功能 |
| SED\_CRY | 哭声检测功能 |
| SED\_SNORE | 鼾声检测功能 |
| DENOISE\_NN | 基于识别的深度降噪功能 |
| DOA | 双麦声源定位功能 |
| CWSL | 自学习功能 |
| DERVERB | 双麦降混响 |
| AEC | 回声消除 |
| CWSL\_AEC | 自学习加回声消除(当前只支持单mic) |
| TTS | 文本转语音功能(只支持中文、数字、字母, 不支持英文) |
| BF | 双麦语音增强功能 |
| AI\_DOA\_AEC | 双麦声源定位+回声消除功能(需外挂codec-推荐7243e) |
| DEREVERB\_AEC | 双麦降混响+回声消除功能(需外挂codec) |
| BF\_AEC | 双麦语音增强+回声消除功能(需外挂codec) |
| DOA\_DEREVERB | 双麦声源定位+双麦降混响功能(仅双mic可用) |
| BF\_DEREVERB | 双麦语音增强+双麦降混响功能(仅双mic可用) |
| CWSL\_DOA | 自学习+双麦声源定位功能(仅双mic可用) |
| ANY\_MIC\_AEC | 任意MIC识别+回声消除功能(需外挂codec) |
| CWSL\_DOA\_AEC | 自学习+双麦声源定位+回声消除功能(需外挂codec) |
| CWSL\_ANY\_MIC\_AEC | 自学习+任意MIC识别+回声消除功能(需外挂codec) |
| DOA\_DEREVERB\_AEC | 双麦声源定位+双麦降混响+回声消除功能(需外挂codec) |
| PWK | 声音能量值值计算功能，区分目标声音距离 |
| ALC | 自动增益控制 |

注意

1. 使用声纹注册、男女声纹检测、哭声鼾声检测、深度降噪、声源定位、语音合成算法时，在firmware\dnn文件中需搭配该算法的前端算法模型使用。

## 2. 算法功能组合说明

| 开启算法 | 说明 |
| --- | --- |
| ASR | 只开识别，不开其他算法 |
| ASR+声纹 | 同时开启识别加VPR声纹注册功能 |
| ASR+男女声纹 | 同时开启识别加WMAN\_VPR男女声纹识别功能 |
| ASR+深度降噪 | 同时开启识别加DENOISE\_NN深度降噪功能 |
| ASR+声源定位 | 同时开启识别加DOA声源定位功能 |
| ASR+自学习 | 同时开启识别加CWSL自学习功能 |
| ASR+降混响 | 同时开启识别加DERVERB降混响功能 |
| ASR+回声消除 | 同时开启识别加AEC回声消除功能 |
| ASR+自学习+回声消除 | 同时开启识别加自学习加回声消除 |
| ASR+双麦语音增强 | 同时开启识别加双麦语音增强 |
| ASR+声源定位+回声消除 | 同时开启识别加声源定位加回声消除 |
| ASR+降混响+回声消除 | 同时开启识别加降混响加回声消除 |
| ASR+双麦语音增强+回声消除 | 同时开启识别加双麦语音增强加回声消除 |
| ASR+双麦声源定位+降混响 | 同时开启识别加双麦声源定位加降混响 |
| ASR+双麦语音增强+降混响 | 同时开启识别加双麦语音增强加降混响 |
| ASR+自学习+双麦声源定位 | 同时开启识别加自学习加双麦声源定位 |
| ASR+任意MIC识别+回声消除 | 同时开启识别加任意MIC识别加回声消除 |
| ASR+自学习加双麦声源定位+回声消除 | 同时开启识别加自学习加双麦声源定位加回声消除 |
| ASR+声源定位+降混响 | 同时开启识别加声源定位加降混响 |
| ASR+自学习+双麦声源定位+回声消除 | 同时开启识别加自学习加双麦声源定位加回声消除 |
| 语音合成 | TTS语音合成不支持识别 |
| 哭声检测 | SED\_CRY哭声检测不支持识别和其他算法功能 |
| 鼾声检测 | SED\_SNORE鼾声检测不支持识别和其他算法功能 |

---

注意

除了上表算法组合功能，不支持其他组合，请勿随意组合算法功能，否则会出现sdk编译异常或者运行故障。

## 3. 算法功能使用说明

**3.1 在CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件定义了CI\_ALG\_TYPE变量，通过修改该变量的值，选择使用对应的算法功能，makefile中会自动根据CI\_ALG\_TYPE的配置去定义和使能对应的宏，用户不需要再去重复定义使能相关宏参数；CI\_ALG\_TYPE默认等于USE\_NULL。例如：应用中只需要用到ASR识别功能，不需要用到其他算法功能，配置如下图：**
![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/CI_ALG_TYPE.png)
**如果需要用自学习+AEC算法，配置如下图：**
![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/CI_ALG_CWSL_AEC.png)

**3.2 CI\_ALG\_TYPE变量和算法功能对应关系表如下：**

| CI\_ALG\_TYPE | 算法功能说明 |
| --- | --- |
| USE\_NULL | 只开语音识别，不开其他算法 |
| USE\_VPR | 开启识别+声纹注册功能 |
| USE\_WMAN\_VPR | 开启识别+男女声纹识别 |
| USE\_SED\_CRY | 开启哭声检测，不支持语音识别 |
| USE\_SED\_SNORE | 开启鼾声检测，不支持语音识别 |
| USE\_TTS | 开启文字转语音，不支持语音识别 |
| USE\_DENOISE\_NN | 开启识别+深度降噪 |
| USE\_AI\_DOA | 开启识别+声源定位 |
| USE\_CWSL | 开启识别+自学习 |
| USE\_DEREVERB | 开启识别+降混响 |
| USE\_AEC | 开启识别+回声消除 |
| USE\_CWSL\_AEC | 开启识别+自学习+回声消除 |
| USE\_BF | 开启识别+双麦语音增强 |
| USE\_AI\_DOA\_AEC | 开启识别+声源定位+回声消除 |
| USE\_DEREVERB\_AEC | 开启识别+降混响+回声消除 |
| USE\_BF\_AEC | 开启识别+双麦语音增强 |
| USE\_DOA\_DEREVERB | 开启识别+声源定位+降混响 |
| USE\_BF\_DEREVERB | 开启识别+双麦语音增强+降混响 |
| USE\_CWSL\_DOA | 开启识别+自学习+声源定位 |
| USE\_CWSL\_DOA\_AEC | 开启识别+自学习+声源定位+回声消除 |
| USE\_CWSL\_ANY\_MIC\_AEC | 开启识别+自学习+任意mic识别+回声消除 |
| USE\_DOA\_DEREVERB\_AEC | 开启识别+声源定位+降混响+回声消除 |

---

## 4. 模型ID定义

开启算法功能需使用不同的前端算法模型，各个算法模型对应ID如下表：

| 模型ID | 模型类型 | 对应算法 |
| --- | --- | --- |
| 60001 | 声纹识别模型 | 声纹注册算法 |
| 60002 | 哭声检测模型 | 哭声检测算法 |
| 60003 | NN深度降噪模型 | 深度降噪算法 |
| 60004 | DOA声源定位模型 | 声源定位算法 |
| 60005 | 鼾声检测模型 | 鼾声检测算法 |
| 60008 | 男女声纹检测模型 | 男女声纹检测算法 |
| 60009 | TTS语音合成模型 | TTS算法(需要同时用60009和60010) |
| 60010 | TTS语音合成模型 | TTS算法(需要同时用60009和60010) |

---

## 5. SDK开发包下载

**5.1 注册并登录AI开发平台：[https://aiplatform.chipintelli.com](https://aiplatform.chipintelli.com/)**

**5.2 获取算法SDK CI13XX\_SDK\_ASR\_ALG\_VXX的软件开发包：<https://aiplatform.chipintelli.com/attachment>， (*若有新版本，请使用最新版本的SDK*),如下图：**
![SDK下载](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/SDK%E4%B8%8B%E8%BD%BD.png)