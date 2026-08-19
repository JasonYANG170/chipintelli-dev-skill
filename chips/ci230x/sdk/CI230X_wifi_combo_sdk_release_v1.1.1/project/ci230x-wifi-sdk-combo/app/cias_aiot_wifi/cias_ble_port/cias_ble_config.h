#ifndef __CIAS_BLE_CONFIG_H__
#define __CIAS_BLE_CONFIG_H__

#include "utils/debug/log.h"
#include "utils/debug/ln_assert.h"
#include "utils/system_parameter.h"
#include "wifi.h"
#include "ln_wifi_err.h"
#include "netif/ethernetif.h"
#include "dhcpd_api.h"
#include "wifi_manager.h"

#include "ln_app_gap.h"
#include "gapm_task.h"
#include "ln_app_gatt.h"
#include "ln_app_callback.h"
#include "usr_ble_app.h"
#include "usr_send_data.h"

#define CIAS_NET_CONFIG_BLE               1
#define CIAS_NET_CONFIG_SOFTAP            0

#define LN_NET_CONFIG_MODE              CIAS_NET_CONFIG_BLE
#define LN_NET_CONFIG_MAX_TIMEOUT		(5*60*1000)
#define LN_UUID                         "AA1AE9" 
#define LN_CLOUD_PUB_KEY                "03C240A8CFFABAD097D1AEDE8BC33134B159CEC9307C5A45FADCABF8019731A684"
#define LN_CLOUD_PRIVATE_KEY            "872DE378AA94F6EDC85A534F128FE6AC37342D1B253C8D6B54BF7AF0C417D44D"
#define LN_MAC                          0x00, 0x50, 0xC2, 0x5E, 0x11, 0x22
#define LN_AP_SSID                      "JDDeng3322"
#define LN_FLASH_BLOCK_SIZE             4096

// #define DEVICE_NAME                  ("LN_CIAS")
// #define DEVICE_NAME_LEN              (sizeof(DEVICE_NAME))
#define ADV_DATA_MAX_LENGTH          (28)

#define BLE_USR_APP_TASK_STACK_SIZE  (1024*2)

typedef enum cias_net_config_state{
    CIAS_NET_CONFIG_INIT = 0,
    CIAS_NET_CONFIG_ING  = 1,
    CIAS_NET_CONFIG_END  = 2,
}_cias_net_config_state_t;

typedef struct{
    uint8_t  product_uuid[6];
    uint8_t  mac[6];
    uint8_t  shared_key[16];//if JL_SECURITY_LEVEL == 1
}cias_dev_info_t;

typedef struct{
    uint8_t service_uuid16[2];
    uint8_t manufacture_data[14];
}cias_gap_data_t;

typedef struct{
    uint8_t service_uuid128[16];
    uint8_t chra_uuid128_write[16];
    uint8_t chra_uuid128_indicate[16];
}cias_gatt_data_t;

typedef enum {
    E_CIAS_CHRA_WRITE    = 0x00,
    E_CIAS_CHRA_WRITE_NR = 0x01,
}CIAS_CHRA_TYPE_E;

//设备给手机返回状态
typedef enum
{
    E_CIAS_NET_CONF_ST_WIFI_CONNECT_FAILED        = 0x00, //WiFi连接失败
    E_CIAS_NET_CONF_ST_WIFI_CONNECT_START         = 0x01, //开始连接WiFi
    E_CIAS_NET_CONF_ST_WIFI_CONNECT_SUCCEED       = 0x02, //WiFi连接成功
    E_CIAS_NET_CONF_ST_WIFI_CONNECT_TIMEOUT       = 0x03, //WiFi连接超时，手机APP有超时设置，可以不用该状态
    E_CIAS_NET_CONF_ST_WIFI_CONNECT_SSID_ERROR    = 0x04, //WiFi SSID错误，比如：搜索不到该SSID等
    E_CIAS_NET_CONF_ST_WIFI_CONNECT_PSK_ERROR     = 0x05, //WiFi密码错误
    E_CIAS_NET_CONF_ST_WIFI_CONNECT_WAN_ERROR     = 0x06, //WiFi无法连接广域网
    E_CIAS_NET_CONF_ST_IOT_CONNECT_TIMEOUT        = 0x07, //IOT连接超时，手机APP有超时设置，可以不用该状态
    E_CIAS_NET_CONF_ST_IOT_CONNECT_FAILED         = 0x08, //IOT连接失败
    E_CIAS_NET_CONF_ST_IOT_CONNECT_SUCCEED        = 0x09, //IOT连接成功，发送状态时，data需要带feedid，长度是18
    E_CIAS_NET_CONF_ST_CLOUD_CONNECT_TIMEOUT      = 0x0A, //云端连接超时
    E_CIAS_NET_CONF_ST_CLOUD_CONNECT_SUCCEED      = 0x0B, //云端连接成功，设备如果没有其他云需要连接（如JAVS云），就在发送IoT连接成功后接着发该消息
    E_CIAS_NET_CONF_ST_TIMEOUT_EXIT               = 0x0C, //设备超时退出配网，设备不会一直处于配网状态，超时时间根据产品需求自定义
    E_CIAS_NET_CONF_ST_EXIT                       = 0x0D, //设备退出配网，例如用户按键等操作让设备退出配网
}E_CIAS_NET_CONF_ST;

extern OS_Thread_t ble_g_usr_app_thread;
void ln_ble_app_task_entry(void *params);


#endif  //__CIAS_BLE_CONFIG_H__