<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E7%AE%97%E6%B3%95%E5%A4%84%E7%90%86%E5%90%8E%E7%9A%84%E9%9F%B3%E9%A2%91%E9%87%87%E9%9B%86/ -->

# 音频数据采集说明

本文将详细讲述mic采集语音数据后，经过语音芯片算法处理，用户如何采集到原始音频和处理后的音频数据。

**以双MIC AEC+降噪算法为例，MIC采集的音频数据经过AEC+降噪算法处理后输出：**

**1.算法功能和参数配置步骤如下：**

**1.1配置算法功能**

打开CI13XX\_SDK\_LLM\_AIoT\_Vx.x.x\project\_file\makefile文件，将CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_AEC\_DENOISE\_NN),如下图：
![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/AEC_DENOISE_NN.png)

**1.2采音参数配置说明：**

打开CI13XX\_SDK\_LLM\_AIoT\_Vx.x.x\projects\nn\_denoise\_rtc\_sample\app\app\_main\ci\_ssp\_config.c,找到iis\_out\_audio\_config结构体，该结构体中iis\_left\_channel和iis\_right\_channel成员变量分别控制iis左通道和右通道的数据输出类型，其他参数不用关注，如下图：
![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E9%87%87%E9%9F%B3%E8%BE%93%E5%87%BA%E9%85%8D%E7%BD%AE.png)

**iis\_left\_channel变量配置表说明如：**

| iis\_left\_channel变量 | 变量值说明 |
| --- | --- |
| MICL | iis左通道输出MICL的原始音频 |
| MICR | iis左通道输出MICR的原始音频 |
| REFL | iis左通道输出参考信号左通道的音频 |
| REFR | iis左通道输出参考信号右通道的音频 |
| DST1 | iis左通道输出MICL经过算法处理后的音频 |
| DST2 | iis左通道输出MICR经过算法处理后的音频 |

---

**iis\_right\_channel变量配置表说明如：**

| iis\_right\_channel变量 | 变量值说明 |
| --- | --- |
| MICL | iis右通道输出MICL的原始音频 |
| MICR | iis右通道输出MICR的原始音频 |
| REFL | iis右通道输出参考信号左通道的音频 |
| REFR | iis右通道输出参考信号右通道的音频 |
| DST1 | iis右通道输出MICL经过算法处理后的音频 |
| DST2 | iis右通道输出MICR经过算法处理后的音频 |

**1.3 IIS采音宏使能**
打开CI13XX\_SDK\_LLM\_AIoT\_Vx.x.x\projects\nn\_denoise\_rtc\_sample\app\app\_main\user\_config.h;将USE\_IIS1\_OUT\_PRE\_RSLT\_AUDIO宏配置为1，如下图：
![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/iis%E9%87%87%E9%9F%B3.png)

**2.SDK编译和固件下载：**

SDK和固件编译请参考:☞[SDK快速开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/)

**3.采音板、语音芯片模组板，PC连接以及采音分析,请参考☞[启英泰伦-采音板操作说明及语音模块板底噪分析\_V1.3.docx](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%B7%A5%E5%85%B7/%E5%90%AF%E8%8B%B1%E6%B3%B0%E4%BC%A6-%E9%87%87%E9%9F%B3%E6%9D%BF%E6%93%8D%E4%BD%9C%E8%AF%B4%E6%98%8E%E5%8F%8A%E8%AF%AD%E9%9F%B3%E6%A8%A1%E5%9D%97%E6%9D%BF%E5%BA%95%E5%99%AA%E5%88%86%E6%9E%90_V1.3.docx)**

**4.降噪和非降噪音频采集数据：**
![降噪前后音频](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E9%99%8D%E5%99%AA%E5%89%8D%E5%90%8E.png)

**5.iis左右通道数据输出**

如果用户想拿到iis左右通道数据，做其他应用，可以参考如下说明：
打开CI13XX\_SDK\_LLM\_AIoT\_Vx.x.x\components\audio\_pre\_rslt\_iis\_out\ci130x\_audio\_pre\_rslt\_out.c；找到对应函数audio\_pre\_rslt\_write\_data, 如下图：
![IIS音频输出](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/iis%E9%9F%B3%E9%A2%91%E6%95%B0%E6%8D%AE%E8%BE%93%E5%87%BA.png)

audio\_pre\_rslt\_write\_data函数的输出参数地址left和地址right分别对应iis左右通道输出的数据，用户可以取left和right数据做对应的二次应用开发；结合本文1.2小节对iis\_left\_channel和iis\_right\_channel的配置，可知当前地址left输出的是MICL的原始数据，地址right输出的MICL经过算法处理降噪后的音频数据。