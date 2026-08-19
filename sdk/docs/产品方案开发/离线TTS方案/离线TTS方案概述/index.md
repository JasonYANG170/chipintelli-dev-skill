<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E7%A6%BB%E7%BA%BFTTS%E6%96%B9%E6%A1%88/%E7%A6%BB%E7%BA%BFTTS%E6%96%B9%E6%A1%88%E6%A6%82%E8%BF%B0/ -->

# 离线TTS方案概述

​ 语音合成（Text To Speech，TTS）技术将文本转化为声音，该方案可以实现无网络状态下，通过串口协议，将接收到的文本转换成语音播报，目前广泛应用于叫号系统，考勤机，玩具、地图导航等场景。基于我司CI1103、CI1302、CI1303、CI1306语音芯片开发的端侧语音合成模块，是一套极具性价比的语音合成方案。

## 结构框图

下面是系统结构框图

![1](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E7%A6%BB%E7%BA%BFTTS%E6%96%B9%E6%A1%88/img/%E7%A6%BB%E7%BA%BFTTS%E6%96%B9%E6%A1%88%E6%A6%82%E8%BF%B01.png)

## 离线TTS案例

### 公交报站器开发案例：

![2](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E7%A6%BB%E7%BA%BFTTS%E6%96%B9%E6%A1%88/img/%E7%A6%BB%E7%BA%BFTTS%E6%96%B9%E6%A1%88%E6%A6%82%E8%BF%B03.png)

报站器MCU可通过串口发送站名文本到我司端侧语音合成模块，经过功放输出到喇叭，进行播放，并且句末停顿，语气舒缓。

### 考勤机开发案例：

![3](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E7%A6%BB%E7%BA%BFTTS%E6%96%B9%E6%A1%88/img/%E7%A6%BB%E7%BA%BFTTS%E6%96%B9%E6%A1%88%E6%A6%82%E8%BF%B02.png)

考勤机MCU可通过串口发送员工姓名文本到我司端侧语音合成模块，经过功放输出到喇叭，进行播放。