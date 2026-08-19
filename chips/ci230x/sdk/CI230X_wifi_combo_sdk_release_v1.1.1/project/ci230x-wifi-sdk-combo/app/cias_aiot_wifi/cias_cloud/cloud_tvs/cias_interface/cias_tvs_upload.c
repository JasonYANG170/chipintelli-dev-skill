/**
 * @file    cias_tvs_upload.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */
#include <stdbool.h>

#include "cias_slave_message_handle.h"
#include "cias_media.h"
#include "cias_user_config.h"
#include "cias_pcm_data_handle.h"
#include "cias_common.h"
#include "cias_freertos_task.h"
#include "cias_freertos_queue.h"
#include "cias_freertos_debug.h"
#include "tvs_api.h"
#include "tvs_api_impl.h"
#include "cias_authentication.h"
#include "lwip/sockets.h"
#include "netif/ethernetif.h"
#include "cias_tvs_upload.h"

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

extern unsigned char g_tvs_wakeup_msg[];
extern cias_media_info_t g_cias_media;
cias_queue_t cias_speech_upload_queue;
TvsHalParamTypedef tvs_hal_param;

int callback_session_id = 0;
int g_start_rcv_audio_data_flag;
extern void notify_player_status_change(const char *url, PLAYER_STATUS_E status);
/*
// 网络连接成功后通知SDK
tvs_platform_adapter_on_network_state_changed(true);
// 网络断开时通知SDK
tvs_platform_adapter_on_network_state_changed(false);

*/
void set_tvs_network_state(bool sta)
{
	tvs_platform_adapter_on_network_state_changed(sta);
	tvs_hal_param.tvs_connect_state = sta;
}
bool get_tvs_network_state(void)
{
	return tvs_hal_param.tvs_connect_state;
}
// extern unsigned int callback_session_id;
extern void tvs_notify_player_status_change(PLAYER_STATUS_E status);

static void callback_on_reader_stop(int session_id, int error)
{
	callback_session_id = session_id;
	if (tvs_api_get_current_session_id() == session_id)
	{
		set_audio_state(SEND_PCM_FINISH);
		CIAS_PRINT_DEBUG("on reader stop, session id %d, error %d\n", session_id, error);
		g_start_rcv_audio_data_flag = 0;
		
		// SDK发送数据结束,如果本地录音没有结束，需要通知其结束
		switch (error)
		{
			case TVS_API_AUDIO_PROVIDER_ERROR_NONE:
			{
				break; // 正常结束
			}

			case TVS_API_AUDIO_PROVIDER_ERROR_STOP_CAPTURE:
			{
				cias_message_send_interface(SEND_VAD_END, DEF_FILL, 0, NULL);
				CIAS_PRINT_DEBUG("TVS_API_AUDIO_PROVIDER_ERROR_STOP_CAPTURE\r\n");
				break; // 收到云端VAD End标识
			}
			case TVS_API_AUDIO_PROVIDER_ERROR_TIME_OUT:
			{
				// 收到SDK Timeout标识，一般是网络原因导致HTTP超时
				break;
			}
			case TVS_API_AUDIO_PROVIDER_ERROR_NETWORK:
			{
				// 网络出错
				break;
			}
			default:
			{
				// 其他原因
				break;
			}
		}
	}
}

#if !PCM_DATA_DEAL
#define buf_len (10)
uint8_t buf[960 * buf_len];
#endif

extern cias_queue_t cias_recv_msg_form_audio_queue;
extern int ip_addr_get_status;
void cias_tvs_main_task(void *parameter)
{
	cias_status ret;
	cias_raw_speech_t recv_msg;
	unsigned short recv_type;
	uint32_t m_toal_tvs_recv_size = 0; // add by whc
	uint32_t m_ret = 0;
	uint8_t m_queue_size = 0;
	uint8_t m_queue_size_max = 0;
	uint8_t m_queue_count = 0;
	uint32_t buf_size = 0;
	while (!ip_addr_get_status) //等待WiFi连接
	{
		//CIAS_LOG_HL("tvs wait ip ...\r\n");
		cias_task_delay_ms(100);
	}
	CIAS_LOG_HL("--------------cias_tvs_main_task is exec-------------------\r\n");
	tvs_api_audio_provider_listen(callback_on_reader_stop);

	tvs_api_impl_init();
#if PCM_DATA_DEAL
	pcm_data_deal();
#else
	for (;;)
	{
		ret = cias_queue_receive(&cias_recv_msg_form_audio_queue, &recv_msg, CIAS_WAIT_FOREVER);
		if (ret == CIAS_OK)
		{
#if CIAS_LAN_NETWORK_ENABLE
			// add by whc 调试，在此处加入局域网通讯
			// ret = cias_queue_send(&ci_lan_network_test_queue, &recv_msg, pdMS_TO_TICKS(10));
			// if (ret != CIAS_OK)
			// {
			// 	CIAS_PRINT_WARN("SEND_PCM_info fail\n");
			// }
#endif
			recv_type = recv_msg.type;
			switch (recv_type)
			{
			case RECV_WAKE_UP:
			{

				memset(buf, 0, buf_size);
				buf_size = 0;
				m_queue_size = 0;
				m_queue_count = 0;
				m_queue_size_max = m_queue_size;
				m_toal_tvs_recv_size = recv_msg.length;
				tvs_api_impl_speech_wakeup();
				break;
			}
			case SEND_PCM_MIDDLE:
			{
				m_queue_size = cias_queue_waiting(&cias_recv_msg_form_audio_queue);
				CIAS_PRINT_DEBUG("1----now m_queue_size (%d)\n", m_queue_size);

				m_queue_size_max = (m_queue_size >= m_queue_size_max) ? m_queue_size : m_queue_size_max;

				if (m_queue_size >= CIAS_PCM_UPLOAD_QUEUE_SIZE - 2)
				{
					CIAS_PRINT_DEBUG("pcm data too more\n");
					continue;
				}
				if (m_queue_size >= CIAS_PCM_UPLOAD_QUEUE_SIZE / 2)
				{
					memcpy(buf + (m_queue_count * recv_msg.length), recv_msg.data, recv_msg.length);
					buf_size += recv_msg.length;
					m_queue_count++;
					if (m_queue_count >= buf_len)
					{
						m_ret = tvs_api_impl_speech_write_audio(buf, buf_size);
						if (m_ret <= 0)
						{
							CIAS_PRINT_DEBUG("tvs_api_impl_speech_write_audio1 erro(%d)\n", m_ret);
						}
						m_toal_tvs_recv_size += buf_size;

						memset(buf, 0, buf_size);
						buf_size = 0;
						m_queue_count = 0;
					}
				}
				else
				{
					m_ret = tvs_api_impl_speech_write_audio(recv_msg.data, recv_msg.length);
					if (m_ret <= 0)
					{
						CIAS_PRINT_DEBUG("tvs_api_impl_speech_write_audio2 erro(%d)\n", m_ret);
					}
					m_toal_tvs_recv_size += recv_msg.length;
				}

				break;
			}
			case SEND_PCM_FINISH:
			{
				CIAS_PRINT_DEBUG("pcm upload finish \n");
				m_toal_tvs_recv_size += recv_msg.length;
				CIAS_PRINT_DEBUG("m_toal_tvs_recv_size (speex recved pcm len)%d,%d,(m_queue_size_max)%d\r\n\n\n",
								 m_toal_tvs_recv_size,
								 recv_msg.length,
								 m_queue_size_max); // add by whc

				m_toal_tvs_recv_size = 0;
				m_queue_size = 0;
				m_queue_size_max = m_queue_size;
				cias_media_set_pcm_state(MEDIA_PCM_FINISH);
				tvs_api_impl_speech_write_end();
				break;
			}
			default:
				break;
			}
		}
	}

#endif
}
#if CIAS_LAN_NETWORK_ENABLE
#include "cias_lan_network.h"
#endif

static cias_task_t tvs_message_upload_handle = {
		.handle = NULL,
	};

int cias_tvs_main(void)
{

	if(cias_slave_message_handle_init() != CIAS_OK)
	{
		return CIAS_FAIL;
	}
	
	cias_task_create(&tvs_message_upload_handle, TVS_TASK_NAME, (cias_task_function_t)cias_tvs_main_task, (void *)0, TVS_TASK_PRIORITY, (unsigned short)TVS_TASK_SIZE);
	// util_trace_stack_add_task(tvs_message_upload_handle.handle);
	
	return CIAS_OK;
}


