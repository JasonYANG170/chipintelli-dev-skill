/**
 * @file tuya_ir_ctrl.h
 * @author zhake@tuya.com
 * @brief tuya Infrared application module
 * @version 1.0.0
 * @date 2021-03-27
 * @copyright Copyright (c) tuya.inc 2021
 */

#ifndef __TUYA_IR_CTRL_H__
#define __TUYA_IR_CTRL_H__

#include "tuya_cloud_types.h"
#include "tuya_ir_drv_struct.h"


#ifdef __cplusplus
extern "C" {
#endif


#define TUYA_IR_DEFAULT_SEND_FREQ               (38000ul)
#define TUYA_IR_REV_NUM_MAX                     (600)
#define TUYA_IR_REV_TIMER_US                    (65000ul)
#define TUYA_IR_REV_MIN_US                      (100)
#define TUYA_IR_REV_MIN_HEAD                    (200)
#define TUYA_IR_REV_MAX_NUM                     (600)
#define TUYA_IR_REV_MAX_IRTIMEDATA              0xFFFF
#define TUYA_IR_REV_LUCKY_TAIL                  (30000)
#define TUYA_IR_REV_TIMEROUT                    (TUYA_IR_REV_TIMER_US<<1)



typedef VOID_T  (*TUYA_IR_SEND_FINISH_CB)(IN UCHAR_T stat, IN VOID *param);
typedef VOID_T  (*TUYA_IR_RECV_FINISH_CB)(IN VOID *param);


typedef enum
{
    TUYA_IR_TX_STAT        = 0,
    TUYA_IR_RX_STAT        = 1,
} TUYA_IR_TXRX_STAT_E;


typedef enum
{
    TUYA_IRCTRL_RX_SUCC       = 0,
    TUYA_IRCTRL_RX_FAIL       = 1,
} TUYA_IRCTRL_RX_RESULT_E;


typedef enum
{
    TUYA_IRCTRL_RX_STATMACH_INIT     = 0,
    TUYA_IRCTRL_RX_STATMACH_DATA     = 1,
} TUYA_IRCTRL_RX_STAT_MACHINE_E;


typedef enum 
{
    TY_IRCTRL_RESULT_SUCCESS     = 0,
    TY_IRCTRL_RESULT_FAILED      = 1,
} TUYA_IRCTRL_SEND_RESULT_E;


typedef struct
{
    UINT_T      freq;
    UCHAR_T     frame_num;
    USHORT_T    data_num;
    USHORT_T    *data;
}TUYA_IRCTRL_DATA_S;


typedef struct
{
    SHORT_T carr_compensation_val;
    SHORT_T lowlevel_compensation_val;
}TUYA_IRCTRL_TX_CFG_S;


typedef struct
{
    USHORT_T                ir_send_idx;
    TUYA_IRCTRL_TX_CFG_S    ir_tx_cfg;
} TUYA_IRCTRL_TX_S;


typedef struct
{
            TUYA_IRCTRL_RX_RESULT_E             rx_result;
            TUYA_IRCTRL_RX_STAT_MACHINE_E       rx_stat_machine;
volatile    UINT_T                              interval_tick;
            UCHAR_T                             clear_tick;
            USHORT_T                            data_num;
            USHORT_T                            *data;
} TUYA_IRCTRL_RX_S;


typedef struct
{
    UCHAR_T                     irctrl_txpin;
    UCHAR_T                     irctrl_rxpin;
    TUYA_IR_SEND_FINISH_CB      irctrl_send_finish_cb;
    TUYA_IR_RECV_FINISH_CB      irctrl_recv_finish_cb;
    VOID_T                      *user_param;
    TUYA_IRCTRL_TX_S            irctrl_tx_info;
    TUYA_IRCTRL_RX_S            irctrl_rx_info;
    TUYA_IRCTRL_DATA_S          *irctrl_irdata;
    TUYA_IR_TXRX_STAT_E         irctrl_ir_stat;
    TUYA_IR_TIM_INFO            irctrl_timinfo;
} TUYA_IR_CTRL_S,*P_TUYA_IR_CTRL_S;

/**************************************************************************************************************************
****************************TUYA Infrared application component production test related API********************************
***************************************************************************************************************************/
/** 
 * @brief: tuya_irctrl_set_carrorlow
 * @desc: Set the infrared transmitting carrier or low level
 * @param[in] enable True:carrier FALSE:low level
 * @note Tuya Infrared production test interface is used to send infrared carrier or send low level
 */
VOID_T tuya_irctrl_set_carrorlow(IN UCHAR_T enable);

/**************************************************************************************************************************
****************************TUYA Infrared application component driver adaptation API*************************************
***************************************************************************************************************************/
/** 
 * @brief: tuya_irctrl_timeout_irq
 * @desc: Infrared timer interrupt handler function
 * @return none
 * @note Called by the developer in the timer interrupt callback function
 */
VOID_T tuya_irctrl_timeout_irq(VOID_T);

/** 
 * @brief: tuya_irctrl_recv_irq
 * @desc: Infrared external interrupt handler function
 * @return none
 * @note Called by developer in external interrupt function callback
 */
VOID_T tuya_irctrl_recv_irq(VOID_T);



#ifdef __cplusplus
} // extern "C"
#endif

#endif

