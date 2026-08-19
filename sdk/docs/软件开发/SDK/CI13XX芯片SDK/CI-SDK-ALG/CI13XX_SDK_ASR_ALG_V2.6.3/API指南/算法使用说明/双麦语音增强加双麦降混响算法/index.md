<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%A2%9E%E5%BC%BA%E5%8A%A0%E5%8F%8C%E9%BA%A6%E9%99%8D%E6%B7%B7%E5%93%8D%E7%AE%97%E6%B3%95/ -->

# 双麦语音增强+双麦降混响算法

该算法组合主要用于半稳态噪声，如新闻噪声、音乐噪声环境下，用于抑制噪声和干扰，提升目标语音信噪比；同时在混响环境较重的条件下配合双麦阵列使用，提升高混响环境下的识别效果，该算法组合仅双mic可用，双mic位置需处于同一平面相同朝向，推荐麦间距为4cm~6cm。

**1.算法功能配置步骤如下：**

打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，将CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_BF\_DEREVERB)

**CI\_ALG\_TYPE变量和算法功能对应说明请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**2. 双麦语音增强算法相关参数与应用函数说明请查看☞[双麦语音增强算法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%A2%9E%E5%BC%BA%E7%AE%97%E6%B3%95/)**

**3. 双麦降混响算法相关参数说明请查看☞[双麦降混响算法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E9%99%8D%E6%B7%B7%E5%93%8D%E7%AE%97%E6%B3%95/)**