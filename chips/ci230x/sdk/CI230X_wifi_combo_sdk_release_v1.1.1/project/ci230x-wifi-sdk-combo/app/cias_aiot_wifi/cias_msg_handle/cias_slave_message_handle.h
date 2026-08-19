/**
 * @file    cias_slave_message_handle.h
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_DATA_MESSAGE_DEAL_H_
#define _CIAS_DATA_MESSAGE_DEAL_H_

#include "cias_common.h"
#ifdef __cplusplus
    extern "C"{
#endif

#define CIAS_STANDARD_MAGIC 0x5a5aa5a5
//#define CIAS_SEND_MSG_BUF_LEN 1042 + 16
#define CIAS_SEND_MSG_BUF_LEN 1024 + 16

#define CIAS_PCM_UPLOAD_QUEUE_SIZE 10   //52->10

#pragma pack(1)
typedef struct
{
    unsigned int magic; /* Start data for frame. Let's define as 0x5a5aa5a5; */
    unsigned short checksum; /* checksum */
    unsigned short type; /* Define command type. */
    unsigned short len; /* Define data stream len. */
    unsigned short version; /*版本信息*/
    unsigned int fill_data; /*填充数据*/
}cias_standard_head_t;
#pragma pack()
#pragma pack(1)
typedef struct
{
    unsigned char data[1024];
    unsigned int length;
    unsigned short type;  
}cias_raw_speech_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    unsigned char data[CIAS_SEND_MSG_BUF_LEN];
    //unsigned char  *data;
    unsigned short length;
    unsigned short type;   
    unsigned int ack_flag;  /*需要ACK*/
}cias_send_msg_t;
#pragma pack()

typedef enum 
{
    DEF_FILL = 0x12345678,
    SPEAK_FILL = 0x12345677,  
    INVAILD_SPEAK_FILL = 0x12345666,
    MUSIC_START_FILL = 0x12345699, 
    MP3_FILL = 0x12345688,
    M4A_FILL = 0x123456aa,
    WAV_FILL = 0x123456bb,
    REPEAT_FILL = 0x123456ab,
}cias_fill_type_t;


typedef enum
{
    MSG_FIND_HEAD = 0,
    MSG_RECV_MSG = 1,
    MSG_VERIFY = 2,
}slave_recv_state_t;

typedef struct
{
    unsigned int wifi_msg_index;
}sdio_msg_t;  

#define AUDIO_MUSIC_BUFF_COUNT    (30 -1)//根据audio 缓存（IOT_AUDIO_PLAY_BUF_SIZE）
#define AUDIO_MUSIC_BUFF_LEN    (AUDIO_MUSIC_BUFF_COUNT*1024)

extern volatile bool audio_player_need_data_1L ;
extern volatile bool audio_player_need_data_2L ;
extern volatile bool audio_player_need_data_3L ;


#define TEST_PLAYER                        0//播报原始数据测试

typedef enum
{
    LOCAL_ASR_RESULT_NOTIFY       =  0x0806,  /*收到本地处理命令，需要跳过连接。 取消语音传输命令。*/
    MP3_BUFF_START_RECV1L         =  0x1815,
    MP3_BUFF_START_RECV2L         =  0x2815,
    MP3_BUFF_START_RECV3L         =  0x3815,
    SEND_PLAY_DATA_CMD            =  0x0815,  /*need ack = 0x0815*/
    SEND_START_PLAY_CMD           =  0x0817,
    SEND_TTS_DATA_END_CMD         =  0x0818,
    SEND_PAUSE_PLAY_CMD           =  0x0819,
    SEND_RESUME_PLAY_CMD          =  0x081a,
    SEND_STOP_MUSIC_CMD           =  0x081c,
    RECV_PLAY_END_CMD             =  0x081d,  /*收到播放结束命令*/
    RECV_PLAY_TTS_END_CMD         =  0x081e,  /*收到播放tts结束命令*/
    RECV_RESUME_PLAY_MP3_SIZE_CMD =  0x081f,  /*播放MP3 size */
    RECV_WAKE_UP                  =  0x0823,  
    SEND_PCM_IDLE                 =  0x0888,  /*发送PCM数据空闲 */
    SEND_PCM_START                =  0x0824,  /*发送PCM数据起始包*/
    SEND_PCM_MIDDLE               =  0x0825,  /*发送PCM数据中间包*/
    SEND_PCM_FINISH               =  0x0826,  /*发送PCM数据结束包*/
    SEND_VAD_END                  =  0x0827,  /*发送云端VAD END*/
    SKIP_INVAILD_SPEAK            =  0x0828,  /*跳过无效语音*/
    CONNECT_URL_CMD               =  0x1800,  /* 建立url连接命令*/
    STOP_MUSIC_CMD                =  0x1801,  /* 停止music命令  下一首，上一首，新的音乐*/
    CI1006_OTA_SUCESS             =  0x1802,  /* CI1006升级完成*/
    WIFI_OTA_SUCESS               =  0x1803,  /* MTK升级完成*/
    SEND_MUSIC_DATA_END           =  0x1804,  /*播放MP3时，云端数据已经完成*/
    RECV_DO_NOT_UPDATE            =  0x1805,  
    RECV_START_BLUE_MUSIC_CMD     =  0x082e,  /*收到蓝牙开始播放cmd*/
    RECV_STOP_BLUE_MUSIC_CMD      =  0x082f,  /*收到蓝牙播放结束cmd*/
    SEND_CONTROL_CMD              =  0x0830,
    SEND_MULTIPLE_DIGLOGE         =  0x0831,  /* 发送多轮*/
    RECV_RESUME_PLAY_CMD          =  0x0840,  /* 继续命令(播放网络音乐的时候)*/
    RECV_STOP_MUSIC_CMD           =  0x0841,  /* 停止播放(播放网络音乐的时候)*/
    RECV_NEXT_MUSIC_CMD           =  0x0842,  /* 下一首(播放网络音乐的时候)*/
    RECV_PREV_MUSIC_CMD           =  0x0843,  /* 上一首(播放网络音乐的时候)*/
    RECV_BLUE_MUSIC_BREAK_CMD     =  0x0844,  /* 打断蓝牙音乐*/
    SEND_EMPTY_CMD                =  0xeeee,  
    RECV_PLAY_NET_TTS             =  0x0850,  /*播放网络TTS*/
    RECV_PLAY_LOCAL_TTS           =  0x0851,  /*播放本地TTS*/
    SEND_NET_TIME_OUT_CMD         =  0x0852,  /* 接受云端超时*/
    SEND_ALARM_CMD                =  0x0860,  /* 发送闹钟*/
    SEND_NET_CONNECT_CMD          =  0x0861,  /*网络连接*/
    SEND_NET_DISCONNECT_CMD       =  0x0862,  /*网络断开*/
    ENTER_SMATRCONFIG_CMD         =  0x0863,  /*进入配网模式*/
    EXIT_SMATRCONFIG_CMD          =  0x0864,  /*退出配网模式*/
    INIT_SMATRCONFIG_CMD          =  0x0865,  /*初始密码状态 出厂配置状态*/
    SEND_SMATRCONFIG_SUCESS_CMD   =  0x086a,  /* 配网成功*/
    SEND_WIFI_DISCONNECTED_CMD    =  0x086b,  /*WIFI断开*/
    SEND_WIFI_CONNECTED_CMD       =  0x086c,  /*WIFI连接*/
    RECV_RESTART_WIFI_CMD         =  0x086d,  /*重启WIFI*/
    SEND_NET_CONNECTED_FAILE_CMD  =  0x0876,  /* 网络连接失败(例如密码错误)*/
    SEND_LOCAL_VOLUME_CMD         =  0x0881,  /*发送云端音量*/
    MEDIA_MTK_UPDTAE_PLAY         =  0x0869,  /*发送到CI1006，1006播报升级完成播报音*/
    START_OTA                     =  0x0882,
    UPDTAE_SOFT_ERR               =  0x0883,  /* OTA升级失败，2S后，MTK自动重启。*/
    UPDTAE_SOFT_SUCESS            =  0x0884,  /* OTA升级成功，2S后，MTK自动重启。*/
    DONOT_UPDATE_SOFT             =  0x0885,  /* 已经是最新版本，不需要升级。MTK 不重启*/
    DONOT_CONNECT_SERVER          =  0x0886,  /*不能连接上OTA服务器 MTK 不重启*/

    RECV_RESTART_CI110X_CMD       =  0x0907,

    QCLOUD_IOT_CMD                =  0x0909,
    ENTER_AIRKISS_CMD             =  0x0910,
    ENTER_BLE_CMD                 =  0x0911,
	
    //and by yjd 
    CIAS_OTA_DATA                 =  0x9090,      //cias ota
    CIAS_OTA_START                =  0x9091,      //cias ota
    CIAS_FACTORY_START            =  0x9092,      //生产测试
    CIAS_FACTORY_OK               =  0x9093,      //生产测试成功
    CIAS_FACTORY_FAIL             =  0x9094,      //生产测试失败
    CIAS_FACTORY_SELF_TEST_START  =  0x9095,      //自测试
    CIAS_IR_DATA                  =  0x9096,      //红外数据发送
    CIAS_IR_LOADING_DATA          =  0x9097,      //红外码库下载中
    CIAS_IR_LOAD_DATA_OVER        =  0x9098,      //红外码库下载完成
    CIAS_IR_LOAD_DATA_START       =  0x9099,       //红外下载码库开始
    RCV_NET_CONFIGING_CMD         =  0x0950,        /*配网中*/

}wifi_communicate_cmd_t;


void cias_message_send_interface(unsigned short cmd, cias_fill_type_t type, int len, unsigned char *buf);
void cias_message_init(void);
int cias_slave_message_handle_init(void);
uint16_t get_audio_state(void);
uint16_t set_audio_state(int state);
#ifdef __cplusplus
    }
#endif

#endif
