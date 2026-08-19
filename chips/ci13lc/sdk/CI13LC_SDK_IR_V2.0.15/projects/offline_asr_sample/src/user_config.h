/**/
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define CI_CHIP_TYPE                13242

#define BOARD_CONFIG_FILE           "CI-F1324XGS0XJ-V10.h"

//**系统主时钟频率配置
#define MAIN_FREQUENCY              210000000               // 主频率配置


//**IIS采音功能开关配置
#define USE_IIS1_OUT_PRE_RSLT_AUDIO 0   //1,开启IIS采音功能，可以使用采音板采音。
                                        //0,关闭IIS采音功能。
#if (CI_CHIP_TYPE == 13081)
#define CONFIG_CI_LOG_UART          HAL_UART2_BASE  //配置log输出使用的串口。
#else
#define CONFIG_CI_LOG_UART          HAL_UART0_BASE  //配置log输出使用的串口。
#endif

#define MSG_COM_USE_UART_EN         0   //0,关闭语音模块通信协议。1,开启语音模块通信协议。
#define UART_PROTOCOL_NUMBER        HAL_UART1_BASE      // 语音模块协议使用的串口HAL_UART0_BASE ~ HAL_UART2_BASE
#define UART_PROTOCOL_BAUDRATE      9600//(UART_BaudRate9600) // 语音模块协议使用的串口波特率。
#define UART_PROTOCOL_VER           2   //串口协议版本号，1：一代协议，2：二代协议，255：平台生成协议(只有发送没有接收)

//**语音识别配置
#define USE_SEPARATE_WAKEUP_EN      1 //是否使用独立的唤醒词模型。1:是 0:否。
#define DEFAULT_MODEL_GROUP_ID      1 //模型ID,用于指定上电启动时,默认进入的语言模型。通常0为
                                      //命令词模型，1为唤醒词模型。
#define ASR_FE_REDUCE_MEM           1 //默认打开   0：原方案   1：省内存方案（15K左右）

#if (!USE_SEPARATE_WAKEUP_EN)
#undef DEFAULT_MODEL_GROUP_ID
#define DEFAULT_MODEL_GROUP_ID      0
#endif

//默认配置不变  只有13081才改
#define IR_BOARN_LEVEL              0   // 0:配置01U   1：配置02U      0u1是可以开红外接收脚   02u是不可以开红外接收脚
#define HPOUT_NIGHTPWM              0   // 是否用功放脚PC来配置成小夜灯输出


//**语音播报配置
#if(HPOUT_NIGHTPWM == 0)
#define PLAY_WELCOME_EN             1   //是否在启动时播放开机提示音。1:是 0:否。
#define PLAY_ENTER_WAKEUP_EN        1   //是否在唤醒时播放提示音。1:是 0:否。
#define PLAY_EXIT_WAKEUP_EN         1   //是否在切换到只监听唤词状态时播放提示音。1:是 0:否。
#define PLAY_OTHER_CMD_EN           1   //是否在识别到命令词时播放提示音。1:是 0:否。
#elif (HPOUT_NIGHTPWM == 1)
#define PLAY_WELCOME_EN             0   //是否在启动时播放开机提示音。1:是 0:否。
#define PLAY_ENTER_WAKEUP_EN        0   //是否在唤醒时播放提示音。1:是 0:否。
#define PLAY_EXIT_WAKEUP_EN         0   //是否在切换到只监听唤词状态时播放提示音。1:是 0:否。
#define PLAY_OTHER_CMD_EN           0   //是否在识别到命令词时播放提示音。1:是 0:否。
#endif
#define ADAPTIVE_THRESHOLD          0
#define ASR_SKIP_FRAME_CONFIG       0
#define EXIT_WAKEUP_TIME            15*1000 /*退出唤醒超时时间,单位毫秒。超过此配置指定的时间长度
                                            内没有识别到任何命令词，就会切换到只监听唤词状态。*/

//**播放器配置
#define AUDIO_PLAYER_ENABLE         1   //是否使用音频播放器。0:不启用,1:启用。不需要播放功能时，关闭此功能可以节省空间。
#define PLAYER_CONTROL_PA           0   //是否由播放器控制音频功放开关。0:功放常开，1:在需要播放时才打开功放，但每次播放可能增加一点延迟。

#define VOLUME_MAX                  7   //设置音量调节的上限值，对应硬件支持的最大音量。
#define VOLUME_MIN                  1   //设置音量调节的下限值，对应最小音量。
#define VOLUME_DEFAULT              5   //设置音量调节的默认值。

#if AUDIO_PLAYER_ENABLE
#define USE_PROMPT_DECODER          1   //播放器是否支持prompt解码器
#define USE_MP3_DECODER             1   //为1时加入mp3解码器
#define AUDIO_PLAY_SUPPT_MP3_PROMPT 1   //播放器是否开启mp3提示音，1:是 0:否。
#endif  // AUDIO_PLAYER_ENABLE

#define USE_DENOISE_MODULE          0   //使用降噪模块:1开启，0关闭

#endif /* _USER_CONFIG_H_ */
