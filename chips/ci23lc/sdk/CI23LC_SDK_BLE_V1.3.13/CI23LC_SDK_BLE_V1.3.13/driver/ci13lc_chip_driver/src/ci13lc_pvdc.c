/**
 * @file ci13lc_pvdc.c
 * @brief  看门狗驱动文件
 * @version 0.1
 * @date 2024-04-019
 *
 * @copyright Copyright (c) 2024  Chipintelli Technology Co., Ltd.
 *
 */
#include "ci_pvdc.h"

/**
 * @brief 寄存器解锁
 * 
 */
void pvdc_reg_unlock()
{
    PVDC->PVDC_LOCK = 0x5a5a5a5a;
}

/**
 * @brief 寄存器上锁
 * 
 */
void pvdc_reg_lock()
{
    PVDC->PVDC_LOCK = 0;
}

/**
 * @brief 读pvdc原始中断状态
 * 
 * @param intr ，中断选择
 */
uint8_t pvdc_raw_irq_status(pvdc_intr_t intr)
{
    return (PVDC->PVDC_INTR_RAW & (0x1 << intr));
}

/**
 * @brief 读屏蔽中断状态
 * 
 * @param intr ，中断选择
 */
uint8_t pvdc_mask_irq_status(pvdc_intr_t intr)
{
    return (PVDC->PVDC_INTR & (0x1 << intr)) >> intr;
}

/**
 * @brief 清除中断状态
 * 
 * @param intr ，中断选择
 */
void pvdc_clear_irq(pvdc_intr_t intr)
{
    //PVDC->PVDC_INTR |= (0x1 << intr);
    PVDC->PVDC_INTR_RAW |= (0x1 << intr);
}

/**
 * @brief 配置中断屏蔽
 * 
 * @param en：ENABLE，屏蔽；DISABLE，不屏蔽
 */
void pvdc_irq_mask(pvdc_intr_t intr,FunctionalState en)
{
    PVDC->PVDC_INTR_MASK &= ~(0x1 << intr);
    PVDC->PVDC_INTR_MASK |= (en << intr);
}

/**
 * @brief 读第1次中断电压比较结果
 * 
 * @return 0 ~ 1
 */
pvdc_result_t pvdc_get_irq_pvd_result()
{
    return (PVDC->PVDC_VOL & (0x1 << 3)) >> 3;
}

/**
 * @brief 读第1次中断电压值
 * 
 * @return 0 ~ 7（2.4V ~ 3.1V，step is 100mV）
 */
pvdc_vol_t pvdc_get_irq_vol()
{
    return (PVDC->PVDC_VOL & (0x7 << 0));
}

/**
 * @brief pvdc使能
 * 
 * @param en：ENABLE，打开；DISABLE，关闭
 */
void pvdc_enable(FunctionalState en)
{
    PVDC->PVDC_EN &= ~(0x1 << 0);
    PVDC->PVDC_EN |= (en << 0);
}

/**
 * @brief 设置高阈值电压
 * 
 * @param threshold：0 ~ 7（2.4V ~ 3.1V，step is 100mV）
 */
void pvdc_set_vol_high_threshold(pvdc_vol_t threshold)
{
    PVDC->PVDC_TH &= ~(0x7 << 4);
    PVDC->PVDC_TH |= (threshold << 4);
}

/**
 * @brief 设置低阈值电压
 * 
 * @param threshold：0 ~ 7（2.4V ~ 3.1V，step is 100mV）
 */
void pvdc_set_vol_low_threshold(pvdc_vol_t threshold)
{
    PVDC->PVDC_TH &= ~(0x7 << 0);
    PVDC->PVDC_TH |= (threshold << 0);
}

/**
 * @brief 读扫描实时电压比较结果
 * 
 * @return 0 ~ 1
 */
pvdc_result_t pvdc_get_current_pvd_result()
{
    return (PVDC->PVDC_CFG_VOL & (0x1 << 3)) >> 3;
}

/**
 * @brief 读扫描实时电压值
 * 
 * @return 0 ~ 7（2.4V ~ 3.1V，step is 100mV）
 */
pvdc_vol_t pvdc_get_current_vol()
{
    return (PVDC->PVDC_CFG_VOL & (0x7 << 0));
}

/**
 * @brief 配置扫描间隔时间
 *        以8K为时间单位，扫描间隔 = 125 us * （time + 1）
 * 
 * @param time：0 ~ 4095
 */
void pvdc_scan_interval_time(uint16_t time)
{
    PVDC->PVDC_TIME &= ~(0xFFF << 0);
    PVDC->PVDC_TIME |= (time << 0);
}

/**
 * @brief 当模拟电压值一直在Vth和Vtl之间跳变时，最大等待次数
 * 
 * @param times：0 ~ 15
 */
void pvdc_max_wait_times(uint16_t times)
{
    PVDC->PVDC_MAX_VAL &= ~(0xF << 0);
    PVDC->PVDC_MAX_VAL |= (times << 0);
}

void PVDC_IRQHandler()
{
    if(pvdc_mask_irq_status(PVDC_INTR_BELOW_LOW))
    {
        pvdc_clear_irq(PVDC_INTR_BELOW_LOW);
    }
    if(pvdc_mask_irq_status(PVDC_INTR_BETWEEN_LOW_HIGH))
    {
        pvdc_clear_irq(PVDC_INTR_BETWEEN_LOW_HIGH);
    }
    if(pvdc_mask_irq_status(PVDC_INTR_ABOVE_HIGH))
    {
        pvdc_clear_irq(PVDC_INTR_ABOVE_HIGH);
    }
}
/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/