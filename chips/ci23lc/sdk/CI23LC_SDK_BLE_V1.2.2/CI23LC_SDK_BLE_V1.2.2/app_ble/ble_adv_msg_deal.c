#include "FreeRTOS.h" 
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "stdlib.h"
#include "stdio.h"
#include "ci_log.h"
#include "prompt_player.h"
#include "cias_ble_msg_deal.h"
#define PACKET_LEN 5

/**
 * @brief 处理接收到的遥控器消息
 * @param buf 接收到的数据；
 */
const uint8_t cid[4] = {0x03, 0x09, 0x4c, 0x5a};
bool ci_ble_recv_adv_data_handle(unsigned char* buf)
{
	static uint16_t recv_counter = 0xff;
	uint8_t recv_data[PACKET_LEN];
 	if((memcmp(cid, &buf[8], 4) != 0) || (0xcc != buf[14]) || (recv_counter == buf[16])) //CID匹配,帧号不重复
		return 0; 

	recv_counter = buf[16];
	covent_data_adv_to_ble(recv_data, &buf[14]); 
	switch (recv_data[2])  //处理不同的2.4G遥控器按键键值
	{	
		case 6:
		prompt_play_by_cmd_id(2, -1, NULL, true);
		break;
		case 8:
		prompt_play_by_cmd_id(3, -1, NULL, true);
		break;
		case 7:
		prompt_play_by_cmd_id(11, -1, NULL, true);
		break;
		case 10:
		prompt_play_by_cmd_id(12, -1, NULL, true);
		break;
		case 11:
		prompt_play_by_cmd_id(14, -1, NULL, true);
		break;
		case 12:
		prompt_play_by_cmd_id(13, -1, NULL, true);
		break;
		case 23:
		prompt_play_by_cmd_id(10, -1, NULL, true);
		break;
		case 22:
		prompt_play_by_cmd_id(20, -1, NULL, true);
		break;
		case 24:
		prompt_play_by_cmd_id(21, -1, NULL, true);
		break;
		case 19:
		prompt_play_by_cmd_id(22, -1, NULL, true);
		break;
		case 18:
		prompt_play_by_cmd_id(25, -1, NULL, true);
		break;
		case 20:
		prompt_play_by_cmd_id(37, -1, NULL, true);
		break;
		default:
			break;
	}
	return 0; 
}
 

//将遥控器键值转换为启英蓝牙通信协议
bool covent_data_adv_to_ble(unsigned char* recv_data, unsigned char* buf)
{
	memset(recv_data, 0, PACKET_LEN);
	uint8_t secret_key = (buf[8] ^ buf[9]) + buf[2];
	for (int i = 0; i < PACKET_LEN; i++)
	{
		recv_data[i] = buf[3+i] ^ secret_key;
	}
	mprintf("address:0x%x%x; cmd:0x%x; cmd_type:0x%x, para:0x%x\r\n",
			recv_data[0], recv_data[1], recv_data[2], recv_data[3], recv_data[4]);
}

