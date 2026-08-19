#pragma once

#include "FreeRTOS.h"
#include <stdbool.h>
#include "ci_system.h"

/**
 * @brief 音频信息数据结构
 * 
 */
typedef struct
{
    uint32_t samprate;          /*!< 采样率               */
    uint8_t nChans;             /*!< 通道数               */
    int32_t out_min_size;       /*!< 帧长度               */
}audio_format_info_t;

typedef enum {
    SMP_MSG_START_PLAY = 0,     // Start playing. This message is always valid. If the smp is not in the SMP_STATE_IDLE state,the last play work will be stoped,and then begin a play work for this message.
    SMP_MSG_STOP_PLAY,          // Force stop play. This message is valid only when the smp is in the "SMP_STATE_START" or "SMP_STATE_PLAY" state.
}smp_msg_id_t;

typedef void(*SMP_PLAY_END_CALLBACK)(int32_t arg);

/**
 * @brief Initialize the simple mp3 player module.
 * 
 * @return int 1: successed; not 1: failed.
 */
int smp_init(void);

/**
 * @brief Start playing.
 * 
 * @param data_addr uint32_t It's used to specifiy the address of the mp3 data.
 * @param play_end_callback A pointer to a function that will be called when play to end.
 * @return int 1: successed; not 1: failed.
t */
int smp_play(uint32_t data_addr, void* play_end_callback);

// 停止接口
void smp_stop();

void audio_play_hw_pa_da_ctl(FunctionalState cmd,bool is_control_pa);

void audio_play_set_vol_gain(int32_t gain);


