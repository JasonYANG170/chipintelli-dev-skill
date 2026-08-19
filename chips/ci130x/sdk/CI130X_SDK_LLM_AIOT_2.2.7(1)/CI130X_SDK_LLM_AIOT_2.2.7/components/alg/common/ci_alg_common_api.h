/*该文件提取算法公共api*/
#ifndef __CI_ALG_COMMON_API__
#define __CI_ALG_COMMON_API__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "sdk_default_config.h"

#include "asr_process_callback.h"
#include "romlib_runtime.h"
#include "asr_api.h"
#include "ci_log.h"
#include "sdk_default_config.h"
#include "debug_time_consuming.h"

#include "alc_auto_switch.h"
#include "ci_denoise.h"
#include "ci_audio_wrapfft.h"
#include "ci_basic_alg.h"
#include "ci_bf.h"
#include "ci_dereverb.h"
#include "ci_adapt_aec.h"
#include "ci_doa.h"
#include "ci_ai_doa.h"
#include "ci_doa_apply.h"
#include "ci_pwk.h"
#include "asr_top_manage.h"
#if USE_DENOISE_NN_RTC
#include "ai_denoise_api_rtc.h"
#include "ci_drc.h"
#include "ci_eq.h"
#else
#include "ai_denoise_api.h"
#endif

#include "ci_log_config.h"
#include "ci130x_audio_pre_rslt_out.h"

#include "alg_preprocess.h"
#include "status_share.h"
#include "ci_alg_malloc.h"
#include "nn_and_flash_manage.h"
#if USE_SED
#include "sed_manage.h"
#include "serial_asr_flow.h"
#endif
#include "user_config.h"
#include "remote_api_for_bnpu.h"
#include "status_share.h"

void alg_calc_audio_eng_f(float *audio_fft_val, int audio_frame_len, float *dst_db);     //计算音频能量
void alg_calc_audio_eng_s(short *audio_fft_val, int audio_frame_len, float *dst_db);
#endif  ///