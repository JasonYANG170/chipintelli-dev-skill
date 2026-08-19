<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# 算法使用说明

---

## 1. 概述

CI13XX系列芯片具有较强的CPU运算能力，支持硬件NN加速，以及充足的RAM。能够完成语音识别、声音处理等算法。使用者可以根据不同产品需求进行合适的选择。具体使用方式及适用场景可参考文档中相关章节。主要包含了以下功能：

| 算法名称 | 算法说明 |
| --- | --- |
| ASR(语音识别) | ☞[语音识别使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/) |
| AEC(回声消除) | ☞[回声消除使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%ABSDK-CI13XX_SDK_ASR_Offline/) |
| CWSL(命令词自学习) | ☞[离线命令词自学习使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E7%A6%BB%E7%BA%BF%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/) |