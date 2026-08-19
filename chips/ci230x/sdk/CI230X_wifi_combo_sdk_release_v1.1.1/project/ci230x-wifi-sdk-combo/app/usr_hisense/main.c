#include "proj_config.h"
#include "osal/osal.h"
#include "wifi.h"
#include "wifi_port.h"
#include "netif/ethernetif.h"
#include "usr_app.h"
#include "utils/system_parameter.h"
#include "utils/debug/CmBacktrace/cm_backtrace.h"
#include "utils/reboot_trace/reboot_trace.h"
#include "utils/runtime/runtime.h"
#include "hal/hal_interrupt.h"
#include "ln_nvds.h"
#include "ln_kv_api.h"
#include "ln_at.h"
#include "ln_utils.h"
#include "flash_partition_table.h"
#include "cias_communication.h"
#include "freertos_timer.h"
#include "cias_log.h"
#include "ota_err.h"
#include "Config.h"
#include "cias_user_config.h"
#include "cias_demo_config.h"
#if CIAS_BLE_CONFIG_ENABLE
#include "cias_ble_config.h"
#endif
#if TENCENT_AP_NET_WORK_EN
#include "cias_ap_net_port.h"
#include "cias_http_client.h"
#include "cias_system.h"
#endif

int main(int argc, char *argv[])
{
    LN_UNUSED(argc);
    LN_UNUSED(argv);

    //0. check reboot cause
    ln_chip_get_reboot_cause();

    //1.sys clock,interrupt
    SetSysClock();
    set_interrupt_priority();
    switch_global_interrupt(HAL_ENABLE);
    ln_runtime_measure_init();

    //2. register os heap mem
    OS_DefineHeapRegions();

    log_init();
    if (LN_AT_ERR_NONE != ln_at_init())
    {
        LOG(LOG_LVL_ERROR, "ln at init fail.\r\n");
        return -1;
    }

    CIAS_LOG_HL("ci230x-wifi-sdk-combo info:\r\n");
    CIAS_LOG_HL("sdk software version: V%d.%d.%d  \r\n", FLASH_IMAGE_VER_MAJOR, FLASH_IMAGE_VER_MIDDLE, FLASH_IMAGE_VER_MINOR);
    CIAS_LOG_HL("demo hardware version: V%d.%d.%d \r\n", CIAS_HARD_VERSION_MAJOR, CIAS_HARD_VERSION_MIDDLE, CIAS_HARD_VERSION_MINOR);
    CIAS_LOG_HL("********************>>>>>(log_init)Heap left: %d <1> min:%d<<<<<********************\r\n",
    xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());

    cias_system_init();
    //5.communicatio port（for audio） init
    if (communication_port_init())
    {
        LOG(LOG_LVL_ERROR, "communication init for audio failed !!!\r\n");
    }
    cm_backtrace_init("ci230x-wifi-sdk-combo", "hw", "sw");
    CIAS_LOG_HL("********************>>>>>(cm_backtrace_init)Heap left: %d <1> min:%d<<<<<********************\r\n",
    xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());

    if (NVDS_ERR_OK != ln_nvds_init(NVDS_SPACE_OFFSET))
    {
        LOG(LOG_LVL_ERROR, "NVDS init filed!\r\n");
    }

    if (KV_ERR_NONE != ln_kv_port_init(KV_SPACE_OFFSET, (KV_SPACE_OFFSET + KV_SPACE_SIZE)))
    {
        LOG(LOG_LVL_ERROR, "KV  init filed!\r\n");
    }
    //init system parameter
    sysparam_integrity_check_all();

#if TENCENT_AP_NET_WORK_EN
    int wifi_config_ret = 0;
    int compare_for_reset_str = 0;
    int compare_for_lock_str = 0;
    char ln_reset_addr_str[10] = {0};
    char wifi_config_lock[10] = {0};
    char wifi_config_strlen[30] = {0};

    ln_nvds_read(LN_RESET_FLAG_ADDR, ln_reset_addr_str, sizeof(ln_reset_addr_str));
    compare_for_reset_str = strcmp((const char *)ln_reset_addr_str, (const char *)"reset");
    ln_nvds_read(WIFI_LOCK_ADDR, wifi_config_lock, sizeof(wifi_config_lock));
    compare_for_lock_str = strcmp((const char *)wifi_config_lock, (const char *)"lock");

#if AP_NET_DEBUG
    LOG(LOG_LVL_INFO, "\r\n ln_reset_addr_str:%s \r\n", ln_reset_addr_str);
    LOG(LOG_LVL_INFO, "\r\nwifi_config_lock:%s\r\n", wifi_config_lock);
#endif //AP_NET_DEBUG

    if (compare_for_reset_str == 0)
    {
        LOG(LOG_LVL_INFO, "\r\nwifi config is effictived, clear \"reset\" flag.\r\n");
        ln_nvds_write(LN_RESET_FLAG_ADDR, (uint8_t *)"000000", strlen("000000") + 1);
    }
    if ((compare_for_lock_str != 0) || (compare_for_reset_str == 0))
    {
        LOG(LOG_LVL_INFO, "wifi_config_init......\r\n");
        wifi_config_ret = ln_nvds_write(SSID_FLASH_ADDR, (uint8_t *)"", strlen("") + 1);
        wifi_config_ret += ln_nvds_write(PWD_FLASH_ADDR, (uint8_t *)"", strlen("") + 1);
        if (wifi_config_ret != 0)
            LOG(LOG_LVL_INFO, "wifi_config_init failed!\r\n");
        ln_nvds_read(SSID_FLASH_ADDR, wifi_config_strlen, sizeof(wifi_config_strlen));
        LOG(LOG_LVL_INFO, "init ssid:%s\r\n", wifi_config_strlen);
        ln_nvds_read(PWD_FLASH_ADDR, wifi_config_strlen, sizeof(wifi_config_strlen));
        LOG(LOG_LVL_INFO, "init pwd:%s\r\n", wifi_config_strlen);
    }
#endif //TENCENT_AP_NET_WORK_EN

	wifi_rf_calibration();  //wifi校准合并为一个函数
    //Init wifi stack.
    wifi_init();
    wlib_pvtcmd_output_cb_set(ln_at_vprintf);
    //Init lwip stack.
    lwip_tcpip_init();
    LOG(LOG_LVL_INFO, "********************>>>>>(lwip_tcpip_init)Heap left: %d <1> min:%d<<<<<********************\r\n", 
    xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());

#if CIAS_AIOT_WIFI_OTA_ENABLE
    if (OTA_ERR_NONE != ota_port_init())
    {
        LOG(LOG_LVL_ERROR, "ota port failed!\r\n");
    }
    else
    {
        LOG(LOG_LVL_INFO, "ota port init ok!\r\n");
    }
#endif

#if CIAS_BLE_CONFIG_ENABLE
    uint8_t mac[6] = {0}; //蓝牙mac地址，随机生成
    ln_generate_random_mac(mac);
    mac[5] |= 0xC0; // This address is random generated, so assign 0x11 => Static Random Address
    extern void rw_init(uint8_t mac[6]);
    rw_init(mac);
#endif

    //Creat usr app task.
    if( creat_usr_app_task() != CIAS_OK)
    {
        LOG(LOG_LVL_ERROR, "creat_usr_app_task call error\r\n");
        return CIAS_FAIL;
    }

    OS_ThreadStartScheduler();

    return 0;
}

// ----------------------------------------------------------------------------