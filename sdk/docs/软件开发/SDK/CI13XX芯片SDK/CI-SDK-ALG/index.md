<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/ -->

# 离线语音算法SDK（CI-SDK-ASR-ALG）

---

## **概述**

目前CI13XX系列芯片的离线语音算法SDK（CI-SDK-ASR-ALG）最新发布版本为：CI13XX\_SDK\_ASR\_ALG\_V2.6.3

主要针对纯离线语音识别与简单应用场景，例如智能家居等，支持的音频前端算法有：

* ASR（Automatic Speech Recognition，自动语音识别），☞[《语音识别使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E7%AE%97%E6%B3%95/)
* CWSL（Command Word Self-Learning，离线命令词自学习），☞[《离线命令词自学习使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%87%AA%E5%AD%A6%E4%B9%A0%E7%AE%97%E6%B3%95/)
* AEC（Acoustic Echo Cancellation，回声消除），☞[《回声消除使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E7%AE%97%E6%B3%95/)
* VPR（Voice Print Recognition，声纹注册），☞[《声纹注册算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%A3%B0%E7%BA%B9%E6%B3%A8%E5%86%8C%E7%AE%97%E6%B3%95/)
* WMAN\_VPR（Woman and Man Voice Print Recognition，男女声纹检测），☞[《男女声纹检测算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B9%E6%A3%80%E6%B5%8B%E7%AE%97%E6%B3%95/)
* SED\_SNORE（Sound Event Detection-Snore，鼾声检测），☞[《哭声鼾声检测算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%93%AD%E5%A3%B0%E4%B8%8E%E9%BC%BE%E5%A3%B0%E6%A3%80%E6%B5%8B%E7%AE%97%E6%B3%95/)
* SED\_CRY（Sound Event Detection-Cry，哭声检测），☞[《哭声鼾声检测算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%93%AD%E5%A3%B0%E4%B8%8E%E9%BC%BE%E5%A3%B0%E6%A3%80%E6%B5%8B%E7%AE%97%E6%B3%95/)
* NN\_DENOISE（Neural Network Denoising，深度降噪），☞[《深度降噪算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E7%AE%97%E6%B3%95/)
* DOA（Direction of Arrival，双mic声源定位），☞[《双mic声源定位算法说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E5%A3%B0%E6%BA%90%E5%AE%9A%E4%BD%8D%E7%AE%97%E6%B3%95/)
* BF（Beamforming，双mic语音增强），☞[《双mic语音增强算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%A2%9E%E5%BC%BA%E7%AE%97%E6%B3%95/)
* DEREVERB（Dereverberation，双mic降混响），☞[《双mic降混响算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E9%99%8D%E6%B7%B7%E5%93%8D%E7%AE%97%E6%B3%95/)
* TTS（Text To Speech，语音合成TTS），☞[《语音合成TTS算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%AF%AD%E9%9F%B3%E5%90%88%E6%88%90TTS%E7%AE%97%E6%B3%95/)
* PWK（Power Weighted K-factor，声音能量计算），☞[《声音能量计算算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%A3%B0%E9%9F%B3%E8%83%BD%E9%87%8F%E5%80%BC%E8%AE%A1%E7%AE%97%E7%AE%97%E6%B3%95/)
* ALC（Automatic Level Control，自动增益控制），☞[《自动增益控制算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%87%AA%E5%8A%A8%E5%A2%9E%E7%9B%8A%E6%8E%A7%E5%88%B6%E7%AE%97%E6%B3%95/)
* ANY\_MIC\_ASR（Any Microphone Automatic Speech Recognition，双mic任意mic识别），☞[《双mic任意mic识别算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E4%BB%BB%E6%84%8FMIC%E8%AF%86%E5%88%AB%E5%8A%A0%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E7%AE%97%E6%B3%95/)

---

## **离线语音算法SDK适配的AI语音芯片型号**

* 芯片型号（☞[启英商城](http://mall.chipintelli.com/chip?product_category=45&brd=1)有售）：

CI 1301 、 CI 1302 、 CI 1303 、 CI 1306 、 CI 1311 、 CI 1312。

* 模块型号（☞[启英商城](http://mall.chipintelli.com/chip?product_category=45&brd=1)有售）：

CI-D01GS01J单麦离线语音识别端子模块

CI-D02GS01J单麦离线语音识别端子模块

CI-D03GS02S单麦离线语音识别贴片模块

CI-D02GS02S单麦离线语音识别贴片模块

CI-D02GS07J-BT单麦离线语音识别蓝牙端子模块

CI-D02GS04U离线语音空调控制器

* 开发板套件型号（☞[启英商城](http://mall.chipintelli.com/chip?product_category=45&brd=1)有售）：

CI1306开发板套件

CI1303开发板套件

CI1302开发板套件