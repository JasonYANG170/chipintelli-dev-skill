<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1122%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1122芯片数据手册.pdf)

# 芯片概述

CI1122是一颗专用于语音处理的人工智能芯片，可广泛应用于家电、家居、照明、玩具等产品领域，实现语音交互及控制。

CI1122内置自主研发的脑神经网络处理器BNPU，支持200条命令词以内的本地语音识别，内置CPU核和高性能低功耗Audio Codec模块，集成多路UART、IIC、PWM、GPIO等外围控制接口，可以开发各类高性价比单芯片智能语音产品方案。

请点击 ☞[CI1122芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1122%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C.pdf) 下载完整的芯片数据手册。

表1 芯片信息

| 芯片型号 | FLASH容量 | RAM容量 | 封装信息 |
| --- | --- | --- | --- |
| CI1122 | 4MByte | 512KByte | QFN48L(6mm\*6mm\*0.85mm) |

CI1122可应用的部分产品领域：

* 智能家电
* 智能玩具
* 智能照明
* 智能遥控器

![CI1122应用框图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1122%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI1122%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-1.png)

图1 芯片应用框图

芯片特性如下：

* 神经网络处理器BNPU
  + 采用硬件进行神经网络运算，内核可配置并独立处理AI语音功能
  + 支持本地语音识别
  + 支持VAD语音检测
* CPU
  + 180MHz运行频率
  + 32-bit单周期乘法器
* 存储器
  + 内置512KB SRAM
  + 内置ROM
  + 内置4MB FLASH
* 音频接口
  + 内置高性能低功耗Audio Codec模块，支持单路ADC采样和DAC播放
  + 支持Automatic Level Control (ALC)功能
  + 支持8kHz/16kHz/24kHz/32kHz/44.1kHz/48kHz采样率，支持44.1kHz时钟频点
  + 支持一路IIS音频扩展通路
* SAR ADC
  + 4路12bit SAR ADC输入通道
  + ADC IO可与数字GPIO进行功能复用
* 外设和定时器
  + 2路UART接口，最高可支持3M波特率
  + 1路IIC接口
  + 6路PWM接口
  + 内置4组32-bit timer
  + 内置1组独立看门狗（IWDG）
  + 内置1组窗口看门狗（WWDG）
* GPIO
  + 最多支持27个GPIO口（IO功能复用）
  + 每个GPIO口可配置中断功能
  + 支持两路带滤波功能外部中断