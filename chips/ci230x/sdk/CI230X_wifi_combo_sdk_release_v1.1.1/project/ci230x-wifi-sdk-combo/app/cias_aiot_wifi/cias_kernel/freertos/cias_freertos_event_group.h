/**
 * @file   cias_freertos_event_group.h
 * @author zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_FREERTOS_EVENT_GROUP_H_
#define _CIAS_FREERTOS_EVENT_GROUP_H_

#include "FreeRTOS.h"
#include "event_groups.h"

#include "cias_freertos_common.h"

#ifdef __cplusplus
    extern "C"{
#endif

typedef struct cias_event_group
{
    EventGroupHandle_t  handle;
}cias_event_group_t;

typedef enum
{
    CIAS_EVENT_GROUP_TRUE = 0,
    CIAS_EVENT_GROUP_FALSE = -1,
}cias_event_group_status_t;

cias_status cias_event_group_create(cias_event_group_t *event_group);
unsigned int cias_event_group_wait_bits(cias_event_group_t *event_group, const unsigned int bits_to_wait, 
                                        const cias_event_group_status_t clear_on_exit, const cias_event_group_status_t wait_all_bits, unsigned int ticks_to_wait);
unsigned int cias_event_group_set_bits(cias_event_group_t *event_group, const unsigned int bits_to_set);
unsigned int cias_event_group_clear_bits(cias_event_group_t *event_group, const unsigned int bits_to_clear);


#ifdef __cplusplus
    }
#endif

#endif
