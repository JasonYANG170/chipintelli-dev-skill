/**
 * @file asr_malloc_port.c
 * @brief 
 * @version 0.1
 * @date 2019-06-19
 * 
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 * 
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "ci_log.h"
#include "asr_malloc_port.h"
#include "ci_system_info.h"
#include "FreeRTOS.h"
#include "remote_api_for_bnpu.h"
#include "ci130x_nuclear_com.h"

static int asr_malloc_times = 0;
static int asr_free_times = 0;


void free_insram_bnpu(void* p)
{
    if(p)
    {
        REMOTE_CALL(free_in_host)(p);
    }
}


void* malloc_insram_bnpu(int size)
{
    if(size > 0)
    {
        char * ptr = NULL;
        ptr = (char*)REMOTE_CALL(malloc_in_host)(size);
        return ptr;
    }
    return NULL;
}


/**
 * @brief decoder lib used malloc function, can add some info for debug
 * 
 * @param size : malloc size
 * @return void* : malloc address,NULL is malloc failed
 */
void *decoder_port_malloc(int size)
{
#if 1
    asr_malloc_times++;
    if(size > 0)
    {
        char *ptr = NULL;
        ptr = (char*)pvPortMalloc(size);
        return ptr;
    }
    return NULL;
#else
    unsigned char *p = NULL;
    if(0 == size)
    {
        while(1);
    }
#if 1//堆空间是否被破坏检查
    //mprintf("#ASR S %d\n",size);
    p = malloc(size+12);
    //mprintf("#ASR M 0x%x\n",(p));
    //PRINT_MEM_STATUS();
    asr_malloc_times++;
    if(p)
    {
        *(unsigned int*)p = 0xAAAACDEF;
        *(unsigned int*)(p+4) = size;
        *(unsigned int*)(p+8+size) = 0x5555CDEF;
        return (void*)(p + 8);
    }
    else
    {
        //ci_loginfo(LOG_USER,"malloc failed size = %d !\n",size);
        //PRINT_MEM_STATUS();
        return NULL;
    }
#endif
    
#if 0//double free检测
    mprintf("#ASR S %d\n",size);
    p = malloc(size+12);
    mprintf("#ASR M 0x%x\n",(p));
    PRINT_MEM_STATUS();
    if (p)
    {
        *(unsigned int*)p = 1;
        *(unsigned int*)(p+4) = size;
        *(unsigned int*)(p+8+size) = 0x5555CDEF;
        return (void*)(p + 8);
    }
    else
    {
        ci_loginfo(LOG_USER,"malloc err!\n");
        PRINT_MEM_STATUS();
        while(1);
        return NULL;
    }
#endif
    
#if 0//统计malloc free次数
    //mprintf("#ASR S %d\n",size);
    p = malloc(size);
    //mprintf("#ASR M 0x%x\n",(p));
    //PRINT_MEM_STATUS();
    if (p)
    {
        asr_malloc_times++;
        return (void*)(p);
    }
    else
    {
        ci_loginfo(LOG_ASR_PCM_BUF,"malloc err!\n");
        mprintf("#ASR S %d\n",size);
        PRINT_MEM_STATUS();
        while(1);// or return NULL;
    }
#endif
#endif
}
/**
 * @brief decoder lib used malloc function, can add some info for debug
 * 
 * @param pp : free address pointer
 */
void decoder_port_free(void *pp)
{
#if 1
    asr_free_times++;
    if(pp)
    {
        vPortFree(pp);
    }
#else
#if 1//堆空间是否被破坏检查
    unsigned char *p = (unsigned char *)pp - 8;
    unsigned int size = *(unsigned int*)(p+4);
    //mprintf("#ASR F 0x%x\n",p);
    asr_free_times++;
    if ((*(unsigned int*)p != 0xAAAACDEF) || (*(unsigned int*)(p+8+size) != 0x5555CDEF))
    {
        //mprintf("dheap overflow p:0x%x, size:%0x e:%0x \n",p,*(unsigned int*)(p+4),*(unsigned int*)(p+8+size));
        while(1) __BKPT(0);
    }
    else
    {
        free(p);
    }
#endif
    
#if 0//double free检测
    unsigned char *p = (unsigned char *)pp - 8;
    unsigned int size = *(unsigned int*)(p+4);
    mprintf("#ASR F 0x%x\n",p);
    *(unsigned int*)p = *(unsigned int*)p  - 1;
    if (*(unsigned int*)p != 0)
    {
        mprintf("dheap overflow p:0x%x, size:%0x e:%0x \n",p,*(unsigned int*)(p+4),*(unsigned int*)(p+8+size));
        while(1);
    }
    else
    {
        free(p);
    }
#endif
    
#if 0//统计malloc free次数
    free(pp);
    asr_free_times++;
#endif
#endif
}




