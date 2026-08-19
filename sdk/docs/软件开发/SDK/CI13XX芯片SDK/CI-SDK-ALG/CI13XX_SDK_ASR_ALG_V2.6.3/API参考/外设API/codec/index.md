<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/codec/ -->

# 多媒体音频编解码器(CODEC)

---

## 1、简介

![CODEC系统结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/CODEC%E7%B3%BB%E7%BB%9F%E7%BB%93%E6%9E%84%E5%9B%BE-1.png)

CODEC系统结构图

* CI13XX内置高性能低功耗音频CODEC，支持两路ADC、一路DAC，MIC输入的模拟信号经MIC增益，再经PGA放大。此PGA可通过CODEC本身的ALC控制，PGA之后，还可通过数字增益进行放大。

---

## 2、特性

* DAC支持最多24bit，SNR可达90dB；
* ADC支持最多24bit，SNR可达90dB；
* 支持单端、差分的MIC输入和line-in输入；
* 自带ALC自动增益控制；
* 采样率支持：8k/12k/16k/24k/32k/44.1k/48k；

---

## 3、使用示例

CODEC使用示例请查阅录☞[《音频系统管理文档》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/Speech-API/%E9%9F%B3%E9%A2%91%E7%B3%BB%E7%BB%9F%E7%AE%A1%E7%90%86/)。

## 4、API 参考