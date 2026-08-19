<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/ -->

# 离在线大模型对话SDK（CI13XX\_SDK\_LLM\_AIoT）

---

## **概述**

目前CI13XX系列芯片的离在线大模型对话SDK（CI13XX\_SDK\_LLM\_AIoT）最新发布版本为：CI13XX\_SDK\_LLM\_AIoT\_V2.1.2

主要针对纯离在线大模型对话应用场景，例如AI玩具，AI医疗，车载等；支持离线自学习，回声消除(AEC)，深度降噪，声源定位(DOA)，VAD端点检测，SPEEX/OPUS/G722语音编解码算法；同时支持语音在线上传和播放功能：

* ASR（Automatic Speech Recognition，自动语音识别），☞[《语音识别使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V2.1.2/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E7%AE%97%E6%B3%95/)
* CWSL（Command Word Self-Learning，离线命令词自学习），☞[《离线命令词自学习使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V2.1.2/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%87%AA%E5%AD%A6%E4%B9%A0%E7%AE%97%E6%B3%95/)
* AEC（Acoustic Echo Cancellation，回声消除），☞[《回声消除使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V2.1.2/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E7%AE%97%E6%B3%95/)
* NN\_DENOISE（Neural Network Denoising，深度降噪），☞[《深度降噪算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V2.1.2/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E7%AE%97%E6%B3%95/)
* DOA（Direction of Arrival，双mic声源定位），☞[《双mic声源定位算法说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V2.1.2/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E5%A3%B0%E6%BA%90%E5%AE%9A%E4%BD%8D%E7%AE%97%E6%B3%95/)

目前离在线大模型对话SDK提供了3个sample供用户选择，具体特性如下

| sample名称 | 录音上传 | 指令控制 | VAD | 音频播放 |
| --- | --- | --- | --- | --- |
| hpout\_sample | 使用hpout上传录音，不支持压缩，支持单/双麦 | 支持串口传输指令 | 不支持VAD | 不支持播放，播放端给语音端回声消除参考信号 |
| iis\_sample | 使用iis上传录音，不支持压缩，只支持单麦 | 支持串口传输指令 | 不支持VAD | 支持iis下传音频播放，只支持pcm格式，不支持本地播放 |
| uart\_sample | 使用uart上传录音，支持g722/speex/opus编码压缩，支持单/双麦 | 支持串口传输指令 | 支持VAD | 支持uart下传音频播放，支持pcm/g722/mp3，支持本地播放 |

---

## **离在线大模型对话SDK适配的AI语音芯片型号**

* 芯片型号（☞[启英商城](http://mall.chipintelli.com/chip?product_category=45&brd=1)有售）：

CI 1302 、 CI 1303 、 CI 1306 。

* 模块型号（☞[启英商城](http://mall.chipintelli.com/chip?product_category=45&brd=1)有售）：

CI-D02GS01J单麦离线语音识别端子模块

CI-D06GT01J双麦离线语音识别蓝牙端子模块

* 开发板套件型号（☞[启英商城](http://mall.chipintelli.com/chip?product_category=45&brd=1)有售）：

CI1306开发板套件

CI1303开发板套件

CI1302开发板套件