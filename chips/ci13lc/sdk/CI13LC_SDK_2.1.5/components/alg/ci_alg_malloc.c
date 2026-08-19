#include <string.h>
#include "romlib_api.h"
#include "ci_alg_malloc.h"
#include "ci_assert.h"
#include "FreeRTOS.h"
#include "task.h"


static uint32_t alg_calloc_size = 0;
uint32_t get_alg_calloc_size(void)
{
    return alg_calloc_size;
}

void *ci_algbuf_calloc(size_t size_in_byte, size_t size_of_byte)
{
    void *addr;
    alg_calloc_size += size_in_byte * size_of_byte;
    
    //addr = pvPortCalloc(size_in_byte, size_of_byte);
    #if 0
    addr = (void *)pvPortMalloc(size_in_byte * size_of_byte);
    #else
    addr = (void *)malloc(size_in_byte * size_of_byte);
    #endif

    CI_ASSERT(addr,"\n");
    
    memset(addr, 0, size_in_byte * size_of_byte);
    return addr;
}

static uint32_t alg_malloc_size = 0;
uint32_t get_alg_malloc_size(void)
{
    return alg_malloc_size;
}

void *ci_algbuf_malloc(size_t size_in_byte)
{
    void *addr;
    alg_malloc_size += size_in_byte;
    #if 0
    addr = (void *)pvPortMalloc(size_in_byte);
    #else
    addr = (void *)malloc(size_in_byte);
    #endif

    CI_ASSERT(addr,"\n");
    return addr;
}

void ci_algbuf_free(void *buf)
{
    CI_ASSERT(buf,"\n");
    #if 0
    vPortFree(buf);
    #else
    free( buf );
    #endif
}
