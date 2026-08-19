/**
 * @file    cias_media.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */
#include <stdbool.h>
#include "cias_freertos_common.h"
#include "cias_freertos_task.h"
#include "cias_freertos_mutex.h"
#include "cias_freertos_queue.h"
#include "cias_freertos_debug.h"
#include "cias_freertos_event_group.h"
#include "cias_freertos_timer.h"


#include "cias_media.h"
#include "cias_http_client.h"
#include "cias_http_client_ops.h"

#define FIRST_DATA_SIZE 2048

cias_task_t cias_media_task_handler;
cias_media_info_t g_cias_media; 
cias_queue_t *cias_media_cmd_queue = NULL;

http_client_c *g_httpclient = NULL;
extern cias_queue_t cias_send_message_queue;
bool ci_wifi_connect = false;

typedef enum {
    PLAYER_URL_PLAYING = 4001,
    PLAYER_URL_PAUSED,
    PLAYER_URL_FINISHED,
    PLAYER_URL_STOPPED,
    PLAYER_URL_SEEKING,
    PLAYER_URL_ERROR,
    PLAYER_URL_UNSUPPORTED_FORMAT,
    PLAYER_URL_RESUME,
} PLAYER_STATUS_E;

void print_meida_state(char *str)
{
    #if 1
    CIAS_PRINT_DEBUG("[dlp][%s]url_type = %d,stop_notify = %d,audio.state = %d,speak.state = %d,recv_size = %d,n_http_content_len = %d,prev_offset = %d,offset = %d\n", str,
    g_cias_media.url_type,g_cias_media.stop_notify,g_cias_media.audio.state,
    g_cias_media.speak.state,g_httpclient->recv_size,g_httpclient->n_http_content_len,
    g_cias_media.audio.prev_offset,
    g_cias_media.offset);
    #endif
}

void cias_media_recv_cmd(unsigned short type)
{
    cias_media_cmd cmd_msg = {0};
    cmd_msg.type = type;
    cias_queue_send(&cias_media_cmd_queue,&cmd_msg,pdMS_TO_TICKS(10));
}

void cias_media_del_music_url(void)
{
    if(g_cias_media.audio.url_head->head != NULL)
    {
        audio_url_node *node = g_cias_media.audio.url_head->head;
        audio_url_node *tmp_node = NULL;
        if((node->next == node->prev) && (node == node->next))
        {
            if(node->url != NULL)
            {
                cias_free(node->url);
                // node->url = NULL;
            }
            cias_free(node);
            // node = NULL;
        }
        else
        {
            node->prev->next = NULL;
            for( ; node != NULL; node = tmp_node)
            {
                tmp_node = node->next;
                if(node->url != NULL)
                {
                    cias_free(node->url);
                    node->url = NULL;
                }
                cias_free(node);
                node = NULL;
            }
        }
    }
    cias_free(g_cias_media.audio.url_head);
    g_cias_media.audio.url_head = NULL;
}

void cias_media_add_audio_url(char * linkurl)
{
    unsigned short url_len;
    if(g_cias_media.url_type == SPEAK_URL || g_cias_media.url_type == SPEAK_AND_MUSIC_URL)
    {
        g_cias_media.url_type = SPEAK_AND_MUSIC_URL;
    }
    else
    {
        g_cias_media.url_type = ONEMUSIC;
    }
    url_len = strlen(linkurl);
    if(g_cias_media.onemp3 != NULL)
    {
        cias_free(g_cias_media.onemp3);
        g_cias_media.onemp3 = NULL;
    }
    g_cias_media.onemp3 = cias_malloc(url_len + 1);
    if(g_cias_media.onemp3 == NULL)
    {
        CIAS_PRINT_ERR("alloc url failed\n");
        return;
    }
    g_cias_media.onemp3[url_len] = 0x0;
    memcpy(g_cias_media.onemp3, linkurl, url_len);
    CIAS_PRINT_DEBUG("cias_media_add_audio_url OK!, type = %d\r\n",g_cias_media.url_type);
}

bool cias_media_is_url_type(cias_url_type url_type)
{
    return g_cias_media.url_type == url_type ? true : false;
}

void cias_media_set_url_type(cias_url_type url_type)
{
    // if(g_cias_media.url_type == ONEMUSIC)
    // {
    //     g_cias_media.url_type = SPEAK_AND_MUSIC_URL;
    // }
    // else
    {
        g_cias_media.url_type = url_type;
    }
}

void cias_media_add_speak_url_array(const unsigned char *url)
{
    unsigned short url_len = os_strlen(url);
    if(NULL == url || (url_len > CIAS_MEDIA_SPEAK_URL_LEN))
    {
        CIAS_PRINT_ERR("speak url error  url_len = %d\n", url_len);
        return;
    }
    memcpy(g_cias_media.speak.url[( g_cias_media.speak.count ++) % CIAS_MEDIA_SPEAK_URL_COUNT],url,url_len +1);
}

void cias_media_clear_speak_url(void)
{
    g_cias_media.speak.count = 0;
    g_cias_media.speak.play_count = 0;
}

void cias_media_finish_tts_recv(void)
{
    CIAS_PRINT_INFO("g_cias_media.speak.state = %d, g_cias_media.audio.state = %d\n",  g_cias_media.speak.state, g_cias_media.audio.state);
    g_cias_media.speak.state = MEDIA_SPEAK_SEND_END;
    cias_message_send_interface(SEND_TTS_DATA_END_CMD, DEF_FILL, 0, NULL);
    cias_message_send_interface(SEND_EMPTY_CMD, DEF_FILL, 0, NULL);
}

void cias_media_set_pcm_state(cias_pcm_state state)
{
    g_cias_media.pcm_state = state;
}

bool get_http_data_is_complete(void)
{
    if((g_httpclient) && (g_httpclient->recv_size == g_httpclient->n_http_content_len))
    {
        return true;
    }
    return false;
}

bool cias_media_is_audio_state(audio_state state)
{
    return g_cias_media.audio.state == state ? true: false;
}

bool cias_media_is_speak_state(speak_state state)
{
    return g_cias_media.speak.state == state ? true:false;
}

void cias_set_audio_format(char *url)
{
    if(NULL == url)
    {
        g_cias_media.audio_format = DEF_FILL;
    }
    else if(strstr(url,".m4a") != NULL)
    {
        g_cias_media.audio_format = M4A_FILL;
    }
    else if(strstr(url,".mp3") != NULL)
    {
        g_cias_media.audio_format = MP3_FILL;
    }
    else if(strstr(url,".wav") != NULL)
    {
        g_cias_media.audio_format = WAV_FILL;
    }
}

bool cias_media_is_http_middle(void)
{
    if(HTTP_MIDDLE == g_cias_media.http_type)
    {
        return true;
    }
    return false;
}

void cias_media_clear_cmd_queue(void)
{
    cias_queue_reset(&cias_media_cmd_queue);
}

unsigned char *cias_media_first_music(void)
{
    return g_cias_media.audio.url_head->head->url;
}

unsigned char *cias_media_next_music(void)
{
    g_cias_media.audio.url_head->head = g_cias_media.audio.url_head->head->next;
    return cias_media_first_music();
}

unsigned char *cias_media_prev_music(void)
{
    g_cias_media.audio.url_head->head = g_cias_media.audio.url_head->head->prev;
    return cias_media_first_music();
}

void cias_media_tencent_resume_music(void)
{
    CIAS_PRINT_INFO("g_cias_media.audio.state = %d\n", g_cias_media.audio.state);
    //if(g_cias_media.audio.state == MEDIA_AUDIO_RESUME || g_cias_media.audio.state == MEDIA_AUDIO_PAUSE)
    {
        //g_cias_media.audio.state = MEDIA_AUDIO_RESUME;
        g_cias_media.url_type = ONEMUSIC;
        cias_media_recv_cmd(CIAS_CONNECT_URL_CMD);
    }
}

bool cias_media_block_size_is_zero()
{
    if(g_cias_media.block_size <= 0)
    {  
        g_cias_media.suspension = true;
        return true;
    }
    return false;
}

void cias_media_set_block_size(int32_t arg)
{
    if(arg == 0)
        g_cias_media.block_size = AUDIO_MUSIC_BUFF_COUNT;
    else
        g_cias_media.block_size = arg;
    g_cias_media.block_init_value = g_cias_media.offset;
}
       

void cias_media_quit_connect(void)
{
    g_cias_media.stop_notify = true;
}

void cias_media_clear_suspension(void)
{
    g_cias_media.suspension = false;
}

void cias_media_pause_music(void)
{   
    if (MEDIA_AUDIO_PLAY == g_cias_media.audio.state)
    {
        g_cias_media.audio.state = MEDIA_AUDIO_PAUSE;
    }
}

bool cias_media_is_pcm_start(int state)
{
    if(g_cias_media.pcm_state == state)
    {
        return true;
    }
    return false;
}

void cias_media_stop_music(void)
{
    if(MEDIA_AUDIO_PAUSE == g_cias_media.audio.state)
    {
        // 发送停止命令
        cias_message_send_interface(SEND_STOP_MUSIC_CMD, DEF_FILL, 0, NULL);
    }
}

bool cias_media_wait_ack_cmd(int time, int num)
{
    //CIAS_PRINT_INFO(" time = [%d] num = [%d]  wait !!!\n",time,num);
    while(num && g_cias_media.suspension)
    {   
        cias_delay(time);
        num--;
    }
    return g_cias_media.suspension;
}

void cias_media_finish_music(void)
{
    cias_media_recv_cmd(CIAS_SEND_MUSIC_DATA_END);
    g_cias_media.audio.state = MEDIA_AUDIO_STOP;
    if(g_cias_media.onemp3){
        tvs_media_player_inner_stop_play();
        free(g_cias_media.onemp3);
        g_cias_media.onemp3 = NULL;
        CIAS_PRINT_INFO("free g_cias_media.onemp3 == NULL\n");
    }
}

void cias_finish_play_music(void)
{
    g_cias_media.audio.prev_offset = 0;
    cias_message_send_interface(SEND_TTS_DATA_END_CMD, DEF_FILL, 0, NULL);
    cias_message_send_interface(SEND_EMPTY_CMD, DEF_FILL, 0, NULL);
}

void cias_media_finish_tts(void)
{
    cias_message_send_interface(SEND_TTS_DATA_END_CMD, DEF_FILL, 0, NULL);
    cias_message_send_interface(SEND_EMPTY_CMD, DEF_FILL , 0, NULL);
}

void cias_media_set_audio_offset(int len)
{
    g_cias_media.audio.prev_offset = len;
    g_cias_media.audio.skip_prev_offset = len;
    g_cias_media.audio.is_set_offset = true;
    if(g_cias_media.audio.state == MEDIA_AUDIO_PAUSE)
    {
        //g_cias_media.audio.state = MEDIA_AUDIO_RESUME;
    }
}

bool cias_media_wait_play_tts_end_cmd(int time, int num)
{
    //CIAS_PRINT_INFO(" time = [%d]  num = <%d> wait  !!!\n", time,num);
    while(num && g_cias_media.play_tts_end)
    {   
/*         if(time > 400)
        {
            CIAS_PRINT_INFO(" num = [%d]  wait !!!\n", num);
        } */
        cias_delay(time);
        num--;
    }
    return g_cias_media.play_tts_end;
}

static cias_timer_t cias_media_tts_url_time_handle = {0};
void cias_media_tts_url_time_func(void)

{

  cias_media_recv_cmd(CONNECT_URL_CMD);

  CIAS_PRINT_INFO("cias_media_tts_url_time_func \n");

}

void cias_media_tts_url_time_start(void)
{
	cias_media_tts_url_time_handle.timer_name = "timer_test_task";
    
	if (cias_media_tts_url_time_handle.handle == NULL)
	{
		cias_timer_create(&cias_media_tts_url_time_handle,1,cias_media_tts_url_time_func, NULL,15000);		
	}
	cias_timer_start(&cias_media_tts_url_time_handle,0);
	CIAS_PRINT_INFO("Start tts url timer ...\n");
}
void cias_media_tts_url_time_stop(void)
{
	if (cias_media_tts_url_time_handle.handle != NULL)
	{
		cias_timer_stop(&cias_media_tts_url_time_handle,0);
		CIAS_PRINT_INFO("Stop tts url timer ...\n");
	}
}
void cias_media_connect_url(void)
{
    unsigned char *url = NULL;
    int offset = 0;
    switch(g_cias_media.url_type)
    {
        case INIT_URL_TYPE:
        {
            return;
        }
        case SPEAK_URL:
        case SPEAK_AND_MUSIC_URL:
        {
            if(g_cias_media.speak.play_count < g_cias_media.speak.count)
            {
                url = g_cias_media.speak.url[g_cias_media.speak.play_count++];
            }else
            {
                CIAS_PRINT_ERR("g_cias_media.speak.url == NULL\n");
                url = NULL;
                return ;
            }
            break;
        }
        case ONEMUSIC:
        {
            url = g_cias_media.onemp3;
            if(url == NULL)
            {
                CIAS_PRINT_ERR("ONEMP3 URL == NULL\n");
                return;
            }
            cias_set_audio_format(url);
            if((MEDIA_AUDIO_RESUME == g_cias_media.audio.state) || 
                (MEDIA_AUDIO_EXCEPTION_RESUME == g_cias_media.audio.state))
            {
                if(g_cias_media.is_disconnect_wifi)
                {
                    offset = g_cias_media.offset;
                }
                else
                {
                    offset = g_cias_media.audio.prev_offset;
                }
                if(offset <= 0)
                {
                    g_cias_media.audio.state = MEDIA_AUDIO_STOP;
                    offset = 0;
                }
            }
            break;
        }
        case FIRST_MUSIC_URL:
        {
            url = cias_media_first_music();
            if(MEDIA_AUDIO_RESUME == g_cias_media.audio.state)
            {
                offset = g_cias_media.audio.prev_offset;
            }
            break;
        }
        case NEXT_MUSIC_URL:
        {
            url = cias_media_next_music();
            break;
        }
        case PREV_MUSIC_URL:
        {
            url = cias_media_prev_music();
            break;
        }
        default:
        {
            break;
        }
    }

    CIAS_PRINT_WARN("cias_media_connect_url  stop_notify is false !********************\n");
    g_cias_media.offset = offset;
    g_cias_media.stop_notify = false;
    g_cias_media.audio.prev_offset = -1;
    g_cias_media.audio.is_set_offset = false;

    if(g_cias_media.url_type != SPEAK_URL)
    {
        cias_message_send_queue_clear();
    }
    
    print_meida_state("connect url:<1>\n");

    cias_http_client_get(g_httpclient, url, g_cias_media.offset, false);
    cias_delay(5);
    
    print_meida_state("connect url:<2>\n");
    if((g_httpclient->p_location != NULL) && (g_httpclient->redirections_too_more))
    {
        cias_media_add_audio_url(g_httpclient->p_location);
        cias_media_recv_cmd(CIAS_CONNECT_URL_CMD);
        CIAS_PRINT_INFO("redirection reconnect\n");
        return;
    }
    g_cias_media.is_disconnect_wifi = false;
    if(g_cias_media.speak.state == MEDIA_SPEAK_PLAY)
    {
        if(g_cias_media.speak.play_count == g_cias_media.speak.count && g_httpclient->recv_size == g_httpclient->n_http_content_len)
        {
            /* 所以URL接收完成 */
            cias_media_finish_tts_recv();
        }
        else if (g_httpclient->recv_size == g_httpclient->n_http_content_len)
        {
            /* 该TTS URL接收完成，进行下一个TTS URL 的连接 */
            g_cias_media.url_type = SPEAK_URL;
            if((g_cias_media.speak.play_count < g_cias_media.speak.count))
            {
                cias_media_tts_url_time_start();
            }
            else
            {

            }
            //cias_media_recv_cmd(CONNECT_URL_CMD);
        }
    }
    else if(false == g_cias_media.stop_notify)
    {
        if(MEDIA_AUDIO_PLAY == g_cias_media.audio.state)
        {
            if(g_httpclient->recv_size != g_httpclient->n_http_content_len)
            {
                if((g_cias_media.offset != 0) && (g_cias_media.offset != g_cias_media.audio.prev_offset))
                {
                    CIAS_PRINT_INFO("renew prev_offset = %d\n",g_cias_media.offset);
                    print_meida_state("connect url:<3>\n");
                    cias_message_send_queue_clear();
                    cias_media_clear_cmd_queue();
                    if(g_cias_media.offset > FIRST_DATA_SIZE)  // 2020.10.19 test
                    {
                        g_cias_media.offset -= FIRST_DATA_SIZE;
                    }
                    g_cias_media.audio.prev_offset = g_cias_media.offset;
                    g_cias_media.audio.state = MEDIA_AUDIO_EXCEPTION_RESUME;
                    g_cias_media.url_type = ONEMUSIC;
                    cias_media_recv_cmd(CIAS_CONNECT_URL_CMD);
                    return;
                }
            }
            else if(g_httpclient->recv_size == g_httpclient->n_http_content_len)
            {
                cias_media_finish_music();
                g_cias_media.url_type = INIT_URL_TYPE;
            }
        }
        if((g_cias_media.offset == 0) || (g_httpclient->n_http_content_len == 0))
        {
        }
    }
    else if(g_cias_media.stop_notify && (MEDIA_AUDIO_RESUME == g_cias_media.audio.state) && (g_cias_media.offset == 0))
    {
        g_cias_media.audio.state = MEDIA_AUDIO_STOP;
    }
    else if(g_cias_media.stop_notify && (!ci_wifi_connect) && (g_cias_media.audio.state == MEDIA_AUDIO_PAUSE))
    {
        g_cias_media.is_disconnect_wifi = true;
    }
    else if(g_cias_media.stop_notify && (g_cias_media.audio.state == MEDIA_AUDIO_PAUSE)
            && (g_cias_media.audio.prev_offset == -1) && (!g_cias_media.audio.is_set_offset))
    {
        CIAS_PRINT_INFO("renew set offset = %d\n",g_cias_media.offset);
        print_meida_state("connect url:<4>\n");
        cias_message_send_queue_clear();
        cias_delay(10);
        cias_media_set_audio_offset(g_cias_media.offset);
    }
    else
    {
    }
                
}

int cias_media_get_offset_size(void)
{
    return g_cias_media.offset;
}

int cias_media_get_block_size(void)
{
    return g_cias_media.block_size;
}

void cias_media_offset_size_increment(int len)
{
    g_cias_media.offset += len;
}

void cias_media_block_size_degrade(void)
{
    g_cias_media.block_size--;
}

bool cias_media_get_skip_speak(void)
{
    if(g_cias_media.skip_speak > 0)
    {
        //CIAS_PRINT_INFO("cias_media_get_skip_speak:[%d]\n",g_cias_media.skip_speak);
        return true;
    }
    return false;
}

void cias_media_set_skip_speak(bool state)
{
    if(state)
    {
        g_cias_media.skip_speak++;
    }
    else 
    {
        if(g_cias_media.skip_speak > 0)
        {
            g_cias_media.skip_speak--;
        }
    }
    CIAS_PRINT_INFO("cias_media_set_skip_speak:[%d]\n",g_cias_media.skip_speak);
}

void cias_media_set_audio_state(audio_state state)
{
    g_cias_media.audio.state = state;
}   

void cias_media_set_speak_state(speak_state state)
{
    g_cias_media.speak.state = state;
}

void cias_media_init(void)
{
    g_cias_media.speak.state = MEDIA_SPEAK_STOP;
    g_cias_media.audio.state = MEDIA_AUDIO_STOP;
    g_cias_media.is_disconnect_wifi = false;
    g_cias_media.onemp3 = NULL;
    if(g_cias_media.audio.url_head != NULL)
    {
        cias_media_del_music_url();
    }
    memset(&g_cias_media, 0, sizeof(cias_media_info_t));
}

void cias_media_play_end(void)
{
    if(g_cias_media.audio.state == MEDIA_AUDIO_PLAY)
    {
        g_cias_media.audio.state = MEDIA_AUDIO_STOP;
    }
}
#if CIAS_LAN_NETWORK_ENABLE
    #include "cias_lan_network.h"
    lan_data_t m_send_msg_lan;//add by whc
#endif

static bool if_set_prev_offset = false;
static unsigned short recv_data_size = 0;

bool recv_resume_music = false;

static void cias_media_get_tts_music_data(e_data_pos pos, const char *buf, size_t len)
{
    //CIAS_PRINT_INFO("cias_media_get_tts_music_data len = %d\n",len);
    // CIAS_PRINT_INFO("http data len2 = %d...\r\n",len);
    unsigned short offset = 0;
    static int32_t temp = 0;
    if(recv_resume_music&&(DATA_FIRST == pos))    //add by roy
    {
        recv_resume_music = false;
        g_cias_media.audio.state = 0;
    }
    uint16_t tvs_stat = get_audio_state();   //add by roy 解决tts播放异常打断音乐，pcm上传错误的问题
    if((tvs_stat == RECV_WAKE_UP)||(tvs_stat == SEND_PCM_MIDDLE))
    return;
    if(DATA_FIRST == pos)
    {
        recv_data_size = 0;
        if_set_prev_offset = false;
        g_cias_media.suspension = true;
        g_cias_media.http_type = HTTP_START;

        if((SPEAK_URL == g_cias_media.url_type) || (SPEAK_AND_MUSIC_URL == g_cias_media.url_type))
        {
            g_cias_media.speak.state = MEDIA_SPEAK_PLAY;
            while(1)
            {
                if (g_cias_media.speak.play_count == 1)
                {
                    cias_message_send_interface(SEND_START_PLAY_CMD, SPEAK_FILL, 0, NULL);
                    if(cias_media_wait_ack_cmd(200,4) == false)
                    {
                        break;
                    }
                }
                else
                {
                    //cias_message_send_interface(SEND_RESUME_PLAY_CMD, SPEAK_FILL, 0, NULL);
                    break;
                }
            }
            if (g_cias_media.speak.play_count == 1)
            {
                if((!g_cias_media.suspension) && g_cias_media.stop_notify)
                {
                    CIAS_PRINT_INFO("cias_media_get_tts_music_data 1\n");
                    return;
                }
            }
        }
        else
        {
            if(MEDIA_AUDIO_EXCEPTION_RESUME != g_cias_media.audio.state)
            {
                if(MEDIA_AUDIO_RESUME == g_cias_media.audio.state)
                {
                    g_cias_media.http_type = HTTP_MIDDLE;
                    if(!g_cias_media.is_disconnect_wifi)
                    {
                        if(!g_cias_media.resume_music_offset_err)
                        {
                            while(1)
                            {
                                cias_message_send_interface(SEND_RESUME_PLAY_CMD, g_cias_media.audio_format, 0, NULL);
                                if(cias_media_wait_ack_cmd(500,4) == false)
                                {
                                    if((!g_cias_media.suspension) && g_cias_media.stop_notify)
                                    {
                                        CIAS_PRINT_INFO("cias_media_get_tts_music_data 2\n");
                                        return;

                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    while(1)
                    {
                        CIAS_PRINT_INFO("g_cias_media.audio_format:<%d>\n",g_cias_media.audio_format);


                        #if TEST_PLAYER
                            break;
                        #else
                            cias_message_send_interface(SEND_START_PLAY_CMD, g_cias_media.audio_format, 0, NULL);
                            if(cias_media_wait_ack_cmd(1000,10) == false)
                            {
                                break;
                            }
                        #endif
                    }
                    cias_message_send_interface(SEND_START_PLAY_CMD, g_cias_media.audio_format, 0, NULL);    
                    g_cias_media.block_init_value = g_cias_media.offset;
                   
                }
                cias_media_set_block_size(0);
            }
            else 
            {
                if_set_prev_offset = true;
                // 2020.10.19 test
                g_cias_media.audio.state = MEDIA_AUDIO_PLAY;
                if(recv_data_size == FIRST_DATA_SIZE)
                {
                    if_set_prev_offset = false;
                }
                return;
            }
            g_cias_media.audio.state = MEDIA_AUDIO_PLAY;
        }
        g_cias_media.http_state = HTTP_MIDDLE;
        g_cias_media.resume_music_offset_err = false;
    }
    else
    {
        if((g_cias_media.offset != 0) && (g_cias_media.audio.prev_offset == g_cias_media.offset))
        {
            cias_message_send_interface(SEND_RESUME_PLAY_CMD, REPEAT_FILL, 0, NULL);
            cias_media_clear_suspension();
            cias_media_set_block_size(0);
        }
    }

    // 2020.10.19 test
    if (if_set_prev_offset && ((recv_data_size + len) <= FIRST_DATA_SIZE))
    {
        recv_data_size += len;
        return;
    }
    if(DATA_FIRST == pos)
    {
        temp = 0;
        audio_player_need_data_1L = false;
        audio_player_need_data_2L = false;
        audio_player_need_data_3L = false;
        // CIAS_PRINT_DEBUG("\nfirst packet start:\n");
        // for(int i=0;i<16;i++)CIAS_PRINT_DEBUG("%2x ",buf[i]);
        // CIAS_PRINT_DEBUG("\nfirst packet end:\n");
        // cias_delay(500);
    }

    
/*     if(cias_media_get_skip_speak())
    {
        CIAS_PRINT_INFO("cias_media_get_tts_music_data 3\n");
        return;
    } */


    #if TEST_PLAYER
        //通过局域网传输 资源到本地服务器？
    #else
        if(cias_media_block_size_is_zero() && ((g_cias_media.offset - g_cias_media.block_init_value)) >= (AUDIO_MUSIC_BUFF_COUNT- temp)*1024  )
        {
            //CIAS_PRINT_DEBUG("%d,%d, g_cias_media.offset = %d, block_init_value = %d",cias_media_block_size_is_zero(),(g_cias_media.offset - g_cias_media.block_init_value ), g_cias_media.offset, g_cias_media.block_init_value);
            while (cias_media_wait_ack_cmd(10,500));
            temp = 0;
        }

        if(audio_player_need_data_3L)
        {
            audio_player_need_data_3L = false;
            temp = AUDIO_MUSIC_BUFF_COUNT - 40;//请求40KB audio 数据
            CIAS_PRINT_DEBUG("audio_player_need_data_3L(%d)\n",temp);            
            
        }
        else if(audio_player_need_data_2L)
        {
            audio_player_need_data_2L = false;
            temp = AUDIO_MUSIC_BUFF_COUNT - 20;//请求20KB audio 数据
            CIAS_PRINT_DEBUG("audio_player_need_data_2L(%d)\n",temp);            
            
            
        }
        if(audio_player_need_data_1L)
        {
            audio_player_need_data_1L = false;
            // CIAS_PRINT_DEBUG("audio_player_need_data_1L\n");     
            // CIAS_PRINT_DEBUG(); 
        //    CIAS_LOG_DEBUG2("*");        
            temp = AUDIO_MUSIC_BUFF_COUNT - 5;//请求5KB audio 数据
        } 
        if(len > 0)
        {
            // #if CIAS_LAN_NETWORK_ENABLE				
            //     memcpy(m_send_msg_lan.data,buf,len);
            //     m_send_msg_lan.len  = len;
            //     if (CIAS_OK != cias_queue_send(&ci_lan_network_test_queue, &m_send_msg_lan, 10))
            //     {
            //        CIAS_PRINT_INFO("SEND_PCM_info(lan) fail\n");
            //     }
            // #endif

            if(if_set_prev_offset)
            {
                
                offset = len - (recv_data_size + len  - FIRST_DATA_SIZE);
                len = (recv_data_size + len  - FIRST_DATA_SIZE);
                if_set_prev_offset = false;
                recv_data_size += offset;
                cias_message_send_interface(SEND_PLAY_DATA_CMD, DEF_FILL, len, buf + offset);
                // CIAS_PRINT_INFO("1-cias_media_get_tts_music_data len = %d, offset = %d, recv_data_size = %d\n",len, offset, recv_data_size);
            }
            else
            {
                cias_message_send_interface(SEND_PLAY_DATA_CMD, DEF_FILL, len, buf);                
            }
            cias_media_block_size_degrade();
            cias_media_offset_size_increment(len);
            memset(buf,0,len);
            cias_delay(1);
        }
        
    #endif
    
}

static int cias_media_data_output_cb(void* p_user_ctx, e_data_pos pos, const char* buf, size_t len, const char* type)
{
    if(pos != DATA_LAST)
    {
        if(len > 0)
        {  
            cias_media_get_tts_music_data(pos, buf, len);
        }
    }
    else 
    {
        g_cias_media.http_type = HTTP_FINISH;
    }
    return 0;
}

static int cias_media_stop_notify_cb(void)
{
    if(g_cias_media.stop_notify == true)
    {
        return 1;
    }
    return 0;
}

void cias_media_task(void *parameter)
{
    cias_status err = CIAS_OK;
    cias_media_cmd cmd_msg;

    g_httpclient = cloud_create_http_client();
    if(g_httpclient == NULL)
    {
        CIAS_PRINT_ERR("http client create failed\n");
    }

    cias_http_client_reg_data_hdlr_cb(g_httpclient, cias_media_data_output_cb, NULL);
    cias_http_client_reg_stop_notify_cb(g_httpclient, cias_media_stop_notify_cb);

    for(;;)
    {
        err = cias_queue_receive(&cias_media_cmd_queue,&cmd_msg,CIAS_WAIT_FOREVER);
        if(err == CIAS_OK)
        {
            
            switch(cmd_msg.type)
            {
                case CIAS_CONNECT_URL_CMD:
                {
                    cias_media_connect_url();
                    break;
                }
                case CIAS_RECV_PLAY_END_CMD:
                {
                    cias_media_play_end();
                    break;
                }
                case CIAS_SEND_MUSIC_DATA_END:
                {
                    cias_finish_play_music();
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

}

int cias_media_init_interface(void)
{
    cias_status err = CIAS_OK;
    cias_media_init();

    err = cias_queue_create(&cias_media_cmd_queue,
                            CIAS_SEND_MEDIA_MSG_QUEUE_SIZE,
							sizeof(cias_media_cmd));

    if(err != CIAS_OK)
    {
        CIAS_PRINT_ERR("cias_media_cmd queue create error\n");
        return CIAS_FAIL;
    }

    cias_task_create(&cias_media_task_handler,
						"cias_media_task",
						(cias_task_function_t)cias_media_task,
						0,
						CIAS_APP_PRIORITY,
						MEDIA_TASK_SIZE);
    return CIAS_OK;
}
