<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1311&CI1312芯片数据手册.zip)

# 芯片概述

CI1311、CI1312是启英泰伦研发的新一代高性能神经网络智能语音芯片，集成了启英泰伦自研的脑神经网络处理器BNPU V3和CPU内核，系统主频可达220MHz，内置高达640KByte的SRAM，集成PMU电源管理单元和RC振荡器，集成单通道高性能低功耗Audio Codec和多路UART、IIC、PWM、GPIO等外围控制接口。芯片仅需少量电阻电容等外围器件就可以实现各类智能语音产品硬件方案，性价比极高。

CI1311、CI1312使用工业级设计标准，具有较高的环境可靠性，芯片工作温度范围在-20°C到 +85°C之间，符合MSL3级湿敏等级，符合IEC 61000-4-2 的4K接触放电试验标准，符合FCC电磁兼容标准，符合ROHS和REACH环保标准。

CI1311、CI1312采用了启英泰伦的3代BNPU技术，该技术支持DNN\TDNN\RNN\CNN等神经网络及并行矢量运算，可实现语音识别、命令词自学习、语音检测及深度学习降噪等功能，具备强劲的回声消除和环境噪声抑制能力。该芯片方案还支持汉语、英语、日语等多种全球语言，可广泛应用于家电、照明、玩具、可穿戴设备、工业、汽车等产品领域，实现语音交互及控制和各类智能语音方案应用。

请点击 ☞[CI1311芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1311_datasheet%20V1.5_chs_20250725.pdf) ☞[CI1312芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1312_datasheet%20V1.5_chs_20250725.pdf)下载完整的芯片数据手册。

表G-1 芯片信息

| 芯片型号 | FLASH容量 | RAM容量 | 封装信息 |
| --- | --- | --- | --- |
| CI1311 | 1MByte | 640KByte | SOP16(9.9mm\*6.0mm\*1.7mm) |
| CI1312 | 2MByte | 640KByte | SOP16(9.9mm\*6.0mm\*1.7mm) |

CI1311、CI1312可应用的部分产品领域：

* 智能家电
* 智能玩具
* 智能照明
* 智能可穿戴

![CI1312应用框图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI1311%26CI1312%E8%8A%AF%E7%89%87%E5%BA%94%E7%94%A8%E6%A1%86%E5%9B%BE.png)

图G-1 CI1311&CI1312芯片应用框图

芯片特性如下：

* 神经网络处理器BNPU V3
  + 采用3代硬件BNPU技术，支持DNN\TDNN\RNN\CNN等神经网络及并行矢量运算，可实现语音识别、命令词自学习、语音检测及深度学习降噪等功能
* CPU
  + 32位高性能CPU，运行频率最高支持220MHz
  + 32-bit单周期乘法器，支持DSP扩展加速
* 存储器
  + 内置640KB SRAM
  + 内置512bit eFuse
  + 内置2MB Flash
* 音频接口
  + 内置高性能低功耗Audio Codec模块，支持单路ADC采样和单路DAC播放
  + 支持Automatic Level Control (ALC)功能
  + 支持8kHz/16kHz/24kHz/32kHz/44.1kHz/48kHz采样率
* 电源管理单元PMU
  + 内置3个高性能LDO，无需外加电源芯片，外围仅需少量阻容器件
  + 支持5V供电直接输入，供电范围最小支持3.6V输入，最大支持5.5V输入
* 时钟
  + 内置RC振荡器
* 外设和定时器
  + 2路UART接口，最高可支持3M波特率
  + 1路IIC接口，可以外接IIC器件进行扩展
  + 3路PWM接口，灯控和电机类应用可直接驱动
  + 内置4组32-bit timer
  + 内置1组独立看门狗（IWDG）
  + 内置1组窗口看门狗（WWDG）
* GPIO
  + 支持5个GPIO口，可以作为主控IC使用
  + 每个GPIO口可配置中断功能，支持上下拉可配置
  + 每个GPIO支持宽压5V电平信号直接通信，无需外接电平转换但需要外接上拉到5V的电阻
* 软件开发支持
  + 提供完整软件开发包、应用方案示例和语音开发平台在线制作固件等功能，详情请访问：[https://aiplatform.chipintelli.com](https://aiplatform.chipintelli.com/)
* 固件烧录和保护
  + 支持UART升级和固件保护
* EMC和ESD
  + 良好EMC设计，支持FCC标准
  + 内部ESD增强设计，可通过4KV接触放电试验
* ROHS和REACH
  + 采用环保材料，支持通过ROHS和REACH测试
* 封装和工作温度范围
  + 封装形式：SOP16，尺寸为长9.9mm，宽6.0mm，高1.7mm
  + 工作环境温度：-20℃ 到85℃