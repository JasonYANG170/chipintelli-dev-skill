<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/hisense_iot/ -->

# 启英泰伦-离线+海信IOT方案介绍

## 1. 方案介绍

### 1.1 功能概述

​ 启英泰伦为离线+IOT方案专业定制的CI2305芯片，集成了离线ASR+WIFI+蓝牙BLE功能，用户可以利用该芯片快速开发离线+IOT方案；结合离线语音识别快，高识别率的优点，配合云端和手机APP（小程序）在线控制功能，给用户带来全新的端云融合的产品体验。 方案SDK使用启英泰伦发布的 ci230x\_audio\_iot\_sdk\_release 和 ci230x\_wifi\_combo\_sdk\_release，下面将基于CI2305芯片+海信IOT云来介绍IOT方案。

### 1.2 方案框图

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/HISENSE_IOT_%E6%A1%86%E6%9E%B6%E5%9B%BE.png)

图 1-1

### 1.3 方案特点

离线+IOT方案有如下功能和特性

* 具备本地识别和本地控制功能，当本地识别指令后可控制外设，播报语音，指令上传等。
* 具备云端远程控制功能，云端下发指令可对设备远程控制，远程查询设备状态，在线对设备进行OTA。
* 设备支持蓝牙BLE配网和AP配网。

---

## 2. IOT方案快速开发

### 2.1 SDK框架图说明

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E6%B5%B7%E4%BF%A1IOT_sdk%E4%BB%A3%E7%A0%81%E6%A1%86%E6%9E%B6.jpg)

图 2-1

---

### 2.2 云端接入流程

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/HISENSE_IOT_%E8%AE%BE%E5%A4%87%E6%8E%A5%E5%85%A5%E4%BA%91%E7%AB%AF%E6%B5%81%E7%A8%8B.png)

图 2-2

### 2.3 启用海信IOT

编译配置：开启海信IOT云使能 ：project\ci230x-wifi-sdk-combo\CMakeLists.txt

```
set(CIAS_IOT_CLOUD_HUAWEI           0)     #华为云使能
set(CIAS_IOT_CLOUD_HISENSE_ENABLE   1)     #海信IOT使能
set(CIAS_IOT_CLOUD_XIAOMI_ENABLE    0)     #小米云使能
set(CIAS_IOT_CLOUD_CI_ENABLE        0)     #启英云使能
set(CIAS_IOT_TENCENT_ENABLE         0)     #腾讯IOT云使能
set(CIAS_IOT_TVS_ENABLE             0)     #腾讯TVS使能
set(CIAS_IOT_TUYA_ENABLE            0)     #涂鸦云使能
set(CIAS_IOT_CLOUD_ALI_ENABLE       0)     #阿里云使能
```

### 2.4 调用海信的入口函数

project\ci230x-wifi-sdk-combo\app\usr\_hisense\usr\_app.c

```
void creat_usr_app_task(void)
{
    ...
    #if CIAS_IOT_CLOUD_HISENSE_ENABLE
        ci_hisense_main();
    #endif
}
```

### 2.5 修改鉴权文件

project\ci230x-wifi-sdk-combo\app\cias\_aiot\_wifi\cias\_cloud\cloud\_iot\_hisense\hisense\_main\_task.h

```
#define   WIFIID                    "86100c00900500100000004080cbbc6eb568"
#define   HISENSE_MD5_KEY            "6ecefdcd7aac8d64"
#define   HISENSE_AP_KEY             "HisenseOverSeasIOTSetUpProtocal;"
```

对应的三元组信息请联系我司fae获取，每个产品的wifiid，与MD5 key值不能相同。

## 3 主要功能API说明

### 3.1 md5加密

```
/** hisense_md5_encrypt 海信MD5计算 
 * 入口参数：*h_encrypt，原始数据地址
 * 入口参数：*h_hisense_md5，秘钥接收地址
 * 返回值 void
*/
void hisense_md5_encrypt(unsigned char *h_encrypt,char* h_hisense_md5)
```

### 3.2 base64 加密

```
/** other_base64_encode 海信base64 加密
 * 入口参数：in 待加密数据地址
 * 入口参数：in_len 加密数据
 * 入口参数：out  加密后数据地址
 * 返回值： erro 0，success 加密后的数据长度
*/
static int other_base64_encode(const uint8_t *in, uint16_t in_len, char *out)
```

### 3.3 base64 解密

```
/** other_base64_decode 海信base64 解密
 * 入口参数：*in 待解密数据地址
 * 入口参数：*out 解密数据地址
 * 入口参数：*out_len 解密数据长度
 * 返回值：err -1，success 0
*/
static int other_base64_decode(const char *in, uint8_t *out, uint16_t *out_len)
```

### 3.4 aes 加密

```
/**hisense_aes_encrypt 海信aes加密
 * 入口参数：aes_src 待加密数据
 * 入口参数：aes_key 秘钥
 * 入口参数：aes_dst 加密后数据
 * 返回值：err NULL，success 加密后的数据首地址
*/
void* hisense_aes_encrypt(int8_t* aes_src,int8_t* aes_key,int8_t* aes_dst)
```

### 3.5 aes解密

```
/**hisense_aes_decrypt 海信aes解密
 * 入口参数：hisense_src 待解密数据
 * 入口参数：hisense_key 秘钥
 * 入口参数：hisense_dst 解密后数据
 * 返回值：err NULL，success 解密后的数据首地址
*/
void* hisense_aes_decrypt(uint8_t *hisense_src,uint8_t *hisense_key,uint8_t *hisense_dst)
```

### 3.6 海信数据接收任务

```
/**hisense_cloud_recv 海信云数据接收
 * 入口参数：hisense_cloud_fd 连接套接字
 * 返回值： 无
*/
static void hisense_cloud_recv(int32_t hisense_cloud_fd)
```

## 4. 海信IOT demo使用