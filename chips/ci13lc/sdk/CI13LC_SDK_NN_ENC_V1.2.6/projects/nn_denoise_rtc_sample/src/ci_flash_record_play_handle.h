#ifndef __CI_FLASH_RECORD_PLAY_HANDLE_H__
#define __CI_FLASH_RECORD_PLAY_HANDLE_H__

#include <stdio.h>
#include <malloc.h>
#include "FreeRTOS.h"
#include "task.h"
#include "ci_nvdata_manage.h"
#include "sdk_default_config.h"
#include "user_config.h"
#include "queue.h"
#include "ci_log.h"

#define RECORD_INFO_MAX                   20
#define ERASE_4K                          (4 * 1024)
#define RECORD_HEAD_INFO_PAGE             10
#define RECORD_BACKUP_LEN                 (32*1024)
#define RECORD_HEAD_INFO_ADDR             0x1F0000
#define RECORD_FLASH_START_ADDR           0x80000
typedef enum 
{
    KEY_STATE_IDLE,
    KEY_STATE_PRESSED,
    KEY_STATE_WAIT_DOUBLE,
    KEY_STATE_WAIT_RELEASE,
} KeyState;

typedef struct 
{
    KeyState state;
    uint32_t press_time;
    uint32_t release_time;
    bool key_pressed;
} KeyDetector;


typedef enum
{
    RTC_CMD_RECORD_START = 1, //开始录音
    RTC_CMD_RECORD_END = 2,   //结束录音
    RTC_CMD_PLAY_NEXT = 3,    //播放下一个录音
    RTC_CMD_PLAY_PREVIOUS = 4,//播放上一个录音
    RTC_CMD_PLAY_NEW = 5,    //开始播放录音
    RTC_CMD_PLAY_STOP = 6,    //结束播放
    RTC_CMD_RECORD_PAUSE = 7, //暂停录音
    RTC_CMD_RECORD_RESUME = 8,//恢复录音
    RTC_CMD_RECORD_MIC = 9,   //录音为降噪前
    RTC_CMD_RECORD_DST = 10,  //录音为降噪后
}rtc_cmd_t;

typedef enum
{
    RECORD_END = 0,            //空闲状态
    RECORD_START,              //开始录音
    RECORD_PAUSE,              //暂停录音
} rtc_record_status_t;

typedef enum
{
    PLAY_END = 0,            //空闲状态
    PLAY_START,               //开始播放
    PLAY_STOP,                //停止播放录音
} rtc_play_status_t;

#pragma pack(1)
typedef struct
{                               //录音数据：[record_start_addr,record_end_addr)
    uint8_t available;        //是否存在录音信息
    uint32_t start_addr; //录音数据开始地址(包含该录音数据)
    uint32_t end_addr;   //录音数据结束地址(不包含该录音数据)
} record_info_t;

typedef struct
{
    uint8_t write_index;     //记录写的id
    uint8_t play_index;      //记录播放录音ID
    uint32_t write_addr;     //当前写flash地址
    record_info_t info[RECORD_INFO_MAX];      //录音开始和结束flash地址信息
    uint16_t crc;
} rtc_flash_record_play_info_t;
#pragma pack()

typedef struct 
{
    uint8_t th;  
    uint8_t wake;
    uint8_t delay;
}vox_info_t;

typedef struct 
{
    uint32_t w_full;  
    uint32_t reset;
}test_info_t;

extern rtc_record_status_t rtc_record_status;
extern rtc_play_status_t rtc_play_status;

cinv_item_ret_t init_vox_info();
cinv_item_ret_t write_vox_info(uint8_t data);
cinv_item_ret_t init_test_info();
cinv_item_ret_t write_test_info();
void wait_flash_record_sem(void);
void give_flash_record_sem(void);
void wait_flash_play_sem(void);
void give_flash_play_sem(void);
void rtc_record_task_init(void);
#endif  //__CI_FLASH_RECORD_PLAY_HANDLE_H__