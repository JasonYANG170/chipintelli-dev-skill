/**
 * @file    cias_freertos_mutex.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#include "cias_freertos_mutex.h"
#include "cias_freertos_common.h"
#include "cias_freertos_debug.h"

/**
 * @brief Create and initialize a mutex object
 * @note A mutex can only be locked by a single thread at any given time.
 * @param[in] mutex Pointer to the mutex object
 * @retval cias_status, CIAS_OK on success
 */
cias_status cias_mutex_create(cias_mutex_t *mutex)
{
    if(mutex->handle != NULL)
    {
        return CIAS_ERR_PARAM;
    }
    mutex->handle = xSemaphoreCreateMutex();
    if(mutex->handle == NULL)
    {
        CIAS_PRINT_ERR("err creat mutex");
        return CIAS_FAIL;
    }
    return CIAS_OK;
}

/**
 * @brief Delete the mutex object
 * @param[in] mutex Pointer to the mutex object
 * @retval cias_status, CIAS_OK on success
 */
cias_status cias_mutex_delete(cias_mutex_t *mutex)
{
    if(mutex->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    vSemaphoreDelete(mutex->handle);
    mutex->handle = NULL;
    return CIAS_OK;
}

/**
 * @brief take the mutex object
 * @note A mutex can only be locked by a single thread at any given time. If
 *       the mutex is already locked, the caller will be blocked for the
 *       specified time duration.
 * @param[in] mutex Pointer to the mutex object
 * @param[in] waitMS The maximum amount of time (in millisecond) the thread
 *                   should remain in the blocked state to wait for the mutex
 *                   to become unlocked.
 *                   OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval cias_status, CIAS_OK on success
 */
cias_status cias_mutex_take(cias_mutex_t *mutex, cias_ticks_t ticks_to_wait)
{
    BaseType_t ret;
    if(mutex->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xSemaphoreTake(mutex->handle, ticks_to_wait);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("err mutex take");
        return CIAS_FAIL;
    }
    return CIAS_OK;
}

/**
 * @brief give the mutex object previously locked using OS_MutexLock()
 * @note The mutex should be unlocked from the same thread context from which
 *       it was locked.
 * @param[in] mutex Pointer to the mutex object
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_mutex_give(cias_mutex_t *mutex)
{
    BaseType_t ret;
    if(mutex->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xSemaphoreGive(mutex->handle);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("err mutex give");
        return CIAS_FAIL;
    }
    return CIAS_OK;
}


