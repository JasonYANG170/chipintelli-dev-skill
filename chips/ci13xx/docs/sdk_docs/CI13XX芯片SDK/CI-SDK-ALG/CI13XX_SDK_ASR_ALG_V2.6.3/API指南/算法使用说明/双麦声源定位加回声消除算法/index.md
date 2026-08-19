<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E5%A3%B0%E6%BA%90%E5%AE%9A%E4%BD%8D%E5%8A%A0%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E7%AE%97%E6%B3%95/ -->

# 双麦声源定位加回声消除算法

该算法组合可进行声源方位角度估计，当前版本支持0-180度检测范围，分辨率为10°，自适应追踪回声路径的变换、实时抑制扬声器到达麦克风终端的回声信号，以提升目标语音的识别效果，该算法组合仅双mic可用。

**1.算法功能配置步骤如下：**

打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，将CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_AI\_DOA\_AEC)

**CI\_ALG\_TYPE变量和算法功能对应说明请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**2. 开启声源定位功能，请把external\model\doa(声源定位)中[60004]nn\_dual\_mic\_doa\_vxxxx.bin算法模型，复制到projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\firmware\dnn文件夹中**

**3. 声源定位相关参数与应用函数说明请查看☞[双麦声源定位算法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E5%A3%B0%E6%BA%90%E5%AE%9A%E4%BD%8D%E7%AE%97%E6%B3%95/)**

**4. 回声消除算法相关参数说明请查看☞[回声消除算法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E7%AE%97%E6%B3%95/)，回声消除涉及硬件参考信号线路的设计，如果贵司自行设计硬件， 请联系启英泰伦技术支持**

注意

1. 该算法组合左右MIC采集的语音信号都用做ANY\_MIC算法，AEC算法必须外挂codec用作回采参考信号(推荐使用7243e codec）。