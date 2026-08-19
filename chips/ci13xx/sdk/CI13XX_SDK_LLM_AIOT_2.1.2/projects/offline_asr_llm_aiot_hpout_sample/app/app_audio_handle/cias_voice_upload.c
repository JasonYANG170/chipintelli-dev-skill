/*
 * @FileName::
 * @Author:
 * @Date: 2022-03-08 16:31:30
 * @LastEditTime: 2025-03-16 21:42:58
 * @Description: 音频上传
 */
#include "ci_log.h"
#include "status_share.h"
#include "cias_network_msg_protocol.h"
#include "cias_network_msg_send_task.h"
#include "cias_uart_protocol.h"
#include "cias_voice_upload.h"
#include "FreeRTOS.h"
#include "audio_play_decoder.h"
#include "timers.h"
#include "cias_aiot_protocol.h"
#include "user_config.h"
#include "codec_manager.h"

CiasAiotRunParamTypedef gCiasAiotRunParam =
    {
        .customer_wakeup_on_callback = NULL,
        .customer_wakeup_exit_callback = NULL,
        .customer_vad_start_callback = NULL,
        .customer_vad_end_callback = NULL,
        .cloud_ans_count_timer = NULL,
        .pcm_compress_stream_buffer = NULL,
        .pcm_debug_stream_buffer = NULL,
        .pcm_play_data_stream_buffer = NULL,
        .local_asr_finish_flag = false,
        .mp3_play_finish_flag = true,
        .is_always_hpout_flag = false,
};
CiasAiotFuncParamTypedef gCiasAiotFuncParam =
    {
        .vad_end_mute_param_index = VOX_VAD_END_CONFIDENCE_DEFAULT,
        .wake_up_continue_timeout = EXIT_WAKEUP_TIME / 1000, // 以S为单位
        .audio_play_mode = AUDIO_PLAY_MODE,
        .upload_audio_by_denoise = UPLOAD_NNDENOISE_AUDIO_DATA_ENABLE,
        .is_play_exit_wakeup_voice = true,
        .is_play_enter_wakeup_voice = true,
#if IIS_CHANNEL_ENG_CALC_EANBLE
        .upload_factory_test_real_val_flag = CIAS_UPLOD_FACTORY_TEST_REAL_VAL,
        .micl_db_thr_val = CIAS_HAVE_AUDIO_ENG_MICL,
        .micr_db_thr_val = CIAS_HAVE_AUDIO_ENG_MICR,
        .refl_db_thr_val = CIAS_HAVE_AUDIO_ENG_REFL,
        .refr_db_thr_val = CIAS_HAVE_AUDIO_ENG_REFR,
#endif
};
uint16_t key_upload_audio_count = 0;
extern bool key_is_busying_flag; // 通过按键控制音频上传标志

extern int get_heap_bytes_remaining_size(void);

typedef enum
{
    ESTVAD_IDLE = 0, /*!<vad的状态处于IDLE状态    */
    ESTVAD_START,    /*!<vad的状态处于START状态   */
    ESTVAD_ON,       /*!<vad的状态处于ON状态      */
    ESTVAD_END,      /*!<vad的状态处于END状态     */
} estvad_state_t;

// speex

extern volatile bool asr_reseult_wakup_flag;

bool cias_aiot_param_refresh(void)
{
}
bool wifi_net_state_check(void)
{
#if CHECK_NET_WORK_STATE_ENABLE
    if (wifi_current_state_get() != CLOUD_CONNECTED_STATE)
    {
        mprintf("wifi disconnected ......\r\n");
        prompt_play_by_cmd_id(2009, -1, NULL, false); // 设备网络异常,请重新配网
        cias_aiot_vad_end_handle(0x01);
        cias_send_cmd(SKIP_INVAILD_SPEAK, DEF_FILL); // 发送无效音频
        return false;
    }
    else
    {
        return true;
    }
#endif
}

void asr_wakeup_on_handle(void)
{
    gCiasAiotRunParam.is_wake_up_flag = true;
    cias_send_cmd(WAKE_UP, DEF_FILL);
}
// 退出唤醒
void asr_wakeup_exit_handle(void)
{
    gCiasAiotRunParam.is_wake_up_flag = false;
    cias_send_cmd(EXIT_WAKE_UP, DEF_FILL);
}
void cias_aiot_vad_start_handle(int cmd)
{
    
}
void cias_aiot_vad_end_handle(int type)
{
    
}

// vad状态检测任务
void vad_state_handle_task(void *p_arg)
{
   
}
// pcm采音调试
void pcm_debug_task(void *p_arg)
{

}
// 录音数据上传任务
void record_data_upload_task(void *p_arg)
{
    
}
// speex编码任务
void speex_encode_task(void)
{
    
}
// speex音频任务处理初始化
bool audio_speex_task_init(void)
{
    
}
// opus编码任务
void opus_encode_task(void)
{
}

// opus音频任务处理初始化
bool audio_opus_task_init(void)
{
}
void cloud_ans_count_timer_callback(TimerHandle_t xTimer)
{

}
bool voice_upload_task_init(void)
{
    return true;
}
