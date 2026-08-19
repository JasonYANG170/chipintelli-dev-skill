/**
  ******************************************************************************
  * @file    ci110x_speex.c
  * @version V1.0.0
  * @date    2018.07.31
  * @brief   speex编码器统一接口
  ******************************************************************************
  */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "speex/speex.h"
#include "speex/speex_bits.h"
#include "speex/speex_types.h"
#include "speex/speex_header.h"
#include "speex/speex_stereo.h"
#include "speex/speex_config_types.h"
#include "ci110x_speex.h"
#include "freertos/freeRTOS.h"

#if 0   //remove by yjd

extern void *pvPortCalloc( size_t nmemb, size_t size );
extern void *pvPortMalloc( size_t xWantedSize );
extern void *pvPortRealloc( void *pv, size_t size );
extern void vPortFree( void *pv );



#endif
unsigned char *g_speex_out_p = NULL;
//unsigned char g_speex_result[100*1024];

void *ci_speex_create(void)
{
    //g_speex_out_p = (unsigned char *)0x70D00000;
    //memset(g_speex_out_p,0x0,1024);
    ci_speex_t *speex = NULL;
    speex = (ci_speex_t *)malloc(sizeof(ci_speex_t));
    if (speex == NULL)
    {
        CIAS_LOG_HL("Memory overflow!!! Alloc speex handler failed.\n");
    }
    memset(speex, 0, sizeof(ci_speex_t));
    CIAS_LOG_HL("11当前堆剩余空间 0x%x\n",xPortGetFreeHeapSize() );
    speex->_encoder = speex_encoder_init(&speex_wb_mode);   /*16K采样率*/
    if (speex->_encoder == NULL)
    {
        CIAS_LOG_HL("Speex encoder state init failed!\n");
    }
    CIAS_LOG_HL("12当前堆剩余空间 0x%x\n",xPortGetFreeHeapSize() );
    int tmp = 5;//CI_SPEEX_COMPRESS_QUALITY;
    speex_encoder_ctl(speex->_encoder, SPEEX_SET_QUALITY, &tmp);
    speex_encoder_ctl(speex->_encoder, SPEEX_GET_FRAME_SIZE, &speex->_frame_size);
    CIAS_LOG_HL("Speex frame size: %d\n", speex->_frame_size);
    speex->_buffer_out = (char *)malloc(speex->_frame_size);
    if (speex->_buffer_out == NULL)
    {
        CIAS_LOG_HL("Memory overflow!!! Alloc speex buffer out failed.\n");
    }

    speex->_buffer_in = (short *)malloc(sizeof(short) * speex->_frame_size);
    if (speex->_buffer_in == NULL)
    {
        CIAS_LOG_HL("Memory overflow!!! Alloc speex buffer in failed.\n");

    }
    speex->_buffer_in_float = (float *)malloc(sizeof(float) * speex->_frame_size);
    if (speex->_buffer_in_float == NULL)
    {
        CIAS_LOG_HL("Memory overflow!!! Alloc speex buffer in failed.\n");

    }

    speex->_frame_cur = 0;
    speex_bits_init(&speex->_bits);
    return speex;
}

void ci_speex_destroy(void * handler)
{
    ci_speex_t *speex = (ci_speex_t *)handler;

    if (speex)
    {
        if (speex->_encoder)
        {
            speex_encoder_destroy(speex->_encoder);
            speex->_encoder = NULL;
        }

        if (speex->_buffer_out)
        {
            free(speex->_buffer_out);
            speex->_buffer_out = NULL;
        }

        if (speex->_buffer_in)
        {
            free(speex->_buffer_in);
            speex->_buffer_in = NULL;
        }
        if (speex->_buffer_in_float)
        {
            free(speex->_buffer_in_float);
            speex->_buffer_in_float = NULL;
        }

        speex_bits_destroy(&speex->_bits);

        free(speex);
    }
}

void ci_voice_send(ci_speex_t *_speex,short* data,unsigned char* raw_addr,unsigned int size)
{
    int written = 0;
    ci_speex_t *speex = (ci_speex_t *)_speex;
    unsigned int cur = 0;
    if (speex && speex->_encoder && speex->_buffer_in && speex->_buffer_out)
    {
        if(CI_SPEEX_INT == speex->ci_speex_mode)
        {
            //CIAS_LOG_HL("CI_SPEEX_INT  !!!\n");
            size_t framesize = speex->_frame_size;
            short *buffer_in = speex->_buffer_in;

            //CIAS_LOG_HL("speex_encode: size = %d, framesize = %d\n", size, framesize);

            while (cur + framesize - speex->_frame_cur <= size)
            {
                //CIAS_LOG_HL("duer_speex_encode: cur = %d, speex->_frame_cur = %d\n", cur, speex->_frame_cur);
                memcpy(buffer_in + speex->_frame_cur, data + cur, (framesize - speex->_frame_cur)*sizeof(short));
                cur += framesize - speex->_frame_cur;
                speex->_frame_cur = 0;
                speex_bits_reset(&speex->_bits);
                /*Encode the frame*/
                speex_encode_int(speex->_encoder, speex->_buffer_in, &speex->_bits);
                written = speex_bits_write(&speex->_bits, speex->_buffer_out, speex->_frame_size);
                //CIAS_LOG_HL("written = %d\n",written);
                memcpy(raw_addr,speex->_buffer_out,written);
                raw_addr += written;
            }


            if (cur < size)
            {
                memcpy(buffer_in + speex->_frame_cur, data + cur, (size - cur)*sizeof(short));
                speex->_frame_cur += size - cur;

                //CIAS_LOG_HL("duer_speex_encode xxx: cur = %d, speex->_frame_cur = %d\n", cur, speex->_frame_cur);

                if (speex->_frame_cur > 0)
                {
                    memset(buffer_in + speex->_frame_cur, 0, (framesize - speex->_frame_cur)*sizeof(short));
                    speex_bits_reset(&speex->_bits);
                    speex_encode_int(speex->_encoder, speex->_buffer_in, &speex->_bits);
                    written = speex_bits_write(&speex->_bits, speex->_buffer_out, speex->_frame_size);
                    memcpy(raw_addr,speex->_buffer_out,written);
                    raw_addr += written;
                }
            }


        }
        else if(CI_SPEEX_FLOAT == speex->ci_speex_mode)
        {
            //CIAS_LOG_HL("CI_SPEEX_FLOAT  !!!\n");
            size_t framesize = speex->_frame_size;
            float *buffer_in = speex->_buffer_in_float;

            //CIAS_LOG_HL("speex_encode: size = %d, framesize = %d\n", size, framesize);

            float tem[320];
            while (cur + framesize - speex->_frame_cur <= size)
            {
                for(unsigned int i = 0; i < 320;i++)
                {
                    tem[i] = (float)(data[i+cur]);
                }

                //CIAS_LOG_HL("duer_speex_encode: cur = %d, speex->_frame_cur = %d\n", cur, speex->_frame_cur);
                memcpy(buffer_in + speex->_frame_cur, tem, (framesize - speex->_frame_cur)*sizeof(float));
                cur += framesize - speex->_frame_cur;
                speex->_frame_cur = 0;
                speex_bits_reset(&speex->_bits);
                /*Encode the frame*/
                speex_encode(speex->_encoder, speex->_buffer_in_float, &speex->_bits);
                written = speex_bits_write(&speex->_bits, speex->_buffer_out, speex->_frame_size);
                //CIAS_LOG_HL("written = %d\n",written);
                memcpy(raw_addr,speex->_buffer_out,written);
                raw_addr += written;
            }


            if (cur < size)
            {
                for(unsigned int i = 0; i < 320;i++)
                {
                    tem[i] = (float)(data[i+cur]);
                }

                memcpy(buffer_in + speex->_frame_cur, tem, (size - cur)*sizeof(float));
                speex->_frame_cur += size - cur;

                //CIAS_LOG_HL("duer_speex_encode xxx: cur = %d, speex->_frame_cur = %d\n", cur, speex->_frame_cur);

                if (speex->_frame_cur > 0)
                {
                    memset(buffer_in + speex->_frame_cur, 0, (framesize - speex->_frame_cur)*sizeof(float));
                    speex_bits_reset(&speex->_bits);
                    speex_encode(speex->_encoder, speex->_buffer_in_float, &speex->_bits);
                    written = speex_bits_write(&speex->_bits, speex->_buffer_out, speex->_frame_size);
                    //CIAS_LOG_HL("written = %d\n",written);
                    memcpy(g_speex_out_p,speex->_buffer_out,written);
                    g_speex_out_p += written;
                }
            }
        }
    }
}

void speex_test(unsigned int pcm_addr,unsigned int raw_addr,unsigned int pcm_size)
{
    ci_speex_t* ci_speex_hander;
    ci_speex_hander = ci_speex_create();
    if(NULL == ci_speex_hander)
    {
        return;
    }
    ci_speex_hander->ci_speex_mode = CI_SPEEX_INT;
    ci_voice_send(ci_speex_hander,(short*)pcm_addr,(unsigned char*)raw_addr,pcm_size/sizeof(short));
    ci_speex_destroy(ci_speex_hander);
}
