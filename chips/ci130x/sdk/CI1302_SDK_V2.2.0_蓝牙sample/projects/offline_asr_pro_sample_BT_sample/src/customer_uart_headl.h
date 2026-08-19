/**
 * @file system_msg_deal.c
 * @brief  
 * @version V1.0.0
 * @date 2019.01.22
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _CUSTOMER_UART_HEAD_H_
#define _CUSTOMER_UART_HEAD_H_
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "command_info.h"
#define MSG_DATA_MAX_SIZE 10
#define SUPPORT_BLE_DATA_LEN 33
#define SUPPORT_APP_DATA_LEN 33


#ifdef __cplusplus
extern "C"
{
#endif
/************************************
         COM
*************************************/
typedef struct
{   
    unsigned char header0;//header
    unsigned char header1;
    unsigned char id;
    unsigned char cmd;
    unsigned char data0;
    unsigned char data1;
    unsigned char chksum;//check sum= add "header0~dta1"
    unsigned char end;
}sys_msg_com_data_t;

typedef struct
{   
	unsigned char header0;//header
	unsigned char header1;//header
	unsigned char Ver;//header
	unsigned char cmd;//header
	unsigned char Payload[SUPPORT_BLE_DATA_LEN];//header
	unsigned char chksum;//check sum= add "header0~dta1"
	unsigned char end;
}sys_app_data_t;

typedef struct
{   
	unsigned char header0;//header
	unsigned char header1;//header
	unsigned char id;//header
	unsigned char cmd;//header
	uint8_t data[SUPPORT_BLE_DATA_LEN]; 
	unsigned char chksum;//check sum= add "header0~dta1"
	unsigned char end;
}sys_bluetooth_data_t;


#ifdef __cplusplus
}
#endif
  
#endif


