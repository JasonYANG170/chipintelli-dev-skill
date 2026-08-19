/**/
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define CI_CHIP_TYPE 1302

#define USE_TRUE_CHIP				0
#define USE_CHIP_D02GS01J_BODER		1

#define USE_CHIP_D02GS02S_BODER		0

#define USE_CHIP_D06SS02J_BODER		0

#define USE_CHIP_D12GS01J_BODER		0
#define USE_CC_D02SS02S_BODER		0
#define USE_CC_D02SS02J_BODER		0

#define USE_CC_D02GS02J_BODER		0

#define USE_CHIP_E02GS01J_BODER		0
#if USE_CHIP_E02GS01J_BODER
#define SUPPORT_BLE_IR				0
#endif
#define USE_V7  0
#define USE_V5  0
#if (USE_V7 + USE_V5 + USE_CC_D02SS02S_BODER+ USE_TRUE_CHIP+ USE_CHIP_D02GS01J_BODER+ USE_CHIP_D02GS02S_BODER  + USE_CC_D02GS02J_BODER  + USE_CC_D02SS02J_BODER+ USE_CHIP_D06SS02J_BODER > 1)
    #error "NO!"
#endif

#define AUDIO_PLAYER_ENABLE				1	//用于屏蔽播放器任务相关代码      0：屏蔽，1：开启

#if AUDIO_PLAYER_ENABLE
#define USE_PROMPT_DECODER				1	//播放器是否支持prompt解码器
#define USE_MP3_DECODER					1	//为1时加入mp3解码器
#define AUDIO_PLAY_SUPPT_MP3_PROMPT		1	//播放器默认开启mp3播报音
#define USE_MS_WAV_DECODER				0	//播放器是否支持ms wav解码器

#define AUDIO_PLAY_BLOCK_CONT			4	//播放器底层缓冲区个数
#endif

#define USE_ALC_AUTO_SWITCH_MODULE			1	//使用动态alc模块:1开启，0关闭
#define USE_DENOISE_MODULE					0	//使用降噪模块:1开启，0关闭
#define USE_DOA_MODULE						0	//使用声源定位模块：1开启，0关闭
#define USE_DEREVERB_MODULE					0	//使用降混响模块：1开启，0关闭
#define USE_BEAMFORMING_MODULE				0	//使用双麦语音增强模块:1开启，0关闭
#define USE_AEC_MODULE						1	//使用回声消除模块:1开启，0关闭

#if USE_AEC_MODULE
#define PAUSE_VOICE_IN_WITH_PLAYING				0	//开启aec时关闭
#define IF_JUST_CLOSE_HPOUT_WHILE_NO_PLAY		1
#define AEC_INPUT_MODE_SINGGLE_ENDED			1	
#endif

#define CIAS_BLE_ENABLE						1	//ble 功能使能
#define CIAS_PROTOCOL_VER					1	//和小程序通信协议版本：1-V1.0  2-V1.1
#define CIAS_BLE_USE_DEFAULT_ADV_DATA		0
#define CIAS_RF_24G_ENABLE					0	//2.4g 功能使能，和ble功能是互斥的，只能同时开启一个

#define CIAS_BLE_DEBUG_ENABLE				0	//ble 测试模式-客户一般用不上
#if (CIAS_BLE_DEBUG_ENABLE == 1)				//开启AT指令
#define CIAS_AT_ENABLE						1
#endif 

#define BLE_HEART_TIMEOUT					0	//ble事件事件管理参数，表示ble在连接状态下，多久没有收到消息判断为连接断开，单位为s，0表示不判断
#define DEV_DRIVER_EN_ID					DEV_LIGHT_CONTROL_MAIN_ID
#define BLE_NAME							"语音茶吧机"


#if USE_BEAMFORMING_MODULE || USE_AEC_MODULE || USE_DOA_MODULE ||USE_DEREVERB_MODULE
#define HOST_CODEC_CHA_NUM  2
#endif

#define USE_CWSL								0	
#define CICWSL_TOTAL_TEMPLATE					12	//可存储模板数量

#define CONFIG_SYSTEMVIEW_EN					0	//不使能systemview

#define SAME_PLAY_CONTENT_NOT_INTERRUPT			1
#define SUPPORT_SYSTEM_PRINTF_FLAG				0	

#define EXIT_WAKEUP_TIME						15	//default exit wakeup time,unit ms

#define SUPPORT_USER_AUTO_TEST			0
#if  USE_CHIP_D12GS01J_BODER
#define   SUPPORT_BLE_APP				0
#define USER_SERIAL_IO_UART0			0
#define USER_SERIAL_IO_UART1			1
#define CONFIG_CI_LOG_UART				HAL_UART0_BASE
#define USE_EXTERNAL_CRYSTAL_OSC		0	
#else
#define USE_EXTERNAL_CRYSTAL_OSC		1
#define USER_SERIAL_IO_UART0			0
#define USER_SERIAL_IO_UART1			0
#define USER_SERIAL_IO_UART2			0
#define SUPPORT_BLUETOOTH_FUNCTION		1

#if SUPPORT_BLUETOOTH_FUNCTION
#define   SUPPORT_BLE_APP				0
#define   USER_BLUETOOTH_UART0			0
#define   USER_BLUETOOTH_UART1			1
#define   USER_BLUETOOTH_UART2			0
#define CONFIG_CI_LOG_UART				HAL_UART0_BASE
#else
#if USE_CHIP_E02GS01J_BODER
#define   SUPPORT_BLE_APP				1
#else
#define   SUPPORT_BLE_APP				0
#endif
#define   USER_BLUETOOTH_UART0			0
#define   USER_BLUETOOTH_UART1			0
#define   USER_BLUETOOTH_UART2			0
#define CONFIG_CI_LOG_UART				HAL_UART0_BASE
#endif
#endif
#if  !USE_EXTERNAL_CRYSTAL_OSC
#define UART_BAUDRATE_CALIBRATE			1	
#define BAUDRATE_SYNC_PERIOD			300000	// 波特率同步周期，单位毫秒
#define BAUDRATE_FAST_SYNC_PERIOD		5000	// 一次校准失败后，启动下一次同步周期，单位毫秒
#define BAUD_CALIBRATE_MAX_WAIT_TIME	70		// 等待反馈包的超时时间，单位毫秒
#endif

#define PLAY_WELCOME_EN						1	//欢迎词主动播报          =1是 =0否
#define PLAY_ENTER_WAKEUP_EN				1	// 唤醒词主动播报          =1是 =0否
#define PLAY_EXIT_WAKEUP_EN					1	//退出唤醒主动播报        =1是 =0否
#define PLAY_CONTROL_EN						0	//调整音量主动播报        =1是 =0否
#define PLAY_VOLUME_ADJUST_EN				1	//调整音量主动播报        =1是 =0否
#define USER_ASR_CMD_PLAY					1	//命令词主动播报        =1是 =0否
	
	
#define USER_VERSION_MAIN_NO				1
#define USER_VERSION_SUB_NO					0
#define USER_TYPE							"启英-蓝牙-空调-小艾小艾-T1"


#endif /* _USER_CONFIG_H_ */ 
