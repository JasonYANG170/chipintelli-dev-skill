#include "cias_audio_data_handle.h"
#include "ci_log.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include <string.h>
#include "cias_network_msg_send_task.h"
#include "semphr.h"
#include "cias_network_msg_protocol.h"
#include "audio_play_api.h"
#include "codec_manager.h"
#include "ci_assert.h"
#include "cias_demo_config.h"
#include "cias_common.h"
#include "cias_voice_upload.h"
#include "status_share.h"
#include "cias_audio_data_handle.h"
#include "timers.h"
#include "cias_aiot_protocol.h"
#include "ci_agc.h"
#include "user_config.h"
#include "system_msg_deal.h"
#if NET_AUDIO_PLAY_BY_OPUS
#include "opus.h"
#include "debug.h"
#include "opus_types.h"
#include "opus_private.h"
#include "opus_defines.h"
#endif
#include "ota_config.h"
static void play_done_callback(cmd_handle_t cmd_handle)
{
}
extern CiasAiotRunParamTypedef gCiasAiotRunParam;
extern CiasAiotFuncParamTypedef gCiasAiotFuncParam;
void get_vad_end_mute_frame(int param_index)
{
    switch (param_index)
    {
    case 6:
        gCiasAiotFuncParam.vad_end_mute_frame = 300 / 10;
        break;
    case 15:
        gCiasAiotFuncParam.vad_end_mute_frame = 400 / 10;
        break;
    case 20:
        gCiasAiotFuncParam.vad_end_mute_frame = 500 / 10;
        break;
    case 50:
        gCiasAiotFuncParam.vad_end_mute_frame = 1000 / 10;
        break;
    case 80:
        gCiasAiotFuncParam.vad_end_mute_frame = 1500 / 10;
        break;
    case 120:
        gCiasAiotFuncParam.vad_end_mute_frame = 2000 / 10;
        break;
    }
}
void opus_data_decode_task(void *parameter)
{
#if NET_AUDIO_PLAY_BY_OPUS
    uint8_t ret = 0;
    uint8_t opus_rcv_data[OPUS_PLAY_QUEUE_TIEM_SIZE] = {0};
    int16_t opus_dec_data[OPUS_DECODE_DATA_SIZE * sizeof(int16_t)] = {0};
    OpusDecoder *opusDecoder = NULL;
    int size = opus_decoder_get_size(1);
    mprintf("==opus_decoder malloc size = %d\r\n", size);
    opusDecoder = pvPortMalloc(size);
    ret = opus_decoder_init(opusDecoder, 16000, 1); // 最低延迟
    if (ret != OPUS_OK)
    {
        mprintf("== opus decoder init error!\r\n");
        return;
    }
    opus_decoder_ctl(opusDecoder, OPUS_SET_BITRATE(16000));
    opus_decoder_ctl(opusDecoder, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_40_MS));
    opus_decoder_ctl(opusDecoder, OPUS_APPLICATION_RESTRICTED_LOWDELAY);
    opus_decoder_ctl(opusDecoder, OPUS_SET_VBR(0));            // 强制CBR
    opus_decoder_ctl(opusDecoder, OPUS_SET_VBR_CONSTRAINT(1)); // 约束VBR
    opus_decoder_ctl(opusDecoder, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_WIDEBAND));
    opus_decoder_ctl(opusDecoder, OPUS_SET_COMPLEXITY(10));
    opus_decoder_ctl(opusDecoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    // 16bit
    opus_decoder_ctl(opusDecoder, OPUS_SET_LSB_DEPTH(16));
    opus_decoder_ctl(opusDecoder, OPUS_SET_PACKET_LOSS_PERC(0));
    opus_decoder_ctl(opusDecoder, OPUS_SET_DTX(0));
    int skip = 0;
    opus_decoder_ctl(opusDecoder, OPUS_GET_LOOKAHEAD(&skip));
    opus_decoder_ctl(opusDecoder, OPUS_SET_FORCE_CHANNELS(OPUS_AUTO));
    mprintf("opus_decoder_init ok\r\n");
    while (1)
    {
        ret = xQueueReceive(gCiasAiotRunParam.opus_play_queue, opus_rcv_data, portMAX_DELAY);
        if (pdPASS == ret)
        {
            // 解码
            //mprintf("===decoded start\r\n");
            // for(int i = 0; i < 20; i++)
            // {
            //     mprintf("%02x ", opus_rcv_data[i]);
            // }
            int decoded_size = opus_decode(opusDecoder, opus_rcv_data, OPUS_PLAY_QUEUE_TIEM_SIZE, opus_dec_data, OPUS_DECODE_DATA_SIZE * sizeof(int16_t), 0);
           // mprintf("decoded_size = %d\r\n", decoded_size);
            if (decoded_size < 0)
            {
                mprintf("Opus decode error: %d", opus_strerror(decoded_size));
            }
            else
            {
                // 发送到播放数据队列
                // int send_pcm_play_size = xStreamBufferSend(gCiasAiotRunParam.pcm_play_data_stream_buffer, (uint32_t)opus_dec_data, OPUS_DECODE_DATA_SIZE * sizeof(int16_t), 300);
                // if (send_pcm_play_size != OPUS_DECODE_DATA_SIZE * sizeof(int16_t))
                // {
                //     mprintf("opus send pcm_play_data_stream_buffer error\r\n");
                // }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
#endif
}
void pcm_data_play_task(void)
{
    static bool send_data_flag = false;
    while (1)
    {
        if (gCiasAiotRunParam.iis_play_start_flag)   
        {
            uint32_t data_addr = 0;
            uint32_t data_size;
            uint16_t block_size = AUDIO_CAP_POINT_NUM_PER_FRM * 2 * sizeof(int16_t);
            int num = block_size / sizeof(int16_t) / 2;

            // if(num*2 != data_size)
            // {
            //     mprintf("write_pcm_addr_cpy buffer is overflow\r\n");
            // }

            cm_read_codec(REF_RECORD_CODEC_ID, &data_addr, &data_size); // 读取IIS0数据
            int16_t *data_p = (int16_t *)data_addr;
            int16_t *size_p = (int16_t *)data_size;
        // mprintf("size_p =%d\r\n",*size_p);
            // for(int i =0;i<32;i++)
            // mprintf("size_p =%d\r\n",size_p[0]);
        #if USE_HP_OUT_PRE_RSLT_AUDIO
            uint32_t write_pcm_addr_cpy = 0;
            cm_get_pcm_buffer(PLAY_CODEC_ID, &write_pcm_addr_cpy, 10); // TODO HSL
            if (0 == write_pcm_addr_cpy)
            {
                mprintf("write_pcm_addr_cpy buffer is overflow\r\n");
                continue;
            }
            int16_t *pcm_data_p_cpy = (int16_t *)write_pcm_addr_cpy;
            for (int i = 0; i < num; i++)
            {
        #if USE_DENOISE_NN_RTC
                if (is_convert_left_right)
                {
                    pcm_data_p_cpy[2 * i] = data_p[2 * i];
                    pcm_data_p_cpy[2 * i + 1] = data_p[i];
                }
                else
        #endif
                {
                    pcm_data_p_cpy[2 * i] = data_p[2 * i];
                    pcm_data_p_cpy[2 * i + 1] = data_p[2 * i + 1];
                }
            }
            // memcpy((void *)pcm_data_p_cpy, (void *)data_p, *size_p);
            
            cm_write_codec(PLAY_CODEC_ID, (void *)write_pcm_addr_cpy, 0);
        #endif
    
            if (false == send_data_flag)
            {
                mprintf("play start\r\n");
                ciss_set(CI_SS_PLAY_STATE, CI_SS_PLAY_STATE_PLAYING);  //更新播放中状态
                cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
                cm_set_codec_mute(PLAY_CODEC_ID, CODEC_OUTPUT, 3, DISABLE);
                send_data_flag = true;
            }
        }
        if(send_data_flag && !gCiasAiotRunParam.iis_play_start_flag)
        {
            send_data_flag = false;
            ciss_set(CI_SS_PLAY_STATE, CI_SS_PLAY_STATE_IDLE);  //更新空闲状态
            cm_set_codec_mute(PLAY_CODEC_ID, CODEC_OUTPUT, 3, ENABLE);
            cm_stop_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
        }
        vTaskDelay(pdMS_TO_TICKS(16));
    }
}
int32_t aiot_wifi_msg_callback(const uint8_t *msg_buf)
{
    int32_t ret = RETURN_OK;
    static uint8_t ans_cmd_buf[3] = {0};
#if NET_AUDIO_PLAY_BY_OPUS
    static uint8_t opus_play_data[REQUEST_ONE_FRAME_SEZIE];
    memset(opus_play_data, 0, REQUEST_ONE_FRAME_SEZIE);
#endif
    /*这里将数据发送到需要的BUF中,例如下面*/
    cias_standard_head_t *pheader = (cias_standard_head_t *)(msg_buf);
    wifi_communicate_cmd_t wifi_cmd = (wifi_communicate_cmd_t)pheader->type;
    uint16_t msg_offset = sizeof(cias_standard_head_t);
    if (wifi_cmd != PLAY_DATA_GET)
        ci_logdebug(LOG_MEDIA, "[wifi msg:]recv type: %04x\n", wifi_cmd);
    ans_cmd_buf[0] = wifi_cmd & 0xff;
    ans_cmd_buf[1] = (wifi_cmd >> 8) & 0xff;
    switch (wifi_cmd)
    {
    case SET_VAD_SENSITIVITY: // 设置vad灵敏度(45-60)
        if (pheader->len == 1)
        {
            uint8_t sensitivity_val = msg_buf[msg_offset];
            if (sensitivity_val >= 45 && sensitivity_val <= 60)
            {
                extern vox_config_t vox_config;
                vox_config.agc_split_boundary = sensitivity_val;
                if ((vox_config.agc_split_boundary >= 45) && (vox_config.agc_split_boundary <= 48))     // 高灵敏度
                {
                    mprintf("set high sensitivity...\r\n");
                    vox_config.agc_gate_h = -3000.0f;
                    vox_config.agc_gate_l = -4500.0f;
                    vox_config.agc_gate_end = -2000.0f;
                }
                else if ((vox_config.agc_split_boundary >= 49) && (vox_config.agc_split_boundary <= 52)) // 中等灵敏度
                {
                    mprintf("set middle sensitivity...\r\n");
                    vox_config.agc_gate_h = -4000.0f;
                    vox_config.agc_gate_l = -6500.0f;
                    vox_config.agc_gate_end = -3000.0f;
                }
                else if ((vox_config.agc_split_boundary >= 53) && (vox_config.agc_split_boundary <= 60)) // 低灵敏度
                {
                    mprintf("set low sensitivity...\r\n");
                    vox_config.agc_gate_h = -5500.0f;
                    vox_config.agc_gate_l = -8000.0f;
                    vox_config.agc_gate_end = -4500.0f;
                }
            }
            else
            {
                goto PARSE_WIFI_MSG_ERR;
            }
        }
        else
        {
           goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_AUDIO_EXIT_WAKE_UP:
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            gCiasAiotFuncParam.is_play_exit_wakeup_voice = param;
            mprintf("SET_AUDIO_EXIT_WAKE_UP = %d\r\n", param);
            exit_wakeup_deal(1);
            mprintf("gCiasAiotRunParam.is_vad_force_on_flag = %d\r\n", gCiasAiotRunParam.is_vad_force_on_flag);
            if(!gCiasAiotRunParam.is_vad_force_on_flag)
            {
                gCiasAiotRunParam.wake_up_pcm_frame_count = 0;
                gCiasAiotRunParam.upload_pcm_frame_count = 0;
                gCiasAiotRunParam.speex_compress_frame_count = 0;
                gCiasAiotRunParam.vad_start_pcm_frame_count = 0;

                gCiasAiotRunParam.is_vad_on_flag = false;
                gCiasAiotRunParam.speex_compress_is_busy = false;
                gCiasAiotRunParam.pcm_upload_is_busy = false;
                gCiasAiotRunParam.compress_pcm_to_wifi_flag = false;
                gCiasAiotRunParam.upload_pcm_to_wifi_flag = false;
                gCiasAiotRunParam.wait_play_end_flag = false;

                // gCiasAiotRunParam.need_send_vad_start_flag = false;
                // 播放参数初始化

                gCiasAiotRunParam.play_cloud_data_flag = false;
                gCiasAiotRunParam.request_play_data_flag = false;
                gCiasAiotRunParam.rcv_cloud_play_data_flag = false;
                gCiasAiotRunParam.cloud_play_data_total_len = 0;
                if (!gCiasAiotFuncParam.upload_play_full_duplex)
                {
                    gCiasAiotRunParam.stop_collect_pcm_flag = false;
                }
                gCiasAiotRunParam.play_cloud_end_flag = true;
            }
            
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case PCM_DENOISE_ENABLE:
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            if (param == 1)
            {
                mprintf("set upload pcm by denoise\r\n");
            }
            else
            {
                mprintf("sset upload pcm no denoise\r\n");
            }
            gCiasAiotFuncParam.upload_audio_by_denoise = param;
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_VAD_FILTER_FRAME:
        if (pheader->len == 1)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            if (param < 15 || param > 40)
            {
                mprintf("SET_VAD_FILTER_FRAME set = %d, error(15-40)\r\n", param);
            }
            else
            {
                mprintf("SET_VAD_FILTER_FRAME set = %d ok\r\n", param);
                gCiasAiotFuncParam.vad_filter_frame = param;
            }
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_VAD_SENSITIVITY_ACTIVATE_LENTH:
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            if (param == 6 || param == 15 || param == 20 || param == 50 || param == 80 || param == 120)
            {
                gCiasAiotFuncParam.vad_end_mute_param_index = param;
                get_vad_end_mute_frame(gCiasAiotFuncParam.vad_end_mute_param_index);
                mprintf("SET_VAD_SENSITIVITY_ACTIVATE_LENTH set ok, set val = %d\r\n", param);
                ciss_set(CI_SS_VOX_SET_END_CONFIDENCE, gCiasAiotFuncParam.vad_end_mute_param_index);
            }
            else
            {
                mprintf("SET_VAD_SENSITIVITY_ACTIVATE_LENTH set error, set val = %d\r\n", param);
            }
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_VAD_START_MAX_TIMEOUT:
        if (pheader->len == 2)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            mprintf("SET_VAD_START_MAX_TIMEOUT set = %d\r\n", param);
            gCiasAiotFuncParam.vad_start_max_timeout = param;
            extern vox_config_t vox_config;
            vox_config.vad_on_max_timeout = param;
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_PLAY_VOICE_ID:
        if (pheader->len == 3)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            bool m_flag = msg_buf[msg_offset + 2];
            mprintf("SET_PLAY_VOICE_ID set  voice id = %d\r\n", param);
            // prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_WAKE_UP_CONTINUE_TIME:
        if (pheader->len == 2)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            mprintf("SET_WAKE_UP_CONTINUE_TIME set %d S\r\n", param);
            gCiasAiotFuncParam.wake_up_continue_timeout = param;
            update_awake_time();
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_ENTER_WAKE_UP:
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            if (param == 1)
            {
                mprintf("SET_ENTER_WAKE_UP 1\r\n");
                // prompt_play_by_voice_id(1, default_play_done_callback, 1);
            }
            else
            {
                mprintf("SET_ENTER_WAKE_UP 0\r\n");
            }
            asr_wakeup_on_handle();
            gCiasAiotFuncParam.is_play_enter_wakeup_voice = param;
            ciss_set(CI_SS_CMD_STATE, CI_SS_CMD_IS_WAKEUP);
            ciss_set(CI_SS_CMD_STATE_FOR_SSP, CI_SS_CMD_IS_WAKEUP);
            enter_wakeup_deal(gCiasAiotFuncParam.wake_up_continue_timeout * 1000, NULL);
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_INTERACTION_NULTI_ROUND_ENABLE:
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            mprintf("SET_INTERACTION_NULTI_ROUND_ENABLE value  = %d\r\n", param);
            gCiasAiotFuncParam.interaction_multi_round = param;
            if(get_wakeup_state() == SYS_STATE_WAKEUP)
            {
                mprintf("gCiasAiotRunParam.is_wake_up_flag set true\r\n");
                gCiasAiotRunParam.is_wake_up_flag = true;
            }
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case UPLOAD_PLAY_FULL_DUPLEX_EANBLE:
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            mprintf("UPLOAD_PLAY_FULL_DUPLEX_EANBLE value = %d\r\n", param);
            gCiasAiotFuncParam.upload_play_full_duplex = param;
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_AUDIO_VOLUME:
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            if (param < VOLUME_MIN || param > VOLUME_MAX)
            {
                goto PARSE_WIFI_MSG_ERR;
            }
            else
            {
                vol_set(param);
                mprintf("set SET_AUDIO_VOLUME\r\n");
            }
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_AUDIO_COMPRESS_TYPE:
        mprintf("set SET_AUDIO_COMPRESS_TYPE\r\n");
        break;
    case SET_VOLUME_MUTE_STATE:
        mprintf("set SET_AUDIO_COMPRESS_TYPE\r\n");
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            if (param == 1)
            {
                power_amplifier_off();   //关闭功放使能-静音模式
            }
            else if (param == 0)
            {
                power_amplifier_on();    // 开启功放使能-关闭静音模式
            }
            else
            {
                goto PARSE_WIFI_MSG_ERR;
            }
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case SET_AUDIO_START_RECORD:
        mprintf("set SET_AUDIO_START_RECORD\r\n");
        #if 0
        asr_wakeup_on_handle();
        cias_aiot_vad_start_handle(VAD_START);
        gCiasAiotRunParam.is_vad_on_flag = true;
        gCiasAiotRunParam.is_vad_force_on_flag = true;
        if (!gCiasAiotFuncParam.upload_play_full_duplex || gCiasAiotFuncParam.vad_start_stop_paly)
        {
            gCiasAiotRunParam.request_play_data_flag = false;
        }
        #else
        asr_wakeup_on_handle();
        //gCiasAiotRunParam.is_vad_force_on_flag = true;
        gCiasAiotRunParam.is_always_iis_flag = true;
        #endif
        break;
    case SET_AUDIO_STOP_RECORD:
        mprintf("set SET_AUDIO_STOP_RECORD\r\n");
        #if 0
        gCiasAiotRunParam.is_vad_force_on_flag = false;
        cias_aiot_vad_end_handle(0x0);
        gCiasAiotRunParam.is_vad_on_flag = false;
        gCiasAiotRunParam.stop_collect_pcm_flag = true; // 最后执行
        #else
        //gCiasAiotRunParam.is_vad_force_on_flag = false;
        gCiasAiotRunParam.is_always_iis_flag = false;
        gCiasAiotRunParam.is_wake_up_flag = false;
        //exit_wakeup_deal(1);
        #endif
        break;
    case VAD_START_STOP_PLAY:
        mprintf("set VAD_START_STOP_PLAY\r\n");
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            mprintf("VAD_START_STOP_PLAY value = %d\r\n", param);
            gCiasAiotFuncParam.vad_start_stop_paly = param;
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
            #if 0
    case NET_PLAY_START:
        mprintf("NET_PLAY_START.......\r\n");
        #if NET_AUDIO_PLAY_BY_MP3	
        gCiasAiotRunParam.request_play_data_flag = false;
        stop_play(NULL, NULL);
        outside_clear_stream(mp3_player, mp3_player_end);
        audio_play_mp3_clear();	
        set_curr_outside_handle(mp3_player, mp3_player_end);
        #endif
        #if CLOUD_ANS_TIME_OUT_ENEABLE
        xTimerStop(gCiasAiotRunParam.cloud_ans_count_timer, 0);
        #endif
        //xStreamBufferReset(gCiasAiotRunParam.pcm_play_data_stream_buffer);
        if (gCiasAiotRunParam.cloud_ans_time_out_flag)
        {
            gCiasAiotRunParam.cloud_ans_time_out_flag = false;
        }
        gCiasAiotRunParam.cloud_parse_is_busy_flag = false; 
        gCiasAiotRunParam.request_play_count = 0;
        gCiasAiotRunParam.request_play_data_flag = true;
        gCiasAiotRunParam.play_cloud_data_flag = true;
        gCiasAiotRunParam.play_cloud_end_flag = false;
        gCiasAiotRunParam.request_play_try_count = 0;
        if (!gCiasAiotFuncParam.upload_play_full_duplex)
        {
            ciss_set(CI_SS_VOX_WORK_STATE, 0); // 关闭vad计算
            gCiasAiotRunParam.stop_collect_pcm_flag = true;//不播放是否删除
            gCiasAiotRunParam.compress_pcm_to_wifi_flag = false;
            gCiasAiotRunParam.upload_pcm_to_wifi_flag = false;
#if NET_AUDIO_PLAY_BY_MP3
            xStreamBufferReset(gCiasAiotRunParam.pcm_compress_stream_buffer);
            xQueueReset(gCiasAiotRunParam.pcm_upload_queue);
#endif
        }
        gCiasAiotRunParam.rcv_cloud_play_data_flag = false;
#if NET_AUDIO_PLAY_BY_PCM||NET_AUDIO_PLAY_BY_OPUS
        mprintf("init play card...\r\n");
        cm_stop_codec(PLAY_CODEC_ID, CODEC_OUTPUT);    //清中断
        audio_pre_rslt_out_play_card_init(); // 重新初始化声卡
        cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
        cm_set_codec_mute(PLAY_CODEC_ID, CODEC_OUTPUT, 3, DISABLE);
#endif
        request_play_data_func();   //预先请求两帧播放数据
        request_play_data_func(); 
        break;
    case NET_PLAY_END:
        if (!gCiasAiotRunParam.is_vad_on_flag)
        {
            gCiasAiotRunParam.request_play_data_flag = false;
#if NET_AUDIO_PLAY_BY_MP3
            stop_play(NULL, NULL);
            outside_clear_stream(mp3_player, mp3_player_end);
            set_curr_outside_handle(mp3_player, mp3_player_end);
#endif
        }
        break;
    case PLAY_DATA_RECV:
        mprintf("pheader->len = %d\r\n", pheader->len);
        gCiasAiotRunParam.request_play_try_count = 0; // 清除超时参数
        if (!gCiasAiotRunParam.play_cloud_end_flag)
        {
            if (pheader->len > 0)
            {
                gCiasAiotRunParam.request_play_count = 0;
#if NET_AUDIO_PLAY_BY_MP3
                ret = outside_write_stream(mp3_player, (uint32_t)(msg_buf + msg_offset), pheader->len, false);
                if (RETURN_OK != ret)
                {
                    mprintf("outside_write_stream write fail...\r\n");
                }
                play_with_outside(0, "mp3", NULL);
#endif
#if NET_AUDIO_PLAY_BY_PCM
                // int rx_pcm_play_size = xStreamBufferSend(gCiasAiotRunParam.pcm_play_data_stream_buffer, (uint32_t)(msg_buf + msg_offset), pheader->len, pdMS_TO_TICKS(1000));
                // if (rx_pcm_play_size != pheader->len)
                // {
                //     mprintf("send pcm_play_data_stream_buffer error\r\n");
                // }
#endif
#if NET_AUDIO_PLAY_BY_OPUS
                memcpy(opus_play_data, msg_buf + msg_offset, pheader->len);
                for(int i = 0; i < pheader->len/OPUS_PLAY_QUEUE_TIEM_SIZE; i++)
                {
                    if (xQueueSend(gCiasAiotRunParam.opus_play_queue, (uint32_t)(opus_play_data + i*OPUS_PLAY_QUEUE_TIEM_SIZE), 100) == pdFALSE)
                    {
                        mprintf(">>>> send gCiasAiotRunParam.opus_play_queue fail !<<<<\n");
                    }
                }
                
#endif
                gCiasAiotRunParam.rcv_cloud_play_data_flag = true;
            }
            else
            {
                mprintf("rcv play null data...\r\n");
            }
        }
        else
        {
            mprintf("rcv err play data\r\n");
        }
        break;
    case PLAY_DATA_END:
        mprintf("auido paly finish...\r\n");
        mprintf("pheader->len = %d\r\n", pheader->len);
        gCiasAiotRunParam.request_play_data_flag = false;
        gCiasAiotRunParam.play_cloud_end_flag = true;

#if NET_AUDIO_PLAY_BY_MP3
        if (pheader->len > 0)
        {
            ret = outside_write_stream(mp3_player, (uint32_t)(msg_buf + msg_offset), pheader->len, false);
            if (RETURN_OK != ret)
            {
                mprintf("outside_write_stream write fail...\r\n");
            }
           
            //ret = outside_write_stream(mp3_player, (uint32_t)(space), 1024, false); // 解决末尾吞音问题
            //play_with_outside(0, "mp3", NULL);
        }
        else
        {
            //ret = outside_write_stream(mp3_player, (uint32_t)(space), 1024, false); // 解决末尾吞音问题
            //play_with_outside(0, "mp3", NULL);
        }
#endif
#if NET_AUDIO_PLAY_BY_PCM
        if (pheader->len > 0)
        {
            // int rx_pcm_play_size = xStreamBufferSend(gCiasAiotRunParam.pcm_play_data_stream_buffer, (uint32_t)(msg_buf + msg_offset), pheader->len, pdMS_TO_TICKS(1000));
            // if (rx_pcm_play_size != pheader->len)
            // {
            //     mprintf("send pcm_play_data_stream_buffer error\r\n");
            // }
        }
#endif
        break;
    case NET_PLAY_STOP: // 立即停止播放
        mprintf("===rcv NET_PLAY_STOP\r\n");
#if NET_AUDIO_PLAY_BY_MP3
        stop_play(NULL, NULL);
        outside_clear_stream(mp3_player, mp3_player_end);
        set_curr_outside_handle(mp3_player, mp3_player_end);
#endif
#if NET_AUDIO_PLAY_BY_PCM
       // xStreamBufferReset(gCiasAiotRunParam.pcm_play_data_stream_buffer);
#endif
        ciss_set(CI_SS_PLAY_STATE, CI_SS_PLAY_STATE_IDLE); // 设置播放结束
        if (!gCiasAiotFuncParam.upload_play_full_duplex)
        {
            ciss_set(CI_SS_VOX_WORK_STATE, 1); // 开启vox vad计算
        }
        gCiasAiotRunParam.request_play_try_count = 0;
        gCiasAiotRunParam.play_cloud_data_flag = false;
        gCiasAiotRunParam.request_play_data_flag = false;
        gCiasAiotRunParam.wait_play_end_flag = false;
        gCiasAiotRunParam.rcv_cloud_play_data_flag = false;
        gCiasAiotRunParam.stop_collect_pcm_flag = false;
        cias_send_cmd(PLAY_TTS_END, DEF_FILL);
        break;
        #endif
        
    case MASTER_PLAY_STATE: //主控MCU发送播放状态
        if (pheader->len == 1)
        {
            uint8_t param = msg_buf[msg_offset];
            if (param == 1)
            {
                gCiasAiotRunParam.iis_play_start_flag = true;   //开始IIS播放
            }
            else if (param == 0)
            {
                gCiasAiotRunParam.iis_play_start_flag = false;  //结束IIS播放
            }
            else
            {
                goto PARSE_WIFI_MSG_ERR;
            }
        }
        break;
    case WIFI_CONNECTED: // wifi已连接成功
        wifi_current_state_set(WIFI_CONNECTED_STATE);
        mprintf("WIFI_CONNECTED ...\r\n");
        if (pheader->len == 3)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            bool m_flag = msg_buf[msg_offset + 2];
            mprintf("SET_PLAY_VOICE_ID set = %d\r\n", param);
            // prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case CLOUD_CONNECTED: // 云平台已连接成功
        mprintf("CLOUD_CONNECTED...\r\n");
        wifi_current_state_set(CLOUD_CONNECTED_STATE);
        if (pheader->len == 3)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            bool m_flag = msg_buf[msg_offset + 2];
            mprintf("SET_PLAY_VOICE_ID set = %d\r\n", param);
            // prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case WIFI_DISCONNECTED: // wifi已断开连接
        mprintf("WIFI_DISCONNECTED !!!\r\n");
        wifi_current_state_set(CLOUD_DISCONNECT_STATE);
        if (pheader->len == 3)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            bool m_flag = msg_buf[msg_offset + 2];
            mprintf("SET_PLAY_VOICE_ID set = %d\r\n", param);
            // prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case NET_CONFIG_FAIL: // wifi已断开连接
        mprintf("NET_CONFIG_FAIL !!!\r\n");
        if (pheader->len == 3)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            bool m_flag = msg_buf[msg_offset + 2];
            mprintf("SET_PLAY_VOICE_ID set = %d\r\n", param);
            // prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case NET_CONFIG_SUCCESS:
        mprintf("NET_CONFIG_SUCCESS !!!\r\n");
        if (pheader->len == 3)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            bool m_flag = msg_buf[msg_offset + 2];
            mprintf("NET_CONFIG_SUCCESS set = %d\r\n", param);
            // prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case IOT_QUITE_WAKE_UP_MODE: // 退出唤醒模式
        // gCiasAiotRunParam.is_wake_up_flag = false;
        // asr_wakeup_exit_handle();   // 退出唤醒
        break;
    case CIAS_AUDIO_RST:
        dpmu_software_reset_system_config();
        break;
    case CIAS_OTA_START:
        mprintf("enter OTA MODE !!!\r\n");
        write_ota_mcu_status(1);
		dpmu_software_reset_system_config();

		break;
#if IIS_CHANNEL_ENG_CALC_EANBLE
    case CIAS_FACTORY_START: // 开始测试
        mprintf("CIAS_FACTORY_START==\r\n");
        if (pheader->len == 5)
        {
            uint8_t micl_enable = msg_buf[msg_offset];
            uint8_t micr_enable = msg_buf[msg_offset + 1];
            uint8_t refl_enable = msg_buf[msg_offset + 2];
            uint8_t refr_enable = msg_buf[msg_offset + 3];
            uint8_t real_upload_flag = msg_buf[msg_offset + 4];
            mprintf("micl_enable = %d\r\n", micl_enable);
            mprintf("micr_enable = %d\r\n", micr_enable);
            mprintf("refl_enable = %d\r\n", refl_enable);
            mprintf("refr_enable = %d\r\n", refr_enable);
            mprintf("real_upload_flag = %d\r\n", real_upload_flag);
            gCiasAiotFuncParam.micl_eng_db_calc_flag = micl_enable;
            gCiasAiotFuncParam.micr_eng_db_calc_flag = micr_enable;
            gCiasAiotFuncParam.refl_eng_db_calc_flag = refl_enable;
            gCiasAiotFuncParam.refr_eng_db_calc_flag = refr_enable;
            gCiasAiotFuncParam.upload_factory_test_real_val_flag = real_upload_flag;
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        if (!cias_factory_test_init())
        {
           goto PARSE_WIFI_MSG_ERR;
        }
        break;
    case CIAS_FACTORY_TEST_ENG_THR_SET:
        if (pheader->len == 4)
        {
            uint8_t micl_thr = msg_buf[msg_offset];
            uint8_t micr_thr = msg_buf[msg_offset + 1];
            uint8_t refl_thr = msg_buf[msg_offset + 2];
            uint8_t refr_thr = msg_buf[msg_offset + 3];
            bool thr_set_flag = true;
            mprintf("CIAS_FACTORY_TEST_ENG_THR_SET MICL THR = %ddb\r\n", micl_thr);
            mprintf("CIAS_FACTORY_TEST_ENG_THR_SET MICR THR= %ddb\r\n", micr_thr);
            mprintf("CIAS_FACTORY_TEST_ENG_THR_SET REFL THR = %ddb\r\n", refl_thr);
            mprintf("CIAS_FACTORY_TEST_ENG_THR_SET REFR THR = %ddb\r\n", refr_thr);
            if (micl_thr > 0 && micl_thr <= 255)
            {
                gCiasAiotFuncParam.micl_db_thr_val = micl_thr;
            }
            else
            {
                thr_set_flag = false;
            }
            if (micr_thr > 0 && micr_thr <= 255)
            {
                gCiasAiotFuncParam.micr_db_thr_val = micr_thr;
            }
            else
            {
                thr_set_flag = false;
            }
            if (refl_thr > 0 && refl_thr <= 255)
            {
                gCiasAiotFuncParam.refl_db_thr_val = refl_thr;
            }
            else
            {
                thr_set_flag = false;
            }
            if (refr_thr > 0 && refr_thr <= 255)
            {
                gCiasAiotFuncParam.refr_db_thr_val = refr_thr;
            }
            else
            {
                thr_set_flag = false;
            }
            if(!thr_set_flag)
            {
                mprintf("[CIAS_FACTORY_TEST_ENG_THR_SET]:thr set err\r\n");
                goto PARSE_WIFI_MSG_ERR;
            }
        }
        else
        {
            goto PARSE_WIFI_MSG_ERR;
        }
        break;
#endif
    case SET_CLOUD_ANS_TIMEOUT_EXIT:
        mprintf("SET_CLOUD_ANS_TIMEOUT_EXIT set\r\n");
        #if CLOUD_ANS_TIME_OUT_ENEABLE
            xTimerStop(gCiasAiotRunParam.cloud_ans_count_timer, 0); 
        #endif
        gCiasAiotRunParam.cloud_parse_is_busy_flag = false;
        break;
    #if USE_CWSL
	case CWSL_UART_REGISTRATION_WAKE: /*开始学习*/
    case CWSL_UART_EXIT_REGISTRATION: /*退出学习*/
    case CWSL_UART_DELETE_WAKE:       /*删除唤醒词*/
        mprintf("CWSL_UART_REGISTRATION %X\r\n", wifi_cmd);
        if(get_wakeup_state() != SYS_STATE_WAKEUP)
        {
            asr_wakeup_on_handle();
            gCiasAiotFuncParam.is_play_enter_wakeup_voice = 0;
            ciss_set(CI_SS_CMD_STATE, CI_SS_CMD_IS_WAKEUP);
            ciss_set(CI_SS_CMD_STATE_FOR_SSP, CI_SS_CMD_IS_WAKEUP);
            enter_wakeup_deal(gCiasAiotFuncParam.wake_up_continue_timeout * 1000, NULL);
        }
        else
        {
            update_awake_time(); // 更新本地唤醒时间
        }
        ciss_set(CI_SS_START_SLEEP_PROCESS, 0);
        cwsl_app_process_asr_msg(NULL, NULL, wifi_cmd);
        break;
    #endif
    default:
        goto PARSE_WIFI_MSG_ERR;
        break;
    }
    ans_cmd_buf[2] = 0x01;   //执行成功
    cias_send_cmd_and_data(CIAS_CMD_EXEC_STATE, ans_cmd_buf, 3, DEF_FILL);
    return 0;
PARSE_WIFI_MSG_ERR:
    mprintf("parse wifi msg error:pheader-len = %d\r\n", pheader->len);
    ans_cmd_buf[2] = 0x02;   //执行失败
    cias_send_cmd_and_data(CIAS_CMD_EXEC_STATE, ans_cmd_buf, 3, DEF_FILL);
return -1;
}
void request_play_data_func(void)
{
   
}
void request_play_data_task(void *parameter)
{
   
}
bool cias_online_func_init(void)
{
    network_uart_port_init();   //配置交互串口参数
    if (!network_port_recv_queue_init())
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    if (!voice_upload_task_init())
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    if(!network_send_task_init())
    {
        mprintf("error network_send_task_init error\r\n");
        return false;
    }
    if (!xTaskCreate(network_send_data_task, "network_send_data_task", 512, NULL, 4, NULL)) // 通过串口上传语音任务
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    if (!xTaskCreate(network_recv_data_task, "network_recv_data_task", 512, NULL, 4, NULL)) // 接收WiFi数据任务
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    if (!xTaskCreate(pcm_data_play_task, "pcm_data_play_task", 512, NULL, 4, NULL)) // pcm播放任务
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
#if NET_AUDIO_PLAY_BY_OPUS
    if (!xTaskCreate(opus_data_decode_task, "opus_data_decode_task", 1024 * 6, NULL, 4, NULL)) // opus数据解析任务
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
#endif
    return true;
}
cinv_item_ret_t write_ota_mcu_status(bool status)
{
    ota_nv_status_t ota_nv_status;
    ota_nv_status.status = status;
    ota_nv_status.ota_chip_type_1306 = OTA_CHIP_TYPE_1306;
    ota_nv_status.timeout = OTA_TIMOUT;
    ota_nv_status.retry_time = OTA_RETRY_TIME;
    ota_nv_status.log_uart_number = CONFIG_CI_LOG_UART;
    ota_nv_status.ota_uart_number = UART_NUM_SEND_PLAY_AUDIO_NUMBER;
    ota_nv_status.ota_uart_baud = UART_NUM_SEND_PLAY_AUDIO_BAUDRATE;
	return cinv_item_write(NVDATA_ID_OTA_MCU_STATUS, sizeof(ota_nv_status_t), &ota_nv_status);
}