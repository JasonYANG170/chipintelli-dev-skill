#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tvs_api.h"
#include "tvs_platform_impl.h"

#include "tvs_media_recorder_test.h"
#include "tvs_api_config.h"
#include "cias_freertos_debug.h"
#include "cias_slave_message_handle.h"
// 设备最小音量
#define MIN_VOLUME  20

// 设备最大音量
#define MAX_VOLUME  100
bool is_set_volume = false;


void recv_device_volume(int volume)
{
    int cloud_volume = ((volume - MIN_VOLUME) * 100) / (MAX_VOLUME - MIN_VOLUME) ;
    TVS_ADAPTER_PRINTF("cloud_volume = %d\r\n", cloud_volume);
    tvs_platform_adapter_on_volume_changed(cloud_volume);
}


static void set_device_volume(int volume) {
	// TO-DO 设置系统音量
	TVS_ADAPTER_PRINTF("*******set device volume %d*******\n", volume);
	cias_message_send_interface(SEND_LOCAL_VOLUME_CMD, DEF_FILL, 4, (unsigned char *)(&volume)); // add by roy
	is_set_volume = true;
	// chipintelli_media_set_device_volume(volume);
}

static int tvs_soundcard_player_open(int bitrate, int channel) {
	// TO-DO 打开声卡，准备播放PCM，用于TTS播放阶段
	TVS_ADAPTER_PRINTF("*******start playing tts, bitrate %d, channel %d*******\n", bitrate, channel);
	return 0;
}

static void tvs_soundcard_player_close() {
	// TO-DO 播放PCM结束，关闭声卡，用于TTS播放阶段
	TVS_ADAPTER_PRINTF("*******stop playing tts*******\n");
}
	
static int tvs_soundcard_player_write(void *data, unsigned int data_bytes) {
	// TO-DO 播放PCM，用于TTS播放阶段
	TVS_ADAPTER_PRINTF("*******playing, data size %d*******\n", data_bytes);
	return data_bytes;
}

int tvs_media_recorder_impl_open(int bitrate, int channel) {
	// TO-DO 打开声卡，准备录制，用于智能语音对话阶段
	//return tvs_media_recorder_test_open(bitrate, channel);
		printf("*******start recording, bitrate %d, channel %d*******\n", bitrate, channel);
	return 0;
}

void tvs_media_recorder_impl_close() {
	// TO-DO 录制结束，关闭声卡，用于智能语音对话阶段
	printf("*******stop recording*******\n");
	//tvs_media_recorder_test_close();
}

static int tvs_media_recorder_impl_read(void *buffer, unsigned int buffer_len) {
	// TO-DO 录制PCM，填充buffer，用于智能语音对话阶段
	return buffer_len; //tvs_media_recorder_test_record(buffer, buffer_len);
}

static int tvs_platform_impl_soundcard_control(bool open, bool recorder, tvs_soundcard_config* sc_config) {
	if (!recorder) {
		if (!open) {
			tvs_soundcard_player_close();
			return 0;
		} else {
			int bitrate = sc_config == NULL ? 16000 : sc_config->bitrate;
			int channel = sc_config == NULL ? 1 : sc_config->channel;
			return tvs_soundcard_player_open(bitrate, channel);
		}
	} else {
		if (open) {
			int bitrate = sc_config == NULL ? 16000 : sc_config->bitrate;
			int channel = sc_config == NULL ? 1 : sc_config->channel;
			return tvs_media_recorder_impl_open(bitrate, channel);
		} else {
			tvs_media_recorder_impl_close();
			return 0;
		}
	}
}

int tvs_platform_impl_set_current_cloud_volume(int cloud_volume, int cloud_max_value, bool do_init) {
	if (cloud_volume > cloud_max_value) {
		cloud_volume = cloud_max_value;
	}

	int device_volume = ((MAX_VOLUME - MIN_VOLUME) * cloud_volume / cloud_max_value) + MIN_VOLUME;
	set_device_volume(device_volume);

	tvs_platform_adapter_on_volume_changed(cloud_volume);

	return 0;
}

const char* tvs_platform_impl_load_preference(int* preference_size) {
	// TO-DO 将保存的preference加载出来，需要通过malloc申请内存，SDK在回调此函数后，会调用free来执行清除操作
	return "";
}

int tvs_platform_impl_save_preference(const char* preference_buffer, int preference_size) {
	// TO-DO 保存preference，preference_buffer为一个json字符串
	return 0;
}

void tvs_init_platform_adapter_impl(tvs_platform_adaptor* adapter) {
	// adapter实现
	adapter->soundcard_control = tvs_platform_impl_soundcard_control;
	adapter->soundcard_pcm_read = tvs_media_recorder_impl_read;
	adapter->soundcard_pcm_write = tvs_soundcard_player_write;
	adapter->set_volume = tvs_platform_impl_set_current_cloud_volume;
	adapter->load_preference = tvs_platform_impl_load_preference;
	adapter->save_preference = tvs_platform_impl_save_preference;
}

