<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# CI13XX降噪SDK开发说明

---

## 1. 概述

CI13XX降噪SDK包含多个音频处理算法功能，不带ASR识别功能，SDK默认开启了降噪功能(不能关闭)，其他算法功能用户可以根据需求选择开关。
**CI13XX\_SDK\_NN\_ENC\_V2.1.8版本主要包含了以下算法功能:**

| 算法名称 | 算法说明 |
| --- | --- |
| NN DENOISE | 深度降噪算法 |
| AEC | 回声消除算法 |
| AGC | 自动增益控制算法 |
| ALC | 自动电平控制算法 |
| DRC | 动态范围控制算法 |
| EQ | 均衡器算法 |
| AHS | 啸叫抑制算法 |
| RECORD | 录音功能 |

**模型ID定义：**

开启算法功能需使用前端算法模型，各个算法模型对应ID如下表：

| 模型ID | 模型类型 |
| --- | --- |
| 60003 | 深度降噪模型 |
| \*\*\* |  |

## 3. SDK开发包下载:

**3.1 注册并登录AI开发平台：[https://aiplatform.chipintelli.com](https://aiplatform.chipintelli.com/)**

**3.2 获取算法SDK CI13XX\_SDK\_ASR\_ALG\_VXX的软件开发包：<https://aiplatform.chipintelli.com/attachment>， (*若有新版本，请使用最新版本的SDK*),如下图：**

## 4. 算法处理后音频数据获取

用户使用算法处理音频后，如果想获取原始音频和算法处理后的音频数据，获取方法请参考☞[降噪和原始音频采集](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E7%AE%97%E6%B3%95%E5%A4%84%E7%90%86%E5%90%8E%E7%9A%84%E9%9F%B3%E9%A2%91%E9%87%87%E9%9B%86/)

## 5. 启英泰伦-语音开发工具”，请前往启英泰伦语音AI平台[开发资料](https://aiplatform.chipintelli.com/attachment)中下载获取chipintelli-audio-tools\_vx.x.x.exe

![语音开发工具](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E8%AF%AD%E9%9F%B3%E5%BC%80%E5%8F%91%E5%B7%A5%E5%85%B7%E4%B8%8B%E8%BD%BD.png)

## 6. 上位机和语音芯片交互协议,请参考☞[启英泰伦-算法参数调节协议\_V2.pdf](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E5%8D%8F%E8%AE%AE/%E7%AE%97%E6%B3%95%E5%8F%82%E6%95%B0%E8%B0%83%E8%8A%82%E5%8D%8F%E8%AE%AE_V2.pdf)\*\*

注意：使用工具调节参数时，请选择V2版本(当前SDK默配置为支持V2协议)，如下图：
![算法参数调节](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E7%AE%97%E6%B3%95%E5%8F%82%E6%95%B0%E8%B0%83%E8%8A%82%E5%B7%A5%E5%85%B7.png)