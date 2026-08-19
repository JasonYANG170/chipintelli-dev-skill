#ifndef __TVS_API_CONFIG_H__
#define __TVS_API_CONFIG_H__

#include "cias_common.h"
#include "cias_log.h"

// 1 -- 访客授权;  0 -- 设备授权
#define CONFIG_USE_GUEST_AUTHORIZE   0    

// 1 -- TTS阶段，在SDK内部解码，并通过回调tvs_platform_adaptor_soundcard_pcm_write播放PCM
// 0 -- TTS阶段，SDK通过回调tvs_mediaplayer_adapter_tts_data, 将MP3数据传出，由上层解码并播放
#define CONFIG_DECODE_TTS_IN_SDK     0

#ifndef PLATFORM_RT_THREAD
#define TVS_FREE             free
#else     

#define TVS_FREE  rt_free
#endif

#define TVS_ADAPTER_PRINTF   CIAS_PRINT_DEBUG

#endif