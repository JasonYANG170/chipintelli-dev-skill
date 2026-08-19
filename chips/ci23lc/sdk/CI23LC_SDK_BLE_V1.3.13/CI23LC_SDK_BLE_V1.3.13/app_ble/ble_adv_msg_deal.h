#ifndef __BLE_ADV_MSG_DEAL_H__
#define __BLE_ADV_MSG_DEAL_H__

#include "FreeRTOS.h" 
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "stdlib.h"
#include "stdio.h"
#include "ci_log.h"
#include "prompt_player.h"
#include "cias_ble_msg_deal.h"
#include "cias_fan_msg_deal.h"
#include "system_msg_deal.h"

typedef struct
{
    uint8_t play_type;  //1:命令词ID播放; 2:命令词字符串播放;
    uint32_t cmd_id;
    uint32_t select_index;
    char *cmd_str;
}sys_msg_ble_data_t;
#endif  //__BLE_ADV_MSG_DEAL_H__