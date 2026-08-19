/**
 * @file tuya_os_adapt_system.c
 * @brief 操作系统相关接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */
#define _UNI_SYSTEM_GLOBAL

#include "tuya_os_adapt_system.h"
#include "tuya_os_adapter_error_code.h"

#include "hal/hal_trng.h"
#include "hal/hal_wdt.h"
#include "osal/osal.h"
// #include "hal/hal_sleep.h"

#include "reboot_trace/reboot_trace.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
unsigned int tuya_os_adapt_watchdog_init_start(const unsigned int timeval);
void tuya_os_adapt_watchdog_refresh(void);
void tuya_os_adapt_watchdog_stop(void);


#define SERIAL_NUM_LEN 32

//realtek supports 0,1,5
typedef enum {
    REASON_CPU_RESET_HAPPEN = 0,/*!< &&&&&&& 0 hardware reset or system reset (distinguish from 'REASON_SYS_RESET_HAPPEN' by Software) &&&&&&&77*/
    REASON_BOR2_RESET_HAPPEN,/*!<&&&&&&&& 1 watchdog reset **^&&&&&&&*/
    REASON_RTC_RESTORE, /*!< 2 this is SW set bit after rtc init */
    REASON_UARTBURN_BOOT,/*!< 3 this is SW set bit before reboot, for uart download */
    REASON_UARTBURN_DEBUG,/*!< 4 this is SW set bit before reboot, for uart download debug */
    REASON_SYS_RESET_HAPPEN,/*!<  &&&&&&&& 5 this is SW set bit before reboot, for distinguish 'REASON_CPU_RESET_HAPPEN'&&&&&&&&&&& */
    REASON_BOR2_RESET_TEMP, /*!<  BOR2 HW temp bit */
    REASON_SYS_BOR_DETECION /*!<  1: enable bor2 detection;  0: disable */
} RST_REASON_E;

/***********************************************************
*************************variable define********************
***********************************************************/
bool is_lp_enable = false;

static const TUYA_OS_SYSTEM_INTF m_tuya_os_system_intfs = {
    .get_systemtickcount   = tuya_os_adapt_get_systemtickcount,
    .get_tickratems        = tuya_os_adapt_get_tickratems,
    .system_sleep          = tuya_os_adapt_system_sleep,
    .system_reset          = tuya_os_adapt_system_reset,
    .watchdog_init_start   = tuya_os_adapt_watchdog_init_start,
    .watchdog_refresh      = tuya_os_adapt_watchdog_refresh,
    .watchdog_stop         = tuya_os_adapt_watchdog_stop,
    .system_getheapsize    = tuya_os_adapt_system_getheapsize,
    .system_get_rst_info   = tuya_os_adapt_system_get_rst_info,
    .system_get_rst_ext_info = NULL,
    .get_random_data       = tuya_os_adapt_get_random_data,
    .set_cpu_lp_mode       = tuya_os_adapt_set_cpu_lp_mode,
};

/***********************************************************
*************************extern define********************
***********************************************************/
extern void ota_platform_reset(void);
extern void srand(unsigned int seed);
extern int rand(void);

/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief tuya_os_adapt_get_systemtickcount用于获取系统运行ticket 
 * @return SYS_TICK_T 
 */
SYS_TICK_T tuya_os_adapt_get_systemtickcount(void)
{
    return (SYS_TICK_T)OS_GetTicks();
}


/**
 * @brief tuya_os_adapt_get_tickratems用于获取系统ticket是多少个ms
 * 
 * @return the time is ms of a system ticket
 */
unsigned int tuya_os_adapt_get_tickratems(void)
{
    return (unsigned int)(1000/OS_HZ);
}


/**
 * @brief tuya_os_adapt_system_sleep用于系统sleep
 * 
 * @param[in] msTime sleep time is ms
 */
void tuya_os_adapt_system_sleep(const unsigned long msTime)
{
    OS_MsDelay(msTime);
}


/**
 * @brief tuya_os_adapt_system_isrstatus用于检查系统是否处于中断上下文
 * 
 * @return true 
 * @return false 
 */
bool tuya_os_adapt_system_isrstatus(void)
{
    return (bool)OS_IsISRContext();
}


/**
 * @brief tuya_os_adapt_system_reset用于重启系统
 * 
 */
void tuya_os_adapt_system_reset(void)
{
    ln_chip_reboot();
}

/**
 * @brief tuya_os_adapt_system_getheapsize用于获取堆大小/剩余内存大小
 * 
 * @return int <0: don't support  >=0: current heap size/free memory
 */
int tuya_os_adapt_system_getheapsize(void)
{
    return (int)OS_GetFreeHeapSize();
}

/**
 * @brief tuya_os_adapt_system_getMiniheapsize/最小剩余内存大小
 * 
 * @return int <0: don't support  >=0: mini heap size/free memory
 */
int tuya_os_adapt_system_getMiniheapsize(void)
{
    // to do
}

/**
 * @brief tuya_os_adapt_system_get_rst_info用于获取硬件重启原因
 * 
 * @return 硬件重启原因
 */
TY_RST_REASON_E tuya_os_adapt_system_get_rst_info(void)
{
    TY_RST_REASON_E reason;
    chip_reboot_cause_t cause;
    cause = ln_chip_get_reboot_cause();
    switch (cause)
    {
        case CHIP_REBOOT_POWER_ON:
            reason = TY_RST_POWER_OFF;
            break;
        case CHIP_REBOOT_SOFTWARE:
            reason = TY_RST_SOFTWARE;
            break;
        case CHIP_REBOOT_WATCHDOG:
            reason = TY_RST_HARDWARE_WATCHDOG;
            break;
        default:
            reason = TY_RST_OTHER;
            break;
    }
    return reason;
}

/**
 * @brief init random
 *
 * @param  void
 * @retval void
 */
void tuya_os_adapt_srandom(void)
{
    srand(tuya_os_adapt_get_systemtickcount());
}

/**
 * @brief tuya_os_adapt_get_random_data用于获取指定条件下的随机数
 * 
 * @param[in] range 
 * @return 随机值
 */
int tuya_os_adapt_get_random_data(const unsigned int range)
{
    unsigned int trngNumber = 0;
    unsigned int random_num[4]; 

    if (0 == hal_trng_get_en_status(TRNG_BASE))
    {
        trng_init_t_def trng_init;
        memset(&trng_init, 0, sizeof(trng_init));
        trng_init.trng_fast_mode_en_status = TRNG_FAST_MODE_DIS;
        trng_init.trng_gap = 10;

        hal_trng_init(TRNG_BASE, &trng_init);
        hal_trng_en(TRNG_BASE, HAL_ENABLE);
    }

    while(hal_trng_get_en_status(TRNG_BASE) == 1);
    while(hal_trng_get_it_flag(TRNG_BASE,TRNG_IT_FLAG_TRNG_DONE) == 0)
       
    random_num[0] = hal_trng_get_data_0_31(TRNG_BASE);
    random_num[1] = hal_trng_get_data_32_63(TRNG_BASE);
    random_num[2] = hal_trng_get_data_64_95(TRNG_BASE);
    random_num[3] = hal_trng_get_data_96_127(TRNG_BASE);
    
    if (range >= 96)
    {
        return random_num[3] % range;
    }
    else if (range >= 64)
    {
        return random_num[2] % range;
    }
    else if (range >= 32)
    {
        return random_num[1] % range;
    }

    return random_num[0] % range;
}

/**
 * @brief tuya_os_adapt_set_cpu_lp_mode用于设置cpu的低功耗模式
 * 
 * @param[in] en 
 * @param[in] mode
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_set_cpu_lp_mode(const bool_t en, const TY_CPU_SLEEP_MODE_E mode)
{
    // sleep_mode_enum_t sleep_mode = ACTIVE;

    // if(mode < TY_CPU_SLEEP || mode > TY_CPU_DEEP_SLEEP){
    //     return OPRT_OS_ADAPTER_INVALID_PARM;
    // }
    // if(en){
    //     if(mode == TY_CPU_SLEEP){
    //         sleep_mode = LIGHT_SLEEP;
    //     }else{
    //         sleep_mode = DEEP_SLEEP;
    //     }
    // }else{
    //     sleep_mode = ACTIVE;
    // }
    // hal_sleep_set_mode(sleep_mode);
    // return OPRT_OS_ADAPTER_OK;

    #warning "TODO: tuya_hal_set_cpu_lp_mode"

    return OPRT_OS_ADAPTER_OK;
}

// void WDT_IRQHandler(void)
// {
// }

/**
 * @brief 用于初始化并运行watchdog
 * 
 * @param[in] timeval watch dog检测时间间隔：如果timeval大于看门狗的
 * 最大可设置时间，则使用平台可设置时间的最大值，并且返回该最大值
 * @return int [out] 实际设置的看门狗时间
 */
unsigned int tuya_os_adapt_watchdog_init_start(const unsigned int timeval)
{
    int i = 0;
    // unsigned int period = 1000 * timeval; /* WDT_RMOD_0 */
    unsigned int period = 1000 * timeval / 2; /* WDT_RMOD_1 */

    wdt_init_t_def wdt_init;
    wdt_top_value_t top = WDT_TOP_VALUE_8;
    const unsigned int watchdog_period[]=
            {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144};

    for (; i < (sizeof(watchdog_period) / sizeof(unsigned int)); i++)
    {
        if (watchdog_period[i] >= period)
        {
            top = (wdt_top_value_t)i;
            break;
        }
    }

    memset(&wdt_init,0,sizeof(wdt_init));

    wdt_init.wdt_rmod = WDT_RMOD_1;     // 等于 0 的时候，计数器溢出时直接复位，等于 1 的时候，先产生中断，如果再次溢出，则产生复位。
    wdt_init.wdt_rpl = WDT_RPL_32_PCLK; // 设置复位延时的时间
    wdt_init.top = top;
    hal_wdt_init(WDT_BASE, &wdt_init);

    NVIC_EnableIRQ(WDT_IRQn);
    hal_wdt_en(WDT_BASE,HAL_ENABLE);

    hal_wdt_cnt_restart(WDT_BASE);
    
    return watchdog_period[i] * 2 / 1000;
}

/**
 * @brief 用于刷新watch dog
 * 
 */
void tuya_os_adapt_watchdog_refresh(void)
{
    hal_wdt_cnt_restart(WDT_BASE);
}

/**
 * @brief 用于停止watch dog
 * 
 */
void tuya_os_adapt_watchdog_stop(void)
{
    /* Can not stop watchdog. */
}

int tuya_os_adapt_reg_system_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_SYSTEM, (void *)&m_tuya_os_system_intfs);
}
