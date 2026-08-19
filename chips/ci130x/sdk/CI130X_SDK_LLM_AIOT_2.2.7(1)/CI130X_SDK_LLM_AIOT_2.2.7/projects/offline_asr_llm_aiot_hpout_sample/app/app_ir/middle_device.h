#ifndef _MIDDLE_DEVICE_H
#define _MIDDLE_DEVICE_H

#include "command_info.h"
#include "system_msg_deal.h"
#include "user_config.h"
#if (UART_PROTOCOL_VER == 255)
#pragma pack(1)
typedef struct
{
    uint16_t header;
    uint16_t data_length;
    uint8_t msg_type;
    uint8_t msg_cmd;
    uint8_t msg_seq;
    uint8_t msg_data[255];
    /*uint16_t chksum; send add auto*/
    /*uint8_t tail; send add auto*/
}sys_msg_com_data_t;


#pragma pack()
#endif

void userapp_deal_asr_msg_ex(sys_msg_asr_data_t *asr_msg);//语音控制红外功能
void userapp_deal_com_msg_ex(sys_msg_com_data_t *com_msg);//串口控制红外功能
void userapp_deal_com_msg_semantic_id(int semantic_id);
#endif
