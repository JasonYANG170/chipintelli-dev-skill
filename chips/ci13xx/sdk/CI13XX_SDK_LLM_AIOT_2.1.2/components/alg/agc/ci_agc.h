#ifndef CI_AGC_H
#define CI_AGC_H
#endif

#include <stdio.h>
#include <stdbool.h>
typedef enum
{
    LOW_SEN = 0,
    MEDIUM_SEN,
    HIGH_SEN,
}VadStartSen;
typedef struct 
{
	float vox_Th;
    float agc_split_boundary;
    float agc_gate_h;
    float agc_gate_l;
    float agc_gate_end;
    int8_t vad_start_sen;
    int8_t vad_timeout_enable;
    int8_t vad_on_max_timeout;
}vox_config_t;

typedef enum {
    VAD_STATE_IDLE,
    VAD_STATE_ACTIVE
} VADState;

#ifdef __cplusplus
extern "C" {
#endif
    int ci_vox_version(void);
	void* ci_vox_create(void* module_config);
	int ci_vox_deal(void* handle, short* pcm_in, short* pcm_out, short *flag);
    void vox_vad_data_clear(void);
#ifdef __cplusplus
}
#endif


