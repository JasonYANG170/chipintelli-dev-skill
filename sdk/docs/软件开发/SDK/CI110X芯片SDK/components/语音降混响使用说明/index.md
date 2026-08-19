<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E8%AF%AD%E9%9F%B3%E9%99%8D%E6%B7%B7%E5%93%8D%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# 语音降混响使用说明

---

降混响（dereveb）是多通道降混响的简称，该算法在多麦克风情况下配合beamforming开启，可提高识别。因算力和配合beamforming的关系，仅仅处理150Hz到4800Hz的内容，所以在听觉上依旧会有混响存在。配合beamforming使用，Dereveb开启后，需要额外占用28.6KB的代码空间。
本文档主要介绍如何开启该功能。

---

## 1. **语音降混响的软件平台**

以下SDK可以支持语音降混响功能，具体为：

* 算法SDK：CI110X\_SDK\_ALG\_Application\_V1.51及以上版本，dereveb默认配合beamforming功能开启。
* 云端SDK：CI110X\_SDK\_Combine\_Cloud\_V1.7.1.1及以上版本，dereveb默认配合beamforming功能开启。

---

## 2. **语音降混响算法软件配置方法**

dereverb开启功能时，只需要打开对应宏定义即可 \工程名称（如\sample\internal\sample\_light）\src\user\_config.h文件，修改下述宏定义配置语音降混响算法功能，如下：

```
//打开语音降混响功能

#define USE_TWO_MIC_DEREVERB         1

//关闭语音降混响功能

#define USE_TWO_MIC_DEREVERB         0
```

语音降混响算法有以下常用函数可供用户使用：

* 初始化函数

```
void* ci_dereverb_create(void);
/**
    * @brief        初始化模块
    * @return int   不等于NULL: 初始化成功, 其他：初始化存在异常 
*/
```

上述函数中主要是创建降混响算法的句柄

```
int ci_dereverb_version( void );
/**
* @brief  获得当前版本号
* @return int, 版本号id,例如10000表示1.00.00;
*/
```

该函数获取当前版本号

```
int ci_dereverb_deal(void* handle, float** fft_in, float** fft_out);
/**
* @brief        降混响运算
* @param handle 模块句柄
* @param fft_in 多路频域输入
* @param fft_out 多路频域输出
* @return int   不等于0: 运算失败, 其他：正常运算
*/
```

该函数主要用于做降混响处理。

```
void ci_set_dereverb_freqbin(void* handle,float startHz,float endHz);
/**
* @brief        输出频率
* @param handle 模块句柄
* @param startHz 起始Hz
* @param endHz  截止Hz
* @return int   无
*/
```

默认设置为起始150-终止4800Hz，累计频率范围超过4650Hz，会被复位为150-4800Hz

```
ci_set_dereverb_config(void* handle,int mode,float gamma,float e_beta,float s_beta)；
/**
* @brief        降混响运算
* @param handle 模块句柄
* @param fft_in 多路频域输入
* @param fft_out 多路频域输出
*  @return int   不等于0: 运算失败, 其他：正常运算
*/
```

mode为0，运算小一点，参数不可调；

mode为1，gamma在0.9到1.0之间；

gamma越小，降混响越强；

e\_beta在0.4到.07之间，e\_beta越大，降混响越强;

s\_beta在0.2到0.35之间，s\_beta越小，降混响越强。

## 3. **语音降混响算法生效说明**

* 由于降混响算法在频谱上的差异不会特别大，为了更直观的观察算法是否生效，用户可以借助录音板进行调试，录音板的使用方法请参考语音AI平台上的文档 ☞[录音板使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/%E5%B7%A5%E5%85%B7/%E5%BD%95%E9%9F%B3%E6%9D%BF%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)。

确保alg\_preprocess\_two\_ch()函数运行且audio\_pre\_rslt\_write\_data( (int16\_t\*)dst, (int16\_t\*)micl )dst即右声道输出为算法处理后的结果。

播放一个1000Hz单频信号，如果单频信号可以被抑制则表示该算法生效。