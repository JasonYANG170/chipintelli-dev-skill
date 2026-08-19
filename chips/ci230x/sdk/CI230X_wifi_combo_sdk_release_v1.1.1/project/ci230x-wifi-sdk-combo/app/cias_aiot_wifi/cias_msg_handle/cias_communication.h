/*
 * @FileName:: 
 * @Author: hongchuan.wu
 * @Date: 2022-03-10 15:24:41
 * @LastEditTime: 2022-03-12 17:12:38
 * @Description: 底层的通讯端口实现（audio and wifi）
 */
#ifndef _COMMUNICATION_PORT_FOR_AUDIO_
#define _COMMUNICATION_PORT_FOR_AUDIO_

#include "cias_freertos_queue.h"


extern cias_queue_t cias_send_slave_msg_queue;
extern cias_queue_t cias_recv_slave_msg_queue;

int communication_port_init(void);
int32_t communication_send(int8_t*str,int32_t length);
int32_t communication_recv(uint8_t*addr,int32_t length);
void communication_rate_change(unsigned long rate);
void uart1_tx_mutex_init(void);
int communication_task_init(void);
#endif //_COMMUNICATION_PORT_FOR_AUDIO_