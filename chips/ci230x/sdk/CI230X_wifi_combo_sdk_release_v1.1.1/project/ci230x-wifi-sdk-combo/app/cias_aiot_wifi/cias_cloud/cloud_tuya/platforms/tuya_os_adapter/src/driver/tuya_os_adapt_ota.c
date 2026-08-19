/**
 * @file tuya_os_adapt_ota.c
 * @brief ota底层操作接口
 * 
 * @copyright Copyright (c) {2018-2020} 涂鸦科技 www.tuya.com
 * 
 */
#include "tuya_os_adapt_ota.h"
#include "tuya_os_adapter_error_code.h"
#include "tuya_os_adapt_system.h"
#include "ota_port.h"
#include "ota_image.h"
#include "ota_types.h"

#include "osal/osal.h"

#include "utils/debug/log.h"
#include "flash_partition_table.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define SIZE_4K             (0x1000)

/***********************************************************
*************************variable define********************
***********************************************************/
static const TUYA_OS_OTA_INTF m_tuya_os_ota_intfs = {
    .start      = tuya_os_adapt_ota_start_inform, 
    .process    = tuya_os_adapt_ota_data_process,
    .end        = tuya_os_adapt_ota_end_inform,
};

/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief 升级开始通知函数
 * 
 * @param[in] file_size 升级固件大小
 * 
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_os_adapt_ota_start_inform(unsigned int file_size)
{
#undef  LN_ALIGN_UP
#undef  LN_ALIGN_DOWN
#undef  LN_IS_ALIGNED
#undef  LN_FLASH_ALIGN_SZIE
#define LN_ALIGN_UP(x, align)        (((x) + (align) - 1) & ~((align) - 1))
#define LN_ALIGN_DOWN(x, align)      ((x) & ~((align) - 1))
#define LN_IS_ALIGNED(x, align)      (((x) & (align - 1)) == 0)

#define LN_FLASH_ALIGN_SZIE 4096

    if (LN_ALIGN_UP(file_size, LN_FLASH_ALIGN_SZIE) > OTA_SPACE_SIZE)
    {
        return OPRT_OS_ADAPTER_OTA_START_INFORM_FAILED;
    }

    tuya_hal_flash_erase(OTA_SPACE_OFFSET, LN_ALIGN_UP(file_size, LN_FLASH_ALIGN_SZIE));
    LOG(LOG_LVL_INFO, "[ota start] ==> sector_num=%d, file_size=0x%x; Aligned:%d, Aligned size:%d\r\n",
            LN_ALIGN_UP(file_size, LN_FLASH_ALIGN_SZIE) / SIZE_4K,
            file_size,
            LN_IS_ALIGNED(file_size, LN_FLASH_ALIGN_SZIE),
            LN_ALIGN_UP(file_size, LN_FLASH_ALIGN_SZIE));

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief Pre-check the image file to be downloaded.
 *
 * @attention None
 *
 * @param[in]  app_offset  The offset of the APP partition in Flash.
 * @param[in]  ota_hdr     pointer to ota partition info struct.
 *
 * @return  whether the check is successful.
 * @retval  #LN_TRUE     successful.
 * @retval  #LN_FALSE    failed.
 */
static int ota_download_precheck(uint32_t app_offset, image_hdr_t * ota_hdr)
{
    image_hdr_t *app_hdr = NULL;

    LOG(LOG_LVL_INFO, "[%s:%d] app_offset:%d; image_type:%d, major:%d, minor:%d\r\n", 
            __func__, __LINE__, app_offset, ota_hdr->image_type, ota_hdr->ver.ver_major, ota_hdr->ver.ver_minor);

    if (NULL == (app_hdr = OS_Malloc(sizeof(image_hdr_t)))) {
        LOG(LOG_LVL_ERROR, "download precheck malloc failed.\r\n");
        return LN_FALSE;
    }

    if ( OTA_ERR_NONE != image_header_fast_read(app_offset, app_hdr) ) {
        LOG(LOG_LVL_ERROR, "failed to read app header.\r\n");
        goto ret_err;
    }

    if ((ota_hdr->image_type == IMAGE_TYPE_ORIGINAL) || \
        (ota_hdr->image_type == IMAGE_TYPE_ORIGINAL_XZ))
    {
        // check file size
        if (((ota_hdr->img_size_orig + sizeof(image_hdr_t)) > APP_SPACE_SIZE) || \
            ((ota_hdr->img_size_orig_xz + sizeof(image_hdr_t)) > OTA_SPACE_SIZE)) {

            LOG(LOG_LVL_ERROR, "check file size failed size_orig=%x,orig_xz=%x,sizeof(image_hdr_t)=%x\r\n",
                ota_hdr->img_size_orig,ota_hdr->img_size_orig_xz,sizeof(image_hdr_t));
            goto ret_err;
        }
    } else {
        //image type not support!
        goto ret_err;
    }

    OS_Free(app_hdr);
    return LN_TRUE;

    ret_err:
    OS_Free(app_hdr);
    return LN_FALSE;
}

/**
 * @brief ota数据包处理
 *
 * @param[in] total_len ota升级包总大小
 * @param[in] offset 当前data在升级包中的偏移
 * @param[in] data ota数据buffer指针
 * @param[in] len ota数据buffer长度
 * @param[out] remain_len 内部已经下发但该函数还未处理的数据长度
 * @param[in] pri_data 保留参数
 *
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_os_adapt_ota_data_process(const unsigned int total_len, const unsigned int offset,
                              const unsigned char* data, const unsigned int len, unsigned int* remain_len, void* pri_data)
{
    static unsigned int total_pos = 0;

    /**
     * 1. handle first packet, read ota header to check ota firmware version and fw size
     */
    static uint8_t ota_hdr_is_checked = 0;
    static image_hdr_t _ota_hdr = {0};
    static uint16_t recved_len = 0;
 
    uint8_t *ota_hdr_buff = (uint8_t *)(&_ota_hdr);

    LOG(LOG_LVL_DEBUG, "[%s:%d] total_len:%d; offset:%d, len:%d, remain_len:%d\r\n", 
            __func__, __LINE__, total_len, offset, len, *remain_len);

    if (ota_hdr_is_checked == 0) {
        if ((offset == 0) && (len >= sizeof(image_hdr_t))) {
            if (ota_download_precheck(APP_SPACE_OFFSET, (image_hdr_t *)data) == LN_FALSE) {
                recved_len = 0;
                return OPRT_OS_ADAPTER_OTA_PROCESS_FAILED;
            }
            ota_hdr_is_checked = 1;
            recved_len = 0;

            LOG(LOG_LVL_INFO, "[%s:%d] first packet get hdr\r\n", 
                    __func__, __LINE__);
        } else { /* continue recv data from uplayer until received 'sizeof(image_hdr_t)' */
            if ((recved_len + len) <= sizeof(image_hdr_t)) {
                memcpy(ota_hdr_buff + recved_len, data, len);
                recved_len += len;
                *remain_len=0;

                LOG(LOG_LVL_INFO, "[%s:%d] continue to get hdr\r\n", 
                    __func__, __LINE__);
                return OPRT_OS_ADAPTER_OK;
            }
            else {
                memcpy(ota_hdr_buff + recved_len, data, sizeof(image_hdr_t) - recved_len);

                if (ota_download_precheck(APP_SPACE_OFFSET, (image_hdr_t *)ota_hdr_buff) == LN_FALSE) {
                    recved_len = 0;
                    return OPRT_OS_ADAPTER_OTA_PROCESS_FAILED;
                }
                ota_hdr_is_checked = 1;

                tuya_hal_flash_write(OTA_SPACE_OFFSET, ota_hdr_buff, recved_len);
                recved_len = 0;
            }
        }
    }

    tuya_hal_flash_write(OTA_SPACE_OFFSET + offset, data, len);
    *remain_len=0;

    if ((offset + len) == total_len) {
        ota_hdr_is_checked = 0;
        LOG(LOG_LVL_INFO, "[%s:%d] OTA FW write complete\r\n", 
            __func__, __LINE__);
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
* @brief check ota image header, body.
* @return return LN_TRUE on success, LN_FALSE on failure.
*/
static int ota_verify_download(void)
{
    image_hdr_t ota_header;
    
    LOG(LOG_LVL_INFO, "[%s:%d] \r\n", 
            __func__, __LINE__);

    if ( OTA_ERR_NONE != image_header_fast_read(OTA_SPACE_OFFSET, &ota_header) ) {
          LOG(LOG_LVL_ERROR, "failed to read ota header.\r\n");
          return LN_FALSE;
    }

    if ( OTA_ERR_NONE != image_header_verify(&ota_header) ) {
          LOG(LOG_LVL_ERROR, "failed to verify ota header.\r\n");
          return LN_FALSE;
    }

    if ( OTA_ERR_NONE != image_body_verify(OTA_SPACE_OFFSET, &ota_header)) {
          LOG(LOG_LVL_ERROR, "failed to verify ota body.\r\n");
          return LN_FALSE;
    }

    LOG(LOG_LVL_INFO, "Succeed to verify OTA image content.\r\n");
    return LN_TRUE;
}

/**
 * @brief 固件ota数据传输完毕通知
 *        用户可以做固件校验以及设备重启
 * param[in]        reset       是否需要重启
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_os_adapt_ota_end_inform(bool reset)
{
    ota_port_ctx_t *ctx = ota_get_port_ctx();
    if (ctx == NULL)
    {
        return LN_FALSE;
    }

    LOG(LOG_LVL_INFO, "[%s:%d] reset:%d \r\n", 
            __func__, __LINE__, reset);

    //1.
    if(LN_FALSE==ota_verify_download()){
        return OPRT_OS_ADAPTER_OTA_VERIFY_FAILED;
    }

    LOG(LOG_LVL_INFO, "[%s:%d] ota_verify_download success! \r\n", 
            __func__, __LINE__);

    //2.update OTA state
    if(LN_FALSE == ctx->upg_state_set(UPG_STATE_DOWNLOAD_OK))
    {
       return OPRT_OS_ADAPTER_OTA_END_INFORM_FAILED;
    }
    LOG(LOG_LVL_INFO, "[%s:%d] update_ota_state success! Will reboot... \r\n", 
            __func__, __LINE__);

    OS_MsDelay(100);

    //3. reboot  execute by another cmd interface
    if (reset) tuya_hal_system_reset();
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_reg_ota_intf 接口注册
 * @return int 
 */
int tuya_os_adapt_reg_ota_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_OTA, (void *)&m_tuya_os_ota_intfs);
}

