/**
* @file  tvs_audio_provider.c  
* @brief TVS SDK语音数据提供者
* @date  2019-5-10  
* 接收外部传入的PCM数据，进行speex编码，为智能语音模块提供语音数据
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "os_wrapper.h"
#include "tvs_audio_provider.h"
#include "tvs_data_buffer.h"
#include "tvs_threads.h"

#define TVS_LOG_DEBUG_MODULE "AUDPROVIDER"
#include "tvs_log.h"
#include "mongoose.h"
#include "tvs_audio_codec.h"
#include "tvs_speex_codec.h"
#include "tvs_audio_recorder_thread.h"
#include "tvs_config.h"
#include "cias_user_config.h"

#define TVS_PROVIDER_TEST_PRINT(fmt, arg...)    \
	do                                          \
	{                                           \
		TVS_SYS_PRINTF("[TVS]\r\n" fmt, ##arg); \
	} while (0)

#define PROVIDER_DEBUG 0

#define CODEC_BUFFER_SIZE 2 * 1024 //12 * 1024  //change by yjd

static struct mbuf g_codec_fifo;
static bool g_writer_end = true;

TVS_LOCKER_DEFINE

static char *g_pcm_buffer;

static int g_last_remain_size = 0;
static char *g_codec_buffer;
static bool g_codec_opened = false;
static bool g_has_init = false;
static tvs_audio_codec *g_codec = NULL;

static int g_total_write = 0;
static int g_total_enc_src = 0;
static int g_total_enc_result = 0;

static int g_pcm_buffer_size = 0;
static int g_codec_buffer_size = 0;

static tvs_api_callback_on_provider_reader_stop g_callback_on_reader_stop = NULL;

extern void tvs_speech_set_end_time(int time);

void tvs_audio_provider_set_callback(tvs_api_callback_on_provider_reader_stop cb)
{
	g_callback_on_reader_stop = cb;
}

static int do_encode(char *pcm, int pcm_size, char *codec_buffer, int codec_buffer_size)
{
	if (g_codec == NULL || g_codec->do_encode == NULL)
	{
		return -1;
	}

	if (!g_codec_opened)
	{
		return 0;
	}

	return g_codec->do_encode(pcm, pcm_size, codec_buffer, codec_buffer_size);
}

static int encoder_open()
{
	if (g_codec_opened)
	{
		return 0;
	}

	if (g_codec == NULL || g_codec->do_open == NULL)
	{
		return -1;
	}

	g_codec->do_open(g_codec->compress);
	g_codec_opened = true;
	return 0;
}

static void encoder_close()
{
	if (!g_codec_opened)
	{
		return;
	}

	if (g_codec == NULL || g_codec->do_close == NULL)
	{
		return;
	}

	g_codec_opened = false;
	g_codec->do_close();
}

static void clear_pcm_buffer()
{
	g_last_remain_size = 0;
}

static void clear_codec_buffer()
{
	mbuf_free(&g_codec_fifo);
}

static void clear_buffer()
{
	clear_pcm_buffer();
	clear_codec_buffer();
}

int init_buffer()
{
	clear_buffer();
	mbuf_init(&g_codec_fifo, CODEC_BUFFER_SIZE);
	if (&g_codec_fifo)
		return 0;
	return -1;
}

static int read_buffer(struct mbuf *buffer, char *data_buf, int data_buf_len)
{
	if (buffer == NULL || buffer->len < data_buf_len)
	{
		return -1;
	}

	memcpy(data_buf, buffer->buf, data_buf_len);

	mbuf_remove(buffer, data_buf_len);

	return 0;
}

static int get_size(struct mbuf *buffer)
{
	return buffer->len;
}

static int write_buffer(struct mbuf *buffer, char *data_buf, int data_buf_len)
{
	return mbuf_append(buffer, data_buf, data_buf_len);
}

static bool isFull(struct mbuf *buffer, int want_size)
{
	return (buffer->len + want_size > buffer->size);
}

static bool isZeroSize(struct mbuf *buffer)
{
	return buffer->size <= 0;
}

#if 0
static int write_to_encode_ex(char* pcm, int pcm_size) {
	int size = 0;
	int enc_size;
	int total = pcm_size;
	while (1) {
		size = pcm_size > g_pcm_buffer_size ? g_pcm_buffer_size : pcm_size;
		write_buffer(&g_pcm_fifo, pcm, size);
		g_total_write += size;

		while (1) {
			size = read_buffer(&g_pcm_fifo, g_pcm_buffer, g_pcm_buffer_size);
			if (size != 0) {
				// 读取失败，一般是因为数据不足640字节，下次写入后再读
				break;
			}

			enc_size = do_encode(g_pcm_buffer, g_pcm_buffer_size, g_codec_buffer, g_codec_buffer_size);

			if (enc_size <= 0) {
				// 压缩失败
				TVS_LOG_PRINTF("speex encoder is failed\n");
				return -1;
			}

			g_total_enc += enc_size;
			
			if (isFull(&g_codec_fifo, enc_size)) {
				// full
				TVS_LOG_PRINTF("codec fifo is full\n");
				return -100;
			}

			write_buffer(&g_codec_fifo, g_codec_buffer, enc_size);
			//TVS_LOG_PRINTF("write_buffer data %d, size %d, session %d\n", enc_size, g_codec_fifo.len, tvs_core_get_current_session_id());
		}

		pcm_size -= g_pcm_buffer_size;
		pcm += g_pcm_buffer_size;
		

		if (pcm_size <= 0) {
			// 数据已经全部写入
			break;
		}
	}

	return total;
}
#endif

// 2，使用外部编码器，及使用audio压缩后的数据，
/**
 * 将外部压缩过的数据上传
*/
int32_t outside_encode_data_write(int8_t *src_addr, int32_t data_len)
{
	if (isFull(&g_codec_fifo, data_len))
	{
		// full
		TVS_LOG_PRINTF("outside speex codec fifo is full(%d), g_codec_fifo.len = %d, g_codec_fifo.size = %d\n", data_len, g_codec_fifo.len, g_codec_fifo.size);
		return -100;
	}

	if (write_buffer(&g_codec_fifo, src_addr, data_len))
	{
		// g_total_enc_src += enc_per_data;
		g_total_enc_result += data_len;
		return data_len;
	}

	return 0;
}

static int enc_and_write(int enc_per_data, int codec_buffe_size)
{
	// 1，使用內部编码器
	int enc_size = do_encode(g_pcm_buffer, enc_per_data, g_codec_buffer, codec_buffe_size);
	if (enc_size <= 0)
	{
		TVS_LOG_PRINTF("speex encoder is failed\n");
		return -1;
	}

	if (isFull(&g_codec_fifo, enc_size))
	{
		// full
		TVS_LOG_PRINTF("enc speex codec fifo is full\n");
		return -100;
	}

	g_total_enc_src += enc_per_data;
	g_total_enc_result += enc_size;

	// 3，编码后的数据写入网络发送fifo 等待发送
	TVS_LOG_PRINTF("do_encode return is(%d)\n", enc_size);
	write_buffer(&g_codec_fifo, g_codec_buffer, enc_size);

	return enc_size;
}

static int write_to_encode(char *pcm, int pcm_size)
{
	if (pcm_size == 0 || pcm == NULL)
	{
		return 0;
	}
	//printf("*****start write %d bytes******\n", pcm_size);

	g_total_write += pcm_size;

	int tmp_pcm_size = pcm_size;
	int size = g_last_remain_size;
	int write_size = 0;
	int enc_per_data = g_pcm_buffer_size;
	int codec_buffe_size = g_codec_buffer_size;

	int loopCount = pcm_size / enc_per_data;

	//printf("*****start write loop count %d, pcm_size %d, enc_per %d, tmp %d****\n",
	//		loopCount, pcm_size, enc_per_data, tmp_pcm_size);

	for (int i = 0; i < loopCount; i++)
	{
		memcpy(g_pcm_buffer, pcm, enc_per_data);
		pcm += enc_per_data;
		pcm_size -= enc_per_data;

		write_size = enc_and_write(enc_per_data, codec_buffe_size);
		if (write_size <= 0)
		{
			return write_size;
		}
	}

	if (pcm_size > 0 && pcm_size < enc_per_data)
	{
		TVS_PROVIDER_TEST_PRINT("2 save = %d\n", pcm_size);
		memcpy(g_pcm_buffer, pcm, pcm_size);

		write_size = enc_and_write(pcm_size, codec_buffe_size);
		if (write_size <= 0)
		{
			return write_size;
		}
		pcm += pcm_size;
		pcm_size -= pcm_size;
	}

	//printf("*****end write loop******\n");
	TVS_PROVIDER_TEST_PRINT("write end\n");
	return tmp_pcm_size;
}

#include "tvs_api_impl.h"

int tvs_audio_provider_write(char *data, int data_size)
{
	do_lock();

	if (g_writer_end)
	{
		// 调用了writer_end或者没有调用writer_begin
		do_unlock();
		return 0;
	}

	if (isZeroSize(&g_codec_fifo))
	{
		// released，一般是调用了reader_end
		do_unlock();
		TVS_LOG_PRINTF("codec fifo is released, reader is end\n");
		return 0;
	}

	int ret = write_to_encode(data, data_size); //将PCM写入speex

	do_unlock();

	return ret;
}
extern int init_buffer();
int tvs_audio_provider_writer_begin()
{
	int ret = 0;
	// 注意，如果不调用writer end，直接调用writer begin，也应该先把上buffer数据清除掉
	TVS_LOG_PRINTF("call writer begin, session %d\n", tvs_core_get_current_session_id());
	do_lock();

	g_writer_end = false;
	g_total_write = 0;
	g_total_enc_src = 0;
	g_total_enc_result = 0;

	ret = init_buffer();

	// encoder_close();//add by whc
	// encoder_open();
	do_unlock();

	return ret;
}

void tvs_audio_provider_writer_end()
{

	//add by whc
	do_lock();
	if (g_writer_end)
	{
		do_unlock();
		return;
	}
	g_writer_end = true;
	// 只清除PCM Buffer，不清除codec buffer，因为reader还要从codec buffer中读取数据
	clear_pcm_buffer();
	// encoder_close();//add by whc

	TVS_LOG_PRINTF("call writer end, total write %d, total to enc %d, enc result %d\n", g_total_write, g_total_enc_src, g_total_enc_result);
	do_unlock();
}

int tvs_audio_provider_reader_begin(int session)
{
	TVS_LOG_PRINTF("audio reader begin, session %d\r\n", session);
	return 0;
}

static void notify_listener(int session_id, int error)
{
	tvs_audio_recorder_thread_notify_reader_end(session_id, error);
	if (g_callback_on_reader_stop != NULL)
	{
		g_callback_on_reader_stop(session_id, error);
	}
}

void tvs_audio_provider_reader_end(int session_id, int error)
{
	do_lock();
	int session = tvs_core_get_current_session_id();
	if (session != session_id)
	{
		do_unlock();
		TVS_LOG_PRINTF("audio reader end but session is stop, cur %d, target %d\n", session, session_id);
		return;
	}

	TVS_LOG_PRINTF("audio reader end, session %d\n", session_id);

	// reader停止，且session并未过期，需要将codec buffer中的数据清除一遍
	clear_codec_buffer();
	do_unlock();

	tvs_speech_set_end_time(os_wrapper_get_time_ms());
	notify_listener(session_id, error);
}
int start_recorder_state = -1; //add by whc
int tvs_audio_provider_read(int session_id, char *buffer, int buffer_size)
{
	do_lock();

	// TVS_LOG_PRINTF("tvs_audio_provider_read (%d)\n",start_recorder_state);
	start_recorder_state = 1;

	int session = tvs_core_get_current_session_id();
	if (session != session_id)
	{
		do_unlock();
		TVS_LOG_PRINTF("audio reader write but session is stop, cur %d, target %d\n", session, session_id);
		return 0;
	}

	int size = get_size(&g_codec_fifo);

	if (size >= buffer_size) //add by whc  buffer_size = 600
	{
		buffer_size = buffer_size / 2;
	}
	else if (size >= buffer_size / 2) //add by whc 300
	{
		buffer_size = buffer_size / 2;
	}
	else if (size >= buffer_size / 3) //add by whc 100
	{
		buffer_size = buffer_size / 3;
	}
	else if (size >= buffer_size / 6) //add by whc 100
	{
		buffer_size = buffer_size / 6;
	}
	else if (size < buffer_size / 6 && g_writer_end) // 读取最后一点数据  //add by whc
	{

		TVS_LOG_PRINTF("read_buffer want last %d, cur size %d, session id %d\n", size, g_codec_fifo.len, session_id);
		buffer_size = size;
	}
	else // 数据不够，下次再读了
	{
		do_unlock();
		// TVS_LOG_PRINTF("TVS_AUDIO_PROVIDER_ERROR_READ_TOO_FAST (%d)\n",start_recorder_state);
		return TVS_AUDIO_PROVIDER_ERROR_READ_TOO_FAST;
	}
	// TVS_LOG_PRINTF("read_buffer2 size %d, cur g_codec_fifo.len %d\n", buffer_size, g_codec_fifo.len);
	if (g_codec_fifo.len > 1024)
	{
		TVS_LOG_PRINTF("read_buffer2 size %d, cur g_codec_fifo.len %d\n", buffer_size, g_codec_fifo.len);
	}
	else
	{
		CIAS_LOG_DEBUG2("*_", buffer_size, g_codec_fifo.len);
	}

	read_buffer(&g_codec_fifo, buffer, buffer_size);
	do_unlock();

	return buffer_size;
}

int tvs_audio_provider_initialize()
{
	if (g_has_init)
	{
		return 0;
	}

	TVS_LOCKER_INIT

	tvs_speex_codec_init(tvs_config_get_speex_compress());

	g_codec = tvs_get_speex_codec();

	g_pcm_buffer_size = g_codec->src_buffer_size;

	g_pcm_buffer = TVS_MALLOC(g_pcm_buffer_size);

	g_codec_buffer_size = g_codec->codec_buffer_size;

	g_codec_buffer = TVS_MALLOC(g_codec_buffer_size);

	g_has_init = true;
	return 0;
}
