#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__


//**板级配置选择
/*板级配置更多细节请查看:https://document.chipintelli.com/硬件资料-->模块手册
chipintelli提供的部分开发板和模组，可以通过下面的宏选择，也可以参考开发板的板级配置文
件添加自定义板级配置文件*/
#define USE_CI_D02GS01J_BOARD       0   //CI-D0XGS01J，端子模块，芯片型号必须设置为1302
#define USE_CI_D02GS02S_BOARD       0   //CI-D0XGS02S，SMT模块，芯片型号必须设置为1302
#define USE_CI_D12GS01J_BOARD       0   //CI-D0XGS01J，端子模块，芯片型号必须设置为1312JE
#define USE_CI_D06GT01D_BOARD       1   //CI-D06GT01D，开发版，芯片型号必须设置为1306
#define USE_CUS_XXXXXXX_BOARD       0   //用户自定义

#if (USE_CI_D02GS01J_BOARD == 1)
#define CI_CHIP_TYPE                1302    //flash:2MB,SSOP24
#define BOARD_PORT_FILE             "CI-D02GS01J.c"
#elif (USE_CI_D02GS02S_BOARD == 1)
#define CI_CHIP_TYPE                1302    //flash:2MB,SSOP24
#define BOARD_PORT_FILE             "CI-D02GS02S.c"
#elif (USE_CI_D12GS01J_BOARD == 1)
#define CI_CHIP_TYPE                1312    //flash:2MB,SSOP16
#define BOARD_PORT_FILE             "CI-D12GS01J.c"
#elif (USE_CI_D06GT01D_BOARD == 1)
#define CI_CHIP_TYPE                1306    //flash:4MB,QFN40
#define BOARD_PORT_FILE             "CI-D06GT01D.c"
#elif (USE_CUS_XXXXXXX_BOARD == 1)
#define CI_CHIP_TYPE                xxxx    //flash:4MB,QFN40
#define BOARD_PORT_FILE             "CI-XXXX.c"
#endif

//**麦克风电路模式配置
#define MIC_DIFF_SINGLE             0   /*1,单端。0，差分（通用模块都是差分模式，省成本的模块为单端
                                        （MICN_L 接GND）时，需要配置为SINGLE）*/

//**IIS彩音功能开关配置
#define USE_IIS1_OUT_PRE_RSLT_AUDIO 0   //1,开启IIS采音功能,可以使用采音板彩音,占用PA2~PA6。
                                        //0,关闭IIS采音功能,PA2~PA6可以用于其它功能。

//**通讯串口配置
#define CONFIG_CI_LOG_UART          HAL_UART0_BASE  //配置log输出使用的串口

#define MSG_COM_USE_UART_EN         1   //0,关闭语音模块通讯协议。1,开启语音模块通讯协议。
#define UART_PROTOCOL_NUMBER        (HAL_UART2_BASE)    //语音模块协议使用的串口。
#define UART_PROTOCOL_BAUDRATE      (UART_BaudRate9600) //语音模块协议使用的串口波特率。
#define UART_PROTOCOL_VER           2   //语音模块协议版本号:1,一代协议。2,二代协议，255,平台生成协议

//**通信串口引脚开漏模式使能配置
//注:推挽模式的IO只能对接3.3V电平的IO，开漏模式可以对接5V电平的IO(外部需要上拉到5V)
#define UART0_PAD_OPENDRAIN_MODE_EN 0   //0,UART0为推挽模式。1,UART0为开漏模式。
#define UART1_PAD_OPENDRAIN_MODE_EN 1   //0,UART1为推挽模式。1,UART1为开漏模式。
#define UART2_PAD_OPENDRAIN_MODE_EN 0   //0,UART2为推挽模式。1,UART2为开漏模式。

//**时钟源配置
#if ((CI_CHIP_TYPE == 1312) || (CI_CHIP_TYPE == 1311))
#define USE_EXTERNAL_CRYSTAL_OSC    0
#else
#define USE_EXTERNAL_CRYSTAL_OSC    1  //0:使用内部RC作为时钟源。1:使用外部晶振作为时钟源。
#endif

//**波特率自适应功能配置
#if (USE_EXTERNAL_CRYSTAL_OSC == 0)    //使用内部RC时,建议开启波特率自适应(需要电控增加对应支持)。
#define UART_BAUDRATE_CALIBRATE     1  //是否使能波特率自适应功能。
#define BAUDRATE_SYNC_PERIOD        300000  // 波特率同步周期，单位毫秒。
#define BAUDRATE_FAST_SYNC_PERIOD   5000    // 一次波特率同步失败后，下一次同步间隔，单位毫秒。
#define BAUD_CALIBRATE_MAX_WAIT_TIME    400 // 等待反馈包的超时时间，单位毫秒。
#endif

//**语音识别配置
#define USE_SEPARATE_WAKEUP_EN      1   //是否使用独立的唤醒词模型。1:是 0:否。
#define DEFAULT_MODEL_GROUP_ID      1   /*模型ID,用于指定上电启动时，默认进入的语言模型。通常0为
                                        命令词模型,1为唤醒词模型。*/

#if (!USE_SEPARATE_WAKEUP_EN)
#undef DEFAULT_MODEL_GROUP_ID
#define DEFAULT_MODEL_GROUP_ID      0
#endif

#define PLAY_WELCOME_EN             1   //是否在启动时播放开机提示音。1:是 0:否。
#define PLAY_ENTER_WAKEUP_EN        1   //是否在唤醒时播放提示音。1:是 0:否。
#define PLAY_EXIT_WAKEUP_EN         1   //是否在切换到只监听唤词状态时播放提示音。1:是 0:否。
#define PLAY_OTHER_CMD_EN           1   //是否在识别到命令词时播放提示音。1:是 0:否。
#define ADAPTIVE_THRESHOLD          0
#define ASR_SKIP_FRAME_CONFIG       0
#define EXIT_WAKEUP_TIME            15*1000 /*退出唤醒超时时间,单位毫秒。超过此配置指定的时间长度
                                            内没有识别到任何命令词，就会切换到只监听唤词状态。*/

//**播放器配置
#define AUDIO_PLAYER_ENABLE         1   //是否启用音频播放器。0:不启用,1:启用。不时用播放功能时，
                                        //关闭此功能可以节省内存空间。
#define PLAYER_CONTROL_PA           0   /*是否有播放器控音频功放开关。0:功放常开,1:播放器在需要播
                                        放时才打开,但可能增加一点每一次播放的延迟时间。*/
#define VOLUME_MAX                  7   //设置音量调节的上限值，对应硬件支持的最大音量。
#define VOLUME_MIN                  1   //设置音量调节的下限值，对应最小音量。
#define VOLUME_DEFAULT              5   //设置音量调节的默认值。

#if AUDIO_PLAYER_ENABLE
#define USE_PROMPT_DECODER          1   //播放器是否支持prompt解码器，1:是 0:否。
#define USE_MP3_DECODER             1   //为1时加入mp3解码器，1:是 0:否。
#define AUDIO_PLAY_SUPPT_MP3_PROMPT 1   //播放器是否开启mp3提示音，1:是 0:否。
#endif

//**算法功能配置及其他参数
#define USE_ALC_AUTO_SWITCH_MODULE  1   //是否开启动态ALC功能，1:开启，0:关闭。
#define USE_DENOISE_MODULE          0   //是否开启降噪功能，1:开启，0:关闭。
#define USE_AEC_MODULE              0   //是否开启回声消除功能，1:开启，0:关闭。

#if USE_AEC_MODULE
#define IF_JUST_CLOSE_HPOUT_WHILE_NO_PLAY   1
#define HOST_CODEC_CHA_NUM          2
#endif


#endif /* _USER_CONFIG_H_ */
