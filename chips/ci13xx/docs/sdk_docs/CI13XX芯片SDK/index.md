<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/ -->

# CI13XX 系列芯片SDK

目前CI13XX系列芯片的SDK根据不同应用场景有如下版本分类：

## 1. [离线语音识别SDK ：CI-SDK-ASR-Offline](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/)

注意

SDK发布版本格式：CI13XX\_SDK\_ASR\_Offline\_Vx.x.x

主要针对纯离线简单应用场景，例如智能家居等，支持的音频前端算法有：

* ASR（Automatic Speech Recognition，自动语音识别）
* CWSL（Command Word Self-Learning，离线命令词自学习）
* AEC（Acoustic Echo Cancellation，回声消除）

---

## 2. [离线语音算法SDK ：CI-SDK-ASR-ALG](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/)

注意

SDK发布版本格式：CI13XX\_SDK\_ASR\_ALG\_Vx.x.x

CI13XX系列芯片集成第三代BNPU具备较强的神经网络及向量并行运算能力，以及充足的RAM，能够完成一些常见的语音处理算法。主要包含了以下算法功能。使用者可以根据不同产品需求进行合适的选择。

* ASR（Automatic Speech Recognition，自动语音识别）：语音识别-单麦克风，固定词条或者自然说；
* CWSL（Command Word Self-Learning，离线命令词自学习）
* AEC（Acoustic Echo Cancellation，回声消除）
* 声纹识别：VPR（Voice Print Recognition，基于特定人的声纹识别）、WMAN\_VPR（Woman and Man Voice Print Recognition，男女声纹检测功能） 等；
* 声音事件检测：SED\_CRY（Sound Event Detection-Cry，哭声检测功能）、 SED\_SNORE（Sound Event Detection-Snore，鼾声检测功能）、咳嗽声 等；
* NN DENOISE（Neural Network Denoise，神经网络降噪功能）
* DOA（Direction of Arrival，双麦声源定位功能）
* DERVERB（Dereverberation，双麦降混响）
* BF（Beamforming，波束成形）：双麦深度分离；
* PWK（Power Weighted K-factor，声音能量值值计算）：区分目标声音距离；
* ALC（Automatic Level Control，自动增益控制）
* TTS（Text To Speech，文本转语音）：只支持中文、数字、字母，不支持英文；

其中ASR功能可以和以上除TTS功能以外的其他功能进行1+1组合，如需多个算法组合请参照☞[《算法SDK开发流程》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)。

---

## 3. [语音通话降噪SDK ：CI-SDK-NN-ENC](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/)

注意

SDK发布版本格式：CI13XX\_SDK\_NN\_ENC\_Vx.x.x

**完善中…**

---

## 4. [离在线大模型对话SDK ：CI-SDK-LLM-AIOT](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/)

注意

SDK发布版本格式：CI13XX\_SDK\_LLM\_AIOT\_Vx.x.x

主要针对纯离在线大模型对话应用场景，例如AI玩具，AI医疗，车载等；支持离线自学习，回声消除(AEC),深度降噪，声源定位(DOA),VAD端点检测，SPEEX/OPUS/G722语音编解码算法；同时支持语音在线上传和播放功能

* ASR（Automatic Speech Recognition，自动语音识别）：语音识别-单麦克风，固定词条或者自然说；
* CWSL（Command Word Self-Learning，离线命令词自学习）
* AEC（Acoustic Echo Cancellation，回声消除）
* NN DENOISE（Neural Network Denoise，神经网络降噪功能）
* DOA（Direction of Arrival，双麦声源定位功能）
* SPEEX（Speex）：采用 CELP（Code Excited Linear Prediction，码激励线性预测编码） 算法，在低码率下能保持清晰语音可懂度的音频编解码器；
* OPUS（Opus Codec，opus 音频编解码器）有损声音编码的格式，以语音编码为导向的SILK和低延迟的CELT；
* G722（G.722 Wideband Speech Codec，G.722宽带语音编解码器）最早被广泛采用的宽带语音编码器之一,技术成熟稳定；
* VAD（Voice Activity Detection，语音活性检测）

---

备注

各版本SDK可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 开发资料板块中进行下载使用。