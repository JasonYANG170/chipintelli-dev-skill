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

#define BUFFER_NUM (4)

#define UART_NUM_SEND_AUDIO_NUM (UART1)//用哪个UART口将语音数据送出

#define USE_UART_SEND_PRE_RSLT_AUDIO 0  //是否使用UART将语音数据送出

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

#if USE_UART_SEND_PRE_RSLT_AUDIO

typedef struct 
{
    uint32_t addr;
    uint32_t size;
}uart_voice_msg_t;

QueueHandle_t uart_voice_msg_queue = NULL;    /*串口采音用队列发送*/
static uint8_t uart_send_flag = 0;            /*1表示正在有数据发送*/

//DMA中断服务函数
void UART_VOICE_DMA_IRQHandler(void)
{
    dma_without_os_int();
}

//配置UART DMA模式发送地址和大小
static void uart_audio_dma_config(uint32_t addr,uint32_t size)
{
    DMAC_Peripherals uart_dma_peripherals_num = DMAC_Peripherals_UART0_TX;
    uint32_t uart_fifo_addr = UART0FIFO_BASE;
    if(((uint32_t)UART_NUM_SEND_AUDIO_NUM == (uint32_t)UART1))
    {
        uart_dma_peripherals_num = DMAC_Peripherals_UART1_TX;
        uart_fifo_addr = UART1FIFO_BASE;
    }
    else if(((uint32_t)UART_NUM_SEND_AUDIO_NUM == (uint32_t)UART2))
    {
        uart_fifo_addr = UART2FIFO_BASE;
        uart_dma_peripherals_num = DMAC_Peripherals_UART2_TX;
    }
    DMAC_M2P_P2M_advance_config(DMACChannel0,uart_dma_peripherals_num,M2P_DMA,addr,uart_fifo_addr,size,
                                TRANSFERWIDTH_8b,BURSTSIZE1,DMAC_AHBMaster1);
}

//DMA中断回调函数，队列有消息就配置发送
void uart_voice_dma_callback()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uart_voice_msg_t recv_msg = {0};

    BaseType_t xResult = xQueueReceiveFromISR(uart_voice_msg_queue, &recv_msg, xHigherPriorityTaskWoken);
    if (pdTRUE == xResult)
    {
        uart_audio_dma_config(recv_msg.addr,recv_msg.size);
    }
    else
    {
        uart_send_flag = 0;
    }

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

//串口采音初始化
void uart_send_voice_init(void)
{
    __eclic_irq_set_vector(DMA_IRQn, (int32_t)UART_VOICE_DMA_IRQHandler);
    set_dma_int_callback(DMACChannel0, uart_voice_dma_callback);
    eclic_irq_enable(DMA_IRQn);

    UARTDMAConfig((UART_TypeDef *)UART_NUM_SEND_AUDIO_NUM, UART_BaudRate921600);
    
    uart_voice_msg_queue = xQueueCreate(5, sizeof(uart_voice_msg_t));
    if (!uart_voice_msg_queue)
    {
        mprintf("not enough memory\n");
    }
}

static void audio_pre_rslt_write_data_from_uart(uint32_t addr,uint32_t size)
{
    uart_voice_msg_t send_msg = {0};
    send_msg.addr = addr;
    send_msg.size = size;
    if (errQUEUE_FULL == xQueueSend(uart_voice_msg_queue, &send_msg, 0))
    {
        mprintf("uart voice queue full\n");
    }

    //中断服务队列不处理时，这里主动处理
    if(0 == uart_send_flag)
    {
        uart_voice_msg_t recv_msg = {0};
        BaseType_t xResult = xQueueReceive(uart_voice_msg_queue, &recv_msg, 0);
        if (pdTRUE == xResult)
        {
            uart_audio_dma_config(recv_msg.addr,recv_msg.size);
        }
        uart_send_flag = 1;
    }
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


void audio_pre_rslt_out_play_card_init(void)
{
    uint16_t block_size = AUDIO_CAP_POINT_NUM_PER_FRM * 2 * sizeof(int16_t);
    
    sg_init_tmp_str.init_str.block_size = block_size;
    sg_init_tmp_str.write_data_cnt = 0;
    sg_init_tmp_str.send_data_cnt = 0;

    #if USE_IIS1_OUT_PRE_RSLT_AUDIO
        #if USE_UART_SEND_PRE_RSLT_AUDIO
        uart_send_voice_init();
        tmp_voice_addr = pvPortMalloc(block_size);
        CI_ASSERT(tmp_voice_addr,"\n");
        #else
        audio_pre_rslt_out_codec_init();
        #endif
    #endif
}


/**
 * @brief 写数据到发送端
 * 
 * @param rslt 处理结果的起始指针
 * @param origin 原始数据的起始指针
 *  此函数不可用于中断
 */
void audio_pre_rslt_write_data(const int16_t* left,const int16_t* right)
{
    
    uint32_t block_size = sg_init_tmp_str.init_str.block_size;

    
    #if USE_IIS1_OUT_PRE_RSLT_AUDIO
        #if USE_UART_SEND_PRE_RSLT_AUDIO
        int16_t* pcm_data_p = (int16_t*)tmp_voice_addr;

        int num = block_size/sizeof(int16_t)/2;

        for(int i=0;i<num;i++)
        {
            pcm_data_p[2*i] = left[i];
            pcm_data_p[2*i + 1] = right[i];
        }
        audio_pre_rslt_write_data_from_uart((uint32_t)tmp_voice_addr,block_size);
        #else
        uint32_t write_pcm_addr = 0;
        cm_get_pcm_buffer(PLAY_PRE_AUDIO_CODEC_ID,&write_pcm_addr,0);    //TODO HSL
        int16_t* pcm_data_p = (int16_t*)write_pcm_addr;
        if(0 == write_pcm_addr)
        {
            return;
        }

        int num = block_size/sizeof(int16_t)/2;

        for(int i=0;i<num;i++)
        {
            pcm_data_p[2*i] = left[i];
            pcm_data_p[2*i + 1] = right[i];
        }
        
        cm_write_codec(PLAY_PRE_AUDIO_CODEC_ID, (void*)write_pcm_addr,0);

        if(0 == sg_init_tmp_str.send_data_cnt)
        {
            cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
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
void audio_pre_rslt_stop(void)
{
    #if USE_IIS1_OUT_PRE_RSLT_AUDIO
        #if USE_UART_SEND_PRE_RSLT_AUDIO
        #else
        cm_stop_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
        #endif
    #endif
}


/**
 * @brief 语音前处理输出开始
 * 
 */
void audio_pre_rslt_start(void)
{
    #if USE_IIS1_OUT_PRE_RSLT_AUDIO
        #if USE_UART_SEND_PRE_RSLT_AUDIO
        #else
        cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
        #endif
    #endif
}

/********** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
