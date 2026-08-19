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
#include "user_data_save.h"
#include "board.h"
#include "prompt_player.h"
#include "bluetooth_play_control.h"
#include "customer_control.h"
#include "user_data_save.h"
#include "status_share.h"

#if  SUPPORT_BLUETOOTH_FUNCTION

static int g_bluetooth_play_state =0;
static int g_call_play_state =0;

static int g_bluetooth_connect_state =0;
static unsigned char g_Send_bluetooth_ID=0;
char g_voice_pause_music_flag =0;
char g_cmd_play_music_flag =0;
bool g_open_play_music_flag =false;

bool g_voice_phone_flag =false;


user_data_stru user_data= {
	.air_mode   = AIR_HAT_MODE,
	.tempture   = AIR_TEMPTURE_MIN,
	.wind_mode  = CLODE_SWEEP_WIND,
	.air_switch = CLOSE_AIRCONDITION,
	.wind_speed = ONE_GEAR_SPEED,
}; 

static bluetoothSend g_BlueTooth1SendIdle = NULL;

static int BlueTooth_rx_state= BLUETOOTH_STATE_HEADER0;
xTimerHandle ble_play_state_timer = NULL;
static sys_msg_t BlueTooth_send_msg;
static unsigned short diotlen =  0;
static unsigned short ddatacnt =  0;
static unsigned char  openmachineflag =  0;
static unsigned char  Teamachineaddwaterflag =  0;
static unsigned char  TeamachineRefrigerationflag =  0;
static unsigned char  Teamachineaddheatflag =  0;
static unsigned char  Teamachineaddheat45flag =  0;
static unsigned char  Teamachineaddheat65flag =  0;
static unsigned char  Teamachineaddheat85flag =  0;
static unsigned char  Teamachineaddheat100flag =  0;
static unsigned char  Teamachineautowaterflag =  0;
static unsigned char  Teamachineautoaddheatflag =  0;
static unsigned char  Teamachinekeepwarmflag =  0;
static unsigned short ddatalen =  0;
static unsigned short dchecksum =  0;
TickType_t blelast_time =0;
extern int Emergency_call_help_time_flag ;

int dbledataoff =0;
/******************************************************************************
  * @void customer_rx_idle(unsigned char rxdata)
  * @parameter:  serial data 
  * @return : none
  * @function : it's interface for receive customer data
*******************************************************************************/
static void customer_BlueTooth_idle(unsigned char rxdata)
{     

	if(((rxdata==BLE_COMMON_DATA_HEADER0)||(rxdata==BLE_TRANSMISSION_HEAD0))&&(BlueTooth_rx_state !=BLUETOOTH_STATE_HEADER0))
	{
		if(( xTaskGetTickCount() -blelast_time)>pdMS_TO_TICKS(500))
		{
			BlueTooth_rx_state = BLUETOOTH_STATE_HEADER0;
		}
	}
	switch(BlueTooth_rx_state)
	{
		case BLUETOOTH_STATE_HEADER0:
		{
			if((rxdata==BLE_COMMON_DATA_HEADER0)||(rxdata==BLE_TRANSMISSION_HEAD0))
			{
				BlueTooth_send_msg.msg_data.bluetooth_data.header0 = rxdata;
				BlueTooth_send_msg.msg_data.app_data.header0 = rxdata;
				dchecksum = rxdata;
				blelast_time =  xTaskGetTickCount();
				BlueTooth_rx_state = BLUETOOTH_STATE_HEADER1;
			}
			else
			{
				APPprintf("@@HEADER0error:%x\n",rxdata);
			}
			break;
		}
		case BLUETOOTH_STATE_HEADER1:
		{
			if((rxdata==BLE_COMMON_DATA_HEADER1)||(rxdata==BLE_TRANSMISSION_HEAD1))
			{
				BlueTooth_send_msg.msg_data.bluetooth_data.header1 = rxdata;
				BlueTooth_send_msg.msg_data.app_data.header1 = rxdata;
				dchecksum += rxdata;
				BlueTooth_rx_state = BLUETOOTH_STATE_VER;
			}
			else
			{
				BlueTooth_rx_state = BLUETOOTH_STATE_HEADER0;
			}
			break;
		}
		case BLUETOOTH_STATE_VER:
		{

			if(rxdata == CUSTOMR_BLUETOOTH_VER)
			{
				if((BlueTooth_send_msg.msg_data.bluetooth_data.header0==BLE_COMMON_DATA_HEADER0)&&(BlueTooth_send_msg.msg_data.bluetooth_data.header1==BLE_COMMON_DATA_HEADER1))
				{
					BlueTooth_send_msg.msg_data.bluetooth_data.id= rxdata;
					BlueTooth_rx_state = BLUETOOTH_STATE_CMD;
				}
				else  if((BlueTooth_send_msg.msg_data.app_data.header0 ==BLE_TRANSMISSION_HEAD0)&&(BlueTooth_send_msg.msg_data.app_data.header1 ==BLE_TRANSMISSION_HEAD1))
				{
					BlueTooth_send_msg.msg_data.app_data.Ver= rxdata;
					BlueTooth_rx_state = APP_STATE_CMD;
					dchecksum += rxdata;
				}
				else
				{
					APPprintf("@@VERerror:%x\n",rxdata);
					BlueTooth_rx_state = BLUETOOTH_STATE_HEADER0;
				}
			}
			else
			{
				APPprintf("@@VERerror:%x\n",rxdata);
				BlueTooth_rx_state = BLUETOOTH_STATE_HEADER0;
			}
			break;
		}
		case BLUETOOTH_STATE_CMD:
		{
			dbledataoff = 0;
			BlueTooth_send_msg.msg_data.bluetooth_data.cmd= rxdata;
			BlueTooth_rx_state = BLUETOOTH_STATE_DATA1;
			break;
		}
		case BLUETOOTH_STATE_DATA1:
		{
			BlueTooth_send_msg.msg_data.bluetooth_data.data[dbledataoff++]= rxdata;
			BlueTooth_rx_state = BLUETOOTH_STATE_DATA2;
			break;
		}
		case BLUETOOTH_STATE_DATA2:
		{

			if(((BlueTooth_send_msg.msg_data.bluetooth_data.cmd == BT_TO_CI_BTNAME_DATA_TYPE)&&((BlueTooth_send_msg.msg_data.bluetooth_data.data[0] ==0x10)||(BlueTooth_send_msg.msg_data.bluetooth_data.data[0]  ==0x11)))||(BlueTooth_send_msg.msg_data.bluetooth_data.cmd == CI_TO_BT_BTMAME_TYPE))
			{
				BlueTooth_send_msg.msg_data.bluetooth_data.data[dbledataoff++] = rxdata;
				if(dbledataoff >=SUPPORT_BLE_DATA_LEN)
				{
					BlueTooth_rx_state = BLUETOOTH_STATE_CHECK_SUM;
				}
			}
			else
			{
				BlueTooth_send_msg.msg_data.bluetooth_data.data[dbledataoff++] = rxdata;
				BlueTooth_rx_state=BLUETOOTH_STATE_CHECK_SUM;
			}
			break;
		}		
		case BLUETOOTH_STATE_CHECK_SUM:
		{
			BlueTooth_send_msg.msg_data.bluetooth_data.chksum= rxdata;
			BlueTooth_rx_state = BLUETOOTH_STATE_END;
			break;
		}		
		case BLUETOOTH_STATE_END:
		{
			if(rxdata==CUSTOMR_BLUETOOTH_END)
			{
				if( (BlueTooth_send_msg.msg_data.bluetooth_data.cmd == 0x80)||( BlueTooth_send_msg.msg_data.bluetooth_data.cmd == 0x82)||( BlueTooth_send_msg.msg_data.bluetooth_data.cmd == 0x81))
				{
					g_Send_bluetooth_ID  = BlueTooth_send_msg.msg_data.bluetooth_data.data[0];
				}
				BlueTooth_send_msg.msg_data.bluetooth_data.end= rxdata;
				BlueTooth_rx_state = BLUETOOTH_STATE_HEADER0;
				BlueTooth_send_msg.msg_type = SYS_MSG_TYPE_BLUETOOTH;
				send_msg_to_sys_task(&BlueTooth_send_msg, NULL);
			}
			break;
		}
		case APP_STATE_CMD:
		{
			BlueTooth_send_msg.msg_data.app_data.cmd= rxdata;
			BlueTooth_rx_state = APP_STATE_PAYLOAD;
			ddatalen = 2;
			ddatacnt = 0;
			dchecksum += rxdata;
			break;
		}
		case APP_STATE_PAYLOAD:
		{

			if((ddatacnt <SUPPORT_BLE_DATA_LEN)&&(ddatacnt < ddatalen))
			{
				BlueTooth_send_msg.msg_data.app_data.Payload[ddatacnt++]= rxdata;
				dchecksum += rxdata;
				if(ddatacnt == ddatalen)
				{
					BlueTooth_rx_state = APP_STATE_CHECK_SUM;
				}
			}
			else 
			{
				APPprintf("@@ddatacnterror:%x,%x\n",ddatacnt,ddatalen);
				BlueTooth_rx_state = BLUETOOTH_STATE_HEADER1;
			}
			break;
		}
		case APP_STATE_CHECK_SUM:
		{
			if((dchecksum&0xff)==rxdata)
			{
				BlueTooth_send_msg.msg_data.app_data.chksum = rxdata;
				BlueTooth_rx_state = APP_STATE_CHECK_END;
			}
			else 
			{
				APPprintf("@@checksumherror:%x,%x\n",rxdata,((dchecksum>>8)&0xff));
				BlueTooth_rx_state =  BLUETOOTH_STATE_HEADER1;
			}
			break;
		}
		case APP_STATE_CHECK_END:
		{
			crx_state =  BLUETOOTH_STATE_HEADER1;
			if(rxdata == BLE_TRANSMISSION_END)
			{
				BlueTooth_send_msg.msg_data.app_data.end = rxdata;
				BlueTooth_send_msg.msg_type = SYS_MSG_TYPE_APP;
				send_msg_to_sys_task(&BlueTooth_send_msg, NULL);
			}
			else
			{
				mprintf("@@end:%x\n",rxdata);
			}
			break;
		}
		default:
			break;
	}
}
/******************************************************************************
  * @bool uart_send_customer_cmd(unsigned int index)
  * @parameter:  comdata 串口数据
  * @return : none
  * @function : 用于发送串口数据
*******************************************************************************/
static void bluetooth_send_data(uint32_t semantic)
{
	sys_bluetooth_data_t bluetooth_data;
	bluetooth_data.header0 = BLE_COMMON_DATA_HEADER0;	
	bluetooth_data.header1 = BLE_COMMON_DATA_HEADER1;	
	bluetooth_data.id = 0;
	bluetooth_data.cmd= (semantic>>16)&0xff;
	bluetooth_data.data[0]= (semantic>>8)&0xff;  
	bluetooth_data.data[1] =  semantic&0xff;
    
	if(g_BlueTooth1SendIdle)
		g_BlueTooth1SendIdle((unsigned char*)&bluetooth_data.header0,6,BLUETOOTH_CHECK_SUM,BLUETOOTH_CHECK_END);

}   

int get_bluetooth_open_state(void)
{
	return g_bluetooth_connect_state ;
}
/******************************************************************************
  * @bool uart_send_customer_cmd(unsigned int index)
  * @parameter:  comdata 串口数据
  * @return : none
  * @function : 用于发送串口数据
*******************************************************************************/
static void bluetooth_send_Recive(uint32_t semantic)
{
	sys_bluetooth_data_t bluetooth_data;
	bluetooth_data.header0 = BLE_COMMON_DATA_HEADER0;	
	bluetooth_data.header1 = BLE_COMMON_DATA_HEADER1;	
	bluetooth_data.id = 0;
	bluetooth_data.cmd= BLUETOOTH_TO_VOICE_CMD;
	bluetooth_data.data[0]=semantic&0xff;  
	bluetooth_data.data[1] = 0;
	if(g_BlueTooth1SendIdle)
		g_BlueTooth1SendIdle((unsigned char*)&bluetooth_data.header0,6,BLUETOOTH_CHECK_SUM,BLUETOOTH_CHECK_END);

}   
/************************************************
 * @brief:璁剧疆bt鎴栬�卋le鍚嶅瓧
 * 
 * @param:bool name_switch  0锛歜t_name 1:ble_name
 * 		  uint32_t name 32浣嶅悕瀛楁暟鎹?utf8缂栫爜	
 * @return:void
 ************************************************/
// bt_name_str bt_about_name;
void set_bt_or_ble_name(bool name_switch)
{
	sys_bluetooth_data_t senddata;
	uint8_t* buf = NULL;
	senddata.header0= BLE_COMMON_DATA_HEADER0,
	senddata.header1= BLE_COMMON_DATA_HEADER1,
	senddata.id= 0x00,
	senddata.cmd= CI_TO_BT_BTMAME_TYPE,
	memset((char*) &senddata.data[0],0,SUPPORT_BLE_DATA_LEN);
	senddata.data[0]= name_switch;
	if(name_switch == WRITE_BLE_NAME)
	{
		strncpy((char*) &senddata.data[1],SET_BLE_NAME,strlen(SET_BLE_NAME));
	}
	else if(name_switch == WRITE_BT_NAME)
	{
		strncpy((char*) &senddata.data[1],SET_BT_NAME,strlen(SET_BT_NAME));
	}
	APPprintf("aaaa:%s\n",(char*)&senddata.data[1]);
	if(g_BlueTooth1SendIdle)
		g_BlueTooth1SendIdle((unsigned char*)&senddata.header0,4+SUPPORT_BLE_DATA_LEN,BLUETOOTH_CHECK_SUM,BLUETOOTH_CHECK_END);

}

/******************************************************************************
  * @bool Chip_Init_Uart_CallBack(unsigned int(void)
  * @parameter:  void
  * @return : none
  * @function : 串口注册函数
*******************************************************************************/
static void Init_bluetooth_CallBack(void)
{

	#if USER_BLUETOOTH_UART1
	Chip_Regist_Uart1CallBack((pUartCallBack)customer_BlueTooth_idle);
	g_BlueTooth1SendIdle = (bluetoothSend)Chip_Uart1_Send;
	#endif

}
int g_pringt_count=0;
/******************************************************************************
  * @void iot_deal_msg(sys_msg_iot_data_t *iot_rev_data)
  * @parameter:  sys_msg_iot_data_t *iot_rev_data 
  * @return : none
  * @function :处理WIFI 发的串口数据
*******************************************************************************/
void bluetooth_deal_msg(sys_bluetooth_data_t *bluetooth_rev_data)
{
	uint32_t semantic_id=0;
	if((bluetooth_rev_data->header0 == BLE_COMMON_DATA_HEADER0)&&(bluetooth_rev_data->header1 == BLE_COMMON_DATA_HEADER1)&&(bluetooth_rev_data->end == CUSTOMR_BLUETOOTH_END))
	{
		switch(bluetooth_rev_data->cmd)
		{
			case BLUETOOTH_TO_VOICE_CMD:
			{
				bluetooth_send_Recive(bluetooth_rev_data->data[0]);      
				APPprintf("@@bluetooth_rev_data->data[0]:%x\n",bluetooth_rev_data->data[0]);
				switch(bluetooth_rev_data->data[0])
				{
					case BL_ANSWER_CONNECT_OK_ID:
					{
						g_bluetooth_connect_state =1;
 						g_open_play_music_flag = false;
						uint32_t blevolset = BLUETTOTH_VOL_ADJUST+2+2*user_get_vol();
						bluetooth_send_cmd(blevolset);
					}
					break;
					case BL_ANSWER_CONNECT_FAIL_ID:
					{
						g_bluetooth_connect_state =0;
						g_bluetooth_play_state = 0;
						g_open_play_music_flag = false;
						ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_IDLE);
					}
					break;
					case BL_ANSWER_PLAY_MUSIC_ID:
					{
						g_bluetooth_play_state = BLUETOOTH_CURRENT_PLAY;
						g_voice_pause_music_flag = 0;
						APPprintf("@@BL_ANSWER_PLAY_MUSIC_ID\n");
						ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_PLAYING);
						set_state_exit_wakeup();
						change_asr_wakeup_word();           
						xTimerStop(ble_play_state_timer,0);
						if(g_cmd_play_music_flag ==0)
						{
							xTimerStart(ble_play_state_timer,0);
						}
						g_cmd_play_music_flag = 0;
					}
					break;
					case BL_ANSWER_PHONE_PAUSE_MUSIC_ID:
					{
						xTimerStop(ble_play_state_timer,0);
						APPprintf("@@BL_ANSWER_PHONE_PAUSE_MUSIC_ID:%d\n",g_voice_pause_music_flag);
						if(g_voice_pause_music_flag ==1)
						{
							g_bluetooth_play_state = BLUETOOTH_CURRENT_VOICE_STOP ;
						}
						else
						{
							g_bluetooth_play_state = BLUETOOTH_CURRENT_MOBILE_STOP ;
						}
						ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_IDLE);
					}
					break;
					case BL_ANSWER_VOICE_PAUSE_MUSIC_ID:
					{
						APPprintf("@@BL_ANSWER_VOICE_PAUSE_MUSIC_ID:%d\n",g_voice_pause_music_flag);
						xTimerStop(ble_play_state_timer,0);
						if(g_voice_pause_music_flag ==1)
						{
							g_bluetooth_play_state = BLUETOOTH_CURRENT_VOICE_STOP ;
						}
						else
						{
							g_bluetooth_play_state = BLUETOOTH_CURRENT_MOBILE_STOP ;
						}
						ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_IDLE);
					}
					break;
					case BL_ANSWER_CALL_OUT_STATE:
					{
						g_call_play_state = g_bluetooth_play_state;
						APPprintf("@@BL_ANSWER_CALL_OUT_STATE\n");
						ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_PLAYING);
						g_voice_phone_flag =true;
						set_state_enter_wakeup();
						cmd_info_change_cur_model_group(2);
						break;			
					}
					case BL_ANSWER_CALL_IN_STATE:
					{
						g_call_play_state = g_bluetooth_play_state;
						APPprintf("@@BL_ANSWER_CALL_IN_STATE\n");
						g_voice_phone_flag =true;
						ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_PLAYING);
						set_state_enter_wakeup();
						cmd_info_change_cur_model_group(2);
						break;
					}
					case BL_ANSWER_HANG_UP_PHONE_STATE:	
					{
						g_voice_phone_flag =false;
						Emergency_call_help_time_flag = 0;	
						set_state_exit_wakeup();
						change_asr_wakeup_word();  
						if(g_call_play_state == BLUETOOTH_CURRENT_PLAY)
						{
							ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_PLAYING);
							bluetooth_send_cmd(BLUETTOTH_PLAY_MUSIC_SEMANTIC_ID);
							xTimerStop(ble_play_state_timer,0);
							if(g_cmd_play_music_flag ==0)
							{
								xTimerStart(ble_play_state_timer,0);
							}
							g_cmd_play_music_flag = 0;
						}
						else
						{
							ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_IDLE);
						}
						break;				
					}
					case BL_ANSWER_CALL_SUCCESSEFUL_STATE:
					{
						APPprintf("@@BL_ANSWER_CALL_SUCCESSEFUL_STATE\n");
						g_voice_phone_flag =true;
						set_state_enter_wakeup();
						ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_PLAYING);
						cmd_info_change_cur_model_group(2);
						break;	
					}
					case BL_ANSWER_VOLUME_MAX_ID:
					{
						user_set_vol(VOLUME_MAX);
					}
					break;
					case BL_ANSWER_VOLUME_MIN_ID:
					{
						user_set_vol(VOLUME_MIN);
					}
					break;
					default:
						break;
				}
				break;
			}
			case BT_TO_CI_BTNAME_DATA_TYPE:
			{
				APPprintf("@@bluetooth_rev_data->data[0]:%x\n",bluetooth_rev_data->data[0]);
				switch(bluetooth_rev_data->data[0])
				{
					case BLE_NAME_ANSWER:
					{
						APPprintf("BLE_NAME_ANSWER:%s\n",(char*)&bluetooth_rev_data->data[1]);
						if((strncmp(SET_BLE_NAME,(char*)&bluetooth_rev_data->data[1],strlen(SET_BLE_NAME))!=0)
							||(strncmp(SET_BLE_NAME,(char*)&bluetooth_rev_data->data[1],strlen((char*)&bluetooth_rev_data->data[1]))!=0))
						{
							set_bt_or_ble_name(WRITE_BLE_NAME);
							vTaskDelay(500);
						}
						#if  !USE_CHIP_D12GS01J_BODER
						bluetooth_send_cmd(SUPPORT_READ_BT_NAME_CMD);
						#endif
						break;
					}	
					case BT_NAME_ANSWER:
					{
						APPprintf("BT_NAME_ANSWER:%s\n",(char*)&bluetooth_rev_data->data[1]);
						if((strncmp(SET_BT_NAME,(char*)&bluetooth_rev_data->data[1],strlen(SET_BT_NAME))!=0)
						||(strncmp(SET_BT_NAME,(char*)&bluetooth_rev_data->data[1],strlen((char*)&bluetooth_rev_data->data[1]))!=0))
						{
							set_bt_or_ble_name(WRITE_BT_NAME);
							vTaskDelay(500);
						}
						bluetooth_send_cmd(BLUETTOTH_OPEN_SEMANTIC_ID1);
						break;
					}	
					default:
					   break;	
				}
				break;
			}
			default:
				break;
		}
	}
}
/******************************************************************************
  * @void iot_deal_msg(sys_msg_iot_data_t *iot_rev_data)
  * @parameter:  sys_msg_iot_data_t *iot_rev_data 
  * @return : none
  * @function :处理WIFI 发的串口数据
*******************************************************************************/
void app_deal_msg(sys_app_data_t *app_rev_data)
{
	uint32_t semantic_id= COMMAND_ID_MAX;
	if((app_rev_data->header0 == BLE_TRANSMISSION_HEAD0)&&(app_rev_data->header1 == BLE_TRANSMISSION_HEAD1))
	{
		mprintf("RRRRRRRRRcmd:%x,%d\n",app_rev_data->cmd,app_rev_data->Payload[0]);
 		switch (app_rev_data->cmd)
		{
			case BLE_TRANSMISSION_CMD1:
			{
				switch (app_rev_data->Payload[0])
				{
					case AIR_CONDITION_SWITCH_AIRCONDITION:
					{
						if(user_data.air_switch==OPEN_AIRCONDITION)
						{
							user_data.air_switch++;
						}
						else
						{
							user_data.air_switch--;
						}
						semantic_id = user_data.air_switch;
						break;
					}
					case AIR_CONDITION_MODE_CHANGE :
					{
						++user_data.air_mode;
						if(user_data.air_mode>=MODE_QUANTITY)
						{
							user_data.air_mode=0;
						}                					
						semantic_id = AIR_HAT_MODE+user_data.air_mode;
						break;
					}
					case AIR_CONDITION_SWITCH_WINDOW :
					{
						if(user_data.wind_mode==CLODE_SWEEP_WIND)
						{
							user_data.wind_mode=user_data.wind_mode-1;
						}
						else
						{
							user_data.wind_mode=user_data.wind_mode+1;
						}
						semantic_id = user_data.wind_mode;
						break;
					}
					case AIR_CONDITION_TEMPTURE_ADD :
					{								
						if(user_data.tempture >= AIR_TEMPTURE_MAX)
						{
							user_data.tempture = AIR_TEMPTURE_MAX;
							prompt_play_by_semantic_id(TEMPTURE_ADD, 1, false);
							return ;
						}
						else
						{
							user_data.tempture = user_data.tempture+1;
							semantic_id = user_data.tempture;
						}
						mprintf("user_data.tempture=%d\n",user_data.tempture);
						break;
					}
					case AIR_CONDITION_TEMPTURE_SUB:
					{
						if(user_data.tempture <= AIR_TEMPTURE_MIN)
						{
							prompt_play_by_semantic_id(TEMPTURE_SUB, 1, false);
							user_data.tempture = AIR_TEMPTURE_MIN;
							return;
						}
						else
						{
							user_data.tempture = user_data.tempture-1;
							semantic_id = user_data.tempture;
						}
					}
					break;
					case AIR_CONDITION_TEMPTURE_SET:
					{
						user_data.tempture= app_rev_data->Payload[1] ;
						mprintf("user_data.tempture=%d\n",user_data.tempture);
						if((user_data.tempture>=AIR_TEMPTURE_MIN)&&(user_data.tempture<=AIR_TEMPTURE_MAX))
						{
							semantic_id = user_data.tempture;
						}
						break;;
					}
					case AIR_CONDITION_SPEED_ADD :
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
							semantic_id = user_data.wind_speed;
						}
					}
					break;
					case AIR_CONDITION_SPEED_SUB :
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
							semantic_id = user_data.wind_speed;
						}
					}
					break;
					case AIR_CONDITION_SPEED_AUTO :
					{
						semantic_id = AUTO_SPEED_MODE;
						break;
					}
					case AIR_CONDITION_EVE_LIGHT :
					{
						user_data.light_state.night_light_state=~user_data.light_state.night_light_state;
						if(user_data.light_state.night_light_state==true)
							semantic_id = OPEN_NIGHT_LIGHT;									
						else
							semantic_id = CLOSE_NIGHT_LIGHT;									
						break;
					}
					case AIR_CONDITION_BALCONY_LIGHT :
					{
						user_data.light_state.balcony_light_state=~user_data.light_state.balcony_light_state;
						if(user_data.light_state.balcony_light_state==true)
							semantic_id = OPEN_BALCONY_LIGHT;
						else
							semantic_id = CLOSE_BALCONY_LIGHT;
						break;
					}
					case AIR_CONDITION_BEDROOM_LIGHT :
					{
						user_data.light_state.bedroom_light_state=~user_data.light_state.bedroom_light_state;
						if(user_data.light_state.bedroom_light_state==true)
							semantic_id = OPEN_BEDROOM_LIGHT;
						else
							semantic_id = CLOSE_BEDROOM_LIGHT;
						break;
					}
					case AIR_CONDITION_GARDEN_LIGHT :
					{
						user_data.light_state.garden_light_state=~user_data.light_state.garden_light_state;
						if(user_data.light_state.garden_light_state==true)
							semantic_id = OPEN_GARDEN_LIGHT;
						else
							semantic_id = CLOSE_GARDEN_LIGHT;
						break;
					}
					case AIR_CONDITION_RESTAURANT_LIGHT :
					{
						user_data.light_state.dining_light_state=~user_data.light_state.dining_light_state;
						if(user_data.light_state.dining_light_state==true)
							semantic_id = OPEN_DINING_LIGHT;
						else
							semantic_id = CLOSE_DINING_LIGHT;
						break;
					}
					case AIR_CONDITION_TOILET_LIGHT :
					{
						user_data.light_state.toilet_light_state=~user_data.light_state.toilet_light_state;
						if(user_data.light_state.toilet_light_state==true)
							semantic_id = OPEN_TOILET_LIGHT;
						else
							semantic_id = CLOSE_TOILET_LIGHT;
						break;
					}
					case AIR_CONDITION_LIVING_LIGHT :
					{
						user_data.light_state.living_light_state=~user_data.light_state.living_light_state;
						if(user_data.light_state.living_light_state==true)
							semantic_id = OPEN_LIVING_ROOM;
						else
							semantic_id = CLOSE_LIVING_ROOM;
						break;
					}
					default:
					{  
						if((app_rev_data->Payload[0]>AIR_CONDITION_EVE_LIGHT)&&(app_rev_data->Payload[0]<AIR_CONDITION_BALCONY_LIGHT))
						{
							semantic_id = BRIGHTNESS_ADD+app_rev_data->Payload[0]-AIR_CONDITION_LIGHT_ADD;
						}
						break;
					}
				}
				break;
			}
			case BLE_TRANSMISSION_CMD2:
			{
				mprintf("@@airdata[0]:%x:%x\n",app_rev_data->Payload[0],app_rev_data->Payload[1]);
				semantic_id = user_data.air_switch;
				break;
			}
			default:
				break;
		}
		mprintf("bbbbbsemantic_id:%d\n",semantic_id);
		if(semantic_id!= COMMAND_ID_MAX)
		{
			prompt_play_by_semantic_id(semantic_id,-1, false);
			uart_send_customer_cmd(semantic_id);
		}

	}
}
void ble_play_state_cb(TimerHandle_t xTimer)
{
	xTimerStop(ble_play_state_timer,0);
	g_voice_pause_music_flag = 0;
	set_state_exit_wakeup();
	change_asr_wakeup_word();
}

void set_aec_state_play_music(void)
{
	if(g_bluetooth_connect_state)
	{
		if((g_bluetooth_play_state == BLUETOOTH_CURRENT_PLAY)||(g_voice_phone_flag == true))
		{
			ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_PLAYING);
		}
		else
		{
			ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_IDLE);
		}
	}
}
void set_cmd_play_music_state(void)
{
	g_cmd_play_music_flag = 1;
}
/***************************************************************************************
* @void Init_Customer_Paramter_If(void)
* @parameter: none
* @return :    none
* @function : use init customer special parameter 
*******************************************************************************************/
void Init_bluetooth_Paramter_If(void)
{
	g_bluetooth_connect_state= 0;
	BlueTooth_rx_state = BLUETOOTH_STATE_HEADER0;
	Init_bluetooth_CallBack();
	#if USER_BLUETOOTH_UART1
	UARTInterruptConfig((UART_TypeDef*)HAL_UART1_BASE, UART_BaudRate9600);
	#endif
	ble_play_state_timer = xTimerCreate("ble_play_state", pdMS_TO_TICKS(7000),
	pdFALSE, (void *)0, ble_play_state_cb);
	xTimerStop(ble_play_state_timer,0);	
	bluetooth_send_cmd(SUPPORT_READ_BLE_NAME_CMD);

}
int get_bluetooth_phone_state(void)
{
	return g_voice_phone_flag ;
}

int get_bluetooth_play_state(void)
{
	return g_bluetooth_play_state ;
}
int get_bluetooth_connect_state(void)
{
	return g_bluetooth_connect_state ;
}

void bluetooth_send_cmd(uint32_t semantic)
{ 
	int id =0;
	unsigned char Send_bluetooth =(semantic>>8)&0xff;
	g_Send_bluetooth_ID =0;
	if(semantic == BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID)
	{
		g_voice_pause_music_flag = 0;
		g_bluetooth_play_state =BLUETOOTH_CURRENT_MOBILE_STOP;
		bluetooth_send_data(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
		vTaskDelay(pdMS_TO_TICKS(10));
		g_open_play_music_flag = true;
		pause_send:
		bluetooth_send_data(semantic);
		APPprintf("@@@:%s.%d\n",__FUNCTION__,__LINE__);
		for (int i=0; i<50; i++)
		{
			vTaskDelay(pdMS_TO_TICKS(10));
			if (g_Send_bluetooth_ID == Send_bluetooth) 
			{
				break;
			}
		}
		id++;
		if(id<3)
		{
			goto pause_send;
		}
	}
	if(semantic == BLUETTOTH_CLOSE_SEMANTIC_ID)
		g_bluetooth_connect_state =0;
	if((semantic == BLUETTOTH_PLAY_MUSIC_SEMANTIC_ID)||(semantic == BLUETTOTH_PRE_MUSIC_SEMANTIC_ID)||(semantic == BLUETTOTH_NEXT_MUSIC_SEMANTIC_ID))
	{
		ciss_set(CI_SS_PLAY_STATE,CI_SS_PLAY_STATE_PLAYING);
		g_bluetooth_play_state =BLUETOOTH_CURRENT_PLAY;
	}
	if((semantic == BLUETTOTH_PLAY_MUSIC_SEMANTIC_ID)&&(g_open_play_music_flag == false))
	{
		bluetooth_send_data(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
		vTaskDelay(pdMS_TO_TICKS(10));
		g_open_play_music_flag = true;
		FPLAY_send:
			bluetooth_send_data(semantic);
			APPprintf("@@@:%s.%d\n",__FUNCTION__,__LINE__);
			for (int i=0; i<50; i++)
			{
				vTaskDelay(pdMS_TO_TICKS(10));
				if (g_Send_bluetooth_ID == Send_bluetooth) 
				{
					break;
				}
			}
			id++;
			if(id<3)
			{
				goto FPLAY_send;
			}
	}
	APPprintf("wwwbluetooth_Play_controlindex:%x,%x\n",Send_bluetooth,semantic);
	g_Send_bluetooth_ID = 0;
	Pre_send:
	bluetooth_send_data(semantic);
	APPprintf("@@@:%s.%d\n",__FUNCTION__,__LINE__);
	for (int i=0; i<50; i++)
	{
		vTaskDelay(pdMS_TO_TICKS(10));
		if (g_Send_bluetooth_ID == Send_bluetooth) 
		{
			vTaskDelay(pdMS_TO_TICKS(20));
			g_Send_bluetooth_ID = 0;
			return;
		}
	}
	id++;
	if(id<4)
	{
		goto Pre_send;
	}
}
void bluetooth_wakeup_send_cmd(uint32_t semantic)
{ 
	int id =0;

	unsigned char Send_bluetooth =(semantic>>8)&0xff;
	g_Send_bluetooth_ID =0;
	if(semantic == BLUETTOTH_CLOSE_SEMANTIC_ID)
		g_bluetooth_connect_state =0;
	if(semantic == BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID)
	{
		g_voice_pause_music_flag = 1;
		g_bluetooth_play_state =BLUETOOTH_CURRENT_VOICE_STOP;
		bluetooth_send_data(BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID);
		vTaskDelay(pdMS_TO_TICKS(10));
		g_open_play_music_flag = true;
		wpause_send:
			bluetooth_send_data(semantic);
			APPprintf("@@@:%s.%d\n",__FUNCTION__,__LINE__);
			for (int i=0; i<50; i++)
			{
				vTaskDelay(pdMS_TO_TICKS(10));
				if (g_Send_bluetooth_ID == Send_bluetooth) 
				{
					break;
				}
			}
			id++;
			if(id<3)
			{
				goto wpause_send;
			}
	}

	APPprintf("wbluetooth_Play_controlindex:%x,%x\n",Send_bluetooth,semantic);

	re_send:
	bluetooth_send_data(semantic);
	
	for (int i=0; i<50; i++)
	{
		vTaskDelay(pdMS_TO_TICKS(10));
		if (g_Send_bluetooth_ID == Send_bluetooth) 
		{
			return;
		}
	}
	id++;
	if(id<10)
	{
		goto re_send;
	}
}

void stop_play_music_timer(void)
{
	if(ble_play_state_timer)
		xTimerStop(ble_play_state_timer,0);
}
#endif
