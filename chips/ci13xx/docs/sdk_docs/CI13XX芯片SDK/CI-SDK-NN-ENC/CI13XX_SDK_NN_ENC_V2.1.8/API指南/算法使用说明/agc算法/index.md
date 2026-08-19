<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/agc%E7%AE%97%E6%B3%95/ -->

# AGC自动增益控制算法

AGC是自动增益控制（Automatic Gain Control）的简称，是音频处理流程中至关重要的一个环节，它的根本目标是解决一个常见问题：​​说话人离麦克风的距离不同、说话声音大小不同，或者环境声音变化，导致接收端听到的音量不稳定（忽大忽小）或过低。​

本文档主要介绍如何使用该功能。

## 1. AGC基本介绍

自动调整增益使语音段的平均能量逼近设定的目标输出电平。若输入信号过弱，则提升增益；过强则降低增益，避免削波；当信号峰值接近最大值（0 dBFS）时，启动压缩算法，按比例降低高幅值信号的增益（避免削波）。
AGC 性能高度依赖环境噪声和麦克风质量；在极端嘈杂场景下，需配合降噪模块才能发挥最佳效果。

![自动增益控制原理框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/agc-1.png)

图1-1 自动增益控制原理框图

## 2. AGC软件平台

以下SDK可以支持AGC功能：
- 离线通话降噪SDK：CI130X\_SDK\_NN\_ENC

## 3. AGC功能配置及参数调节说明

### 3.1 AGC功能开启

用户可打开SDK包中的user\_config.h文件

```
//是否使用AGC功能，1：使用，0：不使用
#define USE_AGC_MODULE                   1
```

### 3.2 AGC算法软件配置

用户可打开SDK包中的../projects/nn\_denoise\_rtc\_samplesrc/ci\_ssp\_config.c文件，自动增益控制算法有以下参数可供用户调试使用：

```
// ci_ssp_config.c
agc_config_t agc_config = 
{
    // AGC作用在输入信号上的最大增益，范围[0,90]，值越大，增益幅度越大
    .compress_gain = 40,    
    // 输出信号的目标电平，范围[0,31]，target_db值越大，AGC输出信号幅度越小
    .target_db = 15,    
    //是否打印AGC调试信息   
    .agc_log = 0,               
};
```

**参数说明**：

**compression\_gain(压缩增益上限)：**

* **作用：控制 AGC ​​最大可提升​​的增益值。**
* **取值：[0, 90]，单位为分贝（dB）。**

**场景建议：**

* **安静环境：6-9 dB（避免放大底噪）**
* **普通环境：9-12 dB**
* **远距离麦克风：15-18 dB**

**target\_db(目标电平)：**

* **作用：控制 AGC 输出的目标电平**
* **取值：[0, 31]，单位为分贝（dB）**

**场景建议：**

* **安静环境：0-5 dB**
* **普通环境：5-10 dB**
* **远距离麦克风：10-15 dB**

**调试建议**：

若噪声过大，可适当调小AGC的增益能力“compress\_gain”，使AGC输出信号幅度减小，从而减小噪声；若语音过小，可适当调大AGC的增益能力“compress\_gain”，和目标电平 “target\_db”，使AGC输出信号幅度增大，从而增大语音信号。

## 4. 异常调试步骤

**注意：调试异常过程中，保存好音频，有利于快速分析。**

* **step1**：检查sdk配置是否存在异常
* **step2**：通过采音板采集参考信号（ref）、输出信号（dst）和麦克风信号（mic）判断异常原因

**目前较多的异常情况举例有：**

1）说话人与MIC间的距离忽远忽近，导致输出信号忽大忽小；

2）噪声环境下，噪声被忽然放大；

* **step3**：替换不同的MIC分析是否是外围硬件设备差异导致异常

外围设备有：麦克风、codec等。

AGC应用不符合预期时建议以标准sdk和硬件作为参考，通过标准配件参考分模块分析具体原因。

## 5. AGC处理效果图如下：

![AGC效果配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/agc%E6%95%88%E6%9E%9C%E5%9B%BE.png)