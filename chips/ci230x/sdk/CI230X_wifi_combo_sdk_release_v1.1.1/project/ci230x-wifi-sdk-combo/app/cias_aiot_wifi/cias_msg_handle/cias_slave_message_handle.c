/**
 * @file    cias_slave_message_handle.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */
#include <stdbool.h>
#include <string.h>
#include "cias_demo_config.h"
#include "cias_freertos_common.h"
#include "cias_freertos_task.h"
#include "cias_freertos_mutex.h"
#include "cias_freertos_queue.h"
#include "cias_log.h"
#include "cias_link_msg_proc.h"
#include "cias_slave_message_handle.h"
#include "cias_user_config.h"
#include "cias_freertos_debug.h"
#include "cias_log.h"
#include "cJSON.h"
#include "cias_media.h"
#include "cias_communication.h"
#include "cias_wifi_port.h"
#include "Config.h"
// #if defined(APP_CIAS_CLOUD_IOT_TENCENT)
#include "cias_qcloud_iot.h"
#include "cias_user_config.h"
#include "cias_qcloud_parse.h"
// #endif

#include "cias_pcm_data_handle.h"
#include "cias_system_manage.h"

#define CIAS_SEND_SLAVE_MSG_QUEUE_SIZE 4

#if CIAS_TUYA_IR_CTRL_ENABLE
#include "cias_slave_message_handle.h"
#endif

#if CIAS_IOT_TVS_ENABLE
#include "tvs_media_player_inner.h"
#endif
/**************************全局变量*************************/

static volatile uint16_t g_audio_state = 0;
static bool is_wakeup = false;

/**************************extern变量*************************/
extern cias_media_info_t g_cias_media;
extern cias_system_manage_param_t g_cias_system_manage_param;
extern bool is_set_volume;
#if defined(CONFIG_APP_CIAS_CLOUD_TVS) // TVS云端功能使能
extern unsigned char g_tvs_wakeup_msg[];
#endif

uint16_t get_audio_state(void)
{
	return g_audio_state;
}

uint16_t set_audio_state(int state)
{
	g_audio_state = state;
}
void cias_message_send_queue_clear(void)
{
	cias_queue_reset(&cias_send_slave_msg_queue);
}

static int cias_init_send_msg_header(unsigned char *input, unsigned char size, unsigned short len,
									 unsigned short type, unsigned int fill_data, unsigned short version)
{
    memset(input, 0x00, size);
    if (input == NULL)
    {
        return -1;
    }

	cias_standard_head_t *head = (cias_standard_head_t *)input;

	head->magic = CIAS_STANDARD_MAGIC;
	head->type = type;
	head->len = len;
	head->version = version;
	head->fill_data = fill_data;

	return 0;
}
// and by whc
void cias_message_send_interface(unsigned short cmd, cias_fill_type_t type, int len, unsigned char *buf)
{
	cias_send_msg_t send_msg;
	int32_t ret = 0;
	unsigned int fill_data = 0;
	fill_data = type;
/* 	if (cmd == SEND_LOCAL_VOLUME_CMD)
	{
		fill_data = 0;
		len = 0;
	} */
	send_msg.type = cmd;
	cias_init_send_msg_header(send_msg.data, 16, len, send_msg.type, fill_data, 0x00);
	if (len > 0 && (len <= CIAS_SEND_MSG_BUF_LEN))
	{
		memcpy(send_msg.data + 16, buf, len);
		//CIAS_LOG_ERR("memcpy send_msg.data buf(%d)\r\n",*buf);
	}
	else
	{
		CIAS_LOG_ERR("send_msg.data len is error(%d)\r\n",len);
	}
	send_msg.length = 16 + len;
	//发送to audio侧数据。。。
	if (cias_queue_send(&cias_send_slave_msg_queue, &send_msg, pdMS_TO_TICKS(100)) != CIAS_OK)
	{
		CIAS_LOG_ERR("sizeof(send_msg) send fail...\r\n");
	}
	
	
}

void wakeup_deal(void)
{
	print_meida_state("uart recv:<1>");
	is_wakeup = true;
	if (g_cias_media.audio.state == MEDIA_AUDIO_PLAY)
	{
		g_cias_media.audio.state = MEDIA_AUDIO_RESUME;
	}
	g_cias_media.play_tts_end = false;
	if (g_cias_media.speak.state == MEDIA_SPEAK_PLAY || g_cias_media.speak.state == MEDIA_SPEAK_SEND_END)
	{
		g_cias_media.speak.state = MEDIA_SPEAK_CANCLE;
	}
	cias_media_tts_url_time_stop();
	g_cias_media.speak.asr_is_valid = false;
	cias_message_send_queue_clear();
	g_cias_media.resume_music_offset_err = false;
	cias_media_set_pcm_state(MEDIA_PCM_START);
	cias_media_set_skip_speak(false);
	cias_media_pause_music();
	cias_media_clear_cmd_queue();
	CIAS_PRINT_WARN("cias_media_quit_connect 3 !********************\n");
	cias_media_quit_connect();
	cias_media_clear_suspension();
	print_meida_state("uart recv:<2>");

	set_audio_state(RECV_WAKE_UP);
}


volatile bool audio_player_need_data_1L = false;
volatile bool audio_player_need_data_2L = false;
volatile bool audio_player_need_data_3L = false;

#define MAX_msg_num 0xffffffff
int32_t cias_slave_msg_handle(uint8_t *msg_buf, int32_t msg_len)
{
	if ((msg_buf == NULL) || (msg_len <= 0))
	{
		return -1;
	}
	cias_standard_head_t *phead = (cias_standard_head_t *)msg_buf;
	uint16_t recv_type = phead->type;
	uint32_t fill_data = phead->fill_data;

	cias_status ret;
	cias_raw_speech_t send_msg_speech;
	int32_t rent = 0;

	#if TEST_MODE_EN
        static uint32_t m_toal_number_recv_1103 = 0; //add by whc
        static uint32_t m_toal_size_recv_1103 = 0;//add by whc
        static uint32_t m_toal_size_send_tvs  = 0;//add by whc
        
		if(m_toal_number_recv_1103 > MAX_msg_num) 
			m_toal_number_recv_1103 = 0;
        else 
			m_toal_number_recv_1103++;

        m_toal_size_recv_1103 += phead->len;

    #endif

	
	
	// if (recv_type != 0x825 && recv_type != 0x3815)
	// {
	// 	CIAS_PRINT_DEBUG("recv_type = 0x%x, msg_len = %d:%d(%d),>>>>>Heap left: %d  min:%d<<<<<", recv_type, msg_len,m_toal_number_recv_1103,phead->len,
	// 		xPortGetFreeHeapSize(), 
	// 		xPortGetMinimumEverFreeHeapSize());//add by whc
	// }
	switch (recv_type)
	{

		case RECV_WAKE_UP:
		{
			wakeup_deal();
			break;
		}
		case SEND_PLAY_DATA_CMD:
		{
			print_meida_state("uart recv:<3>");
			//if (cias_get_sta_connect_status())
			if (cias_get_wifi_sta_connect_state() == CIAS_WIFI_STA_CONNECTED)
			{
				if (cias_media_is_http_middle() &&
					cias_media_is_audio_state(MEDIA_AUDIO_RESUME) &&
					(g_cias_media.offset != phead->fill_data))
				{
					g_cias_media.resume_music_offset_err = true;
					cias_media_set_audio_offset(phead->fill_data);
					CIAS_PRINT_WARN("cias_media_quit_connect 4 !********************\n");
					cias_media_quit_connect();
					cias_media_clear_suspension();
					cias_media_set_block_size(0);
					cias_media_tencent_resume_music();
					CIAS_PRINT_DEBUG("g_cias_media.offset != phead->fill_data[%d]\n", phead->fill_data);
				}
				else
				{
					CIAS_PRINT_DEBUG("SEND_PLAY_DATA_CMD\r\n");
					cias_media_set_pcm_state(MEDIA_PCM_INIT);
					cias_media_clear_suspension();
					cias_media_set_block_size(0);
					if (g_cias_media.audio.state != MEDIA_AUDIO_PLAY)
					{
						cias_message_send_queue_clear();
					}
				}
			}
			print_meida_state("uart recv:<4>");
			break;
		}
		case MP3_BUFF_START_RECV1L:
		{
			cias_media_clear_suspension();
			cias_media_set_block_size(5);//请求5KB audio 数据
			audio_player_need_data_1L = true;
			break;
		}
		case MP3_BUFF_START_RECV2L:
		{
			cias_media_clear_suspension();
			cias_media_set_block_size(20);//请求20KB audio 数据
			audio_player_need_data_2L = true;
			break;
		}
		case MP3_BUFF_START_RECV3L:
		{
			cias_media_clear_suspension();
			cias_media_set_block_size(40);//请求40KB audio 数据
			audio_player_need_data_3L = true;
			break;
		}
		case LOCAL_ASR_RESULT_NOTIFY:
		{
			#if CIAS_IOT_TVS_ENABLE
				is_set_volume = false;
				extern int ip_addr_get_status;
				print_meida_state("uart recv:<5>");
				if (ip_addr_get_status)
				{
					cias_media_set_pcm_state(MEDIA_PCM_FINISH);
					cias_media_set_audio_offset(g_cias_media.audio.skip_prev_offset);
					cias_media_set_skip_speak(true);  //and by yjd
					tvs_api_stop_all_activity();
				}
				print_meida_state("uart recv:<6>");
			#endif
			break;
		}
		case RECV_PLAY_END_CMD:
		{
			print_meida_state("uart recv:<7>");
			if (g_cias_media.speak.state == MEDIA_SPEAK_SEND_END || g_cias_media.speak.state == MEDIA_SPEAK_PLAY)
			{
				g_cias_media.play_tts_end = false;
				g_cias_media.speak.state = MEDIA_SPEAK_PLAY_END;
			}
			cias_media_set_skip_speak(false);
			cias_media_clear_suspension();
			cias_media_set_block_size(0);
			CIAS_PRINT_WARN("cias_media_quit_connect 5 !********************\n");
			cias_media_quit_connect();
			if ((is_wakeup == false) || (g_cias_media.speak.state == MEDIA_SPEAK_PLAY_END))
			{
				if (get_http_data_is_complete() && g_cias_media.url_type == SPEAK_URL)
				{
					cias_media_recv_cmd(CIAS_RECV_PLAY_END_CMD);
					CIAS_PRINT_INFO("play end\n");
				}
				else if (g_cias_media.audio.state == MEDIA_AUDIO_RESUME)
				{
					g_cias_media.audio.state = MEDIA_AUDIO_RESUME;
					g_cias_media.speak.state = MEDIA_SPEAK_STOP;
					CIAS_PRINT_INFO("play resume\n");
				}
				else if (g_cias_media.audio.state == MEDIA_AUDIO_PAUSE)
				{
					CIAS_PRINT_INFO("play pause\n");
				}
				else
				{
					g_cias_media.audio.state = MEDIA_AUDIO_PLAY;
					g_cias_media.speak.state = MEDIA_SPEAK_STOP;
					CIAS_PRINT_INFO("play start\n");
				}
			}
			print_meida_state("uart recv:<8>");
			break;
		}
		case RECV_PLAY_TTS_END_CMD:    //add by roy
		{
			g_cias_media.stop_notify = false;
			break;
		}
		case SKIP_INVAILD_SPEAK:
		{
			print_meida_state("uart recv:<9>");
			cias_message_send_queue_clear();
			g_cias_media.speak.state = MEDIA_SPEAK_STOP;
			cias_media_set_skip_speak(true);
			cias_media_clear_cmd_queue();
			cias_media_clear_suspension();
			print_meida_state("uart recv:<10>");
			break;
		}
		case RECV_RESUME_PLAY_MP3_SIZE_CMD:
		{
			print_meida_state("uart recv:<11>");
			if (MEDIA_AUDIO_EXCEPTION_RESUME == g_cias_media.audio.state)
			{
				g_cias_media.audio.state = MEDIA_AUDIO_RESUME;
			}
			CIAS_PRINT_DEBUG("recv offset = %d\n", phead->fill_data);
			//if (!cias_get_sta_connect_status())
			if (cias_get_wifi_sta_connect_state() != CIAS_WIFI_STA_CONNECTED)
			{
				g_cias_media.is_disconnect_wifi = false;
			}
			cias_media_set_audio_offset(phead->fill_data);
			print_meida_state("uart recv:<12>");
			break;
		}
#if CIAS_IOT_TVS_ENABLE
		case RECV_RESUME_PLAY_CMD:       //add by roy
        {
            asr_set_resume_play_status();
            tvs_media_player_inner_start_play();
            break;
        }
#endif

#if CIAS_BLE_CONFIG_ENABLE
		case ENTER_SMATRCONFIG_CMD:
		{
			extern uint8_t start_CI_BLE_config(bool BLE_switch);
			start_CI_BLE_config(true);//开始蓝牙配网
		}
#endif

#if 0
		case ENTER_SMATRCONFIG_CMD:
		{
			CIAS_PRINT_DEBUG("ENTER_SMATRCONFIG_CMD\n");
			if (cias_smatrconfig_mode_get() == ENTER_AIRKISS_CMD)
			{
				/* 退出 AIRKISS 配网 */
				//   cias_airkiss_net_stop();
			}
			else if (cias_smatrconfig_mode_get() == ENTER_BLE_CMD)
			{
				/* 退出 BLE 配网 */
			}
			cias_smatrconfig_mode_set(ENTER_SMATRCONFIG_CMD);
			cias_start_ap_net();
			break;
		}
		case EXIT_SMATRCONFIG_CMD:
		{
			CIAS_PRINT_DEBUG("EXIT_SMATRCONFIG_CMD\n");
			if (cias_smatrconfig_mode_get() == ENTER_SMATRCONFIG_CMD)
			{
				cias_exit_ap_net();
			}
			else if (cias_smatrconfig_mode_get() == ENTER_AIRKISS_CMD)
			{
				/* 退出 AIRKISS 配网 */
				//     cias_airkiss_net_stop();
			}
			else if (cias_smatrconfig_mode_get() == ENTER_BLE_CMD)
			{
				/* 退出 BLE 配网 */
				// ble_exit();
			}
			cias_smatrconfig_mode_set(0);
			break;
		}
		case ENTER_AIRKISS_CMD:
		{
			CIAS_PRINT_DEBUG("ENTER_AIRKISS_CMD\n");
			if (cias_smatrconfig_mode_get() == ENTER_SMATRCONFIG_CMD)
			{
				cias_exit_ap_net();
			}
			else if (cias_smatrconfig_mode_get() == ENTER_BLE_CMD)
			{
				/* 退出 BLE 配网 */
				// ble_exit();
			}
			break;
		}
		case ENTER_BLE_CMD:
		{
			CIAS_PRINT_DEBUG("ENTER_BLE_CMD\n");
			break;
		}
		case INIT_SMATRCONFIG_CMD:
		{

			CIAS_PRINT_INFO("Defult Factory settings!");
			cias_set_defult_ssid_passwd();
			break;
		}
		case START_OTA:
		{
			break;
		}
		case CIAS_OTA_START:
		{
			CIAS_PRINT_INFO("cias_http_ota_port_task_create is called....\r\n");
			//  cias_http_ota_port_task_create();
			break;
		}
		case CIAS_OTA_DATA: // and by yjd
		{
			http_ota_olcp_recv(msg_buf + 16, phead->len);
			break;
		}
		
#endif
		
        case SEND_PCM_START:
        case SEND_PCM_MIDDLE:
        {
            rent = cias_send_pcm_middle(send_msg_speech,phead,msg_buf);
            #if TEST_MODE_EN
                if(rent) m_toal_size_send_tvs += rent;
            #endif
            break;
        }
        case SEND_PCM_FINISH:
        {
            rent = cias_send_pcm_finish(send_msg_speech,phead,msg_buf);
            #if TEST_MODE_EN
                if(rent) m_toal_size_send_tvs += rent;
                CIAS_PRINT_DEBUG("m_toal_number_recv_audio is:%d toal_len_recv_audio %d m_toal_size(pcm)_send_tvs is %d\r\n",
                                                                                                m_toal_number_recv_1103,
                                                                                                m_toal_size_recv_1103,
                                                                                                m_toal_size_send_tvs);//add by whc
				m_toal_number_recv_1103 = 0;
				m_toal_size_recv_1103 = 0;
				m_toal_size_send_tvs = 0;

			#endif
			
            break;
        }
        
       

		case QCLOUD_IOT_CMD:
		{
			unsigned short recv_cmd = 0;
			if (msg_len < 18)
				break;
			recv_cmd |= (msg_buf[16]);
			recv_cmd |= (msg_buf[17] << 8);

			CIAS_PRINT_DEBUG("recv_aduio_cmd = 0x%x\n", recv_cmd);
			#if (CI2305_RGB_DEMO_ENABLE || TENCENT_AP_NET_WORK_EN)//add by hw			
					recv_audio_cmd_handle(recv_cmd);
			#endif//(CI2305_RGB_DEMO_ENABLE || TENCENT_AP_NET_WORK_EN)
			#if CIAS_TUYA_IR_CTRL_ENABLE
					tuya_ir_ctrl_audio_cmd_handle(recv_cmd);
			#endif
			/* if(recv_cmd == 0x1802)   //该按键(开机)用做生产测试触发标志
				{
				gstart_listen_factory_key = 1;
				}
				if(recv_cmd == 0x8123)
				{
				gfactory_test_mode = 1;
				gci_start_listen_factory_flag = 1;
				}
				if(recv_cmd == 0x1823)
				{
				gfactory_test_mode = 2;
				gci_start_listen_factory_flag = 1;
				}*/
			// recv_audio_cmd_handle(recv_cmd);
			break;
		}
	}
}
// end
void cias_slave_message_recv_deal(void *parameter)
{
	int ret = 0;
	cias_standard_head_t *recv_headpart = NULL;
	static int32_t record_recv_package_length = 0;
	static int32_t recv_package_length = 0;
	static slave_recv_state_t slave_msg_state = MSG_FIND_HEAD;
	
	uint32_t ret_sdio_read;
	uint8_t recv_slave_buf[SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE] = {0};
	while (1)
	{
		//接收audio侧数据。。。
		ret = cias_queue_receive(&cias_recv_slave_msg_queue, recv_slave_buf, CIAS_WAIT_FOREVER);
		if (ret == CIAS_OK)
		{
			recv_headpart = (cias_standard_head_t *)recv_slave_buf;	
			recv_package_length = recv_headpart->len + sizeof(cias_standard_head_t);
			cias_slave_msg_handle((uint8_t *)recv_slave_buf, recv_package_length);
			recv_package_length = 0;
		}
		
		memset(recv_slave_buf, 0, SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE);
	}
}

int cias_slave_message_handle_init(void)
{

	CIAS_LOG_HL("--------cias_slave_message_recv_deal is exec ......\r\n");
	if( communication_task_init() != CIAS_OK)
	{
		return CIAS_FAIL;
	}
#if !test_ota_mode

	cias_task_create(&g_cias_system_manage_param.recv_slave_msg_deal_handle,
					 DEAL_RECV_SLAVE_DATA_TASK_NAME,
					 (cias_task_function_t)cias_slave_message_recv_deal,
					 (void *)0,
					 DEAL_AUDIO_RECV_TASK_PRIO,
					 (unsigned short)DEAL_ADUIO_RECV_TASK_SIZE);
#endif
	return CIAS_OK;
}
