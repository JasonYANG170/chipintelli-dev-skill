<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E/ -->

# CI23LC系列小程序交互协议与功能说明

CI23LC系列语音蓝牙芯片，可使用 **“启英物联”** 和 **“AI语音智控”** 微信小程序进行蓝牙连接设备控制，本文主要描述小程序与CI23LC语音蓝牙设备的控制协议与自定义小程序协议的内容。

## 1. 微信小程序使用

### 1.1 小程序区别

启英提供两款微信小程序，**“启英物联”** 和 **“AI语音智控”** 小程序，主要功能区别如下表，客户根据自身需求进行小程序选择。

| 小程序 | 启英物联 | AI语音智控 |
| --- | --- | --- |
| ble蓝牙控制 | 支持 | 支持 |
| 设备端通过协议自定义按键 | 支持 | 支持 |
| IOT控制 | 支持 | 不支持 |
| 启英平台功能 | 支持 | 不支持 |
| 启英泰伦信息特征 | 包含 | 不包含 |

### 1.2 微信小程序使用

1. 打开微信，扫描小程序码或搜索 **“启英物联”** / **“AI语音智控”** 进入小程序。
2. 打开蓝牙权限(部分手机需要开启位置权限)。
3. 开启手机蓝牙。
4. 添加设备，通过点击“＋”号，或者“添加设备”按键，进行添加设备。
5. 进入扫描设备界面，点击扫描到的设备(小程序已有：风扇、空调、取暖器、茶吧机，灯控等设备…)。
6. 确定连接设备，并设备命名。
7. 连接成功，返回首页。
8. 点击添加的设备即可进入设备操作界面。

![启英物联小程序码](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-1.png)

图1-1 启英物联小程序码

![AI语音智控小程序码](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-2.png)

图1-2 AI语音智控小程序码

![小程序使用步骤](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-3.png)

图1-3 小程序使用步骤

## 2. 设备与小程序交互协议

启英微信小程序与CI23LC/CI231X系列语音蓝牙设备的交互协议，根据设备不同应用领域，协议内容有一定的区别，已对一些设备应用的领域进行ID定义 ，语音端和小程序将根据领域ID进行对应的协议处理。

CI23LC系列语音SDK代码中，在app\_ble\demo\cias\_demo\_config.h文件中对DEV\_DRIVER\_EN\_ID进行修改。

```
#define DEV_DRIVER_EN_ID                    DEV_FAN_MAIN_ID     //设为风扇领域设备

//主设备ID定义
#define DEV_IR_CONTROL_MAIN_ID              0x01   //红外遥控器
#define DEV_AIRCONDITION_MAIN_ID            0X02   //空调
#define DEV_LIGHT_CONTROL_MAIN_ID           0x03   //灯控
#define DEV_SOUND_MAIN_ID                   0x04   //音响设备
#define DEV_TEA_BAR_MAIN_ID                 0x05   //茶吧机
#define DEV_FAN_MAIN_ID                     0x06   //风扇
#define DEV_HEATTABLE_MAIN_ID               0x07   //取暖桌
#define DEV_WARMER_MAIN_ID                  0x08   //取暖器
#define DEV_WATERHEATED_MAIN_ID             0x09   //水暖毯
```

### **2.1 小程序设备类型**

| 设备类型 | BLE蓝牙 |
| --- | --- |
| 0x01 | 红外遥控器 |
| 0x02 | 空调 |
| 0x03 | 灯控 |
| 0x04 | 音响 |
| 0x05 | 茶吧机 |
| 0x06 | 风扇 |
| 0x07 | 取暖桌 |
| 0x08 | 取暖器 |
| 0x09 | 水暖毯 |
| 0x0A | 微波炉 |
| 0x0B | 窗帘 |
| 0x0C | 加湿器 |
| 0x0B | 足浴盆 |

### **2.2 设备页面主题修改协议**

该协议适用于小程序各领域设备，但目前仅风扇与取暖器领域实现该协议功能

协议格式：帧头(2Byte)+协议版本(1Byte)+标题数据长度(2Byte)+页面标题(nByte)+CRC16校验(2Byte)

| **帧头(2Byte)** | **协议版本 (1Byte)** | **标题数据长度（2Byte)** | **页面标题 （nByte）** | **CRC16** **(2Byte)** |
| --- | --- | --- | --- | --- |
| 0xA55A | 0x05(页面标题修改) | byte0：高字节；byte1：低字节； | xx.....； |  |

CI23LC系列语音SDK代码中，直接在app\_ble\demo\cias\_demo\_config.h文件中对APP\_PAGE\_TITLE进行修改。

```
#define APP_PAGE_TITLE                      "语音智能设备"//设备控制页面标题. 中文最大长度9,字母数字最大长度15
```

该协议相关函数代码在app\_ble\demo\cias\_ble\_msg\_deal.c文件中

```
/*
    启英设备控制页面标题内容显示
    协议格式：帧头(2Byte)+协议版本(1Byte)+标题数据长度(2Byte)+页面标题(nByte)+CRC16校验(2Byte)
*/
void app_title_set()
```

其页面主题修改效果图如下：

![设备页面主题](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-4.png)

图2-1 设备页面主题修改

### 2.3 设备页面背景修改协议

该协议适用于小程序各领域设备，但目前仅风扇领域实现该协议功能

协议格式：帧头(2Byte)+协议版本(1Byte)+页面背景数据长度(2Byte)+页面背景-(nByte)+CRC16校验(2Byte)

| **帧头(2Byte)** | **协议版本 (1Byte)** | **页面背景数据长度（2Byte)** | **页面背景（nByte）** | **CRC16** **(2Byte)** |
| --- | --- | --- | --- | --- |
| 0xA55A | 0x06(页面背景修改) | byte0：高字节；byte1：低字节； | xx.....； |  |

CI23LC系列语音SDK代码中，在app\_ble\demo\cias\_demo\_config.h文件中对APP\_PAGE\_BACKGROUND进行修改。

```
#define APP_PAGE_BACKGROUND                 "1"         //设备控制页面背景. "1":默认背景图; "2":灰色; "3":白黑渐变; "4":渐变; "5":开/关机-蓝/白切换;
```

该协议相关函数代码在app\_ble\demo\cias\_ble\_msg\_deal.c文件中

```
/*
    启英设备控制页面页面背景
    协议格式：帧头(2Byte)+协议版本(1Byte)+背景数据长度(2Byte)+页面背景(nByte)+CRC16校验(2Byte)
*/
void app_background_set()
```

其背景图效果如下：

![设备页面背景修改](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-5.png)

图2-2 设备页面背景修改

### 2.4 小程序调用用户层事件回调函数协议

该协议适用于小程序各领域设备

协议格式：帧头(2Byte)+命令(1Byte)+CRC16校验(2Byte)

| 帧头 | 命令\*\*(1Byte)\*\* | **CRC16** **(2Byte)** |
| --- | --- | --- |
| 0xA55B | 0x01(设备ble连接断开) |  |
| 0xA55B | 0x02(小程序获取词条信息) |  |

该协议相关函数代码在app\_ble\demo\cias\_ble\_msg\_deal.c文件中

```
/**
 * @brief 处理蓝牙接收到的手机端消息-只使用于启英物联加密交互，客户使用自己的私有协议
 * @param recv_data 接收到的数据；
 * @param len       接收数据长度；
 */
void ci_ble_recv_data_handle(uint8_t *recv_data, uint8_t len)
```

### 2.5 小程序和设备通信协议格式

![通信协议格式](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-6.png)

图2-3 通信协议格式

### **2.6 通信协议实例**

#### 2.6.1 空调设备

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E7%A9%BA%E8%B0%83.png)

![空调通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-7-1.png)

![空调通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-7-2.png)

![空调通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-7-3.png)

![空调通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-7-4.png)

图2-4 空调通信协议

该协议相关函数代码在app\_ble\demo\cias\_aircondition\_msg\_deal.c文件中

```
/**
 * @brief 识别语音词条，设备状态上行
 * @param cmd_id 命令词ID；
 */
uint8_t aircondition_report(uint16_t cmd_id)

/**
 * @brief 接收小程序下行控制设备数据
 * @param msg 命令词ID；
 */
void aircondition_callback(ble_msg_V1_t msg)

/**
 * @brief 接收小程序下行查询设备状态数据
 * @param msg 命令词ID；
 */
void aircondition_query(ble_msg_V1_t msg)
```

#### 2.6.2 灯控设备

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E7%81%AF%E6%8E%A7.png)

![灯控通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-8.png)

图2-5 灯控通信协议

该协议相关函数代码在app\_ble\demo\cias\_rgb\_msg\_deal.c文件中

```
/**
 * @brief 识别语音词条，设备状态上行
 * @param cmd_id 命令词ID；
 */
uint8_t rgb_report(uint16_t cmd_id)

/**
 * @brief 接收小程序下行控制设备数据
 * @param msg 命令词ID；
 */
void rgb_callback(ble_msg_V1_t msg)

/**
 * @brief 接收小程序下行查询设备状态数据
 * @param msg 命令词ID；
 */
void rgb_query(ble_msg_V1_t msg)
```

#### 2.6.3 茶吧机

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E8%8C%B6%E5%90%A7%E6%9C%BA.png)

![茶吧机通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-9.png)

图2-6 茶吧机通信协议

该协议相关函数代码在app\_ble\demo\cias\_tbm\_msg\_deal.c文件中

```
/**
 * @brief 识别语音词条，设备状态上行
 * @param cmd_id 命令词ID；
 */
uint8_t tbm_report(uint16_t cmd_id)

/**
 * @brief 接收小程序下行控制设备数据
 * @param msg 命令词ID；
 */
void tbm_callback(ble_msg_V1_t msg)

/**
 * @brief 接收小程序下行查询设备状态数据
 * @param msg 命令词ID；
 */
void tbm_query(ble_msg_V1_t msg)
```

#### 2.6.4 风扇设备

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E9%A3%8E%E6%89%87.png)

![风扇通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-10.png)

图2-7 风扇通信协议

该协议相关函数代码在app\_ble\demo\cias\_fan\_msg\_deal.c文件中

```
/**
 * @brief 识别语音词条，设备状态上行
 * @param cmd_id 命令词ID；
 */
uint8_t fan_report(uint16_t cmd_id)

/**
 * @brief 接收小程序下行控制设备数据
 * @param msg 命令词ID；
 */
void fan_callback(ble_msg_V1_t msg)

/**
 * @brief 接收小程序下行查询设备状态数据
 * @param msg 命令词ID；
 */
void fan_query(ble_msg_V1_t msg)
```

#### 2.6.5 取暖桌

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%8F%96%E6%9A%96%E6%A1%8C.png)

![取暖桌通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-11.png)

图2-8 取暖桌通信协议

该协议相关函数代码在app\_ble\demo\cias\_heattable\_msg\_deal.c文件中

```
/**
 * @brief 识别语音词条，设备状态上行
 * @param cmd_id 命令词ID；
 */
uint8_t heattable_report(uint16_t cmd_id)

/**
 * @brief 接收小程序下行控制设备数据
 * @param msg 命令词ID；
 */
void heattable_callback(ble_msg_V1_t msg)

/**
 * @brief 接收小程序下行查询设备状态数据
 * @param msg 命令词ID；
 */
void heattable_query(ble_msg_V1_t msg)
```

#### 2.6.6 取暖器

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%8F%96%E6%9A%96%E5%99%A8.png)

![取暖器通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-12.png)

图2-9 取暖器通信协议

该协议相关函数代码在app\_ble\demo\cias\_warmer\_msg\_deal.c文件中

```
/**
 * @brief 识别语音词条，设备状态上行
 * @param cmd_id 命令词ID；
 */
uint8_t warmer_report(uint16_t cmd_id)

/**
 * @brief 接收小程序下行控制设备数据
 * @param msg 命令词ID；
 */
void warmer_callback(ble_msg_V1_t msg)

/**
 * @brief 接收小程序下行查询设备状态数据
 * @param msg 命令词ID；
 */
void warmer_query(ble_msg_V1_t msg)
```

#### 2.6.7 水暖毯

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E6%B0%B4%E6%9A%96%E6%AF%AF.png)

![水暖毯通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-13.png)

图2-10 水暖毯通信协议

该协议相关函数代码在app\_ble\demo\cias\_waterheated\_msg\_deal.c文件中

```
/**
 * @brief 识别语音词条，设备状态上行
 * @param cmd_id 命令词ID；
 */
uint8_t waterheated_report(uint16_t cmd_id)

/**
 * @brief 接收小程序下行控制设备数据
 * @param msg 命令词ID；
 */
void waterheated_callback(ble_msg_V1_t msg)

/**
 * @brief 接收小程序下行查询设备状态数据
 * @param msg 命令词ID；
 */
void waterheated_query(ble_msg_V1_t msg)
```

#### 2.6.8 窗帘

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E7%AA%97%E5%B8%98.png)

![窗帘通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-14.png)

图2-11 窗帘通信协议

#### 2.6.9 加湿器

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%8A%A0%E6%B9%BF%E5%99%A8.png)

![加湿器通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-15.png)

图2-12 加湿器通信协议

#### 2.6.10 足浴盆

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E8%B6%B3%E6%B5%B4%E7%9B%86.png)

![足浴盆通信协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-16.png)

图2-13 足浴盆通信协议

### 2.7 设备状态全属性上报

支持设备端一条指令把状态全同步给小程序

协议格式：帧头(2Byte)+协议版本(1Byte)+功能状态(nByte)+CRC16校验(2Byte)

功能状态(nByte)，可填入任意个功能，但功能位置顺序保持固定

| **帧头(2Byte)** | **协议版本 (1Byte)** | **功能状态（nByte）** | **CRC16** **(2Byte)** |
| --- | --- | --- | --- |
| 0xA55A | 0x03(全属性上报) | xx.....； |  |

#### 2.7.1 空调全属性上报

![空调全属性上报协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-17.png)

图2-14 空调全属性上报协议

该协议相关函数代码在app\_ble\demo\cias\_aircondition\_msg\_deal.c文件中

```
/**
 * @brief 小程序查询设备状态，设备全属性上报
 */
void aircondition_query_all()
```

#### 2.7.2 灯控全属性上报

![灯控全属性上报协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-18.png)

图2-15 灯控全属性上报协议

该协议相关函数代码在app\_ble\demo\cias\_rgb\_msg\_deal.c文件中

```
/**
 * @brief 小程序查询设备状态，设备全属性上报
 */
void rgb_query_all()
```

#### 2.7.3 风扇全属性上报

![风扇全属性上报协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-19.png)

图2-16 风扇全属性上报协议

该协议相关函数代码在app\_ble\demo\cias\_fan\_msg\_deal.c文件中

```
/**
 * @brief 小程序查询设备状态，设备全属性上报
 */
void fan_query_all()
```

#### 2.7.4 取暖器全属性上报

![取暖器全属性上报协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-20.png)

图2-17 取暖器全属性上报协议

该协议相关函数代码在app\_ble\demo\cias\_warmer\_msg\_deal.c文件中

```
/**
 * @brief 小程序查询设备状态，设备全属性上报
 */
void warmer_query_all()
```

#### 2.7.5 足浴盆全属性上报

![足浴盆全属性上报协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-21.png)

图2-18 足浴盆全属性上报协议

### 2.8 设备小程序功能设置

支持设备端通过协议，选择小程序设备控制界面要展示的功能按键

协议格式：帧头(2Byte)+协议版本(1Byte)+功能设置(nByte)+CRC16校验(2Byte)

| **帧头(2Byte)** | **协议版本 (1Byte)** | **功能设置（nByte）** | **CRC16** **(2Byte)** |
| --- | --- | --- | --- |
| 0xA55A | 0x04(功能设置) | xx.....； |  |

#### 2.8.1 风扇功能性设置

![风扇功能设置协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-22.png)

图2-19 风扇功能设置协议

该协议相关函数代码在app\_ble\demo\cias\_fan\_msg\_deal.c文件中

```
/**
 * @brief 小程序查询设备功能设置状态
 */
void fan_app_set()
```

#### 2.8.2 取暖器功能性设置

![取暖器功能设置协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/CI23LC%E7%B3%BB%E5%88%97%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE%E4%B8%8E%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-23.png)

图2-20 取暖器功能设置协议

该协议相关函数代码在app\_ble\demo\cias\_warmer\_msg\_deal.c文件中

```
/**
 * @brief 小程序查询设备功能设置状态
 */
void warmer_app_set()
```