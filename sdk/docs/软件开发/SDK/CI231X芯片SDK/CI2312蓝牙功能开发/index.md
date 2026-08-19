<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/CI2312%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/ -->

# CI231X系列芯片-BLE蓝牙功能开发

---

## **一. CI231X系列芯片工作模式配置**

​ 通过 **CIAS\_BLE\_ENABLE** 和 **CIAS\_RF\_24G\_ENABLE** 这两个宏控制芯片射频部分的工作模式。

* **CIAS\_BLE\_ENABLE = 1： 设置芯片为BLE蓝牙工作模式，芯片工作在蓝牙ble模式，芯片作为蓝牙设备从端，手机作为主端，可以通过手机微信小程序或APP和芯片进行数据交互**
* **CIAS\_RF\_24G\_ENABLE = 1：设置芯片为2.4G工作模式**

**注意：蓝牙ble模式和2.4G收发模式是互斥的，芯片只能开启一种工作模式；蓝牙为ble模式时，收发数据最大长度固定为20字节，2.4G模式最大长度为31字节。**

## **二 . CI231X系列芯片配置为BLE模式，配合启英物联小程序进行产品开发**

SDK开启CIAS\_BLE\_ENABLE宏使能BLE模式后，可开启对应的产品ID控制宏(仅适用于”启英物联”小程序和设备端的交互协议)，实现和启英物联小程序进行不同产品类型进行数据交互，宏定义在user\_config.h和ble\_user\_config.h，如下：

```
#define DEV_DRIVER_EN_ID                   DEV_HEATTABLE_MAIN_ID     //取暖桌
```

## **三. RF收发数据接口函数(用户最关注的2个函数接口)**

* 在SDK中，定义了设备和手机进行数据交互的回调函数结构体rf\_cb\_funcs；结构体定义如下：

```
typedef struct {
    void (*rf_recv)(uint8_t* recv_data, uint8_t len);
    void (*rf_send)(uint8_t* send_data, uint8_t len);
} rf_cb_funcs_t;
rf_cb_funcs_t rf_cb_funcs; //射频发送和接收的回调函数
```

* 设备接收手机端发送的数据，SDK中已经定义好了接收函数，需要根据需求在main.c中的task\_init函数中进行注册即可，注册方式如下：

```
register_rf_callback(custom_rf_recv_data_handle);    //使用自定义协议进行接收数据处理
register_rf_callback(ci_rf_recv_data_handle);        //使用"启英物联"小程序和设备端进行交互的协议，带加密功能
```

```
/**
 * @brief 处理蓝牙/2.4G接收到的消息-适用于用户自定义协议
 * @param send_data 芯片接收到的数据；
 * @param len       接收数据长度；
 */
void custom_rf_recv_data_handle(uint8_t* recv_data, uint8_t len);
/**
 * @brief 处理蓝牙/2.4G接收到的消息-适用于启英物联小程序交互协议
 * @param send_data 芯片接收到的数据；
 * @param len       接收数据长度；
 */
void ci_rf_recv_data_handle(uint8_t* recv_data, uint8_t len);
```

* 设备发送数据到手机端，用户只需要调用发送回调函数（**注意：register\_rf\_callback函数中默认已注册**）。

```
rf_cb_funcs.rf_send = deal_rf_send_data;
```

## **四. 设备参数注册接口函数**

​ 初始化设备信息接口函数 在SDK代码rf\_msg\_deal.h文件中，定义了函数供用户调用，用于初始化蓝牙ble模式下的广播数据和服务UUID。

通过宏CIAS\_BLE\_USE\_DEFAULT\_ADV\_DATA控制广播数据的格式。

* **CIAS\_BLE\_USE\_DEFAULT\_ADV\_DATA置为1：广播数据段包含自定义的设备类型和名字。**
* **CIAS\_BLE\_USE\_DEFAULT\_ADV\_DATA置为0：广播数据有效载荷完全由用户通过ble\_adv\_data\_init函数自定义。**

函数定义如下：

```
 /**
     * @brief 初始化BLE设备服务UUID
     * @param service_uuid 主服务uuid
     * @param write_uuid   写服务uuid，用于手机向设备传输数据
     * @param service_uuid 通知服务uuid，用于设备向手机传输数据
     */
    void ble_uuid_init(uint16_t service_uuid, uint16_t write_uuid, uint16_t notify_uuid);

   /**
    * @brief 设备类型初始化-匹配启英泰伦小程序
    * @param main_type_id 设备主类型ID
    * @param sub_type_id 设备次类型ID,缺省值为0，
    * @param config_type 配置配网模式，缺省值为0，
    */
    void user_dev_init(uint8_t main_type_id, uint8_t sub_type_id, uint8_t config_type);

    bool ble_name_init(uint8_t* name);   //设备初始化广播名字,最长不能超过18字节
    void ble_mac_init(uint8_t mac[6])    //用户一般不用关注该函数，mac地址芯片出厂已经烧录

    bool ble_adv_data_init(char *adv_data)//设备初始化广播数据有效载荷，超过30字节会返回失败
```

**注意：以上初始化函数必须在蓝牙协议栈任务ble\_exec\_loop启动之前调用，否则BLE初始化会失败。**

## **五. 定时器(重要)**

**由于蓝牙ble模式需要一个高精度的tick，所以SDK会将硬件timer3作为蓝牙协议栈的tick功能，用户不能使用硬件timer3。**

## **六. 启英物联小程序**

启英物联小程序配合CI231X系列芯片完成了七个产品类型：包括灯控、风扇、茶吧机、空调、取暖器、取暖桌、水暖毯的小程序开发；客户可以烧录SDK中projects\offline\_asr\_sample\demo\_bin文件夹下的固件进行体验。

### **6.1 CI231X系列芯片SDK中提的体验固件**

**用户可以烧录固件配合启英物联小程序进行离线语音控制+BLE蓝牙控制功能体验(固件路径：projects\offline\_asr\_sample\demo\_bin)**

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/img/CI2312%E4%BD%93%E9%AA%8C%E5%9B%BA%E4%BB%B6.png)

### **6.2 不同设备类型，小程序使用流程请参考：**

☞[《小程序使用指南》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/)

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/img/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%90%84%E4%B8%AA%E8%AE%BE%E5%A4%87%E7%B1%BB%E5%9E%8B.png)