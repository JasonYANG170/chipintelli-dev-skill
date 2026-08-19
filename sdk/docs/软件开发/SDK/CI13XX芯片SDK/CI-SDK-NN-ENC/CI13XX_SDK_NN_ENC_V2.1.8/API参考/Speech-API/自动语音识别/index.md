<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E5%8F%82%E8%80%83/Speech-API/%E8%87%AA%E5%8A%A8%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/ -->

# 离线语音指令识别简介

## **1. 离线语音（指令）识别**

ASR(Automatic Speech Recognition) 自动语音识别,指通过算法将人类语音实时转换为文本的技术,离线语音指令识别是语音识别的一种，离线语音指令识别是一种在 **本地设备** （不依赖云端服务器，不联网）实时处理语音信号，并将其转换为预定义文本指令的技术。它通过设备内置的算法模型直接解析用户说出的关键词或短语，触发相应的本地操作。

![语音识别流程图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E5%8F%82%E8%80%83/Speech-API/img/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B5%81%E7%A8%8B%E5%9B%BE-1.png)

ASR流程图

## **2. 相关术语说明**

**VAD**（*Voice Activity Detection*）: 语音活动检测，识别音频流中人声的开始与结束点;

**唤醒词**（*Wake Word*）：激活语音系统的起始词（如“启英泰伦”），使设备进入指令监听状态；

**命令词**（*Command Word*）：在唤醒后识别，触发具体操作的语音指令（如“打开空调”）；

**OneShot** ：唤醒词+命令词一次性说完（如“启英泰伦打开空调”），无需分两步操作;

**置信度阈值** ：识别结果可信度分数（0~255），高于阈值才触发操作例：阈值=40时，“打开空调”识别分数超过40则执行，可用于平衡误触发率与漏识别率；

**语义槽位** ：从指令中提取的关键参数 例：“调高温度到25度” → 槽位 {动作:调高, 对象:温度, 值:25}，实现复杂指令解析。

## **3. API 参考**