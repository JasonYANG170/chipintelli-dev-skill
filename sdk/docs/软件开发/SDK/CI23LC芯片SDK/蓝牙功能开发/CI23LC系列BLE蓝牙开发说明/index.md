<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/CI23LC%E7%B3%BB%E5%88%97BLE%E8%93%9D%E7%89%99%E5%BC%80%E5%8F%91%E8%AF%B4%E6%98%8E/ -->

# CI23LC系列BLE蓝牙开发说明

---

## 1. 概述

​ CI23LC系列芯片具有较强的CPU运算能力，支持硬件NN加速，以及充足的RAM。能够完成一些常见的语音处理算法；同时具备蓝牙BLE功能，可以配合手机APP做离线语音识别+蓝牙无线控制方案。

## 2. BLE重要属性说明

* **ble协议栈版本(5.2)**
* **支持连接模式，连接+广播模式，2.4G模式(适配私有协议需要单独支持)**
* **广播数据最大长度42字节(类型:1字节, 长度:1字节， 内容:最大支持40字节)**
* **连接状态下，数据收发最大长度240字节**
* **支持蓝牙配对加密功能**
* **蓝牙广播名称最大支持29字节**

## 3. CI23LC\_SDK\_BLE 开发说明

**注意事项：firmware\user\_file文件夹下的[7000]ble\_fimware.bin蓝牙固件请勿删除**

![蓝牙固件](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/ble%E5%9B%BA%E4%BB%B6.png)

图3-1 BLE蓝牙固件

**注意事项：UART1芯片内部使用，用户不可用**

### 3.1 BLE API函数说明

* **蓝牙主任务函数，主要有3个功能：**

1. **下载蓝牙固件**
2. **初始化蓝牙配置参数**
3. **接收处理手机和遥控器数据**

```
/**
 * @brief 蓝牙主任务，下载蓝牙固件，启动蓝牙广播和扫描广播，等待接收手机和遥控器数据
 */
void ble_main_task(void)
```

* **接收蓝牙核状态机并处理函数**

```
/**
 * @brief 接收并处理蓝牙消息任务，按照蓝牙小程序协议或自定义蓝牙协议，解析设备端收到的手机APP消息(该函数只适用于和启英物联APP交互使用)
 *
 */
void ci_ble_recv_task()
```

* **下载蓝牙协议栈固件函数**

```
/**
 * @brief 蓝牙协议栈固件串口下载
 * @var file_size :uint16 类型,固件头两个字节
 * @var len :固件参数分包传输的大小
 * @return 固件下载完成返回true;下载失败,立即退出并返回false
 */
bool ble_patch_process(void)
```

* **设置广播名称函数**

```
/**
 * @brief 设置蓝牙广播名称
 * @param adv_name: 需广播名称数据
 * @param len: 广播名称长度
 * @return 设置成功返回true;失败返回false
 */
bool ble_adv_name_set(uint8_t *adv_name, uint8_t len)
```

* **设置蓝牙广播数据函数**

```
/**
 * @brief 设置蓝牙广播数据
 * @param adv_data: 需发送的广播数据
 * @param adv_len: 广播数据有效长度
 * @return 设置成功返回true;失败返回false
 */
bool ble_adv_data_set(uint8_t *adv_data, uint8_t adv_len)
```

* **开启蓝牙广播函数**

```
/**
 * @brief 设置蓝牙广播状态(默认关闭)
 * @param adv_status: 蓝牙广播状态
 * @param adv_channel: 蓝牙广播频道
 * @return 设置成功返回true;失败返回false
 */
bool ble_adv_status_set(ble_adv_status_t adv_status, ble_adv_channel_t adv_channel)
```

* **设置蓝牙发送功率函数**

```
/**
 * @brief 设置蓝牙发送功率
 * @param power: 功率等级
 * @return 设置成功返回true;失败返回false
 */
bool ble_tx_power_set(ble_tx_power_t power)
```

* **删除非系统服务和特征函数**

```
/**
 * @brief 删除非系统服务和特征
 * @return 设置成功返回true;失败返回false
 */
bool ble_service_delete()
```

* **设置蓝牙服务函数**

```
/**
 * @brief 设置蓝牙服务和特征
 * @param ble_service_config: 用户配置的蓝牙服务和特征参数
 * @return 设置成功返回true;失败返回false
 */
bool ble_service_set(ble_service_t *ble_service_config)
```

* **设置蓝牙广播扫描数据函数**

```
/**
 * @brief 设置蓝牙广播扫描参数(默认关闭)
 * @param adv_scan_status: 蓝牙广播扫描状态
 * @param adv_scan_channel: 蓝牙广播扫描频道
 * @param adv_scan_len: 蓝牙扫描到的广播包数据长度。扫描功能只能扫描蓝牙设备广播数据，不能扫描广播回复。长度设置为需要接受的蓝牙广播数据加mac地址(6字节)。扫描结果数据：1-2字节为帧头，3-8字节为mac地址，广播有效数据从第9个字节到结尾。
 * @return 设置成功返回true;失败返回false
 */
bool ble_adv_scan_status_set(ble_adv_scan_status_t adv_scan_status, ble_adv_scan_channel_t adv_scan_channel, uint8_t adv_scan_len)
```

* **设置广播扫描效率函数**

```
/**
 * @brief 设置蓝牙广播扫描效率参数(默认不设置)
 * @param adv_scan_efficiency: 蓝牙扫描效率（需要同时开启蓝牙连接功能时，一般设置为LOW或VERY_LOW,否则蓝牙连接会不稳定）
 * @return 设置成功返回true;失败返回false
 */
bool ble_adv_scan_efficiency_set(ble_adv_scan_efficiency_t adv_scan_efficiency)
```

* **公版2.4G遥控器数据接收**

```
/**
  * @brief 解析遥控器发送的按键信息
  * @param buf: 收到的按键信息
  * 函数会调用covent_data_adv_to_ble(根据实际的数据区选择是否调用该函数)，该函数将收到的遥控器按键信息转换为启英蓝牙通信协议，解析后按键信息数据存储到 recv_data数组
  */
bool ci_ble_recv_adv_data_handle(unsigned char* buf)
```

![遥控器](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E9%81%A5%E6%8E%A7%E5%99%A8.png)

* **上电遥控器对码函数**

```
/**
 * @brief 上电对码逻辑初始化
 */
void ble_match_code_init(uint8_t *match_code)
```

* **连接状态下发送蓝牙数据函数**

```
bool ble_send_payload(uint8_t *buf, uint16_t len, uint16_t uuid)发送蓝牙数据到手机：
* @param buf: 需发送的数据
 * @param len: 数据长度
 * @param uuid: 发送数据对应的特征值uuid
 * @return 发送成功返回true;发送返回false
```

* **蓝牙回调函数注册结构体**

用户不使用“启英物联”小程序，需要自己实现下面蓝牙回调结构体并进行注册，可以参考图3-2中的回调注册

```
typedef struct 
{
    void (*ble_send_data_callback)(uint8_t *buf, uint16_t len, uint16_t uuid);           //发送蓝牙数据函数
    void (*ble_recv_data_callback)(uint8_t *recv_data, uint8_t len);                     //接收到蓝牙数据处理函数
    void (*ble_connected_callback)(void);                                                //蓝牙连接成功处理函数
    void (*ble_disconnected_callback)(void);                                             //蓝牙断开连接成功处理函数
    void (*ble_adv_data_init)(uint8_t *adv_data, char *adv_name);                         //广播数据初始化
}BleInitCfg_t;
```

![蓝牙回调结构体](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/ble%E5%9B%9E%E8%B0%83%E7%BB%93%E6%9E%84%E4%BD%93.png)

图3-2 BLE回调注册

### 3.2 状态机状态说明如下(opcode)：

| opcode | 说明 |
| --- | --- |
| BLE\_EVENT\_STACK\_OK | 蓝牙固件下载完成后，蓝牙协议栈启动成功 |
| BLE\_EVENT\_CONN\_REP | 蓝牙连接成功事件 |
| BLE\_EVENT\_DIS\_REP | 蓝牙断开事件 |
| BLE\_EVENT\_DATA\_REP | 收到连接数据 |
| BLE\_EVENT\_NVRAM\_REP | 收到配对成功秘钥 |
| BLE\_EVENT\_PAIRING\_STATE | 收到配对状态 |
| BLE\_EVENT\_ENCRYPTION\_STATE | 收到加密状态 |
| BLE\_EVENT\_ADV\_REP | 收到广播扫描数据 |
| BLE\_EVENT\_UUID\_HANDLE | 收到添加蓝牙服务和特征成功事件 |
| BLE\_PAIRING\_STATE\_SUCCESS | 读取NV存储秘钥成功，修改设备配对状态为配对成功 |
|  |  |

### 3.3 BLE初始化和消息任务处理流程图

![软件运行](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/ble%E6%B5%81%E7%A8%8B.png)

图3-3 BLE任务流程图

### 3.4 SDK常用参数配置说明

* **开启蓝牙功能(user\_config.h)**

```
#define USE_BLE_MOUDLE        1
```

* **蓝牙广播名称配置(user\_config.h)**

```
#define BLE_ADV_NAME_APPEND_FLASH_ID        1           //蓝牙广播名称自动追加2字节flash id, 保证每个设备广播名称不一样,flash_id会占用5字节，自定义长度最长为24字节
define BLE_USER_DEFINE_ADV_NAME_CONTENT  "CI_BLE"  //用户可自定义,最长不能超过29字节，中文一个字符占3字节
```

* **模拟串口配置(user\_config.h)**

```
#define SIMULATE_UART_ENABLE        0     // 1-换成普通IO模拟串口 0-禁用模拟串口，模拟串口使用PC4-固定波特率9600，会影响mute管脚，量产时请关闭该宏
#define SIMULATE_UART_MAX_SIZE      128   //模拟串口最大发送字节数-会占用内存，不建议设置过大
```

* **自定义蓝牙服务和特征参数(user\_config.h)**

用户可根据自己需求添加定义，下面是标准sdk已有的服务和特征参数

```
//服务定义-如需修改该下面三个值，小程序也要做同步修改(不可用0x2900之前的值不可用)
#define BLE_UUID_CIAS_SERVICE              0XAE3A     //服务入口
#define BLE_UUID_CIAS_WRITE                0XAE3B     //手机发送数据到蓝牙芯片特征值
#define BLE_UUID_CIAS_NOTIFY               0XAE3C     //蓝牙芯片发送数据到手机特征值
#define BLE_UUID_CIAS_CMD_NOTIFY           0XAE3D     //获取词条信息特征值

/**
 * @brief 自定义蓝牙服务和特征参数
 * @param uuid: 蓝牙主服务的uuid
 * @param characteris_number: 对外展示的蓝牙特征个数
 * @param characteris: 蓝牙特征
 * {
 *  .uuid：蓝牙特征uuid，
 *  .attribute: 特征属性，
 *  .handle: 设置成功后,芯片返回,本地保存用于数据收发
 * }
 */
ble_service_t ble_service_config =
    {
        .uuid = BLE_UUID_CIAS_SERVICE,
        .characteris_number = 3,
        .characteris[0] = {.uuid = BLE_UUID_CIAS_WRITE, .attribute = BLE_ATTRIBUTE_WRITE},
        .characteris[1] = {.uuid = BLE_UUID_CIAS_NOTIFY, .attribute = BLE_ATTRIBUTE_NOTIFY},
        .characteris[2] = {.uuid = BLE_UUID_CIAS_CMD_NOTIFY, .attribute = BLE_ATTRIBUTE_NOTIFY},
};
```

* **连接和广播模式配置(ble\_param\_config.h)**

```
#define CIAS_BLE_SCAN_ENABLE         0      //0-默认配置为连接模式，只能和小程序进行通信； 1-和app进行连接同时还能进行广播扫描(app通信的同时能接收遥控器广播数据)
#if CIAS_BLE_SCAN_ENABLE
#define CIAS_BLE_SCAN_REMOTE_CONTROL_LEN    0x16        //ble广播扫描遥控器数据的长度,公版0x16
#define BLE_ADV_SCAN_TIMEOUT                500         //开启遥控器扫描后，定时器过滤数据的超时时间(ms)
#define BLE_EVENT_DATA_TIME_FILTER_ENABLE   1           //开启蓝牙连接数据的定时器过滤,ble蓝牙数据高速率应用场景,请关闭该功能
#endif
```