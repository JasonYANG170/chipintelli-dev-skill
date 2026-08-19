/**
  ******************************************************************************
  * @文件    asr_process_callbak.h
  * @版本    V1.0.1
  * @日期    2019-3-15
  * @概要  asr 回调函数，VAD相关
  ******************************************************************************
  * @注意
  *
  * 版权归chipintelli公司所有，未经允许不得使用或修改
  *
  ******************************************************************************
  */ 

#ifndef _ASR_PROCESS_CALLBACK_H_
#define _ASR_PROCESS_CALLBACK_H_


#ifdef __cplusplus
extern "C" {
#endif
  
#include "command_info.h"
  
typedef struct
{
    char* cmd_word;
    cmd_handle_t cmd_handle;
    unsigned int asrvoice_ptr;
    short confidence;
    short vocie_valid_frame_len;
    short voice_start_frame;
    short start_word_confidence;
    short end_word_confidence;
    short lowest_confidence;
    char cwsl_flag;
}callback_asr_result_type_t;


/************************************
         ASR
*************************************/
typedef enum
{
    MSG_ASR_STATUS_GOOD_RESULT = 0x00,
    MSG_ASR_STATUS_NO_RESULT,    
    MSG_ASR_STATUS_VAD_START,    
    MSG_ASR_STATUS_AUDIO_PROCESS,    
    MSG_ASR_STATUS_VAD_END,    
    MSG_CWSL_STATUS_GOOD_RESULT,
}sys_msg_asr_status_t;


typedef struct
{
    sys_msg_asr_status_t asr_status;
    uint32_t asr_cmd_handle;
	uint32_t asr_pcm_base_addr;
    short asr_score;
    uint16_t asr_frames;
}sys_msg_asr_data_t;


int asr_result_callback(callback_asr_result_type_t * asr);


#ifdef __cplusplus
}
#endif


#endif

/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
