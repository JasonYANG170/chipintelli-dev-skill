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
#include "codec_manager.h"
#include "timers.h"
#include "cias_aiot_protocol.h"
#include "user_config.h"
#include "g722_enc_dec.h"


void codec_init()
{
    void *pcm_buffer = NULL;
    cm_pcm_buffer_info_t pcm_buffer_info;
    pcm_buffer_info.play_buffer_info.block_num = 1;
    pcm_buffer_info.play_buffer_info.buffer_num = 4;
    pcm_buffer_info.play_buffer_info.block_size = 640;
    pcm_buffer_info.play_buffer_info.buffer_size = pcm_buffer_info.play_buffer_info.block_size * pcm_buffer_info.play_buffer_info.block_num;
    int pcm_buffer_total_size = pcm_buffer_info.play_buffer_info.buffer_size*pcm_buffer_info.play_buffer_info.buffer_num;
    pcm_buffer = pvPortMalloc(pcm_buffer_total_size);
    pcm_buffer_info.play_buffer_info.pcm_buffer = pcm_buffer;
    cm_config_pcm_buffer(PLAY_CODEC_ID, CODEC_OUTPUT, &pcm_buffer_info);

    audio_format_info_t audio_format_info;
    audio_format_info.samprate = 16000;
    audio_format_info.nChans = 3;
    audio_play_hw_start(DISABLE, &audio_format_info);
}

void encoder_init()
{

}

void decoder_init()
{
    
}

void g722_test_task(void)
{
    codec_init();
    uint32_t p_file_addr, p_file_size, file_offset = 0;
    get_dnn_addr_by_id(60006, &p_file_addr, &p_file_size);
    mprintf("get_dnn_addr %x, %d\r\n",p_file_addr, p_file_size);
    vTaskDelay(pdMS_TO_TICKS(5000));
    int16_t rx_temp[320] = {0};
    uint8_t g722_data[160];
    G722DecoderState* dec_state = NULL;
    dec_state = WebRtc_g722_decode_init(dec_state, G722_BITRATE_64k, 0);
    while(file_offset < p_file_size)
    {
        post_read_flash(g722_data, p_file_addr + file_offset, 160);
        WebRtc_g722_decode(dec_state, rx_temp, g722_data, 160);
        audio_play_hw_write_data(rx_temp, 640);
/*         for(int i = 0; i < 320; i++)
            mprintf(" %d",rx_temp[i]);
        mprintf("\r\n"); */
        vTaskDelay(pdMS_TO_TICKS(19));
        file_offset += 160;
    }
    G722EncoderState* enc_state = NULL;
    enc_state = WebRtc_g722_encode_init(enc_state, G722_BITRATE_64k, 0);
     get_dnn_addr_by_id(60005, &p_file_addr, &p_file_size);
    mprintf("get_dnn_addr %x, %d\r\n",p_file_addr, p_file_size);
 
    while(file_offset < p_file_size)
    {
        post_read_flash(rx_temp, p_file_addr + file_offset, 320);
        WebRtc_g722_encode(enc_state, g722_data, rx_temp, 320);
/*         for(int i = 0; i < 320; i++)
            mprintf(" %d",rx_temp[i]);
        mprintf("\r\n"); */
        file_offset += 640;
    }
    while(1);
}

