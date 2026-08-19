/**/
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define CI_CHIP_TYPE                13322

#define BOARD_CONFIG_FILE           "CI-F32XGT01D-V10.h"

//**系统主时钟频率配置
#define MAIN_FREQUENCY              210000000               // 主频率配置


//**IIS采音功能开关配置
#define USE_IIS1_OUT_PRE_RSLT_AUDIO 0   //1,开启IIS采音功能，可以使用采音板采音。
                                        //0,关闭IIS采音功能。

#define CONFIG_CI_LOG_UART          HAL_UART0_BASE  //配置log输出使用的串口。

#define MSG_COM_USE_UART_EN         1   //0,关闭语音模块通信协议。1,开启语音模块通信协议。
#define UART_PROTOCOL_NUMBER        HAL_UART2_BASE      // 语音模块协议使用的串口HAL_UART0_BASE ~ HAL_UART2_BASE
#define UART_PROTOCOL_BAUDRATE      9600//(UART_BaudRate9600) // 语音模块协议使用的串口波特率。
#define UART_PROTOCOL_VER           2   //串口协议版本号，1：一代协议，2：二代协议，255：平台生成协议(只有发送没有接收)

//**语音识别配置
#define USE_SEPARATE_WAKEUP_EN      1 //是否使用独立的唤醒词模型。1:是 0:否。
#define DEFAULT_MODEL_GROUP_ID      1 //模型ID,用于指定上电启动时,默认进入的语言模型。通常0为
                                      //命令词模型，1为唤醒词模型。

#if (!USE_SEPARATE_WAKEUP_EN)
#undef DEFAULT_MODEL_GROUP_ID
#define DEFAULT_MODEL_GROUP_ID      0
#endif


//**语音播报配置
#define PLAY_WELCOME_EN             1   //是否在启动时播放开机提示音。1:是 0:否。
#define PLAY_ENTER_WAKEUP_EN        1   //是否在唤醒时播放提示音。1:是 0:否。
#define PLAY_EXIT_WAKEUP_EN         1   //是否在切换到只监听唤词状态时播放提示音。1:是 0:否。
#define PLAY_OTHER_CMD_EN           1   //是否在识别到命令词时播放提示音。1:是 0:否。
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

#define USE_CWSL                    1   //命令词自学习(command word self-learning)


#if USE_CWSL
/*
* 自学习最大限制1000个命令词节点（nums_arcs）+ 10条自学习这种组合，
* 节点少了自学习条数可以增加，自学习条数少了命令词节点可以增加。
*/
/*
* 开启自学习功能时，为了避免指令词被学习成模板，
* 请确保Excel中词条语义ID、命令词ID与需学习的词条语义ID、命令词ID不重复。
*/
#define CWSL_WAKEUP_NUMBER          2           // 可学习的唤醒词数量
#define WAKE_UP_ID                  1           // 学习的唤醒词对应的命令词ID
#define CWSL_REG_TIMES              1           // 学习时 每个词需说几遍，默认 1 遍即可,支持1、2、3遍,最大支持 3 遍;
#define CWSL_WAKEUP_THRESHOLD       37          // 学习的唤醒词阈值门限，越小越灵敏，默认 37, 最小可配置到 32;
#define CWSL_CMD_THRESHOLD          35          // 学习的命令词阈值门限，越小越灵敏，默认 35，最小可配置到 30；
#define FOR_REG_2TIMES_FLOW_V2      0           // 学习时，说两遍/三遍逻辑，版本二流程，后续均和第一次的比较，一致学习成功，不一致，最多支持说 3 次\
                                                    FOR_REG_2TIMES_FLOW_V2 配置 1时, CWSL_REG_TIMES 必须是 2或3	
#define CWSL_REG_VAD_LEVEL          0           // 学习过程，灵敏度选项配置： 0 低灵敏度，可减少噪声对学习的干扰，需学习过程大声说话；1 高灵敏度，但也可以导致干扰噪声干扰学习
#define CICWSL_TOTAL_TEMPLATE       10          //可存储模板数量，最好用到多少设置成多少,设置多少就会预留多少空间
#define ASR_FE_REDUCE_MEM           1           //默认打开   0：原方案   1：省内存方案（15K左右）
#endif


#endif /* _USER_CONFIG_H_ */
