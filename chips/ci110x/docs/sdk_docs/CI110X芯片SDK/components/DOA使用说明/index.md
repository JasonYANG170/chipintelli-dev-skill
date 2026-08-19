<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/DOA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# **DOA使用说明**

波达方向（Direct of Arrival,DOA）估计算法一般用于声源方位角度估计，以获取目标声源位置或者为语音增强算法提供先验信息。本文档主要介绍其适用场景及如何开启该功能。

## 1. **SDK中DOA算法的默认配置和对应硬件板**

CI110X系列芯片纯离线SDK不支持DOA算法，算法SDK和离在线SDK中DOA估计算法默认为关闭状态。使用DOA算法必须采用双麦克风硬件方案，可用启英泰伦提供的CIB03ST01J-WL21等离在线模块板或用户自行开发的双麦克风模块板，具体硬件选型可咨询启英泰伦技术支持人员。

## 2. **应用场景**

DOA角度设置与麦克风阵列摆放位置示意图如下图所示。图a,b中麦克风阵列为线性双麦阵列，推荐麦间距为4cm。设置角度从左麦克到右麦克为0度到180度变化，利用DOA算法可以获取目标人声的方位角度。

![流程图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/DOA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-1.jpg)

图 (a) 麦克风阵列横向结构与角度设置示意图

![流程图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/img/DOA%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-2.jpg)

图(b) 麦克风阵列竖向结构与角度设置示意图

## 3. **SDK开发示例的选择**

**离在线SDK:**

* CI110X\_SDK\_Combine\_Cloud/sample/internal/sample\_media

sample\_media是针对离在线方案场景的应用。该示例中算法功能配置方法与下文中算法SDK的sample中所提到的配置方法一致，因此不单独为其进行介绍。
在算法SDK中，DOA的使用一般用于sample\_common工程，默认为关闭状态。当需要获取目标人声方位角度时，可选择打开DOA算法功能，具体实现见下文第4.1节。

## 4. **sample\_common工程使用说明**

### 4.1. DOA估计算法的使用

**步骤一：开启DOA估计算法**

在文件sample\internal\sample\_common\src\user\_config.h中开启相关的宏

应用一：开启AEC功能的应用，需要使用外部codec

```
#define AUDIO_CAPTURE_USE_MULTI_CODEC       1
#define USE_TWO_MIC_DOA                     0 // 1为开启，0为关闭，默认配置为关闭
```

应用二：关闭AEC功能的应用，无需外部codec

```
#define AUDIO_CAPTURE_USE_MULTI_CODEC       0
#define USE_TWO_MIC_DOA                     0 // 1为开启，0为关闭，默认配置为关闭
```

**步骤二：开启DOA的功能模式**

在SDK\components\audio\_in\_manage\alg\_doa\_apply.h中设置DOA的功能模式

DOA存在3种工作模式：

模式（1）DOA\_MODEL\_VAD\_ALWAYS ：根据vad开始状态获取doa角度信息

模式（2）DOA\_MODEL\_WAKE\_UP ：根据唤醒词结束状态获取doa角度信息

模式（3）DOA\_MODEL\_CMD ：根据命令词结束状态获取doa角度信息

```
#define DOA_MODEL        DOA_MODEL_WAKE_UP //默认唤醒词模式
```

**步骤三：角度获取、清除**

```
/**
* @brief  获取当前角度信息（只有开启了DOA算法模块才有意义）,
*         在被clear_current_doa_angle（）清除角度之前，一直保持角度信息；
*         默认情况下，开启DOA算法模块时，当说唤醒词后更新角度信息，
*         开启DOA_MODEL_CMD模式，当说本地命令词后也会更新角度信息，
*         开启DOA_MODEL_VAD_ALWAYS模式，根据唤醒词更新角度信息；
* @return int, 角度信息，正常的数值范围为 0~180， 若返回其它值则表示错误;
*/ 
int get_current_doa_angle( void );
```

该角度信息显示为说话者的角度信息，在产品调试阶段，用户需要实际测试该API反馈的角度信息与实际产品中使用空间方位是否一致。

默认角度方位从左mic到右mic方向，doa的角度由0°到180°方向转变。

Mic方向确认测试方法：使用DOA估计算法功能，根据实际获取的角度信息对mic左右方向进行确定或调整。如用户站在产品左侧说唤醒词，得到的结果为45度，该方位信息实际表示方位为产品左侧， 那么135度就表示是产品的右侧。

```
/**
 * @brief  清除当前角度信息（只有开启了DOA算法模块才有意义）
 * 
 * @return int, 若返回0，表示成功; 若返回其它值则表示错误;
 */ 
int clear_current_doa_angle( void );
```

**步骤四：调整mic间距**

在sdk\compoents\audio\_in\_manage\alg\_preprocess.c文件中算法初始化过程中设置mic间距：

```
#define MIC_DISTANCE    MIC_DISTANCE_40MM   //系统默认间距40mm
doa = ci_doa_create(MIC_DISTANCE)
```

注意

可选间距

30mm: MIC\_DISTANCE\_30MM

40mm: MIC \_DISTANCE\_40MM

50mm: MIC\_DISTANCE\_50MM

80mm: MIC \_DISTANCE\_80MM

120mm: MIC \_DISTANCE\_120MM

## 5.**效果确认**

### 5.1 角度输出分辨率选择

在sdk\compoents\audio\_in\_manage\alg\_doa\_apply.c文件中设置输出角度的分辨率

```
#define DOA_RESOLUTION           (3)   // DOA_RESOLUTION = 3、5、10
```

DOA角度输出分辨率可选择输出角度为3个角度、5个角度和10个角度，默认设置为输出3个角度。
3个角度的输出为45°、90°和135°；
5个角度的输出为0°、45°、90°、135°和180°；
10个角度的输出为0到180°，每间隔20°输出一个角度。

### 5.2 查看log

通过串口打印，当识别到唤醒词后，可以看到打印的角度信息，例如打印显示 **angle = 90**，表示声源在90°方向左右。

不同方位说唤醒词时，打印不同的角度信息，角度信息与实际目标语音所在方向吻合（如站在距离阵列1米位置，分别在mic两侧说唤醒词，一侧显示的角度是45度左右，另一侧显示的是135度左右，阵列正前方说唤醒词为90度左右），说明DOA估计算法模块工作正常。