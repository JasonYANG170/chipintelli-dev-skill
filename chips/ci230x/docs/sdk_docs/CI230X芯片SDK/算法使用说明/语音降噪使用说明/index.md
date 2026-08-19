<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# 语音降噪使用说明

---

降噪（DENOISE）算法能有效抑制稳态噪声，在保证语音失真度的同时具有良好的噪声抑制效果。开启该功能后，可以通过降低稳态噪声的方式进行识别效果的提升，但是同时该功能会消耗24KB的芯片内部存储空间，并且会消耗CPU的带宽资源。本文档主要介绍降噪算法适用的场景，以及如何打开该功能。

---

## 1. 语音降噪算法

* 语音降噪算法的应用原理示意框图如下，人声为目标语音信号A，噪音和信号A在应用环境进行复杂的混合后进入芯片，语音降噪算法可抑制噪音信号，提高信号A的信噪比，从而提升识别效果。
* 目前启英泰伦提供的语音降噪算法仅适用于稳态噪声及半稳态噪声。

![语音降噪算法应用原理示意框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-1.png)

图1-1 语音降噪算法应用原理示意框图

---

## 2. 语音降噪算法软件配置方法

用户可打开SDK包中的ci\_ssp\_config.c文件，语音降噪算法有以下参数可供用户调试使用：

```
denoise_config_t denoise_config =
{
    .start_Hz = 0,                        //降噪起始频率 单位Hz
    .end_Hz = 8000,                       //降噪结束频率 单位Hz
    .fre_resolution = 31.25f,             //频率分辨率 单位Hz :16000/256
    .aggr_mode = 1,                       //算法处理的效果等级:0，1，2，处理效果依次增强，失真也会变大
    .set_denoise_threshold = 3200,        //默认帧平均幅值>=3200起效
    .set_denoise_thr_window_size = 20     //门限判断窗长
};
```

下图是一个set\_denoise\_threshold的值与实际的分贝值的拟合关系曲线，可供用户参考。下图的数据与真实实测数据往往会存在误差，需调试人员根据实际噪音环境进行调试，判断在当前噪声条件下是否进行降噪。

![set_denoise_threshold值与实际分贝值的拟合关系曲线](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-2.png)

图2-1 set\_denoise\_threshold值与实际分贝值的拟合关系曲线

## 3. 语音降噪算法软件调试说明

* 下图为实时处理后时域信号对比效果，该图上可以看到原始的左声道数据（上半图）为目标语音和噪音的混合，经过语音降噪算法处理后，抑制了噪声，提高了目标语音的信噪比（下半图）。

![录音板采音时域效果](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-3.jpg)

图3-1 录音板采音时域效果

* 下面为同样该音频的频域显示。该结果为正常结果。

![音频的频域显示](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E8%AF%AD%E9%9F%B3%E9%99%8D%E5%99%AA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-4.jpg)

图3-2 音频的频域显示