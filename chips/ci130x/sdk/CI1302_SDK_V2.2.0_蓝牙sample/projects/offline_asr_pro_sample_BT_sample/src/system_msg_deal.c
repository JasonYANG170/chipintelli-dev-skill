/**
 * @file system_msg_deal.c
 * @brief  系统消息处理任务
 * @version V1.0.0
 * @date 2019.01.22
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "system_msg_deal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ci_log.h"
#include "audio_play_api.h"
#include "ci130x_uart.h"
#include "sdk_default_config.h"
#include "ci130x_iisdma.h"
#include "ci130x_iis.h"
#include "ci130x_lowpower.h"
#include "ci130x_core_misc.h"
#include "prompt_player.h"
#include "product_semantic.h"
#include "ci_nvdata_manage.h"
#include "asr_api.h"
#include "status_share.h"
#include "customer_control.h"
#if  SUPPORT_BLUETOOTH_FUNCTION
#include "bluetooth_play_control.h"
#endif
#if SUPPORT_BLE_APP
#include "app_control.h"
#endif
#include "user_data_save.h"
#if USE_CWSL
#include "cwsl_manage.h"
#include "cwsl_app_sample.h"
#endif
/* 系统消息处理任务状态 */
typedef enum
{
	USERSTATE_WAIT_MSG = 0,    /* 等待消息 */
	SYS_STATE_WAKUP_TIMEOUT,   /* 消息超时 */
}user_task_state_t;

/* 系统状态结构 */
struct sys_manage_type
{
	uint8_t user_msg_state;            /* 系统消息处理任务状态                    */
	sys_wakeup_state_t wakeup_state;   /* 系统状态         0:非唤醒状态 1:唤醒状态 */
	sys_asr_state_t asr_state;         /* asr状态          0:空闲       1:忙碌   */
}sys_manage_data;
extern int g_auto_test_flag ;
extern int32_t get_system_reset_state(void);

/* 系统消息队列 */
static QueueHandle_t sys_msg_queue = NULL;
static QueueHandle_t Sleep_msg_queue = NULL;



/**
 * @brief 命令词识别结果播报完成回调函数
 * 
 * @param cmd_handle 命令信息句柄
 */
void play_done_callback(cmd_handle_t cmd_handle)
{
	sys_msg_t send_msg;
	send_msg.msg_type = SYS_MSG_TYPE_PLAY_OVER;
	send_msg.msg_data.asr_data.asr_cmd_handle = cmd_handle;
	send_msg_to_sys_task(&send_msg, NULL);
}

/**
 * @brief Get the wakeup state object
 * 
 * @return sys_wakeup_state_t 
 */
sys_wakeup_state_t get_wakeup_state(void)
{
	return sys_manage_data.wakeup_state;
}


/**
 * @brief Get the asr state
 * 
 * @return sys_asr_state_t asr状态
 */
sys_asr_state_t get_asr_state(void)
{
	return sys_manage_data.asr_state;
}


/**
 * @brief 设置状态为唤醒
 * 
 * @param exit_wakup_ms 下次退出唤醒时间，单位ms
 */
void set_state_enter_wakeup(void)
{
	sys_manage_data.wakeup_state = SYS_STATE_ENTERNWAKEUP;/*update wakeup state*/
	ciss_set(CI_SS_WAKING_UP_STATE,CI_SS_WAKEUPED);
	ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP,CI_SS_WAKEUPED);
}

void set_state_wakeup(void)
{
	sys_manage_data.wakeup_state = SYS_STATE_WAKEUP;/*update wakeup state*/
}

/**
 * @brief 设置状态为退出唤醒
 * 
 */
void set_state_exit_wakeup(void)
{
	Reset_PlayVoice_EndTime();        
	sys_manage_data.wakeup_state = SYS_STATE_UNWAKEUP;
	ciss_set(CI_SS_WAKING_UP_STATE,CI_SS_NO_WAKEUP);
	ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP,CI_SS_NO_WAKEUP);
	ciss_set(CI_SS_CMD_STATE,CI_SS_CMD_IS_NULL);
	ciss_set(CI_SS_CMD_STATE_FOR_SSP,CI_SS_CMD_IS_NULL);
}

/**
 * @brief 切换唤醒模型，这个函数是sys msg任务调用，其他任务需要切换模型需要发送切换模型消息
 *          通过sys msg任务调用
 */
void change_asr_wakeup_word(void)
{
	#if ADAPTIVE_THRESHOLD
	dynmic_confidence_en_cfg(0);
	#endif
	#if USE_SEPARATE_WAKEUP_EN  
	#if SUPPORT_USER_AUTO_TEST
	g_auto_test_flag = 1 ;
	cmd_info_change_cur_model_group(1);
	#else
	cmd_info_change_cur_model_group(1);
	#endif
	#endif
	#if (ASR_SKIP_FRAME_CONFIG == 1)
	{
		asr_dynamic_skip_close();
	}
	#endif
}


/**
 * @brief 切换正常模型，这个函数是sys msg任务调用，其他任务需要切换模型需要发送切换模型消息
 *          通过sys msg任务调用
 *
 */
void change_asr_normal_word(void)
{
	#if (ASR_SKIP_FRAME_CONFIG == 1)
	asr_dynamic_skip_open();
	#endif

	#if USE_SEPARATE_WAKEUP_EN
	cmd_info_change_cur_model_group(0);
	#endif

	#if ADAPTIVE_THRESHOLD
	dynmic_confidence_en_cfg(1);
	dynmic_confidence_config(-20, 10, 1);
	#endif
}


/**
 * @brief when exit wakup state, need deal some common thing, such as
 *        power mode set, change asr word, wakup state flag set, play wakeup prompt.
 *        asr_busy_check used for immediately exit or wait for current ASR deal done.
 *
 * @param asr_busy_check : 0:no need check asr busy, 1:need check asr busy
 */
void exit_wakeup_deal(uint32_t asr_busy_check)
{

	/*already in unwakeup state, so do nothing*/
	if(SYS_STATE_UNWAKEUP == get_wakeup_state())
	{
		return;
	}

	/*now asr busy,so need wait not busy, then call this function again*/
	if((1 == asr_busy_check)&&(SYS_STATE_ASR_BUSY == get_asr_state()))
	{
		sys_manage_data.user_msg_state = SYS_STATE_WAKUP_TIMEOUT;
		return;
	}
	if(user_get_voice_onoff())
	{
		if(SYS_STATE_ENTERNWAKEUP == get_wakeup_state())
		{
			#if PLAY_EXIT_WAKEUP_EN    
			#if  SUPPORT_BLUETOOTH_FUNCTION
			if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_PLAY)
			{
				bluetooth_wakeup_send_cmd(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
			}
			#endif  
			prompt_play_by_cmd_string("<inactivate>", -1, false);
			#else
			cmd_handle_t cmd_handle = cmd_info_find_command_by_string("<inactivate>");
			uint32_t semantic = cmd_info_get_semantic_id(cmd_handle);
			uart_send_customer_cmd(semantic);	
			set_state_exit_wakeup();
			change_asr_wakeup_word();
			#endif			
		}
	}
	else
	{
		#if USE_CWSL
		cwsl_app_exit_reg();
		#endif
		set_state_exit_wakeup();
		change_asr_wakeup_word();
		#if  SUPPORT_BLUETOOTH_FUNCTION
		if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_VOICE_STOP)
		{
			bluetooth_send_cmd(BLUETTOTH_PLAY_MUSIC_SEMANTIC_ID);
		}
		#endif
		/*set wakeup state*/

	}


	/*module send msg exit wakeup*/
}


/**
 * @brief 系统消息任务资源初始化
 *
 */
void sys_msg_task_initial(void)
{
	sys_msg_queue = xQueueCreate(16, sizeof(sys_msg_t));
	if(!sys_msg_queue)
	{
		mprintf("not enough memory:%d,%s\n",__LINE__,__FUNCTION__);
	}
	Sleep_msg_queue = xQueueCreate(2, sizeof(int));
	if(Sleep_msg_queue==0)
	{
		mprintf("Sleep_msg_queue creat fail\r\n");    
	}
}


/**
 * @brief A simple code for other component send system message to this module, just wrap freertos queue function
 *
 * @param flag_from_isr : 0 call this function not from isr, other call this from isr
 * @param send_msg : system message
 * @param xHigherPriorityTaskWoken : if call this not from isr set NULL
 * @return BaseType_t
 */
BaseType_t send_msg_to_sys_task(sys_msg_t *send_msg,BaseType_t *xHigherPriorityTaskWoken)
{
	if(sys_msg_queue)
	{
		if(0 != check_curr_trap())
		{
			return xQueueSendFromISR(sys_msg_queue,send_msg,xHigherPriorityTaskWoken);
		}
		else
		{
			return xQueueSend(sys_msg_queue,send_msg,0);
		}
	}
	return  0;
}
void chip_start_machine_play(void)
{
	#if PLAY_WELCOME_EN    
	prompt_play_by_cmd_string("<welcome>", -1, false);
	#else	
	cmd_handle_t cmd_handle = cmd_info_find_command_by_string("<welcome>");
	uint32_t semantic = cmd_info_get_semantic_id(cmd_handle);
	uart_send_customer_cmd(semantic);
	set_state_exit_wakeup();
	change_asr_wakeup_word();
	#endif
}

/**
 * @brief system message deal function and user main UI flow. system message include ASR, player, KEY, COM
 * 
 * @param p_arg 
 */
void UserTaskManageProcess(void *p_arg)
{
	sys_msg_t rev_msg;
	BaseType_t err = pdPASS;

	ciss_set(CI_SS_WAKING_UP_STATE,CI_SS_NO_WAKEUP);
	ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP,CI_SS_NO_WAKEUP);
	sys_manage_data.user_msg_state = USERSTATE_WAIT_MSG;
	sys_manage_data.wakeup_state = SYS_STATE_UNWAKEUP;
	prompt_Regist_PlayFinishCallback(play_done_callback);
	while(1)
	{
		err = xQueueReceive(sys_msg_queue, &rev_msg, portMAX_DELAY);

		if(pdPASS == err)
		{
			switch (rev_msg.msg_type)
			{
				case SYS_MSG_TYPE_ASR:
				{
					sys_msg_asr_data_t *asr_rev_data;
					asr_rev_data = &(rev_msg.msg_data.asr_data);
					custormer_deal_asr_msg(asr_rev_data);
					break;
				}
				case SYS_MSG_TYPE_PLAY_OVER:
				{
					if(prompt_get_next_play_flag())
					{
						if((SYS_STATE_WAKEUP == get_wakeup_state())) /*wakeup word*/
						{
							set_state_enter_wakeup();
							change_asr_normal_word();
						}
						prompt_interrupte_play();
					}
					else 
					{
						resume_voice_in();
						sys_msg_asr_data_t *asr_rev_data;
						asr_rev_data = &(rev_msg.msg_data.asr_data);
						if(asr_rev_data->asr_cmd_handle)
						system_check_playid_status(asr_rev_data->asr_cmd_handle);
					}
					break;
				}
				case SYS_MSG_TYPE_EXIT_WAKEUP:
				{
					exit_wakeup_deal(1);
					break;
				}
				case SYS_MSG_TYPE_COM:
				{
					sys_msg_com_data_t *com_rev_data;
					com_rev_data = &rev_msg.msg_data.com_data;
					custormer_com_deal_msg(com_rev_data);
					break;
				}  
				#if  SUPPORT_BLUETOOTH_FUNCTION
				case SYS_MSG_TYPE_BLUETOOTH:
				{
					sys_bluetooth_data_t *bluetooth_rev_data;
					bluetooth_rev_data = &rev_msg.msg_data.bluetooth_data;
					bluetooth_deal_msg(bluetooth_rev_data);
					break;
				}
				#endif
				#if  SUPPORT_BLE_APP
				case SYS_MSG_TYPE_APP:
				{
					sys_app_data_t *app_rev_data;
					app_rev_data = &rev_msg.msg_data.app_data;
					app_deal_msg(app_rev_data);
					break;
				}
				#endif
				#if  UART_BAUDRATE_CALIBRATE
				case SYS_MSG_TYPE_COM_HAND:
				{
					send_baudrate_sync_req();
					break;
				}  
				#endif
				case SYS_MSG_TYPE_AUDIO_IN_STARTED:
				{
					User_Save_Data_Init();
					Init_Customer_Paramter_If();
					#if  SUPPORT_BLUETOOTH_FUNCTION
					Init_bluetooth_Paramter_If();
					#endif
					#if SUPPORT_USER_AUTO_TEST
					g_auto_test_flag = 1 ;
					change_asr_wakeup_word();					
					set_state_enter_wakeup();
					change_asr_normal_word();
					#else
					if (RETURN_OK == scu_get_system_reset_state())
					{
						change_asr_normal_word();
						set_state_exit_wakeup();
						change_asr_wakeup_word();				
						chip_start_machine_play();
					}
					else
					{
						change_asr_wakeup_word();
						set_state_exit_wakeup();
					}
					#endif	
					#if  UART_BAUDRATE_CALIBRATE
					Init_baudrate_sync();
					#endif				
					break;
				}
				default:
					break;
			}
			switch(sys_manage_data.user_msg_state)
			{
				case SYS_STATE_WAKUP_TIMEOUT:
				{
					if(SYS_STATE_ASR_IDLE == get_asr_state())
					{
						exit_wakeup_deal(0);
						sys_manage_data.user_msg_state = USERSTATE_WAIT_MSG;
					}
					break;
				}
				default:
					break;
			}
		}
		else
		{
			// TODO:
			/*timeout for feed watchdog*/
		}
	}
}




static TickType_t g_end_play_voice_time = 0;
static int  g_Sleep_Flag = 0;
static int g_wakeuptime_interval = EXIT_WAKEUP_TIME;
void set_wakeup_time(int   dWktime)
{
	g_wakeuptime_interval = dWktime;
}
int get_wakeup_time(void)
{
	return g_wakeuptime_interval;
}

TickType_t get_PlayVoice_EndTime(void)
{
	return g_end_play_voice_time;
}
int  get_Sleep_Flag(void)
{
	return g_Sleep_Flag;
}


void Reset_PlayVoice_EndTime(void)
{
	g_Sleep_Flag = 0;
	g_end_play_voice_time = 0;
}
void set_PlayVoice_time(void)
{
	g_end_play_voice_time = xTaskGetTickCount();
	g_Sleep_Flag   = 1;
}

void Check_Sleep_time(void)
{
	TickType_t dLasttime=get_PlayVoice_EndTime();
	TickType_t dCurrentTime = xTaskGetTickCount();
	if((SYS_STATE_ENTERNWAKEUP == get_wakeup_state())&&(get_Sleep_Flag()==1))
	{
		if((dCurrentTime - dLasttime)>pdMS_TO_TICKS(g_wakeuptime_interval*1000))
		{
			sys_msg_t send_msg;
			set_PlayVoice_time();
			send_msg.msg_type = SYS_MSG_TYPE_EXIT_WAKEUP;
			send_msg_to_sys_task(&send_msg, NULL);
		}
	}
}


void UserSleepTask(void *pvParameters)
{
	//unsigned int times = 0;
	int index;
	while(1)
	{
		if(pdPASS== xQueueReceive(Sleep_msg_queue, &index, pdMS_TO_TICKS(1000)))
		{
			//mprintf("%#x:RTOS\n",times++);
			#if  SUPPORT_BLUETOOTH_FUNCTION
			if((g_auto_test_flag ==0)&&(get_bluetooth_phone_state()==false))
				Check_Sleep_time();
			set_aec_state_play_music();	
			#else
			if(g_auto_test_flag ==0)
				Check_Sleep_time();	
			#endif
		}
		else
		{
			//mprintf("%#x:RTOS\n",times++);
			#if  SUPPORT_BLUETOOTH_FUNCTION
			if((g_auto_test_flag ==0)&&(get_bluetooth_phone_state()==false))
				Check_Sleep_time();
			set_aec_state_play_music();	
			#else
			if(g_auto_test_flag ==0)
			Check_Sleep_time();	
			#endif
		}
	}
}
