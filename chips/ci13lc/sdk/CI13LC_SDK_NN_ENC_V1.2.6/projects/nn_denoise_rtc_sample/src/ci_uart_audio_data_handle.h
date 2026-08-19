#ifndef __CI_UART_AUDIO_DATA_HANDLE_H__
#define __CI_UART_AUDIO_DATA_HANDLE_H__

#include <stdio.h>
#include <malloc.h>
#include "FreeRTOS.h"
#include "task.h"
#include "sdk_default_config.h"
#include "ci13lc_dma.h"
#include "ci13lc_uart.h"
#include "user_config.h"
#include "queue.h"
#include "codec_manager.h"


int speex_decode_task(void);
int speex_encode_task(void);
int speex_play_task(void);
bool audio_speex_task_init(void);

#define ORIGIN_PCM_FRAME_LEN 512              // 原始音频16K采样，每帧256个点共521字节
#define RCV_SPEEX_DECODE_LEN 70              // 解码队列长度
#define RCV_SPEEX_DECODE_REQ_LEN 50           // 解码队列请求长度
#define RCV_PCM_PLAY_LEN                      6
#define RCV_PCM_PLAY_GET_LEN                  5
#endif  //__CI_UART_AUDIO_DATA_HANDLE_H__