/**
 * @file    cias_freertos_event_group.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#include "cias_freertos_event_group.h"
#include "cias_freertos_common.h"
#include "cias_freertos_debug.h"

cias_status cias_event_group_create(cias_event_group_t *event_group)
{
    if(event_group->handle != NULL)
    {
        return CIAS_ERR_NOMEM;
    }
    event_group->handle = xEventGroupCreate();
    if(event_group->handle == NULL)
    {
        CIAS_PRINT_ERR("event group create err\n");
        return CIAS_FAIL;
    }
    return CIAS_OK;
} 

unsigned int cias_event_group_wait_bits(cias_event_group_t *event_group, const unsigned int bits_to_wait, 
                                        const cias_event_group_status_t clear_on_exit, const cias_event_group_status_t wait_all_bits, unsigned int ticks_to_wait)
{
    unsigned int ret = 0;
    BaseType_t r_clear_on_exit = pdTRUE;
    BaseType_t r_wait_all_bits = pdTRUE;
    if(event_group->handle == NULL)
    {
        return CIAS_ERR_PARAM;
    }
    if(clear_on_exit == CIAS_EVENT_GROUP_FALSE)
    {
        r_clear_on_exit = pdFALSE;
    }
    if(wait_all_bits == CIAS_EVENT_GROUP_FALSE)
    {
        r_wait_all_bits = pdFALSE;
    }
    ret = xEventGroupWaitBits(event_group->handle, bits_to_wait, r_clear_on_exit, r_wait_all_bits, ticks_to_wait);
    return ret;
}

unsigned int cias_event_group_set_bits(cias_event_group_t *event_group, const unsigned int bits_to_set)
{
    if(event_group->handle == NULL)
    {
        CIAS_PRINT_ERR("evet_group set bits err\n");
        return CIAS_ERR_PARAM;
    }
    return xEventGroupSetBits(event_group->handle,bits_to_set);
}

// unsigned int cias_event_group_set_bits_isr(cias_event_group_t *event_group, const unsigned int bits_to_set,(BaseType_t *)pxHigherPriorityTaskWoken)
// {
//     if(event_group->handle == NULL)
//     {
//         CIAS_PRINT_ERR("evet_group set bits err\n");
//         return CIAS_ERR_PARAM;
//     }
//     return xEventGroupSetBitsFromISR(event_group->handle,bits_to_set, (BaseType_t *)pxHigherPriorityTaskWoken);
// }

unsigned int cias_event_group_clear_bits(cias_event_group_t *event_group, const unsigned int bits_to_clear)
{
    if(event_group->handle == NULL)
    {
        CIAS_PRINT_ERR("event_group clear bits err\n");
        return CIAS_ERR_PARAM;
    }
    return xEventGroupClearBits(event_group->handle,bits_to_clear);
}