/**
 * @file    cias_freertos_timer.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#include "cias_freertos_timer.h"
#include "cias_freertos_common.h"
#include "cias_freertos_debug.h"

static void cias_timer_priv_callback(TimerHandle_t xtimer)
{
    cias_timer_callback_data_t *priv;
    priv = pvTimerGetTimerID(xtimer);

    if(priv && priv->callback)
    {
        priv->callback(priv->argument);
    }
}

/**
 * @brief Create and initialize a timer object
 *
 * @note Creating a timer does not start the timer running. The OS_TimerStart()
 *       and OS_TimerChangePeriod() API functions can all be used to start the
 *       timer running.
 *
 * @param[in] timer Pointer to the timer object
 * @param[in] type Timer type
 * @param[in] cb Timer expire callback function
 * @param[in] arg Argument of Timer expire callback function
 * @param[in] periodMS Timer period in milliseconds
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_timer_create(cias_timer_t *timer, cias_timertype type,
                            cias_timer_callback cb, void *arg, cias_ticks_t period)
{
    cias_timer_callback_data_t *priv;
    if(timer->handle != NULL)
    {
        return CIAS_ERR_PARAM;
    }
    priv = cias_malloc(sizeof(cias_timer_callback_data_t));
    if(priv == NULL)
    {
        return CIAS_ERR_NOMEM;
    }
    priv->callback = cb;
    priv->argument = arg;

    timer->handle = xTimerCreate(timer->timer_name,
                                 period,
                                 type == CIAS_TIMER_PERIODIC ? pdTRUE:pdFALSE,
                                 priv,
                                 cias_timer_priv_callback);
    if(timer->handle == NULL)
    {
        CIAS_PRINT_ERR("err creat timer handler %s\r\n",timer->timer_name);
        cias_free(priv);
        return CIAS_FAIL;
    }
    timer->priv = priv;
    return CIAS_OK;
}

/**
 * @brief Delete the timer object
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_timer_delete(cias_timer_t *timer)
{
    BaseType_t ret;
    if(timer->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }

    ret = xTimerDelete(timer->handle,CIAS_WAIT_FOREVER);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("err timer delete %s\r\n",timer->timer_name);
        return CIAS_FAIL;
    }
    timer->handle = NULL;
    cias_free(timer->priv);
    timer->priv = NULL;
    return CIAS_OK;
}

/**
 * @brief Start a timer running.
 * @note If the timer is already running, this function will re-start the timer.
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_timer_start(cias_timer_t *timer, cias_ticks_t ticks_to_wait)
{
    BaseType_t ret;
    if(timer->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xTimerStart(timer->handle, ticks_to_wait);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("err start timer %s\r\n",timer->timer_name);
        return CIAS_FAIL;
    }
    return CIAS_OK;
}

/**
 * @brief Change the period of a timer
 *
 * If OS_TimerChangePeriod() is used to change the period of a timer that is
 * already running, then the timer will use the new period value to recalculate
 * its expiry time. The recalculated expiry time will then be relative to when
 * OS_TimerChangePeriod() was called, and not relative to when the timer was
 * originally started.

 * If OS_TimerChangePeriod() is used to change the period of a timer that is
 * not already running, then the timer will use the new period value to
 * calculate an expiry time, and the timer will start running.
 *
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_timer_change_period(cias_timer_t *timer, uint32_t new_period_ms)
{
    BaseType_t ret;
    if(timer->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xTimerChangePeriod(timer->handle,new_period_ms,0);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("err change period %s\r\n",timer->timer_name);
        return CIAS_FAIL;
    }
    return CIAS_OK;

}

/**
 * @brief Stop a timer running.
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_timer_stop(cias_timer_t *timer, cias_ticks_t ticks_to_wait)
{
    BaseType_t ret;
    if(timer->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xTimerStop(timer->handle,ticks_to_wait);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("err stop timer %s\r\n",timer->timer_name);
        return CIAS_FAIL;
    }
    return CIAS_OK;
}



