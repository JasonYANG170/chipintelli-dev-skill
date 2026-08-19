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
#include "app_control.h"
#include "customer_control.h"
#include "user_data_save.h"
#include "status_share.h"
#if  SUPPORT_BLE_APP
#include "rf_msg_deal.h"

static int Ble_rx_state= BLE_APP_STATE_HEADER0;
static sys_msg_t Ble_send_msg;
static unsigned short diotlen =  0;
static unsigned short ddatacnt =  0;
unsigned char  ubleplayflag =  0;
unsigned char  openmachineflag =  0;
unsigned char  Teamachineaddwaterflag =  0;
unsigned char  TeamachineRefrigerationflag =  0;
unsigned char  Teamachineaddheatflag =  0;
unsigned char  Teamachineaddheat45flag =  0;
unsigned char  Teamachineaddheat65flag =  0;
unsigned char  Teamachineaddheat85flag =  0;
unsigned char  Teamachineaddheat100flag =  0;
unsigned char  Teamachineautowaterflag =  0;
unsigned char  Teamachineautoaddheatflag =  0;
unsigned char  Teamachinekeepwarmflag =  0;
static unsigned short ddatalen =  0;
static unsigned short dchecksum =  0;
unsigned char  g_auto_heating_function_flag =  0;
unsigned char  g_heating_adjust_flag =  0;
unsigned char  g_temprature_adjust_flag =  0;

TickType_t blelast_time =0;

int dbledataoff =0;
/******************************************************************************
  * @void customer_rx_idle(unsigned char rxdata)
  * @parameter:  serial data 
  * @return : none
  * @function : it's interface for receive customer data
*******************************************************************************/
void app_idle(unsigned char rxdata)
{     

	if((rxdata==BLE_APP_HEAD0)&&(Ble_rx_state !=BLE_APP_STATE_HEADER0))
	{
		if(( xTaskGetTickCount() -blelast_time)>pdMS_TO_TICKS(300))
		{
			Ble_rx_state = BLE_APP_STATE_HEADER0;
		}
	}
	switch(Ble_rx_state)
	{
		case BLE_APP_STATE_HEADER0:
		{
			if(rxdata==BLE_APP_HEAD0)
			{
				Ble_send_msg.msg_data.app_data.uheader0 = rxdata;
				dchecksum = rxdata;
				blelast_time =  xTaskGetTickCount();
				Ble_rx_state = BLE_APP_STATE_HEADER1;
			}
			else
			{
				mprintf("@@HEADER0error:%x\n",rxdata);
			}
			break;
		}
		case BLE_APP_STATE_HEADER1:
		{
			if(rxdata==BLE_APP_HEAD1)
			{
				Ble_send_msg.msg_data.app_data.uheader1 = rxdata;
				dchecksum += rxdata;
				Ble_rx_state = BLE_APP_STATE_VER;
				
			}
			else
			{
			
				Ble_rx_state = BLE_APP_STATE_HEADER0;
			}
			break;
		}
		case BLE_APP_STATE_VER:
		{
			Ble_send_msg.msg_data.app_data.uver= rxdata;
			Ble_rx_state = BLE_APP_STATE_CMD;
			break;
		}
		
		case BLE_APP_STATE_CMD:
		{
			dbledataoff = 0;
			Ble_send_msg.msg_data.app_data.ulen= rxdata;
			Ble_rx_state = BLE_APP_STATE_DATA1;
			break;
		}
		case BLE_APP_STATE_DATA1:
		{
			Ble_send_msg.msg_data.app_data.udata[dbledataoff++]= rxdata;
			Ble_rx_state = BLE_APP_STATE_DATA2;
			break;
		}
		case BLE_APP_STATE_DATA2:
		{
			Ble_send_msg.msg_data.app_data.udata[dbledataoff++] = rxdata;
			Ble_rx_state = BLE_APP_STATE_CHECK_SUM;
			break;
		}		
		case BLE_APP_STATE_CHECK_SUM:
		{
			Ble_send_msg.msg_data.app_data.uchksum= rxdata;
			Ble_rx_state = BLE_APP_STATE_END;
			break;
		}		
		case BLE_APP_STATE_END:
		{
			if(rxdata==BLE_APP_END)
			{
				Ble_send_msg.msg_data.app_data.uend= rxdata;
				Ble_rx_state = BLE_APP_STATE_HEADER0;
				Ble_send_msg.msg_type = SYS_MSG_TYPE_APP;
				send_msg_to_sys_task(&Ble_send_msg, NULL);
			}
			break;
		}
		default:
			break;
	}
}

int g_pringt_count=0;
/******************************************************************************
  * @void iot_deal_msg(sys_msg_iot_data_t *iot_rev_data)
  * @parameter:  sys_msg_iot_data_t *iot_rev_data 
  * @return : none
  * @function :处理WIFI 发的串口数据
*******************************************************************************/
void app_deal_msg(sys_app_data_t *app_data)
{
	uint32_t semantic_id=0;

	APPprintf("@@app_data->header0:%x,%x,%x\n",app_data->uheader0,app_data->uheader1,app_data->uend);
	if((app_data->uheader0 == BLE_APP_HEAD0)&&(app_data->uheader1 == BLE_APP_HEAD1)&&(app_data->uend == BLE_APP_END))
	{
		switch(app_data->udata[0])
		{
			case TEA_MACHINE_ONOFF:
			{
				if(openmachineflag == 0)
				{
					semantic_id = 6;
					openmachineflag =1;                                       
				}
				else
				{
					semantic_id = 7;
					openmachineflag =0;
					Teamachineaddheatflag =0;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;
					Teamachineautoaddheatflag =0;
					Teamachineaddwaterflag =0;
					TeamachineRefrigerationflag = 0;
					Teamachinekeepwarmflag =0;
					Teamachineautowaterflag =0;
				}
				break;
			}
			case TEA_MACHINE_ADD_WATER_ONOFF:
			{
				if(Teamachineaddwaterflag == 0)
				{
					semantic_id = 8;
					Teamachineaddwaterflag =1;
				}
				else
				{
					semantic_id = 9;
					Teamachineaddwaterflag =0;
				}
				break;
			}
			case TEA_MACHINE_HEATING_ONOFF:
			{
				if(Teamachineaddheatflag == 0)
				{
					semantic_id = 10;
					Teamachineaddheatflag =1;
				}
				else
				{
					semantic_id = 11;
					Teamachineaddheatflag =0;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;
					Teamachineautoaddheatflag =0;
					TeamachineRefrigerationflag = 0;
				}
				break;
			}
			case TEA_MACHINE_HEATING_TEMPRATURE_45_ONOFF:
			{
				if(Teamachineaddheat45flag == 0)
				{
					semantic_id = 13;
					Teamachineaddheat45flag =1;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;
				}
				else
				{
					semantic_id = 11;
					Teamachineaddheatflag =0;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;	
					Teamachineautoaddheatflag =0;
					TeamachineRefrigerationflag = 0;
				}
				break;
			}
			case TEA_MACHINE_HEATING_TEMPRATURE_65_ONOFF:
			{
				if(Teamachineaddheat65flag == 0)
				{
					semantic_id = 17;
					Teamachineaddheat65flag =1;
					Teamachineaddheat45flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;
				}
				else
				{
					semantic_id = 11;
					Teamachineaddheatflag =0;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;	
					Teamachineautoaddheatflag =0;
					TeamachineRefrigerationflag = 0;
				}
				break;
			}
			case TEA_MACHINE_HEATING_TEMPRATURE_85_ONOFF:
			{
				if(Teamachineaddheat85flag == 0)
				{
					semantic_id = 21;
					Teamachineaddheat85flag =1;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;					
					Teamachineaddheat100flag =0;
				}
				else
				{
					semantic_id = 11;
					Teamachineaddheatflag =0;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;	
					Teamachineautoaddheatflag =0;
					TeamachineRefrigerationflag = 0;                                      
				}
				break;
			}
			case TEA_MACHINE_HEATING_TEMPRATURE_100_ONOFF:
			{
				if(Teamachineaddheat100flag == 0)
				{
					semantic_id = 30;
					Teamachineaddheat100flag =1;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
				}
				else
				{
					semantic_id = 11;
					Teamachineaddheatflag =0;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;                                    
					Teamachineautoaddheatflag =0;
					TeamachineRefrigerationflag = 0;                                 
				}
				break;
			}
			case TEA_MACHINE_AUTO_HEATING_ONOFF:
			{
				
				if(Teamachineautoaddheatflag == 0)
				{
					semantic_id = 10;
					Teamachineautoaddheatflag =1;
				}
				else
				{
					semantic_id = 11;
					Teamachineautoaddheatflag =0;
					Teamachineaddheatflag =0;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;
					TeamachineRefrigerationflag = 0;                                      
				}
				break;
			}
			case TEA_MACHINE_ADD_ICE_WATER_ONOFF:
			{
				if(Teamachineautowaterflag == 0)
				{
					semantic_id = 24;
					Teamachineautowaterflag =1;
				}
				else
				{
					semantic_id = 25;
					Teamachineautowaterflag =0;
				}
				break;
			}
			case TEA_MACHINE_REFRIGERATION_ONOFF:
			{
				if(TeamachineRefrigerationflag == 0)
				{
					semantic_id = 28;
					TeamachineRefrigerationflag =1;
				}
				else
				{
					semantic_id = 29;
					Teamachineautoaddheatflag =0;
					Teamachineaddheatflag =0;
					Teamachineaddheat45flag =0;
					Teamachineaddheat65flag =0;
					Teamachineaddheat85flag =0;
					Teamachineaddheat100flag =0;
					TeamachineRefrigerationflag =0;
				}
				break;
			}			
			case TEA_MACHINE_KEEP_WARM_ONOFF:
			{
				if(Teamachinekeepwarmflag == 0)
				{
					semantic_id = 26;
					Teamachinekeepwarmflag =1;
				}
				else
				{
					semantic_id = 27;
					Teamachinekeepwarmflag =0;
				}
				break;
			}
			case TEA_MACHINE_OPEN_SCREEN:
			{
				semantic_id = 18;
				return;
			}
			case TEA_MACHINE_CLOSE_SCREEN:
			{
				semantic_id = 19;
				return;
			}
			case TEA_MACHINE_OPEN_BOIL_TEA:
			{
				semantic_id = 20;
				return;
			}
			case TEA_MACHINE_CLOSE_BOIL_TEA:
			{
				semantic_id = 21;
				return;
			}
			case TEA_MACHINE_OPEN_BUBBLE_MILE:
			{
				semantic_id = 22;
				return;
			}
			case TEA_MACHINE_CLOSE_BUBBLE_MILE:
			{
				semantic_id = 23;
				return;
			}
			case TEA_MACHINE_OPEN_BOIL_COFFE:
			{
				semantic_id = 24;
				return;
			}
			case TEA_MACHINE_CLOSE_BOIL_COFFE:
			{
				semantic_id = 25;
				return;
			}
			case TEA_MACHINE_ADJUST_TEMPRATURE_DATA:
			{
				switch(app_data->udata[1])
				{
					case 40:
					{
						semantic_id = 12;
						break;                                
					}
					case 45:
					{
						semantic_id = 13;
						break;                                
					}
					case 50:
					{
						semantic_id = 14;
						break;                                
					}
					case 55:
					{
						semantic_id = 15;
						break;                                
					} 
					case 60:
					{
						semantic_id = 16;
						break;                                
					}
					case 65:
					{
						semantic_id = 17;
						break;                                
					}
					case 70:
					{
						semantic_id = 18;
						break;                                
					}
					case 75:
					{
						semantic_id = 19;
						break;                                
					}
					case 80:
					{
						semantic_id = 20;
						break;                                
					}
					case 85:
					{
						semantic_id = 21;
						break;                                
					}
					case 90:
					{
						semantic_id = 22;
						break;                                
					}
					case 95:
					{
						semantic_id = 23;
						break;                                
					}
					case 100:
					{
						semantic_id = 30;
						break;                                
					}
					default:
					break;
				}
				break;
			}
			default:
				return;
		}	
		ubleplayflag =  1;
		uart_send_customer_cmd(semantic_id);
		#if USER_ASR_CMD_PLAY
		semantic_id = 0x9c00|semantic_id;
		prompt_play_by_semantic_id(semantic_id, -1, false);
		#endif
	}
}
/***************************************************************************************
* @void Init_Customer_Paramter_If(void)
* @parameter: none
* @return :    none
* @function : use init customer special parameter 
*******************************************************************************************/
void Init_ble_Paramter_If(void)
{
	Ble_rx_state = BLE_APP_STATE_HEADER0;
	app_Regist_CallBack((pbledataCallBack)app_idle);
}
#endif
