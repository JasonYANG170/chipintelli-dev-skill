/**
 * @file    cias_ring_buffer.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */
#include <stdint.h>
#include <string.h>

#include "cias_freertos_common.h"
#include "cias_freertos_debug.h"
// #include "cias_freertos_mutex.h"
#include "cias_ring_buffer.h"

cias_status ci_ring_buffer_init(ci_ring_buffer_t *rbuffer, uint32_t total_size)
{
    cias_mutex_create(&rbuffer->lock);

    rbuffer->base_addr = cias_malloc(total_size);
    if (rbuffer->base_addr == NULL)
    {
        CIAS_PRINT_ERR("cias_malloc:malloc fail\n");
        return CIAS_ERR_NOMEM;
    }

    rbuffer->total_size = total_size;

    rbuffer->data_cnt = 0;
    rbuffer->wp = 0;
    rbuffer->rp = 0;

    return CIAS_OK;
}

int ci_ring_buffer_write(ci_ring_buffer_t *rbuffer, uint8_t *data_addr, uint32_t size)
{
    uint32_t copy_lens;
    uint8_t *copy_addr;

    cias_mutex_take(&rbuffer->lock, CIAS_WAIT_FOREVER);
    if ((rbuffer->data_cnt + size) > (rbuffer->total_size))
    {
        cias_mutex_give(&rbuffer->lock);
        return CIAS_ERR_NOMEM;
    }
    if ((rbuffer->wp + size) > rbuffer->total_size)
    {
        copy_lens = rbuffer->total_size - rbuffer->wp;
        memcpy((void *)(rbuffer->base_addr + rbuffer->wp), (void *)data_addr, copy_lens);
        copy_addr = data_addr + copy_lens;
        copy_lens = size - copy_lens;
        memcpy((void *)(rbuffer->base_addr), (void *)copy_addr, copy_lens);
    }
    else
    {
        memcpy((void *)(rbuffer->base_addr + rbuffer->wp), (void *)data_addr, size);
    }

    rbuffer->wp += size;
    if (rbuffer->wp >= rbuffer->total_size)
    {
        rbuffer->wp -= rbuffer->total_size;
    }

    rbuffer->data_cnt += size;
    cias_mutex_give(&rbuffer->lock);
    return CIAS_OK;
}

int ci_ring_buffer_read(ci_ring_buffer_t *rbuffer, uint8_t *data_addr, uint32_t size)
{
    uint32_t copy_lens;
    uint8_t *copy_addr;

    cias_mutex_take(&rbuffer->lock, CIAS_WAIT_FOREVER);
    if (rbuffer->data_cnt <= 0)
    {
        cias_mutex_give(&rbuffer->lock);
        return CIAS_FAIL;
    }

    if (size >= rbuffer->data_cnt)
    {
        size = rbuffer->data_cnt;
        if ((rbuffer->rp + size) > rbuffer->total_size)
        {
            copy_lens = rbuffer->total_size - rbuffer->rp;
            memcpy((void *)data_addr, (void *)(rbuffer->base_addr + rbuffer->rp), copy_lens);
            copy_addr = data_addr + copy_lens;
            copy_lens = size - copy_lens;
            memcpy((void *)(copy_addr), (void *)(rbuffer->base_addr), copy_lens);
        }
        else
        {
            memcpy((void *)data_addr, (void *)(rbuffer->base_addr + rbuffer->rp), size);
        }

        rbuffer->rp += rbuffer->data_cnt;
        rbuffer->data_cnt = 0;
        if (rbuffer->rp >= rbuffer->total_size)
        {
            rbuffer->rp -= rbuffer->total_size;
        }
        cias_mutex_give(&rbuffer->lock);
        return size;
    }

    if ((rbuffer->rp + size) > rbuffer->total_size)
    {
        copy_lens = rbuffer->total_size - rbuffer->rp;
        memcpy((void *)data_addr, (void *)(rbuffer->base_addr + rbuffer->rp), copy_lens);
        copy_addr = data_addr + copy_lens;
        copy_lens = size - copy_lens;
        memcpy((void *)(copy_addr), (void *)(rbuffer->base_addr), copy_lens);
    }
    else
    {
        memcpy((void *)data_addr, (void *)(rbuffer->base_addr + rbuffer->rp), size);
    }

    rbuffer->rp += size;
    if (rbuffer->rp >= rbuffer->total_size)
    {
        rbuffer->rp -= rbuffer->total_size;
    }

    rbuffer->data_cnt -= size;

    cias_mutex_give(&rbuffer->lock);
    return size;
}

uint32_t ci_ring_buffer_get_size(ci_ring_buffer_t *rbuffer)
{
    return rbuffer->data_cnt;
}

int ci_ring_buffer_clear(ci_ring_buffer_t *rbuffer)
{
    if (NULL == rbuffer->lock.handle)
    {
        CIAS_PRINT_ERR("ring buffer lock error\n");
        return CIAS_FAIL;
    }

    memset(rbuffer->base_addr, 0x00, rbuffer->data_cnt);

    rbuffer->data_cnt = 0;
    rbuffer->wp = 0;
    rbuffer->rp = 0;

    return RETURN_OK;
}

int ci_ring_buffer_free(ci_ring_buffer_t *rbuffer)
{
    if (NULL == rbuffer->lock.handle)
    {
        CIAS_PRINT_ERR("ring buffer lock error\n");
        return CIAS_FAIL;
    }

    if (rbuffer->base_addr)
    {
        cias_free(rbuffer->base_addr);
        rbuffer->base_addr = NULL;
    }

    rbuffer->data_cnt = 0;
    rbuffer->wp = 0;
    rbuffer->rp = 0;

    return CIAS_OK;
}
