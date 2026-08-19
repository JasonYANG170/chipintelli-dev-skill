<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI230X_datasheet V0.3_chs_20220902.pdf)

# 芯片概述

CI230X系列芯片是启英泰伦研发的新一代高性能神经网络智能语音芯片，集成了启英泰伦自研的脑神经网络处理器BNPU V3和CPU内核，以及Wi-Fi和BLE的Combo功能。系统主频可达220MHz，内置大容量SRAM，集成PMU电源管理单元、双通道高性能低功耗Audio Codec和多路UART、IIC、IIS、PWM、GPIO等外围控制接口。芯片支持2.4 GHz 802.11 b/g/n Wi-Fi及 BLE 5.1 无线通信协议，仅需少量电阻电容等外围器件就可以实现各类智能语音离在线产品硬件方案，性价比极高。

CI230X系列芯片使用工业级设计标准，具有较高的环境可靠性，芯片工作温度范围在-40°C到 +85°C之间，符合MSL3级湿敏等级，符合IEC 61000-4-2 的4KV接触放电试验标准，符合FCC电磁兼容标准，符合ROHS和REACH环保标准。

CI230X系列芯片采用了启英泰伦的3代BNPU技术，该技术支持DNN\TDNN\RNN\CNN等神经网络及并行矢量运算，可实现语音识别、声纹识别、命令词自学习、语音检测及深度学习降噪等功能，具备强劲的回声消除和环境噪声抑制能力。该芯片方案还支持汉语、英语、日语等多种全球语言，可广泛应用于家电、照明、玩具、可穿戴设备、工业、汽车等产品领域，实现语音交互及控制和各类智能语音方案应用。

当前CI230X系列芯片分CI2305和CI2306两款芯片。这两款芯片引脚完全兼容，仅内置Flash容量不同，CI2305内置4MB Flash，CI2306内置6MB Flash。CI2305因Flash容量较小，仅支持离线语音识别加IOT连接的AIOT类型的应用，CI2306可支持离在线语音识别，且支持更大容量神经网络模型和更多播报音，具备更好的降噪效果。

请点击 ☞[CI230X系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI230X_datasheet%20V0.3_chs_20220902.pdf) 下载完整的芯片数据手册。

表G-1 芯片信息

| 芯片型号 | FLASH容量 | RAM容量 | 封装信息 |
| --- | --- | --- | --- |
| CI2305 | 4MByte | 936KByte | QFN56(7mm\*7mm\*0.85mm) |
| CI2306 | 6MByte | 936KByte | QFN56(7mm\*7mm\*0.85mm) |

CI2306可应用的部分产品领域：

* 智能家电
* 智能玩具
* 智能照明
* 智能可穿戴
* 智能工业
* 智能汽车

![CI2306应用框图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI230X%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-1.png)

图G-1 芯片应用框图

芯片特性如下：

* Wi-Fi

  + 单频2.4GHz IEEE 802.11b/g/n
  + 支持STA/AP/STA+AP模式
  + 片内集成协议栈：TCP/UDP/HTTP/HTTPS/PING/MQTT，TLS support for TCP/UDP/HTTP
  + 片内集成PA/LNA/TRX Switch
* BLE

  + 支持Bluetooth LE 5.1
  + 支持long range (125Kbps, 500Kbps) 及高传输速率 (2Mbps)
* 神经网络处理器BNPU V3

  + 采用3代硬件BNPU技术，支持DNN\TDNN\RNN\CNN等神经网络及并行矢量运算，可实现语音识别、声纹识别、命令词自学习、语音检测及深度学习降噪等功能
* CPU

  + 32位高性能CPU，最高支持220MHz运行频率
  + 32-bit单周期乘法器，支持DSP扩展加速
* 存储器

  + 内置大容量SRAM
  + 内置512bit eFuse
  + 内置4/6MB Flash
* 音频接口

  + 内置高性能低功耗Audio Codec模块，支持双路ADC采样和单路DAC播放
  + 支持Automatic Level Control (ALC)功能
  + 支持8kHz/16kHz/24kHz/32kHz/44.1kHz/48kHz采样率
  + 支持一路IIS音频扩展通路
  + 支持PDM 接口，可对接单个或两个数字MEMS麦克风
* 电源管理单元PMU

  + 内置多个高性能LDO，外围仅需一个电源器件即可工作
  + 支持5V供电直接输入，供电范围最小支持3.6V输入，最大支持5.5V输入
* 加解密

  + 内置AES-128/AES-192/AES-256硬件加解密引擎
  + 内置真随机数发生器
* SAR ADC

  + 2路12bit SAR ADC输入通道，采样频率可达1MHz
* 外设和定时器

  + 2路UART接口，最高可支持3M波特率
  + 1路IIC接口，可以外接IIC器件进行扩展
  + 6路PWM接口，灯控和电机类应用方便直接驱动
  + 内置4组32-bit timer
  + 内置1组独立看门狗（IWDG）
  + 内置1组窗口看门狗（WWDG）
* GPIO

  + 支持33个高速GPIO口，可以作为主控IC使用
  + 除PD对应的GPIO口外，其它GPIO口可配置中断功能
  + 部分GPIO支持宽压5V电平信号直接通信，无需外接电平转换
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

  + 封装形式：QFN56，尺寸为长7mm，宽7mm，高0.85mm
  + 环境工作温度：-40℃ 到85℃