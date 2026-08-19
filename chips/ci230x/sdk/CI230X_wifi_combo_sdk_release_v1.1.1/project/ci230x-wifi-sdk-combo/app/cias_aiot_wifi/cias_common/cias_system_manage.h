#ifndef __CIAS_SYSTEM_MANAGE_H__
#define __CIAS_SYSTEM_MANAGE_H__

#include "cias_common.h"
typedef struct
{   
    uint8_t ota_start_flag;
    uint8_t wifi_ota_start_flag;
    uint8_t tuya_ir_down_flag;
    uint8_t recv_slave_msg_deal_flag;
    uint8_t recv_slave_msg_task_flag;
    uint8_t send_slave_msg_task_flag;
    cias_task_t qcloud_main_handle;    //腾讯云任务handle
    cias_task_t recv_slave_msg_deal_handle;
    cias_task_t recv_slave_msg_task_handle;
    cias_task_t send_slave_msg_task_handle;
}cias_system_manage_param_t;

void cias_system_manage(void);
#endif   //__CIAS_SYSTEM_MANAGE_H__