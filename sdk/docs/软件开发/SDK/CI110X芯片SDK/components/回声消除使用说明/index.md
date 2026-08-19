<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# 回声消除使用说明

---

## 1. **概述**

|  |  |
| --- | --- |
| ``` 1 ``` | ``` AEC是声学回声消除(Acoustic Echo Cancellation)的简称，该算法在有参考的情况下，自适应追踪回声路径的变换、实时抑制扬声器到达麦克风终端的回声信号，以提升目标语音的识别效果，根据实际应用场景评估是否开启AEC。开启该功能后，当存在较长的播放内容，或者在播放音频媒体资源时候（例如音箱播放MP3歌曲），这时也可以进行语音识别打断。AEC开启后，需要额外占用40KB的代码空间，并且会消耗CPU带宽，对于1102来说，会降低支持的最大词条数量。 ``` |

本文档主要介绍AEC适用的场景，以及如何开启该功能。

### 1.1 **AEC软件平台**

以下SDK可以支持AEC功能不同的sdk和sample中，AEC功能默认开关是不一样的，请注意再代码中查看：
\* 纯离线SDK：CI110X\_SDK\_ASR\_OfflineV1.2及以上版本，AEC功能默认关闭。
\* 算法SDK：CI110X\_SDK\_ALG\_Application\_V1.0及以上版本，AEC功能默认关闭。
\* 云加端SDK：CI110X\_SDK\_Combine\_Cloud\_V1.5及以上版本，AEC功能默认开启。

### 1.2\*\*支持AEC的硬件\*\*

不同的AEC硬件与不同的sdk是配合使用的，如下：

* 纯离线SDK配合的硬件：CIB02GS01 or CIB02CS01 启英标准模块板，或者其他基于CI1102 or CI1103 开发的带AEC参考线路的模块板。
* 算法SDK配合的硬件：CI1102 or CI1103 芯片开发的带有AEC参考信号线路的模块。
* 云加端SDK配合的硬件：CIB03ST01J-WL21 ，或者其他方案商开发的基于CI1103并且具有AEC 参考线路的模块板。
* 录音板：录音板用于抓取声音波形，从而分析AEC的效果情况，I2StoUSBToolV2\_0（微信搜索：启英商城–录音板）。

---

## 2. **应用指南**

### 2.1 **AEC应用原理框图**

AEC的应用原理示意框图如下，参考信号经过喇叭产生播放信号B，人声为目标语音信号A，当播放声音时，信号B和信号A在应用环境进行复杂的混合后进入芯片，AEC算法根据参考信号和混合后的信号，抑制信号B，提高信号A的信噪比，再进入语音识别引擎，从而提升识别效果。

![AEC应用原理示意框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-1.png)

图2-1 回声消除原理框图

AEC参考信号（及喇叭播放的信号）有以下两种来源方式：

* 参考来源于语音模块本身的喇叭播放，这种应用简称内部AEC；
* 参考来源于外部其他播放器的喇叭声音播放，这种应用简称外部AEC。

### 2.2 **AEC软件接口使用介绍**

#### 2.2.1 **初始化**

```
/**
     * @brief                       创建模块
     * @param fft_size              默认设置512点的fft取对称256点; 
     * @param mic_channel_num       mic通道数默认为1;
     * @param ref_channel_num       参考信号的通道数默认为1;
     * @return void*                NULL:失败;其它:成功.
     */
void* ci_adapt_aec_create(int fft_size, int mic_channel_num, int ref_channel_num);
```

#### 2.2.2 **AEC处理模块**

```
/**
     * @brief                     AEC处理模块
     * @param handle              模块句柄;
     * @param fft_mic_in          mic信号的频域输入;
     * @param fft_ref_in          ref参考信号频域输入;
     * @param fft_out             AEC处理信号的频域输出;
     * @return int                0: AEC算法处理模块正常运行；其它：AEC算法处理过程出现异常.
     */
int ci_adapt_aec_deal(void* handle, float** fft_mic_in, float** fft_ref_in, float** fft_out);
```

### 2.3 **内部AEC使用介绍**

#### 2.3.1 **硬件线路说明**

* 单麦方案，通过CI110X系列芯片的另外一路未使用的codec通道(micpr)作为AEC参考信号输入通道。
* 低音量情况下功放推荐采用AB类功放，例如SMG4890；大音量情况下功放推荐采用D类功放。
* AEC反馈信号为标准I2S，16bits 16K采样率，满幅电压3.3V，则最大音量下参考分压电路后的电压幅值范围为100-150mv。

![内部AEC参考硬件线路图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-2.png)

图2-2 内部AEC参考硬件线路图

**补充说明：**

如果是贵司自行设计硬件，应用上需要注意以下事项，我司可以协助贵司进行原理图check：
\* 如为立体声双声道，需采用双通道codec对立体声回采后作为AEC参考信号处理
\* 如果采用D类功放，则需要增加滤波线路（参考后续外部AEC消除中的D类功放滤波线路）。

#### 2.3.2 **内部AEC的软件开启设置**

AEC开启功能时，只需要打开对应宏定义即可。

* 路径：sample\internal\sample\_XXX\src\user\_config.h

开启AEC：

```
#define USE_SINGLE_MIC_AEC                1
```

关闭AEC：

```
#define USE_SINGLE_MIC_AEC                0
```

![内部AEC参考硬件线路图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-3.png)

### 2.4 **外部AEC使用介绍**

外部AEC应用中，如果是单mic应用，外部参考信号可以从MIC R以模拟信号的方式进行输入；如果是双mic应用，需要外置codec对参考信号进行采音，同时功放的信号输出类型（数字、模拟）的不同会导致外围硬件线路不一致，以下从功放的分类分别说明如何应用。

#### 2.4.1 **功放输出模拟信号**

**1) 硬件原理**

* AEC参考信号为标准I2S，16bits 16K采样率。
* 功放输出信号为模拟信号（例如 AB类功放），参考信号通路采用adc （顺鑫es7243E，单端输入）作为参考信号的采样（如下图）。满幅电压为3.3V，最大音量下参考分压电路后的电压幅值范围为100-150mv。

分压电路可参考下图：

![内部AEC参考硬件线路图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-4.png)

![内部AEC参考硬件线路图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-5.png)

图2-3 模拟功放相关电路图

**2) 软件配置**

AEC开启功能时，除了打开对应宏定义外，还需要将外部播放器的状态同步给语音模块的CI110X系列芯片的GPIO口。请按下面的1-2流程进行配置。

**开启AEC功能：**

* 在sample\internal\sample\_XXX\src\user\_config.h。

开启AEC：

```
#define USE_SINGLE_MIC_AEC                1
```

关闭AEC：

```
#define USE_SINGLE_MIC_AEC                0
```

开启AEC的同时，需要在此user\_config.h文件中打开使用多个codec的宏：

```
#defineAUDIO_CAPTURE_USE_MULTI_CODEC        1（此宏在platform\sdk_default_config.h默认定义为0，此处设为1表明使用了外部CODEC）
```

同理，关闭AEC时，也需要关闭这个宏，将其置为0即可。

![内部AEC参考硬件线路图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-6.png)

**将播放器的播放状态同步给AEC模块：**

在sample\internal\sample\_XXX\src\system\_msg\_deal.c中check\_current\_playing为aec模块获取播放状态的函数，外部播放器需要通过GPIO（或者串口改变播放状态变量）的方式进行告知aec模块。

如下为GPIO方式告知aec模块的例程：（外部播放器播放音乐的时候将对应GPIO的pin拉高，停止播放的时候将GPIO的pin拉低）：

```
bool check_current_playing(void)
{
  //  if(AUDIO_PLAY_STATE_IDLE != get_audio_play_state()) //内部播放时，告知aec模块状态
if(gpio_get_input_level_single(GPIO2,gpio_pin_7)==1)//当为高时表示外部播放器正在播放
    {
        return true;
    }
    else
    {
        return false;
    }
}
```

#### 2.4.2 **功放输出数字信号**

**1) 硬件原理**

* AEC反馈信号为标准I2S，16bits 16K采样率。
* 功放输出信号为数字信号（如D类功放），参考信号通路采用adc（例如AD51050，顺鑫es7243E，差分输入）作为参考信号的采样（如下图）。满幅电压为3.3V，最大音量下参考分压电路后的电压幅值范围为100-150mv。

分压电路可参考下图：

![内部AEC参考硬件线路图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-7.png)

图2-4 数字功放AEC相关电路图

* 如产品（or 语音模组）播放的音源为双声道立体声，需经过双通道的codec后再接到功放。

**2) 软件配置**

AEC开启功能时，除了打开对应宏定义外，还需要将外部播放器的状态同步给语音模块的CI110X系列芯片的GPIO口，同时要配置codec和设置IIS输入格式。请按下面的1-3流程进行配置。

**开启AEC功能：**

* 在sample\internal\sample\_XXX\src\user\_config.h。

开启AEC：

```
#define USE_SINGLE_MIC_AEC                1
```

关闭AEC：

```
#define USE_SINGLE_MIC_AEC                0
```

开启AEC的同时，需要在此user\_config.h文件中打开使用多个codec的宏：

```
#defineAUDIO_CAPTURE_USE_MULTI_CODEC        1（此宏在platform\sdk_default_config.h默认定义为0，此处设为1表明使用了外部CODEC）
```

同理，关闭AEC时，也需要关闭这个宏，将其置为0即可。

![内部AEC参考硬件线路图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-6.png)

**将播放器的播放状态同步给AEC模块：**

在sample\internal\sample\_XXX\src\system\_msg\_deal.c中check\_current\_playing为aec模块获取播放状态的函数，外部播放器需要通过GPIO（或者串口改变播放状态变量）的方式进行告知aec模块。

如下为GPIO方式告知aec模块的例程：（外部播放器播放音乐的时候将对应GPIO的pin拉高，停止播放的时候将GPIO的pin拉低）：

```
bool check_current_playing(void)
{
  //  if(AUDIO_PLAY_STATE_IDLE != get_audio_play_state()) //内部播放时，告知aec模块状态
if(gpio_get_input_level_single(GPIO2,gpio_pin_7)==1)//当为高时表示外部播放器正在播放
    {
        return true;
    }
    else
    {
        return false;
    }
}
```

---

## 3. **注意事项及AEC效果影响因素**

### 3.1 **参考信号注意事项**

* **避免参考模拟信号饱和**

![参考模拟信号饱和语谱图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-8.png)

图3-1 参考模拟信号饱和语谱图

![参考模拟信号饱和波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-9.png)

图3-2 参考模拟信号饱和波形图

\*\*调试建议：\*\*分压电路前信号失真，需对、音量进行调整或更换低阻抗的喇叭。

* **AEC的参考模拟信号为模拟小信号，容易受到干扰，需要特别注意layout的时候参考信号的干净程度。设计时参考模拟信号尽量包地和远离高频信号。如电路设计存在问题，通常会在采集信号或麦克风信号中引入不良信号，如脉冲、杂波、混叠等，会导致丢数据情况出现，影响效果。**

\*\*调试建议：\*\*出现该情况时应首先修改硬件电路设计，保证电路信号干净后，再进行声学测试，参考信号受到干扰对AEC效果影响非常明显。

* **参考信号从功放后端采集**
  这样的信号是最接近喇叭的真实声音的。当功放无延迟的情况可以用功放前端信号作为参考，功放有延迟（尤其有些自带eq的功放）的时候，建议从功放后端取参考信号。如果从PA后端引出信号，注意功放的输出范围不要超过ADC采样范围。

### 3.2 **mic信号注意事项**

* **避免采集的mic语音信号截幅**

\*\*调试建议：\*\*在扬声器在最大音量下播放满幅扫频信号，扬声器不能出现破音、共振等现象，mic采集信号及参考信号不能出现截幅。喇叭过大会导致失真更严重，在实际应用中需要选择合适的音量范围（喇叭不失真）。

### 3.3 **扬声器注意事项**

* **减少谐波失真**
  谐波失真是系统不是完全线性造成的。在整个音频通路中，造成信号谐波失真影响因素有：扬声器单体的失真曲线、扬声器后腔结构设计、喇叭前网罩、麦克风收音孔结构等；这些都会造成麦克信号的失真度增加。麦克信号失真越大，麦克信号和回采信号的相似度越差，回声消除的性能越差。喇叭的频响在200~4khz一致性太差，结构存在共振情况，都会影响AEC效果。

**调试建议：** 尽量不要用微型扬声器、超薄扬声器等共振频率较高的扬声器，该类单体低频失真大；扬声器后腔结构允许的话，加被动辐射器或导向管设计，可以增加低频，同时减小喇叭的非线性振动。

### 3.4 **环境注意事项**

* **减少机械噪声**
  在家用电器或机器人等产品设备中，风扇噪声和电机噪声引入的变量会导致麦克信号的底噪特别大，识别效果受较大影响。

\*\*调试建议：\*\*在该类设备中，要特别注意内部风扇、电机与麦克风的距离，如结构空间允许，可对麦克风单独做密封处理，防止内部传音。

* **麦克风与扬声器的位置关系**

\*\*调试建议：\*\*麦克风与喇叭的位置关系，原则上在结构空间允许的条件下，麦克风需尽可能的远离喇叭位置，麦克风不超出喇叭朝向所在水平面，麦克风离喇叭越近，信噪比越低，麦克风失真的可能性越大，考虑结构大小建议麦克风离喇叭10cm左右，麦克风与喇叭的位置关系左右上下对AEC处理效果影响不大。

![麦克风与喇叭的位置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-10.png)

图3-3 麦克风与喇叭的位置

\*注意扬声器播放口背离麦克风，避免朝向麦克风。

---

## 4. **调试效果的方法**

### 4.1 **效果调试**

* 为了更直观的观察AEC的效果，可以借助录音板进行调试，录音板的使用请参考语音AI平台上的文档 ☞[录音板使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/%E5%B7%A5%E5%85%B7/%E5%BD%95%E9%9F%B3%E6%9D%BF%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)。
* 在SDK \components\audio\_in\_manage\alg\_preprocess.c文件中，alg\_preprocess\_two\_ch()函数或alg\_preprocess\_four\_ch\_single\_aec()函数调用的

audio\_pre\_rslt\_write\_data( (int16\_t\*)dst, (int16\_t\*)micl )函数为IIS输出的数据内容，开启后，录音板可以从IIS接口录制到声音，该函数说明如下：

audio\_pre\_rslt\_write\_data函数中第一个参数为IIS输出的右声道的数据，第二个参数为IIS输出的左声道的数据，可以填写以下参数：

dst：输出算法处理结果的数据

Ref：输出原始获取到的参考声音

Micl：输出麦克风左声道获取到的原始声音

MicR：输出麦克风右声道获取到的原始声音

例如，当设置为：audio\_pre\_rslt\_write\_data( (int16\_t\*)dst, (int16\_t\*)micl )时，意思为右声道输出算法处理后的结果（AEC算法），左声道为原始的麦克风左声道的数据。

图4-1为录音板采音时域效果，该图上可以看到原始的左声道数据（上半图）为目标语音和喇叭播放声音的混合，经过aec算法处理后，抑制喇叭播放的声音，提高目标语音的信噪比，如图4-1中右声道数据（下半图）所示。

![录音板采音时域效果](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-11.png)

图4-1 录音板采音时域效果

* 图4-2为同样该音频的频域显示，该结果为正常结果。

![音频的频域显示](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-12.png)

图4-2 音频的频域显示

* 设置IIS输出参数为audio\_pre\_rslt\_write\_data((int16\_tXXX)ref, (int16\_tXXX)micl )时，输出参考信号数据和麦克风左声道采音数据，如图4-3，图中可以看到当有播放的时候参考信号有数据，无播放的时候，参考信号非常小接近为0。

![参考信号数据和麦克风左声道采音数据](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-13.png)

图4-3 参考信号数据和麦克风左声道采音数据

* 图4-4 为同样声音的频域显示：

![声音的频域显示](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-14.png)

图4-4 声音的频域显示

**注意:** 尽量保证参考信号和麦克风信号不削顶失真，不能过小，幅值上参考尽量满足平均在2000-20000采样值范围内。

### 4.2 **喇叭影响**

* 通常喇叭都会有一定的失真，音量接近喇叭播放极限的时候，会加剧失真度，尽量在使用的时候，使喇叭工作在失真范围小的幅度，不同喇叭会有不同固定自带的失真干扰。

选择喇叭也需要选择失真小的喇叭，参考图5-5，下图为4个喇叭播放的声音的情况，图中可以看到，有些喇叭的在不同的频段上失真比较大，如下图的后面三个，这些不同频段的语音失真，会严重影响AEC效果，尽量不用采用这种喇叭。图5-5中第一个喇叭的频响比较好。

![4个喇叭播放的声音的情况](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-15.png)

图5-5 4个喇叭播放的声音的情况

喇叭使用的时候，要避免喇叭声音过大导致的麦克风信号削顶。

![避免麦克风信号削顶](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-16.png)

图5-6 避免麦克风信号削顶

---

## 5. **应用注意事项**

### 5.1 **双声道立体声应用**

修改参数文件：CI110X\_SDK\components\alg\aec ci\_adapt\_aec.h

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-17.png)

**注意此方式更改在AEC的V10208以上版本有效，对应的纯离线SDK版本CI110X\_SDK\_ASR\_OfflineV1.5.5及以上、对应的算法SDK版本CI110X\_SDK\_ALG\_Application\_V1.6.7及以上有效!!!**

### 5.2 **效果调试接口**

参数接口：

```
void set_para_api(int alpha_l, int deta_d);
```

在 \工程名称（如\sample\internal\sample\_light）\components\audio\_in\_manage\alg\_preprocess.c中，找到alg\_aec\_init() 函数，修改以下参数

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-18.png)

**alpha\_l和deta\_d均表示帧数的大小，其增大可作为改善回声消除效果的方式，但会增加内存消耗 (alpha\_l或deta\_d每加1，内存将增加5k)， 需谨慎使用。**

**注意此接口在V10208以上版本有效，对应的纯离线SDK版本CI110X\_SDK\_ASR\_OfflineV1.5.5及以上、对应的算法SDK版本CI110X\_SDK\_ALG\_Application\_V1.6.7及以上有效!!!**

具体配置和测试效果如下表所示：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/%E5%9B%9E%E5%A3%B0%E6%B6%88%E9%99%A4%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-19.png)

**注意aecA、aecB库的使用仅在AEC的V10211以上版本有效，对应的算法SDK版本CI110X\_SDK\_ALG\_Application\_V1.7.1及以上有效！！！**
注：唤醒率的测试条件：
1）位置：扬声器放置在麦克风左侧，距离5cm;
2）播放的参考音频：《倍儿爽》，mic处的整体分贝为82-84dB；
3）唤醒词：“智能管家”，人声与mic距离3米，人声分贝60-66 dB，2男2女。
4）环境背景分贝：36-41dB。