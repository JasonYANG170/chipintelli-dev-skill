<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/alc%E7%AE%97%E6%B3%95/ -->

# ALC自动电平控制

自动电平控制（Automatic Level Control，ALC）对输入或输出的某一部分的信号强度或电位数自动控制在一定值或者范围内，其最终目的就是将输出信号的幅值尽可能地调整到设定的level附近。

本文档主要介绍如何使用该功能。

## 1. ALC基本介绍

ALC通常与CODEC增益（前级增益、PGA和数字增益）搭配使用，ALC的工作原理大致如下图所示：

![自动增益控制原理框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/alc-2.png)

图1-1 自动电平控制原理框图

（1）如果输入信号的幅值正好在设定的level附近，ALC将不会改变可编程增益放大（Programmable Gain Amplifier，PGA）的增益；

（2）如果输入信号的幅值小于level，ALC会根据decaytime，一级一级地增加PGA的增益，直到输出信号的幅值到达level附近或达到PGA增益设置的最大值。如果decaytime为2ms，ALC增加增益的时候，就会每隔2ms增加一级PGA的增益。

（3）如果输入信号的幅值大于level，ALC根据attacktime，一级一级减小PGA的增益。

## 2.CODEC ADC的增益介绍

### 2.1 Audio CODEC ADC总体结构介绍

CODEC输入信号为MIC的输出信号，MIC输出的模拟信号幅值比较小，需要经过CODEC的放大，再由ADC转换为数字信号，再对数字音频信号进行滤波等处理，最后通过数字接口将MIC接收到的音频数据传输给控制器。CODEC可以分为两个部分，模拟部分和数字部分，其系统总体结构框图如图1-2所示。

![自Audio CODEC总体结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/alc-4.png)

图1-2 Audio CODEC总体结构图

* 模拟部分包括前级放大、PGA、ADC，有的CODEC还带有MICBIAS等。数字部分包括ALC（自动幅值控制模块）、高通和低通滤波器、数字增益，以及和外部进行数字传输的数字接口。
* 模拟和数字并不是完全独立的。比如ALC根据ADC转换之后的数字信号，判断当前信号的幅值是否符设置的level，如果在当前level允许的情况下，则不改变PGA的增益，如果level不符合，则在允许的范围内，尽量调整PGA的增益，使信号的幅值处于level范围之内。
* 使用的CODEC有三级增益，分别是前级增益、PGA和数字增益。其中前级增益和PGA是模拟放大器放大，数字增益使用CODEC内部的24bit数字信号进行放大，也不是简单的对16bit的数字信号进行乘除。

### 2.2 前级增益（MIC增益）

前级增益即MIC增益，其实也是一种程控增益，但是相对于PGA，前级增益的步进更粗，范围更窄，它的增益可以设置为：

```
//位于文件#include "ci130x_codec.h"中
/**
 * @brief inner CODEC MIC增益配置
 * 
 */
typedef enum
{
    INNER_CODEC_MIC_AMP_0dB = 0,//增益最小
    INNER_CODEC_MIC_AMP_6dB,
    INNER_CODEC_MIC_AMP_9dB,
    INNER_CODEC_MIC_AMP_12dB,
    INNER_CODEC_MIC_AMP_16dB,
    INNER_CODEC_MIC_AMP_20dB,//增益最大
}inner_codec_mic_amplify_t;
```

左右mic可以设置不同的前级增益，例如修改板级配置文件CI-D06GT01D.c（文件路径CI130X\_SDK\driver\boards\CI-D06GT01D.c）的参数：

![自Audio CODEC总体结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/alc-5.png)

图1-3 前级增益修改示意图

##### MIC增益调试建议：

* MIC增益部分定义了六种增益，`INNER_CODEC_MIC_AMP_20dB`表示最大增益，增益依次递减，`INNER_CODEC_MIC_AMP_0dB`为最小增益。
* 推荐使用默认参数，不要做太大改动。其一，该值的改动会影响采集的音频削顶，其二，会对语音降噪或语音识别效果产生负面影响。

### 2.3 PGA增益

PGA是可编程增益放大器，PGA的增益设置有两种工作模式，固定增益模式、自动增益调节模式。如果使用录音的ALC功能，则PGA必须工作在自动增益调节模式。

PGA的增益范围为-18dB到28.5dB，步进为1.5dB。

左右mic可以设置不同的PGA增益，例如修改板级配置文件CI-D06GT01D.c的参数，设置PGA增益的固定增益：

![自Audio CODEC总体结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/alc-6.png)

图1-4 pga增益修改示意图图

* 或者调用音频管理的接口设置PGA增益的固定增益：

  ```
  //位于文件#include "codec_manager.h"中

  int cm_set_codec_adc_gain(int codec_index, cm_cha_sel_t cha, int gain);
  ```
* 或者调用codec驱动的接口设置PGA增益的固定增益：

```
//位于文件ci130x_codec.c中
/**
 * @brief 调节CODEC PGA增益的固定增益
 *
 * @param gain 左声道PGA增益大小
 */
void inner_codec_set_pga_gain(inner_codec_cha_sel_t cha,int32_t ALC_Gain);
```

##### PGA增益调试建议：

* PGA的增益范围为-18dB到28.5dB，步进为1.5dB。左右mic可以设置不同的PGA增益。
* 推荐使用默认参数，不要做太大改动。

### 2.4 数字增益

数字增益的可调范围非常宽，digt\_gain寄存器值设置为195的时候，增益为0dB，设置的寄存器值每增减1，数字增益就每增减0.5dB。数字增益的范围也就是-97dB到30dB。

左右mic可以设置不同的数字增益，例如修改板级配置文件CI-D06GT01D.c的参数：

![自Audio CODEC总体结构图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/alc-8.png)

图1-5 数字增益修改示意图

* 或者调用codec驱动的接口设置数字增益：

```
//位于文件ci130x_codec.c中
/**
 * @brief 调节CODEC ADC的数字增益（195为0dB，加减1为加减0.5dB增益）
 * @param gain 左声道增益大小
*/
   void inner_codec_adc_dig_gain_set(inner_codec_cha_sel_t cha,int32_t gain);
```

##### 数字增益调试建议：

* 数字增益的范围也就是-97dB到30dB。
* 推荐使用默认参数，不要做太大改动。

## 3.ALC参数介绍

一些与时间相关的参数，holdtime、decaytime、attacktime

（1）holdtime：保持增益不变的时间，即使ALC判断应该调整PGA的增益了，如果设置了holdtime为4ms，则PGA也会在4ms之后才会开始调整PGA的增益；

（2）decaytime：每增加一级PGA增益的时间；

（3）attacktime：每减小一级PGA增益的时间。

ALC控制PGA增益减小的动作如下图所示，增益是按照设置的attacktime，一级一级调小的。相应的增益增加的动作，也是类似的：

![ALC控制PGA增益减小示意图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/alc-3.png)

图1-6 ALC控制PGA增益减小示意图

* 可修改ci130x\_codec.c代码中的参数，调节这3个时间参数：

```
//位于ci130x_codec.c中
#define ALC_HOLD_TIME   INNER_CODEC_ALC_HOLD_TIME_2MS
#define ALC_DECAY_TIME  INNER_CODEC_ALC_DECAY_TIME_2MS
#define ALC_ATTACK_TIME INNER_CODEC_ALC_ATTACK_TIME_2MS
```

##### 3.1 与level相关的2个参数，ALC\_MAX\_LEVEL、ALC\_MIN\_LEVEL

* 可修改ci130x\_codec.c代码中的参数，设置的ALC的level：

```
//位于ci130x_codec.c中

#define ALC_MAX_LEVEL     INNER_CODEC_ALC_LEVEL__5dB
#define ALC_MIN_LEVEL     INNER_CODEC_ALC_LEVEL__9dB
```

##### 3.2 对ALC调整PGA进行限制的2个参数，maxgain、mingain

* 这两个参数限制了ALC调整PGA的能力，即ALC调整PGA时最大和最小能调整到的增益挡位。
* 可修改ci130x\_codec.c代码中的参数，设置ALC调整PGA的能力：

```
//位于ci130x_codec.c中

#define ALC_PGA_MAX_GAIN    INNER_CODEC_ALC_PGA_MAX_GAIN_28_5dB
#define ALC_PGA_MIN_GAIN    INNER_CODEC_ALC_PGA_MIN_GAIN__18dB
```

##### 3.4 ALC的开关

* 可修改ci130x\_codec.c代码中的参数，设置是否开启ALC，如果ALC打开了，则PGA的增益由ALC自动调整，如果关闭了ALC功能，则PGA的增益为章节【PGA增益】设置的值。

```
//位于ci130x_codec.c中

#define ALC_AGC_FUNC_SEL    ENABLE   //ENABLE，打开ALC；DISABLE，关闭ALC
```

* 或者调用函数，设置是否开启ALC

```
//位于文件#include "codec_manager.h"中

int cm_set_codec_alc(int codec_index, cm_cha_sel_t cha, FunctionalState alc_enable);
```

##### 3.5 ALC的过零检测功能

* 过零检测功能的主要作用是在调整增益的时候，在信号过零点的时候将增益更新进去，而不是随时都更新增益，这样有利于信号的质量。
* 可修改ci130x\_codec.c代码中的参数，设置是否开启过零检测：

```
//位于ci130x_codec.c中

#define ALC_ZERO_CROSS  INNER_CODEC_GATE_DISABLE  //默认关闭过零检测
```

* 下图是过零检测功能打开时，增益变大时的波形：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-10.png)

* 下图是过零检测功能关闭时，增益增大时的波形：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-11.png)

##### 3.6 ALC的噪声门限功能

* 噪声门限的功能是将低于设定噪声门限的level，输入信号判定为噪声，然后不进行放大；如果信号的幅值高于这个level，就判定为声音信号，ALC正常处理。
* 可修改ci130x\_codec.c代码中的参数，设置是否开启噪声门限功能：

```
//位于ci130x_codec.c中

#define ALC_NOISE_GATE  INNER_CODEC_GATE_DISABLE //noisegate开关
#define ALC_NOISE_GATE_SET  INNER_CODEC_NOISE_GATE_THRE_45dB //噪声门限设置
```

##### 3.7 配置ALC的自动切换功能

AEC回声消除功能开启后，`alc_off_codec_adc_gain = 20`默认关闭ALC，编解码器ADC的增益值为一个固定的增益补偿值20。

## 4.ALC软件平台

* 离线通话降噪SDK：CI130X\_SDK\_NN\_ENC