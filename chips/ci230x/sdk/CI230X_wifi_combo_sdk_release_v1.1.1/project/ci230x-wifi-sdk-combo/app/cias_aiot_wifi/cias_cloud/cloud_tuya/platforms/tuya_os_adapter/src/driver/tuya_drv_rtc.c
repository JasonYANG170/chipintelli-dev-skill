 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 *                                                                            *
 =============================================================================*/

/*============================ INCLUDES ======================================*/
#include "tuya_rtc.h"
#include "tuya_os_adapter_error_code.h"
#include "osal/osal.h"
#include "hal/hal_rtc.h"
#include "hal/hal_misc.h"
/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
static int rtc_dev_init      (void);
static int rtc_dev_time_get  (time_t *timestamp);
static int rtc_dev_time_set  (time_t  timestamp);
static int rtc_dev_control   (uint8_t cmd, void *arg);
static int rtc_dev_deinit    (void);

/*============================ LOCAL VARIABLES ===============================*/
static const tuya_rtc_ops_t  rtc_dev_ops = {
    .init       = rtc_dev_init,
    .time_get   = rtc_dev_time_get,
    .time_set   = rtc_dev_time_set,
    .control    = rtc_dev_control,
    .deinit     = rtc_dev_deinit,
};

static time_t g_time_offset         = 0;   //unit: ms 
static uint32_t rtc_ticks_ms        = 0;   //unit: ms 
static uint16_t rco32k_period       = 0;   //unit: ns
static volatile uint8_t  rtc_cal_done_flag   = 0;
static uint32_t rtc_match_val       = 0;

#define RTC_TICK_TIME       1000    //unit: ms  

void RTC_IRQHandler()
{
   if(hal_rtc_get_it_flag(RTC_BASE, RTC_IT_FLAG_ACTIVE) == 1)
   {     
        rtc_ticks_ms += RTC_TICK_TIME;       
        hal_rtc_clr_it_flag(RTC_BASE, RTC_IT_FLAG_ACTIVE); 
        if(rtc_cal_done_flag == 1){
            //hal_rtc_set_cnt_match(RTC_BASE, g_rtc_match_val); 
            *(volatile uint32_t*)0x40101004 = rtc_match_val;
            rtc_cal_done_flag = 0;
        }    
   }
}

static uint32_t get_rtc_ms(void)
{
    return (rtc_ticks_ms + (hal_rtc_get_cnt(RTC_BASE) * rtc_match_val) / 1000000);
}
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static int rtc_dev_init(void)
{
    hal_misc_awo_set_o_cpu_sleep_counter_bp(0);
    OS_MsDelay(1);
    rco32k_period = hal_misc_awo_get_i_rco32k_period_ns();
    
    rtc_init_t_def rtc_init;
    rtc_init.rtc_warp_en = RTC_WRAP_EN;                      
    hal_rtc_init(RTC_BASE, &rtc_init);
    
    hal_rtc_set_cnt_match(RTC_BASE, ((RTC_TICK_TIME * 1000000.0f) / rco32k_period) + 0.5f  + 1);                        
    hal_rtc_set_cnt_load(RTC_BASE, 0);                           
    hal_rtc_it_cfg(RTC_BASE, RTC_IT_FLAG_ACTIVE, HAL_ENABLE);    
    
    NVIC_SetPriority(RTC_IRQn, 4);
    NVIC_EnableIRQ(RTC_IRQn);
    
    hal_rtc_en(RTC_BASE, HAL_ENABLE); 

    rtc_ticks_ms = 0;
    g_time_offset = 0;
    rtc_cal_done_flag = 0;

    return OPRT_OS_ADAPTER_OK;
}

static int rtc_dev_time_get(time_t *timestamp)
{
    *timestamp = (get_rtc_ms() + g_time_offset) / 1000;
    return OPRT_OS_ADAPTER_OK;
}

static int rtc_dev_time_set(time_t timestamp)
{
    g_time_offset = timestamp * 1000 - get_rtc_ms();
    return OPRT_OS_ADAPTER_OK;
}

static int rtc_dev_control(uint8_t cmd, void *arg)
{
    return OPRT_OS_ADAPTER_OK;

}

static int rtc_dev_deinit(void)
{
    g_time_offset = 0;
    hal_rtc_en(RTC_BASE, HAL_DISABLE);
    hal_rtc_deinit();
    return OPRT_OS_ADAPTER_OK;
}

int platform_rtc_init(void)
{
    tuya_rtc_register(&rtc_dev_ops);

    return OPRT_OS_ADAPTER_OK;
}

uint8_t platform_rtc_cal(void)
{
    uint8_t ret = 0;
    if(rtc_cal_done_flag == 0)
    {
        hal_misc_awo_set_o_cpu_sleep_counter_bp(0);
        OS_MsDelay(1);
        rtc_match_val = ((RTC_TICK_TIME * 1000000.0f) / hal_misc_awo_get_i_rco32k_period_ns() + 0.5f) + 1;
        rtc_cal_done_flag = 1;
        ret = 1;
    }
    return ret;
}
