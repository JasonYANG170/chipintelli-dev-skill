/**
 * @file ci13lc_iwdg.h
 * @brief  看门狗驱动文件
 * @version 0.1
 * @date 2024-04-19
 *
 * @copyright Copyright (c) 2024  Chipintelli Technology Co., Ltd.
 *
 */

#ifndef _CI13LC_PVDC_H_
#define _CI13LC_PVDC_H_

#include "ci_system.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    PVDC_INTR_BELOW_LOW        = 0, /*当前电压低于低阈值电压*/
    PVDC_INTR_BETWEEN_LOW_HIGH = 1, /*当前电压在低阈值和高阈值电压之间*/
    PVDC_INTR_ABOVE_HIGH       = 2, /*当前电压高于高阈值电压*/
}pvdc_intr_t;

typedef enum
{
    PVDC_ANALOG_BELOW_CURRENT  = 0, /*模拟电源小于当前扫描值*/
    PVDC_ANALOG_ABOVE_CURRENT  = 1, /*模拟电源大于当前扫描值*/
}pvdc_result_t;

typedef enum
{
    PVDC_VOL_2_4  = 0, /*2.4V*/
    PVDC_VOL_2_5  = 1, /*2.5V*/
    PVDC_VOL_2_6  = 2, /*2.6V*/
    PVDC_VOL_2_7  = 3, /*2.7V*/
    PVDC_VOL_2_8  = 4, /*2.8V*/
    PVDC_VOL_2_9  = 5, /*2.9V*/
    PVDC_VOL_3_0  = 6, /*3.0V*/
    PVDC_VOL_3_1  = 7, /*3.1V*/
}pvdc_vol_t;

void pvdc_reg_unlock();
void pvdc_reg_lock();
uint8_t pvdc_raw_irq_status(pvdc_intr_t intr);
uint8_t pvdc_mask_irq_status(pvdc_intr_t intr);
void pvdc_clear_irq(pvdc_intr_t intr);
void pvdc_irq_mask(pvdc_intr_t intr,FunctionalState en);
pvdc_result_t pvdc_get_irq_pvd_result();
pvdc_vol_t pvdc_get_irq_vol();
void pvdc_enable(FunctionalState en);
void pvdc_set_vol_high_threshold(pvdc_vol_t threshold);
void pvdc_set_vol_low_threshold(pvdc_vol_t threshold);
pvdc_result_t pvdc_get_current_pvd_result();
pvdc_vol_t pvdc_get_current_vol();
void pvdc_scan_interval_time(uint16_t time);
void pvdc_max_wait_times(uint16_t times);


#ifdef __cplusplus
}
#endif

#endif