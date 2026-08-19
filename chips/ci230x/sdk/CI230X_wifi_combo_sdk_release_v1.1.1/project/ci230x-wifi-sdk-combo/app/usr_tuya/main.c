#include "proj_config.h"
#include "usr_app.h"
#include "ln_at.h"
#include "osal/osal.h"
#include "hal/hal_interrupt.h"

#include "utils/debug/CmBacktrace/cm_backtrace.h"
#include "utils/runtime/runtime.h"
#include "utils/system_parameter.h"

#include "ln_nvds.h"
#include "ln_kv_api.h"
#include "ln_utils.h"
#include "ln_misc.h"

#include "ota_port.h"
#include "flash_partition_table.h"

#include "wifi.h"
#include "wifi_manager.h"
#include "netif/ethernetif.h"

#include "ty_ada_export_api.h"
#include "ln_ty_sdk_version.h"
#include "utils/power_mgmt/ln_pm.h"

#include "tuya_iot_config.h"
#include "cias_user_config.h"
#include "tuya_device_auth.h"
#include "cias_log.h"
#define REBOOT_MAGIC_WORD (0xdeadbeefa5a55a5a)
static volatile uint32_t s_hw_reboot_cnt __attribute__((section(".no_init_data"))) __ZERO_INIT__;
static volatile uint64_t s_hw_reboot_magic_word __attribute__((section(".no_init_data"))) __ZERO_INIT__;

uint32_t usr_reboot_cnt_get(void)
{
    return s_hw_reboot_cnt;
}
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
    //4.init log&AT
    //ln_transfer_init()
    log_init();
    if (LN_AT_ERR_NONE != ln_at_init())
    {
        LOG(LOG_LVL_ERROR, "ln at init fail.\r\n");
        return -1;
    }
    CIAS_LOG_HL("cias_system_init:********************>>>>>Heap left: %d <x> min:%d<<<<<********************\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
    cias_system_init();
    LOG(LOG_LVL_INFO, "project name:%s, project version:%s\r\n", PROJECT_NAME, PROJECT_VERSION);
    LOG(LOG_LVL_INFO, "device uuid:%s, device key:%s\r\n", PROD_UUID, PROD_AUTH_KEY);
    cm_backtrace_init("tuya", "HW_V1.0", LN_TY_SDK_VERSION_STRING);
    if (NVDS_ERR_OK != ln_nvds_init(NVDS_SPACE_OFFSET))
    {
        LOG(LOG_LVL_ERROR, "NVDS init filed!\r\n");
    }
    if (KV_ERR_NONE != ln_kv_port_init(KV_SPACE_OFFSET, (KV_SPACE_OFFSET + KV_SPACE_SIZE)))
    {
        LOG(LOG_LVL_ERROR, " KV init filed!\r\n");
    }
    //5.communicatio port（for audio） init
    if (communication_port_init())
    {
        LOG(LOG_LVL_ERROR, "communication init for audio failed !!!\r\n");
    }
    //init system parameter
    sysparam_integrity_check_all();
    ota_port_init();
    //Init lwip stack(Creat lwip tack).
    lwip_tcpip_init();
    ty_ada_bt_init();
    ln_pm_sleep_mode_set(ACTIVE);

    /**
     * CLK_G_I2S | CLK_G_WS2811 | CLK_G_DBGH |
     * CLK_G_SDIO | CLK_G_EFUSE | CLK_G_AES
    */
    ln_pm_always_clk_disable_select(CLK_G_I2S | CLK_G_WS2811 |
                                    CLK_G_SDIO | CLK_G_AES);
    /**
     * CLK_G_BLE | CLK_G_ADC | CLK_G_GPIOA | CLK_G_GPIOB |
     * CLK_G_SPI0 | CLK_G_SPI1 | CLK_G_I2C0 |
     * CLK_G_UART1 | CLK_G_UART2 | CLK_G_WDT |
     * CLK_G_TIM_REG | CLK_G_TIM1 | CLK_G_TIM2 |
     * CLK_G_TIM3 | CLK_G_TIM4 | CLK_G_MAC |
     * CLK_G_DMA | CLK_G_RF | CLK_G_ADV_TIMER |
     * CLK_G_TRNG
     * 
     * CLK_G_TIM3:
     *      For WiFi hardware burst test, in burst mode, need to enable the clock.
     * CLK_G_ADC:
     *      For WiFi calibration
     * 
    */
    ln_pm_lightsleep_clk_disable_select(CLK_G_GPIOA | CLK_G_GPIOB | CLK_G_SPI0 | CLK_G_SPI1 | CLK_G_I2C0 | CLK_G_UART1 | CLK_G_UART2 | CLK_G_WDT | CLK_G_TIM1 | CLK_G_TIM2 | CLK_G_MAC | CLK_G_DMA | CLK_G_RF | CLK_G_ADV_TIMER| CLK_G_TRNG);
    CIAS_LOG_HL("creat_usr_app_task:********************>>>>>Heap left: %d <x> min:%d<<<<<********************\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
    //Creat usr app task.
    if( creat_usr_app_task() != CIAS_OK)
    {
        LOG(LOG_LVL_ERROR, "creat_usr_app_task call error\r\n");
        return CIAS_FAIL;
    }
    CIAS_LOG_HL("OS_ThreadStartScheduler:********************>>>>>Heap left: %d <x> min:%d<<<<<********************\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
    OS_ThreadStartScheduler();

    return CIAS_OK;
}

// ----------------------------------------------------------------------------
