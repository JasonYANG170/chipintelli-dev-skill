<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E7%AE%97%E6%B3%95/ -->

# 深度降噪算法

该法算能抑制设备本身产生的高噪声，以提升低信噪比环境下的识别率，针对不同应用领域需使用对应的领域模型。

**1. 深度降噪算法请把external\model\nn\_denoise(深度降噪)中[60003]nn\_denoise\_xx.bin算法模型，复制到projects\CI-SDK-LLM-AIOT\_Vx.x.x\firmware\dnn文件夹中**

**2.降噪和原始音频数据采集，请参考☞[降噪和原始音频采集](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E7%AE%97%E6%B3%95%E5%A4%84%E7%90%86%E5%90%8E%E7%9A%84%E9%9F%B3%E9%A2%91%E9%87%87%E9%9B%86/)**

**3.降噪前后数据对比，从图中可以看出降噪算法将底噪和环境噪声进行了抑制**
![降噪前后音频对比](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E9%99%8D%E5%99%AA%E5%89%8D%E5%90%8E.png)

注意

1. 深度降噪算法没有用户需要配置的参数
2. 深度降噪需搭配该算法的前端算法模型使用。
3. 深度降噪在AI平台制作ASR与DNN模型请选择深度降噪专用模型。

![深度降噪模型](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E7%AE%97%E6%B3%95SDK2.4.18%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-6.png)