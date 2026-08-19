CI2305蓝牙使用说明：

蓝牙广播数据说明：

typedef struct cias_ble_product
{
    int8_t    product_name[6];
    int32_t   product_id;  //1红外遥控器 2红外空调伴侣 3台灯
    int8_t    device_ID[6];//设备id 6bytes

}cias_ble_product_t;
cias_ble_product_t device_info;

name：
#define DEVICE_NAME                  ("CI_BLE")
1，数据类型 “0X09”（完整蓝牙名称） 数据长度 len = sizeof(device_info.product_name);

2，数据类型 “0xFF”(厂商自定义数据) 数据长度 sizeof(device_info.product_id)+sizeof(device_info.device_ID)

eg：  0x020106070943495F424C450BFF01000000A101010101B1

0BFF01000000A101010101B1：
0B:表示（厂商自定义数据+协议类型）的长度     FF：表示厂商自定义数据  01000000A101010101B1:厂商自定义数据  （启英自定义数据）

！！！（启英自定义数据说明）
01000000         ： //1红外遥控器 2红外空调伴侣 3台灯    // device_info.product_id = 0x02; 

A101010101B1    ：//设备id 6bytes

device_info.device_ID[0] = 0xFF;    //设备id 6bytes
device_info.device_ID[1] = 0xFF;
device_info.device_ID[2] = 0xFF;
device_info.device_ID[3] = 0xFF;
device_info.device_ID[4] = 0xFF;
device_info.device_ID[5] = 0x88;


070943495F424C45：
07：表示（蓝牙名字+协议类型）的长度          09：表示蓝牙名字类型    43495F424C45：表示蓝牙名字（ASCII 的16进制）


0x020106：LE普通发现模式，不支持BR/EDR


蓝牙配网流程说明：

1，打开蓝牙 ：ln_ble_app_task_entry ble_app_init 。。。 

2，开启蓝牙可连接广播 ： ln_ble_start_adv

3，开启数据传输服务： data_trans_svc_add

4，手机连接蓝牙：    蓝牙名字默认为 CI_BLE

5，向UUID 28be4a4a-cd67-11e9-a32f-2a2ae2dbccff 写入配网信息

5.1 3byte（16进制）数据 0xFA 0xXX（wifi名字长度） 0xXX（wifi密码长度）；发送后进入下一步   !!!(wifi名字长度<128 Byte,wifi密码长度<128 Byte)

5.2 utf-8 （wifi名字wifi密码）；发送后进入下一步

5.3 2byte（16进制）数据 0xXX（crc校验） 0xFB

6，蓝牙会向 28be4cb6-cd67-11e9-a32f-2a2ae2dbccee 通知配网状态

#define BLE_CONF_OK   "ble_success\r\n"
#define BLE_CONF_WAIT "ble_waiting\r\n"
#define BLE_CONF_FAIL "ble_failed!\r\n"

配网状态通知说明：

在蓝牙接收到 5.3的数据后，会在2s内进入配网状态

配网中 每秒发送 "ble_waiting\r\n" 

配网结束：

1，配网成功后发送  "ble_success\r\n"

2，配网超时发送    "ble_failed!\r\n" 默认60s为超时时间(5.3正常结束后开始计时)

失败后请回到第5步；

