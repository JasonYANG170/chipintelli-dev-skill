#ifndef __FIRMWARE_OTA_PORT_H__
#define __FIRMWARE_OTA_PORT_H__

#include "sdk_default_config.h"
#include "ci130x_uart.h"

#define OTA_CMD_PORT    UART1
#if FIRMWARE_OTA_ENABEL
typedef enum
{
    NET_MSG_HEAD = 1,
    NET_MSG_DATE = 2,
    NET_MSG_ERR  = 0,
    NET_MSG_IDE  = 3,
}wifi_communicate_state_t;

typedef struct ci_uart_standard_head
{
    uint32_t magic;     /*帧头 定义为0x5a5aa5a5*/
    uint16_t checksum;  /*校验和*/
    uint16_t type;      /*命令类型*/
    uint16_t len;       /*数据有效长度*/
    uint16_t version;   /*版本信息*/
    uint32_t fill_data; /*填充数据，可以添加私有信息*/
}cias_data_standard_head_t;

// OTA信息-写入flash
typedef struct
{
    unsigned int cias_ota_chip_type;  // 芯片型号
    unsigned char cias_ota_uart_port; // OTA串口 0:uart0  1:uart1  2:uart2
    unsigned int cias_ota_flag_addr;  // OTA信息共享flash地址
} cias_ota_flag_t;

void firmware_ota_task_init(void);   //ota升级任务初始化，接收串口复位指令

#endif

#endif//__FIRMWARE_OTA_PORT_H__