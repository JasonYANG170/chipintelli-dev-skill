<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%87%AA%E5%AD%A6%E4%B9%A0%E5%8A%A0%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E5%8A%A0%E9%99%8D%E5%99%AA%E7%AE%97%E6%B3%95/ -->

# 自学习+双麦声源定位+回声消除

|  |  |
| --- | --- |
| ``` 1 ``` | ```  该算法组合支持非联网状态，用户通过语音对话的方式，学习新的唤醒词或者命令词；自适应追踪回声路径的变换、实时抑制扬声器到达麦克风终端的回声信号，以提升目标语音的识别效果；能抑制设备本身产生的高噪声，以提升低信噪比环境下的识别率，针对不同应用领域需使用对应的领域模型。。 ``` |

**1. 打开CI-SDK-LLM-AIOT\_Vx.x.x\projects\offline\_asr\_alg\_pro\_sample\project\_file\makefile文件，将CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_CWSL\_AEC\_DENOISE\_NN)**

**CI\_ALG\_TYPE变量和算法功能对应说明请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**2. 开启深度降噪算法请把CI-SDK-LLM-AIOT\_Vx.x.x\projects\external\model\nn\_denoise(深度降噪)中[60003]nn\_denoise\_xx.bin算法模型，复制到CI-SDK-LLM-AIOT\_Vx.x.x\projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\firmware\dnn文件夹中**

**3. 回声消除算法相关参数说明请查看☞[回声消除算法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E7%AE%97%E6%B3%95/)，回声消除涉及硬件参考信号线路的设计，如果贵司自行设计硬件， 请联系启英泰伦技术支持**
**4. 自学习相关参数与应用函数说明请查看☞[自学习算法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%87%AA%E5%AD%A6%E4%B9%A0%E7%AE%97%E6%B3%95/)**
**5. 深度降噪相关参数与应用函数说明请查看☞[深度降噪算法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E7%AE%97%E6%B3%95/)**

注意

1. 该算法属于外挂codec方案，需外部挂7243e codec。