<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87ota%E5%8D%87%E7%BA%A7%E8%AF%B4%E6%98%8EV2/ -->

# 启英泰伦-3代芯片OTA方案V2

## **1. OTA升级功能概述**

​ 本文档适用于通过串口对启英泰伦3代语音芯片算法SDK固件进行OTA升级的方案，升级方式为单个分区独立升级，升级方案只对CI1302、CI1303、CI1306、CI1312、CI2305、CI2306、CI2312适用，CI1301、CI1311、CI2311不支持OTA升级。

 **CI-SDK-ASR-ALG\_V2.3.18**算法SDK已支持OTA功能，无需移植代码；OTA相关资料用户可以到 ☞语音AI平台[开发资料](https://aiplatform.chipintelli.com/attachment) 中搜索“三代芯片OTA资料”下载，如图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/ota%E8%B5%84%E6%96%99%E4%B8%8B%E8%BD%BD%E5%9C%B0%E5%9D%80.png)

## **2. Flash分区结构和升级流程介绍**

### **2.1 Flash分区说明**

​ OTA升级方案只会对分区表1、分区表2(备份分区表)、代码分区1、代码分区2(备份代码分区)、ASR分区、DNN分区、voice分区、userfile分区做升级；bootloader和nv\_data不做升级，用户无需关注，分区地址映射如下图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/%E5%88%86%E5%8C%BA%E5%9D%80%E6%98%A0%E5%B0%84%E5%9B%BE.png)

图 2-1

### **2.2 OTA升级交互流程说明**

​ OTA升级方案中升级主端为用户使用的MCU芯片或PC工具，升级从端为启英泰伦3代芯片，升级流程一共包含六个步骤，升级流程图参考图2-2：

1. 主端发送复位指令复位从端，从端请求下载升级代理程序updater到内存中运行，并检测需要升级的分区项，为升级做准备。
2. 升级代码分区1和2(代码分区如有更新，代码两个分区都需要交替更新)
3. 升级ASR分区
4. 升级DNN 分区
5. 升级voice分区
6. 升级userfile分区

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/ota_process.png)

图 2-2

### 2.3 升级代理程序(updater.bin)

updater固件为升级过程中的代理程序，从端复位以后需要下载updater到内存中运行，updater运行起来以后负责和主端进行分区升级的整个交互流程，请选择章节1下载的ota资料包中的updater文件文件（启英泰伦三代芯片OTA升级都使用该updater文件），如图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/update%E6%96%87%E4%BB%B6.png)

图 2-3

## **3. OTA升级方案参考说明**

​ 为了用户能快速将OTA升级方案使用起来，我司开发了OTA升级测试工具，工具会把OTA流程中每个步骤，以及交互数据都做详细打印说明，并提供OTA升级主端参考代码，下面做详细步骤说明：

### **3.1 算法SDK配置说明：**

支持OTA的算法SDK，需要做如下几个配置（可以参考章节1中下载的ota资料包中sdk包）

1. 需要在user\_config.h文件中使能CIAS\_FLASH\_IMAGE\_UPGRADE\_ENABLE宏，并选择升级串口号以及与BootLoader通信的串口波特率，如图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/user_config_ota_enable.jpg)

图 3-1

2. 看门狗IWDG使能(为了防止升级过程中异发生出现再也无法升级的情况，一定要将看门狗使能，芯片超时自动重启为下一次升级做准备)，看门狗使能方法如下图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/%E7%9C%8B%E9%97%A8%E7%8B%97%E5%88%9D%E5%A7%8B%E5%8C%96.png)

图 3-2

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/%E7%9C%8B%E9%97%A8%E7%8B%97%E5%96%82%E7%8B%97.png)

图 3-3

### **3.2 启英3代芯片复位说明(使用小端模式)**

**下面指令是启英OTA测试工具使用的相关指令，测试工具使用的大端模式，复位指令用户需要根据自己串口协议在业务固件中自定义，**

* 复位指令示例(大端模式)：**a5a55a5a000001050000000078563412**
* 和bootloader握手指令(大端模式)：**a50f0000a00300cfe8ff**
* 芯片收到复位指令后调用函数 *dpmu\_software\_reset\_system\_config()* 复位芯片，运行bootloader并等待握手，bootloader等待进入升级模式超时时间为 **200ms**，超时会自动加载业务固件运行，所以在发送复位指令以后，需要在200ms内发送和bootloader握手指令

### 3.3 打包工具使用说明（请使用3.9.6及以上版本的打包工具）

​ OTA语音固件打包升级工具：使用第1章节下载的OTA资料包中“固件打包工具”目录下的PACK\_UPDATE\_TOOL工具打包，打包时请注意选对应芯片型号，如图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/%E6%94%AF%E6%8C%81ota%E6%89%93%E5%8C%85%E5%B7%A5%E5%85%B72.png)

图 3-4

**特别注意打包OTA固件时需修改打包工具的config.ini配置文件，才能打包OTA固件格式，配置文件路径为对应工程的firmware目录下，将[TOOLS]组内的firmware\_version成员改为FW\_V1**

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/%E6%94%AF%E6%8C%81ota%E6%89%93%E5%8C%85%E5%B7%A5%E5%85%B73.png)

图 3-5

**保存config文件修改后，打包工具界面会弹窗提示，点击确认后并查看user分区起始地址是否已更改为0XA000**

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/%E6%94%AF%E6%8C%81ota%E6%89%93%E5%8C%85%E5%B7%A5%E5%85%B74.png)

图 3-6

### **3.4 OTA升级测试工具使用说明**

​ **使能发送串口日志会将工具发给芯片的所有数据打印出来，供开发者参考；用户如果自定义了复位指令，请将复位指令填入升级复位指令框，如果需要测试其它波特率，请将波特率填入通信波特率数据框(重要)**

* 步骤1：选择升级串口号
* 步骤2：选择升级语音芯片型号
* 步骤3：选择通信波特率（主机与语音芯片的串口波特率，默认是115200）
* 步骤4：复位指令配置(用户在SDK自定义实现串口复位指令，试用阶段可断电重启芯片实现复位)
* 步骤5：导入升级固件
* 步骤6：升级串口波特率配置(建议使用921600bit/s，最高支持2Mbit/s，用户根据主端硬件串口参数配置对应的波特率)
* 步骤7：开始升级
* 步骤8：使能发送日志功能，不建议开启，不然会增加很多中间数据打印(调试使用)

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/%E5%8D%87%E7%BA%A7%E6%B5%8B%E8%AF%95%E5%B7%A5%E5%85%B7%E9%85%8D%E7%BD%AE.png)

图 3-7

**注意（工具在使用过程中，如果点了升级按钮没有进入升级状态，请排查：1.升级串口接线及通信波特率配置是否正确，2.芯片型号是否正确 3.手动复位芯片后重试）**

### **3.5 主端参考代码说明**

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX-OTA/%E4%B8%89%E4%BB%A3%E8%8A%AF%E7%89%87OTA%E8%AF%B4%E6%98%8EV2/img/ota%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE.jpg)

图 3-8

### **3.6 主端参考代码说明**

**这里只列举主要的结构体定义和函数说明，详细参考代码，请参考章节1中下载的ota资料包中的参考代码文件**

```
//分区信息结构体
typedef struct
{
    unsigned int version;     //分区版本
    unsigned int address;     //分区起始地址
    unsigned int size;        //分区大小
    unsigned int crc;         //分区CRC16校验
    unsigned char status;     //分区当前状态 0xF0-分区有效  0xFC-需要更新的分区  0xC0-无效分区
}partition_info_t;
//分区表结构体
typedef struct
{
    unsigned int ManufacturerID;              //厂商ID
    unsigned int ProductID[2];                //产品ID

    unsigned int HWName[16];                  //硬件名称
    unsigned int HWVersion;                   //硬件版本
    unsigned int SWName[16];                  //软件名称
    unsigned int SWVersion;                   //软件版本

    unsigned int BootLoaderVersion;           //bootloader版本
    char         ChipName[9];                 //芯片名称
    uint8_t      FirmwareFormatVer;           //固件格式版本
    uint8_t      reserve[4];                  //预留

    partition_info_t user_code1;              //代码分区1信息
    partition_info_t user_code2;              //代码分区2信息
    partition_info_t asr_cmd_model;           //asr分区信息
    partition_info_t dnn_model;               //dnn分区信息
    partition_info_t voice;                   //voice分区信息
    partition_info_t user_file;               //user_file分区信息

    unsigned int     ConsumerDataStartAddr;   //nv_data分区起始地址-ota无需关注
    unsigned int     ConsumerDataSize;        //nv_data分区大小 -ota无需关注
    unsigned short   PartitionTableChecksum;    //分区表校验值
}partition_table_t;

//固件格式版本
typedef enum
{
    FW_FMT_VER_1 = 1,       //固件格式版本1，现用于CI110X SDK和CI130X_SDK、CI230X_SDK、 CI231X_SDK
    FW_FMT_VER_2,           //固件格式版本2，现用于CI110X_SDK_Lite和CI112X_SDK
    FW_FMT_VER_MAX,
} fw_fmt_ver_t;

#pragma pack()
//传输包原始数据类型，用于传输
typedef struct
{
    unsigned char data1[7];
    unsigned char *data2;
    unsigned char data3[3];
} package_raw_data_t;
//传输包解析结构类型，用于构造和解包
typedef struct
{
    unsigned short head;        //包头
    unsigned short data_length; //数据长度
    unsigned char msg_type;     //消息类型
    unsigned char cmd;          //指令
    unsigned char seq;
    unsigned char *data;        //数据
    unsigned short crc;         //CRC16校验
    unsigned char tail;         //包尾
} package_property_t;
//传输包类型
typedef union
{
    package_raw_data_t raw_data;
    package_property_t property;
} package_t;
#endif
```

```
bool StartOTAProcess(void);                    //升级主函数入口-读文件，发复位芯片指令和握手指令
bool ReadUpdaterFile(void);                    //读updater文件
void ReadImageFile(void);                      //读升级固件文件
void SerialOTAReadDataHandleSlot(void);        //串口数据处理
bool StepUpdatePartitionInfoCheck(package_t *pPackage, QByteArray rcvData); //升级步骤1-检测需要升级的分区信息
bool StepUserCodeUpdate(package_t *pPackage, QByteArray rcvData);           //升级步骤2-升级代码分区
bool StepASRUpdate(package_t *pPackage, QByteArray rcvData);                //升级步骤3-升级ASR分区
bool StepDNNUpdate(package_t *pPackage, QByteArray rcvData);                //升级步骤4-升级DNN代码分区
bool StepVoiceUpdate(package_t *pPackage, QByteArray rcvData);              //升级步骤5-升级Voice代码分区
bool StepUserFileUpdate(package_t *pPackage, QByteArray rcvData);           //升级步骤6-升级UserFile分区
```