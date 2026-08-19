/**
 * @file tuya_os_adapt_semaphore.c
 * @brief semaphore相关接口封装
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#define _UNI_SEMAPHORE_GLOBAL

#include "tuya_os_adapt_semaphore.h"
#include "tuya_os_adapter_error_code.h"
#include "osal/osal.h"
#include "tuya_os_adapt_memory.h"

/***********************************************************
*************************micro define***********************
***********************************************************/


/***********************************************************
*************************variable define********************
***********************************************************/
static const TUYA_OS_SEMAPHORE_INTF m_tuya_os_semaphore_intfs = {
    .init    = tuya_os_adapt_semaphore_create_init, 
    .wait    = tuya_os_adapt_semaphore_wait,
    .waittimeout = tuya_os_adapt_semaphore_waittimeout,
    .post    = tuya_os_adapt_semaphore_post,
    .release = tuya_os_adapt_semaphore_release,
};

/**
 * @brief tuya_os_adapt_semaphore_create_init用于创建并初始化semaphore
 * 
 * @param[out] *pHandle semaphore句柄
 * @param[in] semCnt 
 * @param[in] sem_max 
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_create_init(SEM_HANDLE *pHandle, const unsigned int semCnt, const unsigned int sem_max)
{
    OS_Semaphore_t *sem = NULL;

     if(NULL == pHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
     }

    sem = (OS_Semaphore_t *)tuya_os_adapt_system_malloc(sizeof(OS_Semaphore_t));
    if(sem == NULL) {
         return OPRT_OS_ADAPTER_MALLOC_FAILED;
    }
    memset(sem, 0, sizeof(OS_Semaphore_t));
    if (OS_SemaphoreCreate(sem, semCnt, sem_max) != OS_OK) {
        return OPRT_OS_ADAPTER_MALLOC_FAILED;
    }
    *pHandle = (SEM_HANDLE)sem;

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_semaphore_wait用于wait semaphore
 * 
 * @param[in] semHandle semaphore句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_wait(const SEM_HANDLE semHandle)
{
    if(!semHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (OS_OK != OS_SemaphoreWait((OS_Semaphore_t *)semHandle, OS_WAIT_FOREVER)) {
        return OPRT_OS_ADAPTER_SEM_WAIT_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_semaphore_waittimeout used fo wait semaphore with timeout
 *
 * @param[in] semHandle semaphore句柄
 * @param[in] timeout  semaphore超时时间
 * @return int 0=成功，非0=失败
*/
int tuya_os_adapt_semaphore_waittimeout(IN const SEM_HANDLE semHandle, unsigned int timeout)
{
    if(!semHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    if (OS_OK != OS_SemaphoreWait((OS_Semaphore_t *)semHandle, timeout)) {
        return OPRT_OS_ADAPTER_SEM_WAIT_FAILED;
    }
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_semaphore_post用于post semaphore
 * 
 * @param[in] semHandle semaphore句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_post(const SEM_HANDLE semHandle)
{
    if(!semHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (OS_OK != OS_SemaphoreRelease((OS_Semaphore_t *)semHandle)) {
        return OPRT_OS_ADAPTER_SEM_POST_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_semaphore_release用于release semaphore
 * 
 * @param[in] semHandle 
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_release(const SEM_HANDLE semHandle)
{
    if(!semHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

     if (OS_OK != OS_SemaphoreDelete((OS_Semaphore_t *)semHandle)) {
        return OPRT_OS_ADAPTER_SEM_RELEASE_FAILED;
    }
    tuya_os_adapt_system_free(semHandle);
    return OPRT_OS_ADAPTER_OK;
}

//注册函数
int tuya_os_adapt_reg_semaphore_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_SEMAPHORE, (void *)&m_tuya_os_semaphore_intfs);
}
