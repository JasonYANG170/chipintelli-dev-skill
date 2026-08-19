/**/
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define CI_CHIP_TYPE 1306


#define AUDIO_PLAYER_ENABLE 1 //用于屏蔽播放器任务相关代码      0：屏蔽，1：开启

#if AUDIO_PLAYER_ENABLE
#define USE_PROMPT_DECODER 1          //播放器是否支持prompt解码器
#define USE_MP3_DECODER 1             //为1时加入mp3解码器
#define AUDIO_PLAY_SUPPT_MP3_PROMPT 1 //播放器默认开启mp3播报音
#define USE_MS_WAV_DECODER 0          //播放器是否支持ms wav解码器

#define AUDIO_PLAY_BLOCK_CONT 4 //播放器底层缓冲区个数
#endif

#define USE_ALC_AUTO_SWITCH_MODULE 1 //使用动态alc模块:1开启，0关闭
#define USE_DENOISE_MODULE 0         //使用降噪模块:1开启，0关闭
#define USE_AEC_MODULE 0             //使用回声消除模块:1开启，0关闭

#if USE_AEC_MODULE
#define PAUSE_VOICE_IN_WITH_PLAYING  0//开启aec时关闭
#endif

#define CONFIG_SYSTEMVIEW_EN 0 //不使能systemview

// #define SYSTEM_VIEW_UART1_PRINT_UART0   1

// #if SYSTEM_VIEW_UART1_PRINT_UART0
#define CONFIG_CI_LOG_UART HAL_UART0_BASE
// #else
// #define CONFIG_CI_LOG_UART                  HAL_UART1_BASE
// #endif#define CONFIG_CI_LOG_UART                  HAL_UART0_BASE

#define MSG_COM_USE_UART_EN 1
#define UART_PROTOCOL_NUMBER (HAL_UART2_BASE) // HAL_UART0_BASE ~ HAL_UART2_BASE
#define UART_PROTOCOL_BAUDRATE (UART_BaudRate9600)
#define UART_PROTOCOL_VER 2 //串口协议版本号，1：一代协议，2：二代协议，255：平台生成协议(只有发送没有接收)

#define USE_CWSL                            1   //命令词自学习(command word self-learning)
#endif /* _USER_CONFIG_H_ */
