#ifndef __BLE_MAIN_H__
#define __BLE_MAIN_H__

typedef struct 
{
    void (*ble_send_data_callback)(uint8_t *buf, uint16_t len, uint16_t uuid);           //发送蓝牙数据函数
    void (*ble_recv_data_callback)(uint8_t *recv_data, uint8_t len);                     //接收到蓝牙数据处理函数
    void (*ble_recv_adv_data_callback)(uint8_t *recv_data, uint8_t len);                 //接收到2.4G遥控器数据处理函数
    void (*ble_connected_callback)(void);                                                //蓝牙连接成功处理函数
    void (*ble_disconnected_callback)(void);                                             //蓝牙断开连接成功处理函数
    void (*ble_adv_data_init)(uint8_t *adv_data, char *adv_name);                         //广播数据初始化
}BleInitCfg_t;


#endif