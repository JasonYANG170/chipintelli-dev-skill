/**
 * @file system_msg_deal.c
 * @brief  
 * @version V1.0.0
 * @date 2019.01.22
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "system_msg_deal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ci_log.h"
#include "audio_play_api.h"
#include "user_config.h"
#include "sdk_default_config.h"
#include "ci130x_iisdma.h"
#include "ci130x_iis.h"
#include "ci130x_lowpower.h"
#include "ci130x_core_misc.h"
#include "prompt_player.h"
#include "ci_nvdata_manage.h"
#include "user_data_save.h"
#define VOLUME_VALUE (82)
#define DEFAULT_VOLUME_VALUE (0)

static User_Save_Data_t g_UserSaveData;
static  uint8_t g_uASRGroupId = 1;
static  uint8_t g_uVolumeValue[8]= {20,38,49,59,68,73,85,100};

void set_vol(uint8_t vol)
{  
	if(vol <= VOLUME_MAX )
	{
		audio_play_set_vol_gain(user_get_volvalue()+DEFAULT_VOLUME_VALUE);
	}
}

void user_set_vol(uint8_t vol)
{  
	if(vol <= VOLUME_MAX && g_UserSaveData.volset != vol)
	{
		g_UserSaveData.volset = vol;
		audio_play_set_vol_gain(user_get_volvalue()+DEFAULT_VOLUME_VALUE);
		cinv_item_write(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData);
	}
}

void user_set_voice_onoff(uint8_t onoff)
{  
	if(g_UserSaveData.voice_onoff != onoff)
	{
		g_UserSaveData.voice_onoff = onoff;
		cinv_item_write(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData);
	}
}

void user_set_machine_onoff(uint8_t onoff)
{  
	if(g_UserSaveData.machine_onoff != onoff)
	{
		g_UserSaveData.machine_onoff = onoff;
		cinv_item_write(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData);
	}
}

void user_set_mute_onoff(uint8_t onoff)
{  
	if(g_UserSaveData.mute_onoff != onoff)
	{
		g_UserSaveData.mute_onoff = onoff;
		cinv_item_write(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData);
	}
}


void user_set_wakeup_time(uint8_t dtime)
{  
	if(g_UserSaveData.wakeuptime != dtime)
	{
		g_UserSaveData.wakeuptime = dtime;
		cinv_item_write(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData);
	}
}

void user_set_language_id(uint8_t dId)
{  
	if(g_UserSaveData.language_id != dId)
	{
		g_UserSaveData.language_id = dId;
		cinv_item_write(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData);
	}
}


void user_set_ASRGroup_id(uint8_t dId)
{  
	if(g_UserSaveData.uASRGroupId != dId)
	{
		g_UserSaveData.uASRGroupId = dId;
		cinv_item_write(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData);
	}
}

void user_set_VoiceGroup_id(uint8_t dId)
{  
	if(g_UserSaveData.uVoiceGroupId != dId)
	{
		g_UserSaveData.uVoiceGroupId = dId;
		cinv_item_write(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData);
	}
}


uint8_t user_get_vol(void)
{  
	return g_UserSaveData.volset;
}

uint8_t user_get_voice_onoff(void)
{  
	return g_UserSaveData.voice_onoff;
}

uint8_t user_get_machine_onoff(void)
{  
	return g_UserSaveData.machine_onoff;
}
uint8_t user_get_mute_onoff(void)
{  
	return g_UserSaveData.mute_onoff;
}

uint8_t user_get_wakeup_time(void)
{  
	return g_UserSaveData.wakeuptime;
}

uint8_t user_get_language_id(void)
{  
	return g_UserSaveData.language_id;
}

uint8_t user_get_ASRGroup_id(void)
{  
	return g_UserSaveData.uASRGroupId;
}

uint8_t user_get_VoiceGroup_id(void)
{  
	return g_UserSaveData.uVoiceGroupId ;
}

uint8_t user_get_volvalue(void)
{  
	if(g_UserSaveData.volset>VOLUME_MAX)
		g_UserSaveData.volset =VOLUME_DEFAULT; 
	return g_uVolumeValue[g_UserSaveData.volset];
}

void  User_Save_Data_Init(void)
{
	uint16_t real_len;
	if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData, &real_len))
	{
		g_UserSaveData.volset= VOLUME_DEFAULT;
		
		g_UserSaveData.voice_onoff= 1;
        
		g_UserSaveData.mute_onoff= 1;
		
		g_UserSaveData.machine_onoff= 1;

		g_UserSaveData.language_id= 0;

		g_UserSaveData.uVoiceGroupId= 0;

		g_UserSaveData.uASRGroupId= 0;

		g_UserSaveData.wakeuptime= EXIT_WAKEUP_TIME;

		cinv_item_init(NVDATA_ID_VOLUME, sizeof(User_Save_Data_t), &g_UserSaveData);
	} 
	APPprintf("@@@@voice_onoff:%d\n",g_UserSaveData.voice_onoff);
	prompt_player_enable(ENABLE);    
	audio_play_set_vol_gain(user_get_volvalue()+DEFAULT_VOLUME_VALUE);
    
}


