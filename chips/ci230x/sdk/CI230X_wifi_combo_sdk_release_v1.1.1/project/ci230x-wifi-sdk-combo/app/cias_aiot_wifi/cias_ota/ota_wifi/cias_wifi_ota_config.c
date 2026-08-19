#include "cias_wifi_ota_config.h"
#include "cias_log.h"
#include "cias_system_manage.h"
#include "cias_freertos_queue.h"
#include "freertos_common.h"
#include "ln_types.h"
#include "flash_partition_table.h"
#include "ota_types.h"
#include "ota_err.h"
#include "ota_image.h"

#include "cias_olcp.h"
#include "ln_nvds.h"
#include "ota_port.h"
#include "netif/ethernetif.h"
#include "hal/hal_flash.h"
#include "config.h"
#include "cias_qcloud_ota.h"
#include "hal/hal_wdt.h"
#include "base_addr_ln882h.h"
#include "hal/hal_gpio.h"

#if CIAS_IOT_TENCENT_ENABLE
#include "cias_qcloud_main.h"
extern OTAContextData *ota_ctx;
#endif

static char *temp4K_buf = NULL;
static int temp4k_offset = 0;
static uint32_t flash_ota_start_addr = WIFI_OTA_SPACE_OFFSET;
static uint32_t flash_ota_offset = 0;
static uint8_t wifi_ota_is_persistent_started = LN_FALSE;
static uint8_t wifi_ota_is_precheck_ok = LN_FALSE;
static uint8_t wifi_ota_is_ready_to_verify = LN_FALSE;
static int wifi_ota_last_package = 0;
static volatile int wifi_ota_update_flag = 0;
unsigned char *receive_buf;
// extern cias_ota_data_t *cias_ota_data;
uint32_t wifi_ota_binary_size = 0;

cias_wifi_ota_handle_t cias_wifi_ota_handle =
{
    NULL,
    NULL,
};
static int wifi_ota_persistent_start(void)
{
    if (NULL == temp4K_buf)
    {
        temp4K_buf = OS_Malloc(WIFI_OTA_SECTOR_SIZE_4KB);
        if (NULL == temp4K_buf)
        {
            LOG(LOG_LVL_INFO, "failed to alloc 4KB!!!\r\n");
            return LN_FALSE;
        }
        memset(temp4K_buf, 0, WIFI_OTA_SECTOR_SIZE_4KB);
    }

    temp4k_offset = 0;
    flash_ota_start_addr = WIFI_OTA_SPACE_OFFSET;
    flash_ota_offset = 0;
    wifi_ota_is_persistent_started = LN_TRUE;
    return LN_TRUE;
}

static int update_wifi_ota_state(void)
{
    upg_state_t state = UPG_STATE_DOWNLOAD_OK;
    ln_nvds_set_ota_upg_state(state);
    return LN_TRUE;
}

/**
 * @brief check ota image header, body.
 * @return return LN_TRUE on success, LN_FALSE on failure.
 */
static int wifi_ota_verify_download(void)
{
    image_hdr_t ota_header;

    if (OTA_ERR_NONE != image_header_fast_read(OTA_SPACE_OFFSET, &ota_header))
    {
        LOG(LOG_LVL_ERROR, "failed to read ota header.\r\n");
        return LN_FALSE;
    }

    if (OTA_ERR_NONE != image_header_verify(&ota_header))
    {
        LOG(LOG_LVL_ERROR, "failed to verify ota header.\r\n");
        return LN_FALSE;
    }

    if (OTA_ERR_NONE != image_body_verify(OTA_SPACE_OFFSET, &ota_header))
    {
        LOG(LOG_LVL_ERROR, "failed to verify ota body.\r\n");
        return LN_FALSE;
    }

    LOG(LOG_LVL_INFO, "Succeed to verify OTA image content.\r\n");
    return LN_TRUE;
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
static int wifi_ota_download_precheck(uint32_t app_offset, image_hdr_t *ota_hdr)
{
    image_hdr_t *app_hdr = NULL;
    if (NULL == (app_hdr = OS_Malloc(sizeof(image_hdr_t))))
    {
        LOG(LOG_LVL_ERROR, "[%s:%d] malloc failed.\r\n", __func__, __LINE__);
        return LN_FALSE;
    }
    if (OTA_ERR_NONE != image_header_fast_read(app_offset, app_hdr))
    {
        LOG(LOG_LVL_ERROR, "failed to read app header.\r\n");
        goto ret_err;
    }
    if ((ota_hdr->image_type == IMAGE_TYPE_ORIGINAL) ||
        (ota_hdr->image_type == IMAGE_TYPE_ORIGINAL_XZ))
    {
        // check version
        if (((ota_hdr->ver.ver_major << 8) + ota_hdr->ver.ver_minor) ==
            ((app_hdr->ver.ver_major << 8) + app_hdr->ver.ver_minor))
        {
            LOG(LOG_LVL_ERROR, "[%s:%d] same version, do not upgrade!\r\n",
                __func__, __LINE__);
            goto ret_err;
        }

        // check file size
        if (((ota_hdr->img_size_orig + sizeof(image_hdr_t)) > APP_SPACE_SIZE) ||
            ((ota_hdr->img_size_orig_xz + sizeof(image_hdr_t)) > OTA_SPACE_SIZE))
        {
            LOG(LOG_LVL_ERROR, "[%s:%d] size check failed.\r\n", __func__, __LINE__);
            goto ret_err;
        }
    }
    else
    {
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
 * @brief Save block to flash.
 *
 * @param buf
 * @param buf_len
 * @return return LN_TRUE on success, LN_FALSE on failure.
 */
static int wifi_ota_persistent_write(const char *buf, const int32_t buf_len)
{
    int part_len = 0; // [0, 1, 2, ..., 4K-1], 0, 1, 2, ..., (part_len-1)

    if (!wifi_ota_is_persistent_started)
    {
        return LN_TRUE;
    }

    if (temp4k_offset + buf_len < WIFI_OTA_SECTOR_SIZE_4KB)
    {
        // just copy all buf data to temp4K_buf
        memcpy(temp4K_buf + temp4k_offset, buf, buf_len);
        temp4k_offset += buf_len;
        part_len = 0;
    }
    else
    {
        // just copy part of buf to temp4K_buf
        part_len = temp4k_offset + buf_len - WIFI_OTA_SECTOR_SIZE_4KB;
        memcpy(temp4K_buf + temp4k_offset, buf, buf_len - part_len);
        temp4k_offset += buf_len - part_len;
    }

    if (temp4k_offset >= (WIFI_OTA_SECTOR_SIZE_4KB - 1))
    {
        // write to flash
        LOG(LOG_LVL_INFO, "write at flash: 0x%08x\r\n", flash_ota_start_addr + flash_ota_offset);
#if 0 //add by houwang
LOG(LOG_LVL_INFO,"\r\n===================print start====================\r\n");
for(int i = 0; i < WIFI_OTA_SECTOR_SIZE_4KB - 1; i++)
{
    LOG(LOG_LVL_INFO,"%02x ", temp4K_buf[i]);
}
LOG(LOG_LVL_INFO,"\r\n===================print end====================\r\n");
#endif
        if (flash_ota_offset == 0)
        {
            if (LN_TRUE != wifi_ota_download_precheck(APP_SPACE_OFFSET, (image_hdr_t *)temp4K_buf))
            {
                LOG(LOG_LVL_INFO, "ota download precheck failed!\r\n");
                wifi_ota_is_precheck_ok = LN_FALSE;
                return LN_FALSE;
            }
            wifi_ota_is_precheck_ok = LN_TRUE;
        }
#if WIFI_OTA_DEBUG
        LOG(LOG_LVL_INFO, "1===\r\n");
        hal_flash_erase(flash_ota_start_addr + flash_ota_offset, WIFI_OTA_SECTOR_SIZE_4KB);
        LOG(LOG_LVL_INFO, "2===\r\n");
        hal_flash_program(flash_ota_start_addr + flash_ota_offset, WIFI_OTA_SECTOR_SIZE_4KB, (uint8_t *)temp4K_buf);
        LOG(LOG_LVL_INFO, "3===\r\n");
#else
        hal_flash_erase(flash_ota_start_addr + flash_ota_offset, WIFI_OTA_SECTOR_SIZE_4KB);
        hal_flash_program(flash_ota_start_addr + flash_ota_offset, WIFI_OTA_SECTOR_SIZE_4KB, (uint8_t *)temp4K_buf);
#endif //WIFI_OTA_DEBUG
        flash_ota_offset += WIFI_OTA_SECTOR_SIZE_4KB;
        if ((flash_ota_offset + WIFI_OTA_SECTOR_SIZE_4KB) > wifi_ota_binary_size)
        {
            LOG(LOG_LVL_INFO, "wifi ota last package = 1\r\n");
            wifi_ota_last_package = 1;
        }
        memset(temp4K_buf, 0, WIFI_OTA_SECTOR_SIZE_4KB);
        LOG(LOG_LVL_INFO, "4===\r\n");
        temp4k_offset = 0;
    }

    if (part_len > 0)
    {
        memcpy(temp4K_buf + temp4k_offset, buf + (buf_len - part_len), part_len);
        temp4k_offset += part_len;
    }
    LOG(LOG_LVL_INFO, "5===\r\n");
    return LN_TRUE;
}

/**
 * @brief save last block and clear flags.
 * @return return LN_TRUE on success, LN_FALSE on failure.
 */
static int wifi_ota_persistent_finish(const char *buf, const int32_t buf_len)
{
    if (!wifi_ota_is_persistent_started)
    {
        return LN_FALSE;
    }

    memcpy(temp4K_buf, buf, buf_len);
    LOG(LOG_LVL_INFO, "write at flash: 0x%08x\r\n", flash_ota_start_addr + flash_ota_offset);
    hal_flash_erase(flash_ota_start_addr + flash_ota_offset, WIFI_OTA_SECTOR_SIZE_4KB);
    hal_flash_program(flash_ota_start_addr + flash_ota_offset, WIFI_OTA_SECTOR_SIZE_4KB, (uint8_t *)temp4K_buf);
    LOG(LOG_LVL_INFO, "\r\nwifi ota persistent finished\r\n");

    OS_Free(temp4K_buf);
    temp4K_buf = NULL;
    temp4k_offset = 0;

    flash_ota_offset = 0;
    wifi_ota_is_persistent_started = LN_FALSE;
    return LN_TRUE;
}

static int wifi_ota_write_flash(const char *recvbuf, int32_t recvbuf_len, int32_t total_bytes, int is_recv_finished)
{
    if (!is_recv_finished)
    {
        if (LN_TRUE != wifi_ota_persistent_write(recvbuf, recvbuf_len))
        {
            return -1;
        }
    }
    else
    {
        wifi_ota_persistent_finish(recvbuf, recvbuf_len);
        wifi_ota_is_ready_to_verify = LN_TRUE;
        CIAS_LOG_HL("cb info: recv %d finished, no more data to deal with.\r\n", total_bytes);
    }
    return recvbuf_len;
}

static void cias_wifi_ota_get_cloud_data_task(void *params)
{
    int get_image_len = 0;
#if CIAS_IOT_TENCENT_ENABLE
    wifi_ota_binary_size = ota_ctx->fw_file_size;
#endif
    while (1)
    {
        if (wifi_ota_update_flag == WIFI_OTA_GET_START)
        {
            get_image_len = cias_wifi_ota_handle.cias_get_wifi_image_from_cloud(receive_buf, flash_ota_offset, WIFI_OTA_SECTOR_SIZE_4KB);
            if (get_image_len == -1)
            {
#if WIFI_OTA_DEBUG //测试用
                hal_gpio_pin_afio_en(GPIOB_BASE, GPIO_PIN_4, HAL_ENABLE);
                gpio_init_t_def gpio_init;
                memset(&gpio_init, 0, sizeof(gpio_init)); //清零结构体
                gpio_init.dir = GPIO_OUTPUT;              //配置GPIO方向，输入
                gpio_init.pin = GPIO_PIN_4;               //配置GPIO引脚号
                gpio_init.speed = GPIO_HIGH_SPEED;        //设置GPIO速度
                hal_gpio_init(GPIOB_BASE, &gpio_init);    //初始化GPIO
                hal_gpio_pin_set(GPIOB_BASE, GPIO_PIN_4); //拉高
                cias_task_delay_ms(2);
                hal_gpio_pin_set(GPIOB_BASE, GPIO_PIN_4);
#endif //WIFI_OTA_DEBUG
                cias_wifi_ota_handle.cias_report_ota_status_to_iot(1);
                LOG(LOG_LVL_INFO, "\r\n======report_ota_status:failed===========");
                cias_task_delay_ms(10);
                cias_wifi_ota_handle.cias_report_ota_status_to_iot(1);
                LOG(LOG_LVL_INFO, "\r\n>>>ERROR:get_wifi_image_from_cloud(), reset!!!");
                cias_task_delay_ms(1000);
                ln_chip_reboot();
            }
            if (wifi_ota_last_package)
            {
                uint32_t wifi_ota_last_package_end = (wifi_ota_binary_size % WIFI_OTA_SECTOR_SIZE_4KB);
                for (uint32_t i = wifi_ota_last_package_end; i < WIFI_OTA_SECTOR_SIZE_4KB; i++)
                {
                    receive_buf[i] = 0x00;
                }
            }
            wifi_ota_update_flag = WIFI_OTA_WRITE_START;
        }
        cias_task_delay_ms(20);
        hal_wdt_cnt_restart(WDT_BASE);
    }
}

static int cias_wifi_ota_get_cloud_data(void)
{
    cias_task_t cias_wifi_ota_get_cloud_data_handle = {
        .handle = NULL,
    };
    cias_task_create(&cias_wifi_ota_get_cloud_data_handle,
                     "cias_wifi_ota_get_cloud_data",
                     (cias_task_function_t)cias_wifi_ota_get_cloud_data_task,
                     (void *)0,
                     WIFI_OTA_TASK_PRIORITY,
                     WIFI_OTA_GET_TASK_SIZE);
}

static void cias_wifi_ota_progress_task(void *params)
{
    int wifi_ota_is_recv_finished = 0;
    int ret = 0;
    int errCode = 0;
    int32_t receive_buf_len = 0;
    int32_t receive_total_size = 0;
    int write_len = 0, recv_len = 0;
    ota_port_ctx_t *ota_port = NULL;

    ota_port = ota_get_port_ctx();
    while (NETDEV_LINK_UP != netdev_get_link_state(netdev_get_active()))
    {
        cias_task_delay_ms(500);
    }
    if (LN_TRUE == wifi_ota_persistent_start())
    {
        receive_buf = (unsigned char *)malloc(WIFI_OTA_SECTOR_SIZE_4KB);
        wifi_ota_update_flag = WIFI_OTA_GET_START;
        LOG(LOG_LVL_INFO, "\r\n>>>wifi_ota_update_flag = %d\r\n>>>wifi_ota_binary_size:%d\r\n", wifi_ota_update_flag, wifi_ota_binary_size);
        while (1)
        {
            if (wifi_ota_update_flag == WIFI_OTA_WRITE_START)
            {
                if (!wifi_ota_last_package)
                {
                    wifi_ota_is_recv_finished = 0;
                    ret = wifi_ota_write_flash(receive_buf, WIFI_OTA_SECTOR_SIZE_4KB, receive_total_size, wifi_ota_is_recv_finished);
                    if (ret < 0)
                    {
                        errCode = -1;
                        break;
                    }
                    else
                    {
                        wifi_ota_update_flag = WIFI_OTA_GET_START;
                        recv_len += ret;
                    }
                }
                else
                {
                    wifi_ota_is_recv_finished = 1;
                    wifi_ota_write_flash(receive_buf, WIFI_OTA_SECTOR_SIZE_4KB, receive_total_size, wifi_ota_is_recv_finished);
                }
                if (wifi_ota_is_ready_to_verify == LN_TRUE)
                {
                    LOG(LOG_LVL_INFO, "wifi ota verify\r\n");
                    if (LN_TRUE == wifi_ota_verify_download())
                    {
                        LOG(LOG_LVL_INFO, "update wifi ota state\r\n");
                        update_wifi_ota_state();
                        LOG(LOG_LVL_INFO, "http client job done verify ok...\r\n");
#if CIAS_IOT_TENCENT_ENABLE
                        cias_wifi_ota_handle.cias_report_ota_status_to_iot(4); //上报腾讯云升级成功
                        cias_task_delay_ms(2);
                        cias_wifi_ota_handle.cias_report_ota_status_to_iot(4); //上报腾讯云升级成功
#endif
                        cias_task_delay_ms(20);
                        LOG(LOG_LVL_INFO, "\r\n升级完成 WIFI复位!\r\n");
                        cias_task_delay_ms(2);
                        ln_chip_reboot();
                    }
                    break;
                }
            }
            cias_task_delay_ms(20);
#if 1 //debug by houwang
            hal_wdt_cnt_restart(WDT_BASE);
#endif
        }
        if (errCode == -1)
        {
            LOG(LOG_LVL_INFO, "\r\n升级失败 WIFI复位!\r\n");
            cias_task_delay_ms(2);
            ln_chip_reboot();
        }
    }
    while (1)
    {
        cias_task_delay_ms(2000);
        // LOG(LOG_LVL_INFO, "\r\n======cias_wifi_ota_progress_task===========\r\n");
    }
}

static void cias_ota_progress(void)
{
    cias_task_t cias_ota_progress_handle = {
        .handle = NULL,
    };
    cias_task_create(&cias_ota_progress_handle,
                     "cias_wifi_ota_progress",
                     (cias_task_function_t)cias_wifi_ota_progress_task,
                     (void *)0,
                     WIFI_OTA_TASK_PRIORITY,
                     WIFI_OTA_PROGRESS_TASK_SIZE);
}

static void wifi_ota_watch_dog(void)
{
    /* 看门狗初始化 */
    wdt_init_t_def wdt_init;
    memset(&wdt_init, 0, sizeof(wdt_init));
    // wdt_init.wdt_rmod = WDT_RMOD_1;//等于0的时候，计数器溢出时直接复位，等于1的时候，先产生中断，如果再次溢出，则产生复位。
    wdt_init.wdt_rmod = WDT_RMOD_0;
    wdt_init.wdt_rpl = WDT_RPL_32_PCLK; //设置复位延时的时间
    wdt_init.top = WDT_TOP_VALUE_11;    //设置看门狗计数器的值,当TOP=1时，对应计数器的值为0x1FF，而看门狗是用的时钟是一个单独的32k时钟，
                                        //所以此时的喂狗时间必须在 (1/32k) * 0x1FF 内。
    hal_wdt_init(WDT_BASE, &wdt_init);

    /* 配置看门狗中断 */
    NVIC_SetPriority(WDT_IRQn, 4);
    NVIC_EnableIRQ(WDT_IRQn);

    /* 使能看门狗 */
    hal_wdt_en(WDT_BASE, HAL_ENABLE);

    /* 先喂下狗 */
    hal_wdt_cnt_restart(WDT_BASE);
}

extern cias_system_manage_param_t g_cias_system_manage_param;
static void cias_wifi_ota_main_task(void *params)
{
    g_cias_system_manage_param.wifi_ota_start_flag = 0;
    g_cias_system_manage_param.recv_slave_msg_deal_flag = 0;
    g_cias_system_manage_param.qcloud_main_handle.handle = NULL;
    g_cias_system_manage_param.recv_slave_msg_deal_handle.handle = NULL;

    while (1)
    {
        if (g_cias_system_manage_param.wifi_ota_start_flag == 1) //升级任务开始，删除不需要的任务
        {
            CIAS_LOG_HL("\r\n>>>WIFI OTA start\r\n");
            wifi_ota_watch_dog(); //添加看门狗，规避死机问题
            g_cias_system_manage_param.wifi_ota_start_flag = 0;
            extern cias_queue_t cias_recv_slave_msg_queue;
            extern cias_queue_t cias_send_slave_msg_queue;
            cias_queue_delete(&cias_send_slave_msg_queue);
            cias_queue_delete(&cias_recv_slave_msg_queue);

            if (g_cias_system_manage_param.recv_slave_msg_deal_handle.handle)
                cias_task_delete(&g_cias_system_manage_param.recv_slave_msg_deal_handle);
            if (g_cias_system_manage_param.recv_slave_msg_task_handle.handle)
                cias_task_delete(&g_cias_system_manage_param.recv_slave_msg_task_handle);
            if (g_cias_system_manage_param.send_slave_msg_task_handle.handle)
                cias_task_delete(&g_cias_system_manage_param.send_slave_msg_task_handle);
            cias_task_delay_ms(100);
            cias_wifi_ota_get_cloud_data();
            cias_task_delay_ms(2);
            cias_ota_progress();
        }
        else
        {
            cias_task_delay_ms(5000);
            // CIAS_LOG_HL("\r\nversion:%d.%d.%d", FLASH_IMAGE_VER_MAJOR, FLASH_IMAGE_VER_MIDDLE, FLASH_IMAGE_VER_MINOR);
        }
    }
}
int cias_wifi_ota_callback_init(void)
{
#if CIAS_IOT_TENCENT_ENABLE
    cias_wifi_ota_handle.cias_get_wifi_image_from_cloud = get_wifi_image_from_cloud;
    cias_wifi_ota_handle.cias_report_ota_status_to_iot = report_ota_status_to_iot;
#endif
    if (cias_wifi_ota_handle.cias_get_wifi_image_from_cloud == NULL || cias_wifi_ota_handle.cias_report_ota_status_to_iot == NULL)
    {
        CIAS_LOG_ERR("\r\n>>>cias_wifi_ota_callback_init  error\r\n");
        return CIAS_FAIL;
    }
    return CIAS_OK;
}
int cias_wifi_ota_main(void)
{

    if (cias_wifi_ota_callback_init() != CIAS_OK)
    {
        return CIAS_FAIL;
    }
    cias_task_t cias_wifi_ota_main_handle = {
        .handle = NULL,
    };
    cias_task_create(&cias_wifi_ota_main_handle,
                     "cias_wifi_ota_main",
                     (cias_task_function_t)cias_wifi_ota_main_task,
                     (void *)0,
                     WIFI_OTA_TASK_PRIORITY,
                     WIFI_OTA_ENTRY_TASK_SIZE);
    return CIAS_OK;
}