#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "system_msg_deal.h"
#include "prompt_player.h"
#include "ci_nvdata_manage.h"
#include "ci_log.h"
#include "crc.h"
#include "ci130x_gpio.h"
#include "ci130x_uart.h"
#include "ci130x_dma.h"
#include "ci130x_it.h"

#include "user_config.h"
#include "ble_user_config.h"
#include "exe_api.h"
#include "crc.h"
#include "exe_app.h"
#include "ci_ble_rf.h"
#include "rf_msg_deal.h"
static sys_msg_t Ble_send_msg;
extern rf_cb_funcs_t rf_cb_funcs; //射频发送和接收的回调函数
static bool send_flag;            //蓝牙数据发送标志
pbledataCallBack g_bledataIdle = NULL;

void dev_state_init(void)
{
}
void app_Regist_CallBack(pbledataCallBack dbleCB)
{
	g_bledataIdle = (pbledataCallBack)dbleCB;
}

/**
 * @brief 处理蓝牙/2.4G接收到的消息
 * @param send_data 芯片接收到的数据；
 * @param len       接收数据长度；
 */
void custom_rf_recv_data_handle(uint8_t* recv_data, uint8_t len)
{
    if (len > RF_LEN_MAX)
    {
      mprintf("ci_rf_recv_data_handle len error\r\n");
      return;
    }
    else
    {
        //用户根据自己协议实现
    }
}

/**
 * @brief 处理蓝牙/2.4G接收到的消息-只使用于启英物联加密交互，客户使用自己的私有协议，请使用custom_rf_recv_data_handle函数
 * @param send_data 芯片接收到的数据；
 * @param len       接收数据长度；
 */
void ci_rf_recv_data_handle(uint8_t* recv_data, uint8_t len)
{
    if (len > RF_LEN_MAX)
    {
      mprintf("ci_rf_recv_data_handle len error\r\n");
      return;
    }
    mprintf("recv_data : ");                //打印cias_rf_recv数据缓区接收到的数据，长度为recv_len
    for(int i = 0;i < len;i++)
    {
       if(g_bledataIdle)
        g_bledataIdle(recv_data[i]);
    }
    mprintf("\n");
}

void uart_send_asr(uint16_t cmd_id)
{
}

//按串口协议发送数据
void usr_send_asr_result(uint16_t cmd_id)
{
    int ret;
    switch(cmd_id)
    {   
        default:
            ret = 0;
        break;
    }
}
