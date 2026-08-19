#include "cias_freertos_common.h"

void *cias_calloc(size_t nmemb, size_t size)
{
    void *p = pvPortMalloc(nmemb * size);
    if (p)
        memset(p, 0, nmemb * size);
    return p;
}

void *cias_realloc(void *ptr, size_t size)
{
    void *res;

    if (!ptr)
        return pvPortMalloc(size);

    if (!size)
    {
        vPortFree(ptr);
        return NULL;
    }

    res = pvPortMalloc(size);
    if (size)
        memcpy(res, ptr, size);

    vPortFree(ptr);
    return res;
}