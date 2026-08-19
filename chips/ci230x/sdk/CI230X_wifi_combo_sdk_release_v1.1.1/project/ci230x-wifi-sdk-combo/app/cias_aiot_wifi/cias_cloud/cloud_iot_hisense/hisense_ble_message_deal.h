#ifndef __HISENSE_BLE_MESSAGE_DEAL_H__
#define __HISENSE_BLE_MESSAGE_DEAL_H__

#include "cias_ble_config.h"
#include "hisense_main_task.h"
#include "wifi.h"

#define    DEVICE_NAME_HISENSE           ("HIS-")

// WiFiconfig read write 按照 TLV 格式，其中
// TAG（2byte）+LEN（2byte）+ VALUE
/* BLE CONFIG msg tag*/
#define TAG_BT_CMD_SCAN                     0x0001 //APP write 获取模块端扫描路由器信息列表
#define TAG_BT_CMD_REPLY_SCAN               (TAG_BT_CMD_SCAN | 0x8000) //APP read 获取模块端扫描的路由器信息列表
#define TAG_BT_CMD_CONNECT                  0x0002 //APP write 发送 BSSID 与密码，格式与 WiFi 协议规定(第 5部分，原始信息组合)一致
#define TAG_BT_CMD_REPLY_CONNECT            (TAG_BT_CMD_CONNECT | 0x8000)//app read
#define TAG_BT_CMD_GET_STATUS               0x0003//APP write 获取当前 WiFi 连接情况
#define TAG_BT_CMD_REPLY_GET_STATUS         (TAG_BT_CMD_GET_STATUS | 0x8000)//APP read 获取当前 WiFi连接情况。如果返回不是 0x4+0x4 或不包含 0xf,需间隔 500ms 进行重试。
#define TAG_BT_CMD_GET_WLAN_BAND            0x0004 //APP write 获取模块支持的连接方式 2.4G/5G
#define TAG_BT_CMD_REPLY_WLAN_BAND          (TAG_BT_CMD_GET_WLAN_BAND | 0x8000)//APP read 获取模块支持的连接方式 2.4G/5G
#define TAG_BT_CMD_GET_WIFI_ID              0x0005 //APP write 获取 wifi ID
#define TAG_BT_CMD_REPLY_WIFI_ID            (TAG_BT_CMD_GET_WIFI_ID | 0x8000)// //APP read 获取 wifi ID 15
#define TAG_BT_CMD_GET_DEV_ID               0x0006 //APP write 获取 dev ID
#define TAG_BT_CMD_REPLY_DEV_ID             (TAG_BT_CMD_GET_DEV_ID | 0x8000) //APP read 获取 dev ID
#define TAG_BT_CMD_SSID_CONNECT             0x0007 //APP write 发送 SSID 与密码，格式与 WiFi 协议规定(第 5 部分，原始信息组合)一致，蓝牙协议版本大于 1 时支持
#define TAG_BT_CMD_REPLY_SSID_CONNECT       (TAG_BT_CMD_SSID_CONNECT |0x8000)//app read
#define TAG_BT_CMD_START_SCAN               0x0008 //APP 请求模组开始扫描路由器列表，蓝牙协议版本大于 1 时支持
#define TAG_BT_CMD_REPLY_START_SCAN         (TAG_BT_CMD_START_SCAN | 0x8000) //回复
#define TAG_BT_CMD_REPLY_FAIL               0xffff //返回错误


// ap_num(1byte) + current_ap_num(1byte 表示本次传输多少 AP 信息用来分包，当 APP 多次获取到的数量
// 等于 ap_num 时传输完毕) + 若干个 apinfo，蓝牙协议版本大于 1 时支持
// apinfo 由如下组成
// BSSID(6 byte) + RSSI(1 BYTE) + security_type(1 byte) + SSID_len(1 byte) + SSID
// 传输的 apinfo 长度固定；结构体定义如下：

typedef struct{
    unsigned char bssid_octer[6];
    uint8_t rssi;
    uint8_t secuity;
    uint8_t ssid_len;
    unsigned char ssid[33];
}apinfo_t;

// 4. REPLY_GET_STATUS
// wifi status(1byte) + cloud status(1byte) //
// wifi status 定义如下：
typedef enum {
    WIFI_STATE_DISABLED         = 0x00,
    WIFI_STATE__IDLE            = 0x01,
    WIFI_STATE_SCANNING         = 0x02,
    WIFI_STATE_STARTED          = 0x03,//表示开始连接路由器
    WIFI_STATE_CONNECTED        = 0x04, //表示连接路由器 OK
    WIFI_STATE_WAITFORKEY       = 0x05,
    WIFI_STATE_SCAN_NORESULT    = 0x06,//模块未搜索到任何热点
    WIFI_STATE_SCAN_ONCEFAIL    = 0x07,//目标热点，模块一次扫描失败
    WIFI_STATE_SCAN_FINALFAIL   = 0x08,//目标热点，模块最终扫描失败
    WIFI_STATE_ASSOC_FAIL       = 0x09,//模块关联目标热点失败
    WIFI_STATE_WRONG_PASSWORD   = 0x0f,
};

// cloud status 定义如下：
typedef enum {
    CLOUD_STATE_DISABLED        = 0x0,
    CLOUD_STATE__IDLE           = 0x1,
    CLOUD_STATE_STARTED         = 0x3,//表示开始连接云端
    CLOUD_STATE_CONNECTED       = 0x4,//表示连接云端 OK
    CLOUD_STATE_WAITFORKEY      = 0x5,
    CLOUD_STATE_TCP_CONNFAIL    = 0x6,//模块与云端 tcp socket 最终连接失败
    CLOUD_STATE_WRONG_PASSWORD  = 0xf,
};





int hisense_ble_scan_ap_list_write(ap_info_t *ap_info);
void hisense_ble_set_adv_data(void);
uint16_t hisense_ble_netconfig_test(uint8_t * ptr_ch,uint32_t ptr_len,uint16_t handle);

#endif