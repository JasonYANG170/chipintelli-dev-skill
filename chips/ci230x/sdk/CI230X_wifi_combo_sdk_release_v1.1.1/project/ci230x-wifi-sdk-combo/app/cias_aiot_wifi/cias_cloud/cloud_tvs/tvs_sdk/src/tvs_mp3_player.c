

#if 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tvs_threads.h"
#include "tvs_audio_track_interface.h"

#include "tvs_data_buffer.h"
#include "tvs_mp3_player.h"

#define TVS_LOG_DEBUG_MODULE  "MP3"
#include "tvs_log.h"

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ALLOW_MONO_STEREO_TRANSITION
#define MINIMP3_ONLY_MP3
#include "minimp3.h"

static tvs_data_buffer* g_mp3_fifo = NULL;

static tvs_thread_handle_t* g_thread = NULL;

#define CODEC_FIFO_SIZE    1024 * 6

#define ERROR_FULL   -100

static bool g_player_started = false;

TVS_LOCKER_DEFINE

static void start_track(int bitrate, int channels) {
	do_lock();
	if (g_player_started) {
		do_unlock();
		return;
	}

	g_player_started = true;
	tvs_audio_track_open(TVS_MEDIA_TYPE_PCM, bitrate, channels);
	
	do_unlock();
}

static void stop_track() {
	do_lock();
	if (!g_player_started) {
		do_unlock();
		return;
	}

	g_player_started = false;
	tvs_audio_track_close(true);
	do_unlock();
}

static void write_track(char* data, int data_size) {
	do_lock();
	if (!g_player_started) {
		do_unlock();
		return;
	}
	// TVS_LOG_PRINTF("2 play pcm %d bytes\n", data_size);
	tvs_audio_track_write((char*)data, data_size);
	do_unlock();
}

void tvs_mp3_player_stop(bool stop_now) {
	TVS_LOG_PRINTF("%s %d\n", __func__, stop_now);

	tvs_data_buffer_set_producer_alive(g_mp3_fifo, false);
	
	if (stop_now) {
		stop_track();
		tvs_thread_try_stop(g_thread);
	}
	tvs_thread_join(g_thread, 5000);

	tvs_data_buffer_release_content(g_mp3_fifo);
}

int tvs_mp3_player_fill(const char* data, int data_size) {
	int n = 6;
	int size = 0;
	bool has_space = false;
	while (n > 0) {
		n--;
		has_space = tvs_data_buffer_has_space(g_mp3_fifo, data_size);
		if (!has_space) {
			if (tvs_data_buffer_is_consumer_alive(g_mp3_fifo)) {
				tvs_data_buffer_producer_wait(g_mp3_fifo, 2000);
			} else {
				break;
			}
		}
	}

	if (!has_space) {
		TVS_LOG_PRINTF("%s full\n", __func__);
		return ERROR_FULL;
	}

	size = tvs_data_buffer_write(g_mp3_fifo, data, data_size);
	//TVS_LOG_PRINTF("%s %d\n", __func__, size);
	return size;

}

static int do_mp3_dec(mp3dec_t* dec, mp3d_sample_t* dec_frame_buffer, int buffer_size, mp3dec_frame_info_t* frame_info, int size) {
	memset(dec_frame_buffer, 0, buffer_size);
	memset(frame_info, 0, sizeof(mp3dec_frame_info_t));
	
	tvs_data_buffer_lock(g_mp3_fifo);
	int samples = mp3dec_decode_frame(dec, (uint8_t*)tvs_data_buffer_read_ptr(g_mp3_fifo), size, dec_frame_buffer, frame_info);
	tvs_data_buffer_remove_not_lock(g_mp3_fifo, frame_info->frame_bytes);
	tvs_data_buffer_unlock(g_mp3_fifo);
	return samples;
}

void tvs_mp3_player_thread(tvs_thread_handle_t* thread) {
	mp3dec_frame_info_t frame_info;
	mp3d_sample_t* dec_frame_buffer = NULL;
	mp3dec_t* dec = NULL;
	int size = 0;
	int samples = 0;
	int dec_size = 0;
	//bool last_frame = false;
	int dec_buffer_size = MINIMP3_MAX_SAMPLES_PER_FRAME * sizeof(mp3d_sample_t);
	
	bool b_start_track = false;
	TVS_LOG_PRINTF("mp3 thread start\n");
	do {
		dec = TVS_MALLOC(sizeof(mp3dec_t));
		if (dec == NULL) {
			TVS_LOG_PRINTF("create mp3dec_t OOM\n");
			break;
		}

		memset(dec, 0, sizeof(mp3dec_t));
		mp3dec_init(dec);

		dec_frame_buffer = TVS_MALLOC(dec_buffer_size);
		if (NULL == dec_frame_buffer) {
			TVS_LOG_PRINTF("codec create mp3 buffer oom \n");
			break;
		}

		while (tvs_thread_can_loop(thread)) {
			size = tvs_data_buffer_get_data_size(g_mp3_fifo);
			if (size >= 3 * 1024) {
				// 缓冲等待有足够的数据
				TVS_LOG_PRINTF("recv enough mp3 data\n");
				break;
			}

			if (tvs_data_buffer_is_producer_alive(g_mp3_fifo)) {
				os_wrapper_sleep(5);
				tvs_data_buffer_consumer_wait(g_mp3_fifo, 200);
				continue;
			} else {
				// 在收到足够数据的时候，Producer就死了，那么就开始消费
				break;
			}
		}

		while (tvs_thread_can_loop(thread)) {
			samples = 0;
			size = tvs_data_buffer_get_data_size(g_mp3_fifo);
			//TVS_LOG_PRINTF("get mp3 data size %d\n", size);
			if (size >= 700) {
				// 有足够数据才进行解码
				samples = do_mp3_dec(dec, dec_frame_buffer, dec_buffer_size, &frame_info, size);
  			} else if (tvs_data_buffer_is_producer_alive(g_mp3_fifo)) {
  				// mp3数量不够，但是Http又没有结束，需要等待生产者填入mp3数据到队列中
				TVS_LOG_PRINTF("wait mp3 data\n");
				os_wrapper_sleep(5);
				tvs_data_buffer_consumer_wait(g_mp3_fifo, 200);
				continue;
  			} else if (size <= 0) {
				// Http已经结束，mp3数据也消费完毕，解码完毕
				TVS_LOG_PRINTF("decode mp3 end 1\n");
				break;
			} else {
				// Http已经结束，消费队列中剩余的一点mp3
				samples = do_mp3_dec(dec, dec_frame_buffer, dec_buffer_size, &frame_info, size);
				
				//TVS_LOG_PRINTF("decode last mp3 %d bytes, res %d samples\n", size, samples);
				//last_frame = true;
				if (samples <= 0) {
					// 解码失败，退出
					TVS_LOG_PRINTF("decode mp3 end 2\n");
					break;
				}
			}

			tvs_data_buffer_notify_producer(g_mp3_fifo);

			// send to play
			dec_size = samples * sizeof(mp3d_sample_t);
			
			if (!b_start_track && dec_size > 0) {
				TVS_LOG_PRINTF("start play pcm, bitrate %d, channels %d\n", frame_info.hz, frame_info.channels);
				start_track(frame_info.hz, frame_info.channels);
				b_start_track = true;
			}
			

			if (b_start_track && dec_size > 0) {
				write_track((char*)dec_frame_buffer, dec_size);
			}
		}

	} while (0);

	if (b_start_track) {
		stop_track(true);
	}
	
	if (dec != NULL) {
		TVS_FREE(dec);
		dec = NULL;
	}

	if (dec_frame_buffer != NULL) {
		TVS_FREE(dec_frame_buffer);
		dec_frame_buffer = NULL;
	}

	TVS_LOG_PRINTF("mp3 thread end\n");
}

int tvs_mp3_player_start() {
	TVS_LOG_PRINTF("%s\n", __func__);

	if (g_mp3_fifo == NULL) {
		g_mp3_fifo = tvs_data_buffer_new(CODEC_FIFO_SIZE);
	}
	
	tvs_thread_try_stop(g_thread);
	tvs_thread_start_prepare(g_thread, NULL, 0);

	tvs_data_buffer_init_content(g_mp3_fifo);
	tvs_data_buffer_set_consumer_alive(g_mp3_fifo, true);
	tvs_data_buffer_set_producer_alive(g_mp3_fifo, true);

	tvs_thread_start_now(g_thread, "mp3", 4, 1024);
	return 0;
}

int tvs_mp3_player_initialize() {
	TVS_LOCKER_INIT
	
	if (g_thread == NULL) {
		g_thread = tvs_thread_new(tvs_mp3_player_thread, NULL);
	}

	return 0;
}

#endif
