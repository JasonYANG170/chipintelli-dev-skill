#ifndef _CIAS_VOICE_UPLOAD_H_
#define _CIAS_VOICE_UPLOAD_H_
#include "cias_audio_data_handle.h"
#include "stream_buffer.h"
#include "timers.h"
#include "queue.h"
#if AUDIO_DATA_UPLOAD_BY_UART
void asr_wakeup_on_handle(void);
void cias_aiot_vad_end_handle(int type);   //0x0-等待任务结束退出 0x01立即停止
bool voice_upload_task_init(void);
#endif
#endif


