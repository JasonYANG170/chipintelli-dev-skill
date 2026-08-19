

#include "ci_uart_audio_data_handle.h"
// #include "audio_play_process.h"
#include "stream_buffer.h"
#include "semphr.h"
#include "ci_log.h"
#include "status_share.h"

#if USE_UART_SEND_SPEEX_ENABLE || USE_UART_RCV_PLAY_SPEEX_ENABLE
#include "ci110x_speex.h"
#include "sb_celp.h"
#include "nb_celp.h"
#include "audio_pre_rslt_out.h"
#include "ci_flash_record_play_handle.h"

#define UART_SPEEX_FRAME_LEN 43               // speex编码后每帧长度
#define SPEEX_PCM_FRAME_LEN NB_FRAME_SIZE * 2 // speex单次编码160个点(short),320字节, NB_FRAME_SIZE = 160

extern SpeexMode cias_speex_wb_mode;
extern const SpeexSBMode sb_wb_mode;
extern void *sb_encoder_init(const SpeexMode *m);
extern void sb_encoder_destroy(void *state);
extern int sb_decode(void *state, SpeexBits *bits, void *vout);
extern int sb_encoder_ctl(void *state, int request, void *ptr);
extern int sb_decoder_ctl(void *state, int request, void *ptr);
extern int sb_encode(void *state, void *vin, SpeexBits *bits);
extern void *sb_decoder_init(const SpeexMode *m);
extern void sb_decoder_destroy(void *state);
ci_speex_t *g_speex_encode_hander = NULL;
ci_speex_t *g_speex_decode_hander = NULL;

uint8_t speex_init_flag = 0;
int8_t  g_audio_rx_temp_buf[UART_SPEEX_FRAME_LEN] = {0};       // 串口接收数据缓存buf
uint16_t g_audio_rx_temp_index = 0;                            // 串口接收音频数据buf 索引
int16_t g_speex_dec_frame_buf[SPEEX_PCM_FRAME_LEN*2] = {0};      // speex解码音频数据缓存buf
int8_t g_recv_speex_temp_buf[UART_SPEEX_FRAME_LEN] = {0};      // 接收speex播放数据缓存

StreamBufferHandle_t gSpeexPlayStreamBuffer = NULL;            // 播放数据接收队列
StreamBufferHandle_t gSpeexEncodeStreamBuffer = NULL;          // 录音数据接收独队列
QueueHandle_t gSpeexdecodeDataQueue = NULL;                    //speex解码数据队列

// speex解码任务
bool speex_request_play_flag = false;         //请求编码数据标志
_XIF_ int speex_decode_task(void)
{
    uint32_t decode_len = 0;
    gSpeexdecodeDataQueue = xQueueCreate(RCV_SPEEX_DECODE_LEN, UART_SPEEX_FRAME_LEN);    //队列长度至少200
    mprintf("g_speex_decode_hander->_frame_size = %d\r\n", g_speex_decode_hander->_frame_size);
#if 1
/*     vTaskDelay(pdMS_TO_TICKS(5000));
    for (size_t i = 0; i < UART_SPEEX_FRAME_LEN; i++)
    {
        g_recv_speex_temp_buf[i] = 0;
    }
    g_recv_speex_temp_buf[0] = 0x2A;
    speex_bits_read_from(&g_speex_decode_hander->_bits, &g_recv_speex_temp_buf[1], UART_SPEEX_FRAME_LEN - 1);
    memset(g_speex_dec_frame_buf, 0, SPEEX_PCM_FRAME_LEN * sizeof(short));
        //mprintf("*");
    decode_len = speex_decode_int(g_speex_decode_hander->_decoder, &g_speex_decode_hander->_bits, g_speex_dec_frame_buf);
    for (size_t i = 0; i < SPEEX_PCM_FRAME_LEN*2; i++)
    {
        mprintf("%d ", g_speex_dec_frame_buf[i]);
    }
    mprintf("\r\n"); */
    while (1)
    {
        if(rtc_play_status == PLAY_STOP)
        {
           xQueueReset(gSpeexdecodeDataQueue);
        }
        xQueueReceive(gSpeexdecodeDataQueue, g_recv_speex_temp_buf, portMAX_DELAY);
        if(uxQueueSpacesAvailable(gSpeexdecodeDataQueue) >= RCV_SPEEX_DECODE_LEN - RCV_SPEEX_DECODE_REQ_LEN)
        {
            speex_request_play_flag = true;
        }
        else
        {
            speex_request_play_flag = false;
        }
        // speex解码
        if (g_recv_speex_temp_buf[0] != 0x2a) // 编码后有效音频数据长度为42字节
        {
            mprintf("error data..\r\n");
            continue;
        }
        speex_bits_read_from(&g_speex_decode_hander->_bits, &g_recv_speex_temp_buf[1], UART_SPEEX_FRAME_LEN - 1);
        memset(g_speex_dec_frame_buf, 0, SPEEX_PCM_FRAME_LEN * sizeof(short));
        //mprintf("*");
        decode_len = speex_decode_int(g_speex_decode_hander->_decoder, &g_speex_decode_hander->_bits, g_speex_dec_frame_buf);
        #if USE_PRE_RSLT_AUDIO_SWITCH_KEY_ENABLE
        extern uint8_t rtc_audio_out_switch_key_func_status;
        if(!rtc_audio_out_switch_key_func_status)   //按键切换到串口输出
        {
            audio_pre_rslt_write_data_from_uart((uint32_t)g_speex_dec_frame_buf, decode_len * 2);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        else
        #endif
        {
            int8_t *src_addr = (int8_t *)g_speex_dec_frame_buf;
            int ret = xStreamBufferSend(gSpeexPlayStreamBuffer, src_addr, SPEEX_PCM_FRAME_LEN * sizeof(int16_t), 1000);     //IF_16K_DOWNSAMPLE_TO_8K，16k降8K采样后，一包640字节有2帧音频
            if (ret != SPEEX_PCM_FRAME_LEN * sizeof(int16_t))
            {
                mprintf("gSpeexPlayStreamBuffer send error: send len = %d\r\n", ret);
            }
        }
    }
#endif
}

// 播放speex解码后数据任务
extern bool rtc_play_local_flag;
_XIF_ int speex_play_task(void)
{
    uint32_t stream_aviable_len = 0;
    uint32_t write_pcm_addr_cpy = 0;
    gSpeexPlayStreamBuffer = xStreamBufferCreate(ORIGIN_PCM_FRAME_LEN * RCV_PCM_PLAY_LEN, ORIGIN_PCM_FRAME_LEN);
    int16_t speex_play_data[512] = {0};
    if (gSpeexPlayStreamBuffer == NULL)
    {
        mprintf("gSpeexPlayStreamBuffer create error...\r\n");
        // 处理错误情况
    }
    while (1)
    {
        stream_aviable_len = xStreamBufferBytesAvailable(gSpeexPlayStreamBuffer);
        if(rtc_play_status == PLAY_STOP)
        {
            xStreamBufferReset(gSpeexPlayStreamBuffer); 
        } 
        if(rtc_play_local_flag)
        {
            if (stream_aviable_len >= ORIGIN_PCM_FRAME_LEN)     //采样率16k一帧512个字节.
            {
                memset(speex_play_data, 0, ORIGIN_PCM_FRAME_LEN*2);
                int rx_size = xStreamBufferReceive(gSpeexPlayStreamBuffer, speex_play_data, ORIGIN_PCM_FRAME_LEN, portMAX_DELAY);
                if (rx_size != ORIGIN_PCM_FRAME_LEN)
                {
                    mprintf("gSpeexPlayStreamBuffer rcv error\r\n");
                }
                else
                {
                    cm_get_pcm_buffer(PLAY_CODEC_ID, &write_pcm_addr_cpy, portMAX_DELAY); // TODO HSL
                    if (0 == write_pcm_addr_cpy)
                    {
                        mprintf("full 2\r\n");
                        continue;
                    }
                    int16_t *pcm_data_p_cpy = (int16_t *)write_pcm_addr_cpy;
                    for (int i = 0; i < ORIGIN_PCM_FRAME_LEN / 2 ; i++) // 数据填充左右声道pcm
                    {
                        pcm_data_p_cpy[2 * i] = speex_play_data[i];
                        pcm_data_p_cpy[2 * i + 1] = speex_play_data[i];
                    }
                   
                    //mprintf("-");
                    cm_write_codec(PLAY_CODEC_ID, (void *)write_pcm_addr_cpy, 0);
                }
            }
        }
        else
        {
            if (stream_aviable_len >= ORIGIN_PCM_FRAME_LEN / 2)     //采样率8k一帧256个字节.
            {
                memset(speex_play_data, 0, ORIGIN_PCM_FRAME_LEN);
                int rx_size = xStreamBufferReceive(gSpeexPlayStreamBuffer, speex_play_data, ORIGIN_PCM_FRAME_LEN / 2, portMAX_DELAY);
                if (rx_size != ORIGIN_PCM_FRAME_LEN / 2)
                {
                    mprintf("gSpeexPlayStreamBuffer rcv error\r\n");
                }
                else
                {
                    cm_get_pcm_buffer(PLAY_CODEC_ID, &write_pcm_addr_cpy, portMAX_DELAY); // TODO HSL
                    if (0 == write_pcm_addr_cpy)
                    {
                        mprintf("full 1\r\n");
                        continue;
                    }
                    int16_t *pcm_data_p_cpy = (int16_t *)write_pcm_addr_cpy;
                    for (int i = 0; i < ORIGIN_PCM_FRAME_LEN / 2 / 2; i++) // 8k采音率的1帧数据需复制填充为16k，不复制会出现音频播放加快
                    {
                        pcm_data_p_cpy[4 * i] = speex_play_data[i];
                        pcm_data_p_cpy[4 * i + 1] = speex_play_data[i];
                        pcm_data_p_cpy[4 * i + 2] = speex_play_data[i];
                        pcm_data_p_cpy[4 * i + 3] = speex_play_data[i];
                    }
                   
                    mprintf("-");
                    cm_write_codec(PLAY_CODEC_ID, (void *)write_pcm_addr_cpy, 0);
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
// speex编码任务
_XIF_ int speex_encode_task(void)
{
    uint32_t encode_len = 0;
    uint32_t stream_aviable_len = 0;
    uint32_t rx_size = 0;
    int8_t speex_decode_buf[UART_SPEEX_FRAME_LEN] = {0};
    int8_t speex_rx_temp[NB_FRAME_SIZE * 2 * 2] = {0};
    gSpeexEncodeStreamBuffer = xStreamBufferCreate(ORIGIN_PCM_FRAME_LEN * 6, 320);
    CI_ASSERT(gSpeexEncodeStreamBuffer, "\n");
    while (1)
    {
        stream_aviable_len = xStreamBufferBytesAvailable(gSpeexEncodeStreamBuffer);
        if (stream_aviable_len >= NB_FRAME_SIZE * 2 * 2)
        {
            memset(speex_rx_temp, 0, sizeof(speex_rx_temp));
            int rx_size = xStreamBufferReceive(gSpeexEncodeStreamBuffer, speex_rx_temp, NB_FRAME_SIZE * 2 * 2, portMAX_DELAY);
            if (rx_size != NB_FRAME_SIZE * 2 * 2)
            {
                mprintf("gSpeexEncodeStreamBuffer rcv error\r\n");
            }
            else
            {
                for (int k = 0; k < NB_FRAME_SIZE / 160; k++)
                {
                    memset(speex_decode_buf, 0, sizeof(speex_decode_buf));
                    encode_len = cias_speex_compressed_data(g_speex_encode_hander, speex_rx_temp, speex_decode_buf); // 编码音频数据-每次编码20ms数据320个点-640字节
                    //mprintf(".");
                    // audio_pre_rslt_write_data_from_uart((uint32_t)speex_decode_buf, encode_len);   //数据串口输出
                    extern StreamBufferHandle_t gFlashEncodeStreamBuffer;
                    int ret = xStreamBufferSend(gFlashEncodeStreamBuffer, speex_decode_buf, encode_len, 20);      //数据存flash
                    if (ret != encode_len)
                    {
                        mprintf("gFlashEncodeStreamBuffer send error: send len = %d, avaible %d\r\n", ret, xStreamBufferBytesAvailable(gFlashEncodeStreamBuffer));
                    }
                    vTaskDelay(pdMS_TO_TICKS(16)); // 必须加延时，不然会丢数据
                }
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(16));
        }
    }
}
// speex音频任务处理初始化
_XIF_ bool audio_speex_task_init(void)
{
#if USE_UART_SEND_SPEEX_ENABLE || USE_UART_RCV_PLAY_SPEEX_ENABLE
    cias_speex_wb_mode.mode = &sb_wb_mode;
    cias_speex_wb_mode.query = wb_mode_query;
    cias_speex_wb_mode.modeName = pvPortMalloc(strlen("wideband (sub-band CELP)"));
    memcpy(cias_speex_wb_mode.modeName, "wideband (sub-band CELP)", strlen("wideband (sub-band CELP)"));
    cias_speex_wb_mode.modeID = 1;
    cias_speex_wb_mode.bitstream_version = 4;
    cias_speex_wb_mode.enc_init = sb_encoder_init;
    cias_speex_wb_mode.enc_destroy = sb_encoder_destroy;
    cias_speex_wb_mode.enc = sb_encode;
    cias_speex_wb_mode.enc_ctl = sb_encoder_ctl;
    cias_speex_wb_mode.dec_init = sb_decoder_init;
    cias_speex_wb_mode.dec_destroy = sb_decoder_destroy;
    cias_speex_wb_mode.dec = sb_decode;
    cias_speex_wb_mode.dec_ctl = sb_decoder_ctl;
#if USE_UART_SEND_SPEEX_ENABLE
    g_speex_encode_hander = ci_speex_encode_create(); // 初始化编码speex
    if (NULL == g_speex_encode_hander)
    {
        mprintf("g_speex_encode_hander is null\r\n");
        return false;
    }
    g_speex_encode_hander->ci_speex_mode = CI_SPEEX_INT;
    xTaskCreate(speex_encode_task, "speex_encode_task", 600, NULL, 4, NULL);
#endif
#if USE_UART_RCV_PLAY_SPEEX_ENABLE
    g_speex_decode_hander = ci_speex_decode_create(); // 初始化解码speex

    if (NULL == g_speex_decode_hander)
    {
        mprintf("g_speex_decode_hander is null\r\n");

        return false;
    }
    g_speex_decode_hander->ci_speex_mode = CI_SPEEX_INT;
    xTaskCreate(speex_decode_task, "speex_decode_task", 400, NULL, 5, NULL);
    xTaskCreate(speex_play_task, "speex_play_task", 512, NULL, 4, NULL);
#endif

#endif
    speex_init_flag = 1;
    return true;
}
#endif
