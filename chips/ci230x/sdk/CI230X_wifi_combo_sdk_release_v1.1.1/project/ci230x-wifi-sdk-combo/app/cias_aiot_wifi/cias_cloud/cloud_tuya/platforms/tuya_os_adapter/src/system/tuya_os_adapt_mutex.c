/**
 * @file tuya_os_adapt_mutex.c
 * @brief 互斥锁操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */
#define _UNI_MUTEX_GLOBAL
#include <string.h>
#include "tuya_os_adapt_mutex.h"
#include "tuya_os_adapter_error_code.h"
#include "osal/osal.h"
#include "tuya_hal_memory.h"

/***********************************************************
*************************micro define***********************
***********************************************************/


/***********************************************************
*************************variable define********************
***********************************************************/
static const TUYA_OS_MUTEX_INTF m_tuya_os_mutex_intfs = {
    .init    = tuya_os_adapt_mutex_create_init, 
    .lock    = tuya_os_adapt_mutex_lock,
    .unlock  = tuya_os_adapt_mutex_unlock,
    .release = tuya_os_adapt_mutex_release,
};


/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief tuya_os_adapt_mutex_create_init用于创建并初始化tuya mutex
 * 
 * @param[out] pMutexHandle 返回mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_create_init(MUTEX_HANDLE *pMutexHandle)
{
    OS_Mutex_t *mutex = NULL;

    if(!pMutexHandle)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    mutex = (OS_Mutex_t *)tuya_os_adapt_system_malloc(sizeof(OS_Mutex_t));
    if(mutex == NULL) {
        return OPRT_OS_ADAPTER_MALLOC_FAILED;
    }
    memset(mutex, 0, sizeof(OS_Mutex_t));
    if (OS_OK != OS_MutexCreate(mutex)) {
        return OPRT_OS_ADAPTER_MALLOC_FAILED;
    }
    *pMutexHandle = (MUTEX_HANDLE)mutex;

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_mutex_lock用于lock tuya mutex
 * 
 * @param[in] mutexHandle tuya mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_lock(const MUTEX_HANDLE mutexHandle)
{
    if(!mutexHandle)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    if (OS_OK != OS_MutexLock((OS_Mutex_t *)mutexHandle, OS_WAIT_FOREVER)) {
        return OPRT_OS_ADAPTER_MUTEX_LOCK_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_mutex_unlock用于unlock tuya mutex
 * 
 * @param[in] mutexHandle tuya mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_unlock(const MUTEX_HANDLE mutexHandle)
{
    if(!mutexHandle)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    if (OS_OK != OS_MutexUnlock((OS_Mutex_t *)mutexHandle)) {
        return OPRT_OS_ADAPTER_MUTEX_UNLOCK_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_mutex_release用于释放tuya mutex
 * 
 * @param[in] mutexHandle MUTEX_HANDLE tuya mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_release(const MUTEX_HANDLE mutexHandle)
{
    if(!mutexHandle)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    if (OS_OK != OS_MutexDelete((OS_Mutex_t *)mutexHandle)) {
        return OPRT_OS_ADAPTER_MUTEX_RELEASE_FAILED;
    }
    tuya_os_adapt_system_free(mutexHandle);
    return OPRT_OS_ADAPTER_OK;
}

//注册函数
int tuya_os_adapt_reg_mutex_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_MUTEX, (void*)&m_tuya_os_mutex_intfs);
}

