<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/tencent_iot/ -->

# 启英泰伦-离线+腾讯IOT方案介绍

## 1. 方案介绍

### 1.1 功能概述

​ 启英泰伦为离线+IOT方案专业定制的CI2305芯片，芯片集成了离线ASR+WIFI+蓝牙BLE功能，用户可以利用该芯片快速开发离线+IOT方案；结合离线语音识别快，高识别率的优点，配合云端和手机APP（小程序）在线控制功能，给用户带来全新的端云融合的产品体验, 方案SDK使用启英泰伦发布的 ci230x\_audio\_iot\_sdk\_release 和 ci230x\_wifi\_combo\_sdk\_release, 下面将基于CI2305芯片+腾讯IOT云来介绍IOT方案。

### 1.2 方案框图

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/TENCENT_IOT_%E6%A1%86%E6%9E%B6%E5%9B%BE.png)

图 1-1

### 1.3 离线+IOT方案有如下功能和特性

* 具备本地识别功能和本地控制功能，当本地识别指令后可以进行控制外设，播报语音，指令上传。
* 具备云端远程控制功能，云端下发指令可对设备远程控制，远程查询设备状态，在线对设备进行OTA；同时设备也可以接收云端其它自定义功能，如闹钟，音乐播放。
* 设备支持蓝牙BLE配网和AP配网。
* 设备支持腾讯连连小程序进行控制，同时设备可以和云端，小程序端进行状态实时同步。

云端控制和本地语音控制概括图如下：

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/TENENT_IOT_%E6%B5%81%E7%A8%8B%E5%9B%BE.png)

图 1-2

## 2. IOT方案快速开发

### 2.1 腾讯IOT云平台相关操作

* #### 注册登录腾讯云开放平台

登录 ☞ [腾讯云开放平台](https://cloud.tencent.com/login?s_url=https://cloud.tencent.com/)，点击左下角 **立即注册**，注册一个腾讯云开放平台开发者账号。

![腾讯云开发者](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-1.png)

图 2-1

> **注意**：通过以上步骤注册的账号拥有最高权限。请妥善保存您的账号，避免您创建的硬件产品、App、设备运行或者用户数据受到安全风险。

* #### 项目、产品、设备创建说明

1. 在腾讯云首页按照如下图所示步骤进入到 **物联网开发平台**。

![进入物联网开发平台](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-2.png)

图 2-2

2. 进入产品控制台

![进入物联网开发平台](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-3.png)

图 2-3

3. 开发调试时使用公共实例

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-4.png)

图 2-4

4. 新建项目或选择已有项目进行开发

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-5.png)

图 2-5

5. 新建产品，选中项目进入到该项目的开发和服务中心，“产品开发”可进行产品创建并对该产品下的设备定义物模型、topic消息、小程序面板配置、配网引导、产品介绍、设备调试等，批量投产需等待审批通过。固件升级可对该项目下的产品设备OTA。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-6.png)

图 2-6

* #### 关于物联网开发平台的详细说明可在[腾讯云文档中心查看](https://cloud.tencent.com/document/product/1081/34739)

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-7.png)

图 2-7

### 2.2 CI230X系列芯片SDK接入腾讯IOT代码框架说明

![腾讯IOT_sdk代码框架](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AFIOT_sdk%E4%BB%A3%E7%A0%81%E6%A1%86%E6%9E%B6.png)

图 2-8

### 2.3 腾讯IOT云端接入流程

![IOT_设备接入云端流程](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/TENCENT_IOT_%E8%AE%BE%E5%A4%87%E6%8E%A5%E5%85%A5%E4%BA%91%E7%AB%AF%E6%B5%81%E7%A8%8B.png)

图 2-2

### 2.4 启用腾讯IOT

编译配置：开启腾讯IOT云使能 ：project\ci230x-wifi-sdk-combo\CMakeLists.txt

```
set(CIAS_IOT_CLOUD_HUAWEI           0)     #华为云使能
set(CIAS_IOT_CLOUD_XIAOMI_ENABLE    0)     #小米云使能
set(CIAS_IOT_CLOUD_CI_ENABLE        0)     #启英云使能
set(CIAS_IOT_TENCENT_ENABLE         1)     #腾讯IOT云使能
set(CIAS_IOT_TVS_ENABLE             0)     #腾讯TVS使能
set(CIAS_IOT_TUYA_ENABLE            0)     #涂鸦云使能
set(CIAS_IOT_CLOUD_ALI_ENABLE       0)     #阿里云使能
```

### 2.5 调用腾讯IOT的入口函数

project\ci230x-wifi-sdk-combo\app\usr\_ln\usr\_app.c

```
......
#if CIAS_IOT_TENCENT_ENABLE
    if( cias_qcloud_iot_main() != CIAS_OK)
    {
        LOG(LOG_LVL_ERROR, "cias_qcloud_iot_main call error\r\n");
        return CIAS_FAIL;
    }
#endif
......
```

### 2.6 修改鉴权文件(三元组)

```
#define QCLOUD_IOT_PRODUCT_ID   "ZDFXXXXX" //
#define QCLOUD_IOT_DEV_NAME     "D0XX"
#define QCLOUD_IOT_DEV_SECRET   "M72NXXXXXXXXXXXXXXX"
```

该三元组可以联系我司fae获取，每个产品必须分配不同的三元组信息，否则会出现相同三元组的产品相互竞争掉线的情况。

## 3. 主要功能API说明

### 3.1 腾讯IOT物模型初始化，该物模型与云端物模型进行匹配

```
/**_init_data_template 物模型初始化
 * 入口参数：无
 * 返回值：无
*/
void _init_data_template(void)
```

### 3.2 对云端下发的数据进行解析

```
/** deal_down_stream :云端下行数据处理
*/
int deal_down_stream(void)
```

### 3.3 将本地数据进行上传

```
/** deal_up_stream 打包上传本地数据
*/
int deal_up_stream(void)
```

### 3.4 串口打包，发送函数

```
/**
 * cias_message_send_interface 将wifi数据打包发送给audio进行处理
*/
void cias_message_send_interface(unsigned short cmd, cias_fill_type_t type, int len, unsigned char *buf)
```

### 3.5 串口数据接收，处理

```
void cias_slave_message_recv_deal(void *parameter) //串口数据接收与处理
```

## 4.腾讯IOT WIFI端无代码开发

为了提高产品开发效率，我司在腾讯IOT方案上，专门设计了一套高效的交互方案，用户无需关注WIFI端的代码开发，只需要专注做语音端的需求，即可完整个IOT方案的开发；WIFI和语音之间通讯是通过JSON的方式，在标准SDK中已经集成了该功能，WIFI端需要烧录我司开发的标准WIFI固件，下载地址☞[腾讯IOT-CJSON透传WIFI固件.zip](https://aiplatform.chipintelli.com/attachment)

## 5. 腾讯IOT 灯控Demo说明

### 5.1 腾讯IOT灯控Demo固件：

​ 腾讯IOT灯控Demo固件地址如下：☞[腾讯IOT-智能灯控Demo固件.rar](https://aiplatform.chipintelli.com/attachment)

​ 用户拿到开发板如果没有预烧录固件，可以下载固件进行体验，腾讯IOT云平台接入需要烧录鉴权文件，用户没有鉴权文件可以联系我司商务。鉴权文件具体烧录方法请参考：☞[开发环境搭建与使用-11章节](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)

### 5.2 Demo使用参考视频：☞[Demo使用视频](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88/%E6%96%B9%E6%A1%8804%EF%BC%9A%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%26%E8%85%BE%E8%AE%AF%E8%BF%9E%E8%BF%9E%E5%B0%8F%E7%A8%8B%E5%BA%8F%E6%96%B9%E6%A1%88/)

### 5.3 使用启英泰伦IOT开发板（注意：跳线帽按图中短接）

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E7%81%AF%E6%8E%A7IOT%E5%BC%80%E5%8F%91%E6%9D%BF.png)

图4-1 IOT开发板

### 5.4 配网

* 首次使用若没有在SDK中配置路由信息，开机后会主动进入配网模式；若网络已连接需要重新配网，用户需语音唤醒设备（“智能管家”），并说出命令词”开始配网“后用微信或腾讯连连小程序扫描该设备配网的二维码（位于腾讯云平台所在项目中的”产品开发“下选择对应产品，在”交互开发“下配置”配网引导“，如下图所示，保存后进入含配网二维码的页面中）

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/iot%E7%81%AF%E6%8E%A7demo%E9%85%8D%E7%BD%911.png)

图4-2 配网二维码导入页面

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/iot%E7%81%AF%E6%8E%A7demo%E9%85%8D%E7%BD%91%E4%BA%8C%E7%BB%B4%E7%A0%81.png)

图4-3 配网二维码

* 进入到小程序配网界面，勾选“我已完成上述操作”，点击“下一步”，输入待连接wifi的名称和密码，然后按照步骤连接设备热点“CI\_network\_config”，密码为12345678，连接成功后返回到小程序等待配网完成，（注意：该设备不支持5G频段wifi），为避免小程序和云平台配网消息迟滞问题，显示配网超时失败后也可返回至腾讯连连小程序先添加设备至“我的设备”，试试能否控制设备来判断是否已配网成功。设备重新上电后默认连接上次配网的wifi名称和密码，连接超时会自动进入配网模式以便再次通过小程序控制配网。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/iot%E7%81%AF%E6%8E%A7demo%E9%85%8D%E7%BD%912.jpg)![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/iot%E7%81%AF%E6%8E%A7demo%E9%85%8D%E7%BD%913.jpg)![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/iot%E7%81%AF%E6%8E%A7demo%E9%85%8D%E7%BD%914.jpg)

### 5.5 小程序使用

在腾讯连连小程序界面点击下方“+”扫描设备二维码（”产品开发“下选择对应产品，在”设备调试“列表找到对应设备），选择对应设备即可体验小程序控制。也可喊出以下命令词控制RGB灯：

| **唤醒词** | 智能管家 |  |  |  |  |
| --- | --- | --- | --- | --- | --- |
| **配网指令** | **开始配网** |  |  |  |  |
| **命令词** | 打开灯光 | 我想打开灯光 | 帮我开灯 | 替我开灯 | 开灯 |
| 关闭灯光 | 我想关闭灯光 | 帮我关灯 | 替我关灯 | 关灯 | 调亮一点 |
| 亮一点 | 再亮一点 | 最高亮度 | 灯光最亮 | 调暗一点 | 暗一点 |
| 再暗一点 | 最低亮度 | 灯光最暗 | 中等亮度 | 橙色灯光 | 黄色灯光 |
| 绿色灯光 | 蓝色灯光 | 灯光中等 | 打开橙色灯光 | 打开黄色灯光 | 打开绿色灯光 |
| 打开蓝色灯光 | 紫色灯光 | 白色灯光 | 红色灯光 | 切换灯光 | 模式切换 |
| 打开紫色灯光 | 打开白色灯光 | 打开红色灯光 | 切换模式 | 换个模式 | 打开音乐模式 |
| 音乐模式 | 音乐律动 | 打开音乐律动 |  |  |  |
|  |  |  |  |  |  |
|  |  |  |  |  |  |