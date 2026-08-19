<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/tencent_tvs/ -->

# 启英泰伦-腾讯TVS离在线方案

## 1. 方案介绍

### 1.1 方案概述

启英泰伦为离在线方案专业定制的CI2306芯片，芯片集成了离线ASR+WIFI+蓝牙BLE功能，用户可以利用该芯片快速开发离在线方案；结合离线语音识别快，高识别率的优点，配合云端NLP在线识别和海量的内容服务，给用户带来全新的端云融合的产品体验, 方案SDK使用启英泰伦发布的 ci230x\_aiot\_offlineASR&onlineASR\_sdk\_release 和 ci230x\_aiot\_wifi\_combo\_sdk\_release ,下面将基于CI2306芯片+腾讯TVS云小微来介绍离在线方案。

### 1.2 方案框图

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/TVS_%E6%A1%86%E6%9E%B6%E5%9B%BE.png)

图 1-1 离在线方案框图

### 1.3 离在线空调方案

* 本方案为空调定制的离在线方案（用户也可根据需求应用在其他离在线产品上），方案集成双MIC+AEC回声消除算法、盲源分离算法，具有良好的抗噪性能，用户可自定义100+命令词；设备支持蓝牙BLE配网和AP配网。

---

## 2. 离在线方案快速开发

### 2.1 腾讯云小微配套启英CI2306 SDK开发流程

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-03.png)

图 2-1

### 2.2 腾讯云小微平台相关操作

* #### 注册登录腾讯云小微开放平台

1. 登录 ☞ [腾讯云小微开放平台](https://dingdang.qq.com/open#/)，点击右上角 **登录**，注册一个腾讯云小微开放平台开发者账号。
2. 开发者账号注册后，使用已注册的账号登录 ☞ [腾讯云小微开放平台](https://dingdang.qq.com/open#/)；可以创建设备产品和云端技能，并进行调试。如果创建产品和技能发布，需要通过后台审核。
3. 开发者认证：开发者认证账号是指在 ☞ [腾讯云小微开放平台](https://dingdang.qq.com/open#/)注册的账号，并且填写企业相关信息后，通过腾讯云小微的认证。
4. 有关开发者认证的具体操作流程，请参见 ☞ [开发者认证流程](https://dingdang.qq.com/doc/page/16)。

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-20.png)

   图 2-2

> **注意**：通过以上步骤注册的账号拥有最高权限。请妥善保存您的账号，避免您创建的硬件产品、App、设备运行或者用户数据受到安全风险。

* #### 云小微开放平台支持用户根据自身需求创建自定义应用，下面对云小微应用创建与技能配置做详细说明

1. 选择设备开放平台 **点击进入**。

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-22.png)

   图 2-3
2. 创建应用，点击 设备平台下的 **新建应用**

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-23.png)

   图 2-4
3. 配置设备系统与应用场景：选择预期接入应用的设备，其中有屏设备包换手机应用、车载设备、电视、手表、有屏机器人、有屏音响等，无屏设备主要包换无屏音响、耳机、微信公众号、无屏机器人等。不同的设备可支持的操作系统也不同，需要根据您的实际情况进行选择。

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-24.png)

   图 2-5
4. 填写应用基本信息，输入应用名称和应用描述并选择页面展示方式。

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-25.png)

   图 2-6
5. 填写版本号，格式为x.x.x.x，例如1.0.0.0。版本号生效以后将无法修改。版本号主要用来配合终端版本的更新，也可以用版本号来区分所创建的应用。

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-26.png)

   图 2-7
6. 点击\*\*下一步\*\*，查看创建的应用相关信息，添加应用描述后，再点击\*\*完成\*\*，即完成应用创建与技能配置。

* #### 应用配置

1. 基本信息配置：应用建立后，可以查看和配置信息

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-27.png)

   图 2-8
2. 版本信息配置：开发者可在【应用版本】-【技能配置】中进行技能的修改，如需上线正式环境给外部用户使用，请务必先发布。

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-28.png)

   图 2-9
3. 更多配置请查看 ☞[腾讯云小微应用和技能配置](https://dingdang.qq.com/doc/page/331)，需要配合商务同步进行。
4. 厂商APP接入：DMSDK是Device Management SDK的缩写。DMSDK被设备对应的伴生App接入，为伴生App提供账号授权、设备绑定、音乐服务授权、闹钟管理和音色控制等一系列接口，作为伴生App与TVS硬件设备之间的桥梁，让用户能够使用伴生App管理自己的TVS智能设备。腾讯云小微平台提供了Android和IOS SDK。接入请查看 ☞[厂商APP接入说明](https://dingdang.qq.com/doc/page/342)。

### 2.3 CI230X系列芯片SDK接入腾讯云小微代码框架说明

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/TVS_SDK%E4%BB%A3%E7%A0%81%E6%A1%86%E6%9E%B6.png)

图 2-10

---

### 2.4 腾讯TVS云端接入

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/TVS_%E8%AE%BE%E5%A4%87%E6%8E%A5%E5%85%A5%E4%BA%91%E7%AB%AF%E6%B5%81%E7%A8%8B.png)

图 2-11

### 2.5 启用腾讯TVS

编译配置：开启腾讯TVS云使能 ：project\ci230x\_aiot\_wifi\_combo\_sdk\_release\CMakeLists.txt

```
set(CIAS_IOT_CLOUD_HUAWEI           0)     #华为云使能
set(CIAS_IOT_CLOUD_XIAOMI_ENABLE    0)     #小米云使能
set(CIAS_IOT_CLOUD_CI_ENABLE        0)     #启英云使能
set(CIAS_IOT_TENCENT_ENABLE         0)     #腾讯IOT云使能
set(CIAS_IOT_TVS_ENABLE             1)     #腾讯TVS使能
set(CIAS_IOT_TUYA_ENABLE            0)     #涂鸦云使能
set(CIAS_IOT_CLOUD_ALI_ENABLE       0)     #阿里云使能
```

### 2.6 调用腾讯tvs的入口函数

project\ci230x-wifi-sdk-combo\app\usr\_ln\usr\_app.c

```
void creat_usr_app_task(void)
{
    ...
    cias_tvs_main();
}
```

### 2.7 修改鉴权文件

project\ci230x\_aiot\_wifi\_combo\_sdk\_release\app\cias\_aiot\_wifi\cias\_cloud\cloud\_tvs\tvs\_adapter\tvs\_auth\_manager.h

```
#define CLOUD_TVS_DEFAULT_PROFILE "QQOPEN,101470979,
                                9C78976F12BAD3D527D4D5773017D38F,
                                D53FA920E9791C7BF6B30CF3F3FE50A3,
                                refreshToken,43733cc6009c4b18,7776000,    feb711004ae411e9b83f6713e1d602f3:db4eee2b600a49b3b7ee3e764513c61c,
                                8989898911280721"
```

测试的时候可以只修改最后一个元素(898989898911280721)的值，就可被认证为不同的鉴权文件，每个产品应该分配不同的鉴权文件，否则会出现相同鉴权文件的产品相互竞争掉线的情况。

---

## 3. 主要功能API说明

### 3.1 SDK和云端的交互

#### 3.1.1 唤醒云端识别

```
功能：收到本地唤醒词后，调用此函数，同步唤醒云端开始识别
参数：无
返回值：0代表成功，其他代表失败

int tvs_api_impl_speech_wakeup()
```

#### 3.1.2 上传本地音频数据

```
功能：将外部编码压缩后的PCM音频数据 上传到TVS云端，用于云端识别
参数：src_addr，上传数据地址
     data_len，上传数据长度
返回值：返回上传成功的长度，与data_len相同代表成功

int32_t outside_encode_data_write(int8_t* src_addr,int32_t data_len)
```

#### 3.1.3 主动结束云端识别

```
功能：收到本地vad，调用此函数，主动结束云端识别
参数：无
返回值：无
void tvs_api_impl_speech_write_end()
```

#### 3.1.4 云端识别结束

```
功能：云端识别结束后的回调函数
参数：session_id，本次识别分配的云端sessionid,可用于云端日志查询
      error，本次识别的结果状态，0（TVS_API_AUDIO_PROVIDER_ERROR_NONE）代表成功
返回值：无
static void callback_on_reader_stop(int session_id, int error)
```

#### 3.1.5 云端asr识别结果输出

```
功能：云端识别结束后，输出识别结果的回调函数
参数：asr_text，识别结果，unincode字符，可供用户使用。
     is_end，为1表示成功
返回值：无
void tvs_callback_on_asr(const char* asr_text, bool is_end)
```

### 3.2 播放器相关逻辑（mp3+m4a+tts）

#### 3.2.1 播报器初始化

```
功能：初始化media player adapter，赋予SDK播放网络媒体的能力
参数：ad，表示播放器操作的结构体
返回值：无
void tvs_init_mediaplayer_adapter_impl(tvs_mediaplayer_adapter *ad)
```

#### 3.2.2 启动播放

```
功能：云端下发媒体播放指令后，sdk调用平台媒体播放器，播放下发的流媒体url
参数：token，可忽略
返回值：0代表成功
static int mediaplayer_adapter_start_play(const char* token)
```

#### 3.2.3 恢复播放

```
功能：在SDK退出忙状态，比如说云端智能语音对话结束等情况下，会自动调用此接口，继续播放之前的流媒体。
注意：本地命令词识别成功后，不会继续播放，播放进入暂停状态，需主动对话(如：继续播放）恢复。
参数：token，可忽略
返回值：0代表成功
static int mediaplayer_adapter_resume_play(const char* token)
```

#### 3.2.4 停止播放

```
功能：当设备正在播放一首从云端下发的流媒体的时候，用户可以通过说”停止播放“、”暂停播放“等指令，
      停止当前的媒体播放。
参数：token，可忽略
返回值：0代表成功
static int mediaplayer_adapter_stop_play(const char* token)
```

### 3.3 SDK和语音芯片端的交互

SDK和语音芯片的数据交互功能，请参考：CI230X系列芯片串口协议说明文档

主要的API有如下：

#### 3.3.1 启动数据接收任务

```
功能：接收语音芯片的数据，持续读取接收队列的消息
参数：parameter，可忽略
返回值：无
void cias_slave_message_recv_deal(void *parameter)
```

#### 3.3.2 接收语音芯片端数据

```
功能：收到语音端数据后，调用此函数处理数据，收到的数据包括：语音端发送的控制指令，播放时获取网络音频数据的指令，
     压缩后需要上传到云端的语音数据，
参数：msg_buf，数据地址
      msg_len，数据长度
返回值：0，代表成功
int32_t cias_slave_msg_handle(uint8_t *msg_buf, int32_t msg_len)
```

#### 3.3.3 发送语音芯片数据

```
功能：SDK调用此函数，发送数据到语音芯片端，发送的数据包括：控制指令，网络音频数据等。
参数：cmd，数据类型，具体定义参见串口协议文档
     type，默认为DEF_FILL
     len，附加的数据长度
     buf，附加的数据
返回值：0，代表成功
void cias_message_send_interface(unsigned short cmd, cias_fill_type_t type, 
                                  int len, unsigned char *buf)
```

## 4.语言模型

​ **注意**：由于离在线SDK做了内存优化，目前只支持较小的语言模型(也有很好的识别率)；用户开发离在线应用时，在启英泰伦开发者平台做语言模型需要注意，只能选择0557模型，如下图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E7%A6%BB%E5%9C%A8%E7%BA%BF%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B.png)

## 5. 腾讯云小微离在线音响Demo说明

### 5.1 腾讯云小微离在线音响Demo固件：

​ 离在线腾讯TVS云小微音响Demo固件地址：☞[腾讯TVS-离在线双麦音响Demo固件.rar](https://aiplatform.chipintelli.com/attachment)

​ 用户拿到开发板如果没有预烧录固件，可以下载固件进行体验，腾讯TVS云平台接入需要烧录鉴权文件，用户没有鉴权文件可以联系我司商务。鉴权文件具体烧录方法请参考：☞[开发环境搭建与使用-11章节](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)

### 5.2 Demo使用参考视频：☞[Demo使用视频](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88/%E6%96%B9%E6%A1%8802%EF%BC%9A%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%26%E8%85%BE%E8%AE%AF%E4%BA%91%E5%B0%8F%E5%BE%AE%E5%9C%A8%E7%BA%BF%E8%AF%AD%E9%9F%B3%E6%96%B9%E6%A1%88/)

### 5.3 使用启英泰伦离在线开发板（注意：跳线帽按图中短接）

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/img/%E7%A6%BB%E5%9C%A8%E7%BA%BF%E5%BC%80%E5%8F%91%E6%9D%BF.png)

图 4-1 离在线开发板

### 5.4 配网

需要重新配置WIFI网络，请在唤醒设备（智能管家）后，喊话“开始配网”。设备语音回复后，请用腾讯连连小程序开始配网。配网成功并连接上云端后，设备会语音回复”网络已连接“，此时设备可以正常使用。

### 5.5 控制设备

设备的语音控制功能由客户自行编辑，提供的demo里面初始集成了一部分语音命令用于控制空调和音量。首先唤醒设备，设备回复“我在”后，再喊话输入控制命令。demo中已提供的本地基础命令词如下：

| 唤醒词 | 智能管家 |  |  |  |
| --- | --- | --- | --- | --- |
| **配网指令** | **开始配网** |  |  |  |
| **命令词** | 调高温度 | 二十二度 | 最大风 | 最大音量 |
| 打开空调 | 调高一度 | 二十三度 | 最小风 | 最小音量 |
| 关闭空调 | 调低温度 | 二十四度 | 增大风量 | 增大风速 |
| 打开制冷 | 调低一度 | 二十五度 | 减小风量 | 减小风速 |
| 打开制热 | 十六度 | 二十六度 | 左右扫风 | 开始配网 |
| 打开加热 | 十七度 | 二十七度 | 关闭左右扫风 | 退出配网 |
| 打开除湿 | 十八度 | 二十八度 | 上下扫风 |  |
| 打开节能 | 十九度 | 二十九度 | 关闭上下扫风 |  |
| 关闭节能 | 二十度 | 三十度 | 增大音量 |  |
| 调高温度 | 二十一度 | 三十一度 | 减小音量 |  |

### 5.6 云端对话

设备的云端对话能力有腾讯云小微支持，首先唤醒设备，设备回复“我在”后，再喊话输入想要的命令。可用于客户测试的命令如下，客户可以自行扩展。

| 给我播放一首歌 |
| --- |
| 我想听儿歌 |
| 我想听流行歌 |
| 来一首流行歌 |
| 我想听音乐 |
| 给我播放一首歌 |
| 播放一首七里香 |
| 帮我打开网络歌 |
| 播放刘德华的歌 |
| 我想听李建的歌 |
| 我要听纯音乐 |
| 我要听流行音乐 |
| 北京的天气怎么样 |
| 智能管家，讲一个笑话 |
| 智能管家，中国有多少人口 |
| 智能管家，音量调到百分之五十 |
| 智能管家，音量调到百分之一百 |

若云端不支持输入的命令或语音识别，会回复“没有听懂您说什么”等提示。