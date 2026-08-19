<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/drc%E7%AE%97%E6%B3%95/ -->

# DRC动态范围控制算法

DRC(Dynamic Range Control, 动态范围控制)算法工要用于如下3类情况:
- 压缩超过给定阈值的较大声情号的首量。将较大的音频信号压缩到更小的动态范围，同时也可以防止削顶带来的失真
- 对输人的忽大忽小的语言进行动态拉伸，使语首听起来更加平稳
- 抑制低电平噪声，避免噪声在后续处理门程中带来干扰。

![动态范围示意图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/drc-1.png)

图1-1 动态范围示意图

## 1. DRC基本介绍

* DRC可以将输入音频信号的动态范围映射到指定的动态范围，从而使声音听起来更柔和或更大声，即一种信号幅度调节方式。通常映射后的动态范围小于映射前的动态范围，因此称之为动态范围压缩。音频信号可以进行整体的动态范围控制。也可以划分为若干子带分别进行动态范围控制。DRC广泛应用于音频处理领域，例如助听器中最常见的宽动态范围压缩方法(Wide Dynamic Range Compression,WDRC)、首频信量处理中最常用的自动增益控制(Automatic Gain Control, AGC)，方法等。DRC 侧重于从数字域上对增益进行有选择、有日的进行调整，以保让最大信号有合适的 headroom，中间信号得到合适的amplify，小信号或者底噪被cutoff掉
* 动态范围的定义：信号的最大幅值和最小幅值比值的对数(单位 dB)。动态范围会受到系统中各个环节的影响。例如同样是这段音乐，在一个 40dB 背景噪声的环境中播放，由于掩蔽效应等因素的影响，最终实际听到的音乐动态范围实际上是50dB。

## 2. DRC软件平台

以下SDK可以支持DRC功能：
- 离线通话降噪SDK：CI130X\_SDK\_NN\_ENC

## 3. DRC功能配置及参数调节说明

### 3.1 DRC功能开启

用户可打开SDK包中的user\_config.h文件

```
#define USE_DRC_MODULE             1  //是否使用DRC功能，1：使用，0：不使用
```

### 3.2 DRC算法软件配置

用户可打开SDK包中的../projects/nn\_denoise\_rtc\_samplesrc/ci\_ssp\_config.c文件，自动增益控制算法有以下参数可供用户调试使用：

```
//默认配置
drc_config_t drc_config = 
{
    .attack_time = 0.01f,        //算法启动时间，默认设置为0.01S
    .release_time = 0.015f,      //算法释放时间，默认设置为0.015S
    .thr_comp = -10.0f,          //幅值压缩阈值，默认设置为-10dB
    .thr_exp = -25.0f,           //幅值拉伸阈值, 默认设置为-25dB
    .thr_gate = -35.0f,          //噪声抑制阈值, 默认设置为-35dB
    .ratio = 6.0f,               //幅值压缩比，默认设置为6
    .knee = 3.0f,                //拐点宽度，默认设置为3
    .drc_para_from_uart = false, //是否从串口获取配置参数
};
```

### 3.4 DRC工具参数调节示意图如下:

![DRC工具参数调节示意图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/drc-2.png)

### 3.5 Attack和Release示意图如下:

![Attack和Release示意图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/drc-3.png)

### 3.6 DRC算法作用过程示意图如下:

![DRC算法作用过程示意图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/drc-4.png)