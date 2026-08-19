#ifndef __CIAS_AUDIO_DATA_HANDLE_H__
#define __CIAS_AUDIO_DATA_HANDLE_H__
#include "ci130x_scu.h"
#include "ci130x_uart.h"
#include "user_config.h"
#include "ci_nvdata_manage.h"

#pragma pack(1)
typedef struct
{
    bool status;
    bool ota_chip_type_1306;
    uint32_t timeout;
    uint32_t retry_time;
    uint32_t log_uart_number;
    uint32_t ota_uart_number;
    UART_BaudRate ota_uart_baud;
}ota_nv_status_t; 
#pragma pack() 
bool cias_online_func_init(void);
#if AUDIO_DATA_PLAY_BY_UART
bool audio_player_param_init(void);
void request_play_data_func(void);
#endif
cinv_item_ret_t write_ota_mcu_status(bool status);
#endif   //__CIAS_AUDIO_DATA_HANDLE_H__
