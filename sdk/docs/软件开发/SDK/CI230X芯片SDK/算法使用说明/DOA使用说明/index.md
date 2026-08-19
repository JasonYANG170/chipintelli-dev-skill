<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/DOA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# **DOA使用说明**

波达方向（Direction of Arrival,DOA）估计算法一般用于声源方位角度估计，以获取目标声源位置。本文档主要介绍其适用场景及如何开启该功能。

## **1.SDK中DOA算法的默认配置和对应硬件板**

使用DOA算法必须采用双麦克风硬件方案，默认芯片配置为CI1306。

## **2.应用场景**

DOA角度设置与麦克风阵列摆放位置示意图如下图所示。图a,b中麦克风阵列为线性双麦阵列，推荐麦间距为4cm。设置角度从左麦克到右麦克为0度到180度变化，利用DOA算法可以获取目标人声的方位角度。

![流程图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/DOA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-1.jpg)

​ 图 (a) 麦克风阵列横向结构与角度设置示意图

![流程图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/DOA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-2.jpg)

​ 图(b) 麦克风阵列竖向结构与角度设置示意图

## **3. cias\_aiot\_audio工程使用说明**

### **3.1 DOA算法使用说明**

**步骤一：开启DOA算法**

在文件projects\cias\_aiot\_audio\cias\_aiot\_cfg\user\_config.h中开启相关的宏

```
//打开双麦DOA功能
#define USE_DOA_MODULE 1
//关闭双麦DOA功能
#define USE_DOA_MODULE 0
```

**说明：目前只有TwoMic\_SDK支持DOA算法**

**步骤二：参数配置**

在projects\cias\_aiot\_audio\cias\_system\ci\_ssp\_config.c文件中配置DOA算法参数：

```
//doa模块配置
const doa_config_t doa_config =
{
    .distance = 40 ,
    .min_frebin = 40,
    .max_frebin = 130,
    .samplerate = 16000,
    .doa_resolut = 5                // doa_resolut = 3、5、10
};
```

参数说明：

distance：麦克风阵列麦间距，int类型，默认40mm

min\_frebin：进行DOA计算的最低频点，范围为[0 255]，默认40

max\_frebin：进行DOA计算的最高频点，范围为[0 255]，默认130

samplerate：输入音频采样率，默认16000

doa\_resolut：DOA输出分辨率，只能选择3、5、10。分别表示输出为3个角度、5个角度和10个角度。

3个角度的输出为45°、90°和135°；

5个角度的输出为0°、45°、90°、135°和180°；

10个角度的输出为0到180°，每间隔20°输出一个角度。

**备注：DOA默认使用唤醒词模式，即识别到唤醒词后会输出对应角度信息。**

## **4.效果确认**

### **4.1 查看log**

通过串口打印，当识别到唤醒词后，可以看到打印的角度信息，例如打印显示 **angle = 90**，表示声源在90°方向左右。

不同方位说唤醒词时，打印不同的角度信息，角度信息与实际目标语音所在方向吻合（如站在距离阵列1米位置，分别在mic两侧说唤醒词，一侧显示的角度是45度左右，另一侧显示的是135度左右，阵列正前方说唤醒词为90度左右），说明DOA估计算法模块工作正常。