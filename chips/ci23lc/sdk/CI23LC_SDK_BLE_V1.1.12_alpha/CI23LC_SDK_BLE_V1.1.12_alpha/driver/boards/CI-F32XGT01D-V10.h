

#ifndef __CI_F32XGT01D_V10_H__
#define __CI_F32XGT01D_V10_H__


#define BD_SURPPORT_CHIP_NAME       13322                   // 指定支持的芯片型号


//时钟配置/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SRC_FREQUENCY_NORMAL        12288000U               // 配置时钟源频率
#define SRC_FREQUENCY_LOWPOWER      SRC_FREQUENCY_NORMAL        
#define USE_EXTERNAL_CRYSTAL_OSC    1                       //是否使用外部晶振。0：不使用外部晶振时钟；1：使用外部晶振时钟；

//麦克风引脚配置///////////////////////////////////////////////////////////////////////////////////////////////////
#define MIC_DIFF_SINGLE             0                       // MIC是否使用了差分方式输入，0:使用差分输入，1:使用单端输入

//UART0引脚配置////////////////////////////////////////////////////////////////////////////////////////////////////
#define UART0_PIN_EN                1                       // 是否使用能UART0引脚配置，1:使能，0:不使能.
#define UART0_TX_PIN                PB5                     // UART0 TX引脚选择
#define UART0_TX_PULL               DPMU_IO_PULL_DISABLE    // UART0 TX引脚上下拉选择
#define UART0_TX_PIN_REUSE          SECOND_FUNCTION         // UART0 TX引脚复用功能选择
#define UART0_RX_PIN                PB6                     // UART0 RX引脚选择
#define UART0_RX_PULL               DPMU_IO_PULL_DISABLE    // UART0 RX引脚上下拉选择
#define UART0_RX_PIN_REUSE          SECOND_FUNCTION         // UART0 RX引脚复用功能选择
#define UART0_OPEN_DRAIN_EN         0                       // UART0 是否使用开漏模式，1:使用开漏模式，0:不使用开漏模式
                                                            //注:推挽模式的IO只能对接3.3V电平的IO, 开漏模式可以对接5V电平的IO(需要外部上拉到5V)

//UART1引脚配置////////////////////////////////////////////////////////////////////////////////////////////////////
#define UART1_PIN_EN                1                       // 是否使用能UART1引脚配置，1:使能，0:不使能.
#define UART1_TX_PIN                PA2                     // UART1 TX引脚选择
#define UART1_TX_PULL               DPMU_IO_PULL_DISABLE    // UART1 TX引脚上下拉选择
#define UART1_TX_PIN_REUSE          FORTH_FUNCTION          // UART1 TX引脚复用功能选择
#define UART1_RX_PIN                PA3                     // UART1 RX引脚选择
#define UART1_RX_PULL               DPMU_IO_PULL_DISABLE    // UART1 RX引脚上下拉选择
#define UART1_RX_PIN_REUSE          FORTH_FUNCTION          // UART1 RX引脚复用功能选择
#define UART1_OPEN_DRAIN_EN         0                       // UART1 是否使用开漏模式，1:使用开漏模式，0:不使用开漏模式
                                                            //注:推挽模式的IO只能对接3.3V电平的IO, 开漏模式可以对接5V电平的IO(需要外部上拉到5V)

//UART2引脚配置////////////////////////////////////////////////////////////////////////////////////////////////////////
#define UART2_PIN_EN                1                       // 是否使用能UART2引脚配置，1:使能，0:不使能.
#define UART2_TX_PIN                PB1                     // UART2 TX引脚选择
#define UART2_TX_PULL               DPMU_IO_PULL_DISABLE    // UART2 TX引脚上下拉选择
#define UART2_TX_PIN_REUSE          THIRD_FUNCTION          // UART2 TX引脚复用功能选择
#define UART2_RX_PIN                PB2                     // UART2 RX引脚选择
#define UART2_RX_PULL               DPMU_IO_PULL_DISABLE    // UART2 RX引脚上下拉选择
#define UART2_RX_PIN_REUSE          THIRD_FUNCTION          // UART2 RX引脚复用功能选择
#define UART2_OPEN_DRAIN_EN         0                       // UART2 是否使用开漏模式，1:使用开漏模式，0:不使用开漏模式
                                                            //注:推挽模式的IO只能对接3.3V电平的IO, 开漏模式可以对接5V电平的IO(需要外部上拉到5V)

//IIS引脚配置/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXT_IIS_PIN_EN              1                       // 是否使能IIS引脚配置，1:使能，0:不使能.
#define EXT_IIS_SDI_PIN             PA2                     // IIS SDI引脚选择
#define EXT_IIS_SDI_PIN_REUSE       SECOND_FUNCTION         // IIS SDI引脚复用功能选择
#define EXT_IIS_LRCLK_PIN           PA3                     // IIS LRCLK引脚选择
#define EXT_IIS_LRCLK_PIN_REUSE     SECOND_FUNCTION         // IIS LRCLK引脚复用功能选择
#define EXT_IIS_SDO_PIN             PA4                     // IIS SDO引脚选择
#define EXT_IIS_SDO_PIN_REUSE       SECOND_FUNCTION         // IIS SDO引脚复用功能选择
#define EXT_IIS_SCLK_PIN            PA5                     // IIS SCLK引脚选择
#define EXT_IIS_SCLK_PIN_REUSE      SECOND_FUNCTION         // IIS SCLK引脚复用功能选择
#define EXT_IIS_MCLK_PIN            PA6                     // IIS MCLK引脚选择
#define EXT_IIS_MCLK_PIN_REUSE      SECOND_FUNCTION         // IIS MCLK引脚复用功能选择 

//IIC//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define IIC0_PIN_EN                 0                       // 是否使能IIC0引脚配置，1:使能，0:不使能.
#define IIC0_SDA_PIN                PA2                     // IIC0 SDA 引脚选择
#define IIC0_SDA_PIN_REUSE          THIRD_FUNCTION          // IIC0 SDA 引脚复用功能选择
#define IIC0_SCL_PIN                PA3                     // IIC0 SCL 引脚选择
#define IIC0_SCL_PIN_REUSE          THIRD_FUNCTION          // IIC0 SCL 引脚复用功能选择

//功放mute引脚配置///////////////////////////////////////////////////////////////////////////////////////////////////////
#define AMP_MUTE_PIN_EN             1                       // 是否使能功放引脚配置，1:使能，0:不使能.
#define AMP_MUTE_LEVEL_AUTO_DET     1                       // 是否自动检测功放mute引脚的mute电平(依赖于硬件使用上下拉电阻拉到mute电平)
#define AMP_MUTE_PIN                PB7                     // 功放mute引脚选择
#define AMP_MUTE_GPIO_PORT          PB                      // 功放mute引脚GPIO口选择 
#define AMP_MUTE_GPIO_PIN           pin_7                   // 功放mute引脚GPIO pin 号选择 
#define AMP_MUTE_PIN_REUSE          FIRST_FUNCTION          // 功放mute引脚复用功能选择
#define AMP_MUTE_PIN_PULL           DPMU_IO_PULL_DISABLE    // 功放mute引脚上下拉选择

#if (AMP_MUTE_LEVEL_AUTO_DET == 0)
#define AMP_MUTE_PIN_MUTE_LEVEL     1                       // 设置功放使能引脚的mute电平
#endif


#endif


