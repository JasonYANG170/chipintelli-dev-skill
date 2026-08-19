<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E4%B8%BB%E6%8E%A7%E6%96%B9%E6%A1%88-CI13LC%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E5%90%B8%E9%A1%B6%E7%81%AF%E4%B8%BB%E6%8E%A7%E6%96%B9%E6%A1%88-CI13242/ -->

# CI13LC离线语音吸顶灯主控方案

## 一. 方案介绍

### 1.1 方案背景

离线语音在各类产品上的使用越来越多，这为原有的非智能产品增加了能听会说的“耳朵”和“嘴巴”，提升了用户体验。我司的智能语音芯片，既可以用串口和电控板对接的方式快速为产品增加语音功能，也可以作为部分产品的主控芯片，以兼具更小的空间及更低的成本。

目前吸顶灯产品的控制方式有以下几类：

* 墙壁开关控制：传统墙壁开关控制，使用不够方便；
* 遥控器控制：一般采用红外、433、2.4G遥控，但遥控器有时不好找；
* 手机APP控制：每次控制需要打开手机APP，需要联网，操作较为繁琐；
* 智能音箱控制：通过智能音箱直接控制，相对方便，但音箱需联网状态下才可使用，可能存在网络延时，有隐私安全风险；

为解决上述控制的痛点，我司开发了离线语音吸顶灯主控方案，实现智能语音控制，无需联网，想要控制灯具，只需动动嘴就可以了，使用方便快捷。

### 1.2 方案框图

将语音模块内置，和灯驱部分连在一起。通过语音模块输出PWM信号控制灯光输出；语音模块检测AC信号，实现墙壁开关变光功能。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE1%E6%96%B9%E6%A1%88%E6%A1%86%E5%9B%BE.png)

### 1.3 方案描述

**主要功能：**

①支持开关控制；

②支持亮度调节；

③支持色温调节；

④支持快速开关墙壁开关进行变光；

⑤支持场景灯光名称自定义；

**演示视频：** 离线语音吸顶灯主控方案演示视频[点我观看](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88/%E6%96%B9%E6%A1%8809%EF%BC%9A%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E5%90%B8%E9%A1%B6%E7%81%AF%E6%96%B9%E6%A1%88%E6%BC%94%E7%A4%BA/)。

### 1.4 方案特点

* 控制便捷：无需联网、使用控制更方便；
* 主控芯片：使用语音芯片做主控，成本更有优势；
* 调光算法：自研CCT调光算法，调光细腻平滑，无可视频闪；
* 稳定可靠：行业众多大厂量产案例，高稳定性，高可靠性；

### 1.5 可选功能

* 免唤醒：“小X开灯、小X关灯”等固定词条，可做免唤醒直接控制；
* 自然说：可支持离线自然说，用户无需记忆命令词，使用更便捷；
* 自学习：可支持自学习，解决方言口音问题；
* 小程序及遥控：使用CI23LC系列芯片可支持小程序及遥控器控制；

## 二. 硬件设计

### 2.1 参考原理图

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE4%E5%8E%9F%E7%90%86%E5%9B%BE.png)

### 2.2 推荐引脚

* 白光控制脚：PA3;
* 黄光控制脚：PA5;
* AC掉电检测脚：PA6;

### 2.3 硬件设计注意事项

* CI13242语音芯片默认PG\_EN(PA4)升级脚内部上拉，上电会先自动进入升级模式，启动时间约为800ms，故使用我司语音芯片作为灯具主控应用时，需要将PG\_EN(PA4)升级脚加外边下拉电阻，使其上电时跳过升级模式，这样启动时间可做到300ms左右；
* 为了调试升级方便，硬件设计时建议将PG\_EN(PA4)升级脚预留测试点；

## 三.结构设计

### 3.1 麦克风结构设计

* 麦克风类似是人的耳朵，本质是将一个微小声压信号进行放大，然后进入语音芯片进行识别，所以麦克风在安装时候，要特别产品开孔和注意远离噪音避免干扰，详细参考请☞【[麦克风结构设计](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/)】；
* 采用双麦设计，双麦克风推荐距离为4CM(两麦克风的中心距离）；
* 麦克规格（推荐）：灵敏度-32dB，信噪比65dB以上；

### 3.2 结构设计注意事项

* 对于吸顶灯产品，能开麦克风孔的强烈建议尽量都开麦克风孔，开孔后识别效果会更好，对噪音的抵抗能力会更强；
* 对于吸顶灯产品，因结构限制，实在无法开麦克风孔的，只能选择麦克风内置与灯罩内的，注意要将麦克风放置与LED灯板上方，一定不要放置于LED灯板下方，麦克风和LED灯板之间不要有缝隙；
* 对于吸顶灯产品，注意不能有喇叭，否则喇叭播放完仍会在腔体产生混响，导致识别效果下降明显；

## **四. 软件开发**

### **4.1 硬件准备**

**开发套件**：启英泰伦 CI13242开发板套件 (推荐型号 [CI13242开发板套件](http://mall.chipintelli.com/chip2?product_id=123&brd=1))

**烧录工具**：[USB转TTL串口调试工具](http://mall.chipintelli.com/chip2?product_id=60&brd=1)（用于固件烧录、实时通信验证、日志LOG打印等，可5V/3V3供电）

**杜邦线**：[杜邦线](http://mall.chipintelli.com/chip2?product_id=122)（用于固件烧录、实时通信验证、日志LOG打印等）

**麦克风、喇叭**：启英商城购买[麦克风、喇叭](http://mall.chipintelli.com/chip?product_category=18&brd=1)与模块板匹配，批量购买具体参数可参考：[麦克风兼容列表](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%A4%96%E5%9B%B4%E5%99%A8%E4%BB%B6%E5%85%BC%E5%AE%B9%E5%88%97%E8%A1%A8/)

**测试设备**：个人电脑（建议Windows 7及以上系统）

### **4.2 软件准备**

**开发环境搭建**：[编译软件安装与使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)

**软件SDK下载**：[CI13LC\_SDK\_ASR\_Offline\_V2.0.15](https://aiplatform.chipintelli.com/attachment)(*若有新版本，请使用最新版本的SDK*)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE2SDK%E4%B8%8B%E8%BD%BD.png)

### **4.3 资料获取**

1. [启英泰伦语音AI平台](https://aiplatform.chipintelli.com/)
2. [CI13242 芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13241%26CI13242%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/)
3. [CI-F24XGS01S模块数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13LC%E7%B3%BB%E5%88%97/CI-F24XGS01S%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/)
4. [13242开发板套件说明](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI13241%26CI13242%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/)
5. [SDK 软件开发手册](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/)

### 4.4 文件说明

我司为用户基于吸顶灯主控方案提供了完整的SDK包，用户可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/) 或联系我司FAE获取相关SDK开发包。为了方便代码的移植，SDK包中关于灯光控制驱动放置于CI13LC\_SDK\_Offline\_Vx.x.x\projects\offline\_asr\_sample\src\light文件夹内，文件功能说明如下：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE5%E7%81%AF%E6%8E%A7%E6%96%87%E4%BB%B6.png)

| 文件名 | 功能描述 | 使用说明 |
| --- | --- | --- |
| buzzer\_driver.c buzzer\_driver.h | 蜂鸣器驱动，实现唤醒、退出唤醒等提示音功能 | 可通过宏配置蜂鸣器是否开启及蜂鸣器IO引脚配置等 |
| light\_ac\_detect.c light\_ac\_detect.h | AC掉电检测，实现墙壁开关变光，多次开关复位等功能 | 可通过宏配置AC检测是否开启及AC检测IO引脚配置等 |
| light\_drive.c light\_drive.h | 灯光PWM驱动，实现冷暖两路PWM驱动初始化 | 可通过宏配置冷暖PWM引脚，频率，最大占空比等 |
| light\_control.c light\_control.h | 灯光控制源文件，初始化、开关、亮度、色温、模式等调节功能 | 可通过宏配置调光算法，线性调光还是曲线调光，亮度等级极值等 |

### 4.5 **函数说明**

1、灯光控制初始化函数路径：CI13LC\_SDK\_Offline\_Vx.x.x\projects\offline\_asr\_sample\src\light\light\_control.c

```
/**
 * @brief 灯光控制初始化
 *
 * @param [in] void
 * @param [out] None
 *
 * @return void
 *
 * @history
 * 1.Date         : 3/24/2025 16:48:32
 *   Author       : zhh
 *   Modification : Created function
 */
void light_control_init(void)
{
    // 灯驱动初始化
    light_driver_init();

    // 调光定时器初始化
    light_timer_init();

    // 灯状态初始化
    light_status_init();

    //蜂鸣器初始化
    buzzer_driver_init();

    // AC检测初始化
    light_ac_detect_init();

    // 灯光上电输出
    light_power_on_output();

    // 创建灯具配置失效定时器
    config_invalid_timer = xTimerCreate("config_invalid_timer",       pdMS_TO_TICKS(CONFIG_INVALID_TIME), pdFALSE, (void *)0, config_invalid_callback);
    if (NULL == config_invalid_timer)
    {
        mprintf("config_invalid_timer create fail! FUNCTION = %s, LINE =   %d\r\n",__FUNCTION__, __LINE__);
    }
    else
    {
        xTimerStart(config_invalid_timer, 0);
    }
}
```

2、灯光输出更新函数路径：CI13LC\_SDK\_Offline\_Vx.x.x\projects\offline\_asr\_sample\src\light\light\_control.c

```
/**
 * @brief 灯光输出更新
 *
 * @param [in] uint16_t toCct    目标色温
 * @param [in] uint16_t toLevel  目标亮度
 * @param [in] uint16_t fadeTime 从当前色温、亮度到目标色温、亮度的渐变时间，单位ms
 * @param [in] uint16_t keepTime 调光完成后保持时间,单位ms
 * @param [in] void(*finishCB)(void) 调光完成后回调函数
 * @param [out] None
 *
 * @return void
 *
 * @history
 * 1.Date         : 3/24/2025 14:33:52
 *   Author       : zhh
 *   Modification : Created function
 */
void light_output_update(uint16_t toCct, uint16_t toLevel, uint16_t fadeTime, uint16_t keepTime, void(*finishCB)(void))
{
    uint16_t destPwm[LIGHT_MAX_PWM_CHANNEL] = {0};
    uint16_t *pDestPwm = destPwm;
    bool turnOff = false;

    uint16_t fadeCycle = fadeTime * (BASE_FADE_TIME / LIGHT_CTR_TIMER_PERIOD);
    uint16_t keppCycle = keepTime * (BASE_FADE_TIME / LIGHT_CTR_TIMER_PERIOD);

    if (toLevel > DEFAULT_MAX_LEVEL)
    {
        toLevel = DEFAULT_MAX_LEVEL;
    }

    if ((toLevel < DEFAULT_MIN_LEVEL) && (toLevel != 0))
    {
        toLevel = DEFAULT_MIN_LEVEL;
    }

    if (toCct > DEFAULT_COOL_CCT)
    {
        toCct = DEFAULT_COOL_CCT;
    }
    else if (toCct < DEFAULT_WARM_CCT)
    {
        toCct = DEFAULT_WARM_CCT;
    }

    light_pwm_percent_calc(toCct, toLevel, destPwm);

    if (toLevel == 0)
    {
        turnOff = true;
    }
    g_light_status.cct = toCct;

#if LIGHT_OFF_SAVE_EN
    g_light_status.level = toLevel;
#else
    if(toLevel != 0)
    {
        g_light_status.level = toLevel;
    }
#endif

    light_regulate_update(pDestPwm, fadeCycle, keppCycle, turnOff, finishCB);
}
```

3、灯光控制处理函数路径：CI13LC\_SDK\_Offline\_Vx.x.x\projects\offline\_asr\_sample\src\light\light\_control.c

```
/**
 * @brief 通过语义ID进行灯光控制处理
 *
 * @param [in] uint32_t semantic_id 
 * @param [out] None
 *
 * @return int8_t
 *
 * @history
 * 1.Date          : 7/27/2021 15:56:56
 *   Author       : zhh
 *   Modification : Created function
 */
int8_t light_control_deal(uint32_t semantic_id)
{
    int8_t ret = 1;

    if (semantic_id == 1000) //开灯
    {
        ret = light_turn_on_output();
    }
    else if (semantic_id == 1001) //关灯
    {
        ret = light_turn_off_output();
    }
    else if (semantic_id == 1002) //白光
    {
        ret = light_white_output();
    }
    else if (semantic_id == 1003) //黄光
    {
        ret = light_yellow_output();
    }
    else if (semantic_id == 1004) //自然光
    {
        ret = light_nature_output();
    }
    else if (semantic_id == 1005) //白一点
    {
        ret = light_cct_up_output();
    }
    else if (semantic_id == 1006) //黄一点
    {
        ret = light_cct_down_output();
    }
    else if (semantic_id == 1007) //切换色温
    {
        ret = light_asr_change_output();
    }   
    else if (semantic_id == 1008) //亮一点
    {
        ret = light_brightness_up_output();
    }
    else if (semantic_id == 1009) //暗一点
    {
        ret = light_brightness_down_output();
    }
    else if (semantic_id == 1010) //最高亮度
    {
        ret = light_brightness_max_output();
    }
    else if (semantic_id == 1011) //中等亮度
    {
        ret = light_brightness_mid_output();
    }
    else if (semantic_id == 1012) //最低亮度
    {
        ret = light_brightness_min_output();
    }
    return ret;
}
```

### 4.6 命令词和固件制作指南

语音识别的流程及所需资源如下图所示，麦克风将语音转换为数字信号，送到NN做识别。NN识别需要两个资源，声学模型和语言模型，NN识别后输出字符串。然后到命令词信息表里查找NN输出的字符串，如果未找到，说明误识别，不处理。如果查找到，就是有效识别，然后根据查找到的命令词获取相关信息，进行相应应用功能处理，最后调用提示音播放器播放提示音。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE6%E6%A8%A1%E5%9E%8B%E5%88%B6%E4%BD%9C%E8%B5%84%E6%BA%90.png)

模型表格生成、固件开发、固件打包及烧录等内容请☞[命令词和固件制作指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/%E5%91%BD%E4%BB%A4%E8%AF%8D%E5%92%8C%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%E6%8C%87%E5%8D%97/)进行参考学习。

用户如遇到使用问题，请到启英泰伦语音AI平台上☞[提交工单](https://aiplatform.chipintelli.com/tickets)或直接联系我司FAE获取协助。