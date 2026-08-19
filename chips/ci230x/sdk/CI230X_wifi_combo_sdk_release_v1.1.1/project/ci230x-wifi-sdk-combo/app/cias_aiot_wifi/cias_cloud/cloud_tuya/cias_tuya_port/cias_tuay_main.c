#include "FreeRTOS.h"
#include "task.h"
#include "osal/osal.h"
#include "utils/debug/log.h"
#include "utils/reboot_trace/reboot_trace.h"
#include "hal/hal_wdt.h"
#include "ln_nvds.h"
#include "ln_utils.h"

#include "wifi_manager.h"

#include "usr_app.h"
#include "usr_ble_app.h"
#include "ln_ty_ate.h"
#include "ln_ty_sdk_version.h"
#include "tuya_iot_config.h"
#include "cias_log.h"
#define TUYA_CONSOLE 0
extern void console_init(void);


void wait_wifi_semaphore(void)
{
#if 0 //remove by yjd
#include "semphr.h"
    extern xSemaphoreHandle sync_sem; // define in wlan_network.c by nzy

    // waiting to intial sync
    if (sync_sem)
    {
        xSemaphoreTake(sync_sem, portMAX_DELAY);
        vSemaphoreDelete(sync_sem);
        sync_sem = NULL;
    }
#endif //
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void ty_main_task_entry(void *arg)
{
    if (0 != ln_ty_ate_main(ln_ty_ate_mode_check))
    {
        LOG(LOG_LVL_INFO, "Failed to enter ate mode! Start tuya user main.\r\n");
        user_main();
    }

    OS_ThreadDelete(NULL);
}

void usr_work_task_entry(void *arg)
{
    LN_UNUSED(arg);
    uint8_t delay_cnt = 0;
    // Delay output of necessary logs
    OS_MsDelay(2000);
#if (TY_RTC == 1)
    tuya_rtc_init();
#endif
    {
        /**
         * Note: chip-en connected to GND means power-down!!!
        */
        LOG(LOG_LVL_INFO, "reboot type(0:power-on;1:soft-rst;2:wdt-rst): %d\r\n",
                ln_chip_get_reboot_cause());

        /**
         * s_hw_reboot_cnt:
         *     Statistics of restart times without power-down.
         * Note: chip-en connected to GND means power-down!!!
        */
        LOG(LOG_LVL_INFO, "reboot count: %d\r\n", usr_reboot_cnt_get());

        LOG(LOG_LVL_INFO, "[ln_ty_sdk_ver: %s or 0x%08x] [build time:%s]\r\n",
            LN_TY_SDK_VERSION_STRING, LN_TY_SDK_VERSION, LN_TY_SDK_BUILD_DATE_TIME);

        LOG(LOG_LVL_INFO, "[Git info: %s]\r\n", LN_TY_SDK_GIT_INFO);

        LOG(LOG_LVL_INFO, "[BLIB Ver: %s or 0x%08x] [build time:%s]\r\n",
                ble_lib_version_string_get(),
                ble_lib_version_number_get(),
                ble_lib_build_time_get());
    }

    while(1)
    {
        if ((delay_cnt % 10) == 0)
        {
            extern void *heap0_len;
            extern void ln_main_stack_attr_get(uint8_t **start_addr, size_t *size);

            #define HEAP_TOTAL_LEN    (&heap0_len)
            uint8_t *stack_start_addr = NULL;
            size_t   stack_size = 0;
            ln_main_stack_attr_get(&stack_start_addr, &stack_size);

            LOG(LOG_LVL_INFO, 
                "Total:%ld; Free:%ld; main stack:%ld;\r\n", 
                (size_t)HEAP_TOTAL_LEN, OS_GetFreeHeapSize(), stack_size);
        }
#if (TY_RTC == 1)
        {
             platform_rtc_cal();
        }    
#endif
        OS_MsDelay(1000);
        delay_cnt++;
    }

    OS_ThreadDelete(NULL);
}
