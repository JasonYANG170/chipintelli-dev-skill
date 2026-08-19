#ifndef __CIAS_CIRCLE_QUEUE_H__
#define __CIAS_CIRCLE_QUEUE_H__
#include <stdbool.h>
#include <stdint.h>
#include "system_msg_deal.h"
#include "ci130x_gpio.h"
#include "semphr.h"


// 定义循环队列结构体
typedef struct
{
    uint8_t *pdata;
    uint32_t queue_size;       // 队列总长度size
    uint32_t queue_len;        //队列个数
    uint32_t item_size;        //队列元素长度
    uint32_t front;            //队头指针
    uint32_t rear;             //队尾指针
} CircularQueue;
#endif