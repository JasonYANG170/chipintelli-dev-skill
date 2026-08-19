/*
 * @FileName:: 
 * @Author: hongchuan.wu
 * @Date: 2022-03-10 10:42:02
 * @LastEditTime: 2022-03-12 17:14:35
 * @Description: 
 */
/**
 * @file   cias_freertos_task.h
 * @author zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_FREERTOS_TASK_H
#define _CIAS_FREERTOS_TASK_H

#include "FreeRTOS.h"
#include "task.h"

#include "cias_freertos_common.h"
#ifdef __cplusplus
    extern "C"{
#endif


#define CIAS_APP_PRIORITY                          5

/** @brief Thread entry definition, which is a pointer to a function */
typedef TaskFunction_t cias_task_function_t;

/** @brief Thread handle definition */
typedef TaskHandle_t cias_task_handle_t;

/**
 * @brief Thread object definition
 */
typedef struct cias_task 
{
    cias_task_handle_t   handle;
}cias_task_t;

cias_status cias_task_create(cias_task_t * task, const char *name, 
                             cias_task_function_t func, void *arg, 
                             uint32_t priority, uint32_t stack_size);
cias_status cias_task_delete(cias_task_t *task);
cias_status cias_task_suspend(cias_task_t *task);

#ifdef __cplusplus
    }
#endif

#endif