/**
 * @file tuya_ir_app.h
 * @author zhake@tuya.com
 * @brief tuya Infrared application module
 * @version 1.0.0
 * @date 2021-03-27
 * @copyright Copyright (c) tuya.inc 2021
 */

#ifndef __TUYA_IR_APP_H_
#define __TUYA_IR_APP_H_


#include "tuya_cloud_types.h"
#include "tuya_ir_drv_struct.h"
#include "tuya_ir_control.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    TY_IR_SEND_START_STAT         = 0,
    TY_IR_SEND_FINISH_STAT        = 1,
    TY_IR_STUDY_START_STAT        = 2,
    TY_IR_STUDY_FINISH_STAT       = 3,
}TUYA_IR_STAT_E;

typedef VOID_T (*TUYA_IR_STAT_CB)(TUYA_IR_STAT_E ir_stat);
typedef VOID_T (*TUYA_IR_DATARECV_CB)(IN TY_IR_CODE_S *data);

typedef struct
{
    SHORT_T carr_compensation_val;              //Infrared carrier transmission compensation value
    SHORT_T lowlevel_compensation_val;          //Infrared low level transmission compensation value
}TUYA_IRAPP_TX_CFG_S;

typedef struct
{
    UCHAR_T                 txpin;                  //Infrared send pin
    UCHAR_T                 rxpin;                  //Infrared recv pin
    TUYA_IR_STAT_CB         ir_stat_cb;             //Infrared sending learning status callback
    TUYA_IRAPP_TX_CFG_S     ir_tx_cfg;              //Infrared send compensation setting
} TUYA_IRAPP_CFG_S,*P_TUYA_IRAPP_CFG_S;



/**************************************************************************************************************************
****************************TUYA Infrared application component common interface API/*************************************
***************************************************************************************************************************/

/** 
 * @brief: tuya_irapp_module_init
 * @desc: tuya Infrared function initialization
 * @param[in] TUYA_IRAPP_CFG_S: p_ir_cfg
 * @param[in] TUYA_IR_DRV_OPTS: p_ir_opts
 * @return OPERATE_RET
 * @note none
 */
OPERATE_RET tuya_irapp_module_init(IN TUYA_IRAPP_CFG_S *p_ir_cfg,IN TUYA_IR_DRV_OPTS *p_ir_opts);


/**************************************************************************************************************************
****************************TUYA Infrared application component production test related API/*******************************
***************************************************************************************************************************/

/** 
 * @brief: tuya_irapp_irdata_send
 * @desc: tuya Infrared data transmission
 * @param[in] ir_code: infrared data 
 * @param[in] code_num: num of ir_code
 * @return VOID_T
 * @note Tuya infrared production test interface is used to test the infrared transmission, 
 * and the infrared data can be sent by filling the infrared structure
 */
VOID_T tuya_irapp_irsend_ctrl(IN TY_IR_CODE_S *ir_code, IN UCHAR_T code_num);

/** 
 * @brief: tuya_irapp_irstudy_ctrl
 * @desc: tuya Infrared learning state control
 * @param[in] irstudy_mode: Infrared learning state
 * @return VOID_T
 * @note Tuya Infrared production test function is used to set infrared entry or exit learning. 
 * For entering infrared learning, you must call tuya_irapp_set_irdata_recv_cb() function to register infrared learning callback to obtain infrared learning data;
 * For exiting infrared learning, you must call tuya_irapp_set_irdata_recv_cb(NULL) to unregister infrared learning callback
 */
VOID_T tuya_irapp_irstudy_ctrl(IN TY_IR_STUDY_CTL_E irstudy_mode);

/** 
 * @brief: tuya_irapp_set_irdata_recv_cb
 * @desc: tuya Infrared learning data callback
 * @param[in] TUYA_IR_DATARECV_CB: Infrared learning data callback function pointer
 * @return VOID_T
 * @note Tuya infrared production test interface is used to register the infrared data receiving callback to work with tuya_irapp_irstudy_ctrl API. 
 * The normal operation of the infrared must be guaranteed tuya_irapp_set_irdata_recv_cb (NULL)
 */
VOID_T tuya_irapp_set_irdata_recv_cb(IN TUYA_IR_DATARECV_CB cb);

/** 
 * @brief: tuya_irapp_is_irsend_stat
 * @desc: Gets the infrared transmission status
 * @return True OR False
 * @note Get real-time whether infrared is in the sending state
 */
CHAR_T tuya_irapp_is_irsend_stat(VOID_T);



#ifdef __cplusplus
} // extern "C"
#endif

#endif

