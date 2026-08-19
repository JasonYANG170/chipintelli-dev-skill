#ifndef __CI_AGC_H__
#define __CI_AGC_H__
#endif

#include <stdio.h>
#include <stdbool.h>
typedef struct 
{
	float compress_gain;
	float target_db;
	float agc_gain;
	int32_t vox_Th;
	int16_t vox_Wake;
	int16_t vox_Delay;
	bool compute_agc;
	int32_t min_fast_deccent;
	int32_t min_gradient;
	int32_t min_avg_longterm;
	int32_t accelerated_r;
	int32_t agc_log;
}agc_config_t;

typedef struct
{
    void* agcInst;
    float agc_gain;
    int16_t* in_buffer;
    int16_t* out_buffer;
    bool compute_agc;
}AGC;
#ifdef __cplusplus
extern "C" {
#endif
    int ci_agc_version(void);
	void* ci_agc_create(void* module_config);
	int ci_agc_deal(void* handle, short* pcm_in, short* pcm_out, short *trigger);
	bool WebRtcAgc_vox_param_set(int32_t voxTh,  int16_t voxWake, int16_t voxDelay);
#ifdef __cplusplus
}
#endif


