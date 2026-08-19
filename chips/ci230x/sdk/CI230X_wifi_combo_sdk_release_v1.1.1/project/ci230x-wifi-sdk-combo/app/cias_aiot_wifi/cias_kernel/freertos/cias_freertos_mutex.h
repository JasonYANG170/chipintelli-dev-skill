/**
 * @file   cias_freertos_mutex.h
 * @author zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_FREERTOS_MUTEX_H_
#define _CIAS_FREERTOS_MUTEX_H_

#include "FreeRTOS.h"
#include "semphr.h"

#include "cias_freertos_common.h"

#ifdef __cplusplus
    extern "C"{
#endif

/**
 * @brief Mutex object definition
 */
typedef struct cias_mutex 
{
	SemaphoreHandle_t	handle;
}cias_mutex_t;

cias_status cias_mutex_create(cias_mutex_t *mutex);
cias_status cias_mutex_delete(cias_mutex_t *mutex);
cias_status cias_mutex_take(cias_mutex_t *mutex, cias_ticks_t ticks_to_wait);
cias_status cias_mutex_give(cias_mutex_t *mutex);


#ifdef __cplusplus
    }
#endif

#endif