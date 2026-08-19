/**
 * @file system_msg_deal.c
 * @brief  
 * @version V1.0.0
 * @date 2019.01.22
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _SYSTEM_USER_SAVE_DATA_H_
#define _SYSTEM_USER_SAVE_DATA_H_
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "system_msg_deal.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ci_log.h"
#include "audio_play_api.h"
#include "sdk_default_config.h"
#include "ci130x_iisdma.h"
#include "ci130x_iis.h"
#include "ci130x_lowpower.h"
#include "ci130x_core_misc.h"
#include "prompt_player.h"


#ifdef __cplusplus
extern "C"
{
#endif
// volume set
#define VOLUME_MAX                      7       //voice play max volume level
#define VOLUME_MID                      3       //voice play middle volume level
#define VOLUME_MIN                      1       //voice play min volume level
#define VOLUME_DEFAULT                  4     //voice play default volume level

typedef struct User_Save_Data_type
{
    uint8_t voice_onoff;
    uint8_t machine_onoff;/*0:stop,1:playing*/
    uint8_t volset;
    uint8_t language_id;   
    uint8_t uVoiceGroupId ;
    uint8_t mute_onoff;
    uint8_t uASRGroupId;
    uint8_t wakeuptime;
}User_Save_Data_t;
void set_vol(uint8_t vol);
void user_set_vol(uint8_t vol);
void user_set_voice_onoff(uint8_t onoff);
void user_set_mute_onoff(uint8_t onoff);
void user_set_wakeup_time(uint8_t dtime);
void user_set_language_id(uint8_t dId);
void user_set_ASRGroup_id(uint8_t dId);
void user_set_VoiceGroup_id(uint8_t dId);
uint8_t user_get_vol(void);
uint8_t user_get_voice_onoff(void);
uint8_t user_get_mute_onoff(void);
uint8_t user_get_wakeup_time(void);
uint8_t user_get_language_id(void);
uint8_t user_get_ASRGroup_id(void);
uint8_t user_get_VoiceGroup_id(void);
void  User_Save_Data_Init(void);
uint8_t user_get_volvalue(void);
uint8_t user_get_machine_onoff(void);
void user_set_machine_onoff(uint8_t onoff);
#ifdef __cplusplus
}
#endif
  
#endif


