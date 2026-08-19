<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E4%BA%91%E7%AB%AF%E8%AF%86%E5%88%AB%E6%9C%8D%E5%8A%A1/ -->

# 云端识别服务(ASR ONLINE)

---

## 1. 概述

CI110X系列芯片SDK中集成各类云端在线识别服务，在网络连接的情况下可以进行云+端识别。

---

## 2. 使用说明

### 2.1. 通过串口收发数据方式

通过串口与WIFI模块通信，包括语音数据上传、播放数据下载、主要交互命令等。

### 2.2. 代码结构

| 源文件 | 说明 |
| --- | --- |
| ci\_uart\_protocol.c ci\_uart\_protocol.h | 串口数据收发处理 |
| voice\_upload\_manage.c voice\_upload\_manage.h | 语音数据上传管理 |
| flash\_update.c flash\_update.h | OTA升级 |
| task\_uart\_send\_data.c task\_uart\_send\_data.h | 串口数据发送 |
| LinkMsgProc.c LinkMsgProc.h | 数据打包解码 |

目前支持功能：

* 单轮模式，全双工模式
* 播放控制流程
* 可设置本地VAD
* 多轮对话
* 云端本地音量控制
* 启动配网功能
* OTA升级功能

#### 2.2.1. 单轮模式，全双工模式

CI110X系列芯片SDK中支持单次唤醒上传一次语音数据，响应一次的单轮模式，和单次唤醒上传多次数据，响应多次的全双工模式。

* 单轮模式：当前SDK默认是单轮模式，唤醒后调用 upload\_wakeup\_voice\_data() 开始上传语音数据，当检测到VAD EDN或者超时时自动结束语音上传，等待识别响应。接口具体使用参考SDK代码。

```
/**
 * @brief 开始上传语音，可处理唤醒词语音
 * @param addr 语音数据地址
 * @param frames 语音帧数量
 * @param upload_wakeup 是否上传唤醒词
 * @return int RETURN_OK 成功， RETURN_ERR 失败
 */
int upload_wakeup_voice_data(unsigned int addr,int frames,bool upload_wakeup)
```

* 全双工模式：如需设置为全双工模式需要调用 enter\_full\_duplex\_mode()，进入全双工模式后在60S内设备检测到VAD START后开始上传语音，VAD EDN结束上传，云端反馈识别结果后响应结果。

```
/**
 * @brief 进入全双工模式
 */
void enter_full_duplex_mode(void)
{
    full_duplex_mode_flag = true;
    // local_vad_close();//检测到有效语音才上传
    save_voice_data_start();
    set_state_enter_wakeup(60*1000);//可设置
}
```

#### 2.2.2. 播放控制流程

目前能够支持ADPCM、MP3、M4A、WAV音频格式，详细接口可参考 ☞[《音频播放器(AUDIO-PLAYER)》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E9%9F%B3%E9%A2%91%E6%92%AD%E6%94%BE%E5%99%A8/)文档。当前SDK支持MP3的TTS播报音，MP3、M4A、WAV格式的音乐资源播放，音乐打断后等TTS响应完毕可自行恢复播放。目前不能自动区分音频资源格式，需要云端下发资源时标记音频格式。

#### 2.2.3. 可设置本地VAD

SDK默认是开启本地VAD 检测，检测到VAD END后会结束语音上传。

VAD 设置接口如下：

```
/**
 * @brief 本地VAD END 时结束语音
 */
void local_vad_open(void)
{
    use_local_vad = true;
}

/**
 * @brief 语音上传与VAD无关
 */
void local_vad_close(void)
{
    use_local_vad = false;
}
```

#### 2.2.4. 多轮对话

进入多轮对话需要由云端控制进入，所以需要云端支持发起多轮多话。

接口调用如下：

```
/**
 * @brief 进入多轮对话
 * @param timeout_ms 超时时间
 */
void enter_next_dialogue(uint32_t timeout_ms)
{
    send_cmd_to_mtk(CI_WAKEUP_STATE,NO_WAKEUP_FILL_DATA);
    set_send_voice_type(NO_VOICE_DATA);
    set_need_response_flag(false);
    set_local_player_status(0);
    set_upload_timeout(timeout_ms);/*updata timer*/
    set_wakeup_when_playing_flag( check_current_playing() );
    set_wake_up_cmd_status(1);
    save_voice_data_start();
    clear_dialogue_num();
    clear_cloud_response_num();
}
```

#### 2.2.5. 云端本地音量控制

SDK支持本地和云端共同控制音量，需要保持本地音量与云端音量同步，目前是按照百分比同步。

音量设置接口如下：

```
/**
 * @brief 设置音量，同步到云端
 * @param vol 音量值
 */
void vol_set(int32_t vol)
{
    /*set codec or PA vol*/
    if((vol <= SPK_VOL_10) && (vol >= SPK_VOL_2))
    {
        audio_play_set_vol_gain(vol*SPK_VOL_MAX/100);
        ci_logdebug(LOG_MEDIA,"sys real volume = %d\n",vol*SPK_VOL_MAX/100);
        VolSetNWrite(vol);
        send_cmd_to_mtk(SEND_LOCAL_VOLUME_CMD,volume_set_hl);
    }
    else
    {
        ci_logdebug(LOG_MEDIA,"volume error = %d\n",vol);
    }
}
```

#### 2.2.6. 启动配网功能

可以通过本地命令进入配网模式，本地识别到配网命令时，向WIFI模块发送 SEND\_ENTER\_SMARTCONFIG\_CMD 命令，进入配网模式。

配网相关命令如下：

```
SEND_ENTER_SMARTCONFIG_CMD      =  0x0863,  //进入配网模式
SEND_EXIT_SMARTCONFIG_CMD       =  0x0864,  //退出配网模式
INIT_SMARTCONFIG_CMD            =  0x0865,  // 初始密码状态 出厂配置状态
```

#### 2.2.7. OTA升级功能

本地开启版本检测，进入升级流程需要向WIFI模块发送 START\_OTA 命令，WIFI会获取服务器最新固件版本，检测是否需要更新本地固件，如果需要更新会进入升级流程。

```
START_OTA                   =  0x0882,     //进入OTA
```