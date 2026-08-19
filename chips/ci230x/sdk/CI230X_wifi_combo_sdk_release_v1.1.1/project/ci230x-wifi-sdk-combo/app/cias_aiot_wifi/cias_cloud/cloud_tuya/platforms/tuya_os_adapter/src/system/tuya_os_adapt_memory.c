/**
 * @file tuya_os_adapt_memory.c
 * @brief 内存操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */
 
#include "tuya_os_adapt_memory.h"
#include "osal/osal.h"

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/
static const TUYA_OS_MEMORY_INTF m_tuya_os_memory_intfs = {
    .malloc  = tuya_os_adapt_system_malloc, 
    .free    = tuya_os_adapt_system_free,
};


/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief tuya_os_adapt_system_malloc用于分配内存
 * 
 * @param[in]       size        需要分配的内存大小
 * @return  分配得到的内存指针
 */
void *tuya_os_adapt_system_malloc(const size_t size)
{
    if(size == 0) {
        return NULL;
    }
    return OS_Malloc(size);
}

/**
 * @brief tuya_os_adapt_system_free用于释放内存
 * 
 * @param[in]       ptr         需要释放的内存指针
 */
void tuya_os_adapt_system_free(void* ptr)
{
 if(ptr == NULL) {
        return;
    }
    OS_Free(ptr);
}

int tuya_os_adapt_reg_memory_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_MEMORY, (void *)&m_tuya_os_memory_intfs);
}

