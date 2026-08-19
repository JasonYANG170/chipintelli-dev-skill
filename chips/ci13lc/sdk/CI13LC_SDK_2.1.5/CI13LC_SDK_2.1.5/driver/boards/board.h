#ifndef __CI_BOARD_H__
#define __CI_BOARD_H__

#include "user_config.h"
#include "ci_system.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define CONCAT(a,b) a##b
#define INCLUDE_BOARD_HEADER(a,b) TOSTRING(CONCAT(a,b).h)
#define PREFEX  ci
#include INCLUDE_BOARD_HEADER(PREFEX, CI_CHIP_TYPE)

#include BOARD_CONFIG_FILE

//**波特率自适应功能配置
#if (USE_EXTERNAL_CRYSTAL_OSC == 0)     //使用内部RC时,建议开启波特率自适应（需要电控增加对应支持）。
#undef UART_BAUDRATE_CALIBRATE
#define UART_BAUDRATE_CALIBRATE         1  // 是否使能波特率校准功能。开启后,连续发包间隔必须大于1ms,否则可能丢包。
#endif

/**
 * @brief 引脚复用配置为UART功能
 *
 * @param UARTx UART组 : UART0, UART1
 */
void pad_config_for_uart(UART_TypeDef *UARTx);

/**
 * @brief 引脚复用配置为IIS功能
 */
void pad_config_for_iis(void);

/**
 * @brief 引脚复用配置为GPIO,用于控制功放使能
 *
 */
void pad_config_for_power_amplifier(void);

/**
 * @brief 开启功放使能
 *
 */
void power_amplifier_on(void);

/**
 * @brief 关闭功放使能
 *
 */
void power_amplifier_off(void);

/**
  * @功能:引脚复用配置为IIC功能
  * @
  */
void pad_config_for_i2c(void);

/**
 * @brief 录音codec注册
 *
 */
void audio_in_codec_registe();


/**
 * @brief 语音前处理使用IIS输出功能的初始化
 * 
 */
void audio_pre_rslt_out_codec_init(void);

#endif //__CI_BOARD_H__

