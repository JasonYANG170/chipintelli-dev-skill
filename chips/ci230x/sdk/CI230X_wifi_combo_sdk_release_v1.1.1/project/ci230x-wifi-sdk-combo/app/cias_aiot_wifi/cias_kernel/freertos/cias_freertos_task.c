/**
 * @file    cias_freertos_task.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#include "cias_freertos_task.h"
#include "cias_freertos_common.h"
#include "cias_freertos_debug.h"
#include "cias_log.h"
/**
 * @brief Create and start a thread
 *
 * This function starts a new thread. The new thread starts execution by
 * invoking entry(). The argument arg is passed as the sole argument of entry().
 *
 * @note After finishing execution, the new thread should call OS_ThreadDelete()
 *       to delete itself. Failing to do this and just returning from entry()
 *       will result in undefined behavior.
 *
 * @param[in] thread Pointer to the thread object
 * @param[in] name A descriptive name for the thread. This is mainly used to
 *                 facilitate debugging.
 * @param[in] entry Entry, which is a function pointer, to the thread function
 * @param[in] arg The sole argument passed to entry()
 * @param[in] priority The priority at which the thread will execute
 * @param[in] stackSize The number of bytes the thread stack can hold
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_task_create(cias_task_t * task, const char *name, 
                             cias_task_function_t func, void *arg, 
                             uint32_t priority, uint32_t stack_size)
{
    BaseType_t ret;
    // CIAS_LOG_HL("cias_task_create start :%s request %d KBytes memory---------------\r\n",name,  stack_size / 1024 * 4);
    if(task->handle != NULL)
    {
        return CIAS_ERR_PARAM;
    }
    
    ret = xTaskCreate(func, name, stack_size / sizeof(StackType_t), arg, priority,&task->handle);

    // CIAS_LOG_HL("********************>>>>>Heap left: %d <x> min:%d<<<<<********************\r\n",xPortGetFreeHeapSize() ,xPortGetMinimumEverFreeHeapSize());

    if(ret != pdPASS)
    {
        CIAS_PRINT_ERR("err create task %s\r\n",name);
        task->handle = NULL;
        return CIAS_FAIL;
    }
    return CIAS_OK;
}

/**
 * @brief Terminate the thread
 * @note Only memory that is allocated to a thread by the kernel itself is
 *       automatically freed when a thread is deleted. Memory, or any other
 *       resource, that the application (rather than the kernel) allocates
 *       to a thread must be explicitly freed by the application when the task
 *       is deleted.
 * @param[in] thread Pointer to the thread object to be deleted.
 *                   A thread can delete itself by passing NULL in place of a
 *                   valid thread object.
 * @retval OS_Status, OS_OK on success
 */
cias_status cias_task_delete(cias_task_t *task)
{
    if(task == NULL)
    {
        vTaskDelete(NULL);
        return CIAS_OK;
    }

    if(task->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    vTaskDelete(task->handle);
    task->handle = NULL;
    return CIAS_OK;
}

cias_status cias_task_suspend(cias_task_t *task)
{
    if(task == NULL)
    {
        vTaskSuspend(NULL);
        return CIAS_OK;
    }
    if(task->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    vTaskSuspend(task->handle);
    return CIAS_OK;
}

cias_status cias_task_resume(cias_task_t *task)
{
    if(task == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    if(task->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    vTaskResume(task->handle);
    return CIAS_OK;
}