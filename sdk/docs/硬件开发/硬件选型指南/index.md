<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E7%A1%AC%E4%BB%B6%E9%80%89%E5%9E%8B%E6%8C%87%E5%8D%97/ -->

请点击保存PDF文档

# 硬件选型指南

本文档将芯片和模块的主要功能参数以表格方式列举供选型使用。用户可以点击下述表格中芯片数据手册行里面的内容和模块的型号，进入对应的数据手册文档页面。

## AI语音芯片选型

备注

**可以点击下表中数据手册行中的内容进入对应的芯片数据手册页面。**

另外我们还提供了相应工具供开发者进行芯片的选择与对比：

☞[产品选型工具](https://www.chipintelli.com/zh-cn/page/128.html)

☞[产品对比工具](https://www.chipintelli.com/zh-cn/page/129.html)

---

### CI23LC系列

**三点五代AI语音BLE芯片硬件参数选型表**

| 硬件参数 | CI23161 | CI23162 | CI23242 |
| --- | --- | --- | --- |
| 数据手册 | [CI23LC系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI23161%26CI23162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI23LC系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI23161%26CI23162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI23242芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI23242%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) |
| BNPU版本 | BNPU V3.5 | BNPU V3.5 | BNPU V3.5 |
| CPU主频 | 210MHz | 210MHz | 210MHz |
| SRAM | 288KB | 288KB | 288KB |
| FLASH | 内置1MB | 内置2MB | 内置2MB |
| CODEC | Mono Codec SNR>95dB | Mono Codec SNR>95dB | Mono Codec SNR>95dB |
| UART接口数 | 1 | 1 | 2 |
| PWM接口数 | 3 | 3 | 4 |
| IIC接口数 | 1 | 1 | 1 |
| GPIO数量（含复用引脚） | 3 | 3 | 11 |
| VDDRF供电（蓝牙供电） | 外置 | 外置 | 外置 |
| VDD33供电 | 内置 | 内置 | 外置 |
| VDD11供电 | 内置 | 内置 | 内置 |
| 晶振 | 外置 | 外置 | 内置 |
| 封装 | SOP-16 | SOP-16 | SSOP24 |

**三点五代AI语音BLE芯片应用功能选型表**

| 应用功能 | CI23161 | CI23162 | CI23242 |
| --- | --- | --- | --- |
| 数据手册 | [CI23LC系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI23161%26CI23162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI23LC系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI23161%26CI23162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI23242芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI23242%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) |
| 本地语音识别 | 支持 | 支持 | 支持 |
| 支持语言 | 中、英、日、韩 | 中、英、日、韩 | 中、英、日、韩 |
| 离线词条数量 | 100 | 300 | 300 |
| 单麦降噪 | 支持 | 支持 | 支持 |
| AEC播放打断 | 不支持 | 不支持 | 不支持 |
| 双麦增强 | 不支持 | 不支持 | 不支持 |
| 双麦定向 | 不支持 | 不支持 | 不支持 |
| 本地自学习 | 支持 | 支持 | 支持 |
| 本地声纹识别（同时支持本地语音识别） | 不支持 | 支持 | 支持 |
| 离线自然说 | 支持 | 支持 | 支持 |
| 单/双麦克风 | 单麦 | 单麦 | 单麦 |
| ADPCM解码 | 不支持 | 支持 | 支持 |
| MP3解码 | 支持 | 支持 | 支持 |

---

### CI13LC系列

**三点五代AI语音芯片硬件参数选型表**

| 硬件参数 | CI13082V | CI13161 | CI13162 | CI13162P | CI13241 | CI13242 | CI13322 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 数据手册 | [CI1308XV系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13082V%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI13161数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13161%26CI13162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI13162数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13161%26CI13162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1316XP系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13162P%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI13241数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13241%26CI13242%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI13242数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13241%26CI13242%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI13322数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13322%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") |
| BNPU版本 | BNPU V3.5 | BNPU V3.5 | BNPU V3.5 | BNPU V3.5 | BNPU V3.5 | BNPU V3.5 | BNPU V3.5 |
| CPU主频 | 210MHz | 210MHz | 210MHz | 210MHz | 210MHz | 210MHz | 210MHz |
| SRAM | 288KB | 288KB | 288KB | 288KB | 288KB | 288KB | 288KB |
| DRAM | / | / | / | / | / | / | / |
| FLASH | 内置2MB | 内置1MB | 内置2MB | 内置2MB | 内置1MB | 内置2MB | 内置2MB |
| CODEC | Mono Codec SNR>95dB | Mono Codec SNR>95dB | Mono Codec SNR>95dB | Mono Codec SNR>95dB | Mono Codec SNR>95dB | Mono Codec SNR>95dB | Mono Codec SNR>95dB |
| UART接口数 | 1 | 2 | 2 | 2 | 3 | 3 | 3 |
| I2S接口数 | 0 | 0 | 0 | 0 | 1 | 1 | 1 |
| PWM接口数 | 4 | 3 | 3 | 3 | 4 | 4 | 4 |
| IIC接口数 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
| GPIO数量（含复用引脚） | 4 | 6 | 6 | 4 | 11 | 11 | 19 |
| 功放 | 外置 | 外置 | 外置 | **内置** | 外置 | 外置 | 外置 |
| VCC33供电 | **外置** | 内置 | 内置 | 内置 | 内置 | 内置 | 内置 |
| VCC11供电 | 内置 | 内置 | 内置 | 内置 | 内置 | 内置 | 内置 |
| 晶振 | 内置 | 内置，支持外接 | 内置，支持外接 | 内置 | 内置 ，支持外接 | 内置 ，支持外接 | 内置 ，支持外接 |
| 封装 | SOP8 | SOP16 | SOP16 | SOP-16 | SSOP24 | SSOP24 | QFN32-4X4 |

**三点五代AI语音芯片应用功能选型表**

| 应用功能 | CI13082V | CI13161 | CI13162 | CI13162P | CI13241 | CI13242 | CI13322 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 数据手册 | [CI13082V系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13082V%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI13161数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13161%26CI13162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI13162数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13161%26CI13162%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1316XP系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13162P%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI13241数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13241%26CI13242%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI13242数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13241%26CI13242%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI13322数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13322%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") |
| 本地语音识别 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| 支持语言 | 中、英、日、韩 | 中、英 | 中、英 | 中、英、日、韩 | 中、英 | 中、英 | 中、英 |
| 离线词条数量 | 300 | 100 | 300 | 300 | 100 | 300 | 300 |
| 单麦降噪 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| AEC播放打断 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 |
| 双麦增强 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 |
| 双麦定向 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 |
| 本地自学习 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| 本地声纹识别(同时支持本地语音识别) | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 | 不支持 |
| 离线自然说 | 不支持 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| 单/双麦克风 | 单麦 | 单麦 | 单麦 | 单麦 | 单麦 | 单麦 | 单麦 |
| ADPCM解码 | 支持 | 不支持 | 不支持 | 支持 | 不支持 | 不支持 | 不支持 |
| MP3解码 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| 外置IOT模块 | 不支持 | 支持 | 支持 | 不支持 | 支持 | 支持 | 支持 |

---

### CI13XX系列

**三代AI语音芯片硬件参数选型表**

| 硬件参数 | CI1301 | CI1302 | CI1303 | CI1306 | CI1311 | CI1312 |
| --- | --- | --- | --- | --- | --- | --- |
| 数据手册 | [CI1301数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1302数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1303数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1306数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1311数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1312数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") |
| BNPU版本 | BNPU V3 | BNPU V3 | BNPU V3 | BNPU V3 | BNPU V3 | BNPU V3 |
| CPU主频 | 220MHz | 220MHz | 220MHz | 240MHz | 220MHz | 220MHz |
| SRAM | 640KB | 640KB | 640KB | 640KB | 640KB | 640KB |
| DRAM | / | / | / | / | / | / |
| FLASH | 内置1MB | 内置2MB | 内置4MB | 内置4MB | 内置1MB | 内置2MB |
| CODEC | Stereo Codec SNR>95dB | Stereo Codec SNR>95dB | Stereo Codec SNR>95dB | Stereo Codec SNR>95dB | Mono Codec SNR>95dB | Mono Codec SNR>95dB |
| UART接口数 | 3 | 3 | 3 | 3 | 2 | 2 |
| I2S接口数 | 1 | 1 | 1 | 1 | 0 | 0 |
| PWM接口数 | 6 | 6 | 6 | 6 | 3 | 3 |
| IIC接口数 | 1 | 1 | 1 | 1 | 1 | 1 |
| PDM | 1 | 1 | 1 | 1 | 0 | 0 |
| SAR ADC通道数 | 1 | 1 | 1 | 4 | 0 | 0 |
| GPIO数量（含复用引脚） | 10 | 10 | 10 | 26 | 5 | 5 |
| VCC33供电 | 内置 | 内置 | 内置 | 内置 | 内置 | 内置 |
| VCC11供电 | 内置 | 内置 | 内置 | 内置 | 内置 | 内置 |
| 晶振 | 内置，支持外接 | 内置，支持外接 | 内置，支持外接 | 内置，支持外接 | 内置 | 内置 |
| 封装 | SSOP24 | SSOP24 | SSOP24 | QFN40 | SOP16 | SOP16 |

备注

***注：CI1301、CI1302和CI1303管脚和封装完全兼容，区别为CI1303的Flash更大，可以支持更大的算法模型和更多的软件功能。***

**三代AI语音芯片应用功能选型表**

| 应用功能 | CI1301 | CI1302 | CI1303 | CI1306 | CI1311 | CI1312 |
| --- | --- | --- | --- | --- | --- | --- |
| 数据手册 | [CI1301数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1302数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1303数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1306数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1311数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") | [CI1312数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/ "点击查看数据手册") |
| 本地语音识别 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| 支持语言 | 中、英、日、韩 | 中、英、日、韩 | 中、英、日、韩 | 中、英、日、韩 | 中、英、日、韩 | 中、英、日、韩 |
| 词条数量 | 100 | 300 | 500+ | 500+ | 100 | 300 |
| 单麦降噪 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| AEC播放打断 | 支持 | 支持 | 支持 | 支持 | 不支持 | 不支持 |
| 双麦增强 | 不支持 | 支持 | 支持 | 支持 | 不支持 | 不支持 |
| 双麦定向 | 不支持 | 支持 | 支持 | 支持 | 不支持 | 不支持 |
| 本地自学习 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| 本地声纹识别（同时支持本地语音识别） | 不支持 | 支持 | 支持 | 支持 | 不支持 | 不支持 |
| 离线自然说（十万级自然说法） | 不支持 | 支持 | 支持 | 支持 | 不支持 | 支持 |
| 单/双麦克风 | 单麦 | 单/双麦 | 单/双麦 | 单/双麦 | 单麦 | 单麦 |
| ADPCM解码 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| MP3解码 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |
| 外置IOT模块 | 支持 | 支持 | 支持 | 支持 | 支持 | 支持 |

---

---

### CI230X系列

**三代AI语音Wi-Fi Combo芯片硬件参数选型表**

| 硬件参数 | CI2305 | CI2306 |
| --- | --- | --- |
| 数据手册 | [CI230X系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI230X系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) |
| BNPU版本 | BNPU V3 | BNPU V3 |
| CPU主频 | 220MHz | 220MHz |
| SRAM | 936KB | 936KB |
| DRAM | / | / |
| FLASH | 内置4MB | 内置6MB |
| CODEC | Stereo Codec SNR>95dB | Stereo Codec SNR>95dB |
| UART接口数 | 2 | 2 |
| I2S接口数 | 1 | 1 |
| SDIO接口数 | / | / |
| PWM接口数 | 6 | 6 |
| SPI接口数 | / | / |
| IIC接口数 | 1 | 1 |
| PDM | 1 | 1 |
| SAR ADC通道数 | 2 | 2 |
| GPIO数量（含复用引脚） | 33 | 33 |
| VDDA33供电（Wi-Fi供电） | 外置 | 外置 |
| VDD33供电 | 内置 | 内置 |
| VDD11供电 | 内置 | 内置 |
| 晶振 | 内置，支持外接 | 内置，支持外接 |
| 封装 | QFN56-7x7 | QFN56-7x7 |

**三代AI语音Wi-Fi Combo芯片应用功能选型表**

| 应用功能 | CI2305 | CI2306 |
| --- | --- | --- |
| 数据手册 | [CI230X系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI230X系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) |
| 在线语音识别 | 不支持 | 支持 |
| 本地语音识别 | 支持 | 支持 |
| 支持语言 | 中、英、日、韩 | 中、英、日、韩 |
| 离线词条数量 | 200 | 200 |
| 单麦降噪 | 支持 | 支持 |
| AEC播放打断 | 支持 | 支持 |
| 双麦增强 | 不支持 | 支持 |
| 双麦定向 | 不支持 | 不支持 |
| 本地自学习 | 支持 | 支持 |
| 本地声纹识别（同时支持本地语音识别） | 支持 | 支持 |
| 离线自然说 | 不支持 | 不支持 |
| 单/双麦克风 | 单麦 | 单/双麦 |
| ADPCM解码 | 不支持 | 支持 |
| MP3解码 | 支持 | 支持 |

---

### CI112X系列

**二点五代AI语音芯片硬件参数选型表**

| 硬件参数 | CI1122 |
| --- | --- |
| 数据手册 | [CI1122数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1122%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) |
| BNPU版本 | BNPU V2 |
| CPU主频 | 180MHz |
| SRAM | 512KB |
| DRAM | / |
| FLASH | 内置4MB |
| CODEC | Mono Codec SNR>85dB |
| UART接口数 | 2 |
| I2S接口数 | 1 |
| SDIO接口数 | / |
| PWM接口数 | 6 |
| SPI接口数 | / |
| IIC接口数 | 1 |
| PDM | / |
| SAR ADC通道数 | 4 |
| GPIO数量(含复用引脚) | 27 |
| VCC33供电 | LDO 3.3V |
| VCC12供电 | DCDC 1.2V |
| 晶振 | 12.288晶振 |
| 封装 | QFN48 |

**二点五AI语音芯片应用功能选型表**

| 应用功能 | CI1122 |
| --- | --- |
| 数据手册 | [CI1122数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1122%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) |
| 本地语音识别 | 支持 |
| 支持语言 | 中、英、日、韩 |
| 词条数量 | 100~200 |
| 单麦降噪 | 支持 |
| AEC播放打断 | 不支持 |
| 双麦增强 | 不支持 |
| 双麦定向 | 不支持 |
| 本地自学习 | 支持 |
| 本地声纹识别 | 不支持 |
| 离线自然说 | 不支持 |
| 单/双麦克风 | 单麦 |
| ADPCM解码 | 支持 |
| MP3解码 | 支持 |
| 外置IOT模块 | 支持 |

---

### CI110X系列

**二代AI语音芯片硬件参数选型表**

| 硬件参数 | CI1102 | CI1103 |
| --- | --- | --- |
| 数据手册 | [CI1102数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1102%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI1103数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1103%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) |
| BNPU版本 | BNPU V2 | BNPU V2 |
| CPU主频 | 168MHz | 168MHz |
| SRAM | 512KB | 512KB |
| DRAM | / | 2MB |
| FLASH | 外置 | 外置 |
| CODEC | Stereo Codec SNR>85dB | Stereo Codec SNR>85dB |
| UART接口数 | 3 | 3 |
| I2S接口数 | 1 | 1 |
| SDIO接口数 | 1 | 1 |
| PWM接口数 | 6 | 6 |
| SPI接口数 | 1 | 1 |
| IIC接口数 | 2 | 2 |
| PDM | / | / |
| SAR ADC通道数 | 4 | 4 |
| GPIO数量(含复用引脚) | 38 | 38 |
| VCC33供电 | LDO 3.3V | LDO 3.3V |
| VCC12供电 | DCDC 1.2V | DCDC 1.2V |
| 晶振 | 12.288晶振 | 12.288晶振 |
| 封装 | QFN56 | QFN56 |

备注

***注：CI1102和CI1103管脚和封装完全兼容，区别为CI1103相比CI1102多了2MB的DRAM，可以做更多的本地命令词条和更多的算法功能。***

**二代AI语音芯片应用功能选型表**

| 应用功能 | CI1102 | CI1103 |
| --- | --- | --- |
| 数据手册 | [CI1102数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1102%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) | [CI1103数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1103%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) |
| 本地语音识别 | 支持 | 支持 |
| 支持语言 | 中、英、日、韩 | 中、英、日、韩 |
| 词条数量 | 100~200 | 300+ |
| 单麦降噪 | 支持 | 支持 |
| AEC播放打断 | 支持 | 支持 |
| 双麦增强 | 不支持 | 支持 |
| 双麦定向 | 不支持 | 支持 |
| 本地自学习 | 不支持 | 支持 |
| 本地声纹识别 | 不支持 | 支持 |
| 离线自然说 | 不支持 | 不支持 |
| 单/双麦克风 | 单/双麦 | 单/双麦 |
| ADPCM解码 | 支持 | 支持 |
| MP3解码 | 支持 | 支持 |
| 外置IOT模块 | 支持 | 支持 |

备注

1、开启一些算法功能后，词条数量可能会小于标注值。

2、外部增加IOT模块后，根据不同模块型号支持的词条数量有差异。

3、CI1301、CI1302和CI1312开启本地声纹识别功能后，不能同时支持本地语音识别，如果要同时支持本地声纹识别和本地语音识别，请采用CI1303和CI1306。

---

## AI语音模块选型指南

以下模块稳定供货，大批量需提前联系我司订购，点击下方 **模块数据手册概述** 查看详情。

模块数据手册概述

* ☞[模块数据手册概述](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C%E6%A6%82%E8%BF%B0/)

---

## 购买方法

用户如果要采购我司产品样品，请点击 ☞[样品购买](http://mall.chipintelli.com/) ，也可以点击 ☞[样品和批量采购](https://document.chipintelli.com/%E6%A0%B7%E5%93%81%E5%92%8C%E6%89%B9%E9%87%8F%E9%87%87%E8%B4%AD/) 获取更多信息。

如果您在使用中有任何问题，欢迎通过以下方式和我司联系。

商务电话：028-61375925 或 18161228763

商务邮箱：[support@chipintelli.com](mailto:support@chipintelli.com)

技术咨询：技术交流QQ群（127468697）