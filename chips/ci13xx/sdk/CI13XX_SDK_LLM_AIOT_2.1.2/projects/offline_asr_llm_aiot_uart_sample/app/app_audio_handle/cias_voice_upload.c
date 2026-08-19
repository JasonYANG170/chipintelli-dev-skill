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
#include "user_config.h"
#if !SIMPLE_AUDIO_PLAYER_ENABLE
#include "audio_play_decoder.h"
#endif
#include "timers.h"
#include "cias_aiot_protocol.h"
#if AUDIO_COMPRESS_G722_ENABLE
#include "g722_enc_dec.h"
#endif
#if AUDIO_COMPRESS_SPEEX_ENABLE
#include "ci110x_speex.h"
#include "sb_celp.h"
#include "nb_celp.h"
extern SpeexMode *cias_speex_wb_mode;
extern const SpeexSBMode sb_wb_mode;
extern void *sb_encoder_init(const SpeexMode *m);
extern void sb_encoder_destroy(void *state);
extern int sb_decode(void *state, SpeexBits *bits, void *vout);
extern int sb_encoder_ctl(void *state, int request, void *ptr);
extern int sb_decoder_ctl(void *state, int request, void *ptr);
extern int sb_encode(void *state, void *vin, SpeexBits *bits);
extern void *sb_decoder_init(const SpeexMode *m);
extern void sb_decoder_destroy(void *state);
static ci_speex_t *ci_speex_hander = NULL;
#elif AUDIO_COMPRESS_OPUS_ENABLE
#include "opus.h"
#include "debug.h"
#include "opus_types.h"
#include "opus_private.h"
#include "opus_defines.h"
#endif

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
};
CiasAiotFuncParamTypedef gCiasAiotFuncParam =
{
        .vad_filter_frame = VAD_ON_MIN_NUM,
        .vad_end_mute_param_index = VOX_VAD_END_CONFIDENCE_DEFAULT,
        .vad_start_max_timeout = VAD_FORCE_OVER_NUM_TIME,
        .wake_up_continue_timeout = EXIT_WAKEUP_TIME / 1000, // 以S为单位
        .interaction_multi_round = CUR_INTERACTION_MULTI_ROUND_ENABLE,
        .upload_play_full_duplex = UPLOAD_PLAY_FULL_DUPLEX_ENABLE,
        .vad_start_stop_paly = VAD_START_STOP_PLAY_ENABLE,
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
    // 录音参数初始化
    gCiasAiotRunParam.wake_up_pcm_frame_count = 0;
    gCiasAiotRunParam.upload_pcm_frame_count = 0;
    gCiasAiotRunParam.speex_compress_frame_count = 0;
    gCiasAiotRunParam.vad_start_pcm_frame_count = 0;
    if (!gCiasAiotFuncParam.interaction_multi_round) // 单轮交互
    {
        gCiasAiotRunParam.is_wake_up_flag = false;
    }
    gCiasAiotRunParam.speex_compress_is_busy = false;
    gCiasAiotRunParam.pcm_upload_is_busy = false;
    gCiasAiotRunParam.compress_pcm_to_wifi_flag = false;
    gCiasAiotRunParam.upload_pcm_to_wifi_flag = false;
    gCiasAiotRunParam.wait_play_end_flag = false;
    // 播放参数初始化
    gCiasAiotRunParam.stop_collect_pcm_flag = false;
    ciss_set(CI_SS_VOX_WORK_STATE, 1); // 开启vox vad计算
    ciss_set(CI_SS_VOX_VAD_STATE, CI_SS_VAD_IDLE);
// #if CLOUD_ANS_TIME_OUT_ENEABLE
//     xTimerStop(gCiasAiotRunParam.cloud_ans_count_timer, 0);
// #endif
#if !UPLOAD_PCM_DATA_ENABLE
    xStreamBufferReset(gCiasAiotRunParam.pcm_compress_stream_buffer);
#if !AUDIO_COMPRESS_RECORD_DISABLE
    xQueueReset(gCiasAiotRunParam.pcm_upload_queue);
#endif
#endif
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
#if CHECK_NET_WORK_STATE_ENABLE
    if (wifi_current_state_get() != CLOUD_CONNECTED_STATE)
    {
        prompt_play_by_voice_id(2005, default_play_done_callback, true);
        return;
    }
#endif
    ciss_set(CI_SS_VOX_VAD_CLEAR, 1); // 唤醒之后必须清除状态，解决唤醒后马上路录音，vad 状态还保持vad on状态，新的vad start无法起来的问题
    cias_aiot_param_refresh();

    gCiasAiotRunParam.is_vad_force_on_flag = false;
    gCiasAiotRunParam.is_wake_up_flag = true;
    gCiasAiotRunParam.is_vad_on_flag = false;
    gCiasAiotRunParam.cloud_parse_is_busy_flag = false; // 云端解析中
    gCiasAiotRunParam.cloud_ans_time_out_flag = false;  // 云端响应超时
    gCiasAiotRunParam.request_play_data_flag = false;
    gCiasAiotRunParam.play_cloud_data_flag = false;
    gCiasAiotRunParam.rcv_cloud_play_data_flag = false;
    gCiasAiotRunParam.cloud_play_data_total_len = 0;
#if CLOUD_ANS_TIME_OUT_ENEABLE
    xTimerStop(gCiasAiotRunParam.cloud_ans_count_timer, 0);
#endif
#if !UPLOAD_PCM_DATA_ENABLE
    cias_send_cmd(WAKE_UP, DEF_FILL);
    if (gCiasAiotRunParam.customer_wakeup_on_callback)
    {
        gCiasAiotRunParam.customer_wakeup_on_callback();
    }
#endif // 上传数据（打包协议）
    mprintf("upload speex first data...\r\n");
}
// 退出唤醒
void asr_wakeup_exit_handle(void)
{
    if(!gCiasAiotRunParam.is_vad_force_on_flag)
    {
        gCiasAiotRunParam.is_wake_up_flag = false;
    }
    
#if !UPLOAD_PCM_DATA_ENABLE
    cias_send_cmd(EXIT_WAKE_UP, DEF_FILL);
    if (gCiasAiotRunParam.customer_wakeup_exit_callback)
    {
        gCiasAiotRunParam.customer_wakeup_exit_callback();
    }
#endif // 上传数据（打包协议）
}
int8_t speex_src_temp_buf[PCM_ALG_FRAME_LEN] = {0};
void cias_aiot_vad_start_handle(int cmd)
{
    uint16_t wakeup_pcm_frame_len = 0;
    uint16_t rollback_pcm_frame_len = 0;
    ciss_set(CI_SS_VOX_VAD_STATE, CI_SS_VAD_ON);
    
    wakeup_pcm_frame_len = xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_compress_stream_buffer) / PCM_ALG_FRAME_LEN;
    rollback_pcm_frame_len = (wakeup_pcm_frame_len >= PCM_ALG_ROOLBACK_FRAME_LEN) ? (wakeup_pcm_frame_len - PCM_ALG_ROOLBACK_FRAME_LEN) : 0;
    mprintf("rollback_pcm_frame_len = %d\r\n", rollback_pcm_frame_len);
    for (int i = 0; i < rollback_pcm_frame_len; i++)
    {
        int rx_size = xStreamBufferReceive(gCiasAiotRunParam.pcm_compress_stream_buffer, speex_src_temp_buf, PCM_ALG_FRAME_LEN, pdMS_TO_TICKS(5));
        if (rx_size != PCM_ALG_FRAME_LEN)
        {
            mprintf("roll back pcm_compress_stream_buf rcv error2\r\n");
        }
        gCiasAiotRunParam.wake_up_pcm_frame_count--;
    }
    gCiasAiotRunParam.is_vad_on_flag = true;
    gCiasAiotRunParam.need_send_vad_start_flag = true;
    gCiasAiotRunParam.compress_pcm_to_wifi_flag = true;
    if (!gCiasAiotFuncParam.upload_play_full_duplex || gCiasAiotFuncParam.vad_start_stop_paly)
    {
        gCiasAiotRunParam.request_play_data_flag = false;
    }

#if UPLOAD_PCM_DATA_ENABLE && UPLOAD_PCM_VAD_TAG_ENABLE
    short vad_end_tag = 32767;
    voice_data_packet_and_send(PCM_MIDDLE, &vad_end_tag, 2);
#endif
#if !UPLOAD_PCM_DATA_ENABLE
    cias_send_cmd(cmd, DEF_FILL);
    gCiasAiotRunParam.cloud_ans_time_out_flag = false; // 清除超时标志
#endif
    if (gCiasAiotRunParam.customer_vad_start_callback)
    {
        gCiasAiotRunParam.customer_vad_start_callback();
    }
}
void cias_aiot_vad_end_handle(int type)
{
    gCiasAiotRunParam.stop_collect_pcm_flag = true;
    ciss_set(CI_SS_VOX_VAD_STATE, CI_SS_VAD_IDLE);
    if (!type) // 强制更新参数，直接返回
    {
#if CLOUD_ANS_TIME_OUT_ENEABLE
        xTimerStart(gCiasAiotRunParam.cloud_ans_count_timer, 0); // 开始计时
#endif
        gCiasAiotRunParam.cloud_parse_is_busy_flag = true;
    }
    else
    {
#if CLOUD_ANS_TIME_OUT_ENEABLE
        xTimerStop(gCiasAiotRunParam.cloud_ans_count_timer, 0);//本地识别到则停止云端超时
#endif
    }
    if (!type) // 等待数据传输完成再退出
    {
        uint16_t tryCount = 0;
        bool ret = false;
        while (1)
        {
            if(gCiasAiotRunParam.local_asr_finish_flag) //本地识别完成直接退出
            {
                gCiasAiotRunParam.local_asr_finish_flag = false;
                mprintf("===local asr finish\r\n");
                break;
            }
            if (tryCount++ < 30) // 发送检测超时3S
            {
                ret = true;
#if !AUDIO_COMPRESS_RECORD_DISABLE
                // 清必须的状态
                mprintf("xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_compress_stream_buffer) = %d\r\n", xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_compress_stream_buffer));
                if (xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_compress_stream_buffer) > COMPRESS_NEED_PCM_LEN * 2)
                {
                    ret = false;
                    mprintf("wait data compress over1\r\n");
                }
                if (gCiasAiotRunParam.speex_compress_is_busy)
                {
                    ret = false;
                    mprintf("wait compress send over2\r\n");
                }
#endif
                if (gCiasAiotRunParam.pcm_upload_is_busy)
                {
                    ret = false;
                    mprintf("wait uplod send over2\r\n");
                }
                if (ret)
                {
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            else
            {
                break;
            }
        }
        mprintf("\r\n==== wake up frame count= %d)\r\n", gCiasAiotRunParam.wake_up_pcm_frame_count);
        mprintf("\r\n==== compress frame count= %d)\r\n", gCiasAiotRunParam.speex_compress_frame_count);
        mprintf("\r\n==== upload frame count= %d)\r\n", gCiasAiotRunParam.upload_pcm_frame_count);
    }
    cias_aiot_param_refresh();
#if 0
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_PLAY;
    send_msg.msg_data.play_data.play_index = 10003; // 嗡鸣器<beep>
    send_msg_to_sys_task(&send_msg, NULL);
#endif
#if !UPLOAD_PCM_DATA_ENABLE
    cias_send_cmd(PCM_FINISH, DEF_FILL);
    if (gCiasAiotRunParam.customer_vad_end_callback)
    {
        gCiasAiotRunParam.customer_vad_end_callback();
    }
#endif
    
}

// vad状态检测任务
void vad_state_handle_task(void *p_arg)
{
    while (1)
    {
#if CHECK_NET_WORK_STATE_ENABLE
        if (wifi_current_state_get() != CLOUD_CONNECTED_STATE)
        {
            vTaskDelay(pdMS_TO_TICKS(5));
            continue;
        }
#endif
#if USE_CWSL
        if (ciss_get(CI_SS_CWSL_IN_REG)) // 自学习学习状态不进行vad检测
        {
            vTaskDelay(pdMS_TO_TICKS(5));
            continue;
        }
#endif
        if (gCiasAiotRunParam.key_is_busy_flag) // 按键处理中
        {
            vTaskDelay(pdMS_TO_TICKS(5));
            continue;
        }
        if (!gCiasAiotRunParam.is_vad_on_flag)
        {
            if (!gCiasAiotRunParam.is_wake_up_flag)
            {
                vTaskDelay(pdMS_TO_TICKS(5));
                continue;
            }
        }
        if (!gCiasAiotFuncParam.upload_play_full_duplex)
        {
            if (CI_SS_PLAY_STATE_PLAYING == ciss_get(CI_SS_PLAY_STATE))
            {
                vTaskDelay(pdMS_TO_TICKS(5));
                continue;
            }
        }
        if(!gCiasAiotRunParam.is_vad_force_on_flag)
        {
            if (ciss_get(CI_SS_VOX_VAD_STATE) == CI_SS_VAD_START) // 检测到vox vad start
            {
    #if CLOUD_ANS_TIME_OUT_ENEABLE
                if (gCiasAiotRunParam.cloud_parse_is_busy_flag) // 云端处理中
                {
                    ciss_set(CI_SS_VOX_VAD_STATE, CI_SS_VAD_IDLE);
                    vTaskDelay(pdMS_TO_TICKS(5));
                    mprintf("gCiasAiotRunParam.cloud_parse_is_busy_flag is busying...\r\n");
                    // if (get_wakeup_state() == SYS_STATE_WAKEUP) // 唤醒中才播放
                    //     prompt_play_by_voice_id(2050, default_play_done_callback, true);
                    continue;
                }
    #endif
                mprintf("----vad start----\r\n");
                cias_aiot_vad_start_handle(VAD_START);
            }
            if ((ciss_get(CI_SS_VOX_VAD_STATE) == CI_SS_VAD_END) && gCiasAiotRunParam.is_vad_on_flag) // nn vad end检测
            {
                gCiasAiotRunParam.is_vad_on_flag = false;
    #if UPLOAD_PCM_DATA_ENABLE && UPLOAD_PCM_VAD_TAG_ENABLE
                short vad_end_tag = -32767;
                voice_data_packet_and_send(PCM_MIDDLE, &vad_end_tag, 2);
    #endif
                ciss_set(CI_SS_VOX_VAD_STATE, CI_SS_VAD_IDLE);
                if (!asr_reseult_wakup_flag)
                {
                    mprintf("\r\n---vad end---\r\n");
                    cias_aiot_vad_end_handle(0x0);
                }
            }
            if (1 == ciss_get(CI_SS_VOX_VAD_FORCE_END) && gCiasAiotRunParam.is_vad_on_flag) // 检测到强制结束
            {
                if (gCiasAiotRunParam.vad_start_pcm_frame_count >= gCiasAiotFuncParam.vad_start_max_timeout * 1000 / 16) // vad on持续的帧数
                {
                    gCiasAiotRunParam.is_vad_on_flag = false;
                    mprintf("\r\n-----force vad end-----\r\n");
                    // short vad_end_tag = -28000;
                    // voice_data_packet_and_send(PCM_MIDDLE, &vad_end_tag, 2);
                    // ciss_set(CI_SS_VOX_VAD_CLEAR, 1);
                    ciss_set(CI_SS_VOX_VAD_STATE, CI_SS_VAD_IDLE);
                    ciss_set(CI_SS_VOX_VAD_FORCE_END, 0);
                    cias_aiot_vad_end_handle(0x0);
                }
            }
        }
        
#if 0
        if (!gCiasAiotRunParam.is_vad_force_on_flag) // 强制录音不用检测vad end
        {
            if ((CI_SS_VAD_START == ciss_get(CI_SS_VOX_VAD_STATE)) || (CI_SS_VAD_ON == ciss_get(CI_SS_VOX_VAD_STATE)) ||
                (1 == ciss_get(CI_SS_VOX_VAD_FORCE_END))) // 超时机制检测
            {
                if (gCiasAiotRunParam.vad_start_pcm_frame_count >= gCiasAiotFuncParam.vad_start_max_timeout * 1000 / 16)
                {
                    gCiasAiotRunParam.is_vad_on_flag = false;
                    mprintf("\r\n-----force vad end-----\r\n");
                    // short vad_end_tag = -28000;
                    // voice_data_packet_and_send(PCM_MIDDLE, &vad_end_tag, 2);
                    //ciss_set(CI_SS_VOX_VAD_CLEAR, 1);
                    ciss_set(CI_SS_VOX_VAD_STATE, CI_SS_VAD_IDLE);
                    ciss_set(CI_SS_VOX_VAD_FORCE_END, 0);
                    cias_aiot_vad_end_handle(0x0);
                }
            }
        }
#endif
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
// 录音数据上传任务
void record_data_upload_task(void *p_arg)
{
    int ret = 0;
    uint8_t index = 0;
    int8_t upload_src_data_buf[COMPRESS_UPLOAD_QUEUE_TIEM_SIZE] = {0};
    while (1)
    {
#if !AUDIO_COMPRESS_RECORD_DISABLE
#if UPLOAD_PCM_DATA_ENABLE
        if (1)
#else
        if (gCiasAiotRunParam.upload_pcm_to_wifi_flag)
#endif
        {
            ret = xQueueReceive(gCiasAiotRunParam.pcm_upload_queue, upload_src_data_buf, pdMS_TO_TICKS(200));
            if (pdTRUE == ret)
            {
                gCiasAiotRunParam.pcm_upload_is_busy = true;
                wifi_net_state_check();
                voice_data_packet_and_send(PCM_MIDDLE, upload_src_data_buf, COMPRESS_UPLOAD_QUEUE_TIEM_SIZE); // 上传数据（打包协议） //带长度字节
                mprintf(".");
                gCiasAiotRunParam.upload_pcm_frame_count++;
                if (gCiasAiotRunParam.upload_pcm_frame_count % 5 == 0)
                {
                    update_awake_time();
                }
                gCiasAiotRunParam.pcm_upload_is_busy = false;
                memset(upload_src_data_buf, 0, sizeof(upload_src_data_buf));
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(5));
        }
#else // 不需要压缩，直接上传pcm数据
        if (gCiasAiotRunParam.compress_pcm_to_wifi_flag)
        {
            int stream_aviable_len = xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_compress_stream_buffer);
            if (stream_aviable_len > 0) //&& network_dma_trans_done
            {
                memset(upload_src_data_buf, 0, sizeof(upload_src_data_buf));
                gCiasAiotRunParam.pcm_upload_is_busy = true;
                int rx_size = xStreamBufferReceive(gCiasAiotRunParam.pcm_compress_stream_buffer, upload_src_data_buf, COMPRESS_UPLOAD_QUEUE_TIEM_SIZE, portMAX_DELAY);
                if (rx_size != COMPRESS_UPLOAD_QUEUE_TIEM_SIZE)
                {
                    mprintf("pcm_compress_stream_buffer rcv error\r\n");
                }
                else
                {
                    for (int i = 0; i < 4; i++)
                    {
                        voice_data_packet_and_send(PCM_MIDDLE, upload_src_data_buf + i * (COMPRESS_UPLOAD_QUEUE_TIEM_SIZE / 4), COMPRESS_UPLOAD_QUEUE_TIEM_SIZE / 4);
                        // network_send(upload_src_data_buf + i*(COMPRESS_UPLOAD_QUEUE_TIEM_SIZE/8), COMPRESS_UPLOAD_QUEUE_TIEM_SIZE/8);
                    }
                }
                gCiasAiotRunParam.pcm_upload_is_busy = false;
                mprintf(".");
                gCiasAiotRunParam.upload_pcm_frame_count++;
                if (gCiasAiotRunParam.upload_pcm_frame_count % 5 == 0)
                {
                    update_awake_time();
                }
                memset(upload_src_data_buf, 0, sizeof(upload_src_data_buf));
            }
            else
            {
                vTaskDelay(pdMS_TO_TICKS(5));
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(5));
        }
#endif
    }
}
#if AUDIO_COMPRESS_SPEEX_ENABLE
// speex编码任务
void speex_encode_task(void)
{
    uint32_t encode_len = 0;
    uint32_t stream_aviable_len = 0;
    uint32_t rx_size = 0;
    int8_t speex_encode_buf[COMPRESS_UPLOAD_QUEUE_TIEM_SIZE] = {0};
    int8_t speex_rx_temp[COMPRESS_NEED_PCM_LEN * 2] = {0};
    while (1)
    {
#if UPLOAD_PCM_DATA_ENABLE
        if (1)
#else
        if (gCiasAiotRunParam.compress_pcm_to_wifi_flag)
#endif
        {
            // mprintf("gCiasAiotRunParam.compress_pcm_to_wifi_flag----\r\n");
            stream_aviable_len = xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_compress_stream_buffer);
            if (stream_aviable_len >= COMPRESS_NEED_PCM_LEN * 2 && !gCiasAiotRunParam.speex_compress_is_busy) // NB_FRAME_SIZE * 2 * 2
            {
                memset(speex_rx_temp, 0, sizeof(speex_rx_temp));
                rx_size = xStreamBufferReceive(gCiasAiotRunParam.pcm_compress_stream_buffer, speex_rx_temp, COMPRESS_NEED_PCM_LEN * 2, portMAX_DELAY);
                if (rx_size != COMPRESS_NEED_PCM_LEN * 2)
                {
                    mprintf("gCiasAiotRunParam.pcm_compress_stream_buffer rcv error\r\n");
                }
                else
                {
                    for (int k = 0; k < NB_FRAME_SIZE / 160; k++)
                    {
#if UPLOAD_PCM_DATA_ENABLE
                        voice_data_packet_and_send(PCM_MIDDLE, speex_rx_temp, NETWORK_SEND_BUFF_MAX_SIZE);
                        mprintf("#");
#else
                        memset(speex_encode_buf, 0, sizeof(speex_encode_buf));
                        gCiasAiotRunParam.speex_compress_is_busy = true;
                        encode_len = cias_speex_compressed_data(ci_speex_hander, speex_rx_temp, speex_encode_buf); // 编码音频数据-每次编码20ms数据320个点-640字节
                        gCiasAiotRunParam.speex_compress_is_busy = false;
                        // mprintf("gCiasAiotFuncParam.vad_end_mute_frame = %d\r\n", gCiasAiotFuncParam.vad_end_mute_frame);
                        uint16_t upload_queue_len = PCM_UPLOAD_QUEUE_NUM - uxQueueSpacesAvailable(gCiasAiotRunParam.pcm_upload_queue);
                        if (upload_queue_len * 2 >= (50 + gCiasAiotFuncParam.vad_filter_frame)) // 过滤500ms+过滤帧长数据
                        {
                            gCiasAiotRunParam.upload_pcm_to_wifi_flag = true;
                        }
                        if (xQueueSend(gCiasAiotRunParam.pcm_upload_queue, speex_encode_buf, pdMS_TO_TICKS(100)) == pdFALSE)
                        {
                            ci_loginfo(LOG_VOICE_UPLOAD, ">>>> send gCiasAiotRunParam.pcm_upload_queue fail !!!<<<<\n");
                        }
                        mprintf("*");
                        gCiasAiotRunParam.speex_compress_frame_count++;
                        memset(speex_encode_buf, 0, sizeof(speex_encode_buf));
#endif
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
// speex音频任务处理初始化
bool audio_speex_task_init(void)
{
    SpeexMode *speex_wb_mode_new = pvPortMalloc(sizeof(SpeexMode));
    if (speex_wb_mode_new != NULL)
    {
        speex_wb_mode_new->mode = &sb_wb_mode;
        speex_wb_mode_new->query = wb_mode_query;
        speex_wb_mode_new->modeName = pvPortMalloc(strlen("wideband (sub-band CELP)"));
        memcpy(speex_wb_mode_new->modeName, "wideband (sub-band CELP)", strlen("wideband (sub-band CELP)"));
        speex_wb_mode_new->modeID = 1;
        speex_wb_mode_new->bitstream_version = 4;
        speex_wb_mode_new->enc_init = sb_encoder_init;
        speex_wb_mode_new->enc_destroy = sb_encoder_destroy;
        speex_wb_mode_new->enc = sb_encode;
        speex_wb_mode_new->enc_ctl = sb_encoder_ctl;
        cias_speex_wb_mode = speex_wb_mode_new;
    }
    else
    {
        mprintf("speex_wb_mode_new malloc error");
        return pdFALSE;
    }
    ci_speex_hander = ci_speex_create(); // 初始化speex
    if (NULL == ci_speex_hander)
    {
        return pdFALSE;
    }
    ci_speex_hander->ci_speex_mode = CI_SPEEX_INT;
    xTaskCreate(speex_encode_task, "speex_encode_task", 1024+256, NULL, 4, NULL);

    return pdTRUE;
}
#endif
#if AUDIO_COMPRESS_OPUS_ENABLE
// opus编码任务
void opus_encode_task(void)
{
    uint32_t encode_len = 0;
    uint32_t stream_aviable_len = 0;
    uint32_t rx_size = 0;
    int8_t opus_encode_buf[COMPRESS_UPLOAD_QUEUE_TIEM_SIZE] = {0};
    int8_t opus_rx_temp[COMPRESS_NEED_PCM_LEN * 2] = {0};
    OpusEncoder *opusEncoder = NULL;
    int size = opus_encoder_get_size(1);
    mprintf("==opus_encoder malloc size = %d\r\n", size);
    opusEncoder = pvPortMalloc(size);
    opus_encoder_init(opusEncoder, 16000, 1, OPUS_APPLICATION_RESTRICTED_LOWDELAY); // 最低延迟
    opus_encoder_ctl(opusEncoder, OPUS_SET_BITRATE(16000));
    opus_encoder_ctl(opusEncoder, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_40_MS));
    opus_encoder_ctl(opusEncoder, OPUS_SET_VBR(0));            // 强制CBR
    opus_encoder_ctl(opusEncoder, OPUS_SET_VBR_CONSTRAINT(1)); // 约束VBR
    opus_encoder_ctl(opusEncoder, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_WIDEBAND));
    opus_encoder_ctl(opusEncoder, OPUS_SET_COMPLEXITY(10));
    opus_encoder_ctl(opusEncoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    // 16bit
    opus_encoder_ctl(opusEncoder, OPUS_SET_LSB_DEPTH(16));
    opus_encoder_ctl(opusEncoder, OPUS_SET_PACKET_LOSS_PERC(0));
    opus_encoder_ctl(opusEncoder, OPUS_SET_DTX(0));
    int skip = 0;
    opus_encoder_ctl(opusEncoder, OPUS_GET_LOOKAHEAD(&skip));
    opus_encoder_ctl(opusEncoder, OPUS_SET_FORCE_CHANNELS(OPUS_AUTO));
    mprintf("opus_encoder_init ok\r\n");
    while (1)
    {
#if UPLOAD_PCM_DATA_ENABLE
        if (1)
#else
        if (gCiasAiotRunParam.compress_pcm_to_wifi_flag)
#endif
        {
            // mprintf("gCiasAiotRunParam.compress_pcm_to_wifi_flag----\r\n");
            stream_aviable_len = xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_compress_stream_buffer);
            if (stream_aviable_len >= COMPRESS_NEED_PCM_LEN * 2) // NB_FRAME_SIZE * 2 * 2
            {
                gCiasAiotRunParam.speex_compress_is_busy = true;
                memset(opus_rx_temp, 0, sizeof(opus_rx_temp));
                rx_size = xStreamBufferReceive(gCiasAiotRunParam.pcm_compress_stream_buffer, opus_rx_temp, COMPRESS_NEED_PCM_LEN * 2, portMAX_DELAY);
                if (rx_size != COMPRESS_NEED_PCM_LEN * 2)
                {
                    mprintf("gCiasAiotRunParam.pcm_compress_stream_buffer rcv error\r\n");
                }
                else
                {

#if UPLOAD_PCM_DATA_ENABLE
                    voice_data_packet_and_send(PCM_MIDDLE, opus_rx_temp, NETWORK_SEND_BUFF_MAX_SIZE);
                    mprintf("#");
#else
                    memset(opus_encode_buf, 0, sizeof(opus_encode_buf));
                    encode_len = opus_encode(opusEncoder, opus_rx_temp, COMPRESS_NEED_PCM_LEN, opus_encode_buf, COMPRESS_UPLOAD_QUEUE_TIEM_SIZE); // 每次编码40ms数据160*2*4=1280字节
                    // mprintf("opus encode_len len = %d\r\n", encode_len);
                    //  mprintf("gCiasAiotFuncParam.vad_end_mute_frame = %d\r\n", gCiasAiotFuncParam.vad_end_mute_frame);
                    uint16_t upload_queue_len = PCM_UPLOAD_QUEUE_NUM - uxQueueSpacesAvailable(gCiasAiotRunParam.pcm_upload_queue); //
                    if (upload_queue_len * 4 >= (50 + gCiasAiotFuncParam.vad_filter_frame))                                        // 过滤500ms+过滤帧长数据
                    {
                        gCiasAiotRunParam.upload_pcm_to_wifi_flag = true;
                    }
                    if (xQueueSend(gCiasAiotRunParam.pcm_upload_queue, opus_encode_buf, 100) == pdFALSE)
                    {
                        ci_loginfo(LOG_VOICE_UPLOAD, ">>>> send gCiasAiotRunParam.pcm_upload_queue fail !!!<<<<\n");
                    }
                    mprintf("*");
                    gCiasAiotRunParam.speex_compress_frame_count++;
                    memset(opus_encode_buf, 0, sizeof(opus_encode_buf));
#endif
                }
                gCiasAiotRunParam.speex_compress_is_busy = false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// opus音频任务处理初始化
bool audio_opus_task_init(void)
{
    if (!xTaskCreate(opus_encode_task, "opus_encode_task", 512 * 11, NULL, 4, NULL))
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    return pdTRUE;
}
#endif
#if AUDIO_COMPRESS_G722_ENABLE
// speex编码任务
void g722_encode_task(void)
{
    uint32_t encode_len = 0;
    uint32_t stream_aviable_len = 0;
    uint32_t rx_size = 0;
    int8_t g722_encode_buf[COMPRESS_UPLOAD_QUEUE_TIEM_SIZE] = {0};
    int8_t g722_rx_temp[COMPRESS_NEED_PCM_LEN * 2] = {0};

    G722EncoderState *g722Encoder = NULL;
    g722Encoder = WebRtc_g722_encode_init(g722Encoder, G722_BITRATE_64k, 0);

    while (1)
    {
#if UPLOAD_PCM_DATA_ENABLE
        if (1)
#else
        if (gCiasAiotRunParam.compress_pcm_to_wifi_flag)
#endif
        {
            // mprintf("gCiasAiotRunParam.compress_pcm_to_wifi_flag----\r\n");
            stream_aviable_len = xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_compress_stream_buffer);
            if (stream_aviable_len >= COMPRESS_NEED_PCM_LEN * 2) // NB_FRAME_SIZE * 2 * 2
            {
                gCiasAiotRunParam.speex_compress_is_busy = true;
                memset(g722_rx_temp, 0, sizeof(g722_rx_temp));
                rx_size = xStreamBufferReceive(gCiasAiotRunParam.pcm_compress_stream_buffer, g722_rx_temp, COMPRESS_NEED_PCM_LEN * 2, portMAX_DELAY);
                if (rx_size != COMPRESS_NEED_PCM_LEN * 2)
                {
                    mprintf("gCiasAiotRunParam.pcm_compress_stream_buffer rcv error\r\n");
                }
                else
                {
#if UPLOAD_PCM_DATA_ENABLE
                    voice_data_packet_and_send(PCM_MIDDLE, speex_rx_temp, NETWORK_SEND_BUFF_MAX_SIZE);
                    mprintf("#");
#else
                    memset(g722_encode_buf, 0, sizeof(g722_encode_buf));
                    encode_len = WebRtc_g722_encode(g722Encoder, g722_encode_buf, g722_rx_temp, COMPRESS_NEED_PCM_LEN); // 编码音频数据-320个点
                   // mprintf("encode_len = %d\r\n", encode_len);
                    // mprintf("gCiasAiotFuncParam.vad_end_mute_frame = %d\r\n", gCiasAiotFuncParam.vad_end_mute_frame);
                    uint16_t upload_queue_len = PCM_UPLOAD_QUEUE_NUM - uxQueueSpacesAvailable(gCiasAiotRunParam.pcm_upload_queue);
                    if (upload_queue_len * 2 >= (50 + gCiasAiotFuncParam.vad_filter_frame)) // 过滤500ms+过滤帧长数据
                    {
                        gCiasAiotRunParam.upload_pcm_to_wifi_flag = true;
                    }
                    if (xQueueSend(gCiasAiotRunParam.pcm_upload_queue, g722_encode_buf, 500) == pdFALSE)
                    {
                        ci_loginfo(LOG_VOICE_UPLOAD, ">>>> send gCiasAiotRunParam.pcm_upload_queue fail !!!<<<<\n");
                    }
                    mprintf("*");
                    gCiasAiotRunParam.speex_compress_frame_count++;
                    memset(g722_encode_buf, 0, sizeof(g722_encode_buf));
#endif
                }
                gCiasAiotRunParam.speex_compress_is_busy = false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}
#endif
void cloud_ans_count_timer_callback(TimerHandle_t xTimer)
{
    mprintf("cloud_ans_count_timer_callback is called...\r\n");
    cias_send_cmd(SKIP_INVAILD_SPEAK, DEF_FILL);
    // 唤醒中才播放
    if (get_wakeup_state() == SYS_STATE_WAKEUP)
        prompt_play_by_voice_id(2030, default_play_done_callback, false);
    gCiasAiotRunParam.cloud_parse_is_busy_flag = false;
    gCiasAiotRunParam.cloud_ans_time_out_flag = true;
    gCiasAiotRunParam.cloud_play_state = CLOUD_PLAY_END;
    cias_aiot_param_refresh();
}
bool voice_upload_task_init(void)
{
    gCiasAiotRunParam.key_is_busy_flag = false;
    gCiasAiotRunParam.is_first_wake_up = true;
    gCiasAiotRunParam.cloud_parse_is_busy_flag = false;
    gCiasAiotRunParam.cloud_ans_time_out_flag = false;
    gCiasAiotRunParam.cur_doa_aec_work_state = 1;
#if CLOUD_ANS_TIME_OUT_ENEABLE && (CLOUD_ANS_TIME_OUT_VALUE > 0)
    gCiasAiotRunParam.cloud_ans_count_timer = xTimerCreate("cloud_ans_count_timer", pdMS_TO_TICKS(CLOUD_ANS_TIME_OUT_VALUE * 1000), pdFALSE, (void *)0, cloud_ans_count_timer_callback);
    if(!gCiasAiotRunParam.cloud_ans_count_timer)
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }     
#endif    
    gCiasAiotRunParam.pcm_play_data_stream_semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(gCiasAiotRunParam.pcm_play_data_stream_semaphore);
#if AUDIO_DATA_UPLOAD_BY_UART && AUDIO_COMPRESS_SPEEX_ENABLE
    if (!audio_speex_task_init())
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
#endif
#if AUDIO_DATA_UPLOAD_BY_UART && AUDIO_COMPRESS_OPUS_ENABLE
    if (!audio_opus_task_init())
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
#endif
#if AUDIO_DATA_UPLOAD_BY_UART && AUDIO_COMPRESS_G722_ENABLE
    if (!xTaskCreate(g722_encode_task, "g722_encode_task", 1024*2, NULL, 4, NULL))
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
#endif
#if !AUDIO_COMPRESS_RECORD_DISABLE
    gCiasAiotRunParam.pcm_upload_queue = xQueueCreate(PCM_UPLOAD_QUEUE_NUM, COMPRESS_UPLOAD_QUEUE_TIEM_SIZE);
    if (!gCiasAiotRunParam.pcm_upload_queue)
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
#endif
#if NET_AUDIO_PLAY_BY_OPUS
    gCiasAiotRunParam.opus_play_queue = xQueueCreate(PCM_UPLOAD_QUEUE_NUM, OPUS_PLAY_QUEUE_TIEM_SIZE);
    if (!gCiasAiotRunParam.opus_play_queue)
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
#endif
    gCiasAiotRunParam.pcm_compress_stream_buffer = xStreamBufferCreate(PCM_ALG_FRAME_LEN * PCM_MSG_STREAM_NUM, PCM_ALG_FRAME_LEN);
    if (gCiasAiotRunParam.pcm_compress_stream_buffer == NULL)
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        // 处理错误情况
        return false;
    }
#if !SIMPLE_AUDIO_PLAYER_ENABLE
#if NET_AUDIO_PLAY_BY_PCM || NET_AUDIO_PLAY_BY_OPUS
    gCiasAiotRunParam.pcm_play_data_stream_buffer = xStreamBufferCreate(PLAY_PCM_FRAME_BUF_LEN, 1);
    if (gCiasAiotRunParam.pcm_play_data_stream_buffer == NULL)
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        // 处理错误情况
        return false;
    }
#endif
#else
#if  NET_AUDIO_PLAY_BY_OPUS
    gCiasAiotRunParam.pcm_play_data_stream_buffer = xStreamBufferCreate(PLAY_PCM_FRAME_BUF_LEN, 1);
    if (gCiasAiotRunParam.pcm_play_data_stream_buffer == NULL)
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        // 处理错误情况
        return false;
    }
#endif
#endif
    if (!xTaskCreate(vad_state_handle_task, "vad_state_handle_task", 256, NULL, 4, NULL))
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    if (!xTaskCreate(record_data_upload_task, "record_data_upload_task", 512, NULL, 4, NULL))
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    cias_aiot_param_refresh(); // 初始化aiot相关参数
    return true;
}
