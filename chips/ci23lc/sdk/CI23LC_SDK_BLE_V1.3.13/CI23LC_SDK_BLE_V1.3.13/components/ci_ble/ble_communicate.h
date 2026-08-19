#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ci_uart.h"
#include "sdk_default_config.h"
#include "system_msg_deal.h"
#include "ble_param_config.h"

#ifndef _BLE_COMMUNICATE_
#define _BLE_COMMUNICATE_

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum 
{
    BLE_STATUS_DISCONNECT = 0x04,   //蓝牙状态已断开
    BLE_STATUS_CONNECT    = 0x20,   //蓝牙状态已连接
}ble_status_t;

typedef enum 
{
    ADV_OFF = 0x00,                 //关闭蓝牙广播
    ADV_ON  = 0x01,                 //开启蓝牙广播
}ble_adv_status_t;

typedef enum 
{
    ADV_CHAN_ALL =  0x00,            //所有频道轮流广播
    ADV_CHAN_0   =  0x37,            //单独在37频道广播,频点为2402MHz
    ADV_CHAN_1   =  0x38,            //单独在38频道广播,频点为2426MHz          
    ADV_CHAN_2   =  0x39,            //单独在39频道广播,频点为2480MHz 
}ble_adv_channel_t;

typedef enum 
{
    ADV_SCAN_OFF = 0x00,             //关闭蓝牙广播扫描
    ADV_SCAN_ON  = 0x01,             //开启蓝牙广播，持续扫描
}ble_adv_scan_status_t;

typedef enum 
{
    ADV_SCAN_CHAN_0   =  0x37,        //单独在37频道扫描广播,频点为2402MHz
    ADV_SCAN_CHAN_1   =  0x38,        //单独在38频道扫描广播,频点为2426MHz
    ADV_SCAN_CHAN_2   =  0x39,        //单独在39频道扫描广播,频点为2480MHz
}ble_adv_scan_channel_t;

typedef enum 
{
    VERY_HIGH   =  0x10,               //蓝牙广播扫描效率最高
    HIGH        =  0x20,         
    MIDDLE      =  0x30, 
    LOW         =  0x40, 
    VERY_LOW    =  0x50,                //蓝牙广播扫描效率最低     
}ble_adv_scan_efficiency_t;

typedef enum 
{
    BLE_REV_STATE_PACK_TYPE    = 0x00,
    BLE_REV_STATE_OPCODE       = 0x01,
    BLE_REV_STATE_LENGTH       = 0x02,
    BLE_REV_STATE_DATA         = 0x03,
}ble_receive_state_t;                    //ble 事件接收状态机

typedef enum 
{
    BLE_PAIRING_STATE_IDLE     = 0x00,
    BLE_PAIRING_STATE_START    = 0x01,
    BLE_PAIRING_STATE_SUCCESS  = 0x02,
    BLE_PAIRING_STATE_FAIL     = 0x03,
}ble_pairing_state_t;                    //ble配对的状态

typedef enum 
{
    BLE_PAIRING_NONE           = 0x00, //不加密
    BLE_PAIRING_JUSTWORK       = 0x01, //加密，用户不需要操作，NO MITM
    BLE_PAIRING_PASSKEY        = 0x02, //需要手机输入验证码并确认，支持 MITM
    BLE_PAIRING_SECURE_CONNECT_JUSTWORK     = 0x81, //具有 SC 属性的加密，用户不需要操作，NO MITM
    BLE_PAIRING_SECURE_CONNECT_NUMERIC      = 0x82, //具有 SC 属性数字比对的配对模式，手机和设备显示确认码， 用户确认是否相同。
    BLE_PAIRING_SECURE_CONNECT_PASSKEY      = 0x83,
}ble_pairing_mode_t;                    //ble配对加密模式

// 办公室测试环境(发射功率设置越大功耗越大):
// BLE_POWER_0dB:通讯距离在30m左右
// BLE_POWER_3dB:通讯距离在40m-50m左右
// BLE_POWER_5dB:通讯距离在60m-70m左右
typedef enum 
{
    BLE_POWER_0dB           = 0x00, //0db,默认
    BLE_POWER_3dB           = 0x01, 
    BLE_POWER_5dB           = 0x02, 
    BLE_POWER_n3dB          = 0x03, //-3dB
    BLE_POWER_n5dB          = 0x04, //-5dB
}ble_tx_power_t;                    //ble发射功率

#pragma pack(1)
typedef struct
{
    uint8_t type;
    uint8_t opcode;
    uint8_t length;
    uint8_t msg_data[BLE_MSG_DATA_MAX_SIZE];
}ble_msg_data_t;
#pragma pack()


typedef enum 
{
    BLE_ATTRIBUTE_BROADCAST    = 0x01,
    BLE_ATTRIBUTE_READ         = 0x02,
    BLE_ATTRIBUTE_WRITE_NRQ    = 0x04,
    BLE_ATTRIBUTE_WRITE        = 0x08,
    BLE_ATTRIBUTE_NOTIFY       = 0x10,
    BLE_ATTRIBUTE_INDICATE     = 0x20,
}ble_attribute_t;

typedef struct
{
    uint16_t uuid;
    ble_attribute_t attribute;
    uint8_t handle;
}ble_characteris_t;

typedef struct
{
    uint16_t uuid;
    uint8_t characteris_number;
    ble_characteris_t characteris[];
}ble_service_t;

//Packet Type: 包类型
#define BLE_PACK_TPYE_CMD           0x01  //设置命令 
#define BLE_PACK_TPYE_ENENT         0x02  //命令事件回复
#define BLE_PACK_TPYE_PATCH         0x04  //patch回复

/**
 * @brief   Opcode：操作码 
 * CMD： 是MCU发送给蓝牙模块的指令，用于配置蓝牙模块、控制蓝牙连接和发送数据等。
 * EVENT：模块接收到每个CMD后都会回复一个与之对应的EVENT作为回应。收到此EVENT后再发送新的CMD。
 */

#define BLE_CMD_SET_ADDR            0x01  //设置 BLE 地址
#define BLE_CMD_SET_NAME            0x04  //设置 BLE 名称
#define BLE_CMD_SEND_DATA           0x09  //发送 BLE 数据
#define BLE_CMD_STATUS_REQ          0x0B  //查询蓝牙状态
#define BLE_CMD_SET_UART_FLOW       0x0E  //设置 UART 流控
#define BLE_CMD_SET_UART_BAUD       0x0F  //设置 UART 波特率
#define BLE_CMD_VERSION_REQUEST     0x10  //查询模块固件版本
#define BLE_CMD_DISCONNECT          0x12  //断开 BLE 连接
#define BLE_CMD_SET_NVRAM           0x26  //设置蓝牙配对加密nv数据
#define BLE_CMD_SET_PAIRING         0x33  //设置 BLE 配对模式
#define BLE_CMD_ADV_DATA            0x34  //设置广播数据
#define BLE_CMD_ADV_RSP_DATA        0x35  //设置广播回复数据
#define BLE_CMD_CONN_UPDATE         0x36  //更新连接参数
#define BLE_CMD_ADV_PARA            0x37  //更新广播参数参数
#define BLE_CMD_START_PAIRING       0x38  //开始配对
#define BLE_CMD_SET_TX_POWER        0x42  //设置发送功率
#define BLE_CMD_RESET_CHIP_REQ      0x51  //软件复位芯片
#define BLE_CMD_ADV_SCAN_STATUS     0x67  //设置广播扫描状态
#define BLE_CMD_ADV_SCAN_EFFICIENCY 0x68  //设置广播扫描效率
#define BLE_CMD_ADV_STATUS          0x69  //设置广播状态
#define BLE_CMD_DELETE_SERVICE      0x76  //删除 BLE 非系统服务及特征
#define BLE_CMD_ADD_SERVICE         0x77  //增加 BLE 自定义服务
#define BLE_CMD_ADD_CHARACTERISTIC  0x78  //增加 BLE 自定义特征

#define BLE_EVENT_CONN_REP          0x02  //BLE 连接建立
#define BLE_EVENT_DIS_REP           0x05  //BLE 连接断开
#define BLE_EVENT_CMD_RES           0x06  //CMD命令已完成
#define BLE_EVENT_DATA_REP          0x08  //接收到 BLE 数据
#define BLE_EVENT_STACK_OK          0x09  //模块准备好
#define BLE_EVENT_STATUS_REP        0x0A  //蓝牙查询状态事件
#define BLE_EVENT_NVRAM_REP         0x0D  //收到NV存储事件
#define BLE_EVENT_PATCH_ACK         0x0E  //patch固件下载命令回复
#define BLE_EVENT_PAIRING_STATE     0x14  //接收到 BLE 配对状态
#define BLE_EVENT_ENCRYPTION_STATE  0x15  //接收到 BLE 加密状态
#define BLE_EVENT_UUID_HANDLE       0x29  //新设置的 UUID 对应的 handle
#define BLE_EVENT_ADV_REP           0x2A  //接收到 ADV 数据

#define BLE_EVENT_TYPE_PATCH        0x01  //patch固件下载正确

#define BLE_CMD_SUCCESS 	        0
#define BLE_CMD_ERROR 	            1

#define PATCH_FILE_LEN_SIZE         2
#define PATCH_BLOCK_LEN_SIZE        1

#define BLE_SET_LEN                 40
#define BLE_SET_HEAD_LEN            3
#define BLE_MAC_LEN                 6
#define BLE_SET_ADV_CHAN_LEN        3
#define BLE_SET_ADV_SCAN_LEN        3
#define BLE_SET_ADV_SCAN_EFFICIENCY_LEN        4
#define BLE_UUID_DELETE_LEN         0
#define BLE_UUID_LEN                2
#define BLE_UUID_ADD_LEN            3
#define BLE_ADD_HANDLE_LEN          4
#define BLE_SET_PAIRING_LEN         1
#define BLE_CONN_UPDATA_LEN         8

extern uint8_t adv_ind[BLE_ADV_LEN];
extern volatile uint8_t ble_recv_cmd;
extern volatile uint8_t ble_handle;
extern uint8_t ble_status;
extern ble_pairing_state_t ble_pairing_status;

bool ble_send_payload(uint8_t *buf, uint16_t len, uint16_t uuid);
bool ble_send_packet(uint8_t *buf, uint16_t len, uint8_t req_cmd);
bool ble_patch_process(void);
bool ble_port_mutex_create(void);
int ble_port_protocol_hw_init(UART_BaudRate baud);
void ble_send_recv_msg(ble_msg_data_t * msg, BaseType_t *xHigherPriorityTaskWoken);
void ble_main_task(void);

//蓝牙收发数据的加密函数，pack_data为待加/解密数据，len为数据总长度
void cias_crypto_data(uint8_t* pack_data, uint8_t len); 
#ifdef __cplusplus
}
#endif



#endif

