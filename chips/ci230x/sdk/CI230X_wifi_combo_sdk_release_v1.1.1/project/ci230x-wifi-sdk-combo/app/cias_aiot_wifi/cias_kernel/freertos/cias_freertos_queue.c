/**
 * @file    cias_freertos_queue.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */
#include "FreeRTOS.h"

#include "cias_freertos_queue.h"
#include "cias_freertos_common.h"
#include "cias_freertos_debug.h"



/**
 * @brief Create and initialize a queue object
 * @param[in] queue Pointer to the queue object
 * @param[in] queueLen The maximum number of items that the queue can hold at
 *                     any one time.
 * @param[in] itemSize The size, in bytes, of each data item that can be stored
 *                     in the queue.
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_queue_create(cias_queue_t *queue, unsigned int queue_length, unsigned int item_size)
{
    queue->handle = xQueueCreate(queue_length, item_size);
    if(queue->handle == NULL)
    {
        CIAS_PRINT_ERR("creat queue err");
        return CIAS_FAIL;
    }
    return CIAS_OK;
}

/**
 * @brief Delete the queue object
 * @param[in] queue Pointer to the queue object
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_queue_delete(cias_queue_t *queue)
{
    if(queue->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    vQueueDelete(queue->handle);
    queue->handle = NULL;
    return CIAS_OK;
}

/**
 * @brief Send (write) an item to the back of the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the data to be copied into the queue.
 *                 The size of each item the queue can hold is set when the
 *                 queue is created, and that many bytes will be copied from
 *                 item into the queue storage area.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for space to become available on the
 *                   queue, should the queue already be full.
 *                   HAL_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_queue_send(cias_queue_t *queue, const void *item, cias_ticks_t ticks_to_wait)
{
    BaseType_t ret;
    if(queue->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xQueueSend(queue->handle,item,ticks_to_wait);
    if(ret != pdPASS)
    {
        // CIAS_PRINT_ERR("%s() fail @line %d send \r\n",__func__, __LINE__);
        return CIAS_FAIL;
    }
    return CIAS_OK;
}

cias_status cias_queue_sendfront(cias_queue_t *queue, const void *item, cias_ticks_t ticks_to_wait)
{
    BaseType_t ret;
    if(queue->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xQueueSendToFront(queue->handle,item,ticks_to_wait);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("%s() fail @line %d send \r\n",__func__, __LINE__);
        return CIAS_FAIL;
    }
    return CIAS_OK;
}

cias_status cias_queue_sendback(cias_queue_t *queue, const void *item, cias_ticks_t ticks_to_wait)
{
    BaseType_t ret;
    if(queue->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xQueueSendToBack(queue->handle,item,ticks_to_wait);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("%s() fail @line %d send \r\n",__func__, __LINE__);
        return CIAS_FAIL;
    }
    return CIAS_OK;
}

/**
 * @brief Receive (read) an item from the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the memory into which the received data will be
 *                 copied. The length of the buffer must be at least equal to
 *                 the queue item size which is set when the queue is created.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for data to become available on the
 *                   queue, should the queue already be empty.
 *                   HAL_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_queue_receive(cias_queue_t *queue, void *item, cias_ticks_t ticks_to_wait)
{
    BaseType_t ret;
    if(queue->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret = xQueueReceive(queue->handle, item, ticks_to_wait);
    if(ret != pdPASS)
    {
        // CIAS_PRINT_ERR("%s(0 fail @ line %d receive \r\n",__func__, __LINE__);
        return CIAS_FAIL;
    }
    return CIAS_OK;
}

unsigned int cias_queue_waiting(cias_queue_t *queue)
{
    BaseType_t ret;
    unsigned int wait_queue_size = 0;
    if(queue->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    wait_queue_size = uxQueueMessagesWaiting(queue->handle);
    return wait_queue_size;
}

cias_status cias_queue_reset(cias_queue_t *queue)
{
    BaseType_t ret;
    if(queue->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    ret =  xQueueReset(queue->handle);
    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("%s(0 fail @ line %d reset \r\n",__func__, __LINE__);
        return CIAS_FAIL;
    }
    return CIAS_OK;
}


/**
 * @brief 覆盖写入队列消息
 * 
 * @param xQueue 队列句柄
 * @param pvItemToQueue 消息
 * 
 * @retval AUDIO_PLAY_OS_SUCCESS 写入成功
 * @retval AUDIO_PLAY_OS_FAIL 写入失败
 */
cias_status cias_queue_overwrite(cias_queue_t *queue, const void *const pvItemToQueue)
{
    if(queue->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    if (pdFALSE == xQueueOverwrite(queue->handle, pvItemToQueue))
    {
        return CIAS_FAIL;
    }
    return CIAS_OK;
}