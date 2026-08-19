/**
 * @file system_msg_deal.c
 * @brief  
 * @version V1.0.0
 * @date 2019.01.22
 * 
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 * 
 */
#ifndef _SYSTEM_MSG_DEAL_H_
#define _SYSTEM_MSG_DEAL_H_

#include "FreeRTOS.h"
#include "task.h"
#include "customer_uart_headl.h"
#include "command_info.h"

#ifdef __cplusplus
extern "C"
{
#endif


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
	cmd_handle_t asr_cmd_handle;
	uint32_t asr_pcm_base_addr;
	short asr_score;
	uint16_t cwsl_flag;
}sys_msg_asr_data_t;

/************************************
         KEY
*************************************/
typedef enum
{
    MSG_KEY_STATUS_PRESS = 0x10,
    MSG_KEY_STATUS_PRESS_LONG,    
    MSG_KEY_STATUS_RELEASE,    
}sys_msg_key_status_t;


typedef struct
{
    sys_msg_key_status_t key_status;
    uint32_t key_index;
    uint32_t key_time_ms;           /*!< 按键首次被按下到现在的时间*/
}sys_msg_key_data_t;

//!宏定义，无按键值
#define KEY_NULL                (0xff) 

/************************************
         PLAY
*************************************/
typedef enum
{
    MSG_PLAY_STATUS_START = 0x20,
    MSG_PLAY_STATUS_PAUSE,    
    MSG_PLAY_STATUS_RESUME,    
    MSG_PLAY_STATUS_DONE,    
}sys_msg_play_status_t;


typedef struct
{
    sys_msg_play_status_t asr_status;
    uint32_t play_index;
}sys_msg_play_data_t;



/************************************
         NET
*************************************/
typedef enum
{
    MSG_NET_STATUS_DISCONNECT = 0x30,
    MSG_NET_STATUS_CONNECTED,    
}sys_msg_net_status_t;


/************************************
         MSG
*************************************/
typedef enum
{
	SYS_MSG_TYPE_ASR = 0,
	SYS_MSG_TYPE_EXIT_WAKEUP,
	SYS_MSG_TYPE_KEY,
	SYS_MSG_TYPE_COM,
	SYS_MSG_TYPE_PLAY_OVER,
	SYS_MSG_TYPE_AUDIO_IN_STARTED,
	SYS_MSG_TYPE_BLUETOOTH,
	SYS_MSG_TYPE_APP,
	SYS_MSG_TYPE_COM_HAND,
}sys_msg_type_t;


typedef struct
{
	sys_msg_type_t msg_type;/*here will be modify use union*/
	union
	{
		sys_msg_asr_data_t  asr_data;
		sys_msg_com_data_t  com_data;
		sys_bluetooth_data_t bluetooth_data;
		sys_app_data_t app_data;
	}msg_data;
}sys_msg_t;



typedef enum
{
    SYS_STATE_UNWAKEUP = 0,
    SYS_STATE_WAKEUP,
	SYS_STATE_ENTERNWAKEUP,
}sys_wakeup_state_t;

typedef enum
{
    SYS_STATE_ASR_IDLE = 0,
    SYS_STATE_ASR_BUSY,
    SYS_STATE_ASR_SUSPEND,
}sys_asr_state_t;

void sys_msg_task_initial(void);
void play_done_callback(cmd_handle_t cmd_handle);
void Start_play_done_callback(cmd_handle_t cmd_handle);
sys_wakeup_state_t get_wakeup_state(void);
void set_state_enter_wakeup(void);
void set_state_exit_wakeup(void);
void set_state_wakeup(void);
void change_asr_wakeup_word(void);
void change_asr_normal_word(void);
BaseType_t send_msg_to_sys_task(sys_msg_t *send_msg,BaseType_t *xHigherPriorityTaskWoken);
void UserTaskManageProcess(void *p_arg);
void set_wakeup_time(int   dWktime);
int get_wakeup_time(void);
TickType_t get_PlayVoice_EndTime(void);
int  get_Sleep_Flag(void);
void Reset_PlayVoice_EndTime(void);
void set_PlayVoice_time(void);
void UserSleepTask(void *pvParameters);
#ifdef __cplusplus
}
#endif
  
#endif


