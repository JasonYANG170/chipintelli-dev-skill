/**
 * @file   cias_freertos_queue.h
 * @author zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_FREERTOS_QUEUE_H
#define _CIAS_FREERTOS_QUEUE_H

#include "FreeRTOS.h"
#include "cias_user_config.h"
#include "cias_freertos_common.h"
#include "os_queue.h"





#ifdef __cplusplus
    extern "C"{
#endif

/**
 * @brief Queue object definition
 */
typedef struct cias_queue
{
    QueueHandle_t   handle;
}cias_queue_t;

cias_status cias_queue_create(cias_queue_t *queue, unsigned int queue_length, unsigned int item_size);
cias_status cias_queue_delete(cias_queue_t *queue);
cias_status cias_queue_send(cias_queue_t *queue, const void *item, cias_ticks_t ticks_to_wait);
cias_status cias_queue_sendfront(cias_queue_t *queue, const void *item, cias_ticks_t ticks_to_wait);
cias_status cias_queue_sendback(cias_queue_t *queue, const void *item, cias_ticks_t ticks_to_wait);
cias_status cias_queue_receive(cias_queue_t *queue, void *item, cias_ticks_t ticks_to_wait);
unsigned int cias_queue_waiting(cias_queue_t *queue);
cias_status cias_queue_reset(cias_queue_t *queue);


#ifdef __cplusplus
    }
#endif

#endif
