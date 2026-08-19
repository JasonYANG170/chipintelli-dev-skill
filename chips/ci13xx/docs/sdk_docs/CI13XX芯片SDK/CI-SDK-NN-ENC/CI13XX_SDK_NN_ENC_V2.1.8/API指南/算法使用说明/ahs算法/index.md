<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ahs%E7%AE%97%E6%B3%95/ -->

# AHS啸叫抑制算法

啸叫抑制算法(Howling Suppression Algorithm‌, AHS),主要应用于语音通信设备（如助听器、耳机）、音响系统及会议系统，解决因声音循环反馈导致的刺耳噪声问题。

## 1. AHS基本介绍

使用通信设备时，若两个设备在同一房间且距离很近，发射设备将麦克风接收到的声音信号传输到接收设备，由于接收设备扬声器播放的声音又传回给发射设备，形成声音反馈闭环回路，当扩音的增益足够大，在某些频率就会产生自激震荡，产生刺耳的啸叫，通常需要手动拉开物理间距或者减小音量来解决，啸叫产生过程如下图：
![啸叫产生过程](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/ahs-1.png)

图1-1 啸叫产生过程

## 2. AHS软件平台

* 离线通话降噪SDK：CI130X\_SDK\_NN\_ENC

## 3. AHS功能开启

打开SDK包中的user\_config.h文件

```
//是否使用AHS功能，1：使用，0：不使用
#define USE_AHS_MODULE             1
```

## 4. 啸叫抑制效果展示

![AHS算法对单频波的抑制效果](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/ahs-2.png)

图4-1 AHS 算法对单频波的抑制效果