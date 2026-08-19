
#include "ble_adv_msg_deal.h"
#include "user_config.h"
#define PACKET_LEN 5

/**
 * @brief 处理接收到的遥控器消息
 * @param buf 接收到的数据；
 */
const uint8_t cid[4] = {0x03, 0x09, 0x4c, 0x5a};

USE_XFI bool ci_ble_recv_adv_data_handle(unsigned char* buf)
{
	static uint16_t recv_counter = 0xff;
	uint8_t recv_data[PACKET_LEN];
 	if((memcmp(cid, &buf[8], 4) != 0) || (0xcc != buf[14]) || (recv_counter == buf[16])) //CID匹配,帧号不重复
		return 0; 

	recv_counter = buf[16];
	// covent_data_adv_to_ble(recv_data, &buf[14]); 
	uint8_t recv_cmd = buf[19];
	uint32_t cmd_id = NULL;
	uint32_t select_index = 0;
	extern fan_dev_t fan_dev;
	switch (recv_cmd)  //处理不同的2.4G遥控器按键键值
	{	
		case 1:
		cmd_id = TURN_ON;
		break;
		case 2:
		cmd_id = TURN_OFF;
		break;
		case 3:
		cmd_id = LIGHT_RAISE;
		break;
		case 4:
		cmd_id = SPEED_RAISE;
		break;
		case 5:
		if(fan_dev.lamp == FUN_TURN_OFF)
			cmd_id = LAMP_ON;
		else
			cmd_id = LAMP_OFF;
		select_index = 1;
		break;
		case 6:
		cmd_id = SPEED_REDUCE;
		break;
		case 7:
		cmd_id = LIGHT_REDUCE;
		break;
		case 8:
		cmd_id = LAMP_ON;
		break;
		case 9:
		cmd_id = WARM_LAMP_ON;
		break;
		case 10:
		cmd_id = TIMING_OFF;
		break;
		case 11:
		if(fan_dev.timing == FUN_TURN_OFF)
		{
			cmd_id = TIMMING_1H;
		}else{
			if(--fan_dev.timing <= DEV_TIMING_MIN)
				cmd_id = TIMMING_1H;
			else
				cmd_id = fan_dev.timing-DEV_TIMING_MIN + TIMMING_1H;
		}
		break;
		case 12:
		if(fan_dev.timing == FUN_TURN_OFF)
		{
			cmd_id = TIMMING_1H;
		}else{
			if(++fan_dev.timing >= DEV_TIMING_MAX)
				cmd_id = TIMMING_8H;
			else
				cmd_id = fan_dev.timing-DEV_TIMING_MIN + TIMMING_1H;
		}
		break;
		default:
			break;
	}
	if(cmd_id)
	{
		fan_report(cmd_id);
        sys_msg_t sys_msg;
        sys_msg.msg_type = SYS_MSG_TYPE_BLE;
        sys_msg_ble_data_t *msg_data = (sys_msg_ble_data_t*)sys_msg.msg_data; 
		msg_data->play_type = 1;
        msg_data->cmd_id = cmd_id;
        msg_data->select_index = select_index;

        send_sys_msg_inner(&sys_msg, sizeof(sys_msg), NULL);
	}
	return 0; 
}
 

//将遥控器键值转换为启英蓝牙通信协议
USE_XFI bool covent_data_adv_to_ble(unsigned char* recv_data, unsigned char* buf)
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

