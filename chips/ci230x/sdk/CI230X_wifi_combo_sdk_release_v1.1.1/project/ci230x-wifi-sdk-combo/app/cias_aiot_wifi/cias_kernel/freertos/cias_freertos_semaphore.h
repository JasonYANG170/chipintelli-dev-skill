/**
 * @file   cias_freertos_semaphore.h
 * @author zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_FREERTOS_SEMAPHORE_H
#define _CIAS_FREERTOS_SEMAPHORE_H

#include "FreeRTOS.h"
#include "semphr.h"

#include "cias_freertos_common.h"

#ifdef __cplusplus
    extern "C"{
#endif

/**
 * @brief Semaphore object definition
 */
typedef struct cias_semaphore
{
    SemaphoreHandle_t   handle;
}cias_semaphore_t;

cias_status cias_semaphore_create_binary(cias_semaphore_t *sem);
cias_status cias_semaphore_delete(cias_semaphore_t *sem);
cias_status cias_semaphore_take(cias_semaphore_t *sem, cias_ticks_t ticks_to_wait);
cias_status cias_semaphore_give(cias_semaphore_t *sem);
#ifdef __cplusplus
    }
#endif

#endif