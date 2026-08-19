/**/
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define CI_CHIP_TYPE                23242  //23242/23162

#if (CI_CHIP_TYPE == 23162)
#define BOARD_CONFIG_FILE           "CI-G16XGS02J-V10.h"
#elif (CI_CHIP_TYPE == 23242)
#define BOARD_CONFIG_FILE           "CI-G24XGS02J-V10.h"
#endif

//**系统主时钟频率配置
#define MAIN_FREQUENCY              210000000               // 主频率配置

#define CONFIG_CI_LOG_UART          HAL_UART0_BASE  //配置log输出使用的串口。HAL_UART0_BASE/HAL_UART2_BASE;不支持HAL_UART1_BASE-内部使用了

#define MSG_COM_USE_UART_EN         1   //0,关闭语音模块通信协议。1,开启语音模块通信协议。
#define UART_PROTOCOL_NUMBER        HAL_UART2_BASE      // 语音模块协议使用的串口HAL_UART0_BASE/HAL_UART2_BASE;不支持HAL_UART1_BASE-串口内部使用了
#define UART_PROTOCOL_BAUDRATE      9600//(UART_BaudRate9600) // 语音模块协议使用的串口波特率。
#define UART_PROTOCOL_VER           1   //串口协议版本号，1：一代协议，2：二代协议，255：平台生成协议(只有发送没有接收)

//**语音识别配置
#define USE_SEPARATE_WAKEUP_EN      1 //是否使用独立的唤醒词模型。1:是 0:否。
#define DEFAULT_MODEL_GROUP_ID      1 //模型ID,用于指定上电启动时,默认进入的语言模型。通常0为
                                      //命令词模型，1为唤醒词模型。
#define ASR_FE_REDUCE_MEM           0 //默认   0：原方案   1：省内存方案（15K左右）

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

#define USE_DENOISE_MODULE          0     //使用降噪模块:1开启，0关闭
#define SIMULATE_UART_ENABLE        0     // 1-换成普通IO模拟串口 0-禁用模拟串口，模拟串口使用PC4-固定波特率9600，会影响mute管脚，量产时请关闭该宏
#define SIMULATE_UART_MAX_SIZE      128   //模拟串口最大发送字节数-会占用内存，不建议设置过大
//----蓝牙相关用户配置------
#define USE_BLE_MOUDLE                      1           //使能蓝牙功能
#define USE_CI_APPLET_ENABEL                1           //使用启英物联小程序1-使能 0-关闭，客户如果用自己的小程序务必关闭
#define BLE_ADV_NAME_APPEND_FLASH_ID        1           //蓝牙广播名称自动追加2字节flash id, 保证每个设备广播名称不一样,flash_id会占用5字节，自定义长度最长为24字节
#define BLE_USER_DEFINE_ADV_NAME_CONTENT   "CI_BLE"     //用户可自定义,最长不能超过29字节，中文一个字符占3字节;
                                                        //注意如果使用启英物联小程序，最大长度不能超过18字节(厂商信息占用了字节数)

#define USE_XFI                     		_XIF_     //_XIF_:函数代码运行在flash中省内存,会降低函数运行速度;  定义为空(不赋值),不省内存;
//服务定义-如需修改该下面三个值，小程序也要做同步修改(不可用0x2900之前的值不可用)
#define BLE_UUID_CIAS_SERVICE              0XAE3A     //服务入口
#define BLE_UUID_CIAS_WRITE                0XAE3B     //手机发送数据到蓝牙芯片特征值
#define BLE_UUID_CIAS_NOTIFY               0XAE3C     //蓝牙芯片发送数据到手机特征值
#define BLE_UUID_CIAS_CMD_NOTIFY           0XAE3D     //获取词条信息特征值

#endif /* _USER_CONFIG_H_ */
