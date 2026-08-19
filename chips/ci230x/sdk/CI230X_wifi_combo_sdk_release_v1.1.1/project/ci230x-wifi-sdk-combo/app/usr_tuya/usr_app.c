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
#include "uni_log.h"
#include "tuya_iot_config.h"
#include "cias_log.h"
#define USR_WORK_THREAD_STACK_SIZE 4 * 256 //Byte

#if (TY_RTC == 1)
extern uint8_t platform_rtc_cal(void);
#endif

static void usr_work_task_entry(void *arg)
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
        LOG(LOG_LVL_INFO, "--reboot type(0:power-on;1:soft-rst;2:wdt-rst): %d\r\n",
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
    while (1)
    {
    
        if ((delay_cnt % 10) == 0)
        {
            
            extern void *heap0_len;
            extern void ln_main_stack_attr_get(uint8_t * *start_addr, size_t * size);

#define HEAP_TOTAL_LEN (&heap0_len)
            uint8_t *stack_start_addr = NULL;
            size_t stack_size = 0;
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

static void ty_main_task_entry(void *arg)
{
     LN_UNUSED(arg);

#if (LN_SELF_ATE_TEST_MODE)
    LOG(LOG_LVL_INFO, "Enter ate mode!\r\n");
    ln_nvds_set_ate_result(NV9_ATE_RESULT_OK);
    ln_ty_ate_main(self_test_ty_ate_mode_check);
#elif (LN_SELF_NORMAL_ADD_ATE_UART)
    LOG(LOG_LVL_INFO, "Enter Normal mode, but have ate uart cmd!\r\n");
    user_main();
    extern int ln_ty_ate_uart_test_use_start(void);
    ln_ty_ate_uart_test_use_start();
#else
    if (0 != ln_ty_ate_main(ln_ty_ate_mode_check)) {
        LOG(LOG_LVL_INFO, "Failed to enter ate mode! Start tuya user main.\r\n");
        user_main();
    }
#endif /* LN_SELF_ATE_TEST_MODE */
    SetLogManageAttr(TY_LOG_LEVEL_INFO);

    OS_ThreadDelete(NULL);
}
#include "hal/hal_efuse.h"
#include "cias_communication.h"
#include "cias_slave_message_handle.h"
#include "cias_system_manage.h"
#include "cias_tuya_ir_protocol_handle.h"
int creat_usr_app_task(void)
{
    /**
     * create init task
    */
    {
        OS_Thread_t work_task = {0};
        if (OS_OK != OS_ThreadCreate(&work_task, "worker",
                                     usr_work_task_entry, NULL,
                                     OS_PRIORITY_BELOW_NORMAL, USR_WORK_THREAD_STACK_SIZE))
        {
            LOG(LOG_LVL_ERROR, "Failed to create cal task!\r\n");
        }
    }
//5.communicatio port（for audio） init
#if 1
    if (communication_port_init() != CIAS_OK)
    {
        LOG(LOG_LVL_ERROR, "communication init for audio failed !!!\r\n");
        return CIAS_FAIL;
    }
    if( cias_slave_message_handle_init() != CIAS_OK)
    {
        LOG(LOG_LVL_ERROR, "cias_slave_message_handle_init call error !!!\r\n");
        return CIAS_FAIL;
    }
    uart1_tx_mutex_init();
#endif
    tuya_ir_protocol_init();
    tuya_air_conditioner_timer_init(60);  //60S软中断一次
    update_local_ir_code_info();   //上报一次本地状态
#if CIAS_SYSTEM_MANAGE_ENABLE
    cias_system_manage();
#endif
    {
#define TY_MAIN_TASK_STACK_SIZE (6 * 1024)   //最小需要6K的栈空间，保证涂鸦任务稳定执行
        OS_Thread_t ty_main_thr = {0};
        if (OS_OK != OS_ThreadCreate(&ty_main_thr, "ty_main", ty_main_task_entry,
                                     NULL, OS_PRIORITY_NORMAL, TY_MAIN_TASK_STACK_SIZE))
        {
            LOG(LOG_LVL_ERROR, "Failed to create TY task!\r\n");
        }
    }
    return CIAS_OK;
}

void ln_wdt_start(void)
{
    wdt_init_t_def wdt_init;
    memset(&wdt_init, 0, sizeof(wdt_init));
    wdt_init.wdt_rmod = WDT_RMOD_1;
    wdt_init.wdt_rpl = WDT_RPL_32_PCLK;
    wdt_init.top = WDT_TOP_VALUE_10;
    hal_wdt_init(WDT_BASE, &wdt_init);

    NVIC_EnableIRQ(WDT_IRQn);

    hal_wdt_en(WDT_BASE, HAL_ENABLE);
    hal_wdt_cnt_restart(WDT_BASE);
}

void WDT_IRQHandler()
{
}
