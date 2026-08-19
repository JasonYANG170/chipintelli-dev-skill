/*
 * @Description: PCM原始数据处理，
 * @Author: hongchuan.wu
 * @Date: 2021-12-02 13:33:33
 * @LastEditTime: 2022-05-24 18:18:31
 * @LastEditors: Please set LastEditors
 * @Reference:
 */
#include <string.h>
#include <freeRTOS.h>
#include "cias_media.h"

#if CIAS_IOT_TVS_ENABLE // TVS云端功能使能
#include "tvs_audio_provider.h"
#include "tvs_api_impl.h"
#endif //#if CIAS_IOT_TVS_ENABLE

#include "cias_user_config.h"
#include "cias_pcm_data_handle.h"
#include "cias_freertos_debug.h"
#include "cias_freertos_common.h"
#include "cias_common.h"
#include "cias_slave_message_handle.h"
#if PCM_DATA_DEAL

static pam_data_buff_t this_pcm_info;

void pcm_data_buf_init(pam_data_buff_t *p)
{
	if (p->pcm_data_head != NULL)
	{
		memset(p->pcm_data_head, 0, PCM_BUFF_LEN * PCM_BUFF_COUNT);
		p->pcm_data_tail = p->pcm_data_head + PCM_BUFF_LEN * PCM_BUFF_COUNT;
		p->pcm_data_read = p->pcm_data_head;
		p->pcm_data_write = p->pcm_data_head;
		p->pcm_data_writ_count = 0;
		p->pcm_data_read_count = 0;
	}
}
void pcm_data_buf_free(pam_data_buff_t *p)
{
	if (p->pcm_data_head != NULL)
	{
		memset(p->pcm_data_head, 0, PCM_BUFF_LEN * PCM_BUFF_COUNT);
		cias_free(p->pcm_data_head);
		p->pcm_data_head = NULL;
		p->pcm_data_tail = p->pcm_data_head;
		p->pcm_data_read = p->pcm_data_head;
		p->pcm_data_write = p->pcm_data_head;
		p->pcm_data_writ_count = 0;
		p->pcm_data_read_count = 0;
	}
}
/**
 * @brief Get the free space object
 * @return int32_t 获取到的当前剩余缓存，<0 表示出错
 */
int32_t get_free_space(pam_data_buff_t *p)
{
	if (SEND_PCM_MIDDLE == get_audio_state())
	{
		if ((p->pcm_data_tail - p->pcm_data_write) < 0 || p->pcm_data_write - p->pcm_data_head < 0)
		{
			return -103;
		}

		if (p->pcm_data_write == p->pcm_data_read)
		{
			if (p->pcm_data_write == p->pcm_data_head && p->pcm_data_read_count == 0)
			{
				CIAS_PRINT_INFO("write start\r\n");
				return (p->pcm_data_tail - p->pcm_data_head);
			}
			else if (p->pcm_data_read_count < p->pcm_data_writ_count)
			{
				return 0;
			}
			else
			{
				return (p->pcm_data_tail - p->pcm_data_head);
			}
		}
		else if (p->pcm_data_write > p->pcm_data_read)
		{
			if (p->pcm_data_read_count == p->pcm_data_writ_count)
			{
				return ((p->pcm_data_tail - p->pcm_data_write) + (p->pcm_data_read - p->pcm_data_head));
			}
			else if (p->pcm_data_writ_count > p->pcm_data_read_count)
			{
				return -1;
			}
			else
			{
				CIAS_PRINT_INFO("read too fast,write too slow\r\n"); //写慢了
				return (p->pcm_data_tail - p->pcm_data_head);
			}
		}
		else
		{
			if (p->pcm_data_read_count < p->pcm_data_writ_count)
			{
				return (p->pcm_data_read - p->pcm_data_write);
			}
			else
			{
				return (p->pcm_data_tail - p->pcm_data_head);
			}
		}
	}
	return 0;
}

/**
 * @brief 写数据
 *
 * @param date
 * @param len
 * @return uint32_t 写入数据的len
 */
uint32_t write_pcm_data(unsigned char *date, uint32_t len)
{
	int32_t rent = 0;
	uint32_t temp = 0;
	uint32_t temp2 = 0;
	static uint32_t rent_min = 100;
	uint32_t rent_count = 0;

	if (this_pcm_info.pcm_data_head == NULL)
	{
		CIAS_PRINT_INFO("pcm_data_head is NULL");
		return -1;
	}
	if (date == NULL)
	{
		CIAS_PRINT_INFO("pcm_data is NULL");
		return -2;
	}
	if (len == 0)
	{
		CIAS_PRINT_INFO("pcm_len is NULL");
		return -3;
	}

	while (1) //判断可写条件
	{
		rent = get_free_space(&this_pcm_info);
		if (rent <= 0)
		{
			if (rent == 0)
			{
				CIAS_PRINT_INFO("write full and read too slow\r\n"); //数据写满了
				cias_task_delay_ms(50);
			}
			if (rent == -1)
			{
				CIAS_PRINT_INFO("write too fast,read too slow\r\n"); //数据写超了
				cias_task_delay_ms(50);
			}
			if (rent == -103)
			{
				CIAS_PRINT_ERR("!!! write overtask !!!!\r\n\n\n\n"); //写指针越界
				return -1;
			}
		}
		else
		{
			rent = rent / PCM_BUFF_LEN;
			if (rent <= PCM_BUFF_COUNT_MIN)
			{
				CIAS_PRINT_ERR("Free_Space is < PCM_BUFF_COUNT_MIN\n");
				break;
			}
			else if (rent > PCM_BUFF_COUNT)
			{
				CIAS_PRINT_ERR("error (now_free)(%d,%d%%)\n", (rent), rent_count);
			}
			else if ((rent > PCM_BUFF_COUNT_MIN) && (rent <= PCM_BUFF_COUNT))
			{
				// rent_count = ((rent)*100) / PCM_BUFF_COUNT;
				// rent_min = rent_count < rent_min ? rent_count : rent_min;
				// CIAS_PRINT_INFO("Free_Space is (min_free)%d%%,(now_free)(%d,%d%%)\n", rent_min, (rent), rent_count);
				break;
			}
		}
	}

	if ((this_pcm_info.pcm_data_write + len) > this_pcm_info.pcm_data_tail) //写入数据超过 tail
	{
		temp = this_pcm_info.pcm_data_tail - this_pcm_info.pcm_data_write;			// tail 之前数据len
		temp2 = (this_pcm_info.pcm_data_write + len) - this_pcm_info.pcm_data_tail; // tail 之后数据len

		memcpy(this_pcm_info.pcm_data_write, date, temp);			// 写 tail 之前数据len
		this_pcm_info.pcm_data_write = this_pcm_info.pcm_data_head; //写指针回到头
		this_pcm_info.pcm_data_writ_count++;
		memcpy(this_pcm_info.pcm_data_write, date + temp, temp2); //写剩下的数据
		this_pcm_info.pcm_data_write += temp2;
		return len;
	}
	else //写入数据不超过 tail
	{
		memcpy(this_pcm_info.pcm_data_write, date, len);
		this_pcm_info.pcm_data_write += len;
		return len;
	}
}

/**
 * @brief Get the used space object
 *
 * @return int32_t 当前可用的数据len，<0 表示出错
 */
int32_t get_used_space(pam_data_buff_t *p)
{
	if ((p->pcm_data_tail - p->pcm_data_read) < 0 || p->pcm_data_read - p->pcm_data_head < 0)
	{
		return -103;
	}

	if (p->pcm_data_read < p->pcm_data_write)
	{
		if (p->pcm_data_read_count == p->pcm_data_writ_count)
		{
			return (p->pcm_data_write - p->pcm_data_read);
		}
		else if (p->pcm_data_read_count < p->pcm_data_writ_count)
		{
			return -100;
		}
		else
			return -101;
	}
	else if (p->pcm_data_read == p->pcm_data_write)
	{
		if (p->pcm_data_read_count == p->pcm_data_writ_count)
		{
			if (p->pcm_data_read_count == 0 && (p->pcm_data_read == p->pcm_data_head))
			{
				return -102;
			}
			else
				return 0;
		}
		else if (p->pcm_data_read_count < p->pcm_data_writ_count)
		{
			return (p->pcm_data_tail - p->pcm_data_head);
		}
		else
		{
			return -101;
		}
	}
	else
	{
		if (p->pcm_data_read_count < p->pcm_data_writ_count)
		{
			return ((p->pcm_data_tail - p->pcm_data_read) + (p->pcm_data_write - p->pcm_data_head));
		}
		else
		{
			return -101;
		}
	}
}
extern int start_recorder_state;
void pcm_data_deal(void)
{
	int32_t m_speex_len = 0;
	int32_t read_size_temp = 0;
	int32_t m_speex_ret = 0;
	int32_t ret = 0;
	uint16_t tvs_stat = 0;
	while (1)
	{

		tvs_stat = get_audio_state();
		switch (tvs_stat)
		{
		case RECV_WAKE_UP: // tvs start
		{
			CIAS_PRINT_DEBUG("<<<<<<<<<<<<<<<<<<<<RECV_WAKE_UP>>>>>>>>>>>>>>>>>>>>\n");
			if (this_pcm_info.pcm_data_head == NULL)
			{
				this_pcm_info.pcm_data_head = (char *)cias_malloc(PCM_BUFF_LEN * PCM_BUFF_COUNT);
				if (this_pcm_info.pcm_data_head == NULL)
				{
					CIAS_PRINT_DEBUG("pcm src malloc failed\n");
					continue;
				}
			}

			pcm_data_buf_init(&this_pcm_info);
			start_recorder_state = -1;

			tvs_api_impl_speech_write_end();
			ret = tvs_api_impl_speech_wakeup();
			int8_t build_new_recorder_timeout = 20; //等待tvs recorder 建立新的 session_id 的超时时间 20*100ms
			while (build_new_recorder_timeout--)
			{
				if (ret == 0 && (start_recorder_state > 0))
				{
					break;
				}
				cias_task_delay_ms(100);
			}
			CIAS_PRINT_DEBUG("speech wakeup used time:%d(ms)", (20 - build_new_recorder_timeout) * 100);

			if (ret == 0 && (start_recorder_state > 0))
			{
				set_audio_state(SEND_PCM_MIDDLE);
				CIAS_PRINT_DEBUG("speech wakeup OK(%d,%d)", ret, start_recorder_state);
			}
			else
			{
				set_audio_state(RECV_WAKE_UP);
				CIAS_PRINT_DEBUG("speech wakeup failed(%d,%d)", ret, start_recorder_state);
				//cias_task_delay_ms(500);
				continue;
			}

			CIAS_PRINT_DEBUG(">>>>>Heap leftw: %d  min:%d<<<<<\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
			break;
		}
		case SEND_PCM_MIDDLE: // tvs MIDDLE
		{

		last_data_lable:
			m_speex_len = get_used_space(&this_pcm_info);

#if AUDIO_SPEEX_WORK
			if (m_speex_len > 0)
			{
				if (this_pcm_info.pcm_data_read + m_speex_len <= this_pcm_info.pcm_data_tail)
				{
					m_speex_ret = outside_encode_data_write(this_pcm_info.pcm_data_read, m_speex_len);
					if (m_speex_ret > 0)
					{
						this_pcm_info.pcm_data_read += m_speex_len;
					}
					else
					{
						set_audio_state(SEND_PCM_FINISH);
						CIAS_PRINT_ERR("m_speex_ret 1 err(%d)\n", m_speex_ret);
					}
				}
				else
				{
					read_size_temp = this_pcm_info.pcm_data_tail - this_pcm_info.pcm_data_read; //写tail 之前数据 to speex
					if (read_size_temp == 0)
					{
						this_pcm_info.pcm_data_read = this_pcm_info.pcm_data_head; // read 回到头
						this_pcm_info.pcm_data_read_count++;
					}
					else if (read_size_temp > 0)
					{
						m_speex_ret = outside_encode_data_write(this_pcm_info.pcm_data_read, read_size_temp);
						if (m_speex_ret > 0)
						{
							this_pcm_info.pcm_data_read = this_pcm_info.pcm_data_head; // read 回到头
							this_pcm_info.pcm_data_read_count++;
						}
						else
						{
							set_audio_state(SEND_PCM_FINISH);
							CIAS_PRINT_ERR("m_speex_ret 2 err(%d)\n", m_speex_ret);
						}
					}
					else
						CIAS_PRINT_DEBUG("error (read point overstep )!!!\n");

					read_size_temp = (m_speex_len - read_size_temp);
					if (read_size_temp)
					{
						m_speex_ret = outside_encode_data_write(this_pcm_info.pcm_data_read, read_size_temp); //写tail 之后数据 to speex
						if (m_speex_ret > 0)
						{
							this_pcm_info.pcm_data_read += read_size_temp;
						}
						else
						{
							set_audio_state(SEND_PCM_FINISH);
							CIAS_PRINT_ERR("m_speex_ret 3 err(%d)\n", m_speex_ret);
						}
					}
				}
			}
			else if (m_speex_len == (-100)) //写超了/读慢了(需要读所有数据)
			{
				CIAS_PRINT_DEBUG("read ALL (%d)\n", m_speex_len);
			}
			else if (m_speex_len == (-101)) //读超了，数据无法使用了 延时等待
			{
				CIAS_PRINT_DEBUG("read too fast(%d)\n", m_speex_len);
				cias_task_delay_ms(20);
				break;
			}
			else if (m_speex_len == (-102)) //刚开始读
			{
				CIAS_PRINT_DEBUG("read start(%d)\n", m_speex_len);
				cias_task_delay_ms(300);
			}
			else if (m_speex_len == (-103)) // read point 越界
			{
				CIAS_PRINT_ERR("read error(%d)\n", m_speex_len);
			}
			else if (m_speex_len == 0) //读完了
			{
				// CIAS_PRINT_DEBUG("read flish   (%d)\n", m_speex_len);
				cias_task_delay_ms(200);
				break;
			}
			else
				cias_task_delay_ms(50);
#else  //AUDIO_SPEEX_WORK
			if (m_speex_len > 0)
			{

				read_size_temp = m_speex_len / PCM_BUFF_LEN; //按 PCM_BUFF_LEN 取数据
				if (read_size_temp > 0 || tvs_stat == SEND_PCM_FINISH)
				{

					if (tvs_stat != SEND_PCM_FINISH)
					{
						m_speex_len = (PCM_BUFF_LEN * read_size_temp);
					}

					if (this_pcm_info.pcm_data_read + m_speex_len <= this_pcm_info.pcm_data_tail)
					{
						m_speex_ret = tvs_audio_provider_write(this_pcm_info.pcm_data_read, m_speex_len);
						if (m_speex_ret > 0)
						{
							CIAS_PRINT_DEBUG("m_speex_ret 1(%d)\n", m_speex_ret);
							this_pcm_info.pcm_data_read += m_speex_len;
						}
						else
							CIAS_PRINT_ERR("m_speex_ret 1 err(%d)\n", m_speex_ret);
					}
					else
					{

						read_size_temp = this_pcm_info.pcm_data_tail - this_pcm_info.pcm_data_read; //写tail 之前数据 to speex
						if (read_size_temp == 0)
						{
							this_pcm_info.pcm_data_read = this_pcm_info.pcm_data_head; // read 回到头
							this_pcm_info.pcm_data_read_count++;
						}
						else if (read_size_temp > 0)
						{
							m_speex_ret = tvs_audio_provider_write(this_pcm_info.pcm_data_read, read_size_temp);
							if (m_speex_ret > 0)
							{
								CIAS_PRINT_DEBUG("m_speex_ret 2(%d)\n", m_speex_ret);
								this_pcm_info.pcm_data_read = this_pcm_info.pcm_data_head; // read 回到头
								this_pcm_info.pcm_data_read_count++;
							}
							else
								CIAS_PRINT_ERR("m_speex_ret 2 err(%d)\n", m_speex_ret);
						}
						else
							CIAS_PRINT_DEBUG("error (read point overstep )!!!\n");

						read_size_temp = (m_speex_len - read_size_temp);
						if (read_size_temp)
						{
							m_speex_ret += tvs_audio_provider_write(this_pcm_info.pcm_data_read, read_size_temp); //写tail 之后数据 to speex
							if (m_speex_ret > 0)
							{
								CIAS_PRINT_DEBUG("m_speex_ret 3(%d)\n", m_speex_ret);
								this_pcm_info.pcm_data_read += read_size_temp;
							}
							else
								CIAS_PRINT_ERR("m_speex_ret 3 err(%d)\n", m_speex_ret);
						}
					}
				}
				else //数据不足BUFF_LEN，下次读
				{

					CIAS_PRINT_DEBUG("m_speex_len < PCM_BUFF_LEN,delay(10)\n");
					cias_task_delay_ms(10);
				}
			}
			else if (m_speex_len == (-100)) //全部读
			{
				m_speex_len = this_pcm_info.pcm_data_tail - this_pcm_info.pcm_data_head;	//总长度
				read_size_temp = this_pcm_info.pcm_data_tail - this_pcm_info.pcm_data_read; // tail 之前的长度
				this_pcm_info.pcm_data_read = this_pcm_info.pcm_data_write;

				m_speex_ret = tvs_audio_provider_write(this_pcm_info.pcm_data_read, read_size_temp); //写tail 之前数据 to speex
				if (m_speex_ret > 0)
				{
					CIAS_PRINT_DEBUG("m_speex_ret4(%d)\n", m_speex_ret);
					this_pcm_info.pcm_data_read = this_pcm_info.pcm_data_head; // read 回到头
					this_pcm_info.pcm_data_read_count++;
				}

				read_size_temp = m_speex_len - read_size_temp; // tail 之后
				if (read_size_temp)
				{
					m_speex_ret += tvs_audio_provider_write(this_pcm_info.pcm_data_read, read_size_temp); //写tail 之后数据 to speex
					if (m_speex_ret > 0)
					{
						CIAS_PRINT_DEBUG("m_speex_ret5(%d)\n", m_speex_ret);
						this_pcm_info.pcm_data_read += read_size_temp;
					}
				}
			}
			else if (m_speex_len == (-101))
			{
				CIAS_PRINT_DEBUG("read too fast(%d)\n", m_speex_len);
				cias_task_delay_ms(20);
				break;
			}
			else if (m_speex_len == (-102))
			{
				CIAS_PRINT_DEBUG("read start(%d)\n", m_speex_len);
				cias_task_delay_ms(20);
			}
			else if (m_speex_len == -103)
			{
				CIAS_PRINT_ERR("read error(%d)\n", m_speex_len);
			}
			else if (m_speex_len == 0)
			{
				CIAS_PRINT_DEBUG("read flish   (%d)\n", m_speex_len);
				cias_task_delay_ms(50);
				break;
			}
#endif //AUDIO_SPEEX_WORK
			break;
		}

		case SEND_PCM_FINISH: // tvs end   SEND_PCM_FINISH
		{

			cias_task_delay_ms(10);
			pcm_data_buf_free(&this_pcm_info);
			tvs_api_impl_speech_write_end();
			CIAS_PRINT_DEBUG("SEND_PCM_FINISH-11111\n");
			set_audio_state(SEND_PCM_IDLE);
			break;
		}

		default:
		{

			cias_task_delay_ms(200);
			break;
		}
		}
	}
}

#endif

int32_t cias_send_pcm_middle(cias_raw_speech_t send_msg_speech, cias_standard_head_t *phead, uint8_t *msg_buf)
{
	int32_t rent = 0;

	send_msg_speech.type = phead->type;
	send_msg_speech.length = phead->len;
	memcpy(send_msg_speech.data, msg_buf + 16, phead->len);

#if PCM_DATA_DEAL
	if (get_audio_state() == SEND_PCM_MIDDLE)
	{
		rent = write_pcm_data(send_msg_speech.data, send_msg_speech.length);
		if (rent < CIAS_OK)
		{
			CIAS_PRINT_WARN("SEND_PCM_MIDDLE FAIL1(%d)\n", rent);
		}
	}

#else
	rent = cias_queue_send(&cias_recv_msg_form_audio_queue, &send_msg_speech, pdMS_TO_TICKS(10));
	if (rent != CIAS_OK)
	{
		CIAS_PRINT_WARN("SEND_PCM_MIDDLE FAIL2(%d)\n", rent);
	}
#endif
	else
	{
		// CIAS_PRINT_DEBUG("SEND_PCM_MIDDLE OK!\n");
	}
	return rent;
}
int32_t cias_send_pcm_finish(cias_raw_speech_t send_msg_speech, cias_standard_head_t *phead, uint8_t *msg_buf)
{
	int32_t rent = 0;

	CIAS_PRINT_INFO("RECV PCM FINISH(%d)\n", phead->len);

	send_msg_speech.type = phead->type;
	send_msg_speech.length = phead->len;
	memcpy(send_msg_speech.data, msg_buf + sizeof(cias_standard_head_t), phead->len);

#if PCM_DATA_DEAL
	set_audio_state(SEND_PCM_FINISH);
#else

	rent = cias_queue_send(&cias_recv_msg_form_audio_queue, &send_msg_speech, pdMS_TO_TICKS(10));
	if (rent != CIAS_OK)
	{
		CIAS_PRINT_WARN("SEND_PCM_FINISH FIAL2(%d)\n", rent);
	}

#endif
}
