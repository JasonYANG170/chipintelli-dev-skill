/**
 * @file system_msg_deal.c
 * @brief  
 * @version V1.0.0
 * @date 2019.01.22
 * 
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 * 
 */

#pragma once

#ifndef _SYSTEM_MSG_DEAL_H_
#define _SYSTEM_MSG_DEAL_H_


#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "command_info.h"
#include "ci_flash_data_info.h"
#include "ci_audio_wrapfft.h"
#include "ci_uart_eq_param.h"
#ifdef __cplusplus
extern "C"
{
#endif



#define SYS_MAX_MSG_LEN       32



/************************************
         CMD_INFO
*************************************/
typedef enum
{
    MSG_CMD_INFO_STATUS_EXIT_WAKEUP = 0x00,
    MSG_CMD_INFO_STATUS_ENABLE_EXIT_WAKEUP,
    MSG_CMD_INFO_STATUS_ENABLE_PROCESS_ASR,
    MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD,  
    MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD,  
}sys_msg_cmd_info_status_t;


typedef struct
{
    sys_msg_cmd_info_status_t cmd_info_status;
}sys_msg_cmd_info_data_t;

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
         MSG
*************************************/
#pragma pack(1)
typedef enum
{
    SYS_MSG_TYPE_ASR = 0,
    SYS_MSG_TYPE_CMD_INFO,
    SYS_MSG_TYPE_KEY,
    SYS_MSG_TYPE_COM,
    SYS_MSG_TYPE_AUDIO_IN_STARTED,
    SYS_MSG_TYPE_I2C,
}sys_msg_type_t;


typedef struct
{
    sys_msg_type_t msg_type;/*here will be modify use union*/
    uint8_t msg_data[SYS_MAX_MSG_LEN];
}sys_msg_t;
#pragma pack()

#if USE_TWO_MIC_DOA
#   define WAKEUP_CMD_COMING()             (get_wake_up_cmd_status()==1)
#   define SET_WAKEUP_CMD_STATUS(a)        set_wake_up_cmd_status(a)
#   define WAKEUP_WHEN_PLAYING()           wakeup_when_playing()
#   define SET_WAKEUP_WHEN_PLAYING_FLAG(a) set_wakeup_when_playing_flag(a)
#endif

void userapp_initial(void);
void sys_msg_task_initial(void);


void UserTaskManageProcess(void *p_arg);


typedef enum
{
    SYS_STATE_UNWAKEUP = 0,
    SYS_STATE_WAKEUP,
}sys_wakeup_state_t;

typedef enum
{
    SYS_STATE_ASR_IDLE = 0,
    SYS_STATE_ASR_BUSY,
    SYS_STATE_ASR_SUSPEND,
}sys_asr_state_t;

typedef enum
{
    CI_RTC_MODE_UP = 0,
    CI_RTC_MODE_DOWN = 1,
}ci_rtc_mode_t;


typedef enum
{  
    CI_RTC_ALG_NULL = 0, 
    CI_RTC_ALG_AHS = 1<<0, //防啸叫算法
    CI_RTC_ALG_AEC = 1<<1, //回音消除
    CI_RTC_ALG_DN = 1<<2,  //降噪算法
    CI_RTC_ALG_EQ = 1<<3,  //均衡算法
    CI_RTC_ALG_DRC = 1<<4, //动态范围压缩算法
}ci_rtc_alg_t;


typedef struct 
{
    ci_rtc_mode_t mode;    //上行还是下行
}ci_rtc_state_t;

typedef struct 
{
    int32_t cha_num;//通道数
    uint32_t channel_flag; /*!< 声道选择，bit[0] = 1选择左声道，bit[1]=1选择右声道，可以用或运算组合 */
    uint32_t addr1;//最多支持两个声道，两个声道的地址
    uint32_t addr2;
    uint32_t size; 
}audio_in_get_voice_t;

void exit_wakeup_deal(uint32_t asr_busy_check);
void enter_wakeup_deal(uint32_t exit_wakup_ms, cmd_handle_t cmd_handle);
sys_wakeup_state_t get_wakeup_state(void);
void update_awake_time(void);
BaseType_t send_msg_to_sys_task(void *msg_data, uint32_t msg_len, BaseType_t *xHigherPriorityTaskWoken);
BaseType_t send_sys_msg_inner(void *msg_data,uint32_t msg_len, BaseType_t *xHigherPriorityTaskWoken);

bool get_mute_voice_in_state( void );

void default_play_done_callback(cmd_handle_t cmd_handle);
uint8_t vol_set(char vol);
uint8_t vol_get(void);
void pause_voice_in(void);
void resume_voice_in(void);
int ifft_data_handle_task_rtc_init(void);
void ifft_data_handle_task_rtc(void *p);
void get_denoise_model_addr(void);
void audio_in_manage_inner_task_rtc(void *p);
void voice_recode_task(void* p);
bool get_rtc_flash_record_play_enable();
int get_rtc_fft_parame();
int get_rtc_ifft_parame();
#ifdef __cplusplus
}
#endif
  
#endif


