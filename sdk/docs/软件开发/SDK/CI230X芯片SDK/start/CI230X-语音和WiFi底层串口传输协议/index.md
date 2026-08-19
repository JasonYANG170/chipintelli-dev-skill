<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/CI230X-%E8%AF%AD%E9%9F%B3%E5%92%8CWiFi%E5%BA%95%E5%B1%82%E4%B8%B2%E5%8F%A3%E4%BC%A0%E8%BE%93%E5%8D%8F%E8%AE%AE/ -->

# **CI230X系列芯片-语音端和WIFI端串口传输协议**

## 1. 概述

​ 为了降低语音端和WIFI端的耦合性，增加芯片运行稳定性，CI230X系列芯片内部运行了2套独立的freeRTOS系统；语音端和WIFI端通过串口进行通信，通信速率可根据需求设置为9600bits/s~2Mbits/s；CI2306芯片使用离在线方案，传输音频数据到云端时，串口通信速率至少设置为921600bits/s。

​ 下面将详细介绍语音端和WIFI端通信的数据结构、通信协议、以及通信指令；如果用户使用第三方WIFI芯片配合启英泰伦CI130系列语音芯片使用，该协议同样适用。

## 2. 传输交互数据-数据结构：

```
typedef struct
{
    unsigned int magic; /* Start data frame. Let's define as 0x5a5aa5a5; */
    unsigned short checksum; /* checksum */
    unsigned short type; /* Define command type. */
    unsigned short len; /* Define data stream len. */
    unsigned short version; /*版本信息*/   
    unsigned int fill_data; /*填充数据*/
}cias_standard_head_t;
```

## 3. 传输交互数据-数据字段解析：

| 名称 | 字节长度 | 含义 |
| --- | --- | --- |
| magic | 4 | 默认协议头，unsigned int类型，默认“5a5aa5a5” |
| checksum | 2 | 校验和， unsigned short类型，默认“0000” |
| type | 2 | 当前消息的类型，unsigned short类型 |
| len | 2 | 当前消息中的数据长度（不加结构体本身长度，仅表示数据长度） |
| version | 2 | version”版本信息，默认“00” |
| fill\_data | 4 | 填充数据，默认“0x12345678” 代表的含义如下 所示 |
|  |  | 若有其他数据将直接跟在该结构体的后面,eg memcpy(output + sizeof(cias\_data\_standard\_head\_t), (uint8\*)input, len); （注意，用户要自己管理output的内存） |

```
#define INVAILD_SPEAK                (0x12345666)
#define RECV_TTS_PLAY                (0x12345677)
#define RECV_MP3_PLAY                (0x12345688)
#define RECV_M4A_PLAY                (0x123456aa)
#define IDLE_STATUS_RECV_M4A_PLAY    (0x123456ab)
#define RECV_WAV_PLAY                (0x123456bb)
#define DEF_FILL                     (0x12345678)
#define WAKEUP_FILL_DATA             (0x12345678)
#define NO_WAKEUP_FILL_DATA          (0x0)
```

​

## 4. 示例：语音端传输一包PCM数据到WIFI端流程：

### 4.1 应用层发送：

```
voice_data_packet_and_send(SEND_PCM_MIDDLE, dst_addr, encode_t);
```

### 4.2 底层打包：

```
broadlink_frame_create(packet_type, voice_data, packet_len, 
(broad_link_buf + buff_count * 1024),1024,0x12345678,0);
```

### 4.3 添加到发送队列：

```
send_msg_to_network_task(&send_msg,NULL)
```

### 4.4 串口发送：

```
network_send((int8_t *)data->payload,data->length)
```

## 5.语音端和WIFI端串口交互指令

​ 语音端和wifi端串口通信指令从0x0101~0x0fff为系统内部使用，用户如果需要添加指令，请添加该范围以外的值，否则会出现冲突，导致通信异常。

* 语音端指令定义源文件

SDK/projects/cias\_aiot\_audio/cias\_msg\_handle/cias\_network\_msg\_protocol.h

* wifi端指令定义源文件：

SDK/project/ci230x-wifi-sdk-combo/app/cias\_aiot\_wifi/cias\_msg\_handle/cias\_slave\_message\_handle.h

| 指令类型 | 指令值 | 指令说明 |
| --- | --- | --- |
| 语音识别指令 | 0x0101 | 本地语音识别通知 |
|  | 0x0102 | 唤醒 |
|  | 0x0103 | 云端VAD END |
|  | 0x0104 | 跳过无效语音 |
|  | 0x0105 | PCM数据中间包 |
|  | 0x0106 | PCM数据结束包 |
|  | 0x0107 | PCM数据空闲 |
|  | 0x0108~0x01ff | 预留 |
|  |  |  |
| 网络播放指令 | 0x0201 | 开始播放 |
|  | 0x0202 | 暂停播放 |
|  | 0x0203 | 恢复播放 |
|  | 0x0204 | 停止播放 |
|  | 0x0205 | 重新播放 |
|  | 0x0206 | 播放下一首 |
|  | 0x0207 | 播放本地TTS |
|  | 0x0208 | 播放结束 |
|  | 0x0209 | 重新获取播放链接 |
|  | 0x020a | 获取后续播放数据 |
|  | 0x020b | 接收播放数据 |
|  | 0x020c | 播放数据接收完 |
|  | 0x020d | 播放tts结束 |
|  | 0x020e | 播放空指令 |
|  | 0x020f~0x02ff | 预留 |
|  |  |  |
| IOT自定义协议 | 0x0301 | 云端IOT指令 |
|  | 0x0302 | 云端音量 |
|  | 0x0303 | 本地音量 |
|  | 0x0304 | 增大音量 |
|  | 0x0305 | 减小音量 |
|  | 0x0306 | 最大音量 |
|  | 0x0307 | 最小音量 |
|  | 0x0308~0x03ff | 预留 |
|  |  |  |
| 网络配置指令 | 0x0401 | 进入配网模式 |
|  | 0x0402 | 正在配网 |
|  | 0x0403 | 退出配网模式 |
|  | 0x0404 | 初始密码状态 |
|  | 0x0405 | 网络断开 |
|  | 0x0406 | 网络连接成功 |
|  | 0x0407 | 已获取鉴权文件 |
|  | 0x0408 | 需要配置鉴权文件 |
|  | 0x0409 | 云端已连接 |
|  | 0x040a | 云端已断开 |
|  | 0x040a~0x04ff | 预留 |
|  |  |  |
| OTA相关指令 | 0x0501 | 开始ota |
|  | 0x0502 | ota 数据 |
|  | 0x0503 | ota升级成功 |
|  | 0x0504 | 生产测试 |
|  | 0x0505 | 生产测试成功 |
|  | 0x0506 | 生产测试失败 |
|  | 0x0507 | 自测试 |
|  | 0x0508 | 红外数据发送 |
|  | 0x0509 | 红外码库下载中 |
|  | 0x050a | 红外码库下载完成 |
|  | 0x050b | 红外下载码库开始 |
|  | 0x050c~0x05ff | 预留 |
|  |  |  |
|  | 0x0601~0x0fff | 预留 |