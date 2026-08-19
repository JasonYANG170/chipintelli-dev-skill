/*
 * @FileName:: 
 * @Author: 
 * @Date: 2023-03-08 09:58:20
 * @LastEditTime: 2023-05-17 13:52:33
 * @Description: #
 */
#ifndef __BLE_IOT_MSG_DEAL_H__
#define __BLE_IOT_MSG_DEAL_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include "FreeRTOS.h"
#include "user_config.h"


typedef enum{
    IR_DEV = 1,          
    AIRCONDITION_DEV,
    RGB_DEV,
    AUDIO_DEV,
    TEABAR_DEV,
    FAN_DEV,
    HEATTABLE_DEV,
    WARMER_DEV,
    WATERHEATED_DEV,
}dev_type_t;

typedef enum{
    ATTRIBUTE_SETUP = 1,
    EVENT_REPORT,
    STATE_QUERY,
    STATE_RECOVERY,
}data_type_t;

typedef enum{
    FUN_TURN_OFF = 1,
    FUN_TURN_ON,
}dev_function_t;

typedef void (*pbledataCallBack)(unsigned char); 

void dev_state_init(void);
void uart1_send_str(char *str);
void ci_ble_recv_task(void);
void deal_ble_send_msg(uint16_t cmd_id);

void ci_rf_recv_data_handle(uint8_t* recv_data, uint8_t len);
void custom_rf_recv_data_handle(uint8_t* recv_data, uint8_t len);
void uart_send_asr(uint16_t cmd_id);
void usr_send_asr_result(uint16_t cmd_id);
void app_Regist_CallBack(pbledataCallBack dbleCB);

#endif
