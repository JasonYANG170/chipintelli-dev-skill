
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TVS_LOG_DEBUG_MODULE  "TTS"
#include "tvs_log.h"
#include "tvs_tts_player.h"
#include "tvs_audio_track_interface.h"

#include "tvs_mp3_player.h"

void tvs_speech_save_put_data(void *data, int len);

static bool DEC_MP3_IN_SDK  = false;

static bool g_tts_open = false;

int tvs_tts_player_open() {
	if (g_tts_open) {
		return 0;
	}
	g_tts_open = true;
	if (!DEC_MP3_IN_SDK) {
		return tvs_audio_track_open(TVS_MEDIA_TYPE_MP3, 0, 0);
	} else {
		return tvs_mp3_player_start();
	}
}

void tvs_tts_player_close() {
	if (!g_tts_open) {
		return;
	}

	g_tts_open = false;
	if (!DEC_MP3_IN_SDK) {
		tvs_audio_track_close();
	} else {
		tvs_mp3_player_stop(true);
	}
}

void tvs_tts_player_no_more_data() {
	if (!g_tts_open) {
		return;
	}

	if (!DEC_MP3_IN_SDK) {
		tvs_audio_track_no_more_data();
	} else {
		tvs_mp3_player_stop(false);
	}
}

int tvs_tts_player_write(char* data, int data_size) {
	if (!g_tts_open) {
		return -1;
	}
	if (!DEC_MP3_IN_SDK) {
		return tvs_audio_track_write(data, data_size);
	} else {
		return tvs_mp3_player_fill(data, data_size);
	}
}

void tvs_tts_set_dec_in_sdk(bool in_sdk) {
	DEC_MP3_IN_SDK = in_sdk;
	TVS_LOG_PRINTF("%s %d\n", __func__, in_sdk);
}

