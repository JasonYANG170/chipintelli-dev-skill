/**
 * @file prompt_player.c
 * @brief 
 * @version 0.1
 * @date 2019-04-30
 * 
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 * 
 */

#include <stdio.h>
#include "asr_api.h"
#include "command_file_reader.h"
#include "command_info.h"
#include "audio_play_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "ci_log.h"
#include "user_config.h"
#include "prompt_player.h"
#include "codec_manager.h"
#include "status_share.h"
#include "ci_flash_data_info.h"
#include "system_msg_deal.h"
#include "status_share.h"

 
#define SUPPORT_BUZZER_SEMANTIC_ID        0xB000F9

typedef 
struct prompt_player_st
{
	FunctionalState enabled_flag;               //!提示音播报是否使能
	uint8_t dcombination_number;
	uint8_t combination_index;
	uint8_t upreemptive;
	uint8_t enterPlayflag;               //!提示音播报是否使能
	SemaphoreHandle_t semaphore;
	cmd_handle_t cmd_handle;
	uint32_t combination_list[MAX_COMBINATION_COUNT];
}prompt_player_t;
typedef struct next_play_info_st
{
	cmd_handle_t cmd_handle;			    //!Command ID.
	int select_index;			//!Index of specifiled prompt in options. -1: means select by command information.
}next_play_info_t;

typedef struct 
{
	uint8_t play_number;
	uint8_t upreemptive;
	next_play_info_t next_play [MAX_COMBINATION_COUNT];
}player_content_t;

player_content_t dCurrentPlay;             
player_content_t dNextPlay;             
play_done_callback_t g_play_done_callback = NULL;    //!播放结束的回调函数


static prompt_player_t prompt_player = 
{
	ENABLE,
	0,          //combination_number
	0,          //combination_index
	0,          //preemptive
	0,          //enterPlayflag
	NULL,       //semaphore
	INVALID_HANDLE,       //cmd_handle
	{0,0,0},       //cmd_handle
};
static bool mute_voice_in_flag = 0;
static bool Buzzer_voice_close_flag = 0;
static bool g_aec_flag = 0;

bool set_Buzzer_voice_state( uint8_t dbuzzerflag )
{
	 Buzzer_voice_close_flag = dbuzzerflag;
}
bool get_Buzzer_voice_state( void )
{
	return Buzzer_voice_close_flag;
}
bool set_aec_flag( uint8_t daecflag )
{
	 g_aec_flag = daecflag;
}

/**
 * @brief Get the mute voice in state object, DENOISE will used this function
 * 
 * @return true mute voice in, data is 0
 * @return false voice in is normal
 */
bool get_mute_voice_in_state( void )
{
	return mute_voice_in_flag;
}

void pause_voice_in(void)
{   
	Reset_PlayVoice_EndTime();  
	if((g_aec_flag == 0)&&(!mute_voice_in_flag))
	{
		mute_voice_in_flag = true;
		cm_set_codec_mute(HOST_MIC_RECORD_CODEC_ID,CODEC_INPUT,1,ENABLE);//TODO 要修改
		ciss_set(CI_SS_MIC_VOICE_STATUE,CI_SS_MIC_VOICE_MUTE);
		asrtop_asr_system_pause();
	}
}


/**
 * @brief resume voice in, so can recover ASR, call this after play done, 
 * 
 */
void  resume_voice_in(void)
{
	if((g_aec_flag == 0)&&(mute_voice_in_flag==true ))
	{
		mute_voice_in_flag = false;
		cm_set_codec_mute(HOST_MIC_RECORD_CODEC_ID,CODEC_INPUT,1,DISABLE);//TODO 要修改
		ciss_set(CI_SS_MIC_VOICE_STATUE,CI_SS_MIC_VOICE_NORMAL);
		asrtop_asr_system_continue();
	}
}

static void combination_callback(int32_t play_cb_state);

static uint32_t prompt_play_inner(cmd_handle_t cmd_handle, int select_index, bool from_callback)
{
	uint16_t voice_id_buffer[MAX_COMBINATION_COUNT];
	uint16_t start_index  = ((command_info_t*)cmd_handle)->voice_start_index;
	uint16_t end_index = ((command_info_t*)cmd_handle)->voice_end_index;

	
	int32_t combination_number = cmd_info_get_voice_index(start_index,end_index,select_index, voice_id_buffer, MAX_COMBINATION_COUNT);
	int dselect_index = 0;
	if (combination_number <= 0)
	{
		if (g_play_done_callback)
		{
		
			prompt_player.enterPlayflag =0;
			g_play_done_callback(cmd_handle);
		}
		return 1; 
	}
	else 
	{
		if (combination_number <= MAX_COMBINATION_COUNT)
		{
			get_voice_addr_by_id(voice_id_buffer, prompt_player.combination_list, combination_number);
			prompt_player.dcombination_number = combination_number;
			prompt_player.combination_index = 0;
			/*audio PA on*/
			#if (PLAYER_CONTROL_PA)
			audio_play_hw_pa_da_ctl(ENABLE,true);
			vTaskDelay(pdMS_TO_TICKS(PLAY_PA_DA_DELAY_TIME));
			#else
			audio_play_hw_pa_da_ctl(ENABLE,false);
			#endif
			pause_audio_play_prompt(prompt_player.combination_list[prompt_player.combination_index++], 1, combination_callback);
		}
	}
	return 0;
}


static void combination_callback(int32_t play_cb_state)
{
	if (prompt_player.semaphore == NULL)
	{
	prompt_player.semaphore = xSemaphoreCreateMutex();
	}
	if (prompt_player.semaphore)
	{
	xSemaphoreTake(prompt_player.semaphore, portMAX_DELAY);
	}
	if ((prompt_player.combination_index >= prompt_player.dcombination_number) 
	|| (AUDIO_PLAY_CB_STATE_PAUSE == play_cb_state)
	|| (play_cb_state < 0))
	{

		//调用上一个播报音的结束回调
		prompt_player.upreemptive  = 0;
		prompt_player.enterPlayflag =0;
		prompt_player.dcombination_number = 0;
		if (g_play_done_callback)
		{
			g_play_done_callback(prompt_player.cmd_handle);
		}
		#if (PLAYER_CONTROL_PA)
		audio_play_hw_pa_da_ctl(DISABLE,true);
		#else
		audio_play_hw_pa_da_ctl(DISABLE,false);
		#endif
	}
	else if (prompt_player.combination_index < prompt_player.dcombination_number)
	{
		//播放下一个组合播报音
		pause_audio_play_prompt(prompt_player.combination_list[prompt_player.combination_index++], 1, combination_callback);
	}

	if (prompt_player.semaphore)
	{
		xSemaphoreGive(prompt_player.semaphore);
	}
}



void prompt_player_enable(FunctionalState state)
{
	prompt_player.enabled_flag = state;
}


uint32_t prompt_play_by_cmd_handle(cmd_handle_t cmd_handle,int select_index,bool preemptive)
{
	uint32_t ret = 1;

	if(Buzzer_voice_close_flag)
	{
		cmd_handle = cmd_info_find_command_by_semantic_id(SUPPORT_BUZZER_SEMANTIC_ID);
	}

	if (prompt_player.semaphore == NULL)
	{
		prompt_player.semaphore = xSemaphoreCreateMutex();
	}
	if (prompt_player.semaphore)
	{
		xSemaphoreTake(prompt_player.semaphore, portMAX_DELAY);
	}

	if (cmd_handle == INVALID_HANDLE || !prompt_player.enabled_flag)
	{
		if (prompt_player.semaphore)
		{
			xSemaphoreGive(prompt_player.semaphore);
		}
		return ret;
	}

	if( prompt_player.enterPlayflag&& !preemptive)
	{

		if(prompt_player.upreemptive ==1)
		{
			dNextPlay.next_play[0].cmd_handle = cmd_handle;
			dNextPlay.play_number = 1;
			dNextPlay.next_play[0].select_index = select_index;
			dNextPlay .upreemptive = preemptive;
			if (prompt_player.semaphore)
			{
				xSemaphoreGive(prompt_player.semaphore);
			}
			
			return ret;
		}
		#if SAME_PLAY_CONTENT_NOT_INTERRUPT
		if(dCurrentPlay.play_number ==1)
		{
			if(dCurrentPlay.next_play[0].cmd_handle == cmd_handle)
			{
				if (prompt_player.semaphore)
				{
					xSemaphoreGive(prompt_player.semaphore);
				}
				return ret;
			}
		}
		#endif
		stop_play(NULL,NULL);
		dNextPlay.next_play[0].cmd_handle = cmd_handle;
		dNextPlay.play_number = 1;
		dNextPlay.next_play[0].select_index = select_index;
		dNextPlay .upreemptive = preemptive;
		if (prompt_player.semaphore)
		{
			xSemaphoreGive(prompt_player.semaphore);
		}
		return ret;
	}
	prompt_player.enterPlayflag =1;
	pause_voice_in();

	dCurrentPlay.next_play[0].cmd_handle = cmd_handle;
	dCurrentPlay.play_number = 1;
	dCurrentPlay.next_play[0].select_index = select_index;
	prompt_player.upreemptive = preemptive;
	if (prompt_player.dcombination_number <= 0)
	{
		prompt_player.cmd_handle = cmd_handle;
		prompt_play_inner(cmd_handle,select_index ,false);
	}
	if (prompt_player.semaphore)
	{
		xSemaphoreGive(prompt_player.semaphore);
	}
	return ret;
}

uint32_t prompt_play_by_cmd_id(uint16_t cmd_id,int select_index,bool preemptive)
{
	cmd_handle_t cmd_handle = cmd_info_find_command_by_id(cmd_id);
	return prompt_play_by_cmd_handle(cmd_handle, select_index,  preemptive);
}

uint32_t prompt_play_by_semantic_id(uint32_t semantic_id,int select_index,bool preemptive)
{
	cmd_handle_t cmd_handle = cmd_info_find_command_by_semantic_id(semantic_id);
	return prompt_play_by_cmd_handle(cmd_handle, select_index,  preemptive);
}

uint32_t prompt_play_by_cmd_string(char* cmd_string,int select_index,bool preemptive)
{
	cmd_handle_t cmd_handle = cmd_info_find_command_by_string(cmd_string);
	return prompt_play_by_cmd_handle(cmd_handle, select_index,  preemptive);
}

uint32_t prompt_play_by_multi_cmd_id(prompt_play_info_t *p_play_info, int number,bool preemptive)
{
	uint32_t ret = 1;

	if (prompt_player.semaphore == NULL)
	{
		prompt_player.semaphore = xSemaphoreCreateMutex();
	}
	if (prompt_player.semaphore)
	{
		xSemaphoreTake(prompt_player.semaphore, portMAX_DELAY);
	}

	if ( (!prompt_player.enabled_flag) || (number > MAX_COMBINATION_COUNT))
	{
		if (g_play_done_callback)
		{
			cmd_handle_t cmd_handle = cmd_info_find_command_by_id(p_play_info[0].cmd_id);
			g_play_done_callback(cmd_handle);
		}
		if (prompt_player.semaphore)
		{
			xSemaphoreGive(prompt_player.semaphore);
		}
		return ret;
	}
	int i=0;
	int dinterrupteflag = 0;    
	if( prompt_player.enterPlayflag&& !preemptive)
	{

		if(prompt_player.upreemptive ==1)
		{
			dNextPlay.play_number = number;
			for ( i = 0;i < number;i++)
			{
				cmd_handle_t cmd_handle = cmd_info_find_command_by_id(p_play_info[i].cmd_id);
				dNextPlay.next_play[i].cmd_handle = cmd_handle;
				dNextPlay.next_play[i].select_index = p_play_info[i].select_index;


			}
			dNextPlay .upreemptive = preemptive;
			if (prompt_player.semaphore)
			{
				xSemaphoreGive(prompt_player.semaphore);
			}
			return ret;
		}
		if(dCurrentPlay.play_number ==number)
		{
			for(i=0;i<number;i++)
			{
			
				cmd_handle_t cmd_handle = cmd_info_find_command_by_id(p_play_info[i].cmd_id);
				if(dCurrentPlay.next_play[i].cmd_handle != cmd_handle)
				{
					dinterrupteflag = 1;
					break;
				}
			}
		}
		else 
		{
			dinterrupteflag = 1;
		}
		#if SAME_PLAY_CONTENT_NOT_INTERRUPT
		if(dinterrupteflag == 0)
		{
			if (prompt_player.semaphore)
			{
				xSemaphoreGive(prompt_player.semaphore);
			}
			return ret;
		}
		#endif
		stop_play(NULL,NULL);
		
		dNextPlay.play_number = number;
		for ( i = 0;i < number;i++)
		{
			cmd_handle_t cmd_handle = cmd_info_find_command_by_id(p_play_info[i].cmd_id);
			dNextPlay.next_play[i].cmd_handle = cmd_handle;
		}
		dNextPlay .upreemptive = preemptive;
		if (prompt_player.semaphore)
		{
			xSemaphoreGive(prompt_player.semaphore);
		}
		return ret;
	}
	prompt_player.upreemptive = preemptive;
	dCurrentPlay.play_number = number;
	prompt_player.enterPlayflag =1;
	prompt_player.dcombination_number =0 ;
	for ( i = 0;i < number;i++)
	{
		cmd_handle_t cmd_handle = cmd_info_find_command_by_id(p_play_info[i].cmd_id);
		if (cmd_handle)
		{
			if(i ==0)
			prompt_player.cmd_handle = cmd_handle;
			dCurrentPlay.next_play[i].cmd_handle = cmd_handle;
			uint8_t select_index = p_play_info[i].select_index;
			uint16_t voice_id_buffer[MAX_COMBINATION_COUNT];
			uint16_t start_index  = ((command_info_t*)cmd_handle)->voice_start_index;
			uint16_t end_index = ((command_info_t*)cmd_handle)->voice_end_index;
			int32_t combination_number = cmd_info_get_voice_index(start_index,end_index,select_index, voice_id_buffer, MAX_COMBINATION_COUNT);
			if(prompt_player.dcombination_number+combination_number<MAX_COMBINATION_COUNT)
			{
				get_voice_addr_by_id(voice_id_buffer, &prompt_player.combination_list[prompt_player.dcombination_number ], combination_number);
				prompt_player.dcombination_number += combination_number;
			}
		}
	}
	if (prompt_player.dcombination_number  <= 0)
	{
		prompt_player.enterPlayflag =0;
		if (prompt_player.semaphore)
		{
			xSemaphoreGive(prompt_player.semaphore);
		}
		return ret ; 
	}
	pause_voice_in();

	prompt_player.combination_index = 0;
	/*audio PA on*/
	#if (PLAYER_CONTROL_PA)
	audio_play_hw_pa_da_ctl(ENABLE,true);
	vTaskDelay(pdMS_TO_TICKS(PLAY_PA_DA_DELAY_TIME));
	#else
	audio_play_hw_pa_da_ctl(ENABLE,false);
	#endif
	pause_audio_play_prompt(prompt_player.combination_list[prompt_player.combination_index++], 1, combination_callback);

	ret = 0;
	if (prompt_player.semaphore)
	{
		xSemaphoreGive(prompt_player.semaphore);
	}
	return ret;
}

uint32_t prompt_play_by_multi_semantic_id(prompt_play_info_t *p_play_info, int number,bool preemptive)
{
	uint32_t ret = 1;

	if (prompt_player.semaphore == NULL)
	{
		prompt_player.semaphore = xSemaphoreCreateMutex();
	}
	if (prompt_player.semaphore)
	{
		xSemaphoreTake(prompt_player.semaphore, portMAX_DELAY);
	}

	if ( (!prompt_player.enabled_flag) || (number > MAX_COMBINATION_COUNT))
	{
		if (g_play_done_callback)
		{
			cmd_handle_t cmd_handle = cmd_info_find_command_by_semantic_id(p_play_info[0].cmd_id);
			g_play_done_callback(cmd_handle);
		}
		if (prompt_player.semaphore)
		{
			xSemaphoreGive(prompt_player.semaphore);
		}
		return ret;
	}
	int i=0;
	int dinterrupteflag = 0;    
	if( prompt_player.enterPlayflag&& !preemptive)
	{
		if(prompt_player.upreemptive ==1)
		{
			dNextPlay.play_number = number;
			for ( i = 0;i < number;i++)
			{
				cmd_handle_t cmd_handle = cmd_info_find_command_by_semantic_id(p_play_info[i].cmd_id);
				dNextPlay.next_play[i].cmd_handle = cmd_handle;
				dNextPlay.next_play[i].select_index= p_play_info[i].select_index;

			}
			dNextPlay .upreemptive = preemptive;
			if (prompt_player.semaphore)
			{
				xSemaphoreGive(prompt_player.semaphore);
			}
			return ret;
		}
		if(dCurrentPlay.play_number ==number)
		{
			for(i=0;i<number;i++)
			{
				cmd_handle_t cmd_handle = cmd_info_find_command_by_semantic_id(p_play_info[i].cmd_id);
				if(dCurrentPlay.next_play[i].cmd_handle != cmd_handle)
				{
					dinterrupteflag = 1;
					break;
				}
			}
		}
		else 
		{
			dinterrupteflag = 1;
		}
		#if SAME_PLAY_CONTENT_NOT_INTERRUPT
		if(dinterrupteflag == 0)
		{
			if (prompt_player.semaphore)
			{
				xSemaphoreGive(prompt_player.semaphore);
			}
			return ret;
		}
		#endif
		stop_play(NULL,NULL);
		dNextPlay.play_number = number;
		for ( i = 0;i < number;i++)
		{
			cmd_handle_t cmd_handle = cmd_info_find_command_by_semantic_id(p_play_info[i].cmd_id);
			dNextPlay.next_play[i].cmd_handle = cmd_handle;
			dNextPlay.next_play[i].select_index= p_play_info[i].select_index;
		}
		dNextPlay .upreemptive = preemptive;
		if (prompt_player.semaphore)
		{
			xSemaphoreGive(prompt_player.semaphore);
		}
		return ret;
	}

	prompt_player.upreemptive = preemptive;
	dCurrentPlay.play_number = number;
	prompt_player.enterPlayflag =1;
	prompt_player.dcombination_number =0 ;
	for ( i = 0;i < number;i++)
	{
		cmd_handle_t cmd_handle = cmd_info_find_command_by_semantic_id(p_play_info[i].cmd_id);
		if (cmd_handle)
		{
			if(i ==0)
			prompt_player.cmd_handle = cmd_handle;
			dCurrentPlay.next_play[i].cmd_handle = cmd_handle;
			uint8_t select_index = p_play_info[i].select_index;
			dCurrentPlay.next_play[i].select_index= p_play_info[i].select_index;
			uint16_t voice_id_buffer[MAX_COMBINATION_COUNT];
			uint16_t start_index  = ((command_info_t*)cmd_handle)->voice_start_index;
			uint16_t end_index = ((command_info_t*)cmd_handle)->voice_end_index;

			int32_t combination_number = cmd_info_get_voice_index(start_index,end_index,select_index, voice_id_buffer, MAX_COMBINATION_COUNT);
			if(prompt_player.dcombination_number+combination_number<MAX_COMBINATION_COUNT)
			{
				get_voice_addr_by_id(voice_id_buffer, &prompt_player.combination_list[prompt_player.dcombination_number ], combination_number);
				prompt_player.dcombination_number += combination_number;
			}
		}
	}

	prompt_player.combination_index = 0;
	if (prompt_player.dcombination_number  <= 0)
	{
		prompt_player.enterPlayflag =0;
		if (prompt_player.semaphore)
		{
			xSemaphoreGive(prompt_player.semaphore);
		}
		return ret; 
	}
	pause_voice_in();

	/*audio PA on*/
	#if (PLAYER_CONTROL_PA)
	audio_play_hw_pa_da_ctl(ENABLE,true);
	vTaskDelay(pdMS_TO_TICKS(PLAY_PA_DA_DELAY_TIME));
	#else
	audio_play_hw_pa_da_ctl(ENABLE,false);
	#endif
	pause_audio_play_prompt(prompt_player.combination_list[prompt_player.combination_index++], 1, combination_callback);

	ret = 0;
	if (prompt_player.semaphore)
	{
		xSemaphoreGive(prompt_player.semaphore);
	}
	return ret;
}

uint32_t prompt_is_playing()
{
	if (prompt_player.dcombination_number > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint32_t prompt_stop_play()
{
	stop_play(NULL,NULL);
	int timeout = 2000;
	while(prompt_player.dcombination_number > 0 && timeout > 0)
	{
		timeout--;
		vTaskDelay(1);
	}
	return 0;
}
void  prompt_interrupte_play(void)
{
	if (prompt_player.semaphore)
	{
		xSemaphoreTake(prompt_player.semaphore, portMAX_DELAY);
	}

	int dplayid =0;
	prompt_player.enterPlayflag =1;
	if(dNextPlay.play_number == 1)
	{
		dCurrentPlay.next_play[0].cmd_handle = dNextPlay.next_play[0].cmd_handle;
		dCurrentPlay.next_play[0].select_index= dNextPlay.next_play[0].select_index;
		dCurrentPlay.play_number = dNextPlay.play_number;
		prompt_player .upreemptive = dNextPlay.upreemptive;
		if (prompt_player.dcombination_number <= 0)
		{
			prompt_player.combination_index = 0;
			prompt_player.cmd_handle = dNextPlay.next_play[0].cmd_handle;
			prompt_play_inner(dNextPlay.next_play[0].cmd_handle,dCurrentPlay.next_play[0].select_index,false);
		}
	}
	else if(dNextPlay.play_number > 1)
	{

		int i;
		dplayid =0;
		prompt_player.upreemptive = dNextPlay.upreemptive;
		dCurrentPlay.play_number = dNextPlay.play_number;
		prompt_player.cmd_handle = dNextPlay.next_play[0].cmd_handle;
		prompt_player.dcombination_number = 0;
		for ( i = 0;i < dNextPlay.play_number;i++)
		{
			cmd_handle_t cmd_handle = dNextPlay.next_play[i].cmd_handle;
			dCurrentPlay.next_play[i].cmd_handle = cmd_handle;
			uint8_t select_index = dNextPlay.next_play[i].select_index;
			uint16_t voice_id_buffer[MAX_COMBINATION_COUNT];
			uint16_t start_index  = ((command_info_t*)cmd_handle)->voice_start_index;
			uint16_t end_index = ((command_info_t*)cmd_handle)->voice_end_index;

			int32_t combination_number = cmd_info_get_voice_index(start_index,end_index,select_index, voice_id_buffer, MAX_COMBINATION_COUNT);
			if(prompt_player.dcombination_number+combination_number<MAX_COMBINATION_COUNT)
			{
				get_voice_addr_by_id(voice_id_buffer, &prompt_player.combination_list[prompt_player.dcombination_number ], combination_number);
				prompt_player.dcombination_number += combination_number;
			}
		}
		/*audio PA on*/
		#if (PLAYER_CONTROL_PA)
		audio_play_hw_pa_da_ctl(ENABLE,true);
		vTaskDelay(pdMS_TO_TICKS(PLAY_PA_DA_DELAY_TIME));
		#else
		audio_play_hw_pa_da_ctl(ENABLE,false);
		#endif
		prompt_player.combination_index = 0;
		if (prompt_player.dcombination_number  <= 0)
		{
			prompt_player.enterPlayflag =0;
			if (prompt_player.semaphore)
			{
				xSemaphoreGive(prompt_player.semaphore);
			}
			return ; 
		}
		pause_voice_in();
		pause_audio_play_prompt(prompt_player.combination_list[prompt_player.combination_index++], 1, combination_callback);
	}
	dNextPlay.play_number = 0;
	if (prompt_player.semaphore)
	{
		xSemaphoreGive(prompt_player.semaphore);
	}
}
void	prompt_Regist_PlayFinishCallback(play_done_callback_t dPlayCB)
{
	g_play_done_callback = (play_done_callback_t)dPlayCB;
}

int prompt_get_next_play_flag(void)
{
	if (prompt_player.semaphore)
	{
		xSemaphoreTake(prompt_player.semaphore, portMAX_DELAY);
	}
	int dPlayNumber = dNextPlay.play_number;
	if (prompt_player.semaphore)
	{
		xSemaphoreGive(prompt_player.semaphore);
	}
	return dPlayNumber;
}
