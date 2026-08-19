#include "codec_manager.h"
#include "audio_pre_rslt_out.h"
#include <string.h>
#include "ci_system.h"
#include "ci_codec.h"
#include "ci_dpmu.h"
#include "ci_gpio.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "sdk_default_config.h"
#include "ci_uart.h"
#include "ci_dma.h"
#include "debug_time_consuming.h"
#include "ci_log.h"
#include "timers.h"
#include "stream_buffer.h"
#include "ci_flash_record_play_handle.h"


uint8_t g_local_play_flag = false;   // 本地播放标记
#define BUFFER_NUM (4)

#if IF_16K_DOWNSAMPLE_TO_8K
#define LOWPASS_FILTER_SIZE 4
static const float LOWPASS_FILTER[4] = {0.0167f,0.4833f,0.4833f,0.0167f};

int32_t tmp = 0;
int16_t x_buffer_0 = 0;
int16_t x_buffer[LOWPASS_FILTER_SIZE-1] = {0};
int16_t tmp_buffer[128] = {0};
#endif

typedef struct
{
    audio_pre_rslt_out_init_t init_str;
    //写了多少次数据
    uint32_t write_data_cnt;
    //已经发送了多少次数据，写数据频次是固定的，我们不能改变，只能改变发送数据的频度，
    //如果发送太快，会在mute模式再发送上一个buf，
    //如果发送太慢，会扔掉一个buf不发送
    uint32_t send_data_cnt;
    
    int32_t write_send_sub_slave;//在slave模式下，write和send的差值
    //是否使用硬件tx merge功能
    bool hardware_tx_merge;
}audio_pre_init_tmp_t;

#if USE_UART_SEND_SPEEX_ENABLE
extern StreamBufferHandle_t gSpeexEncodeStreamBuffer;
#endif

#if USE_UART_SEND_PRE_RSLT_AUDIO || USE_PRE_RSLT_AUDIO_SWITCH_KEY_ENABLE
void uart_send_voice_init(void)
{
    UARTDMAConfig((UART_TypeDef *)UART_PRE_RSLT_AUDIO_TX_NUMBER, UART_PRE_RSLT_AUDIO_TX_BAUDRATE);
}


volatile uint8_t uart_dma_trans_done = 0;

static void uart_dma_read_irq_callback(void)
{
    uart_dma_trans_done = 1;
}


void audio_pre_rslt_write_data_from_uart(uint32_t addr,uint32_t size)
{
    // init_timer0();
    // timer0_start_count();
    if(!uart_dma_trans_done)
    {
        // mprintf("pre voice data overflow\n");
    }
    uart_dma_trans_done = 0;
    extern void set_dma_int_callback(DMACChannelx dmachannel,dma_callback_func_ptr_t func);
    set_dma_int_callback(DMACChannel1,uart_dma_read_irq_callback);

    DMAC_Peripherals uart_dma_peripherals_num = DMAC_Peripherals_UART0_TX;
    uint32_t uart_fifo_addr = UART0FIFO_BASE;
    if(((uint32_t)UART_PRE_RSLT_AUDIO_TX_NUMBER == (uint32_t)UART1))
    {
        uart_dma_peripherals_num = DMAC_Peripherals_UART1_TX;
        uart_fifo_addr = UART1FIFO_BASE;
    }
    else if(((uint32_t)UART_PRE_RSLT_AUDIO_TX_NUMBER == (uint32_t)UART2))
    {
        uart_fifo_addr = UART2FIFO_BASE;
        uart_dma_peripherals_num = DMAC_Peripherals_UART2_TX;
    }

    DMAC_M2P_P2M_advance_config(DMACChannel0,uart_dma_peripherals_num,M2P_DMA,addr,uart_fifo_addr,size,
                                TRANSFERWIDTH_8b,BURSTSIZE1,DMAC_AHBMaster1);

    // while(!uart_dma_trans_done);
    // uart_dma_trans_done = 0;
    // timer0_end_count_only_print_time_us();
}
#endif


static audio_pre_init_tmp_t sg_init_tmp_str;


#define PI (3.1416926f)
void sine_wave_generate(int16_t* sine_wave, uint32_t sample_rate,uint32_t wave_fre,uint32_t point_num)
{
    int16_t num_of_one_period = sample_rate/wave_fre;//每个周期的采样点数
    for(int i=0;i<point_num;i++)
    {
        sine_wave[i] = (int16_t)(32767.0f * sinf( (2*PI*i) / num_of_one_period ));
    }
}


uint32_t tmp_voice_addr = 0;
 
_XIF_ void audio_pre_rslt_out_play_card_init(void)
{
     uint16_t block_size = AUDIO_CAP_POINT_NUM_PER_FRM * 2 * sizeof(int16_t);
    
    sg_init_tmp_str.init_str.block_size = block_size;
    sg_init_tmp_str.write_data_cnt = 0;
    sg_init_tmp_str.send_data_cnt = 0;

    #if USE_IIS0_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
        #if USE_UART_SEND_PRE_RSLT_AUDIO
        // uart_send_voice_init();
        if(!tmp_voice_addr)
        {
            tmp_voice_addr = pvPortMalloc(block_size);
            CI_ASSERT(tmp_voice_addr,"\n");
        }
        #else
            #if USE_IIS0_OUT_PRE_RSLT_AUDIO 
            audio_pre_rslt_out_codec_init();
            #endif

            #if USE_HP_OUT_PRE_RSLT_AUDIO
            audio_pre_rslt_out_codec_init_pa_out();
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

_XIF_  void play_local_voice_by_id(uint32_t play_id)
{
    //g_local_play_flag = true;
    //prompt_play_by_voice_id(play_id, NULL, false);
}
#if USE_UART_SEND_SPEEX_ENABLE
void send_voice_to_speex_encode(uint32_t block_size, const int16_t* left,const int16_t* right)
{
    //mprintf("-");
    int8_t *src_addr;
    extern uint8_t rtc_flash_record_left; 
    if(is_convert_left_right)
        src_addr = (int8_t *)right;
    else
        src_addr = (int8_t *)left;
        
    #if IF_16K_DOWNSAMPLE_TO_8K     //16k音频数据通过低通滤波和抽样为8k音频数据

    uint16_t *right_addr;
    
    if(is_convert_left_right)
        right_addr = (uint16_t *)right;
    else    
        right_addr = (uint16_t *)left;
    
    for (int i = 0; i < 256; i++)
    {
        // 低通滤波
        tmp = 0;
        x_buffer_0 = right_addr[i];
        for (int j = 1; j < LOWPASS_FILTER_SIZE; j++)
        {
            tmp += (int32_t)(LOWPASS_FILTER[j] * x_buffer[j - 1]);
        }
        tmp += (int32_t)(LOWPASS_FILTER[0] * x_buffer_0);
        
        // 防溢出
        if (tmp >= 32767)
        right_addr[i] = 32767;
        else if (tmp <= -32767)
        right_addr[i] = -32767;
        else
        right_addr[i] = (int16_t)tmp;
        
        memmove(x_buffer + 1, x_buffer, (LOWPASS_FILTER_SIZE - 2) * sizeof(x_buffer[0]));
        x_buffer[0] = x_buffer_0;
    }
    // 降采样
    for (int i = 0; i < 128; i++)
    {
        tmp_buffer[i] = right_addr[2 * i];
    }
    
    src_addr = (int8_t *)tmp_buffer;
    #endif
    
    extern uint8_t speex_init_flag;
    if(speex_init_flag)
    {
        uint16_t xDataLengthBytes = IF_16K_DOWNSAMPLE_TO_8K ? block_size/4 : block_size/2;   //单通道输出一帧音频,16k一帧512个字节,8k一帧256个字节.
        int ret = 0;
        ret = xStreamBufferSend(gSpeexEncodeStreamBuffer, src_addr, xDataLengthBytes, 5);
        if(ret != xDataLengthBytes)
        {   
            mprintf("xSpeexRecordStreamBuffer send error, send len = %d\r\n", ret);
        }
    }
}
#endif
/**
 * @brief 写数据到发送端
 * 
 * @param rslt 处理结果的起始指针
 * @param origin 原始数据的起始指针
 *  此函数不可用于中断
 */

void audio_pre_rslt_write_data(const int16_t* left,const int16_t* right)
{
    //mprintf("#");
    uint32_t block_size = sg_init_tmp_str.init_str.block_size;

    
    #if USE_IIS0_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
        #if USE_UART_SEND_PRE_RSLT_AUDIO
        int16_t* pcm_data_p = (int16_t*)tmp_voice_addr;

        int num = block_size/sizeof(int16_t)/2;
        for(int i=0;i<num;i++)
        {
            pcm_data_p[2*i] = left[i];
            pcm_data_p[2*i + 1] = right[i];
        }
        static int cnt = 0;
        cnt++;
        if(cnt == 30)
        {
            uart_send_voice_init();
        }
        else if(cnt >= 30)
        {
            audio_pre_rslt_write_data_from_uart((uint32_t)tmp_voice_addr, block_size);
        }
        #else
        int num = block_size/sizeof(int16_t)/2;

        #if USE_IIS0_OUT_PRE_RSLT_AUDIO
        uint32_t write_pcm_addr = 0;
        cm_get_pcm_buffer(PLAY_PRE_AUDIO_CODEC_ID,&write_pcm_addr,0);    //TODO HSL
        int16_t* pcm_data_p = (int16_t*)write_pcm_addr;
        for(int i=0;i<num;i++)
        {
            #if USE_DENOISE_NN_RTC
            if(is_convert_left_right)
            {
                pcm_data_p[2*i] = right[i];
                pcm_data_p[2*i + 1] = left[i];
            }
            else
            #endif
            {
                pcm_data_p[2*i] = left[i];
                pcm_data_p[2*i + 1] = right[i];
            }
        }
        if(0 == write_pcm_addr)
        {
            return;
        }
        cm_write_codec(PLAY_PRE_AUDIO_CODEC_ID, (void*)write_pcm_addr,0);
        #endif

        #if USE_HP_OUT_PRE_RSLT_AUDIO
        #if USE_PRE_RSLT_AUDIO_SWITCH_KEY_ENABLE
        static int cnt = 0;
        cnt++;
        if(cnt == 30)
        {
            uart_send_voice_init();
        }
        extern uint8_t rtc_audio_out_switch_key_func_status;
        if(!rtc_audio_out_switch_key_func_status)   //音频串口输出
        {
            audio_pre_rslt_write_data_from_uart((uint32_t)right, block_size/2);
        }
        else
        #endif
        {              //16k采样率音频 HPOUT输出
            uint32_t write_pcm_addr_cpy = 0;
            cm_get_pcm_buffer(PLAY_CODEC_ID,&write_pcm_addr_cpy,portMAX_DELAY);    //TODO HSL
            int16_t* pcm_data_p_cpy = (int16_t*)write_pcm_addr_cpy;
            if(0 == write_pcm_addr_cpy)
            {
                return;
            }
            for(int i=0;i<num;i++)
            {
                #if USE_DENOISE_NN_RTC
                if(is_convert_left_right)
                {
                    pcm_data_p_cpy[2*i] = right[i];
                    pcm_data_p_cpy[2*i + 1] = left[i];
                }
                else
                #endif
                {
                    pcm_data_p_cpy[2*i] = left[i];
                    pcm_data_p_cpy[2*i + 1] = right[i];
                }
            }
            cm_write_codec(PLAY_CODEC_ID, (void*)write_pcm_addr_cpy,portMAX_DELAY);
        }

        #if USE_UART_SEND_SPEEX_ENABLE
        if(rtc_record_status == RECORD_START)  //发送数据到speex压缩buffer
        {
           send_voice_to_speex_encode(block_size, left, right);
        }
        #endif
        #endif
            // mprintf("iis pre write %d\n", sg_init_tmp_str.send_data_cnt);
            if((AUDIO_IN_BUFFER_NUM - 2) == sg_init_tmp_str.send_data_cnt)
            {
                #if USE_IIS0_OUT_PRE_RSLT_AUDIO
                cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
                #endif
                #if USE_HP_OUT_PRE_RSLT_AUDIO
                cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
                #endif
            }
        #endif
    #endif

    sg_init_tmp_str.send_data_cnt++;

    sg_init_tmp_str.write_data_cnt++;
}


/**
 * @brief 语音前处理输出停止
 * 
 */
_XIF_ void audio_pre_rslt_stop(void)
{
    #if USE_IIS0_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
        #if USE_UART_SEND_PRE_RSLT_AUDIO
        #else

            #if USE_IIS0_OUT_PRE_RSLT_AUDIO
            cm_stop_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
            #endif
            
            #if USE_HP_OUT_PRE_RSLT_AUDIO
            cm_stop_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
            #endif
        #endif
    #endif
}


/**
 * @brief 语音前处理输出开始
 * 
 */
_XIF_ void audio_pre_rslt_start(void)
{
    #if USE_IIS0_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
        #if USE_UART_SEND_PRE_RSLT_AUDIO
        #else

            #if USE_IIS0_OUT_PRE_RSLT_AUDIO
            cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
            #endif
            #if USE_HP_OUT_PRE_RSLT_AUDIO
            cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
            #endif
        #endif
    #endif
}

/********** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
