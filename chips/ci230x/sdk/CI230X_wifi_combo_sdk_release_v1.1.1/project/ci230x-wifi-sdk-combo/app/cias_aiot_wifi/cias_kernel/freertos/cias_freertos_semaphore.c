/**
 * @file   cias_freertos_semaphore.c
 * @author zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#include "cias_freertos_semaphore.h"
#include "cias_freertos_common.h"
#include "cias_freertos_debug.h"

/**
 * @brief Create and initialize a binary semaphore object
 * @note A binary semaphore is equal to a counting semaphore created by calling
         OS_SemaphoreCreate(sem, 0, 1).
 * @param[in] sem Pointer to the semaphore object
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_semaphore_create_binary(cias_semaphore_t *sem)
{
    sem->handle = xSemaphoreCreateBinary();
    if(sem->handle ==NULL)
    {
        CIAS_PRINT_ERR("err create binary\r\n");
        return CIAS_FAIL;
    }
    return CIAS_OK;
}


/**
 * @brief Delete the semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_semaphore_delete(cias_semaphore_t *sem)
{
    if(sem->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    vSemaphoreDelete(sem->handle);
    sem->handle = NULL;
    return CIAS_OK;
}

/**
 * @brief Wait until the semaphore object becomes available
 * @param[in] sem Pointer to the semaphore object
 * @param[in] waitMS The maximum amount of time (in millisecond) the thread
 *                   should remain in the blocked state to wait for the
 *                   semaphore to become available.
 *                   HAL_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_semaphore_take(cias_semaphore_t *sem, cias_ticks_t ticks_to_wait)
{
    BaseType_t ret;

    if(sem->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }

    ret = xSemaphoreTake(sem->handle,ticks_to_wait);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("%s() fail @line %d\r\n",__func__, __LINE__);
    }
    return CIAS_OK;
}

/**
 * @brief Release the semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_semaphore_give(cias_semaphore_t *sem)
{
    BaseType_t ret;
    if(sem->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xSemaphoreGive(sem->handle);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("%s() fail @line %d\r\n",__func__, __LINE__);
    }
    return CIAS_OK;
}

