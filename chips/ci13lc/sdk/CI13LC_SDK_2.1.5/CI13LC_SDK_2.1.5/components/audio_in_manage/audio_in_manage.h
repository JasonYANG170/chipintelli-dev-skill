/**
  ******************************************************************************
  * @file    audio_in_manage.h
  * @version V1.0.0
  * @date    2019.04.04
  * @brief 
  ******************************************************************************
  */

#ifndef _VOICE_IN_MANAGE_H_
#define _VOICE_IN_MANAGE_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "debug_time_consuming.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*get_fft_rslt_fun_t)(float* ,int);
typedef void (*get_voice_energy_fun_t)(float);

void is_open_voice_energy(bool gate,get_voice_energy_fun_t fun);
void set_get_fft_rslt_fun_callback(get_fft_rslt_fun_t fun);

  int32_t audio_in_buffer_init(void);

  int32_t audio_in_hw_straight_mode_init(void);

  void audio_in_preprocess_mode_task(void *p);

extern void s_asr_f_vadfe_nn_clear(void);
  // void audio_in_rpmsg_init(void);

#ifdef __cplusplus
}
#endif

#endif
