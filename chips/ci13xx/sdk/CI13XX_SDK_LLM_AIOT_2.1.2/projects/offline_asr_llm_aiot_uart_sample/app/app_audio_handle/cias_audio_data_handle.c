#include "cias_audio_data_handle.h"
#include "ci_log.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include <string.h>
#include "cias_network_msg_send_task.h"
#include "semphr.h"
#include "cias_network_msg_protocol.h"
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
#include "cwsl_app_handle.h"
#include "cwsl_manage.h"
#if SIMPLE_AUDIO_PLAYER_ENABLE
#include "simple_audio_player.h"
#include "player_data_fetcher.h"
#else
#include "audio_play_api.h"
#endif
#if NET_AUDIO_PLAY_BY_OPUS && !SIMPLE_AUDIO_PLAYER_ENABLE
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
#if SIMPLE_AUDIO_PLAYER_ENABLE
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
                int send_pcm_play_size = xStreamBufferSend(gCiasAiotRunParam.pcm_play_data_stream_buffer, (uint32_t)opus_dec_data, OPUS_DECODE_DATA_SIZE * sizeof(int16_t), 300);
                if (send_pcm_play_size != OPUS_DECODE_DATA_SIZE * sizeof(int16_t))
                {
                    mprintf("opus send pcm_play_data_stream_buffer error\r\n");
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
#endif
}
static void play_end_callback() 
{ 
}

static void data_request_callback(int32_t data_size) 
{
    uint32_t request_len = data_size;
    mprintf("===PLAY_DATA_GET\r\n");
    cias_send_cmd_and_data(PLAY_DATA_GET, &request_len, 4, DEF_FILL);
    update_awake_time();
}
#else
//该任务当前版本暂未使用
void pcm_data_play_task(void *parameter)
{
#if NET_AUDIO_PLAY_BY_PCM || NET_AUDIO_PLAY_BY_OPUS
    uint8_t pcm_play_buffer[512] = {0};
    uint32_t write_pcm_addr_cpy = 0;
    while (1)
    {
        if (xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_play_data_stream_buffer) >= 512) // 固定512字节，否则会出现播放卡顿
        {
            xSemaphoreTake(gCiasAiotRunParam.pcm_play_data_stream_semaphore, pdMS_TO_TICKS(300));
            memset(pcm_play_buffer, 0, sizeof(pcm_play_buffer));
            int rx_pcm_play_size = xStreamBufferReceive(gCiasAiotRunParam.pcm_play_data_stream_buffer, pcm_play_buffer, 512, pdMS_TO_TICKS(500));
            if (rx_pcm_play_size > 0)
            {
                //mprintf("rx_pcm_play_size = %d\r\n", rx_pcm_play_size);
                cm_get_pcm_buffer(PLAY_CODEC_ID, &write_pcm_addr_cpy, 100); // TODO HSL
                int8_t *pData = (int8_t *)write_pcm_addr_cpy;
                for (int i = 0; i < rx_pcm_play_size; i++)    // 数据复制一分进行播放，不复制会出现音频播放加快
                {
                    pData[2 * i] = pcm_play_buffer[i];
                    pData[2 * i + 1] = pcm_play_buffer[i];
                }
                if (0 == write_pcm_addr_cpy)
                {
                    mprintf("write_pcm_addr_cpy error...\r\n");
                    continue;
                }
                else
                {
                    //mprintf("send pcm data to play...\r\n");
                    ciss_set(CI_SS_PLAY_STATE, CI_SS_PLAY_STATE_PLAYING);  //更新播放中状态
                    cm_write_codec(PLAY_CODEC_ID, (void *)write_pcm_addr_cpy, pdMS_TO_TICKS(100));
                }
            }
            xSemaphoreGive(gCiasAiotRunParam.pcm_play_data_stream_semaphore);
        }
        else
        {
            if(ciss_get(CI_SS_PLAY_STATE) == CI_SS_PLAY_STATE_PLAYING)
            {
                //mprintf("pcm_play_data_stream_buffer = %d\r\n", xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_play_data_stream_buffer));
                #if NET_AUDIO_PLAY_BY_OPUS
                if(xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_play_data_stream_buffer) < 512 && \
                    uxQueueSpacesAvailable(gCiasAiotRunParam.opus_play_queue) ==  OPUS_PLAY_QUEUE_NUM)
                #else
                if(xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_play_data_stream_buffer) < 512)
                #endif
                {
                    set_audio_play_state(AUDIO_PLAY_STATE_STOP);
                    ciss_set(CI_SS_PLAY_STATE, CI_SS_PLAY_STATE_IDLE);  //更新播放中状态
                }
           }
           else
           {
                vTaskDelay(5);
           }
            
        }
    }
#endif 
   
}
#endif
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
                gCiasAiotRunParam.cloud_play_state = CLOUD_PLAY_END;
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
            prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
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
            asr_wakeup_on_handle();
            gCiasAiotFuncParam.is_play_enter_wakeup_voice = param;
            ciss_set(CI_SS_CMD_STATE, CI_SS_CMD_IS_WAKEUP);
            ciss_set(CI_SS_CMD_STATE_FOR_SSP, CI_SS_CMD_IS_WAKEUP);
            enter_wakeup_deal_v1(gCiasAiotFuncParam.wake_up_continue_timeout * 1000, NULL);
            if (param == 1)
            {
                mprintf("SET_ENTER_WAKE_UP 1\r\n");
                prompt_play_by_voice_id(1, default_play_done_callback, 1);
            }
            else
            {
                mprintf("SET_ENTER_WAKE_UP 0\r\n");
            }
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
        asr_wakeup_on_handle();
        cias_aiot_vad_start_handle(VAD_START);
        gCiasAiotRunParam.is_vad_on_flag = true;
        gCiasAiotRunParam.is_vad_force_on_flag = true;
        if (!gCiasAiotFuncParam.upload_play_full_duplex || gCiasAiotFuncParam.vad_start_stop_paly)
        {
            gCiasAiotRunParam.request_play_data_flag = false;
        }
        break;
    case SET_AUDIO_STOP_RECORD:
        mprintf("set SET_AUDIO_STOP_RECORD\r\n");
        gCiasAiotRunParam.is_vad_force_on_flag = false;
        cias_aiot_vad_end_handle(0x0);
        gCiasAiotRunParam.is_vad_on_flag = false;
        gCiasAiotRunParam.stop_collect_pcm_flag = true; // 最后执行
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
    case NET_PLAY_START:
        mprintf("NET_PLAY_START.......\r\n");
#if USE_CWSL
        uint16_t timeout_cnt =0;
        extern cwsl_app_t cwsl_app;
        if (ciss_get(CI_SS_START_SLEEP_PROCESS) == 0 && cwsl_app.app_mode == CWSL_APP_REG)
        {
            /*半双工时，云端退出自学习需提前关闭VAD*/
            if (!gCiasAiotFuncParam.upload_play_full_duplex)
            {
                ciss_set(CI_SS_VOX_WORK_STATE, 0); // 关闭vad计算
                gCiasAiotRunParam.stop_collect_pcm_flag = true;
                gCiasAiotRunParam.compress_pcm_to_wifi_flag = false;
                gCiasAiotRunParam.upload_pcm_to_wifi_flag = false;
    #if NET_AUDIO_PLAY_BY_MP3
                xStreamBufferReset(gCiasAiotRunParam.pcm_compress_stream_buffer);
                xQueueReset(gCiasAiotRunParam.pcm_upload_queue);
    #endif
            }
            #if USE_AEC_MODULE
            ciss_set(CI_SS_CWSL_AEC_MUTE_STATE,CI_SS_CWSL_AEC_MUTE_OFF);
            #endif
            set_state_enter_wakeup(EXIT_WAKEUP_TIME); // 更新退出唤醒时间
            sys_ignore_exit_msg_in_queue();        
            // cwsl_reg_record_stop();
            cwsl_exit_reg_word();
            cwsl_app.app_mode = CWSL_APP_REC;
            prompt_play_by_cmd_id(CWSL_EXIT_REGISTRATION, -1, NULL, true);
            #if	SIMPLE_AUDIO_PLAYER_ENABLE
            while(sap_get_state() != SAP_STATE_IDLE && timeout_cnt<1000)
            #else
            //老播放器播放任务收到并处理停止事件需要时间
            while(AUDIO_PLAY_STATE_IDLE != get_audio_play_state() || timeout_cnt<1000)
            #endif
            {
                timeout_cnt++;
                vTaskDelay(pdMS_TO_TICKS(2));
            }
        }
#endif
#if	SIMPLE_AUDIO_PLAYER_ENABLE
        if (prompt_is_playing())
        {
            prompt_stop_play();
        }
        {
            gCiasAiotRunParam.cloud_play_state = CLOUD_PLAY_START;
            gCiasAiotRunParam.request_play_data_flag = false;
            #if NET_AUDIO_PLAY_BY_G722
            sap_play_stream("g722", 8192, REQUEST_ONE_FRAME_SEZIE, play_end_callback, data_request_callback);
            pdf_cloud_pre_init(8192);
            #elif NET_AUDIO_PLAY_BY_PCM
            sap_play_stream("pcm", 8192, REQUEST_ONE_FRAME_SEZIE, play_end_callback, data_request_callback);
            pdf_cloud_pre_init(8192);
            #elif NET_AUDIO_PLAY_BY_MP3
            sap_play_stream("MP3", 4096, REQUEST_ONE_FRAME_SEZIE, play_end_callback, data_request_callback);
            pdf_cloud_pre_init(4096);
            #endif

        }
#else		
        #if NET_AUDIO_PLAY_BY_MP3	
        gCiasAiotRunParam.request_play_data_flag = false;
        stop_play(NULL, NULL);
        outside_clear_stream(mp3_player, mp3_player_end);
        audio_play_mp3_clear();	
        set_curr_outside_handle(mp3_player, mp3_player_end);
        #endif
		xStreamBufferReset(gCiasAiotRunParam.pcm_play_data_stream_buffer);
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
        gCiasAiotRunParam.request_play_data_flag = true;
#endif
        #if CLOUD_ANS_TIME_OUT_ENEABLE
        xTimerStop(gCiasAiotRunParam.cloud_ans_count_timer, 0);
        #endif

        if (gCiasAiotRunParam.cloud_ans_time_out_flag)
        {
            gCiasAiotRunParam.cloud_ans_time_out_flag = false;
        }
        gCiasAiotRunParam.play_cloud_data_flag = true;
        gCiasAiotRunParam.cloud_parse_is_busy_flag = false; 
        gCiasAiotRunParam.play_cloud_end_flag = false;
        gCiasAiotRunParam.request_play_count = 0;
		gCiasAiotRunParam.request_play_try_count = 0;
        if (!gCiasAiotFuncParam.upload_play_full_duplex)
        {
            ciss_set(CI_SS_VOX_WORK_STATE, 0); // 关闭vad计算
            gCiasAiotRunParam.stop_collect_pcm_flag = true;
            gCiasAiotRunParam.compress_pcm_to_wifi_flag = false;
            gCiasAiotRunParam.upload_pcm_to_wifi_flag = false;
#if NET_AUDIO_PLAY_BY_MP3
            xStreamBufferReset(gCiasAiotRunParam.pcm_compress_stream_buffer);
            xQueueReset(gCiasAiotRunParam.pcm_upload_queue);
#endif
        }

        break;
    case NET_PLAY_END:
        if (!gCiasAiotRunParam.is_vad_on_flag)
        {
            gCiasAiotRunParam.request_play_data_flag = false;
#if NET_AUDIO_PLAY_BY_MP3 && !SIMPLE_AUDIO_PLAYER_ENABLE
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
#if	SIMPLE_AUDIO_PLAYER_ENABLE
		        pdf_push_data((uint8_t*)(msg_buf + msg_offset), pheader->len, pdMS_TO_TICKS(1000));
#else
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
                int rx_pcm_play_size = xStreamBufferSend(gCiasAiotRunParam.pcm_play_data_stream_buffer, (uint32_t)(msg_buf + msg_offset), pheader->len, pdMS_TO_TICKS(1000));
                if (rx_pcm_play_size != pheader->len)
                {
                    mprintf("send pcm_play_data_stream_buffer error\r\n");
                }
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
#endif
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
        gCiasAiotRunParam.play_cloud_end_flag = true;  
#if	SIMPLE_AUDIO_PLAYER_ENABLE
        if (pheader->len > 0)
        {
		    pdf_push_data((uint8_t*)(msg_buf + msg_offset), pheader->len, pdMS_TO_TICKS(1000));
        }
        gCiasAiotRunParam.cloud_play_state = CLOUD_PLAY_END;
#else   //精简播放器数据请求任务在播放结束后不能立刻释放,由play_cloud_end_flag控制
        gCiasAiotRunParam.request_play_data_flag = false;
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
            int rx_pcm_play_size = xStreamBufferSend(gCiasAiotRunParam.pcm_play_data_stream_buffer, (uint32_t)(msg_buf + msg_offset), pheader->len, pdMS_TO_TICKS(1000));
            if (rx_pcm_play_size != pheader->len)
            {
                mprintf("send pcm_play_data_stream_buffer error\r\n");
            }
        }
#endif
#endif
        break;
    case NET_PLAY_STOP: // 立即停止播放
        mprintf("===rcv NET_PLAY_STOP\r\n");
#if SIMPLE_AUDIO_PLAYER_ENABLE	
        sap_stop();
#else
#if NET_AUDIO_PLAY_BY_MP3
        stop_play(NULL, NULL);
        outside_clear_stream(mp3_player, mp3_player_end);
        set_curr_outside_handle(mp3_player, mp3_player_end);
#endif
#if NET_AUDIO_PLAY_BY_PCM
        xStreamBufferReset(gCiasAiotRunParam.pcm_play_data_stream_buffer);
#endif
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
    case WIFI_CONNECTED: // wifi已连接成功
        wifi_current_state_set(WIFI_CONNECTED_STATE);
        mprintf("WIFI_CONNECTED ...\r\n");
        if (pheader->len == 3)
        {
            uint16_t param = msg_buf[msg_offset];
            param |= (msg_buf[msg_offset + 1] << 8);
            bool m_flag = msg_buf[msg_offset + 2];
            mprintf("SET_PLAY_VOICE_ID set = %d\r\n", param);
            prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
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
            prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
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
            prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
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
            prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
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
            prompt_play_by_voice_id(param, default_play_done_callback, m_flag);
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
#if CI_OTA_ENABLE 
    case CIAS_OTA_START:
        mprintf("enter OTA MODE !!!\r\n");
        write_ota_mcu_status(1);
		dpmu_software_reset_system_config();
		break;
#endif
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
    case SET_FORCE_VAD_END:
        mprintf("==SET_FORCE_VAD_END\r\n");
        if(gCiasAiotRunParam.is_vad_on_flag)
        {
            gCiasAiotRunParam.is_vad_on_flag = false;
            cias_aiot_vad_end_handle(0x0);
        }
        break;
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
#if !SIMPLE_AUDIO_PLAYER_ENABLE
#if AUDIO_DATA_PLAY_BY_UART
void request_play_data_func(void)   //老播放器使用该函数请求云端数据
{
    uint8_t request_len[4] = {0};
#if NET_AUDIO_PLAY_BY_MP3
    mprintf("request one frame2, play buffer data size =%d\r\n", xStreamBufferBytesAvailable(mp3_player));
#endif
    request_len[0] = (REQUEST_ONE_FRAME_SEZIE & 0xff);
    request_len[1] = ((REQUEST_ONE_FRAME_SEZIE >> 8) & 0xff);
    request_len[2] = ((REQUEST_ONE_FRAME_SEZIE >> 16) & 0xff);
    request_len[3] = ((REQUEST_ONE_FRAME_SEZIE >> 24) & 0xff);
    if (gCiasAiotRunParam.request_play_count++ > 30)
    {
        gCiasAiotRunParam.request_play_data_flag = false;
    }
    cias_send_cmd_and_data(PLAY_DATA_GET, request_len, 4, DEF_FILL);
    update_awake_time(); // 更新本地唤醒时间
}
void request_play_data_task(void *parameter)
{
    static int try_count = 0;
    bool ret = false;
    while (1)
    {
        if (gCiasAiotRunParam.request_play_data_flag)
        {
#if NET_AUDIO_PLAY_BY_MP3
            if (xStreamBufferBytesAvailable(mp3_player) < PLAY_BUF_GET_DATA_MIN_SIZE)
#elif NET_AUDIO_PLAY_BY_PCM || NET_AUDIO_PLAY_BY_OPUS
            if ((xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_play_data_stream_buffer) < PLAY_BUF_GET_DATA_MIN_SIZE))
#endif
            {
                request_play_data_func();
            }
#endif
        }
        if (gCiasAiotRunParam.play_cloud_data_flag) //&& gCiasAiotRunParam.rcv_cloud_play_data_flag
        {
#if NET_AUDIO_PLAY_BY_MP3
            if (xStreamBufferBytesAvailable(mp3_player) == 0) // 播放完成
#elif NET_AUDIO_PLAY_BY_PCM || NET_AUDIO_PLAY_BY_OPUS
    if (xStreamBufferBytesAvailable(gCiasAiotRunParam.pcm_play_data_stream_buffer) < 512)
#endif
            {
                if (gCiasAiotRunParam.play_cloud_end_flag)
                {
                    if (gCiasAiotRunParam.request_play_try_count >= 10)
                    {
                        ret = true;
                    }
                }
                else if (gCiasAiotRunParam.request_play_try_count >= 30)
                {
                    ret = true;
                }
                if (ret)
                {
                    mprintf("play stop sync state to wifi ....\r\n");
                    ret = false;
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
#if NET_AUDIO_PLAY_BY_MP3
                    stop_play(NULL, NULL); // 必须保留，不然下一段播放音频会保留上一段音频数据
#elif NET_AUDIO_PLAY_BY_PCM || NET_AUDIO_PLAY_BY_OPUS
            cm_stop_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
            cm_set_codec_mute(PLAY_CODEC_ID, CODEC_OUTPUT, 3, DISABLE);
#endif
                    int try_count = 30;
                    while(try_count--)   //等待播放状态同步完成
                    {
                        if(gCiasAiotRunParam.mp3_play_finish_flag)
                        {
                            break;
                        }
                        else
                        {
                            mprintf("===wait audio play over\r\n");
                            vTaskDelay(pdMS_TO_TICKS(10));
                        }
                    }
                    cias_send_cmd(PLAY_TTS_END, DEF_FILL);
                }
                gCiasAiotRunParam.request_play_try_count++;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#endif
bool cias_online_func_init(void)
{
    network_uart_port_init();   //配置交互串口参数
    get_vad_end_mute_frame(gCiasAiotFuncParam.vad_end_mute_param_index); // 获取vad静音产生的帧数
#if !SIMPLE_AUDIO_PLAYER_ENABLE
    #if NET_AUDIO_PLAY_BY_MP3
    if(!audio_player_param_init())
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    #endif
    if (!xTaskCreate(request_play_data_task, "request_play_data_task", 512, NULL, 4, NULL)) // 请求播放数据任务
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    #if (NET_AUDIO_PLAY_BY_PCM || NET_AUDIO_PLAY_BY_OPUS)
    if (!xTaskCreate(pcm_data_play_task, "pcm_data_play_task", 512, NULL, 4, NULL)) // pcm播放任务
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    #endif
#else
    #if NET_AUDIO_PLAY_BY_OPUS
    if (!xTaskCreate(opus_data_decode_task, "opus_data_decode_task", 1024 * 6, NULL, 4, NULL)) // opus数据解析任务
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    #endif
#endif
    if (!xTaskCreate(network_recv_data_task, "network_recv_data_task", 512, NULL, 4, NULL)) // 接收WiFi数据任务
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
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