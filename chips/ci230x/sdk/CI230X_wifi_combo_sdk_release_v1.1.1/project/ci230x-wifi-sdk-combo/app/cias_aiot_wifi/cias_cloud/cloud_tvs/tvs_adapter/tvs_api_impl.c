#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tvs_api.h"

#include "tvs_alert_impl.h"
#include "tvs_platform_impl.h"
#include "tvs_mediaplayer_impl.h"
#include "tvs_auth_manager.h"

#include "tvs_api_config.h"
#include "tvs_common_def.h"

//#include "cias_freertos_common.h"
#include "cias_slave_message_handle.h"
#include "cias_freertos_debug.h"
static bool g_tvs_status_idle = true;
int g_session_id = 0;
unsigned char g_tvs_wakeup_msg[] = "智能管家";


extern const char* tvs_get_states_name(tvs_recognize_state state);

extern void init_authorize_on_boot();

extern void tvs_callback_on_asr(const char* asr_text, bool is_end);
extern void tvs_event_callback_on_asr(const char* asr_text);


static void on_tvs_sdk_enter_working_state() {
	// SDK进入工作状态，比如开始语音识别、开始播控等
}

static void on_tvs_sdk_exit_working_state(void* state_param) {
	// SDK回到空闲状态
	tvs_api_state_param* api_state_param = (tvs_api_state_param*)state_param;

	if (api_state_param == NULL) {
		return;
	}

	if (api_state_param->control_type == TVS_CONTROL_PLAY_FINISH) {
		switch(api_state_param->error) {
		case TVS_API_ERROR_NETWORK_ERROR:
			// 播放歌曲列表时，网络异常
			break;
		case TVS_API_ERROR_NO_MORE_MEDIA:
			// 播放歌曲列表时，未下发媒体，一般是歌单播放结束，没有下一首了
			break;
		}
	}
}

bool tvs_sdk_is_idle() {
	// SDK是否处于空闲状态
	return g_tvs_status_idle;
}

static void callback_on_state_changed(tvs_recognize_state last_state, tvs_recognize_state new_state, void* param) {
	// TO-DO 监听SDK状态变换
	g_tvs_status_idle = (new_state == TVS_STATE_IDLE);

	if (last_state == TVS_STATE_IDLE && new_state != TVS_STATE_IDLE) {
		on_tvs_sdk_enter_working_state();
	} else if (last_state != TVS_STATE_IDLE && new_state == TVS_STATE_IDLE) {
		on_tvs_sdk_exit_working_state(param);
	}
}

static void callback_on_terminal_sync(const char* text, const char* token) {
	TVS_ADAPTER_PRINTF("api impl get terminal sync %s -- %s\n", text, token);
	// TO-DO 监听APP端推送
}

static void callback_on_mode_changed(tvs_mode src_mode, tvs_mode dst_mode) {
	TVS_ADAPTER_PRINTF("api impl get mode changed %d -- %d\n", src_mode, dst_mode);
	// TO-DO 监听模式变换
}

static void callback_on_expect_speech() {
	TVS_ADAPTER_PRINTF("api impl on expect speech\n");
    // cias_message_send_interface(MIDEA_ENTER_DIALOGID, DEF_FILL, 0, NULL);
	 // sdio_send_msg_pcm_data(SEND_TTS_DATA_END_CMD, DEF_FILL, 0, NULL);   
    //sdio_send_msg_pcm_data(SEND_EMPTY_CMD, DEF_FILL, 0, NULL);
	// TO-DO 多轮会话，在此函数中拉起下一轮语音识别流程
}

static void callback_on_tvs_control(const char* json) {
	TVS_ADAPTER_PRINTF("api impl get control %s\n", json);
	
	// TO-DO 处理自定义技能
}

static void callback_on_speech_reader_stop(int session_id, int error) {
	if (tvs_api_get_current_session_id() == session_id) {
		
		TVS_ADAPTER_PRINTF("on speech reader stop, session id %d, error %d\n", session_id, error);
		set_audio_state(SEND_PCM_FINISH);
		// SDK发送数据结束,如果本地录音没有结束，需要通知其结束
		switch(error) {
			case TVS_API_AUDIO_PROVIDER_ERROR_NONE:
				// 正常结束
				break;
			case TVS_API_AUDIO_PROVIDER_ERROR_STOP_CAPTURE:
				cias_message_send_interface(SEND_VAD_END, DEF_FILL, 0, NULL);
				// 收到云端VAD End标识
				break;
			case TVS_API_AUDIO_PROVIDER_ERROR_TIME_OUT:
				// 收到SDK Timeout标识，一般是网络原因导致HTTP超时
				break;
			case TVS_API_AUDIO_PROVIDER_ERROR_NETWORK:
				// 网络出错
				break;
			default:
				// 其他原因
				break;
		}
	}
}

int tvs_api_impl_speech_write_audio(const char* audio_data, int data_size) {
	return tvs_api_audio_provider_write(audio_data, data_size);
}

int tvs_api_impl_speech_wakeup() {

    g_session_id = tvs_api_new_session_id();

	int ret = tvs_api_audio_provider_writer_begin();
	if(ret == 0)
	{
		TVS_ADAPTER_PRINTF("tvs_api_audio_provider_writer_begin success\n");
	}
	else
	{
		TVS_ADAPTER_PRINTF("tvs_api_audio_provider_writer_begin failed\n");
	}
	
	// TO-DO 需要将“叮当叮当”替换为接入方自己的热词
	ret = tvs_api_on_voice_wakeup(g_session_id, g_tvs_wakeup_msg, 0, 0);

	if (ret != 0) {
		// SDK启动识别流程出错，此处需要通知录音模块结束
		tvs_api_audio_provider_writer_end();
	}
	return ret;

}


void tvs_api_impl_speech_write_end() {
	tvs_api_audio_provider_writer_end();
}

void tvs_api_impl_speech_init() {
	tvs_api_audio_provider_listen(callback_on_speech_reader_stop);
}



#if 0

TaskHandle_t g_test_task = NULL;
static bool g_test_running = false;
extern unsigned char acweather[];
extern int get_test_audio_size();

void test_reco_task(void* param) {
	while (g_test_running) {
		size_t total = xPortGetTotalHeapSize();
		size_t free = xPortGetFreeHeapSize();
		size_t min = xPortGetMinimumEverFreeHeapSize();
		printf("heap total %u, use %u, free %u, min ever %u\n",
		                 total, total - free, free, min);
		
		vTaskDelay(1000);
		int startTime = xTaskGetTickCount();
		tvs_api_impl_speech_wakeup();

		int media_size = get_test_audio_size();
		printf("**************start %d size***********!\n", media_size);
		char* media = (char*)acweather;

		while (1) {
			if (xTaskGetTickCount() - startTime > 5000) {
				printf("**************time out***********!\n");
				break;
			} else {
				media_size -= 960;
				if (media_size <= 0) {
					media_size = get_test_audio_size();
					media = (char*)acweather;
				} else {
					media += 960;
				}
				tvs_api_impl_speech_write_audio(media, 960);
				vTaskDelay(40);
			}
		}

		tvs_api_impl_speech_write_end();
	}

	g_test_task = NULL;
	vTaskDelete(NULL);
}

void stop_test_reco() {
	g_test_running = false;
}

void start_test_task() {
	if (g_test_task == NULL) {
		g_test_running = true;
		xTaskCreate(test_reco_task, "tvs_test", 1024, NULL, 4, &g_test_task);
	}
}

#endif

void tvs_api_impl_init() {
	g_tvs_status_idle = true;

	// 初始化API，设置回调监听
	tvs_api_callback api_callback = {0};
	api_callback.on_state_changed = callback_on_state_changed;
	api_callback.on_terminal_sync = callback_on_terminal_sync;
	api_callback.on_mode_changed = callback_on_mode_changed;
	api_callback.on_expect_speech = callback_on_expect_speech;
	api_callback.on_recv_tvs_control = callback_on_tvs_control;
	
	tvs_default_config config = {0};
	// 默认环境，量产版本必须默认为正式环境
	config.def_env = TVS_API_ENV_NORMAL;
	// 默认沙箱，量产版本必须默认为false
	config.def_sandbox_open = false;

	config.recorder_bitrate = 16000;
	config.recorder_channels = 1;

	// 设置产品信息
	tvs_product_qua qua = {0};
	qua.version = "1.0.0.1000";
	qua.package_name = "com.tencent.tvs.demo";
	
     // 设置上传音频参数
	tvs_api_init(&api_callback, &config, &qua);

	// 允许输出ASR结果
	extern void tvs_config_print_asr_result(bool enable);
	tvs_config_print_asr_result(true);
	// tvs_api_set_asr_callback(tvs_callback_on_asr,);
	tvs_api_set_asr_callback(tvs_callback_on_asr, tvs_event_callback_on_asr);
	
	// 初始化platform adapter，赋予SDK操作终端的能力
	tvs_platform_adaptor platform_adapter_impl = {0};
	tvs_init_platform_adapter_impl(&platform_adapter_impl);
	tvs_platform_adapter_init(&platform_adapter_impl);

	// 初始化media player adapter，赋予SDK播放网络媒体的能力
	tvs_mediaplayer_adapter media_adapter_impl = {0};
	tvs_init_mediaplayer_adapter_impl(&media_adapter_impl);
	tvs_mediaplayer_adapter_init(&media_adapter_impl);

	// 初始化alert adapter，赋予SDK操作闹钟的能力
	tvs_alert_adapter alert_adapter_impl = {0};
	tvs_init_alert_adater_impl(&alert_adapter_impl);
	tvs_alert_adapter_init(&alert_adapter_impl);
	
	tvs_api_log_enable(true);

	// 授权
	init_authorize_on_boot();
	// 启动SDK
	tvs_api_start();
}

