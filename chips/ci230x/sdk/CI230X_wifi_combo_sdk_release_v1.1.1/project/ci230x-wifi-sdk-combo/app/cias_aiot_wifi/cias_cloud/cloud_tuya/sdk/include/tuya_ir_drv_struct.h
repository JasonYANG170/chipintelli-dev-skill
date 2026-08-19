/**
 * @file tuya_ir_drv_struct.h
 * @author zhake@tuya.com
 * @brief tuya Infrared drive adapter structure
 * @version 1.0.0
 * @date 2021-03-27
 * @copyright Copyright (c) tuya.inc 2021
 */

#ifndef __TUYA_IR_DRV_STRUCT_H__
#define __TUYA_IR_DRV_STRUCT_H__

#include "tuya_cloud_types.h"


#ifdef __cplusplus
extern "C" {
#endif



typedef enum
{
    TIM_COUNT_UP_MODE   = 0,
    TIM_COUNT_DOWN_MODE = 1,
    TIM_INVALID_MODE    = 2,
}TIM_COUNT_MODE_E;


typedef struct
{
    FLOAT_T timer_mhz;               //Timer frequency unit mhz
    TIM_COUNT_MODE_E count_mode;     //Timer counting method
}TUYA_IR_TIM_INFO,*P_TUYA_IR_TIM_INFO;


typedef struct
{
    VOID_T  (*tuya_ir_drv_timer_init)(VOID_T);
    VOID_T  (*tuya_ir_drv_timer_start)(IN UINT_T tim_us);
    VOID_T  (*tuya_ir_drv_timer_stop)(VOID_T);
    UINT_T  (*tuya_ir_drv_timer_getcount)(VOID_T);
    VOID_T  (*tuya_ir_drv_timer_getinfo)(INOUT TUYA_IR_TIM_INFO *tim_info);

    VOID_T  (*tuya_ir_drv_pwm_init)(IN UCHAR_T ir_txpin,IN UINT_T freq);
    VOID_T  (*tuya_ir_drv_pwm_setfreq)(IN UINT_T freq);
    VOID_T  (*tuya_ir_drv_pwm_send)(VOID_T);
    VOID_T  (*tuya_ir_drv_pwm_stop)(VOID_T);
    
    VOID_T  (*tuya_ir_drv_gpioirq_init)(IN UCHAR_T ir_rxpin);
    VOID_T  (*tuya_ir_drv_gpioirq_en)(IN UCHAR_T is_enable);

    //Infrared driver is not necessary for different platform initialization and related callback interfaces
    VOID_T  (*tuya_ir_drv_plat_related_init)(VOID_T);
    VOID_T  (*tuya_ir_drv_irtx_start_cb)(VOID_T);
    VOID_T  (*tuya_ir_drv_irtx_finish_cb)(VOID_T);
    VOID_T  (*tuya_ir_drv_irrx_start_cb)(VOID_T);
    VOID_T  (*tuya_ir_drv_irrx_finish_cb)(VOID_T);
}TUYA_IR_DRV_OPTS,*P_TUYA_IR_DRV_OPTS;



#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TUYA_IR_MODULE_STRUCT_H__

