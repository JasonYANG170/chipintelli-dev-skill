/**
 * @file    cias_media.h
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_MEDIA_H
#define _CIAS_MEDIA_H
#include <stdbool.h>
#include "cias_slave_message_handle.h"


#ifdef __cplusplus
    extern "C"{
#endif

#define CIAS_SEND_MEDIA_MSG_QUEUE_SIZE  4
#define CIAS_URL_LIST_MAX_LEN           10
#define UART_SEND_BLOCK_SIZE            256
#define CIAS_MEDIA_SPEAK_URL_LEN        256
#define CIAS_MEDIA_SPEAK_URL_COUNT      10       // 存储SPEAK的URL数量
#if 1
#define MEDIA_DEBUG  CIAS_PRINT_DEBUG
#else
#define MEDIA_DEBUG(flags, ...)  \
    do {                            \
            ;                          \
    } while (0)
#endif

typedef struct cias_audio_url_list_s
{
    unsigned char * url;
    struct cias_audio_url_list_s * next;
}cias_url_list_t;

#define CIAS_CONNECT_URL_CMD        0x1800
#define CIAS_RECV_PLAY_END_CMD      0x1801
#define CIAS_SEND_MUSIC_DATA_END    0x1802

typedef enum 
{
    CIAS_PLAYER_URL_PLAYING = 4001,
    CIAS_PLAYER_URL_PAUSED,
    CIAS_PLAYER_URL_FINISHED,
    CIAS_PLAYER_URL_STOPPED,
    CIAS_PLAYER_URL_SEEKING,
    CIAS_PLAYER_URL_ERROR,
    CIAS_PLAYER_URL_UNSUPPORTED_FORMAT,
    CIAS_PLAYER_URL_RESUME,
}cias_player_status_t;

typedef enum
{
    CIAS_AUDIO_PLAY_STATE_IDLE,     /*!< 播放空闲       */
    CIAS_AUDIO_PLAY_STATE_START,    /*!< 播放开始       */
    CIAS_AUDIO_PLAY_STATE_PLAYING,  /*!< 播放中         */
    CIAS_AUDIO_PLAY_STATE_STOP,     /*!< 播放停止       */
    CIAS_AUDIO_PLAY_STATE_FINISH,   /*!< 播放完成       */
}cias_audio_play_state_t;

typedef enum
{
    CIAS_AUDIO_PLAY_EVENT_NONE          =  (1<<0),    /*!< 无                       */
    CIAS_AUDIO_PLAY_EVENT_SEND_MSG_DONE =  (1<<4),    /*!< 请求音频数据完整发送完毕   */
    CIAS_AUDIO_PLAY_EVENT_PAUSE         =  (1<<5),    /*!< 暂停播放                 */
    CIAS_AUDIO_PLAY_EVENT_PLAYER_READY  =  (1<<6),    /*!< 播放器准备就绪           */
    CIAS_AUDIO_PLAY_EVENT_STOP          =  (1<<7),    /*!< 停止播放                 */
    CIAS_AUDIO_PLAY_EVENT_PLAY_DONE     =  (1<<8),    /*!< 保留                     */
    CIAS_AUDIO_PLAY_EVENT_START         =  (1<<9),    /*!< 启动播放                 */
    CIAS_AUDIO_PLAY_EVENT_PLAY_TIMEOUT  =  (1<<10),   /*!< 超时                     */
    CIAS_AUDIO_PLAY_EVENT_PREV          =  (1<<11),   /*!< 保留                     */
    CIAS_AUDIO_PLAY_EVENT_STOP_SUCCESS  =  (1<<12),
}cias_audio_play_event_t;

typedef enum
{
   INIT_URL_TYPE = 0,
   SPEAK_URL,
   SPEAK_AND_MUSIC_URL,
   FIRST_MUSIC_URL,
   NEXT_MUSIC_URL,
   PREV_MUSIC_URL,
   SPEAK_AND_OTA_URL,
   ONEMUSIC,
}cias_url_type;

typedef enum
{
    MUSIC_TYPE_PCM_DATA,
    MUSIC_TYPE_URL_DATA,
}cias_play_msg_type_t;
typedef struct 
{
    cias_play_msg_type_t                data_type;
    union 
    {
        struct 
        {
            cias_url_type             url_type;
            cias_fill_type_t            fill_type;
            const unsigned char         *audio_url;
            int offset;
        }url_info;
        struct 
        {
            int                             len;
            unsigned short                  cmd;
            unsigned char                   buff[1024];
        }pcm_info;
    };
    
}cias_play_msg_t;

typedef enum
{
    AUDIO_PLAY_CB_STATE_UNKNOWN_ERR               = -99,
    AUDIO_PLAY_CB_STATE_INTERNAL_ERR              = -1,
    AUDIO_PLAY_CB_STATE_DONE                      = 0,
    AUDIO_PLAY_CB_STATE_PAUSE                     = 1,
    AUDIO_PLAY_CB_STATE_STOP                      = 2,
} cias_audio_callback_state_t;

typedef struct audio_url_node_t
{
    unsigned char   *url;
    unsigned short  len;
    struct audio_url_node_t   *next;
    struct audio_url_node_t   *prev;
}audio_url_node;

typedef struct audio_url_head_t
{
    unsigned char   count;
    struct audio_url_node_t *head;
    struct audio_url_node_t *cur_node;
}audio_url_head;

typedef struct 
{
    unsigned short type;
    cias_url_type url_type;
}cias_media_cmd;

typedef enum 
{
    MEDIA_PCM_INIT = 0,
    MEDIA_PCM_START,
    MEDIA_PCM_MIDDLE,
    MEDIA_PCM_FINISH,
}cias_pcm_state;

typedef enum 
{
    HTTP_START = 0,  // 开始，开始连接
    HTTP_MIDDLE,  //传输中
    HTTP_FINISH,   //结束
}cias_http_state;

typedef enum
{
    MEDIA_SPEAK_STOP = 0,
    MEDIA_SPEAK_PLAY,
    MEDIA_SPEAK_SEND_END,
    MEDIA_SPEAK_PLAY_END,
    MEDIA_SPEAK_CANCLE,
}speak_state;

typedef enum
{
    MEDIA_AUDIO_STOP = 0,
    MEDIA_AUDIO_PLAY,
    MEDIA_AUDIO_PAUSE,
    MEDIA_AUDIO_RESUME,
    MEDIA_AUDIO_EXCEPTION_RESUME,
}audio_state;

typedef struct
{
    bool    asr_is_valid;
    speak_state     state;
    //unsigned char *url;
    unsigned char count;        // 记录一次下发的TTS URL总个数
    unsigned char play_count;   // 当前播放的URL下标
    unsigned char  url[CIAS_MEDIA_SPEAK_URL_COUNT][CIAS_MEDIA_SPEAK_URL_LEN];   // 播报音URL
}cias_speak_info;

typedef struct 
{
    audio_state     state;
    int     prev_offset;
    bool    cloud_stop;
    int     skip_prev_offset;
    bool    is_set_offset;
    audio_url_head  *url_head;
}cias_audio_info;

typedef struct 
{
    cias_speak_info     speak;
    cias_audio_info     audio;
    cias_pcm_state      pcm_state;
    cias_url_type       url_type;
    cias_http_state     http_state;
    unsigned char       *onemp3;
    int                 offset;
    int                 block_size;
    bool                stop_notify;
    bool                connect_state;
    unsigned char       skip_speak;
    bool                dialog_type;
    cias_fill_type_t    audio_format;
    bool                play_tts_end;
    bool                is_disconnect_wifi;
    bool                resume_music_offset_err;
    bool                suspension;
    int                 block_init_value;
    cias_http_state     http_type;
}cias_media_info_t;

typedef struct 
{
    cias_url_type             url_type;
    cias_fill_type_t            fill_type;
    unsigned char               *audio_url;
    int                         offset;
}cias_play_msg_backup_t;

typedef struct 
{
    unsigned char         *audio_url;
    int offset;
}cias_http_msg_t;

typedef struct 
{
    unsigned char         *audio_url;
    int offset;
}cias_http_msg_info_t;

typedef enum 
{
    HTTP_STATUS_IDEL,
    HTTP_STATUS_BUSY,
}cias_media_http_status_t;

bool cias_media_get_skip_speak(void);
bool cias_media_wait_ack_cmd(int time, int num);
bool cias_media_is_pcm_start(int state);
void cias_media_stop_music(void);
void cias_media_quit_connect(void);
void cias_media_tencent_resume_music(void);
void cias_media_clear_cmd_queue(void);
void cias_media_clear_suspension(void);
void cias_media_finish_tts(void);
bool cias_media_wait_play_tts_end_cmd(int time, int num);
bool cias_media_is_http_middle(void);
void cias_media_set_block_size(int32_t arg);
void cias_media_set_audio_offset(int len);
void cias_media_pause_music(void);
void cias_media_set_pcm_state(cias_pcm_state state);
void cias_media_set_skip_speak(bool state);
void cias_media_set_pcm_state(cias_pcm_state state);
bool get_http_data_is_complete(void);
void cias_media_set_audio_state(audio_state state);
void cias_media_set_speak_state(speak_state state);
bool cias_media_is_audio_state(audio_state state);
bool cias_media_is_speak_state(speak_state state);
int cias_media_init_interface(void);
void cias_media_block_size_degrade(void);
void cias_media_offset_size_increment(int len);
int cias_media_get_block_size(void);
int cias_media_get_offset_size(void);
void cias_media_tts_url_time_stop(void);
void cias_media_set_url_type(cias_url_type url_type);
bool cias_media_is_url_type(cias_url_type url_type);
void cias_media_add_speak_url_array(const unsigned char *url);
void cias_media_clear_speak_url(void);
void print_meida_state(char *str);

#ifdef __cplusplus
    }
#endif

#endif