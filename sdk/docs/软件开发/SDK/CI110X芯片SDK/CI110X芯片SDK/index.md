<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/CI110X%E8%8A%AF%E7%89%87SDK/ -->

# SDK概述

---

## 1. 概述

为了应对不同的应用场景，同时简化SDK的配置和使用复杂度，目前SDK提供3个版本。分别是：

* 纯离线SDK：CI110X\_SDK\_ASR\_Offline
* 算法SDK：CI110X\_SDK\_ALG\_Application
* 离在线SDK：CI110X\_SDK\_Combine\_Cloud

上述SDK可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载并使用。

---

## 2. SDK版本介绍

### 2.1. CI110X\_SDK\_ASR\_Offline

主要针对纯离线简单应用场景，例如语音插座等，支持的音频前端算法有：

* AEC 回声消除，[《回声消除使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* Denoise 降噪，[《语音降噪使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)

### 2.2. CI110X\_SDK\_ALG\_Application

主要针对需要更多音频前端算法，处理大噪声或特殊算法应用的场景，例如语音跑步机、抽油烟机等，支持的前端算法有：

* AEC 回声消除，[《回声消除使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* Denoise 降噪，[《语音降噪使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* DOA 方位角度估计，[《DOA使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/DOA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* 语音增强，[《语音增强使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E5%A2%9E%E5%BC%BA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* dereverb 降混响，[《语音降混响使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E9%99%8D%E6%B7%B7%E5%93%8D%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* 离线命令词自学习 ,[《离线命令词自学习》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E7%A6%BB%E7%BA%BF%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)

### 2.2. CI110X\_SDK\_Combine\_Cloud

主要针对本地识别与云端识别结合的应用场景，例如智能音箱等，支持的前端算法有：

* AEC 回声消除，[《回声消除使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* Denoise 降噪，[《语音降噪使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* DOA 方位角度估计，[《DOA使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/DOA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* 语音增强，[《语音增强使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E5%A2%9E%E5%BC%BA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)
* dereverb 降混响，[《语音降混响使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E9%99%8D%E6%B7%B7%E5%93%8D%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)