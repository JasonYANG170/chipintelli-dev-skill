/**
 * @file tuya_os_adapt_storge.c
 * @brief flash操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */
#include "tuya_os_adapt_storge.h"
#include "tuya_os_adapter_error_code.h"
#include "tuya_os_adapter.h"
#include "hal/hal_flash.h"
#include "flash_partition_table.h"

#include "utils/debug/log.h"


// 如果flash驱动没有做加锁逻辑，在这一层要做对flash read/write/erase的加锁逻辑
//tuya sdk调用会涉及到多线程操作
/***********************************************************
*************************micro define***********************
***********************************************************/

#define FLH_BLOCK_SZ            (0x1000)
#define FLH_TOTAL_SZ            (0x200000)

#define TUYA_SPACE_OFFSET       (FLH_TOTAL_SZ - TUYA_SPACE_SIZE)
#define TUYA_SPACE_SIZE         (1024 * 212)

/**********************************************
              uf1        uf2  kv  simpl   swap 
________+______________+_____+__+________+____+
|       |              |     |  |        |    |
|       |              |     |  |        |    |
|       |     96K      | 32K |4K|   64K  | 16K|
|       |              |     |  |        |    |
--------+--------------+-----+--+--------+----+
0                                     2048K  2M
***********************************************
total used 212k
***********************************************/

#define UF1_BASE                    (TUYA_SPACE_OFFSET)
#define UF1_SIZE                    (96 * 1024)
#define UF2_BASE                    (UF1_BASE + UF1_SIZE)
#define UF2_SIZE                    (32 * 1024)
#define TY_KV_BASE                  (UF2_BASE + UF2_SIZE)
#define TY_KV_SIZE                  (4 * 1024)

#define SIMPL_BASE                  (TY_KV_BASE + TY_KV_SIZE)
#define SIMPL_SIZE                  (64 * 1024)
#define SWAP_BASE                   (SIMPL_BASE + SIMPL_SIZE)
#define SWAP_SIZE                   (16 * 1024)

#if ((SWAP_BASE + SWAP_SIZE) > (TUYA_SPACE_OFFSET + TUYA_SPACE_SIZE))
#error "Flash map error! Please check 'flash_partition_table.h'."
#endif

/***********************************************************
*************************variable define********************
***********************************************************/

static MUTEX_HANDLE tuya_hal_flash_mutex = NULL;


static const TUYA_OS_STORAGE_INTF m_tuya_os_storage_intfs = {
    .read  = tuya_os_adapt_flash_read, 
    .write = tuya_os_adapt_flash_write,
    .erase = tuya_os_adapt_flash_erase,
    .get_storage_desc = tuya_os_adapt_storage_get_desc,
    .get_uf_desc = tuya_os_adapt_uf_get_desc,
    .get_legacy_swap_desc = NULL,
    .get_rcd_desc = NULL
};

static UNI_STORAGE_DESC_S storage = {
    SIMPL_BASE,
    SIMPL_SIZE,
    FLH_BLOCK_SZ,
    SWAP_BASE,
    SWAP_SIZE,
    TY_KV_BASE
};

static UF_PARTITION_TABLE_S uf_file = {
    .sector_size = 4 * 1024,
    .uf_partition_num = 2,
    .uf_partition = {
        {UF2_BASE, UF2_SIZE},
        {UF1_BASE, UF1_SIZE}
    }
};

/***********************************************************
*************************function define********************
***********************************************************/
static void tuya_os_adapt_flash_mutex_lock(void)
{
    if (NULL==tuya_hal_flash_mutex) {
        tuya_os_adapt_mutex_create_init(&tuya_hal_flash_mutex);
    }
    tuya_os_adapt_mutex_lock(tuya_hal_flash_mutex);
}

static void tuya_os_adapt_flash_mutex_unlock(void)
{
    if (NULL!=tuya_hal_flash_mutex) {
        tuya_os_adapt_mutex_unlock(tuya_hal_flash_mutex);
    }
}

/**
 * @brief read data from flash
 *
 * @param[in]       addr        flash address
 * @param[out]      dst         pointer of buffer
 * @param[in]       size        size of buffer
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_flash_read(const unsigned int addr, unsigned char *dst, const unsigned int size)
{
    // LOG(LOG_LVL_DEBUG, "[%s:%d]\r\n", __func__, __LINE__);
    if (NULL == dst) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    tuya_os_adapt_flash_mutex_lock();
    hal_flash_read_by_cache(addr, size, dst);
    tuya_os_adapt_flash_mutex_unlock();

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief write data to flash
 *
 * @param[in]       addr        flash address
 * @param[in]       src         pointer of buffer
 * @param[in]       size        size of buffer
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_flash_write(const unsigned int addr, const unsigned char *src, const unsigned int size)
{
    LOG(LOG_LVL_DEBUG, "[%s:%d]\r\n", __func__, __LINE__);
    if (NULL == src) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    tuya_os_adapt_flash_mutex_lock();
    hal_flash_program(addr, size, (uint8_t *)src);
    tuya_os_adapt_flash_mutex_unlock();

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief erase flash block
 *
 * @param[in]       addr        flash block address
 * @param[in]       size        size of flash block
 * @retval          zero        success
 * @retval          not zero    failed
 */
int tuya_os_adapt_flash_erase(const unsigned int addr, const unsigned int size)
{
    LOG(LOG_LVL_DEBUG, "[%s:%d]\r\n", __func__, __LINE__);
    tuya_os_adapt_flash_mutex_lock();
    hal_flash_erase(addr, size);
    tuya_os_adapt_flash_mutex_unlock();

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief get description of storage
 *
 * @return  pointer to storage description structure
 */
UNI_STORAGE_DESC_S *tuya_os_adapt_storage_get_desc(VOID)
{
    // LOG(LOG_LVL_DEBUG, "[%s:%d]\r\n", __func__, __LINE__);
    return &storage;

}

/**
 * @brief get UF file description
 * 
 * @return  pointer to descrtion of UF file
 */
UF_PARTITION_TABLE_S *tuya_os_adapt_uf_get_desc(VOID)
{
    // LOG(LOG_LVL_DEBUG, "[%s:%d]\r\n", __func__, __LINE__);
    return &uf_file;
}

/**
 * @brief get swap description
 * 
 * @return  pointer to descrtion of swap file
 */
int tuya_os_adapt_legacy_swap_get_desc(unsigned int *addr, unsigned int *size)
{
    // to do 
    LOG(LOG_LVL_DEBUG, "[%s:%d]\r\n", __func__, __LINE__);
    if (addr == NULL || size == NULL)
    {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    *addr = storage.swap_start_addr;
    *size = storage.swap_flash_sz;
    return OPRT_OS_ADAPTER_OK; 
}

/**
 * @brief tuya_os_adapt_reg_storage_intf 接口注册
 * @return int 
 */
int tuya_os_adapt_reg_storage_intf(void)
{
    LOG(LOG_LVL_DEBUG, "[%s:%d]\r\n", __func__, __LINE__);
    return tuya_os_adapt_reg_intf(INTF_STORAGE, (void *)&m_tuya_os_storage_intfs);
}
