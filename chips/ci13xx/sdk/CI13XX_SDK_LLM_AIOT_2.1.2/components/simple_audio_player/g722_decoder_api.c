
#include "user_config.h"
#if	SIMPLE_AUDIO_PLAYER_ENABLE
#include "ci_log.h"
#include "simple_audio_player.h"
#include "g722_enc_dec.h"

static void *decoder_init(void)
{
    return WebRtc_g722_decode_init(NULL, G722_BITRATE_64k, 0);
}

static void docoder_deinit(void* decoder_haldle)
{
    WebRtc_g722_decode_release(decoder_haldle);
}

static int get_info(void* decoder_handle, uint8_t *data, int *bytes_left, uint8_t *pcm_buf, audio_format_info_t *format_info)
{
    format_info->samprate = 16000;
    format_info->bits_per_sample = 16;
    format_info->channels = 1;
    format_info->samples_per_frame = 320;
    format_info->src_data_size = 0;
    return 0;
}

static int decode_one_frame(void* decoder_handle, uint8_t *data, int *bytes_left, uint8_t *pcm_buf, uint32_t *pcm_data_size)
{
    int ret = 0;

    int decoded_size = WebRtc_g722_decode(decoder_handle, pcm_buf, data, 160); // 320 samples
    if (pcm_data_size)
    {
        *pcm_data_size = decoded_size*sizeof(short);
    }
    *bytes_left -= 160;
    return ret;
}


#if NET_AUDIO_PLAY_BY_G722
register_audio_decoder(g722, decoder_init, docoder_deinit, get_info, decode_one_frame); 
#endif
#endif
