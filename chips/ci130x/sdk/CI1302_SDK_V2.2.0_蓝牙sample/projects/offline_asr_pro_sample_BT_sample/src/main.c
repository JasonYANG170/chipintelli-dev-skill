/**
 * @file main.c
 * @brief 示例程序
 * @version 1.0.0
 * @date 2021-03-19
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */
#include <stdio.h> 
#include <malloc.h>
#include "FreeRTOS.h" 
#include "task.h"
#include "platform_config.h"
#include "sdk_default_config.h"
#include "ci130x_core_eclic.h"
#include "ci130x_spiflash.h"
#include "ci130x_gpio.h"
#include "ci130x_core_misc.h"
#include "audio_play_api.h"
#include "audio_play_decoder.h"
#include "ci_flash_data_info.h"
#include "codec_manage_inner_port.h"
#include "board.h"
#include "ci130x_uart.h"
#include "flash_manage_outside_port.h"
#include "system_msg_deal.h"
#include "ci130x_dpmu.h"
#include "ci130x_mailbox.h"
#include "ci130x_nuclear_com.h"
#include "flash_control_inner_port.h"
#include "romlib_runtime.h"
#include "audio_in_manage_inner.h"
#include "ci_log.h"
#include "status_share.h"
#include "platform_config.h"
#include "asr_api.h"
#include "alg_preprocess.h"
#include "ci130x_iwdg.h"
#include "user_config.h"
#if USE_CWSL 
#include "cwsl_manage.h"
#endif
#if USE_CHIP_E02GS01J_BODER
#if CIAS_BLE_ENABLE||CIAS_RF_24G_ENABLE
#include "exe_app.h"
#include "exe_api.h"
#include "ci_ble_rf.h"
#include "rf_msg_deal.h"
#include "ble_user_config.h"
#include "exe_app_at.h"
extern TaskHandle_t ble_task_handle;
extern TaskHandle_t rf_24g_task_handle;
#endif
#endif


#if USE_LED_TM1640_LIGHT
#include "led_TM1640_control.h"
#endif

#if USE_LED_TML1668_LIGHT
#include "led_tml1668_control.h"
#endif
#if USE_LED_HBS650_LIGHT
#include "led_hbs650_control.h"
#endif
/**
 * @brief 硬件初始化
 *          这个函数主要用于系统上电后初始化硬件寄存器到初始值，配置中断向量表初始化芯片io配置时钟
 *          配置完成后，系统时钟配置完毕，相关获取clk的函数可以正常调用
 */
static void hardware_default_init(void)
{
	/* 配置外设复位，硬件外设初始化 */
	extern void SystemInit(void);
	SystemInit();

	/* 设置中断优先级分组 */
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO0);

	/* 开启全局中断 */
	eclic_global_interrupt_enable();

	enable_mcycle_minstret();

	init_platform();
	
	#if (USE_CHIP_E02GS01J_BODER == 0)
	#if SUPPORT_LOG_PRINTF
	UARTPollingConfig((UART_TypeDef*)CONFIG_CI_LOG_UART, UART_BaudRate921600);
	#endif
	#endif
	/* 初始化maskrom lib */
	maskrom_lib_init();

	#if !(USE_INNER_LDO3)
	dpmu_ldo3_en(false);
	dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO3);
	#endif
	#if USE_INNER_LDO2
	dpmu_ldo2_en(true);
	dpmu_ldo2_lv_set(0x0E);
	dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO2);
	#endif

	//DMA通道中断开启
	scu_set_dma_mode(DMAINT_SEL_CHANNEL1);
	scu_set_device_reset(HAL_GDMA_BASE);
	scu_set_device_reset_release(HAL_GDMA_BASE);
}


/**
 * @brief 用于平台初始化相关代码
 *
 * @note 在这里初始化硬件需要注意：
 *          由于部分驱动代码中使用os相关接口，在os运行前调用这些接口会导致中断被屏蔽
 *          其中涉及的驱动包括：QSPIFLASH、DMA、I2C、SPI
 *          所以这些外设的初始化需要放置在vTaskVariablesInit进行。
 *          如一定需要（非常不建议）在os运行前初始化这些驱动，请仔细确认保证：
 *              1.CONFIG_DIRVER_BUF_USED_FREEHEAP_EN  宏配置为0
 *              2.DRIVER_OS_API                     宏配置为0
 */
static int platform_init(void)
{   


	#if CONFIG_SYSTEMVIEW_EN   
	/* 初始化SysView RTT，仅用于调试 */
	SEGGER_SYSVIEW_Conf();
	/* 使用串口方式输出sysview信息 */
	vSYSVIEWUARTInit();
	ci_logdebug(CI_LOG_DEBUG, "Segger Sysview Control Block Detection Address is 0x%x\n",&_SEGGER_RTT);
	#endif

	iwdg_init_t init;
	init.irq = iwdg_irqen_enable;
	init.res = iwdg_resen_enable;
	init.count = ((get_src_clk()/0x10)*3);/* IWDG时钟从src_clk经过16分频得到, 当前配置为2秒*/
	scu_set_device_gate(IWDG, ENABLE);
	dpmu_iwdg_reset_system_config();
	iwdg_init(IWDG,init);
	iwdg_open(IWDG);

	return 0;
}


/**
 * @brief sdk上电信息打印
 *
 */
static void welcome(void)
{
	OMPrintf(LOG_USER,"\r\n");
	OMPrintf(LOG_USER,"\r\n");
	OMPrintf(LOG_USER,"ci130x_sdk_%s_%d.%d.%d Built-in\r\n",
				SDK_TYPE,
				SDK_VERSION,SDK_SUBVERSION,SDK_REVISION);
	OMPrintf(LOG_USER,"\033[1;32mWelcome to CI130x_SDK.\033[0;39m\r\n");
	extern char heap_start;
	extern char heap_end;
	OMPrintf(LOG_USER,"Heap size:%dKB\n", (&heap_end - &heap_start)/1024);


	OMPrintf(LOG_USER,"UserVer_%s  %d.%d Built-in\r\n",USER_TYPE,
		USER_VERSION_MAIN_NO,USER_VERSION_SUB_NO);
	OMPrintf(LOG_USER,"Freq factor %d\n", (int)(get_freq_factor()*1000));
	OMPrintf(LOG_USER,"Freq %d\n", (int)(get_ipcore_clk()));
	// 实际主频检查
	if (abs(((int)get_ipcore_clk()) - ((int)MAIN_FREQUENCY)) > 10000000)
	{
		OMPrintf(LOG_USER,"PLL config err!\n");
		while(1);
	}
}


static void task_init(void *p_arg)
{
	#if USE_CHIP_E02GS01J_BODER
	ble_port_init();
	vTaskDelay(30);
	extern void board_clk_source_set(void);
	board_clk_source_set();
	#if SUPPORT_LOG_PRINTF
	UARTPollingConfig((UART_TypeDef*)CONFIG_CI_LOG_UART, UART_BaudRate921600);
	#endif
	welcome();
	#endif

    extern char SDK_PRO_SRAM_HOST_END_ADDR;
    dsu_init((uint32_t)&SDK_PRO_SRAM_HOST_END_ADDR);    

	cm_init();
	
	#if USE_INFRARED_REMOTE
	user_ir_init();                              
	#endif
	
	#if USE_LED_TM1640_LIGHT    
	TM1640_init();
	#endif
	
	#if USE_LED_TML1668_LIGHT
	tm1668_init();
	#endif
	
	#if USE_LED_HBS650_LIGHT    
	HBS650_init();
	uint8_t dData[4]={0xff,0xff,0xff,0xff};
	HBS650_ShowTest(&dData);
	#endif

	/* 注册录音codec */
	audio_in_codec_registe();

	//等待bnpu初始化完成后的消息，之后发送ack回应bnpu
	//nuclear_com_init需在mailboxboot_sync之前
	nuclear_com_init();

	/*各个通信组件的初始化*/
	decoder_port_inner_rpmsg_init();
	flash_control_inner_port_init();
	dnn_nuclear_com_outside_port_init();
	asr_top_nuclear_com_outside_port_init();
	vad_fe_nuclear_com_outside_port_init();
	flash_manage_nuclear_com_outside_port_init();
	codec_manage_inner_port_init();
	ciss_init();
    ciss_set(CI_SS_DECODER_MIN_ACTIVE,DECODER_MIN_ACTIVE);
    float beam = DECODER_BEAM;
    ciss_set(CI_SS_DECODER_BEAM,*(uint32_t*)&beam);

	mailboxboot_sync();

	//注册语音前段信号处理模块
	extern ci_ssp_config_t ci_ssp;
	extern audio_capture_t audio_capture;
	REMOTE_CALL(set_ssp_registe(&audio_capture, (ci_ssp_st*)&ci_ssp, sizeof(ci_ssp)/sizeof(ci_ssp_st)));

	REMOTE_CALL(set_freqvad_start_para_gain(VAD_SENSITIVITY));


	ci_flash_data_info_init(DEFAULT_MODEL_GROUP_ID);
		
	/*离线命令词自学习功能信号量和任务*/
 
	//audio_in_manage_inner_task需要asr创建成功之后再创建
	extern void decoder_task_init_port(void);
	decoder_task_init_port();

	#if USE_CWSL 
	cwsl_set_vad_alc_config(1);
	cwsl_init();
	#endif
    
	#if  USE_CHIP_E02GS01J_BODER
	#else
	vTaskDelay(200);
	#endif
	//这里的栈原来的128有点不够，申请到的地址破坏了voice_table的一片区域
	xTaskCreate(audio_in_manage_inner_task,"audio_in_manage_inner_task",300,NULL,4,NULL);

	#if AUDIO_PLAYER_ENABLE
	/* 播放器任务 */
	audio_play_init();
	#endif

	/* 用户任务 */
	sys_msg_task_initial();
	xTaskCreate(UserTaskManageProcess,"UserTaskManageProcess",480,NULL,4,NULL);
	xTaskCreate(UserSleepTask,"UserSleepTask",200,NULL,4,NULL);
	#if  USE_CHIP_E02GS01J_BODER
	#if CIAS_BLE_ENABLE
	#if CIAS_BLE_USE_DEFAULT_ADV_DATA
	user_dev_init(DEV_TYPE_ID, DEV_NUMBER_ID, CONFIG_TYPE);   //设置服务信息
	ble_name_init(BLE_NAME);                                     //初始蓝牙广播名称
	#else
	ble_adv_data_init(BLE_NAME);
	#endif
	dev_state_init(); //初始化设备功能状态
	#if BLE_HEART_TIMEOUT  //心跳功能
	xTaskCreate(ble_heart_task, "ble_heart_task",100,NULL,4,NULL);
	#endif

	#if CIAS_AT_ENABLE   
	xTaskCreate(product_recv_task, "product_recv_task", 480, NULL, 4, NULL);
	xTaskCreate(at_msg_task, "at_msg_task",100,NULL,3,NULL);
	#endif
	register_rf_callback(ci_rf_recv_data_handle); 
	ble_uuid_init(BLE_UUID_CIAS_SERVICE, BLE_UUID_CIAS_WRITE, BLE_UUID_CIAS_NOTIFY);
	xTaskCreate(ble_exec_loop,"ble_exec_loop",512, NULL, 5, &ble_task_handle);
	#endif //CIAS_BLE_ENABLE
	#if CIAS_RF_24G_ENABLE
	xTaskCreate(ci_24g_exec_loop, "ci_24g_exec_loop",480,NULL,5,NULL);
	#endif
	#endif

  extern void config_adpt_cnt(int enable);
    config_adpt_cnt(ADAPTIVE_CNT_ENABLE);

    extern void config_max_stop_cfd(int enable,int nocnt_max_stop_cfd,int cnt_max_stop_cfd);
    config_max_stop_cfd(MAX_STOP_CFD_ENABLE,MAX_STOP_CFD_NOCNT,MAX_STOP_CFD_CNT);

    extern void config_max_vad_end_frm(int max_vad_end_frm);
    config_max_vad_end_frm(MAX_STOP_VAD_FRM);

    extern int config_base_confidence_count(short base_confidence,unsigned char valid_count);
    config_base_confidence_count(DEFAULT_CONFIDENCE,DEFAULT_CNT);

    extern void config_recover_result(int enable,int mode,int max_frm);
    config_recover_result(RECOVER_RESULT_ENABLE,RECOVER_RESULT_MODE,RECOVER_RESULT_MAX_FRM);
    
    extern void config_silprob_cnt(float base_silprob,int base_silcnt );
    config_silprob_cnt(DEFAULT_STOP_SILPROB,DEFAULT_STOP_SILCNT);
	#if 0
	while(1) 
	{
		UBaseType_t ArraySize = 10;
		TaskStatus_t *StatusArray = NULL;
		ArraySize = uxTaskGetNumberOfTasks();
		StatusArray = pvPortMalloc(ArraySize*sizeof(TaskStatus_t));
		if (StatusArray && ArraySize)
		{
			uint32_t ulTotalRunTime;
			#if 0
			volatile UBaseType_t ArraySize2 = uxTaskGetSystemState(StatusArray, ArraySize, &ulTotalRunTime);
			APPprintf("TaskName\t\tPriority\tTaskNumber\tMinStk\t%d\n", ArraySize2);
			for (int i = 0;i < ArraySize2;i++)
			{
				APPprintf("% -16s\t%d\t\t%d\t\t%d\r\n",
					StatusArray[i].pcTaskName,
					(int)StatusArray[i].uxCurrentPriority,
					(int)StatusArray[i].xTaskNumber,
					(int)StatusArray[i].usStackHighWaterMark
				);
			}
			APPprintf("\n");
			#endif
			
		}
		vPortFree(StatusArray);
		extern int get_heap_bytes_remaining_size(void);
		APPprintf("Heap left: %dKB\n", get_heap_bytes_remaining_size()/1024);
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
	#else
	vTaskDelay(pdMS_TO_TICKS(10000));
	vTaskDelete(NULL);
	#endif
}

/**
 * @brief 
 * 
 */
int main(void)
{
	hardware_default_init();

	/*平台相关初始化*/
	platform_init();

	/* 版本信息 */
	#if (USE_CHIP_E02GS01J_BODER == 0)
	welcome();
	#endif
	/* 创建启动任务 */
	xTaskCreate(task_init,"init task",280,NULL,4,NULL);

	/* 启动调度，开始执行任务 */
	vTaskStartScheduler();

	while(1){}
}


