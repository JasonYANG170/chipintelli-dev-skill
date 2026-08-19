/**
* @file  tvs_audio_recorder_thread.c  
* @brief TVS SDK录音线程，用于SDK内部录音的情况
* @date  2019-5-10  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tvs_audio_recorder_thread.h"
#include "tvs_audio_recorder_interface.h"

#include "tvs_data_buffer.h"
#include "tvs_threads.h"

#include "tvs_audio_provider.h"

#define TVS_LOG_DEBUG_MODULE  "REC_THREAD"
#include "tvs_log.h"

#include "tvs_core.h"
#include "tvs_executor_service.h"

#include "tvs_config.h"
#include "tvs_tts_player.h"


static tvs_thread_handle_t* g_recorder_thread = NULL;

TVS_LOCKER_DEFINE

typedef struct {
	int timeout_ms;
	int session_id;
	int bitrate;
	int channels;
} recorder_params;

static void rec_provider_callback_on_reader_stop(int session_id, int error) {
	int cur_session_id = tvs_core_get_current_session_id();

	if (cur_session_id == session_id && !tvs_thread_is_stop(g_recorder_thread)) {
		TVS_LOG_PRINTF("%s, session id %d, cur %d, error %d\n", __func__, session_id, cur_session_id, error);
		tvs_audio_recorder_thread_stop();
	}
}

static void recorder_thread(tvs_thread_handle_t* thread_handle) {
	int total_size = 0;
	
	recorder_params* params = (recorder_params*)tvs_thread_get_param(thread_handle);

	int bitrate = params->bitrate;
	int channels = params->channels;
	int period_count = 512;
	int period_size = 2;

	int buffer_size = period_count * period_size;

	TVS_LOG_PRINTF("start read pcm, session %d, bitrate %d, channels %d, buffer size %d\n", params->session_id, bitrate, channels, buffer_size);
	
	int time = os_wrapper_get_time_ms();
	
	int ret = 0;

	tvs_audio_recorder_open(bitrate, channels,
		period_count, period_size, params->session_id);

	char* buffer = TVS_MALLOC(buffer_size);

	while (tvs_thread_can_loop(thread_handle)) {
		if (os_wrapper_get_time_ms() - time > params->timeout_ms) {
			TVS_LOG_PRINTF("read pcm timeout\n");
			break;
		}

		ret = tvs_audio_recorder_read(buffer, buffer_size, params->session_id);
		//os_wrapper_sleep(10);

		if (ret <= 0) {
			TVS_LOG_PRINTF("read pcm error, ret %d\n", ret);
			break;
		} else {
			total_size += ret;
			ret = tvs_audio_provider_write(buffer, ret);
			if (ret <= 0) {
				TVS_LOG_PRINTF("write audio ret %d\n", ret);
				break;
			}
		}
	}
	TVS_LOG_PRINTF("stop read pcm, total size %d bytes\n", total_size);

	tvs_audio_recorder_close(0, params->session_id);
	
	TVS_FREE(buffer);
	tvs_audio_provider_writer_end();
}

extern bool tvs_is_executor_valid(int cmd, bool check_for_auth);

void tvs_audio_recorder_thread_start(int time_out_ms, int session_id) {
	tvs_tts_player_close();
	
	recorder_params params = {0};
	params.timeout_ms = time_out_ms;
	params.session_id = session_id;
	params.bitrate = tvs_config_get_recorder_bitrate();
	params.channels = tvs_config_get_recorder_channels();

	if (params.timeout_ms <= 10 * 1000) {
		params.timeout_ms = 10 * 1000;
	}
	tvs_thread_handle_t* recorder_thread = g_recorder_thread;
	tvs_thread_try_stop(recorder_thread);
	tvs_thread_start_prepare(recorder_thread, &params, sizeof(recorder_params));
	
	tvs_audio_provider_writer_begin();
	
	tvs_thread_start_now(recorder_thread, "recorder", 4, 256 * 4);
}

void tvs_audio_recorder_thread_notify_reader_end(int session_id, int error) {
	rec_provider_callback_on_reader_stop(session_id, error);
}

void tvs_audio_recorder_thread_stop() {
	if (tvs_thread_is_stop(g_recorder_thread)) {
		return;
	}
	
	tvs_audio_provider_writer_end();
	tvs_thread_try_stop(g_recorder_thread);
}

int tvs_audio_recorder_thread_init() {
	TVS_LOCKER_INIT

	tvs_audio_recorder_init();
	
	if (g_recorder_thread == NULL) {
		g_recorder_thread = tvs_thread_new(recorder_thread, NULL);
	}
	return 0;
}

