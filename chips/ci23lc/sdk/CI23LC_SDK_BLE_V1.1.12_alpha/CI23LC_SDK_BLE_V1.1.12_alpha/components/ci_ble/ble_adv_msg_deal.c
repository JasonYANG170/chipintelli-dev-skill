#include "FreeRTOS.h" 
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "stdlib.h"
#include "stdio.h"
#include "ci_log.h"
#include "prompt_player.h"
#include "cias_ble_msg_deal.h"
#define PACKET_LEN 16
extern void *ble_msg_queue;

/**
 * @brief 处理接收到的遥控器消息
 * @param buf 接收到的数据；
 */
bool ci_ble_recv_adv_data_handle(unsigned char* buf)
{
	static uint16_t recv_counter = 0xffff;
	unsigned char rf_send_data[PACKET_LEN];
	uint16_t counter = buf[2]*256 + buf[3];

	if (recv_counter == counter) //帧号重复
		return 0;	
	recv_counter = counter;
/*  	for (size_t i = 0; i < 16; i++)
	{
		mprintf("%02X ",buf[i]);
	}
	mprintf(", %04x\r\n",recv_counter);  */
	
	switch (buf[1])
	{	
		case 0x78:            //清码
			break;

		case 0x79:            //遥控
			covent_data_adv_to_ble(rf_send_data, buf);
			ble_msg_V1_t *recv_ble_msg = (ble_msg_V1_t *)rf_send_data;
			BaseType_t err = xQueueSend(ble_msg_queue, recv_ble_msg, 0);  //发送消息到蓝牙消息接收任务处理 
			if (err != pdPASS)
			{
				mprintf("deal_ble_recv_msg send fail:%d,%s\n",__LINE__,__FUNCTION__);
			}
			break;	

		default:
			break;
	}
	return 0; 
}
 

//将遥控器键值转换为启英蓝牙通信协议
bool covent_data_adv_to_ble(unsigned char* rf_send_data, unsigned char* buf)
{
	memset(rf_send_data, 0, PACKET_LEN);
	rf_send_data[0] = 0xa5;//帧头
	rf_send_data[1] = 0x5a;
	rf_send_data[2] = 0x01;//协议版本
	rf_send_data[3] = 0x01;//厂商ID
	rf_send_data[4] = 0x08;//设备类型
	rf_send_data[5] = 0x01;//设备编号
	rf_send_data[6] = 0x11;//消息类型-功能高4位数据低4位
	rf_send_data[8] = 0;   //数据长度
	rf_send_data[9] = 0x01;

	uint8_t key_input = buf[7];
	rf_send_data[7] = (key_input+1)/2;
	rf_send_data[10] = 2-(key_input+1)%2; 

	memcpy(buf,rf_send_data,PACKET_LEN);
	for (int i = 0; i < PACKET_LEN; i++)
	{
		mprintf("%02X ",buf[i]);
	}
	mprintf("\r\n");
}
