/**
 * @file    cias_ring_buffer.h
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */


#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <stdint.h>
#include "cias_freertos_mutex.h"


#ifdef __cplusplus
extern "C"
{
#endif



#define RETURN_ERR -1
#define RETURN_OK 0

/**************************************************************************
                    type define 
****************************************************************************/
/*need freertos mutex, data can't overlap, need copy times*/
typedef struct
{
    uint8_t *base_addr;
    uint32_t total_size;
    cias_mutex_t lock;

    uint32_t wp;
    uint32_t rp;
    uint32_t data_cnt;
}ci_ring_buffer_t;

cias_status ci_ring_buffer_init(ci_ring_buffer_t *rbuffer, uint32_t total_size);
int ci_ring_buffer_write(ci_ring_buffer_t *rbuffer, uint8_t *data_addr, uint32_t size);
int ci_ring_buffer_read(ci_ring_buffer_t *rbuffer, uint8_t *data_addr, uint32_t size);
uint32_t ci_ring_buffer_get_size(ci_ring_buffer_t *rbuffer);
int ci_ring_buffer_clear(ci_ring_buffer_t *rbuffer);
int ci_ring_buffer_free(ci_ring_buffer_t *rbuffer);

#ifdef __cplusplus
}
#endif

#endif


