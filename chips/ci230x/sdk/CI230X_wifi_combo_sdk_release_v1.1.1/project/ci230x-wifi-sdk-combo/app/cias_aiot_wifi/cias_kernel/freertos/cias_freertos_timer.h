/**
 * @file   cias_freertos_timer.h
 * @author zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_FREERTOS_TIMER_H_
#define _CIAS_FREERTOS_TIMER_H_

#include "FreeRTOS.h"
#include "timers.h"

#include "cias_freertos_common.h"

#ifdef __cplusplus
    extern "C"{
#endif

/**
 * @brief Timer type definition
 *     - one shot timer: Timer will be in the dormant state after it expires.
 *     - periodic timer: Timer will auto-reload after it expires.
 */
typedef enum 
{
    CIAS_TIMER_ONCE       = 0, /* one shot timer */
    CIAS_TIMER_PERIODIC   = 1  /* periodic timer */
}cias_timertype;

typedef void (*cias_timer_callback)(void *arg);


/**
 * @brief Timer expire callback data definition
 */
typedef struct cias_timer_callback_data 
{
    cias_timer_callback  callback;   /* Timer expire callback function */
    void                *argument;  /* Argument of timer expire callback function */
} cias_timer_callback_data_t;

/**
 * @brief Timer object definition
 */
typedef struct cias_timer 
{
    TimerHandle_t           handle;
    cias_timer_callback_data_t  *priv;	/* private data for internally usage */
    char *timer_name;
}cias_timer_t;


cias_status cias_timer_create(cias_timer_t *timer, cias_timertype type,
                            cias_timer_callback cb, void *arg, cias_ticks_t period);
cias_status cias_timer_delete(cias_timer_t *timer);
cias_status cias_timer_start(cias_timer_t *timer, cias_ticks_t ticks_to_wait);
cias_status cias_timer_change_period(cias_timer_t *timer, uint32_t new_period_ms);
cias_status cias_timer_stop(cias_timer_t *timer, cias_ticks_t ticks_to_wait);

#ifdef __cplusplus
    }
#endif

#endif
