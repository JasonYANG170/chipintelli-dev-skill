/**
 * @file    cias_freertos_common.h
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_FREERTOS_COMMON_H_
#define _CIAS_FREERTOS_COMMON_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
    extern "C"{
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define CIAS_WAIT_FOREVER	portMAX_DELAY
#define CIAS_WAIT_NONE		0

/**
 * 
 * @brief OS status definetion
 */
typedef enum
{
    CIAS_OK             = 0,    /*success*/
    CIAS_FAIL           = -1,   /*failure*/
    CIAS_ERR_NOMEM      = -2,   /*out of memory*/
    CIAS_ERR_PARAM      = -3,   /*invalid parameter*/
    CIAS_ERR_TIMEOUT    = -4,   /*timeout*/
}cias_status;

typedef uint32_t cias_ticks_t;

#define CIAS_TICK    configTICK_RATE_HZ

#define CIAS_MS_TO_TICKS(msec)   ((uint32_t)(msec) * (1000 / CIAS_TICK))

#define cias_malloc(l) pvPortMalloc(l)
#define cias_free(p)   vPortFree(p)
#define cias_delay(ticks)         vTaskDelay((TickType_t)ticks)

void *cias_calloc(size_t nmemb, size_t size);
void *cias_realloc(void *ptr, size_t size);
#ifdef __cplusplus
    }
#endif

#endif
