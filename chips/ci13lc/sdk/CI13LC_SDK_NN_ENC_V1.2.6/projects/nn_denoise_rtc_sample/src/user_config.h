/**/
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define CI_CHIP_TYPE                13322

#define BOARD_CONFIG_FILE           "CI-F32XGT01D-V10.h"

//**系统主时钟频率配置
#define MAIN_FREQUENCY              215000000            // 主频率配置

#define TEST_RECORD                 0
#define PCM_DATA_CHECK              0   //0 关闭   1打开 降噪后音频检测，解决电流声，建议下行才打开，打开后最后一个声音有点掉音，待优化
#define CONFIG_CI_LOG_UART          HAL_UART0_BASE       //配置log输出使用的串口。
#define RTC_EQ_UART_PORT            (HAL_UART1_BASE)     //通话降噪EQ等参数配置使用的串口-默认使用PA7(TX1) PB0(RX1)，如需修改，请改板级配置

#define MSG_COM_USE_UART_EN         1   //0,关闭语音模块通信协议。1,开启语音模块通信协议。
#define UART_PROTOCOL_NUMBER        HAL_UART2_BASE      // 语音模块协议使用的串口HAL_UART0_BASE ~ HAL_UART2_BASE
#define UART_PROTOCOL_BAUDRATE      UART_BaudRate9600//(UART_BaudRate9600) // 语音模块协议使用的串口波特率。
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
#define AUDIO_PLAYER_ENABLE         0   //是否使用音频播放器。0:不启用,1:启用。不需要播放功能时，关闭此功能可以节省空间。
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


//通话降噪相关
#define USE_AHS_MODULE             1        //使用啸叫抑制模块:1开启，0关闭

#define VOX_0_AGC_1                0        //关闭声音触发检测、使用自动增益控制
#define VOX_1_AGC_0                0        //使用声音触发检测、关闭自动增益控制
#define VOX_1_AGC_1                1        //使用声音触发检测、自动增益控制
#if VOX_0_AGC_1 || VOX_1_AGC_0 || VOX_1_AGC_1
#define USE_AGC_MODULE             1        //不可修改
#define AGC_EXPAND_AGIN 	20		// AGC作用在输入信号上的最大增益，范围[0,90]，值越大，可拉伸幅度越大
#define AGC_TARGET_dB 	    3	// 输出信号的目标电平，范围[0,31]，target_db值越大，AGC输出信号幅度越小
#define AGC_MIN_NRG_TERM 	700	//快速下降阈值，阈值越低小信号放大能量越强（噪声的抑制效果越差）
#endif

#define USE_DRC_MODULE             0        //使用动态范围压缩模块:1开启，0关闭

#define USE_EQ_MODULE              0        //使用均衡器算法模块:1开启，0关闭

#define USE_OUTPUT_BANDWIDTH       4000     //4000 输出4kHz带宽的音频；8000 输出8kHz带宽的音频
#define HPF_FILTER_CUT_OFF_FREQ    200      //fft_hpf 输入信号高通滤波器的截止频率为200Hz 


#define USE_IIS0_OUT_PRE_RSLT_AUDIO 	      1   //通话降噪的结果通过IIS0输出-采音
#define USE_IIS0_AUDIO_IN_SPEAKER_OUT         0   //IIS0采音通过SPEAKER输出,和USE_HP_OUT_PRE_RSLT_AUDIO互斥
#define USE_IIS0_AUDIO_IN_SPEAKER_OUT_MODE    0   //1-slave   0-master,语音芯片默认做主
#define USE_HP_OUT_PRE_RSLT_AUDIO             1   //通话降噪的结果通过HPOUT输出
#define SYS_BOOT_AUDIO_DATA_PREPROCESS        0   //开机音频数据预处理-通话降噪工程专用
#define SYS_BOOT_READ_EQ_DRC_FROM_NV          1   //开机自动从nv读数据，如果NV中没有存数据，则默认加载代码中固定写入的参数
#define USE_RECORD_IIS_HPOUT_FREQ_INDEX       1   //录音/iis采音/HPOUT 频率设置 0:8K 1:16K
#define USE_UART_SEND_PRE_RSLT_AUDIO          0   //是否使用UART将语音数据送出
 

/*是否使用FLASH录音播放功能,修改makefile文件。
0: CI_RTC_TYPE = USE_RTC_ENABLE_RECORD，开启录音功能并且固件加密。
1：CI_RTC_TYPE = USE_RTC_DISABLE_RECORD，关闭录音功能且固件未加密。*/
#if USE_FLASH_RECORD_PLAY_ENABLE              
#define USE_HP_OUT_PRE_RSLT_AUDIO             1   //通话降噪的结果通过HPOUT输出
#define IF_16K_DOWNSAMPLE_TO_8K               1   //音频16K降8K输出-录音使用
#define USE_UART_SEND_SPEEX_ENABLE            1   //串口发送数据经过speex压缩使能
#define USE_UART_RCV_PLAY_SPEEX_ENABLE        1   //串口接收数据经过speex解压并播放使能
#define USE_PRE_RSLT_AUDIO_SWITCH_KEY_ENABLE  0   //音频喇叭/串口两种输出方式切换按键使能
#define AUDIO_IN_BUFFER_NUM                   (4*2)
#define USE_IIS0_OUT_PRE_RSLT_AUDIO           0
#else
#define AUDIO_IN_BUFFER_NUM                   4
#define USE_IIS0_OUT_PRE_RSLT_AUDIO           1
#endif

#if USE_UART_SEND_PRE_RSLT_AUDIO || USE_PRE_RSLT_AUDIO_SWITCH_KEY_ENABLE
#define UART_PRE_RSLT_AUDIO_TX_NUMBER         (HAL_UART2_BASE)        //通过串口输出音频数据串口号
#define UART_PRE_RSLT_AUDIO_TX_BAUDRATE       (UART_BaudRate1M)       //通过串口输出音频数据波特率

#define UART_PRE_RSLT_AUDIO_RX_NUMBER         (HAL_UART1_BASE)        //通过串口输入音频数据串口号
#define UART_PRE_RSLT_AUDIO_RX_BAUDRATE       (UART_BaudRate1M)       //通过串口输入音频数据波特率
#endif

#define USE_INNER_LDO3                      1//1 内置LDO，功耗较高；0外置LDO，功耗较低

//内部codec ALC的目标值
#define ALC_MAX_LEVEL     INNER_CODEC_ALC_LEVEL__5dB
#define ALC_MIN_LEVEL     INNER_CODEC_ALC_LEVEL__9dB
//内部codec ALC的最大增益
#define ALC_PGA_MAX_GAIN    INNER_CODEC_ALC_PGA_MAX_GAIN_22_5dB
#define ALC_PGA_MIN_GAIN    INNER_CODEC_ALC_PGA_MIN_GAIN_6dB

//通话降噪hp输出音量调节
#define RTC_HPOUT_DEFAULT_VOL               80

/*通话降噪输出降噪前，还是降噪后的数据*/
#define RTC_OUT_IS_VIA_DENOISE              1// 0：降噪之前的数据，1：降噪之后的数据

#define RTC_MIC_LEFT_RIGHT_CHOOSE           0//1：可以选择声音来自左MIC，还是右MIC 0：数据只来自于左MIC
#if USE_AEC_MODULE
#define RTC_MIC_LEFT_RIGHT_CHOOSE           0
#endif

#if RTC_MIC_LEFT_RIGHT_CHOOSE
#define HOST_CODEC_CHA_NUM  2
#endif


/*上下行通道的选择，默认PA5，高电平左声道输出，低电平右声道输出*/
/*注意 RTC_MIC_LEFT_RIGHT_CHOOSE  改为1*/
#define RTC_UPLINK_SWITCH_GPIO_DEFAULT      0 //1 ENABLE，0 DISABLE
#define RTC_UPLINK_SWITCH_GPIO_BASE         PA
#define RTC_UPLINK_SWITCH_GPIO              PA5
#define RTC_UPLINK_SWITCH_GPIO_PIN          pin_5
#define RTC_UPLINK_SWITCH_GPIO_IRQ          PA_IRQn


/*降噪功能开关，默认PA6，高电平开降噪，低电平关降噪*/ 
#define RTC_DENOISE_FUNC_GPIO_DEFAULT       0 //1 ENABLE，0 DISABLE   
#define RTC_DENOISE_FUNC_GPIO_BASE          PA
#define RTC_DENOISE_FUNC_GPIO               PA6
#define RTC_DENOISE_FUNC_GPIO_PIN           pin_6
#define RTC_DENOISE_FUNC_GPIO_IRQ           PA_IRQn

/*配置用户按键功能实例*/
#define RTC_USER_KEY_FUNC_GPIO_DEFAULT      0  //1 ENABLE，0 DISABLE   
#define RTC_USER_KEY_FUNC_GPIO_BASE         PA
#define RTC_USER_KEY_FUNC_GPIO              PA7
#define RTC_USER_KEY_FUNC_GPIO_PIN          pin_7
#define RTC_USER_KEY_FUNC_GPIO_IRQ          PA_IRQn

/* 录音播放功能按键：默认PA2，长按(开始/结束 录音)、短按一次(下一个录音)、短按两次(上一个录音) */
#define RTC_FLASH_MUTY_KEY_FUNC_GPIO_DEFAULT      0  //1 ENABLE，0 DISABLE   
#define RTC_FLASH_MUTY_KEY_FUNC_GPIO_BASE         PA
#define RTC_FLASH_MUTY_KEY_FUNC_GPIO              PA6
#define RTC_FLASH_MUTY_KEY_FUNC_GPIO_PIN          pin_6
#define RTC_FLASH_MUTY_KEY_FUNC_GPIO_IRQ          PA_IRQn

/* VOX功能按键：有声音PB1输出低，没声音输出高 */
#define RTC_FLASH_VOX_FUNC_GPIO_DEFAULT      1  //1 ENABLE，0 DISABLE   
#define RTC_FLASH_VOX_FUNC_GPIO_BASE         PB
#define RTC_FLASH_VOX_FUNC_GPIO              PB1
#define RTC_FLASH_VOX_FUNC_GPIO_PIN          pin_1
#define RTC_FLASH_VOX_FUNC_GPIO_IRQ          PB_IRQn

/* 音频输出方式选择：默认PA5，高电平喇叭输出，低电平串口输出 */
#define RTC_AUDIO_OUT_SWITCH_KEY_FUNC_GPIO_DEFAULT      0  //1 ENABLE，0 DISABLE   
#define RTC_AUDIO_OUT_SWITCH_KEY_FUNC_GPIO_BASE         PA
#define RTC_AUDIO_OUT_SWITCH_KEY_FUNC_GPIO              PA3
#define RTC_AUDIO_OUT_SWITCH_KEY_FUNC_GPIO_PIN          pin_3
#define RTC_AUDIO_OUT_SWITCH_KEY_FUNC_GPIO_IRQ          PA_IRQn

/* 客户串口协议，时钟+数据引脚 */
#if MSG_COM_USE_UART_EN 
#define CUSTOMER_PROTOCOL_CLK_GPIO_DEFAULT      0  //1 ENABLE，0 DISABLE   ----客户协议，时钟+数据引脚
#else
#define CUSTOMER_PROTOCOL_CLK_GPIO_DEFAULT      1  //1 ENABLE，0 DISABLE   ----客户协议，时钟+数据引脚
#endif
#define CUSTOMER_PROTOCOL_CLK_GPIO_BASE         PA          //时钟IO引脚
#define CUSTOMER_PROTOCOL_CLK_GPIO              PA7
#define CUSTOMER_PROTOCOL_CLK_GPIO_PIN          pin_7
#define CUSTOMER_PROTOCOL_CLK_GPIO_IRQ          PA_IRQn
#define CUSTOMER_PROTOCOL_DATA_GPIO_BASE        PB          //数据IO引脚
#define CUSTOMER_PROTOCOL_DATA_GPIO             PB0
#define CUSTOMER_PROTOCOL_DATA_GPIO_PIN         pin_0
#define CUSTOMER_PROTOCOL_DATA_GPIO_IRQ         PB_IRQn

#if USE_AHS_MODULE && USE_AEC_MODULE
#error "AHS AEC can't use togher\n"
#endif
#if USE_IIS0_AUDIO_IN_SPEAKER_OUT&&USE_HP_OUT_PRE_RSLT_AUDIO
#error "iis0 out hp out can't use togher\n"
#endif
#endif /* _USER_CONFIG_H_ */
