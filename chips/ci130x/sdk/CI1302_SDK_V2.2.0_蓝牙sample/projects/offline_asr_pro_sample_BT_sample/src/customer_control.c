/******************************************************************************
* @file    customer asr.c 
* @company  Chipintelli Technology Co., Ltd.
* @author  chenronglin.
* @version V1.0.0
* @date    2018.03.03
* @function: use customer command
******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "system_msg_deal.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "com_task.h"
#include "ci130x_it.h"
#include "ci130x_uart.h"
#include "ci_log.h"
#include "ci130x_iisdma.h"
#include "ci130x_iis.h"
#include "ci130x_lowpower.h"
#include "ci130x_core_misc.h"
#include "ci_assert.h"
#include "prompt_player.h"
#include "ci_nvdata_manage.h"

#include "bluetooth_play_control.h"
#include "customer_control.h"
#include "user_data_save.h"
#include "board.h"
#include "status_share.h"
#if  UART_BAUDRATE_CALIBRATE
#include "baudrate_calibrate.h"
#endif
#define CUSTOMR_SEND_DATA_LEN    6

static pUartSend g_UartSendIdle = NULL;

static sys_msg_t g_send_msg;
static int change_wake_time_flag = 0;
static int g_wakeup_flag = 0;
int g_auto_test_flag = 0;
unsigned char g_checksum =0;
TickType_t comlast_time =0;
int Emergency_call_help_time_flag = 0;
TickType_t  Emergency_call_help_time =0;
extern char g_voice_pause_music_flag;
extern user_data_stru user_data;
TickType_t dLasttsendime = 0;

/******************************************************************************
  * @void customer_rx_idle(unsigned char rxdata)
  * @parameter:  serial data 
  * @return : none
  * @function : it's interface for receive customer data
*******************************************************************************/
static void  customer_Uart1_idle(unsigned char rxdata)
{
	if((rxdata==CUSTOMR_UART_HEADER0)&&(crx_state !=HEADER_STATE1))
	{
		if(( xTaskGetTickCount() -comlast_time)>pdMS_TO_TICKS(300))
		{
			crx_state = HEADER_STATE1;
		}
	}
	switch(crx_state)
	{
		case HEADER_STATE1:
		{
			if(rxdata==CUSTOMR_UART_HEADER0)
			{
				g_send_msg.msg_data.com_data.header0 = rxdata;
				comlast_time =  xTaskGetTickCount();
				crx_state = HEADER_STATE2;
				g_checksum = rxdata;
			}
			break;
		}
		case HEADER_STATE2:
		{
			if(rxdata==CUSTOMR_UART_HEADER1)
			{
				g_send_msg.msg_data.com_data.header1 = rxdata;
				crx_state = DATA_STATE1;
				g_checksum += rxdata;
			}
			else
			{
				crx_state = HEADER_STATE1;
			}
			break;
		}
		case DATA_STATE1:
		{
			g_send_msg.msg_data.com_data.id = rxdata;
			crx_state = DATA_STATE2;
			g_checksum += rxdata;
			break;
		}
		case DATA_STATE2:
		{
			g_send_msg.msg_data.com_data.cmd = rxdata;
			crx_state = DATA_STATE3;
			g_checksum += rxdata;
			break;
		}
		case DATA_STATE3:
		{
			g_send_msg.msg_data.com_data.data0 = rxdata;
			crx_state = DATA_STATE4;
			g_checksum += rxdata;
			break;
		}
		case DATA_STATE4:
		{
			g_send_msg.msg_data.com_data.data1 = rxdata;
			crx_state = CHECKSUM_STATE1;
			g_checksum += rxdata;
			break;
		}
		case CHECKSUM_STATE1:
		{			
			g_send_msg.msg_data.com_data.chksum = rxdata;
			crx_state = END_STATE2;
			break;
		}		
		case END_STATE2:
		{
			if(rxdata==CUSTOMR_UART_END)
			{
				g_send_msg.msg_data.com_data.end = rxdata;
				crx_state = HEADER_STATE1;
				#if UART_BAUDRATE_CALIBRATE
				if(g_send_msg.msg_data.com_data.data0 == 0xF8)
				{
					baudrate_calibrate_set_ack();
					return;
				}
				#endif
				g_send_msg.msg_type = SYS_MSG_TYPE_COM; 
				send_msg_to_sys_task(&g_send_msg, NULL);
			}
			break;
		}
		default:
			break;
	}
}

/******************************************************************************
  * @bool Chip_Init_Uart_CallBack(unsigned int(void)
  * @parameter:  void
  * @return : none
  * @function : 串口注册函数
*******************************************************************************/
static void Chip_Init_Uart_CallBack(void)
{
	#if USER_SERIAL_IO_UART0
	Chip_Regist_Uart0CallBack((pUartCallBack)customer_Uart1_idle);
	g_UartSendIdle = (pUartSend)Chip_Uart0_Send;
	#endif
	#if USER_SERIAL_IO_UART1
	Chip_Regist_Uart1CallBack((pUartCallBack)customer_Uart1_idle);
	g_UartSendIdle = (pUartSend)Chip_Uart1_Send;    
	#endif
	#if USER_SERIAL_IO_UART2
	Chip_Regist_Uart2CallBack((pUartCallBack)customer_Uart1_idle);
	g_UartSendIdle = (pUartSend)Chip_Uart2_Send;    
	#endif

}
int deal_volume_control(cmd_handle_t cmd_handle)
{
	uint8_t  ret = 0;
	int select_index = -1;

	uint32_t semantic = cmd_info_get_semantic_id(cmd_handle);       
	uint8_t  volset = user_get_vol();
	if(semantic == VOLUME_UP_COMMAND_INDEX_1)
	{
		user_set_vol((volset >= VOLUME_MAX) ? volset : (volset + 1));
		ret = (volset == VOLUME_MAX) ? 1:0;
	}
	else if(semantic == VOLUME_DOWN_COMMAND_INDEX_1)
	{
		user_set_vol((volset == VOLUME_MIN) ? volset : (volset - 1));
		ret = (volset == VOLUME_MIN) ? 2:0;
	}
	else if(semantic==VOLUME_MAX_COMMAND_INDEX_1)
	{
		user_set_vol(VOLUME_MAX);
	}
	else if(semantic==VOLUME_MIN_COMMAND_INDEX_1)
	{
		user_set_vol(VOLUME_MIN);
	}
	else 
	{
		return -1;
	}

	if(ret == 1)//max
	{
		select_index = 1;
	}
	else if(ret == 2)//min
	{
		select_index = 1;
	}
	else
	{
		select_index = -1;
	}

	if(user_get_voice_onoff())
	{
		if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_PLAY)
		{
			bluetooth_wakeup_send_cmd(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
		}
		prompt_play_by_cmd_handle(cmd_handle, select_index, false);   
	}
	vTaskDelay(100);
	uint32_t blevolset = BLUETTOTH_VOL_ADJUST+2*user_get_vol()-2;
	bluetooth_send_cmd(blevolset);
	return 0;
}
/******************************************************************************
  * @bool uart_send_customer_cmd(unsigned int index)
  * @parameter:  comdata 串口数据
  * @return : none
  * @function : 用于发送串口数据
*******************************************************************************/
void uart_send_customer_cmd(unsigned int index)
{
	TickType_t dcurtime=xTaskGetTickCount();
	if(dcurtime <(dLasttsendime+200))
		vTaskDelay(200);
	sys_msg_com_data_t com_data;
	com_data.header0 = CUSTOMR_UART_HEADER0;	
	com_data.header1 = CUSTOMR_UART_HEADER1;	
	com_data.id = CUSTOMR_UART_HEADER2;
	com_data.cmd= CI_TX_ASR;
	com_data.data0=index;  
	com_data.data1 = 0;
	if(g_UartSendIdle)
		g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);
	dLasttsendime = xTaskGetTickCount();
	set_PlayVoice_time();
}   
void custormer_deal_asr_msg(sys_msg_asr_data_t *asr_msg)
{
	cmd_handle_t cmd_handle;
	cmd_handle = asr_msg->asr_cmd_handle;
	uint16_t score = cmd_info_get_cmd_score(cmd_handle);
	if(asr_msg->asr_score<score)
	{
		return;
	}
	if(g_auto_test_flag ==0)
	{
		cmd_handle = asr_msg->asr_cmd_handle;
		uint16_t cmd_id = cmd_info_get_command_id(cmd_handle);
		uint32_t semantic = cmd_info_get_semantic_id(cmd_handle);		
		if(SYS_STATE_ENTERNWAKEUP != get_wakeup_state()&&cmd_info_is_wakeup_word(cmd_handle)) /*wakeup word*/
		{
			ciss_set(CI_SS_CMD_STATE,CI_SS_CMD_IS_WAKEUP);
			ciss_set(CI_SS_CMD_STATE_FOR_SSP,CI_SS_CMD_IS_WAKEUP);
			ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_IDLE);
			set_state_wakeup();
			#if PLAY_ENTER_WAKEUP_EN       
			if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_PLAY)
			{
				bluetooth_wakeup_send_cmd(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
			}
			stop_play_music_timer();
			prompt_play_by_cmd_handle(cmd_handle, -1, false);   
			#endif    
			uart_send_customer_cmd(semantic);
			set_state_enter_wakeup();
			change_asr_normal_word();
		}
		else if (SYS_STATE_ENTERNWAKEUP == get_wakeup_state())
		{
			ciss_set(CI_SS_CMD_STATE,CI_SS_CMD_IS_NORMAL);
			ciss_set(CI_SS_CMD_STATE_FOR_SSP,CI_SS_CMD_IS_NORMAL);
			ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_IDLE);
			if(cmd_info_is_wakeup_word(cmd_handle)) /*wakeup word*/
			{
				#if PLAY_ENTER_WAKEUP_EN      
				if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_PLAY)
				{
					bluetooth_wakeup_send_cmd(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
				}
				prompt_play_by_cmd_handle(cmd_handle, -1, false);
				#endif				
				uart_send_customer_cmd(semantic);
			}
			else if((semantic >=VOLUME_UP_COMMAND_INDEX_1)&&(semantic <=VOLUME_MIN_COMMAND_INDEX_1))
			{
				#if PLAY_VOLUME_ADJUST_EN       
				deal_volume_control(cmd_handle);
				#else
				uart_send_customer_cmd(semantic);
				#endif
			}
			else if(semantic == SUPPORT_CHECK_VERSION_ID)//??£¤¨¨¡¥¡é??o????¡ë????
			{				
				prompt_play_by_cmd_handle(cmd_handle, -1, false);
			}
			else   if((semantic >=BLUETTOTH_PLAY_MUSIC_SEMANTIC_ID)&&(semantic <BLUETOOTH_SEMANTIC_ID_END))
			{
				if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_PLAY)
				{
					bluetooth_wakeup_send_cmd(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
				}
				if((semantic ==BLUETTOTH_OPEN_SEMANTIC_ID1)||(semantic ==BLUETTOTH_OPEN_SEMANTIC_ID2)||(semantic ==BLUETTOTH_CLOSE_SEMANTIC_ID))
				{
					prompt_play_by_cmd_handle(cmd_handle, -1, false);
					return;
				}
				if(!get_bluetooth_connect_state())
				{
					prompt_play_by_semantic_id(0x10000003, -1, false);
				}
				else
				{
					if(semantic ==BLUETTOTH_CALL_EMERGENCY_NUMBER_SEMANTIC_ID)
					{
						if(Emergency_call_help_time_flag == 0)
						{
							g_voice_pause_music_flag = 0;
							Emergency_call_help_time= xTaskGetTickCount();
							prompt_play_by_cmd_handle(cmd_handle, -1, false);
							Emergency_call_help_time_flag = 1;
						}
						else if((Emergency_call_help_time_flag == 1)&&(xTaskGetTickCount() - Emergency_call_help_time)<(pdMS_TO_TICKS(10*1000)))
						{
							bluetooth_send_cmd(semantic);
							prompt_play_by_cmd_handle(cmd_handle, 1, false);
							Emergency_call_help_time = 0;
							Emergency_call_help_time_flag = 2;
						}
						else
						{
							g_voice_pause_music_flag = 0;
							Emergency_call_help_time= xTaskGetTickCount();
							prompt_play_by_cmd_handle(cmd_handle, -1, false);
							Emergency_call_help_time_flag = 1;
						}
					}
					else
					{
						prompt_play_by_cmd_handle(cmd_handle, -1, false);
					}
				}
			}
			else
			{
				Emergency_call_help_time = 0;
				Emergency_call_help_time_flag = 0;
				if(semantic == TEMPTURE_ADD)
				{
					mprintf("ffffffffff:%d\n",user_data.tempture);
					if(user_data.tempture >= AIR_TEMPTURE_MAX)
					{
						user_data.tempture = AIR_TEMPTURE_MAX;
						prompt_play_by_semantic_id(TEMPTURE_ADD, 1, false);
						return;
					}
					else
					{
						user_data.tempture = user_data.tempture+1;
						prompt_play_by_semantic_id(user_data.tempture, -1, false);
					}
					uart_send_customer_cmd(user_data.tempture);
				}
				else  if(semantic == TEMPTURE_SUB)
				{
					mprintf("eeeeeeeeee:%d\n",user_data.tempture);
					if(user_data.tempture <= AIR_TEMPTURE_MIN)
					{
						prompt_play_by_semantic_id(TEMPTURE_SUB, 1, false);
						user_data.tempture = AIR_TEMPTURE_MIN;
						return;
					}
					else
					{
						user_data.tempture = user_data.tempture-1;
						prompt_play_by_semantic_id(user_data.tempture, -1, false);
					}
					uart_send_customer_cmd(user_data.tempture);
				}
				else  if(semantic == MODE_CHANGE)
				{
					++user_data.air_mode;
					if(user_data.air_mode>=MODE_QUANTITY)
					{
						user_data.air_mode=0;
					}
					prompt_play_by_semantic_id(AIR_HAT_MODE+user_data.air_mode,-1, false);
					uart_send_customer_cmd(user_data.air_mode);
				}
				else  if(semantic == WIND_SPEED_ADD)
				{
					if(user_data.wind_speed >= THREE_GEAR_SPEED)
					{
						user_data.wind_speed = THREE_GEAR_SPEED;
						prompt_play_by_semantic_id(MAX_SPEED, -1, false);
						return;
					}
					else
					{
						user_data.wind_speed++;
						prompt_play_by_semantic_id(user_data.wind_speed, -1, false);
					}
					uart_send_customer_cmd(user_data.wind_speed);
				}
				else  if(semantic == WIND_SPEED_SUB)
				{
					if(user_data.wind_speed<=ONE_GEAR_SPEED)
					{
						user_data.wind_speed=ONE_GEAR_SPEED;
						prompt_play_by_semantic_id(MIN_SPEED, -1, false);
						return;
					}
					else
					{
						user_data.wind_speed--;
						prompt_play_by_semantic_id(user_data.wind_speed, -1, false);
					}
					uart_send_customer_cmd(user_data.wind_speed);
				}
				else
				{
					#if USER_ASR_CMD_PLAY   	
					if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_PLAY)
					{
						bluetooth_wakeup_send_cmd(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
					}
					prompt_play_by_cmd_handle(cmd_handle, -1, false);
					#endif
					if((semantic>=AIR_TEMPTURE_MIN)&&(semantic<=AIR_TEMPTURE_MAX))
						user_data.tempture = semantic;
					uart_send_customer_cmd(semantic);
				}
			}
		}
		else if(semantic ==BLUETTOTH_CALL_EMERGENCY_NUMBER_SEMANTIC_ID)
		{
			if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_PLAY)
			{
				g_voice_pause_music_flag = 2;
				bluetooth_wakeup_send_cmd(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
			}	
			if(Emergency_call_help_time_flag == 0)
			{
				Emergency_call_help_time= xTaskGetTickCount();
				prompt_play_by_cmd_handle(cmd_handle, -1, false);
				Emergency_call_help_time_flag = 1;
			}
			else if((Emergency_call_help_time_flag == 1)&&(xTaskGetTickCount() - Emergency_call_help_time)<(pdMS_TO_TICKS(10*1000)))
			{
				bluetooth_send_cmd(semantic);
				prompt_play_by_cmd_handle(cmd_handle, 1, false);
				Emergency_call_help_time = 0;
				Emergency_call_help_time_flag = 2;
			}
			else
			{
				Emergency_call_help_time= xTaskGetTickCount();
				prompt_play_by_cmd_handle(cmd_handle, -1, false);
				Emergency_call_help_time_flag = 1;
			}
		}
	}
}
/******************************************************************************
  * @void Deal_Serial_msg_If(sys_com1_msg_data_t *com_rev_data)
  * @parameter:  sys_com1_msg_data_t *com_rev_data 
  * @return : none
  * @function :处理控制器发的串口数据
*******************************************************************************/
void custormer_com_deal_msg(sys_msg_com_data_t *com_rev_data)
{
	sys_msg_com_data_t com_data;
	int dindex=0;
	com_data.header0 = CUSTOMR_UART_HEADER0;	
	com_data.header1 = CUSTOMR_UART_HEADER1;	
	com_data.id = CUSTOMR_UART_HEADER2;

	if((com_rev_data->header0 == CUSTOMR_UART_HEADER0)&&(com_rev_data->header1 == CUSTOMR_UART_HEADER1)&&(com_rev_data->end == CUSTOMR_UART_END))
	{
		switch(com_rev_data->cmd)
		{
			case CI_RX_PLAY_INDEX:
			{
				com_data.cmd= CI_TX_PLAY_INDEX;
				com_data.data0= 1;  
				com_data.data1= 0;
				dindex=com_rev_data->data0;	//查找对应的index
				//mprintf("@@@@:%s,%d,%d\n",__FUNCTION__,__LINE__,dindex);
				if((cmd_info_find_command_by_semantic_id(dindex))==INVALID_HANDLE)
				{
					return;	
				}
				if(dindex ==-1)
					return;
				if(g_UartSendIdle)
					g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);
				
				
				if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_PLAY)
				{
					bluetooth_wakeup_send_cmd(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
				}
				prompt_play_by_semantic_id(dindex, -1, false);
				break;   	
			}
			case CI_RX_ASR_SWITCH:
			{
				com_data.cmd= CI_TX_ASR_START_OK;
				com_data.data0= com_rev_data->data0;  
				com_data.data1= com_rev_data->data1;
				mprintf("@@@@CI_RX_ASR_SWITCH:%s,%x,%x\n",__FUNCTION__,com_data.data0,com_data.data1);
				if(g_UartSendIdle)
					g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);
				break;
			}

			case CI_RX_RESET:
			{
				if(com_rev_data->data0==1)
				{
					//Scu_SoftwareRst_System();
				}
				break;
			}
			case CI_RX_UNWAKE_CMD:
			{
				if(com_rev_data->data0==1)
				{
					/*set wakeup state*/
					set_state_exit_wakeup();
					/*change asr word*/
					change_asr_wakeup_word();
				}
                break;
			}
			case CI_RX_MUTE://
			{ 
				user_set_voice_onoff(com_rev_data->data0);
				com_data.cmd= CI_TX_VOICE_ONOFF_OK;
				com_data.data0= 1;  
				com_data.data1= 0;
				if(g_UartSendIdle)
					g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);
				break;
			}
			#if 0
			case CI_RX_VOLSET://play wav index (com_data->data0)
			{
				user_set_vol(com_rev_data->data0);
				com_data.cmd= CI_TX_VOLSET_OK;
				com_data.data0= 1;  
				com_data.data1= 0;
				if(g_UartSendIdle)
					g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);
				break;
			}
			#endif
			case CI_RX_GETVER:
			if(com_rev_data->data0==1)
			{
				com_data.cmd= CI_TX_VER;
				com_data.data0= USER_VERSION_MAIN_NO;  
				com_data.data1= USER_VERSION_SUB_NO;

				if(g_UartSendIdle)
					g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);				
				break;
			}
			#if 1
			case CI_RX_SET_WAKE_TIME://set wake time
			{
				if(com_rev_data->data0 >= 0x05)
				{
					user_set_wakeup_time(com_rev_data->data0);
					set_wakeup_time(com_rev_data->data0*1000);
				} 
				com_data.cmd= CI_TX_SET_WAKE_TIME_OK;
				com_data.data0= 1;  
				com_data.data1= 0;
				if(g_UartSendIdle)
					g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);
				break;
			}
			#endif
			case CI_RX_AUTO_TEST:
			{
				g_auto_test_flag = 1;
				user_set_voice_onoff(com_rev_data->data0);
				change_asr_normal_word();
				com_data.cmd= CI_TX_VOICE_ONOFF_OK;
				com_data.data0= 1;  
				com_data.data1= 0;
				if(g_UartSendIdle)
					g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);
				break;
			}
			default:
				break;
		}
	}
}
/******************************************************************************
  * @bool system_check_playid_status(cmd_handle_t cmd_handle)
  * @parameter:  none 
  * @return : none
  * @function : 播放完后处理协议
*******************************************************************************/
void system_check_playid_status(cmd_handle_t cmd_handle)
{

	uint32_t semantic = cmd_info_get_semantic_id(cmd_handle);
	if(cmd_info_is_wakeup_word(cmd_handle))
	{
		if(SYS_STATE_WAKEUP == get_wakeup_state())  /*wakeup word*/
		{
			stop_play_music_timer();
			set_state_enter_wakeup();
			change_asr_normal_word();
			ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_IDLE);
		}
	}
	else 
	{
		char* cmd_string = cmd_info_get_command_string(cmd_handle);
		if (strcmp(cmd_string,"<inactivate>")==0)
		{
			#if USE_CWSL
			cwsl_app_exit_reg();
			#endif
			set_state_exit_wakeup();
			change_asr_wakeup_word();
			if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_VOICE_STOP)
				bluetooth_send_cmd(BLUETTOTH_PLAY_MUSIC_SEMANTIC_ID);
		}
		else if(strcmp(cmd_string,"<welcome>")==0)
		{
			change_asr_normal_word();
			set_state_exit_wakeup();
			change_asr_wakeup_word();
		}
		else 
		{
			switch(semantic)
			{

				#if  SUPPORT_BLUETOOTH_FUNCTION
				case BLUETTOTH_PLAY_MUSIC_SEMANTIC_ID:  //play music
				case BLUETTOTH_PRE_MUSIC_SEMANTIC_ID:  //play last music
				case BLUETTOTH_NEXT_MUSIC_SEMANTIC_ID:  //play next music
				{
					ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_PLAYING);
					set_state_exit_wakeup();
					set_cmd_play_music_state();
					change_asr_wakeup_word();			
					bluetooth_send_cmd(semantic);
					break;
				}
				case BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID:
				{
					bluetooth_send_cmd(semantic);
					break;
				}
				case BLUETTOTH_HUNG_UP_PHONE_SEMANTIC_ID:
				{			
					bluetooth_send_cmd(semantic);
					cmd_info_change_cur_model_group(1);
					set_state_exit_wakeup();
					break;
				}
				case BLUETTOTH_OPEN_SEMANTIC_ID1:
				case BLUETTOTH_OPEN_SEMANTIC_ID2:
				case BLUETTOTH_CLOSE_SEMANTIC_ID:
				case BLUETTOTH_ANSWER_PHONE_SEMANTIC_ID:
				case BLUETTOTH_ANSWER_BL_SEMANTIC_ID:
				case BLUETTOTH_SAVE_FAMILY_NUMBER_SEMANTIC_ID:
				case BLUETTOTH_SAVE_FATHER_NUMBER_SEMANTIC_ID:
				case BLUETTOTH_SAVE_FRENDS_NUMBER_SEMANTIC_ID:
				case BLUETTOTH_SAVE_EMERGENCY_NUMBER_SEMANTIC_ID:
				case BLUETTOTH_CALL_FAMILY_NUMBER_SEMANTIC_ID:
				case BLUETTOTH_CALL_FATHER_NUMBER_SEMANTIC_ID:
				case BLUETTOTH_CALL_FRENDS_NUMBER_SEMANTIC_ID:
				{			
					bluetooth_send_cmd(semantic);
					break;
				}
				#endif
				default:
				{
					if(SYS_STATE_WAKEUP == get_wakeup_state()) /*wakeup word*/
					{
						set_state_enter_wakeup();
						change_asr_normal_word();
					}
					else if(SYS_STATE_UNWAKEUP == get_wakeup_state())  
					{
						#if  SUPPORT_BLUETOOTH_FUNCTION
						if(get_bluetooth_play_state()==BLUETOOTH_CURRENT_VOICE_STOP)
						{
							bluetooth_send_cmd(BLUETTOTH_PLAY_MUSIC_SEMANTIC_ID);
						}
						#endif
					}
					break;
				}
			}
		}
	}
	if((get_Sleep_Flag() == 0)&&(SYS_STATE_ENTERNWAKEUP == get_wakeup_state()))
	{
		set_PlayVoice_time();
	}
}

/******************************************************************************
  * @bool Play_Voice_Over_If(void)
  * @parameter:  none 
  * @return : none
  * @function : 播放完后处理协议
*******************************************************************************/

void Play_Voice_Over_If(void)
{
	sys_msg_com_data_t com_data;
	com_data.header0 = CUSTOMR_UART_HEADER0;	
	com_data.header1 = CUSTOMR_UART_HEADER1;	
	com_data.id = CUSTOMR_UART_HEADER2;
	com_data.cmd= CI_TX_OVER_PLAY;
	com_data.data0= 1;  
	com_data.data1= 0;
	if(g_UartSendIdle)
		g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);	
}
/***************************************************************************************
* @void Init_Customer_Paramter_If(void)
* @parameter: none
* @return :    none
* @function : use init customer special parameter 
*******************************************************************************************/
void	Init_Customer_Paramter_If(void)
{
	#if USER_SERIAL_IO_UART0
	Chip_Init_Uart_CallBack();
	UARTInterruptConfig((UART_TypeDef*)HAL_UART0_BASE, UART_BaudRate9600);
	#endif
	#if USER_SERIAL_IO_UART1
	Chip_Init_Uart_CallBack();
	UARTInterruptConfig((UART_TypeDef*)HAL_UART1_BASE, UART_BaudRate9600);
	#endif
	#if USER_SERIAL_IO_UART2
	Chip_Init_Uart_CallBack();
	UARTInterruptConfig((UART_TypeDef*)HAL_UART2_BASE, UART_BaudRate9600);
	#endif
	dLasttsendime = xTaskGetTickCount();
}
#if UART_BAUDRATE_CALIBRATE
 void send_baudrate_sync_req(void)
{
	TickType_t dcurtime=xTaskGetTickCount();
	if(dcurtime <(dLasttsendime+200))
		vTaskDelay(200);
	sys_msg_com_data_t com_data;
	com_data.header0 = CUSTOMR_UART_HEADER0;	
	com_data.header1 = CUSTOMR_UART_HEADER1;	
	com_data.id = CUSTOMR_UART_HEADER2;
	com_data.cmd= CI_TX_ASR;
	com_data.data0=0xF8;  
	com_data.data1 = 0;
	if(g_UartSendIdle)
		g_UartSendIdle((unsigned char*)&com_data.header0,CUSTOMR_SEND_DATA_LEN,CUSTOMR_CHECK_SUM,CUSTOMR_UART_END);
	dLasttsendime = xTaskGetTickCount();
}
static void baudrate_sync_callbackl(void)
{
	sys_msg_t send_msg;
	send_msg.msg_type = SYS_MSG_TYPE_COM_HAND;
	send_msg_to_sys_task(&send_msg, NULL);
}
/***************************************************************************************
* @void Init_Customer_Paramter_If(void)
* @parameter: none
* @return :    none
* @function : use init customer special parameter 
*******************************************************************************************/
void Init_baudrate_sync(void)
{
	#if USER_SERIAL_IO_UART0
	baudrate_calibrate_init((UART_TypeDef*)HAL_UART0_BASE, UART_BaudRate9600, baudrate_sync_callbackl);          // 初始化波特率校准
	#endif
	#if USER_SERIAL_IO_UART1
	baudrate_calibrate_init((UART_TypeDef*)HAL_UART1_BASE, UART_BaudRate9600, baudrate_sync_callbackl);          // 初始化波特率校准
	#endif
	#if USER_SERIAL_IO_UART2
	baudrate_calibrate_init((UART_TypeDef*)HAL_UART2_BASE, UART_BaudRate9600, baudrate_sync_callbackl);          // 初始化波特率校准
	#endif
}
#endif

