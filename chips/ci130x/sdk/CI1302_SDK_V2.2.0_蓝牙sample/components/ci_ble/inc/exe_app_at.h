/*
 * @FileName:: 
 * @Author: 
 * @Date: 2023-05-27 13:56:39
 * @LastEditTime: 2023-05-31 09:31:18
 * @Description: 
 */
#ifndef __APP_AT_H__
#define __APP_AT_H__

#include "user_config.h"

#define NVDATA_ID_BLE_MAC               0x70000001
#define NVDATA_ID_BLE_NAME              0x80000001
#define NVDATA_ID_BLE_PWR               0x90000001
#define NVDATA_ID_BLE_XTAL              0x90000004
#define NVDATA_ID_BLE_MODE              0x90000008
#define NVDATA_ID_BLE_CH                0x90000010
#define NVDATA_ID_BLE_SN                0x90000014

#define DEFAULT_MAC          "FF:FF:FF:FF:FF:FF"
#define DEFAULT_NAME         "CI_BLE"
#define DEFAULT_PWR          7
#define DEFAULT_XTAL         8 

bool updata_ble_mac(uint8_t *recv_data, uint8_t recv_len);
bool updata_ble_name(uint8_t *recv_data, uint8_t recv_len);
bool updata_ble_pwr(uint8_t recv_data);
bool updata_ble_xtal(uint8_t* recv_data, uint8_t recv_len);
bool updata_ble_mode(uint8_t recv_data);
bool updata_ble_ch(uint8_t* recv_data, uint8_t recv_len);
bool updata_ble_sn(uint8_t* recv_data, uint8_t recv_len);

void product_recv_task(void);
void at_msg_task(void);

#endif