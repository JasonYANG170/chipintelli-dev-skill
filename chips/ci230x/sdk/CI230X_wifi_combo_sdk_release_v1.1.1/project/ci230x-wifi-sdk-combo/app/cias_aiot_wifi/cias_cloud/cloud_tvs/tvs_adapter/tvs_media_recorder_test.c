#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tvs_media_recorder_test.h"
#include "tvs_api_config.h"


extern unsigned char acweather[];
extern int get_test_audio_size();

static int g_index = 0;

int tvs_media_recorder_test_open(int bitrate, int channel) {
	g_index = 0;
	TVS_ADAPTER_PRINTF("*****record begin!*****\n");
	return 0;
}

int tvs_media_recorder_test_record(char* data_buffer, int data_buffer_len) {
	int total_size = get_test_audio_size();
	
	if (g_index >= total_size) {
		TVS_ADAPTER_PRINTF("*****record end!*****\n");
		return 0;
	}

	char* audio_data = (char*)&acweather[g_index];

	int data_len = total_size - g_index;

	data_len = data_len > data_buffer_len ? data_buffer_len : data_len;

	memcpy(data_buffer, audio_data, data_len);

	TVS_ADAPTER_PRINTF("*****record %d to %d, %d bytes*****\n", g_index, g_index + data_len, data_len);
	
	g_index += data_len;

	return data_len;
}

void tvs_media_recorder_test_close() {
	TVS_ADAPTER_PRINTF("*****record close!*****\n");

	return;
}

