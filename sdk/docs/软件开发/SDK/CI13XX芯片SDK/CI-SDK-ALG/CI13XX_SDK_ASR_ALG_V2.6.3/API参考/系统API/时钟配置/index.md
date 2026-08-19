<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/%E6%97%B6%E9%92%9F%E9%85%8D%E7%BD%AE/ -->

# 时钟配置

---

## 1、简介

本文围绕芯片的主频配置、内部晶振和外部晶振的选择，每个外围设备接口的分频如何修改，IIS时钟配置展开说明。

---

## 2、主频配置

* CI13XX系列芯片的主频最大支持240M，如需修改主频，先找到文件（CI13XX\_SDK\driver\ci130x\_chip\_driver\inc\ci130x\_scu.h）

```
#define MAIN_FREQUENCY          240000000          //主频宏定义
```

* 或者直接调用接口配置主频

```
#include "ci130x_dpmu.h"
uint32_t src_frequency = 12288000                 //通常为12.288M
uint32_t main_frequency = 240000000               //主频最大支持240M

dpmu_pll_config(src_frequency, main_frequency);   //根据晶振时钟配置需要的主频
```

注意

* 使用外部晶振时，主频最大可以用240M。使用内部晶振时，建议主频最大使用220M。

## 3、晶振选择

* 如果模组外挂了晶振，可以配置选择外部晶振，否则选择内部晶振，作为系统工作频率时钟源。如需修改，找到文件（CI13XX\_SDK\projects\offline\_asr\_alg\_pro\_sample\src\user\_config.h）

```
#define USE_EXTERNAL_CRYSTAL_OSC    1  //0:使用内部RC作为时钟源。1:使用外部晶振作为时钟源。
```

* 或者直接调用接口配置晶振选择

```
#include "ci130x_dpmu.h"

dpmu_set_src_source(DPMU_SRC_USE_OUTSIDE_OSC);   //使用外部晶振作为时钟源。

dpmu_set_src_source(DPMU_SRC_USE_INNER_RC);      //使用内部RC作为时钟源
```

## 4、外设系统时钟频率配置

* 先明确几个概念：
* AHB：Advanced High performance Bus，高级高性能总线，这是一种系统总线。主要用于高性能模块如CPU、DMA、UART等之间的连接
* APB：是Advanced Peripheral Bus的缩写，这是一种外围总线。主要用于低频率的周围外设之间的连接，例如IIC、GPIO、PWM等
* IPCORE\_CLK：系统主频
* 下图为时钟分频图，简易说明系统时钟和各外设时钟分频关系。
* 晶振时钟通常为12.288 MHz
* 晶振时钟经过倍频后得到PLL时钟。
* PLL时钟经过一级分频可得到IPCORE\_CLK主频，主频最大支持240M。
* PLL时钟经过两级分频可得到PCLK。

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/img/%E6%97%B6%E9%92%9F%E9%85%8D%E7%BD%AE-1.png)

图1 时钟分频图

* 有些外设基于IPCORE\_CLK分频，有些外设基于PCLK分频，有些外设也能基于晶振时钟分频，下面会以表格的形式详细说明。

| 外围设备接口 | 描述 | 时钟来源 | 分频参数宽度 | 芯片默认分频值 |
| --- | --- | --- | --- | --- |
| IPCORE\_CLK | 提供给高速总线AHB的时钟，也是主频 | 来源于PLL时钟 | 6bit宽度（值可选 0 ~ 63） | 0（1分频） |
| PCLK | 提供给低速总线APB的时钟 | 来源于IPCORE\_CLK | 4bit宽度（值可选 0 ~ 15） | 2 |
| UART | UART时钟决定波特率 | 来源于IPCORE\_CLK | 6bit宽度（值可选 0 ~ 63） | 8 |
| PWM | PWM时钟决定输出频率 | 来源于PCLK（分频不可配，默认为1分频），或晶振时钟（分频可配） | 7bit宽度（值可选 0 ~ 127） | 16 |
| TIMER | TIMER时钟决定计数值 | 来源于PCLK（分频不可配，默认为1分频），或晶振时钟（分频可配） | 7bit宽度（值可选 0 ~ 127） | 16 |
| IWDG | IWDG时钟决定计数值 | 来源于晶振时钟 | 7bit宽度（值可选 0 ~ 127） | 16 |
| DTRFLASH | DTRFLASH时钟决定CLK频率 | 来源于IPCORE\_CLK | 3bit宽度（值可选 0 ~ 7） | 2 |
| ADC | ADC时钟决定采样频率 | 来源于IPCORE\_CLK | 6bit宽度（值可选 0 ~ 63） | 12 |
| WWDG | 窗口看门狗时钟决定计数值 | 来源于PCLK（分频不可配，默认为1分频） | - | - |
| IIC | IIC时钟决定SCL的频率 | 来源于PCLK（分频不可配，默认为1分频） | - | - |
| GPIO | GPIO时钟决定IO翻转的频率 | 来源于PCLK（分频不可配，默认为1分频） | - | - |

* 可调用接口得到IPCORE\_CLK等时钟的值，再根据分频值去推算每个外围设备接口的默认时钟

```
#include "platform_config.h"

void device_clk_test()
{
    //获取IPCORE_CLK主频有2种方式
    #if 0
    uint32_t ipcore_clk = dpmu_get_pll_frequency() / 1;      //默认为1分频，从PLL计算得到IPCORE_CLK主频
    #else
    uint32_t ipcore_clk = get_ipcore_clk();                  //从接口读取IPCORE_CLK主频
    #endif

    //获取PCLK时钟有2种方式
    #if 0
    uint32_t pclk = ipcore_clk / 2;                          //默认为2分频，从主频计算得到PCLK
    #else
    uint32_t pclk = get_apb_clk();                           //从接口读取PCLK时钟
    #endif

    //获取UART时钟有2种方式
    #if 0
    uint32_t uart_clk = ipcore_clk / 2;                      //SDK配置的是2分频，从主频计算得到UART时钟
    #else
    uint32_t uart_clk = get_ahb_clk() / 2;                   //AHB时钟和IPCORE_CLK时钟相同
    #endif

    //获取ADC时钟
    uint32_t adc_clk = ipcore_clk / 12;                      //默认为12分频，从主频计算得到ADC时钟

    //获取DTRFLASH时钟
    uint32_t system_dtrflash_clk = ipcore_clk / 1 ;          //SDK配置的是1分频，从主频计算得到DTRFLASH系统时钟
    uint32_t dtrflash_out_clk = system_dtrflash_clk / 2      //SDK配置FLASH控制器内部为2分频（2、4、6、8可选），FLASH输出的CLK最大为主频的1半

    //获取PWM或TIMER时钟有2种方式
    #if 0
    uint32_t pwm_timer_clk = pclk;                           //选择时钟源为PCLK时，PWM或timer的系统时钟为PCLK
    #else
    uint32_t pwm_timer_clk = get_src_clk() / 16;             //选择时钟源为晶振时，默认分频为16，计算得到PWM或timer的系统时钟
    #endif

    //获取IWDG的时钟
    uint32_t iwdg_clk = get_src_clk() / 16;                  //默认分频为16，计算得到IWDG的系统时钟

    //获取WWDG、iic、gpio的时钟
    uint32_t wwdg_clk = pclk;
    uint32_t iic_clk = pclk;
    uint32_t gpio_clk = pclk;
}
```

* 如需修改各外设的分频值，可参考下列方式调用接口：

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_system.h"
#include "platform_config.h"

void device_div_test()
{
    scu_set_div_parameter(IPCORE_BASE,2);       //设置IPCORE_CLK为2分频

    uint32_t ipcore_clk = dpmu_get_pll_frequency() / 2;
    set_ipcore_clk(ipcore_clk);                 //设置系统主频全局变量
    set_apb_clk(ipcore_clk);                    

    scu_set_div_parameter(APB_BASE,2);          //设置PCLK为2分频
    set_apb_clk(ipcore_clk/2);                  //设置PCLK全局变量

    scu_set_div_parameter(HAL_UART0_BASE,2);    //设置UART0为2分频
    scu_set_div_parameter(HAL_UART1_BASE,2);    //设置UART1为2分频
    scu_set_div_parameter(HAL_UART2_BASE,2);    //设置UART2为2分频
    scu_set_div_parameter(HAL_PWM0_BASE,4);     //设置PWM0、PWM1、PWM2、PWM3、TIMER0、TIMER1、TIMER2、TIMER3为4分频，共用1个分频器
    scu_set_div_parameter(HAL_PWM4_BASE,4);     //设置PWM4、PWM5均为4分频，共用1个分频器
    scu_set_div_parameter(HAL_IWDG_BASE,6);     //设置IWDG为6分频
    scu_set_div_parameter(HAL_DTRFLASH_BASE,2); //设置DTRFLASH为2分频
    scu_set_div_parameter(HAL_ADC_BASE,8);      //设置ADC为8分频
}
```

## 5、IIS时钟配置

### 5.1、IIS时钟分频关系介绍

* 先了解几个IIS的专有名词，以下会用此类名词来说明IIS时钟配置。
* IIS具有独立的时钟和数据信号，IIS接口由MCLK、SCK、LRCK、SDI、SDO这些信号线组成。
* MCLK：主时钟，通常为音频采样率（LRCK的频率）的128/192/256/384倍。例如16000的音频采样率，倍频256，可得到4096000的MCLK。
* SCK：位时钟，一个时钟周期传输一个bit数据，通常为音频采样率（LRCK时钟）的32、64倍。例如16000的音频采样率，倍频64，可得到1024000的SCK。
* LRCK：帧时钟，用于切换左右声道的数据，IIS格式下LRCK为0表示当前数据帧是左声道数据，为1表示当前数据帧是右声道数据。
* 更多关于IIS的内容，请参考文档《音频数字传输总线(IIS)》
* IIS的时钟分频配置，有单独的一套配置接口，且支持多组IIS使用独立的时钟通路，以下仅说明做为master模式时的时钟配置流程。
* 下图展示了IIS的时钟分频流程

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/img/%E6%97%B6%E9%92%9F%E9%85%8D%E7%BD%AE-2.png)

图2 IIS时钟分频图

* 第1级时钟来源选择有4个，分别为：

```
//位于#include "ci130x_scu.h"中

/**
 * @brief MCLK 时钟源SRC的 来源选择
 *
 */
typedef enum
{
    //!来源于 IPCORE 主频
    IIS_SRC_SOURCE_IPCORE   = 0,
    //!来源于 EXT_OSC 外部晶振
    IIS_SRC_SOURCE_EXT_OSC  = 1,
    //!来源于 INTERNAL RC 内部晶振
    IIS_SRC_SOURCE_INTER_RC = 2,
    //!来源于 PAD IN 从IIS_MCLK_PAD输入
    IIS_SRC_SOURCE_PAD_IN   = 3,
}IIS_Src_Source_t;
```

* 第2级SRC\_MCLK选择有4个，前3个可独立配置不同的分频和时钟来源（IIS\_Src\_Source\_t），第4个由PAD输入，这4个SRC\_MCLK可作为CODEC、PDM、MCLK0~MCLK3的时钟来源：

```
//位于#include "ci130x_scu.h"中

/**
 * @brief MCLK 时钟来源选择
 *
 */
typedef enum
{
    //!来源于 SRC0
    IIS_MCLK_SOURCE_SRC0    = 0,  //可配置分频，分频宽度为10bit（0 ~ 1023）
    //!来源于 SRC1
    IIS_MCLK_SOURCE_SRC1    = 1,  //可配置分频，分频宽度为10bit（0 ~ 1023）
    //!来源于 SRC2
    IIS_MCLK_SOURCE_SRC2    = 2,  //可配置分频，分频宽度为10bit（0 ~ 1023）
    //!来源于 PAD IN，
    IIS_MCLK_SOURCE_PAD_IN  = 3,  //不分频，从IIS_MCLK_PAD输入
}IIS_Mclk_Source_t;
```

* 第3级MCLK的选择有3个，可独立配置不同的过采样率和SCK/LRCK比值，支持的过采样率有4个，支持的SCK和LRCK比值有2个

```
//位于#include "ci130x_scu.h"中

/**
 * @brief MCLK 选择
 *
 */
typedef enum
{
    //!MCLK0
    IIS_MCLK_MCLK0    = 0,
    //!MCLK1
    IIS_MCLK_MCLK1    = 1,
    //!MCLK2
    IIS_MCLK_MCLK2    = 2,
}IIS_Mclk_t;

/**
 * @brief IIS MCLK 过采样率选择
 *
 */
typedef enum
{
    //!过采样率128
    IIS_MCLK_FS_128   = 0,
    //!过采样率192
    IIS_MCLK_FS_192   = 1,
    //!过采样率256
    IIS_MCLK_FS_256   = 2,
    //!过采样率384
    IIS_MCLK_FS_384   = 3,
}IIS_Mclk_Fs_t;

/**
 * @brief IIS SCK和LRCK的频率关系比值
 *
 */
typedef enum
{
    //!SCK/LRCK=32
    IIS_SCK_LRCK_WID_32  = 0,
    //!SCK/LRCK=64
    IIS_SCK_LRCK_WID_64  = 1,
}IIS_Sck_Lrck_Wid_t;
```

* 第4级每个IIS的SCK和LRCK来源选择有6个，也能作为PAD输出SCK和LRCK的来源：

```
//位于#include "ci130x_scu.h"中

/**
 * @brief IIS SCK/LRCK输出来源
 *
 */
typedef enum
{
    //!MCLK0产生的SCK/LRCK
    IIS_CLK_SOURCE_MCLK0     = 0,
    //!MCLK1产生的SCK/LRCK
    IIS_CLK_SOURCE_MCLK1     = 1,
    //!MCLK2产生的SCK/LRCK
    IIS_CLK_SOURCE_MCLK2     = 2,
    //!PAD输入的SCK/LRCK
    IIS_CLK_SOURCE_PAD_IN    = 3,
    //!codec_ad_sck_in
    IIS_CLK_SOURCE_CODEC_AD  = 4,
    //!codec_da_sck_in
    IIS_CLK_SOURCE_CODEC_DA  = 5,
    //!pdm_sck_in
    IIS_CLK_SOURCE_PDM       = 6,
}IIS_Clk_Source_t;
```

### 5.2、IIS时钟示例代码

* 分一步配置的接口定义：

```
//位于#include "ci130x_scu.h"中

void iis_clk_config(IIS_Clk_ConfigTypedef* config);
```

* 调用一步配置接口，配置IIS0为master模式的时钟通路示例：

```
#include "ci130x_scu.h"
#include "platform_config.h"

void init_iis_clk_sample()
{
    uint32_t sample_rate = 16000;                       //音频采样率
    uint32_t ipcore_clk = get_ipcore_clk();             //时钟源频率
    uint32_t fs = 256;

    float div = ((float)ipcore_clk / (float)sample_rate / (float)fs);  //根据时钟源频率、音频采样率和过采样率，计算分频值

    IIS_Clk_ConfigTypedef config = {0};
    config.device_select = IISNum0;                     //配置IIS0的时钟通路
    config.model_sel = IIS_MASTER;                      //配置为master模式
    config.src_cfg.source = IIS_SRC_SOURCE_IPCORE;      //选择时钟源为IPCORE_CLK主频
    config.mclk_cfg.src = IIS_MCLK_SOURCE_SRC0;         //选择MCLK_SRC0寄存器
    config.mclk_cfg.mclk = IIS_MCLK_MCLK0;              //选择MCLK0寄存器和SCK/LRCK比值
    config.mclk_cfg.fs = IIS_MCLK_FS_256;               //配置过采样率为256
    config.mclk_cfg.sck_lrck = IIS_SCK_LRCK_WID_64;     //配置SCK/LRCK比值为64
    config.clk_cfg = IIS_CLK_SOURCE_MCLK0;              //选择MCLK0产生的SCK和LRCK作为IIS0的时钟
    config.mclk_mode = IIS_MCLK_OUT;                    //通过PAD输出MCLK时钟
    config.clk_mode = IIS_SCKLRCK_OUT;                  //通过PAD输出SCK、LRCK时钟
    config.src_cfg.source_div = (uint16_t)div;          //配置分频参数
    iis_clk_config(&config);

    uint32_t lrck = sample_rate;                        //LRCK的频率
    uint32_t sck = sample_rate * 64;                    //计算得到SCK的频率
    uint32_t mclk = sample_rate * fs;                   //计算得到MCLK的频率
}
```

* 分多步配置的接口定义：

```
//位于#include "ci130x_scu.h"中

void scu_iis_src_config(IIS_Src_Config_t *config,IIS_Mclk_Source_t src);        //选择MCLK_SRC0/1/2寄存器，配置时钟来源和分频
void scu_iis_mclk_config(IIS_Mclk_Config_t *config);                            //选择MCLK0/1/2寄存器，配置过采样率和SCK/LRCK比值
void scu_iis_clk_config(IISNumx device, IIS_Clk_Source_t clk_source);           //配置IIS0/1/2的SCK、LRCK时钟来源
void scu_iis_codec_mclk_config(Codec_Channel_t channel, IIS_Mclk_Source_t src); //配置CODEC AD或CODEC DA的MCLK时钟来源
void scu_iis_pdm_mclk_config(IIS_Mclk_Source_t src);                            //配置PDM的MCLK时钟来源
void scu_iis_pad_mclk_config(IIS_Mclk_Source_t src, IIS_Clk_Mode_t mode);       //配置MCLK PAD的输入输出模式，为输出时选择输出时钟来源
void scu_iis_pad_clk_config(IIS_Clk_Source_t clk_source, IIS_Clk_Mode_t mode);  //配置SCK、LRCK PAD的输入输出模式，为输出时选择输出时钟来源
```

* 其实一步配置接口内，也是调用的分步接口。

## 6、注意事项

* 通常配置晶振选项是在配置PLL时钟之前
* 修改外设分频时，需关闭外设时钟，再修改分频值，再打开时钟。