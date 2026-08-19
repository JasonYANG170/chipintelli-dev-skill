<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82/ -->

# 增益调节(GAIN)

## 增益调节

### 1、概述

* 增益是输出信号幅度与输入信号幅度的比值，表示放大器的放大能力，常用dB这个单位表示。例如MIC输入的信号幅度较小，播放的原始音频信号幅度较小，调节CODEC的增益，就能将其放大。本文讲述CODEC的增益调节，以及AEC参考信号的增益调节。

### 2、dB单位介绍

* 单位dB可以表示很多含义，常用的有放大倍数的dB，语音信号幅值的dB（dBFS）。
* dB还有很多，比如dBSPL、dBm、dBu、dBV，dBSPL是声压的单位（我们使用的分贝仪显示的单位）；dBm表示功率的dB，功率的dB不是20倍log，而是10倍log。

#### 2.1、放大倍数的dB

* 多级放大倍数如果使用dB表示，最终的放大倍数可以每级放大倍数直接相加得到。放大倍数如果特别大的时候，可以使用比较小的dB值来表示。
* 放大倍数的dB的计算公式为：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-1.png)

* 常用放大倍数与dB的对应关系如下表，例如放大1000倍时，可以用60dB来表示：

| 放大倍数 | dB |
| --- | --- |
| 1 | 0 |
| 2 | 6 |
| 10 | 20 |
| 100 | 40 |
| 1000 | 60 |

#### 2.2、语音信号幅值的dB

* 语音信号的幅值可以通过数值来表示，也可以通过dB来表示，dB全称实际是dBFS（Decibels Full Scale），幅值的dB比放大倍数的dB计算要复杂一些，采样深度不一样的系统，公式是不一样的。

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-2.png)

* 例如在16bit的系统中，其中sample就是采样值，65536是2的16次方，在不同采样位数的系统中，sample除的分母就不一样了。幅值dB的计算公式为：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-3.png)

### 3、Audio CODEC ADC总体结构介绍

* CODEC输入信号为MIC的输出信号，MIC输出的模拟信号幅值比较小，需要经过CODEC的放大，再由ADC转换为数字信号，再对数字音频信号进行滤波等处理，最后通过数字接口将MIC接收到的音频数据传输给控制器。CODEC可以分为两个部分，模拟部分和数字部分。

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-4.png)

* 模拟部分包括前级放大、PGA、ADC，有的CODEC还带有MICBIAS等。数字部分包括ALC（自动幅值控制模块）、高通和低通滤波器、数字增益，以及和外部进行数字传输的数字接口。
* 模拟和数字并不是完全独立的。比如ALC根据ADC转换之后的数字信号，判断当前信号的幅值是否符设置的level，如果在当前level允许的情况下，则不改变PGA的增益，如果level不符合，则在允许的范围内，尽量调整PGA的增益，使信号的幅值处于level范围之内。

### 4、CODEC ADC的增益介绍

* 使用的CODEC有三级增益，分别是前级增益、PGA和数字增益。其中前级增益和PGA是模拟放大器放大，数字增益使用CODEC内部的24bit数字信号进行放大，也不是简单的对16bit的数字信号进行乘除。

#### 4.1、前级增益（MIC增益）

* 前级增益即MIC增益，其实也是一种程控增益，但是相对于PGA，前级增益的步进更粗，范围更窄，它的增益可以设置为：

```
//位于文件#include "ci13lc_codec.h"中
/**
 * @brief inner CODEC MIC增益配置
 * 
 */
typedef enum
{
    INNER_CODEC_MIC_AMP_0dB = 0,
    INNER_CODEC_MIC_AMP_6dB,
    INNER_CODEC_MIC_AMP_9dB,
    INNER_CODEC_MIC_AMP_12dB,
    INNER_CODEC_MIC_AMP_16dB,
    INNER_CODEC_MIC_AMP_20dB,
}inner_codec_mic_amplify_t;
```

* 左右mic可以设置不同的前级增益，例如修改板级配置文件board.c（文件路径CI13LC\_SDK\driver\boards\board.c）的参数：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-5.png)

#### 4.2、PGA增益

* PGA是可编程增益放大器，PGA的增益设置有两种工作模式，固定增益模式、自动增益调节模式。如果使用录音的ALC功能，则PGA必须工作在自动增益调节模式。
* PGA的增益范围为-18dB到28.5dB，步进为1.5dB。
* 左右mic可以设置不同的PGA增益，例如修改板级配置文件board.c的参数，设置PGA增益的固定增益：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-6.png)

* 或者调用音频管理的接口设置PGA增益的固定增益：

```
//位于文件#include "codec_manager.h"中

int cm_set_codec_adc_gain(int codec_index, cm_cha_sel_t cha, int gain);
```

* 或者调用codec驱动的接口设置PGA增益的固定增益：

```
//位于文件ci13lc_codec.c中

/**
 * @brief 调节CODEC PGA增益的固定增益
 *
 * @param gain 左声道PGA增益大小
 */
void inner_codec_set_pga_gain(inner_codec_cha_sel_t cha,int32_t ALC_Gain);
```

#### 4.3、数字增益

* 数字增益的可调范围非常宽，digt\_gain寄存器值设置为195的时候，增益为0dB，设置的寄存器值每增减1，数字增益就每增减0.5dB。数字增益的范围也就是-97dB到30dB。
* 左右mic可以设置不同的数字增益，例如修改板级配置文件board.c的参数：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-7.png)

* 或者调用codec驱动的接口设置数字增益：

```
//位于文件ci13lc_codec.c中

/**
 * @brief 调节CODEC ADC的数字增益（195为0dB，加减1为加减0.5dB增益）
 *
 * @param gain 左声道增益大小
 */
void inner_codec_adc_dig_gain_set(inner_codec_cha_sel_t cha,int32_t gain);
```

### 5、CODEC的ALC介绍

#### 5.1、ALC工作原理

* ALC的全称为auto level control，自动幅值控制模块。其最终目的就是将输出信号的幅值尽可能地调整到设定的level附近。
* ALC的工作原理大致如下图所示：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-8.png)

* （1）如果输入信号的幅值正好在设定的level附近，ALC将不会改变PGA的增益；
* （2）如果输入信号的幅值小于level，ALC会根据decaytime，一级一级地增加PGA的增益，知道输出信号的幅值到达level附近或者PGA增益达到设置的最大值。比如如果decaytime为2ms，ALC增加增益的时候，就会每隔2ms增加一级PGA的增益。
* （3）如果输入信号的幅值大于level，ALC根据attacktime，一级一级减小PGA的增益。

#### 5.2、ALC参数介绍

##### 5.2.1、 一些与时间相关的参数，holdtime、decaytime、attacktime

* （1）holdtime：保持增益不变的时间，即使ALC判断应该调整PGA的增益了，如果设置了holdtime为4ms，则PGA也会在4ms之后才会开始调整PGA的增益；
* （2）decaytime：每增加一级PGA增益的时间；
* （3）attacktime：每减小一级PGA增益的时间。
* ALC控制PGA增益减小的动作如下图所示，增益是按照设置的attacktime，一级一级调小的。相应的增益增加的动作，也是类似的：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-9.png)

* 可修改ci13lc\_codec.c代码中的参数，调节这3个时间参数：

```
//位于ci13lc_codec.c中

#define ALC_HOLD_TIME   INNER_CODEC_ALC_HOLD_TIME_2MS
#define ALC_DECAY_TIME  INNER_CODEC_ALC_DECAY_TIME_2MS
#define ALC_ATTACK_TIME INNER_CODEC_ALC_ATTACK_TIME_2MS
```

##### 5.2.2、与level相关的4个参数，low\_8bit\_maxlevel、low\_8bit\_minlevel、high\_8bit\_maxlevel、high\_8bit\_minlevel

* 可修改ci13lc\_codec.c代码中的参数，设置的ALC的level：

```
//位于ci13lc_codec.c中

#define ALC_LOW_8_BIT_MAX_LEVEL     ((uint8_t)(INNER_CODEC_ALC_LEVEL__6dB >> 0))
#define ALC_HOGH_8_BIT_MAX_LEVEL    ((uint8_t)(INNER_CODEC_ALC_LEVEL__6dB >> 8))
#define ALC_LOW_8_BIT_MIN_LEVEL     ((uint8_t)(INNER_CODEC_ALC_LEVEL__7_5dB >> 0))
#define ALC_HOGH_8_BIT_MIN_LEVEL    ((uint8_t)(INNER_CODEC_ALC_LEVEL__7_5dB >> 8))
```

##### 5.2.3、对ALC调整PGA进行限制的2个参数，maxgain、mingain

* 这两个参数限制了ALC调整PGA的能力，即ALC调整PGA时最大和最小能调整到的增益挡位。
* 可修改ci13lc\_codec.c代码中的参数，设置ALC调整PGA的能力：

```
//位于ci13lc_codec.c中

#define ALC_PGA_MAX_GAIN    INNER_CODEC_ALC_PGA_MAX_GAIN_28_5dB
#define ALC_PGA_MIN_GAIN    INNER_CODEC_ALC_PGA_MIN_GAIN__18dB
```

##### 5.2.4、ALC的开关

* 可修改ci13lc\_codec.c代码中的参数，设置是否开启ALC，如果ALC打开了，则PGA的增益由ALC自动调整，如果关闭了ALC功能，则PGA的增益为章节【4.2、PGA增益】设置的值。

```
//位于ci13lc_codec.c中

#define ALC_AGC_FUNC_SEL    ENABLE   //ENABLE，打开ALC；DISABLE，关闭ALC
```

* 或者调用函数，设置是否开启ALC

```
//位于文件#include "codec_manager.h"中

int cm_set_codec_alc(int codec_index, cm_cha_sel_t cha, FunctionalState alc_enable);
```

##### 5.2.5、ALC的过零检测功能

* 过零检测功能的主要作用是在调整增益的时候，在信号过零点的时候将增益更新进去，而不是随时都更新增益，这样有利于信号的质量。
* 可修改ci13lc\_codec.c代码中的参数，设置是否开启过零检测：

```
//位于ci13lc_codec.c中

#define ALC_ZERO_CROSS  INNER_CODEC_GATE_DISABLE  //默认关闭过零检测
```

* 下图是过零检测功能打开时，增益变大时的波形：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-10.png)

* 下图是过零检测功能关闭时，增益增大时的波形：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/Speech-API/img/%E5%A2%9E%E7%9B%8A%E8%B0%83%E8%8A%82-11.png)

##### 5.2.6、ALC的噪声门限功能

* 噪声门限的功能是将低于设定噪声门限的level，输入信号判定为噪声，然后不进行放大；如果信号的幅值高于这个level，就判定为声音信号，ALC正常处理。
* 可修改ci13lc\_codec.c代码中的参数，设置是否开启噪声门限功能：

```
//位于ci13lc_codec.c中

#define ALC_NOISE_GATE  INNER_CODEC_GATE_DISABLE //noisegate开关
#define ALC_NOISE_GATE_SET  INNER_CODEC_NOISE_GATE_THRE_45dB //噪声门限设置
```

### 6、CODEC DAC的增益介绍

* CODEC ADC的增益，一般分为数字增益和hpout的增益，数字增益通常配置为mute，应用只设置hpout的增益调节播放的音量。
* 可以调用函数设置hpout的增益：

```
//位于文件#include "codec_manager.h"中

int cm_set_codec_dac_gain(int codec_index, cm_cha_sel_t cha, int gain);
```