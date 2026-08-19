#ifndef __CIAS_TUYA_IR_PROTOCOL_HANDLE_H__
#define __CIAS_TUYA_IR_PROTOCOL_HANDLE_H__

#include "FreeRTOS.h"
#include "cias_log.h"

void tuya_ir_ctrl_audio_cmd_handle(int cmd);
void tuya_ir_protocol_init(void);
int tuya_air_conditioner_timer_init(int value);
#endif   //__CIAS_TUYA_IR_PROTOCOL_HANDLE_H__