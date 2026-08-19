<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E5%8F%82%E8%80%83/Speech-API/%E9%9F%B3%E9%A2%91%E7%B3%BB%E7%BB%9F%E7%AE%A1%E7%90%86/ -->

# 音频系统管理(CODEC MANAGER)

---

## 1、音频系统简介

* CI13XX作为一款智能语音芯片，其语音通路（CODEC、IIS、IISDMA）占有非常重要的部分。CI13XX还拥有片内的CODEC（单通道ADC和单通道DAC），使用CI13XX进行拾音和播放，可以不再外接CODEC芯片，大大增加了系统的稳定性，且降低了成本。专用的IISDMA使得配置IIS接收或者发送数据更加灵活、方便，且不用考虑其他外设使用DMA对其造成影响。且CI13XX为片内CODEC的ADC和DAC设计了单独的IIS，二者独立工作，互不影响，使用灵活。另外，CI13XX还有一路IIS为外接CODEC预留，保证用户依然可以根据自己的需要选择其他的CODEC芯片，采集多路语音，已满足更复杂的算法需求。拾音和播放组件将CODEC、IIS和IISDMA串联起来，形成了CI13XX的语音通路。

---

## 2、音频系统管理程序简介

![音频系统管理放音API](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E5%8F%82%E8%80%83/Speech-API/img/%E9%9F%B3%E9%A2%91%E7%B3%BB%E7%BB%9F%E7%AE%A1%E7%90%86-1.png)

图1-1 音频系统管理放音API

![音频系统管理录音API](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E5%8F%82%E8%80%83/Speech-API/img/%E9%9F%B3%E9%A2%91%E7%B3%BB%E7%BB%9F%E7%AE%A1%E7%90%86-2.png)

图1-2 音频系统管理录音API

* CI13XX的SDK提供了一套拾音设备和播放设备的管理程序。通过调用这套驱动程序提供的API，用户就可以完成常规配置的拾音和播放操作，而不需要考虑繁琐的IIS、IISDMA、CODEC的配置。这套驱动基于FreeRTOS编写，因此只能在FreeRTOS的基础上运行。

---

## 3、API列表

| 函数名 | 描述 |
| --- | --- |
| cm\_reg\_codec | 注册codec |
| cm\_config\_pcm\_buffer | 配置录音/放音 PCM buffer |
| cm\_config\_codec | 配置录音/放音 音频格式 |
| cm\_start\_codec | 开始录音/放音 |
| cm\_stop\_codec | 停止录音/放音 |
| cm\_read\_codec | 获取录音数据 |
| cm\_write\_codec | 写入放音数据 |
| cm\_get\_pcm\_buffer | 获取放音 PCM buffer |
| cm\_release\_pcm\_buffer | 释放放音 PCM buffer |
| cm\_set\_codec\_dac\_gain | 设置放音音量 |
| cm\_set\_codec\_alc | 设置codec alc |
| cm\_set\_codec\_dac\_enable | 设置codec dac 使能 |
| cm\_set\_codec\_mute | 录音设备静音 |

## 4、录音示例

驱动文件地址：”SDK\components\codec\_manager\codec\_manager.c”

初始化代码、获取数据的示例：

```
#include "audio_in_manage_inner.h"
#include "codec_manager.h"
#include "audio_play_api.h"
#include "audio_play_decoder.h"

#define RECORD_CODEC_INDEX 0

const cm_codec_hw_info_t inner_codec_info = 
{
    .IICx = IIC_NULL,
    .input_iis.IISx = IIS1,
    .input_iis.iis_mode_sel = IIS_MASTER,
    .input_iis.oversample = IIS_MCLK_FS_256,
    .input_iis.clk_source = AUDIO_PLAY_CLK_SOURCE_IPCORE,
    .input_iis.mclk_out_en = IIS_MCLK_OUT,
    .input_iis.iis_data_format = IIS_DF_IIS,
    .input_iis.sck_lrck_radio = IIS_SCK_LRCK_64,
    .input_iis.rx_cha = IIS_RX_CHANNAL_RX0,
    .input_iis.scklrck_out_en = IIS_SCKLRCK_OUT,
    codec_if = 
    {
        .codec_init = icodec_init,
        .codec_config = icodec_config,
        .codec_start = icodec_stop,
        .codec_ioctl = icodec_ioctl,
    }
}

const cm sound_info_t record_sound_info = 
{
    .sapmple_rate = 16000,
    .channel_flag = 3,
    .sample_depth = IIS_DW_16BIT,
}

void record_task(void* p)
{
    //注册内部codec
    cm_reg_codec(RECORD_CODEC_INDEX, (cm_codec_hw_info_t*)&inner_codec_info);

    //配置录音PCM buffer
    cm_record_buffer_info_t record_buffer_info;
    record_buffer_info.block_num = 2;
    record_buffer_info.block_size = 320*4; //576;
    record_buffer_info.buffer_size = record_buffer_info.block_size * record_buffer_info.block_num;
    record_buffer_info.pcm_buffer = pvPortMalloc(record_buffer_info.buffer_size);
    cm_config_pcm_buffer(RECORD_CODEC_INDEX, CODEC_INPUT, &record_buffer_info);

    //配置录音音频格式
    cm_config_codec(RECORD_CODEC_INDEX, CODEC_INPUT, &record_sound_info);

    //开始录音
    cm_start_codec(RECORD_CODEC_INDEX, CODEC_INPUT);

    while(1)
    {
        //录音
        uint32_t data_addr, data_size;
        cm_read_codec(RECORD_CODEC_INDEX, &data_addr, &data_size);

        if(data_addr)
        {

        }
        else
        {
            //mprintf("iisdma int too slow\n");
            continue;
        }
    }
}
```

## 5、放音示例

驱动文件地址：”SDK\components\codec\_manager\codec\_manager.c”

初始化代码、写入数据播放示例：

```
#include "audio_in_manage_inner.h"
#include "codec_manager.h"
#include "audio_play_api.h"
#include "audio_play_decoder.h"

#define PLAYER_CODEC_INDEX 0
#define ALG_FRAME_SIZE (320) /*16ms perframe*/

uint16_t send_buf_addr[2048] = {0};

const cm_codec_hw_info_t inner_codec_info = 
{
    .IICx = IIC_NULL,
    .output_iis.IISx = IIS1,
    .output_iis.iis_mode_sel = IIS_MASTER,
    .output_iis.oversample = IIS_MCLK_FS_256,
    .output_iis.clk_source = AUDIO_PLAY_CLK_SOURCE_INTER_RC,
    .output_iis.mclk_out_en = IIS_MCLK_MODENULL,
    .output_iis.iis_data_format = IIS_DF_IIS,
    .output_iis.sck_lrck_radio = IIS_TX_CHANNAL_TX0,
    .output_iis.sck_lrck_radio = IIS_SCK_LRCK_64,
    .output_iis.scklrck_out_en = IIS_SCKLRCK_MODENULL,
    codec_if = 
    {
        .codec_init = icodec_init,
        .codec_config = icodec_config,
        .codec_start = icodec_stop,
        .codec_stop = icodec_stop,
        .codec_ioctl = icodec_ioctl,
    }
}

const cm sound_info_t play_sound_info = 
{
    .sapmple_rate = 16000,
    .channel_flag = 3,
    .sample_depth = IIS_DW_16BIT,
}

void play_task(void* p)
{
    //注册内部codec
    cm_reg_codec(PLAYER_CODEC_INDEX, (cm_codec_hw_info_t*)&inner_codec_info);

    //配置放音PCM buffer
    cm_play_buffer_info_t play_buffer_info;
    play_buffer_info.block_num = 2;
    play_buffer_info.buffer_num = 4;
    play_buffer_info.block_size = 320*4;
    play_buffer_info.buffer_size = play_buffer_info.block_size * play_buffer_info.block_num;
    play_buffer_info.pcm_buffer = pvPortMalloc(play_buffer_info.buffer_size*play_buffer_info.buffer_num);
    cm_config_pcm_buffer(PLAYER_CODEC_INDEX, CODEC_OUTPUT, &play_buffer_info);

    //配置放音音频格式
    cm_config_codec(PLAYER_CODEC_INDEX, CODEC_OUTPUT, &play_sound_info);

    //开始放音
    cm_start_codec(PLAYER_CODEC_INDEX, CODEC_OUTPUT);

    while(1)
    {
        //放音
        uint32_t ret_p = 0;
        cm_get_pcm_buffer(PLAYER_CODEC_INDEX,&ret_p,portMAX_DELAY);    //TODO HSL
        volatile int16_t * out_p = (void*)ret_p;

        if(out_p)
        {
            memcpy(out_p, send_buf_addr, ALG_FRAME_SIZE*2*2);    //播放数据来源于send_buf_addr
            cm_write_codec(PLAYER_CODEC_INDEX, out_p,portMAX_DELAY);
        }
    }
}
```

## 6、API 参考