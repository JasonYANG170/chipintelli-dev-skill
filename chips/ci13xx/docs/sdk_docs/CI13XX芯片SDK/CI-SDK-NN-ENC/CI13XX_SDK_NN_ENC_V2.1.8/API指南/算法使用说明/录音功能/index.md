<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%BD%95%E9%9F%B3%E5%8A%9F%E8%83%BD/ -->

# 录音功能

## 1. 录音功能介绍

通话降噪SDK录音功能用于存储两类音频数据，第一类是mic采集的原始音频数据；第二类是mic采集的原始音频经过降噪处理后的音频数据；录音存储方式为循环存储(当音频存储空间满以后会从头覆盖音频数据存储)，最多能存20条音频数据(共20分钟)；支持音频存储和播放存储音频功能，同时支持本地提示音播放(speex/pcm)。

## 2. 存储音频格式

存储音频格式为8K 16bit 经过speex编码后的音频数据。通话降噪SDK已提供了speex的编码和解码功能。

## 3.具体录音存储功能及协议说明请参考☞[启英泰伦-通话降噪SDK录音功能使用说明\_V1.0.pdfx](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E5%8D%8F%E8%AE%AE/%E9%80%9A%E8%AF%9D%E9%99%8D%E5%99%AASDK%E5%BD%95%E9%9F%B3%E5%8A%9F%E8%83%BD%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E_V1.0.pdf)

## 4. 注意事项：

产品如果带录音功能，给降噪IC供电的电压需要严格满足硬件设计要求（3.3V供电方案：电压范围3.15V-3.45V，纹波小于50mv；5V供电方案：电压范围3.6V-5V，纹波小于3000mv），不然会有损坏FLASH中数据的风险