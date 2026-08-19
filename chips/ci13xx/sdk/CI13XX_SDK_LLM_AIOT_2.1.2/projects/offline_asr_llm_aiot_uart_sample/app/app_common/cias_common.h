#ifndef __CIAS_COMMON_H__
#define __CIAS_COMMON_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"

#define CIAS_STANDARD_MAGIC  0x5a5aa5a5 //0x1a2aa3a4 
#define   cias_task_delay_ms(tick)    vTaskDelay((TickType_t)tick)

typedef struct ci_uart_standard_head
{
    uint32_t magic; /*帧头 定义为0x5a5aa5a5*/
    uint16_t checksum;  /*校验和*/
    uint16_t type;  /*命令类型*/
    uint16_t len;   /*数据有效长度*/
    uint16_t version;   /*版本信息*/
    uint32_t fill_data;     /*填充数据，可以添加私有信息*/
}cias_standard_head_t;

typedef enum{
    NET_MSG_HEAD = 1,
    NET_MSG_DATE = 2,
    NET_MSG_ERR  = 0,
    NET_MSG_IDE  = 3,
}cias_communicate_state_t;
#endif  //__CIAS_COMMON_H__
