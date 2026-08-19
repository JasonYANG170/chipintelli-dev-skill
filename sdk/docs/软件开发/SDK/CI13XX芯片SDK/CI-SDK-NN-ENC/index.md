<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/ -->

# 语音通话降噪SDK（CI-SDK-NN-ENC）

---

## **概述**

目前CI13XX系列芯片的降噪算法SDK（CI-SDK-NN-ENC）最新发布版本为：CI13XX\_SDK\_NN\_ENC\_V2.1.8

下面介绍前端音频处理算法，主要包括如下算法：

| NN DENOISE | 深度降噪算法 |
| --- | --- |
| AEC | 回声消除算法 |
| AGC | 自动增益控制算 |
| ALC | 自动电平控制算法 |
| DRC | 动态范围控制算法 |
| EQ | 均衡器算法 |
| AHS | 啸叫抑制算法 |
| RECORD | 录音功能 |

---

* AEC （回声消除算法），☞[《回声消除算法实用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/aec%E7%AE%97%E6%B3%95/)
* AGC（自动增益控制），☞[《自动增益控制使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/agc%E7%AE%97%E6%B3%95/)
* ALC （自动电平控制），☞[《自动电平控制使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/alc%E7%AE%97%E6%B3%95/)
* DRC （动态范围控制），☞[《动态范围控制使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/drc%E7%AE%97%E6%B3%95/)
* EQ（均衡器算法），☞[《均衡器算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/eq%E7%AE%97%E6%B3%95/)
* AHS （啸叫抑制算法），☞[《啸叫抑制算法使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ahs%E7%AE%97%E6%B3%95/)
* RECORD （录音功能），☞[《录音功能使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%BD%95%E9%9F%B3%E5%8A%9F%E8%83%BD/)

---

## **降噪算法SDK适配的AI语音芯片型号**

* 芯片型号（☞[启英商城](http://mall.chipintelli.com/chip?product_category=45&brd=1)有售）：

CI 1301 、 CI 1302 、 CI 1303 、 CI 1306 、 CI 1311 、 CI 1312。

* 模块型号（☞[启英商城](http://mall.chipintelli.com/chip?product_category=45&brd=1)有售）：

CI-D01GS01J单麦离线语音降噪端子模块

CI-D02GS01J单麦离线语音降噪端子模块

CI-D03GS02S单麦离线语音降噪贴片模块

CI-D02GS02S单麦离线语音降噪贴片模块

CI-D02GS07J-BT单麦离线语音降噪蓝牙端子模块

CI-D02GS04U离线语音空调控制器

* 开发板套件型号（☞[启英商城](http://mall.chipintelli.com/chip?product_category=45&brd=1)有售）：

CI1306开发板套件

CI1303开发板套件

CI1302开发板套件