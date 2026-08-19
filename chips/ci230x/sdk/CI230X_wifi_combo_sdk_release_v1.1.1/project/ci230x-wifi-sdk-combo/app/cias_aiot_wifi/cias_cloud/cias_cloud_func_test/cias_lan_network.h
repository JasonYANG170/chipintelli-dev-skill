/*
 * @Description: 
 * @Author: hongchuan.wu
 * @Date: 2021-12-09 16:36:48
 * @LastEditTime: 2022-10-12 14:35:17
 * @LastEditors: Please set LastEditors
 * @Reference: 
 */


#ifndef __CIAS_LAN_NETWORK_H__
#define __CIAS_LAN_NETWORK_H__

#include "cias_freertos_queue.h"

typedef struct 
{
    uint8_t data[300];
    uint32_t len;

}lan_data_t;

extern cias_queue_t ci_lan_network_test_queue;


void ci_lan_network_test_fun(void);
int ci_lan_network_test_task(void);

#endif //__CIAS_LAN_NETWORK_H__