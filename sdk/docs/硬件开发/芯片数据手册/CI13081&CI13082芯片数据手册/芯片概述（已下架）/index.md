<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13081%26CI13082%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0%EF%BC%88%E5%B7%B2%E4%B8%8B%E6%9E%B6%EF%BC%89/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI13081&CI13082芯片数据手册.zip)

# 芯片概述

CI1308X是启英泰伦研发的新一代高性能神经网络智能语音芯片，集成了启英泰伦自研的脑神经网络处理器BNPU V3.5和CPU内核，系统主频可达210MHz，内置高达288KByte的SRAM，集成PMU电源管理单元和RC振荡器，集成单通道高性能低功耗Audio Codec和多路UART、IIC、PWM、GPIO等外围控制接口。CI1308X芯片仅需少量电阻电容等外围器件，即可实现各类智能语音产品硬件方案，性价比极高。

CI1308X采用工业级设计标准，具有很好的环境可靠性，其工作温度范围-40℃～+85℃，符合MSL3级湿敏等级、符合IEC 61000-4-2 的4KV接触放电试验标准、符合RoHS和REACH环保标准。

CI1308X采用启英泰伦新一代BNPU技术，该技术支持DNN\TDNN\RNN\CNN等神经网络及并行矢量运算，可实现高性能语音识别、语音降噪等功能，具备强劲的环境噪声抑制能力。CI1308X方案还支持汉语、英语、日语等多种全球语言，可广泛应用于家电、照明、玩具、可穿戴设备、工业、汽车等产品领域，实现语音交互及控制和各类智能语音方案应用。

请点击 ☞[CI13081芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI13081_datasheet%20V1.0_chs_20240819.pdf) ☞[CI13082芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI13082_datasheet%20V1.1_chs_20250703.pdf)下载完整的芯片数据手册。

表G-1 芯片信息

| 芯片型号 | FLASH容量 | RAM容量 | 封装信息 |
| --- | --- | --- | --- |
| CI13081 | 1MByte | 288KByte | SOP8(4.9mm \* 6.0mm \* 1.75mm) |
| CI13082 | 2MByte | 288KByte | SOP8(4.9mm \* 6.0mm \* 1.75mm) |

CI1308X可应用的部分产品领域：

* 智能家电
* 智能玩具
* 智能照明
* 智能可穿戴

![CI13081&CI13082芯片应用框图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13081%26CI13082%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI1308X%E5%BA%94%E7%94%A8%E6%A1%86%E5%9B%BE.png)

图G-1 CI1308X芯片应用框图

芯片特性如下：

* 脑神经网络处理器BNPU V3.5
  + 采用启英泰伦新一代硬件 BNPU 技术，支持 DNN\TDNN\RNN\CNN 等神经网络及并行矢量运算，可实现高性能语音识别、语音降噪等功能
* CPU
  + 32位高性能CPU，最高支持210MHz运行频率
  + 32bit单周期乘法器，支持DSP扩展加速
* 存储器
  + 内置288KB SRAM
  + 内置256bit eFuse
  + 内置1MB/2MB Flash
* 音频接口
  + 内置高性能低功耗Audio Codec模块，支持单路ADC采样和单路DAC播放
  + 支持Automatic Level Control (ALC)功能
  + 支持8kHz/16kHz/24kHz/32kHz/44.1kHz/48kHz采样率
* 电源管理单元PMU
  + 支持宽电源电压供电，供电范围3.6V～5.5V
  + 内置2路高性能LDO电路，无需配置外置电源芯片，应用方案仅需少量外围阻容器件
* 时钟
  + 内置RC振荡器
* 外设和定时器
  + 1路UART接口，支持最高3M波特率通讯
  + 1路IIC接口，可外接IIC器件扩展
  + 3路PWM接口，灯控和电机类的应用均可直接驱动
  + 内置2组32bit timer
  + 内置1组独立看门狗（IWDG）
* GPIO
  + 支持3路GPIO口，可作为主控IC应用
  + 每路GPIO口可配置中断功能，可配置上下拉状态
  + 2路GPIO可通过外接5V上拉电阻直接支持5V电平通讯
* 软件开发支持
  + 提供完整软件开发包、应用方案示例、利用语音开发平台直接在线制作固件等支持，详情请访问：[https://aiplatform.chipintelli.com](https://aiplatform.chipintelli.com/)
* 固件烧录和保护
  + 支持UART升级和固件保护
* ESD性能
  + 采用内部ESD增强设计，可通过4KV接触放电试验
* ROHS和REACH
  + 采用环保材料，支持RoHS和REACH标准
* 封装和工作温度范围
  + 封装形式：SOP8，尺寸为长4.9mm，宽6.0mm，高1.75mm
  + 工作环境温度：-40℃～+85℃