
/*
  2022022301,lwt,添加fe_psd计算配置
  2020.06.28，lwt,
 *****************************************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ci_audio_wrapfft.h"
#include "ci_alg_malloc.h"
#include "ci_log.h"
#include "status_share.h"
#include "ci_fft.h"

#undef M_PI
#define M_PI (3.14159265358979323846f)

#undef FIX_SCALE
#define FIX_SCALE (1.0f / 32767.0f)

#define CI_AUDIO_WRAPFFT_VERSION 10103
#define INTERNAL_DEVELOPER_VERSION 2022022301

extern void config_use512fft_forasr(void);

int ci_stft_istft_module_version(void)
{
    return CI_AUDIO_WRAPFFT_VERSION;
}

int ci_stft_istft_module_developer_version(void)
{
    return INTERNAL_DEVELOPER_VERSION;
}

/*
static inline void hanming_window( float *w, int wlen )
{
    int i = 0;
    float step = 2*M_PI / (float)wlen;
    float v = 0.5f * M_PI / (float)wlen;
    for (i = 0; i < wlen; i++)
    {
        w[i] = 0.54f - 0.46f * cosf(v);
        v = v + step;
    }
}
*/

/*static void sin_window( float *w, int wlen )//存窗长的一半
{
    int i = 0;
    float step = M_PI / (float)wlen;
    float v = 0.5f * M_PI / (float)wlen;
	//for (i=0; i<wlen; i++)
#if USE_HALF_FRAME_WIN
     for (i=0; i<wlen/2; i++)
    {
        w[i] = sinf( v );
        v = v + step;
    }
#else
    for (i=0; i<wlen; i++)
    {
        w[i] = sinf( v );
        v = v + step;
    }
#endif
}*/
static int hamming_window_fixedpoint(short *out_ptr, int frame_len)
{

    float alpha = 0.46f;

    for (int i = 0; i < frame_len / 2; i++)
    {
        float x = (1 - alpha) - alpha * MASK_ROM_LIB_FUNC->newlibcfunc.cosf_p(2 * 3.1415926f * i / (frame_len - 1));
        int y = (int)(x * 32767.0f);
        if (y > 32767)
        {
            y = 32767;
        }
        else if (y < -32767)
        {
            y = -32767;
        }
        out_ptr[i] = (short)y;
    }

    return 0;
}

_XIF_ void *ci_stft_create(void *module_config, void *wrapfft_audio_t)
{

    stft_istft_config_t *stft_istft_config = (stft_istft_config_t *)module_config;
    ci_wrapfft_audio *h = (ci_wrapfft_audio *)wrapfft_audio_t;
    if (!h)
    {
        return NULL;
    }
    int i = 0;
    int ret = 0;

    h->module_config = stft_istft_config;
    h->fft_size = stft_istft_config->fft_size;

    int mic_channel_num = h->mic_channel_num;
    int ref_channel_num = h->ref_channel_num;

    int frame_size = stft_istft_config->frame_size;

    int frame_shift = stft_istft_config->frame_shift;
    h->frame_overlap_size = frame_size - frame_shift;

    int fft_size = stft_istft_config->fft_size; //512;
    int fft_frm_size = stft_istft_config->fft_frm_size;

    //使用半长的窗
    h->fix_window = (short *)ci_algbuf_malloc(frame_size / 2 * sizeof(short));
    hamming_window_fixedpoint(h->fix_window, frame_size);
    if (stft_istft_config->fe_psd_enable) //asr_fe才需要计算psd
    {
        for(int i = 0;i<stft_istft_config->psd_compute_channel_num;i++)
        {
            h->psd[i] = (float *)ci_algbuf_malloc(257 * sizeof(float));
            memset(h->psd[i], 0, 257 * sizeof(float));
        }
       
    }

    ret = ci_software_fft_init(&h->S, fft_frm_size);

    h->fft_buf_tmp = (float *)ci_algbuf_malloc((fft_frm_size) * sizeof(float));

    //根据通道数确认fft中需要用到的buffer

    for (i = 0; i < mic_channel_num; i++) //mic_channels,目前通道数最多支持4个通道
    {
        h->frame_mic_data[i] = (short *)ci_algbuf_calloc(frame_size, sizeof(short));
        h->fft_mic_out[i] = (float *)ci_algbuf_calloc(fft_size * 2, sizeof(float));
    }

    for (i = 0; i < ref_channel_num; i++) //ref_channels,目前通道数最多支持2个通道
    {
        h->frame_ref_data[i] = (short *)ci_algbuf_calloc(frame_size, sizeof(short));
        h->fft_ref_out[i] = (float *)ci_algbuf_calloc(fft_size * 2, sizeof(float));
    }

    return h;
}

void *ci_istft_create(void *module_config, void *wrapfft_audio_t)
{
    stft_istft_config_t *stft_istft_config = (stft_istft_config_t *)module_config;
    ci_wrapfft_audio *h = (ci_wrapfft_audio *)wrapfft_audio_t;
    int i = 0;
    int ret = 0;

    int frame_size = stft_istft_config->frame_size;
    int frame_shift = stft_istft_config->frame_shift;
    int fft_size = stft_istft_config->fft_size;
    int result_out_channels = stft_istft_config->result_out_channel;
    int fft_frm_size = stft_istft_config->fft_frm_size;

    h->ifft_result_buf = (float *)ci_algbuf_calloc(fft_size * 2, sizeof(float));

    for (i = 0; i < result_out_channels; i++)
    {
        (h->ifft_tmp_buf)[i] = (float *)ci_algbuf_calloc(frame_shift, sizeof(float));
        h->dst[i] = (short *)ci_algbuf_calloc(frame_shift, sizeof(short));
    }

    //综合窗输出后进行增益缩放时需要的增益值。
    h->adjust_gain = (float *)ci_algbuf_malloc(frame_shift * sizeof(float));

    //半长窗
    int first_index = frame_size / 2 - (frame_size - 2 * frame_shift);
    int first_cur_start_index = h->frame_overlap_size - frame_shift; //全长窗
    float scale = FIX_SCALE;

    for (i = 0; i < first_index; i++)
    {

        float tmp = scale * h->fix_window[i + first_cur_start_index];
        float sum_tmp = tmp * tmp;
        tmp = scale * h->fix_window[frame_shift - 1 - i];
        sum_tmp += tmp * tmp;
        h->adjust_gain[i] = 1.0f / sum_tmp;
    }
    for (i = first_index; i < frame_shift; i++)
    {
        int j = i - first_index;

        float tmp = scale * h->fix_window[frame_size / 2 - 1 - j];
        float sum_tmp = tmp * tmp;
        tmp = scale * h->fix_window[frame_size / 2 - frame_shift - 1 - j];
        sum_tmp += tmp * tmp;
        h->adjust_gain[i] = 1.0f / sum_tmp;
    }
}

static int ci_wrapfft_fft(void *handle, short *pcm, short *frame_date, float *fft)
{
    int ret = 0;
    int i = 0;
    short *x = NULL;

    ci_wrapfft_audio *h = (ci_wrapfft_audio *)handle;
    stft_istft_config_t *module_config = h->module_config;

    int frame_shift = module_config->frame_shift;
    int frame_size = module_config->frame_size;

    int fft_frm_size = module_config->fft_frm_size; //module_config->fft_frm_size

    //滑窗
    x = frame_date;

    memcpy(x, x + frame_shift, h->frame_overlap_size * sizeof(short)); //滑窗
    memcpy(x + h->frame_overlap_size, pcm, frame_shift * sizeof(short));

    float scale = FIX_SCALE;
    short *fix_window = h->fix_window; //(short*)window_data;

    int win_len = frame_size;
    int win_len_half = win_len / 2;
    //预加重
    if (module_config->time_pre_emphasis_enable)
    {
        config_use512fft_forasr(); //设置不使用频域的预加重
        
        status_t vad_state = ciss_get(CI_SS_VAD_STATE); //asr_vad 获取vad_start时刻
        static int vad_end_marked = 0;
        if ((CI_SS_VAD_END == vad_state) && (0 == vad_end_marked))
        {
            vad_end_marked = 1;
            h->sg_last_pcm = 0;
        }
        else if (CI_SS_VAD_START == vad_state)
        {
            vad_end_marked = 0;
        }
        short last_pcm = h->sg_last_pcm;
        for (i = 0; i < win_len; i++)
        {
            h->fft_buf_tmp[i] = x[i] - 0.94f * last_pcm;
            last_pcm = x[i];
        }
        h->sg_last_pcm = x[159];
		for (i = 0; i < win_len_half; i++)
	    {
	        float tmp = scale * fix_window[i];
	        h->fft_buf_tmp[i] *= tmp;
	        h->fft_buf_tmp[win_len - 1 - i] *= tmp;
	    }
    }
	else
	{
	    //加窗
	    for (i = 0; i < win_len_half; i++)
	    {
	        float tmp = scale * fix_window[i];
	        h->fft_buf_tmp[i] = x[i] * tmp;
	        h->fft_buf_tmp[win_len - 1 - i] = x[win_len - 1 - i] * tmp;
	    }
	}
    for (i = win_len; i < fft_frm_size; i++)
    {
        h->fft_buf_tmp[i] = 0.0f;
    }

    ret = ci_software_fft(&h->S, h->fft_buf_tmp, fft);

    for (int i = 0; i < HPF_CUT_OFF_FREQ; i++)
    {
        //fft[1] = 0.0f; //直流分量的虚部强制为0
        fft[2 * i] = 0;
        fft[2 * i + 1] = 0;
    }

    if (module_config->downsampled_enable) //fft点数为1024 此时数据的采样率是32k采样率，需要滤除高频
    {
        for (int i = 257; i < 512; i++)
        {
            fft[2 * i] = 0;
            fft[2 * i + 1] = 0;
        }
    }

   

    if (ret != 0)
    {
        return -__LINE__;
    }

    return 0;
}
static int ci_wrapfft_ifft(void *handle, float *frame_date, float *fft, short *pcm_out)
{
    int i = 0;
    int j = 0;
    int tt = 0;
    int ret = 0;
    float temp = 0.0f;
    short pcm_out_start_index = 0;
    short first_num = 0;

    ci_wrapfft_audio *h = (ci_wrapfft_audio *)handle;

    if (!h || !pcm_out || !fft)
    {
        return (__LINE__);
    }
    stft_istft_config_t *module_config = h->module_config;
    int fft_frm_size = module_config->fft_frm_size;

    ret = ci_software_ifft(&h->S, fft, h->ifft_result_buf);

    float scale = FIX_SCALE;
    short *fix_window = h->fix_window;
    int frame_shift = module_config->frame_shift;
    int frame_size = module_config->frame_size;

    pcm_out_start_index = h->frame_overlap_size - frame_shift;
    first_num = frame_size / 2 - (frame_size - frame_shift * 2);
    //半窗定点非50%帧移的合窗，需分区段处理
    for (i = 0; i < frame_shift; i++)
    {
        if (i < first_num)
        {
            temp = fix_window[i + pcm_out_start_index] * scale;
            temp *= h->ifft_result_buf[i + pcm_out_start_index];
            temp += frame_date[i];
            temp *= h->adjust_gain[i];
        }
        else
        {
            j = i - first_num;
            temp = fix_window[frame_size / 2 - 1 - j] * scale;
            temp *= h->ifft_result_buf[i + pcm_out_start_index];
            temp += frame_date[i];
            temp *= h->adjust_gain[i];
        }
        tt = (int)temp;
        if (tt > 32767)
        {
            pcm_out[i] = 32767;
        }
        else if (tt < -32768)
        {
            pcm_out[i] = -32768;
        }
        else
        {
            pcm_out[i] = (short)tt;
        }
        frame_date[i] = (h->ifft_result_buf[i + h->frame_overlap_size] * fix_window[frame_shift - 1 - i] * scale);
    }
    if (module_config->downsampled_enable)
    {
        for (int i = 0; i < module_config->frame_shift/2; i++)
        {
            pcm_out[i] = pcm_out[2 * i];
        }
    }

    return 0;
}

int ci_stft_deal(void *handle, void *wrapfft_audio)
{
    int ret = 0;
    ci_wrapfft_audio *st = (ci_wrapfft_audio *)wrapfft_audio;
    stft_istft_config_t *module_config = st->module_config;

    //mic_in_mic_fft_out
    for (int i = 0; i < st->mic_channel_num; i++)
    {

        ret = ci_wrapfft_fft(st, st->mic[i], st->frame_mic_data[i], st->fft_mic_out[i]);

        if (ret != 0)
        {
            return __LINE__;
        }
    }

    if (module_config->fe_psd_enable) //asr_fe处理的数据是16k采样率，采用的是512stft需要计算psd
    {
        short psd_index = 257;
        if(module_config->psd_compute_channel_num <= st->mic_channel_num)
        {
            for (int i = 0; i < module_config->psd_compute_channel_num; i++)
            {
                float* fft = st->fft_mic_out[i];
                float* psd = st->psd[i];
                for (int j = 0; j < psd_index; j++)
                {
                   psd[j] = fft[2 * j] * fft[2 * j] + fft[2 * j + 1] * fft[2 * j + 1];     
                }  
            }
        }
        else
        {
            ci_logdebug(LOG_SSP_MODULE, "psd_module_config_error:\n");
        }
        
    }

    //ref_in_mic_fft_out
    for (int i = 0; i < st->ref_channel_num; i++)
    {
        ret = ci_wrapfft_fft(st, st->ref[i], st->frame_ref_data[i], st->fft_ref_out[i]);
        if (ret != 0)
        {
            return __LINE__;
        }
    }

    return ret;
}

int ci_istft_deal(void *handle, void *wrapfft_audio)
{
    int ret = 0;

    ci_wrapfft_audio *st = (ci_wrapfft_audio *)wrapfft_audio;
    stft_istft_config_t *module_config = st->module_config;

    for (int i = 0; i < module_config->result_out_channel; i++)
    {
        ret = ci_wrapfft_ifft(st, st->ifft_tmp_buf[i], st->fft_mic_out[i], st->dst[i]);

        if (ret != 0)
        {
            return __LINE__;
        }
    }

    return ret;
}

void ci_stft_istft_destroy(void *handle)
{
    int i = 0;
    ci_wrapfft_audio *h = (ci_wrapfft_audio *)handle;

    if (!h)
    {
        return;
    }

    ci_algbuf_free(h->fix_window);

    ci_algbuf_free(h->fft_buf_tmp);
    //根据通道数确认fft中需要用到的buffer
    int mic_channels = h->mic_channel_num;
    int ref_channels = h->ref_channel_num;

    for (i = 0; i < mic_channels; i++) //mic_channels,目前通道数最多支持4个通道
    {
        ci_algbuf_free(h->frame_mic_data[i]);
        ci_algbuf_free(h->fft_mic_out[i]);
    }

    for (i = 0; i < ref_channels; i++) //ref_channels,目前通道数最多支持2个通道
    {
        ci_algbuf_free(h->frame_ref_data[i]);
        ci_algbuf_free(h->fft_ref_out[i]);
    }
    for (i = 0; i < h->module_config->result_out_channel; i++)
    {
        ci_algbuf_free(h->dst[i]);
    }

    ci_algbuf_free(h->ifft_result_buf);

    for (i = 0; i < h->module_config->result_out_channel; i++)
    {
        ci_algbuf_free(h->ifft_tmp_buf[i]);
    }

    ci_algbuf_free(h);
    h = NULL;
}
