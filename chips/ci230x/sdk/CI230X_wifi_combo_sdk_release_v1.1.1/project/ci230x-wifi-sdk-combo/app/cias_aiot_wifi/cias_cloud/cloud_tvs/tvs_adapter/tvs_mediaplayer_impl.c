#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tvs_api.h"
#include "tvs_mediaplayer_impl.h"
#include "tvs_api_config.h"
#include "tvs_api_config.h"

#include "cias_media.h"
#include "cias_slave_message_handle.h"
#include "cias_ring_buffer.h"
#include "cias_freertos_debug.h"

#define AUDIO_PLAYER_DBG 1

ci_ring_buffer_t   g_tts_ring_buf = {0};
int g_total_tts_size = 0;
bool g_tts_play_start = false;


//#define test_1  1
//#define test_2  1

void tvs_init_tts_ring_buf(void);
void tvs_send_data(int len);

extern cias_media_info_t g_cias_media;
extern unsigned int tts_state;
extern int g_session_id ;
extern int callback_session_id ;
extern bool is_set_volume ;

#define TVS_LOG(flags, fmt, arg...)             \
    do                                      \
    {                                       \
        if (flags)                          \
            TVS_ADAPTER_PRINTF(fmt, ##arg); \
    } while (0)

#define AUDIO_PLAYER_DEBUG(fmt, arg...) \
    TVS_LOG(AUDIO_PLAYER_DBG, "[tvs_audio] " fmt, ##arg)

typedef enum
{
    PLAYER_URL_PLAYING = 4001,
    PLAYER_URL_PAUSED,
    PLAYER_URL_FINISHED,
    PLAYER_URL_STOPPED,
    PLAYER_URL_SEEKING,
    PLAYER_URL_ERROR,
    PLAYER_URL_UNSUPPORTED_FORMAT,
    PLAYER_URL_RESUME,
} PLAYER_STATUS_E;

// 网络流媒体Url
static char *g_player_url = NULL;
// 网络流媒体唯一标识
static char *g_media_player_token = NULL;
// 下发的seek进度
static uint32_t g_player_offset_sec = 0;

static int tvs_api_impl_media_player_init()
{
    return 0;
}

static void do_release_music()
{
	if (g_player_url != NULL)
    {
		cias_free(g_player_url);
		g_player_url = NULL;
	}

	if (g_media_player_token != NULL)
    {
		cias_free(g_media_player_token);
		g_media_player_token = NULL;
	}
}

static int mediaplayer_adapter_set_source(const char* url, const char* token, uint32_t offset_sec) 
{
	if (NULL == url || NULL == token)
    {
		return -1;
	}
	// 后台下发了新的媒体
	do_release_music();

	g_player_url = strdup(url);
	g_media_player_token = strdup(token);
	g_player_offset_sec = offset_sec;

	AUDIO_PLAYER_DEBUG("set new music %s, token %s, seek %d seconds\n", g_player_url == NULL ? "null" : g_player_url, 
		g_media_player_token == NULL ? "null" : g_media_player_token, g_player_offset_sec);

	return 0;
}

static int mediaplayer_adapter_start_play(const char* token) 
{
	if (g_player_url == NULL)
    {
		return 0;
	}
	// 开始播放后台下发的媒体
	AUDIO_PLAYER_DEBUG("start play music %s, token %s, seek %d seconds, skip speak = %d\n", g_player_url == NULL ? "null" : g_player_url,
		g_media_player_token == NULL ? "null" : g_media_player_token, g_player_offset_sec, cias_media_get_skip_speak());
    /* if(cias_media_get_skip_speak())
    {
        g_cias_media.suspension = true;
        if(cias_media_wait_ack_cmd(20, 900))
        {
            AUDIO_PLAYER_DEBUG("skip play timeout\n");
            return 0;
        }
        else 
        {
            AUDIO_PLAYER_DEBUG("quit play\n");
        }
        
         if(cias_media_is_pcm_start(MEDIA_PCM_START))
        {
            return 0;//add by whc return -> return 0
        } 
    } */

	// TO-DO 调用播放器播放流媒体
	if(g_tts_play_start == false)
    {
        g_cias_media.speak.state = MEDIA_SPEAK_STOP;
        if( (strncmp(g_player_url, g_cias_media.onemp3, strlen(g_player_url)) != 0))
        {
            g_cias_media.audio.state = MEDIA_AUDIO_STOP;
        }
        else 
        {
            cias_media_set_audio_state(MEDIA_AUDIO_RESUME);
        }

        cias_media_add_audio_url(g_player_url);
        cias_media_recv_cmd(CIAS_CONNECT_URL_CMD);
    }

	return 0;
}

static int mediaplayer_adapter_stop_play(const char* token) 
{
	if (g_player_url == NULL) 
    {
		return 0;
	}

	// 后台下发了结束播放命令
	AUDIO_PLAYER_DEBUG("stop play music %s, token %s\n", g_player_url == NULL ? "null" : g_player_url,
		g_media_player_token == NULL ? "null" : g_media_player_token);

	// TO-DO 调用播放器停止播放流媒体
	cias_media_stop_music();
	return 0;
}



static int mediaplayer_adapter_resume_play(const char* token) 
{
	if (g_player_url == NULL) 
    {
		return 0;
	}
	AUDIO_PLAYER_DEBUG("resume play music %s, token %s g_cias_media.speak.state = %d, cias_media_get_skip_speak() = %d\n", g_player_url == NULL ? "null" : g_player_url,
		g_media_player_token == NULL ? "null" : g_media_player_token, g_cias_media.speak.state, cias_media_get_skip_speak());

    if(cias_media_get_skip_speak())
    {
        g_cias_media.suspension = true;
        if(cias_media_wait_ack_cmd(21, 900))
        {
            AUDIO_PLAYER_DEBUG("skip resume timeout\n");
            return 0;
        }
        else 
        {
            AUDIO_PLAYER_DEBUG("quit resume play");

        }

/*         if(cias_media_is_pcm_start(MEDIA_PCM_START))
        {
            return 0; //add by whc return -> return 0
        } */
    }

    if(MEDIA_SPEAK_CANCLE == g_cias_media.speak.state )
    {
        return 0; //add by whc return -> return 0
    }
    cias_media_quit_connect();
    cias_media_set_audio_state(MEDIA_AUDIO_RESUME);
    cias_media_add_audio_url(g_player_url);
    cias_media_tencent_resume_music();
	// TO-DO 调用播放器继续播放流媒体
	return 0;
}

static int mediaplayer_adapter_pause_play(const char* token) 
{
	if (g_player_url == NULL)
    {
		return 0;
	}
	AUDIO_PLAYER_DEBUG("pause play music %s, token %s, g_cias_media.audio.state = %d\n", g_player_url == NULL ? "null" : g_player_url, 
		g_media_player_token == NULL ? "null" : g_media_player_token, g_cias_media.audio.state);


    cias_media_set_audio_state(MEDIA_AUDIO_PAUSE);
    cias_media_clear_cmd_queue();
    CIAS_PRINT_WARN("cias_media_quit_connect 2 !********************\n");
    cias_media_quit_connect();
    cias_media_clear_suspension(); 
	// TO-DO 调用播放器短时间暂停播放流媒体
	
	return 0;
}

bool mediaplayer_adapter_is_player_valid()
{
    return true;
}

char *mediaplayer_adapter_get_url_token()
{
    return g_media_player_token;
}

static int mediaplayer_adapter_get_offset(const char *token)
{
    // TO-DO 获取网络流媒体的播放进度

    return 0;
}

void tvs_init_tts_ring_buf(void)
{
    if(NULL == g_tts_ring_buf.base_addr)
    {   
        ci_ring_buffer_init(&g_tts_ring_buf, 2500);
        if(NULL == g_tts_ring_buf.base_addr)
        {
            AUDIO_PLAYER_DEBUG("malloc ota ring buf fail");
        }
    }
    else 
    {
    ci_ring_buffer_clear(&g_tts_ring_buf);
    }
}

void tvs_send_data(int len)
{
    if(ci_ring_buffer_get_size(&g_tts_ring_buf) > 0)
    {
        char buf[1024] = {0};
        int read_len = ci_ring_buffer_read(&g_tts_ring_buf, buf, len);
        cias_message_send_interface(SEND_PLAY_DATA_CMD,DEF_FILL,read_len,buf);
    }
}

void tvs_callback_on_asr(const char* asr_text, bool is_end)
{
    cias_media_set_pcm_state(MEDIA_PCM_INIT);
    if(is_end)
    {
        printf("asr_text = %s\r\n", asr_text);
        g_cias_media.speak.asr_is_valid = true;
        tvs_init_tts_ring_buf();
        cias_message_send_interface(SEND_START_PLAY_CMD,SPEAK_FILL,0,NULL);
        if(cias_media_wait_ack_cmd(40,100))
        {
            AUDIO_PLAYER_DEBUG("tts wait cmd timeout\n");
        }
        if(memcmp(asr_text, "继续播放", 8) == 0)      //add by roy
        {
            printf("********************\r\n");
            asr_set_resume_play_status();
            tvs_media_player_inner_start_play();
        }
    }
}

void tvs_event_callback_on_asr(const char* asr_text)
{
    char *p_text = NULL;
}

bool test_print_flag = false;
int on_play_tts_start(int type) 
{
	AUDIO_PLAYER_DEBUG("*******tts start play mp3, g_session_id = %d, callback_session_id=%d********\n", g_session_id, callback_session_id);

    if(is_set_volume)
    {
        // cias_media_wait_ack_cmd(30,100); //add by whc
        is_set_volume = false;
    }

    if(g_session_id != callback_session_id)
    {
        return 0;
    }
    
    if(cias_media_get_skip_speak())
    {
        return 0;
    }
    
    if(g_cias_media.audio.state == MEDIA_AUDIO_PLAY)
    {
        return 0 ;
    }

    tvs_init_tts_ring_buf();
	test_print_flag = false;
	g_total_tts_size = 0;
    audio_player_need_data_1L = false;
    audio_player_need_data_2L = false;
    audio_player_need_data_3L = false;
	g_tts_play_start = true;
    g_cias_media.suspension = true;
    g_cias_media.speak.state = MEDIA_SPEAK_PLAY;
    if(!g_cias_media.speak.asr_is_valid)
    {
        AUDIO_PLAYER_DEBUG(" tts  SEND_START_PLAY_CMD  !!!!!!!!!!!!!!\r\n");
        cias_message_send_interface(SEND_START_PLAY_CMD,SPEAK_FILL,0,NULL);
        // if(cias_media_wait_ack_cmd(40,100))
        // {
        //     AUDIO_PLAYER_DEBUG("tts wait cmd timeout\n");
        // }
        //add by whc
    }

	return 0;
}

void on_play_tts_stop(bool force_stop) 
{
    AUDIO_PLAYER_DEBUG("*******tts end, 2total %d bytes  force_stop = %d g_session_id=%d callback_session_id=%d********\n", g_total_tts_size,  force_stop, g_session_id, callback_session_id);

    tvs_send_data(1024);
    if(g_session_id != callback_session_id)
    {
        g_tts_play_start = false;
        return;
    }
    
    if(cias_media_is_pcm_start(MEDIA_PCM_START))
    {
        return;
    }

    if(cias_media_get_skip_speak())
    {
        cias_media_set_skip_speak(false);
        return;
    }
    

    if(g_cias_media.audio.state == MEDIA_AUDIO_PLAY)
    {
        return ;
    }
    AUDIO_PLAYER_DEBUG("g_tts_play_start=%d,force_stop=%d\n", g_tts_play_start,force_stop);
    if(g_tts_play_start &&  !force_stop)
    {
    	cias_media_finish_tts();
    	g_tts_play_start = false;
        g_cias_media.play_tts_end = true;
        g_cias_media.speak.state = MEDIA_SPEAK_SEND_END;
        if(cias_media_wait_play_tts_end_cmd(50, 1000))
        {
            AUDIO_PLAYER_DEBUG("play tts end timeout\n");
        }
    }
    else if (force_stop)
    {
        g_tts_play_start = false;
        g_cias_media.speak.state = MEDIA_SPEAK_STOP;
    }
}
/**
 * @brief on_play_tts_recv_data 接收tts数据
 * 
 * @param data 
 * @param data_len 
 * @return int 
 */
int on_play_tts_recv_data(char* data, int data_len)
{
    if(g_session_id != callback_session_id)
    {
        AUDIO_PLAYER_DEBUG("g_session_id = %d, callback_session_id = %d\n",g_session_id,callback_session_id);
        return 0;
    }

    if(g_total_tts_size > 4 * 1024 && (!test_print_flag))
    {
        test_print_flag = true;
        AUDIO_PLAYER_DEBUG("*******tts playing, recv %d bytes, total %d bytes********\n", data_len, g_total_tts_size);
    }
    if(cias_media_get_skip_speak())
    {
        return;
    }

    if(g_cias_media.audio.state == MEDIA_AUDIO_PLAY)
    {
        return 0 ;
    }

    if (!g_tts_play_start)
    {
        return -1;
    }
    //AUDIO_PLAYER_DEBUG("g_total_tts_size=%d\n", g_total_tts_size);
    // if(g_total_tts_size >= 254 * 1024 && (g_cias_media.speak.state == MEDIA_SPEAK_PLAY))
    if(g_total_tts_size >= AUDIO_MUSIC_BUFF_LEN && (g_cias_media.speak.state == MEDIA_SPEAK_PLAY)) //add by whc
    {
        g_cias_media.suspension = true;
        audio_player_need_data_1L = false;
        audio_player_need_data_2L = false;
        audio_player_need_data_3L = false;
        cias_media_wait_ack_cmd(10, 500);
        g_total_tts_size = 0;
        
    }
    if(audio_player_need_data_3L)
    {
        audio_player_need_data_3L = false;
        AUDIO_PLAYER_DEBUG("audio_player_need_data_3L\n");            
        g_total_tts_size = AUDIO_MUSIC_BUFF_LEN - 40*1024;//请求40KB audio 数据
    }
    else if(audio_player_need_data_2L)
    {
        audio_player_need_data_2L = false;
        AUDIO_PLAYER_DEBUG("audio_player_need_data_2L\n");            
        g_total_tts_size = AUDIO_MUSIC_BUFF_LEN - 20*1024;//请求20KB audio 数据
    }
    else if(audio_player_need_data_1L)
    {
        audio_player_need_data_1L = false;
        //AUDIO_PLAYER_DEBUG("audio_player_need_data_1L\n");            
        g_total_tts_size = AUDIO_MUSIC_BUFF_LEN - 5*1024;//请求5KB audio 数据
    }
    
            
    
    g_total_tts_size += data_len;

    if(data_len > 0)
    {   
        if(ci_ring_buffer_get_size(&g_tts_ring_buf) > 1024)
        {
            tvs_send_data(1024);
        }
        
        if(ci_ring_buffer_get_size(&g_tts_ring_buf) + data_len <  2500 )
        {
            ci_ring_buffer_write(&g_tts_ring_buf, data, data_len);
        }
        else 
        {
            AUDIO_PLAYER_DEBUG("recv error!!!!!!!!!!!!!!!!");
        }
    }

    

	return 0;
}

bool tvs_tts_adapter_play_inner()
{
    // 设置是否由适配层接管TTS的播放
    return CONFIG_DECODE_TTS_IN_SDK;
}

void tvs_init_mediaplayer_adapter_impl(tvs_mediaplayer_adapter *ad)
{
    if (ad == NULL)
    {
        return;
    }

    // adapter实现
    ad->get_offset = mediaplayer_adapter_get_offset;
    ad->set_source = mediaplayer_adapter_set_source;
    ad->stop_play = mediaplayer_adapter_stop_play;
    ad->start_play = mediaplayer_adapter_start_play;
    ad->pause_play = mediaplayer_adapter_pause_play;
    ad->resume_play = mediaplayer_adapter_resume_play;

    ad->play_inner = tvs_tts_adapter_play_inner;
    ad->tts_start = on_play_tts_start;
    ad->tts_data = on_play_tts_recv_data;
    ad->tts_end = on_play_tts_stop;

    // 初始化媒体播放器
    tvs_api_impl_media_player_init();
}

// TO-DO 监听播放器状态，调用此函数
void notify_player_status_change(const char *url, PLAYER_STATUS_E status)
{
    if (g_player_url == NULL || strlen(g_player_url) == 0 || url == NULL)
    {
        return;
    }

    int len = strlen(url);
    if (len == 0)
    {
        return;
    }
    // printf("1111 %s\r\n",url);
    // printf("2222 %s\r\n",g_player_url);

    len = strlen(g_player_url) > len ? len : strlen(g_player_url);

    if (strncmp(g_player_url, url, len) != 0)
    {
        return;
    }

    printf("%s status = %x\r\n", __func__, status);

    switch (status)
    {
    case PLAYER_URL_FINISHED:
        tvs_mediaplayer_adapter_on_play_finished(g_media_player_token);
        do_release_music();
        break;
    case PLAYER_URL_PLAYING:
        tvs_mediaplayer_adapter_on_play_started(g_media_player_token);
        break;
    case PLAYER_URL_PAUSED:
        tvs_mediaplayer_adapter_on_play_paused(g_media_player_token);
        break;
    case PLAYER_URL_STOPPED:
        tvs_mediaplayer_adapter_on_play_stopped(0, g_media_player_token);
        break;
    case PLAYER_URL_ERROR:
        tvs_mediaplayer_adapter_on_play_stopped(-1, g_media_player_token);
        break;
    default:
        break;
    }
}
