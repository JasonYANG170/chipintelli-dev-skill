#include "codec_manager.h"
#include "ci130x_audio_pre_rslt_out.h"
#include <string.h>
#include "ci130x_codec.h"
#include "ci130x_dpmu.h"
#include "board.h"
#include "ci130x_gpio.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "sdk_default_config.h"
#include "ci130x_uart.h"
#include "ci130x_dma.h"
#include "debug_time_consuming.h"
#include "stream_buffer.h"
#include "cias_voice_upload.h"
#include "cias_aiot_protocol.h"
#include "status_share.h"
#include "user_config.h"
#include "ci_audio_wrapfft.h"
#define BUFFER_NUM (4)
typedef struct
{
    audio_pre_rslt_out_init_t init_str;
    // 写了多少次数据
    uint32_t write_data_cnt;
    // 已经发送了多少次数据，写数据频次是固定的，我们不能改变，只能改变发送数据的频度，
    // 如果发送太快，会在mute模式再发送上一个buf，
    // 如果发送太慢，会扔掉一个buf不发送
    uint32_t send_data_cnt;

    int32_t write_send_sub_slave; // 在slave模式下，write和send的差值
    // 是否使用硬件tx merge功能
    bool hardware_tx_merge;
} audio_pre_init_tmp_t;

volatile uint8_t uart_dma_trans_done = 1;

static void uart_dma_read_irq_callback(void)
{
    uart_dma_trans_done = 1;
}

static audio_pre_init_tmp_t sg_init_tmp_str;

#define PI (3.1416926f)
void sine_wave_generate(int16_t *sine_wave, uint32_t sample_rate, uint32_t wave_fre, uint32_t point_num)
{
    int16_t num_of_one_period = sample_rate / wave_fre; // 每个周期的采样点数
    for (int i = 0; i < point_num; i++)
    {
        sine_wave[i] = (int16_t)(32767.0f * sinf((2 * PI * i) / num_of_one_period));
    }
}

uint32_t tmp_voice_addr = 0;

uint8_t *uart_send_pre_data_buffer = NULL;
void audio_pre_rslt_out_play_card_init(void)
{
    uint16_t block_size = AUDIO_CAP_POINT_NUM_PER_FRM * 2 * sizeof(int16_t);

    sg_init_tmp_str.init_str.block_size = block_size;
    sg_init_tmp_str.write_data_cnt = 0;
    sg_init_tmp_str.send_data_cnt = 0;
#if (USE_HP_OUT_NET_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO) && !SIMPLE_AUDIO_PLAYER_ENABLE
    audio_pre_rslt_out_codec_init_pa_out();
#endif
#if USE_IIS1_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
#if USE_AUDIO_UPLOAD_BY_IIS
    audio_pre_rslt_out_codec_init();
    ref_in_codec_registe();
    cm_start_codec(REF_RECORD_CODEC_ID, CODEC_INPUT);
#else
#if USE_IIS1_OUT_PRE_RSLT_AUDIO
    audio_pre_rslt_out_codec_init();
#endif
#endif

#endif
}

#if USE_DENOISE_NN_RTC
static bool is_convert_left_right = false;
void convert_left_and_right(bool r)
{
    is_convert_left_right = r;
}
#endif
extern CiasAiotRunParamTypedef gCiasAiotRunParam;
extern CiasAiotFuncParamTypedef gCiasAiotFuncParam;
#if IIS_CHANNEL_ENG_CALC_EANBLE
/*计算能量*/
void alg_calc_audio_eng(short *p_audio_pcm_data, uint16_t audio_fft_data_len, uint32_t *p_dst_db, uint32_t *P_dst_eng)
{
    float eng_tmp_avg = 0.0f, eng_tmp = 0.0f, dst_db_cur = 0.0f;
    for(int i = 0; i < audio_fft_data_len; i++)
    {
        eng_tmp += p_audio_pcm_data[i]*p_audio_pcm_data[i];
    }
    eng_tmp_avg = sqrtf(eng_tmp/audio_fft_data_len/32767.0f);
    *p_dst_db = (uint32_t)(20*log10f(eng_tmp_avg/6.0f) + 102.0f);
    *P_dst_eng = sqrt(eng_tmp/audio_fft_data_len);
}

static void audio_eng_calc(ci_wrapfft_audio * p_wrapfft_audio)
{
    static int calc_eng_frame_interval = 0; 
    if(calc_eng_frame_interval++ == ENG_CALC_INTERVAL_FRAME)
    {
        calc_eng_frame_interval = 0;
        //计算左mic能量和DB
        if(gCiasAiotFuncParam.micl_eng_db_calc_flag)
        {
            alg_calc_audio_eng(p_wrapfft_audio->mic[0], AUDIO_CAP_POINT_NUM_PER_FRM, &gCiasAiotFuncParam.micl_db, &gCiasAiotFuncParam.micl_eng);
            //mprintf("micl dst_db = %d\r\n", gCiasAiotFuncParam.micl_db);
           // mprintf("micl dst_eng = %d\r\n", gCiasAiotFuncParam.micl_eng);
        }
        //计算右mic能量和DB
        if(gCiasAiotFuncParam.micr_eng_db_calc_flag)
        {
            alg_calc_audio_eng(p_wrapfft_audio->mic[1], AUDIO_CAP_POINT_NUM_PER_FRM, &gCiasAiotFuncParam.micr_db, &gCiasAiotFuncParam.micr_eng);
           // mprintf("micr dst_db = %d\r\n", gCiasAiotFuncParam.micr_db);
           // mprintf("micr dst_eng = %d\r\n", gCiasAiotFuncParam.micr_eng);
        }
        //计算参考信号左通道能量和DB
        if(gCiasAiotFuncParam.refl_eng_db_calc_flag)
        {
            alg_calc_audio_eng(p_wrapfft_audio->ref[0], AUDIO_CAP_POINT_NUM_PER_FRM, &gCiasAiotFuncParam.refl_db, &gCiasAiotFuncParam.refl_eng);
           // mprintf("refl dst_db = %d\r\n", gCiasAiotFuncParam.refl_db);
           // mprintf("refl dst_eng = %d\r\n", gCiasAiotFuncParam.refl_eng);
        }
        //计算参考信号右通道能量和DB
        if(gCiasAiotFuncParam.refr_eng_db_calc_flag)
        {
            alg_calc_audio_eng(p_wrapfft_audio->ref[1], AUDIO_CAP_POINT_NUM_PER_FRM, &gCiasAiotFuncParam.refr_db, &gCiasAiotFuncParam.refr_eng);
          //  mprintf("refr dst_db = %d\r\n", gCiasAiotFuncParam.refr_db);
          //  mprintf("refr dst_eng = %d\r\n", gCiasAiotFuncParam.refr_eng);
        }
    }   
}
#endif
#if USE_AUDIO_UPLOAD_BY_IIS
void audio_pre_rslt_upload_by_iis(int16_t *left, int16_t *right, ci_wrapfft_audio *p_wrapfft_audio)
{
    uint32_t write_pcm_addr = 0;
    uint32_t block_size = sg_init_tmp_str.init_str.block_size;
    int num = block_size / sizeof(int16_t) / 2;
#if IIS_UPLOAD_IS_WAKEUP
    if ((gCiasAiotRunParam.is_wake_up_flag || gCiasAiotRunParam.is_always_iis_flag))  
#endif 
    {
        cm_get_pcm_buffer(PLAY_PRE_AUDIO_CODEC_ID, &write_pcm_addr, 0); // TODO HSL
        if (0 == write_pcm_addr)
        {
            mprintf("write_pcm_addr buffer is overflow\r\n");
            return;
        }
        int16_t *pcm_data_p = (int16_t *)write_pcm_addr;
        for (int i = 0; i < num; i++)
        {
#if USE_DENOISE_NN_RTC
            if (is_convert_left_right)
            {
                pcm_data_p[2 * i] = right[i];
                pcm_data_p[2 * i + 1] = left[i];
            }
            else
#endif
            {    
                if (gCiasAiotFuncParam.upload_audio_by_denoise) // 上传降噪音频
                {
                    if(ciss_get(CI_SS_AEC_WORK_STATE))  //AEC处理状态上传AEC处理后，降噪前的数据
                    {
                        pcm_data_p[2 * i] = p_wrapfft_audio->asr_src_data[i];
                        pcm_data_p[2 * i + 1] = p_wrapfft_audio->asr_src_data[i];
                    }
                    else
                    {
                        pcm_data_p[2 * i] = right[i];   
                        pcm_data_p[2 * i + 1] = right[i];   //非AEC处理状态上传降噪后的音频
                    }
                }
            }
        }
        cm_write_codec(PLAY_PRE_AUDIO_CODEC_ID, (void *)write_pcm_addr, 0);
    }
    if (0 == sg_init_tmp_str.send_data_cnt)
    {
        cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
// #if USE_HP_OUT_PRE_RSLT_AUDIO && !NET_AUDIO_PLAY_BY_PCM
//         cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
// #endif
    }
    sg_init_tmp_str.send_data_cnt++;
    sg_init_tmp_str.write_data_cnt++;
}
#elif USE_AUDIO_UPLOAD_BY_HPOUT
void audio_pre_rslt_upload_by_hpout(int16_t *left, int16_t *right, ci_wrapfft_audio *p_wrapfft_audio)
{
    int ret = 0;
    uint32_t block_size = sg_init_tmp_str.init_str.block_size;
    int num = block_size / sizeof(int16_t) / 2;

#if USE_IIS1_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
    if ((gCiasAiotRunParam.is_wake_up_flag || gCiasAiotRunParam.is_always_hpout_flag))    
    {
    #if USE_HP_OUT_PRE_RSLT_AUDIO
        uint32_t write_pcm_addr_cpy = 0;
        cm_get_pcm_buffer(PLAY_CODEC_ID, &write_pcm_addr_cpy, 10); // TODO HSL
        if (0 == write_pcm_addr_cpy)
        {
            mprintf("write_pcm_addr_cpy buffer is overflow\r\n");
            return;
        }
        int16_t *pcm_data_p_cpy = (int16_t *)write_pcm_addr_cpy;
        for (int i = 0; i < num; i++)
        {
    #if USE_DENOISE_NN_RTC
            if (is_convert_left_right)
            {
                pcm_data_p_cpy[2 * i] = right[i];
                pcm_data_p_cpy[2 * i + 1] = left[i];
            }
            else
    #endif
            {
                pcm_data_p_cpy[2 * i] = right[i];
                pcm_data_p_cpy[2 * i + 1] = right[i];
            }
        }
        cm_write_codec(PLAY_CODEC_ID, (void *)write_pcm_addr_cpy, 0);
    #endif
    }
#endif
    if (0 == sg_init_tmp_str.send_data_cnt)
    {
#if USE_IIS1_OUT_PRE_RSLT_AUDIO
        cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
#endif
#if USE_HP_OUT_PRE_RSLT_AUDIO 
        cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
#endif
    }
    sg_init_tmp_str.send_data_cnt++;
    sg_init_tmp_str.write_data_cnt++;
}
#elif USE_AUDIO_UPLOAD_BY_UART
void audio_pre_rslt_upload_by_uart(int16_t *left, int16_t *right, ci_wrapfft_audio *p_wrapfft_audio)
{
    int ret = 0;
    uint32_t block_size = sg_init_tmp_str.init_str.block_size;
    int num = block_size / sizeof(int16_t) / 2;
    extern int8_t speex_src_temp_buf[PCM_ALG_FRAME_LEN];  //p_wrapfft_audio->asr_src_data);
#if AUDIO_DATA_UPLOAD_BY_UART
    if (!gCiasAiotFuncParam.upload_play_full_duplex)
    {
        if (CI_SS_PLAY_STATE_PLAYING == ciss_get(CI_SS_PLAY_STATE))
        {
            return;
        }
    }
#if UPLOAD_PCM_DATA_ENABLE
    if (1)
#else
    // mprintf("gCiasAiotRunParam.is_wake_up_flag = %d\r\n", gCiasAiotRunParam.is_wake_up_flag);
    // mprintf("gCiasAiotRunParam.stop_collect_pcm_flag = %d\r\n", gCiasAiotRunParam.stop_collect_pcm_flag);
    if (gCiasAiotRunParam.is_wake_up_flag && !gCiasAiotRunParam.stop_collect_pcm_flag)
#endif
    {
        if (gCiasAiotRunParam.is_vad_on_flag)
        {
            gCiasAiotRunParam.vad_start_pcm_frame_count++;
        }
        if (xStreamBufferIsFull(gCiasAiotRunParam.pcm_compress_stream_buffer)) // 缓存满开始清老的数据
        {
            // mprintf("wakeup_pcm_frame_len is full, remove pcm data: %d frame\r\n", PCM_MSG_STREAM_NUM - PCM_ALG_ROOLBACK_FRAME_LEN);

            for (int i = 0; i < (PCM_MSG_STREAM_NUM - PCM_ALG_ROOLBACK_FRAME_LEN); i++)
            {
                int rx_size = xStreamBufferReceiveFromISR(gCiasAiotRunParam.pcm_compress_stream_buffer, speex_src_temp_buf, PCM_ALG_FRAME_LEN, pdMS_TO_TICKS(5));
                if (rx_size != PCM_ALG_FRAME_LEN)
                {
                    mprintf("roll back pcm_compress_stream_buf rcv error1\r\n");
                }
                else
                {
                    gCiasAiotRunParam.wake_up_pcm_frame_count--;
                }
            }
        }
        gCiasAiotRunParam.wake_up_pcm_frame_count++;
        if (gCiasAiotRunParam.pcm_compress_stream_buffer)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            uint8_t *p_upload_src = NULL;
            if (gCiasAiotFuncParam.upload_audio_by_denoise) // 上传降噪音频
            {
                if(ciss_get(CI_SS_AEC_WORK_STATE))  //AEC处理状态上传AEC处理后，降噪前的数据
                {
                    p_upload_src = p_wrapfft_audio->asr_src_data;
                }
                else
                {
                    p_upload_src = right;   //非AEC处理状态上传降噪后的音频
                }
                ret = xStreamBufferSendFromISR(gCiasAiotRunParam.pcm_compress_stream_buffer, (int8_t *)p_upload_src, PCM_ALG_FRAME_LEN, &xHigherPriorityTaskWoken);
            }
            else
            {
                //上传降噪之前，AEC处理后的数据
                ret = xStreamBufferSendFromISR(gCiasAiotRunParam.pcm_compress_stream_buffer, (int8_t *)p_wrapfft_audio->asr_src_data, PCM_ALG_FRAME_LEN, &xHigherPriorityTaskWoken);
            }
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            if (ret != PCM_ALG_FRAME_LEN)
            {
                mprintf("xSpeexRecordStreamBuffer send error, send len = %d\r\n", ret);
            }
        }
    }
#endif
#if USE_IIS1_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
#if USE_IIS1_OUT_PRE_RSLT_AUDIO
    uint32_t write_pcm_addr = 0;
    cm_get_pcm_buffer(PLAY_PRE_AUDIO_CODEC_ID, &write_pcm_addr, 0); // TODO HSL
    int16_t *pcm_data_p = (int16_t *)write_pcm_addr;
    for (int i = 0; i < num; i++)
    {
#if USE_DENOISE_NN_RTC
        if (is_convert_left_right)
        {
            pcm_data_p[2 * i] = right[i];
            pcm_data_p[2 * i + 1] = left[i];
        }
        else
#endif
        {
            pcm_data_p[2 * i] = left[i];
            pcm_data_p[2 * i + 1] = right[i];
        }
    }
    if (0 == write_pcm_addr)
    {
        return;
    }
    cm_write_codec(PLAY_PRE_AUDIO_CODEC_ID, (void *)write_pcm_addr, 0);
#endif

#if USE_HP_OUT_PRE_RSLT_AUDIO
    uint32_t write_pcm_addr_cpy = 0;
    cm_get_pcm_buffer(PLAY_CODEC_ID, &write_pcm_addr_cpy, 0); // TODO HSL
    int16_t *pcm_data_p_cpy = (int16_t *)write_pcm_addr_cpy;
    for (int i = 0; i < num; i++)
    {
#if USE_DENOISE_NN_RTC
        if (is_convert_left_right)
        {
            pcm_data_p_cpy[2 * i] = right[i];
            pcm_data_p_cpy[2 * i + 1] = left[i];
        }
        else
#endif
        {
            pcm_data_p_cpy[2 * i] = left[i];
            pcm_data_p_cpy[2 * i + 1] = right[i];
        }
    }
    if (0 == write_pcm_addr_cpy)
    {
        return;
    }
    cm_write_codec(PLAY_CODEC_ID, (void *)write_pcm_addr_cpy, 0);
#endif

    if (0 == sg_init_tmp_str.send_data_cnt)
    {
#if USE_IIS1_OUT_PRE_RSLT_AUDIO
        cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
#endif
#if USE_HP_OUT_PRE_RSLT_AUDIO && !NET_AUDIO_PLAY_BY_PCM
        cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
#endif
    }
#endif
    sg_init_tmp_str.send_data_cnt++;
    sg_init_tmp_str.write_data_cnt++;
}

#else

#endif
/**
 * @brief 写数据到发送端
 *
 * @param rslt 处理结果的起始指针
 * @param origin 原始数据的起始指针
 *  此函数不可用于中断
 */
void audio_pre_rslt_write_data(int16_t *left, int16_t *right, uint32_t wrapfft_audio_addr)
{
    ci_wrapfft_audio *p_wrapfft_audio = (ci_wrapfft_audio *)wrapfft_audio_addr;   //p_wrapfft_audio->asr_src_data);
#if IIS_CHANNEL_ENG_CALC_EANBLE
    audio_eng_calc(p_wrapfft_audio);
#endif
#if USE_AUDIO_UPLOAD_BY_IIS
    audio_pre_rslt_upload_by_iis(left,right,p_wrapfft_audio);
#elif USE_AUDIO_UPLOAD_BY_HPOUT
    audio_pre_rslt_upload_by_hpout(left,right,p_wrapfft_audio);
#elif USE_AUDIO_UPLOAD_BY_UART
    audio_pre_rslt_upload_by_uart(left,right,p_wrapfft_audio);
#else
#endif
}

/**
 * @brief 语音前处理输出停止
 *
 */
void audio_pre_rslt_stop(void)
{
#if USE_IIS1_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
#else

#if USE_IIS1_OUT_PRE_RSLT_AUDIO
    cm_stop_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
#endif

#if USE_HP_OUT_PRE_RSLT_AUDIO
    cm_stop_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
#endif
#endif

}

/**
 * @brief 语音前处理输出开始
 *
 */
void audio_pre_rslt_start(void)
{
#if USE_IIS1_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO

#else

#if USE_IIS1_OUT_PRE_RSLT_AUDIO
    cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
#endif
#if USE_HP_OUT_PRE_RSLT_AUDIO
    cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
#endif
#endif

}

/********** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
