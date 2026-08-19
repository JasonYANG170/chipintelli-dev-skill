#include <stdbool.h>
#include "ci_log.h"
#include "system_msg_deal.h"
#include "cwsl_app_sample.h"
#include "cwsl_manage.h"
#include "cwsl_template_manager.h"
#include "prompt_player.h"

#if USE_CWSL


#define CWSL_WAKEUP_NUMBER 1       // 可注册的唤醒词数量
#define WAKE_UP_ID 1  // 注册的唤醒词对应的命令词ID
#define CWSL_CMD_NUMBER ((sizeof(reg_cmd_list) / sizeof(reg_cmd_list[0])))     // 可注册的命令词数量

#define CWSL_REG_TIMES (3)  // 学习时 每个词需说几遍，默认 1 遍即可,支持1遍;
#define CWSL_TPL_MINWORD            (3)// 自学习模板的最小字数，默认2个字，可设置模板最小2 ~ 5个字;
#define CWSL_WAKEUP_THRESHOLD       (37)            // 学习的唤醒词阈值门限，越小越灵敏，默认 37, 最小可配置到 32;
#define CWSL_CMD_THRESHOLD          (35)            // 学习的命令词阈值门限，越小越灵敏，默认 35，最小可配置到 30；

#define FOR_REG_2TIMES_FLOW_V2      1  
#define CWSL_REG_VAD_LEVEL (0) // 学习过程，灵敏度选项配置： 0 低灵敏度，可减少噪声对学习的干扰，需学习过程大声说话；1 高灵敏度，但也可以导致干扰噪声干扰学习

#define MAX_LEARN_ERROR_NUMBER     (5 )     // 学习时，重复的总次数, 建议范围 CWSL_REG_TIMES + 1、CWSL_REG_TIMES + 2、CWSL_REG_TIMES + 3

int g_cur_learn_cmd_id = 0xff;         // 命令词ID

uint8_t g_register_again_flag = 0;   
int g_earn_error_number = 0;         // 命令词ID
int g_earn_successful_number = 0;         // 命令词ID

typedef struct cwsl_reg_asr_struct
{
	int reg_cmd_id;         // 命令词ID
}cwsl_reg_asr_struct_t;

const cwsl_reg_asr_struct_t reg_cmd_list[]=
{   //命令词ID     //注册提示播报音ID
	{3},
	{5},
	{6},
	{7},
	{8},
	{9},
	{25},
	{17},
	{18},
	{19},
};


typedef struct
{
	int word_id;// 正在注册的命令词ID
	cwsl_word_type_t word_type; // 正在注册的命令词类型
	cwsl_app_mode_t app_mode;       // 当前工作模式
	uint8_t continus_flag;          // 是否连续注册，用于简化连续学习命令词时的提示音,0:非连续学习; 1:连续学习
	uint8_t repeat_times;   
} cwsl_app_t;

cwsl_app_t cwsl_app;

static int get_next_reg_cmd_word_index(int word_id);

////cwsl 事件响应函数////////////////////////////////////////////////

// cwsl 模块初始化事件响应
// 必须返回可学习的模板数量

///////////重新学习中的删除上一次学习词条的逻辑 ////////////
static uint16_t sg_prev_group_id = 0;
static uint32_t sg_prev_cmd_id = 0;
static cwsl_word_type_t sg_prev_wordtype ;

static uint16_t sg_prev_group_id_tmp = 0;
static uint32_t sg_prev_cmd_id_tmp = 0;
static cwsl_word_type_t sg_prev_wordtype_tmp ;
static int sg_prev_appword_id = 0;

bool adjust_other_cmd_cur_learn_by_cmd_id(uint16_t cmd_id)
{
	if (g_cur_learn_cmd_id == cmd_id) 
	{
		return true;
	}
	return false;
}


void cwsl_set_prev_appwordid(int appword_id)
{
	sg_prev_appword_id = appword_id;
}

void cwsl_save_prev_info(uint32_t cmd_id,uint16_t group_id,cwsl_word_type_t wordtype)
{
	sg_prev_group_id_tmp = group_id;
	sg_prev_cmd_id_tmp = cmd_id;
	sg_prev_wordtype_tmp = wordtype;
}

void cwsl_update_prev_info(void)
{
	sg_prev_group_id = sg_prev_group_id_tmp;
	sg_prev_cmd_id = sg_prev_cmd_id_tmp;
	sg_prev_wordtype = sg_prev_wordtype_tmp;
}

void cwsl_clear_prev_info(void)
{
	sg_prev_group_id = (uint16_t)-1;
	sg_prev_cmd_id = (uint32_t)-1;
	sg_prev_wordtype = CMD_WORD;
	sg_prev_group_id_tmp = (uint16_t)-1;
	sg_prev_cmd_id_tmp = (uint32_t)-1;
	sg_prev_wordtype_tmp = CMD_WORD;
	sg_prev_appword_id = (int)-1;
}

void cwsl_get_prev_info(uint32_t* cmd_id,uint16_t* group_id,int *appword_id,cwsl_word_type_t *wordtype )
{
	*group_id = sg_prev_group_id;
	*cmd_id = sg_prev_cmd_id ;
	*appword_id = sg_prev_appword_id;
	*wordtype = sg_prev_wordtype;
}

//重新学习前 删除上一次模板
void cwsl_app_delete_prev_word(void)
{
	//if(1 == CWSL_REG_TIMES)
	{// 满足前提，先删除 上一次的 模板
		uint32_t cmd_id;
		uint16_t group_id;
		cwsl_word_type_t wordtype;
		int app_wordid ;
		//获取上一次模板的信息
		cwsl_get_prev_info(&cmd_id,&group_id,&app_wordid,&wordtype);
		
		if( (cmd_id != (uint32_t)-1) && (group_id != (uint16_t)-1) )
		{//删除模板
			// 在学习状态下 发送 删除指定模板 消息
			cwsl_delete_word_when_reg(cmd_id, group_id,wordtype);
			//更新 重新 学习 信息和 播报提示信息
			cwsl_set_wordinfo(cmd_id,group_id,wordtype);

			if(app_wordid != (int)-1)
			{
				cwsl_app.word_id = app_wordid;
			}
		}
	}
}

int on_cwsl_init(cwsl_init_parameter_t *cwsl_init_parameter)
{

	g_earn_error_number = 0;
	g_earn_successful_number = 0;    
    cwsl_app.app_mode = CWSL_APP_REC;
    cwsl_app.word_id = -1;
    CI_ASSERT(CICWSL_TOTAL_TEMPLATE >= (CWSL_WAKEUP_NUMBER + CWSL_CMD_NUMBER), "not enough template space\n");
    cwsl_init_parameter->wait_time = 0;                                 // 使用默认值(20ms)
    cwsl_init_parameter->sg_reg_times = CWSL_REG_TIMES ;                
    cwsl_init_parameter->tpl_min_word_length = CWSL_TPL_MINWORD ;       
    cwsl_init_parameter->wakeup_threshold = CWSL_WAKEUP_THRESHOLD ;     
    cwsl_init_parameter->cmdword_threshold = CWSL_CMD_THRESHOLD ;       
    cwsl_init_parameter->reg_2times_flow_v2 = FOR_REG_2TIMES_FLOW_V2 ;

	if(FOR_REG_2TIMES_FLOW_V2)
	{
		if(CWSL_REG_TIMES < 2)
		{
			cwsl_init_parameter->sg_reg_times = 2;
			mprintf("CWSL_REG_TIMES %d must be 2 or 3\n",CWSL_REG_TIMES);
		}
		else if(CWSL_REG_TIMES > 3)
		{
			cwsl_init_parameter->sg_reg_times = 3;
			mprintf("CWSL_REG_TIMES %d must be 2 or 3\n",CWSL_REG_TIMES);
		}
	}
    cwsl_init_parameter->reg_vad_level = CWSL_REG_VAD_LEVEL ;
    return CWSL_WAKEUP_NUMBER + CWSL_CMD_NUMBER;
}

int  cwsl_play_done_check_status(cmd_handle_t cmd_handle)
{
	uint16_t cmd_id = cmd_info_get_command_id(cmd_handle);
	mprintf("@@@@:%s,cmd_id:%d\n",__FUNCTION__,cmd_id);

	switch(cmd_id)
	{
		case  CWSL_REGISTRATION_SUCCESSFUL_AGAIN:
		case  CWSL_REGISTRATION_FAILE_AGAIN:
		{
			Reset_PlayVoice_EndTime();
			cwsl_reg_record_start(); 
			return 1;
		}
		case  CWSL_REGISTRATION_FAILED_EXIT:
		case  CWSL_REGISTRATION_FINSH:
		case  CWSL_NO_REGISTRATION_WAKE:
		case  CWSL_NO_REGISTRATION_CMD:
		case  CWSL_EXTI_REGISTRATION:
		case CWSL_DELETE_WAKE_SUCCESSFUL:
		case CWSL_DELETE_CMD_SUCCESSFUL:
		case CWSL_EXTI_DELETE:
		{
			cwsl_app_exit_reg();
			return 0;
		}
		case CWSL_MUTE_REGISTRATION_CMD:
		{
			//prompt_play_by_cmd_id(reg_cmd_list[cwsl_app.word_id].reg_learn_play_id, -1,  false);
			return 0;
		}
		default:
			break;
	}
	return 0;

}


// 注册开始事件响应
int on_cwsl_reg_start(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type)
{    
	set_state_enter_wakeup(); // 更新退出唤醒时间
	cwsl_reg_record_stop(); // 在提示音播放期间，关闭模板录制功能
	cwsl_save_prev_info(cmd_id,group_id,word_type);// 记录 学习的命令词 cmd_id 、 group_id;
	if (word_type == WAKEUP_WORD)
	{
		// 播放提示音 "开始学习唤醒词"
		g_register_again_flag =0;
		prompt_play_by_cmd_id(CWSL_REGISTRATION_WAKE, -1,  false);
	}
	else
	{
		if (!cwsl_app.continus_flag) // 如果是连续注册，就不播报“开始注册”
		{
			if(g_register_again_flag == 1)
			{
				prompt_play_by_cmd_id(CWSL_MUTE_REGISTRATION_CMD, -1,  false);
			}
			g_register_again_flag =0;
		}
		else 
		{
			Reset_PlayVoice_EndTime();
			g_register_again_flag =0;
			//prompt_play_by_cmd_id(reg_cmd_list[cwsl_app.word_id].reg_learn_play_id, -1,  false);
			cwsl_app.repeat_times = 0;
		}
	}
	return 0;
}

// 注册停止事件响应
int on_cwsl_reg_abort()
{
	ci_logdebug(LOG_CWSL, "==on_cwsl_reg_abort\n");
	//prompt_play_by_cmd_id(CWSL_EXIT_REGISTRATION, -1,  false);
	cwsl_app.app_mode = CWSL_APP_REC;
	return 0;
}
int get_cwsl_app_mode(void)
{
	return cwsl_app.app_mode ;
}

// 录制开始事件响应
int on_cwsl_record_start()
{
	ci_logdebug(LOG_CWSL, "==on_cwsl_record_start\n");
	return 0;
}

// 录制结束事件响应
int on_cwsl_record_end(int times, cwsl_reg_result_t result)
{
	set_state_enter_wakeup(); // 更新退出唤醒时间
	APPprintf( "==on_cwsl_record_end %d,%d,%d\n", times, result,g_earn_error_number);
	if(g_register_again_flag ==1)
	{
		g_register_again_flag =0;
		return 0;
	}
	if ((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
	{
		cwsl_update_prev_info(); //本次记录学习的 cmd_id 、 group_id 已处理完（学习完成或失败);
		if (CWSL_RECORD_SUCCESSED == result)
		{
		
			g_earn_successful_number++;
			if (CWSL_REG_TIMES >= g_earn_successful_number)
			{
				prompt_play_by_cmd_id(CWSL_REGISTRATION_SUCCESSFUL_AGAIN, -1, false);
			}
			else
			{
				// 注册次数超过上限，自动退出
				g_earn_error_number = 0;
				g_earn_successful_number = 0;
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FINSH, -1,  false);
			}
		}
		else if (CWSL_RECORD_FAILED == result)
		{
		
			g_earn_error_number++;
			if (MAX_LEARN_ERROR_NUMBER > g_earn_error_number)
			{
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FAILE_AGAIN, -1,  false);
			}
			else
			{
				// 学习次数超过上限，自动退出
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FAILED_EXIT, -1,  false);
				cwsl_app_reset();
			}
		}
		else if (CWSL_REG_FINISHED == result)
		{
			g_earn_error_number = 0;
			g_earn_successful_number = 0;
			if(CMD_WORD == cwsl_app.word_type)
			{
				prompt_play_by_cmd_id(CWSL_REGISTRATION_CMD_FINSH_CONTINUE_EXIT, -1,  false);
			}
			else
			{
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FINSH, -1,  false);
			}
		}
		else if (CWSL_NOT_ENOUGH_FRAME == result)
		{
		
			g_earn_error_number++;
			if (MAX_LEARN_ERROR_NUMBER > g_earn_error_number)
			{
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FAILE_AGAIN, -1,  false);
			}
			else
			{
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FAILED_EXIT, -1,  false);
				cwsl_recognize_start(ALL_WORD);
				cwsl_app_reset();
			}
		}
		else if (CWSL_RECORD_FAILED_BY_DEFAULTCMD == result)
		{
		
			g_earn_error_number++;
			if (MAX_LEARN_ERROR_NUMBER > g_earn_error_number)
			{
				prompt_play_by_cmd_id(CWSL_OTHER_CMD_AGAIN, -1,  false);
				cwsl_reg_record_start(); 
			}
			else
			{
				// 学习次数超过上限，自动退出
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FAILED_EXIT, -1,  false);
				cwsl_app_reset();
			}
		}
		else if (CWSL_REG_INVALID_DATA == result)
		{
			g_earn_error_number++;
			if (MAX_LEARN_ERROR_NUMBER > g_earn_error_number)
			{
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FAILE_AGAIN, -1,  false);
				cwsl_reg_record_start(); 
			}
			else
			{
				// 学习次数超过上限，自动退出
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FAILED_EXIT, -1,  false);
				cwsl_app_reset();
			}
		}
	}
	return 0;
}

// 删除模板成功事件响应
int on_cwsl_delete_successed()
{
	mprintf("@@@@:%s,tpl_number:%d\n",__FUNCTION__,cwsl_tm_get_reg_tpl_number(CMD_WORD));
	if ((cwsl_app.app_mode == CWSL_APP_DEL_ALL)||(cwsl_tm_get_reg_tpl_number(CMD_WORD) ==0))
	{
		if(cwsl_app.app_mode == CWSL_APP_DEL_WAKE)
		{
			prompt_play_by_cmd_id(CWSL_DELETE_WAKE_SUCCESSFUL, -1,  false);
		}
		else
		{
			prompt_play_by_cmd_id(CWSL_DELETE_CMD_SUCCESSFUL, -1,  false);
		}
	}
	else if(cwsl_app.app_mode == CWSL_APP_DEL_WAKE)
	{
		prompt_play_by_cmd_id(CWSL_DELETE_WAKE_SUCCESSFUL, -1,  false);
	}
	else
	{
		prompt_play_by_cmd_id(CWSL_DELETE_CMD_SUCCESSFUL, -1,  false);
	}
	return 0;
}

// 识别成功事件响应
int on_cwsl_rgz_successed(uint16_t cmd_id, uint32_t distance)
{
	cmd_handle_t cmd_handle = cmd_info_find_command_by_id(cmd_id);
	sys_msg_t send_msg;
	send_msg.msg_type = SYS_MSG_TYPE_ASR;
	send_msg.msg_data.asr_data.asr_status = MSG_CWSL_STATUS_GOOD_RESULT;
	send_msg.msg_data.asr_data.asr_cmd_handle = cmd_handle;
	send_msg_to_sys_task(&send_msg, NULL);
	ci_logdebug(LOG_CWSL, "cwsl result: %d, %d\n", cmd_id, distance);
	return 0;
}

// 查找下一个需要注册的命令词索引，用于实现“从上次中断处开始注册”.
static int get_next_reg_cmd_word_index(int reg_cmd_id)
{
	int ret = -1;
	int i = 0;
	for ( i = 0; i < CWSL_CMD_NUMBER; i++)
	{
		if (reg_cmd_id == reg_cmd_list[i].reg_cmd_id)
		{
			return i;
		}
	}
	return ret;
}

////cwsl API///////////////////////////////////////////////

// 学习唤醒词
uint8_t state = 0;
void  cwsl_app_reg_word(cwsl_word_type_t word_type)
{
	if( (cwsl_app.app_mode == CWSL_APP_REC)|| (cwsl_app.app_mode == CWSL_APP_REG_CMD))
	{
		cwsl_clear_prev_info();
		if (WAKEUP_WORD == word_type)
		{
			if (CWSL_WAKEUP_NUMBER > cwsl_tm_get_reg_tpl_number(WAKEUP_WORD))
			{
				cwsl_app.word_type = WAKEUP_WORD;
				g_cur_learn_cmd_id = WAKE_UP_ID;
				g_earn_error_number = 0;
				cwsl_reg_word(WAKE_UP_ID, 0, WAKEUP_WORD);
				cwsl_app.app_mode = CWSL_APP_REG_WAKE;
			}
			else
			{
				// 唤醒词已经注册满了
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FULL, -1,  false);
				state = 1;
			}
		}
		else if (CMD_WORD == word_type)
		{
			if (CWSL_CMD_NUMBER > cwsl_tm_get_reg_tpl_number(CMD_WORD))
			{
				cwsl_app.word_type = CMD_WORD;
				g_earn_error_number = 0;
				g_cur_learn_cmd_id = reg_cmd_list[cwsl_app.word_id ].reg_cmd_id;
				cwsl_reg_word(reg_cmd_list[cwsl_app.word_id ].reg_cmd_id, 0, CMD_WORD);
				cwsl_app.app_mode = CWSL_APP_REG_CMD;
				cwsl_app.continus_flag = 0;
			}
			else
			{
				// 命令词已经注册满了
				prompt_play_by_cmd_id(CWSL_REGISTRATION_FULL, -1,  false);
			}
		}
	}
}

// 重新学习
void cwsl_app_reg_word_restart()
{
	if ((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
	{
		cwsl_reg_record_stop();
		cwsl_set_reg_restart_flag();
		cwsl_app_delete_prev_word();
		cwsl_reg_restart();
		cwsl_reg_record_start(); 
		g_earn_error_number = 0;
		g_earn_successful_number = 0;
	}
}


// 退出注册
void cwsl_app_exit_reg(void)
{
	mprintf("@@@@:%s,%d\n",__FUNCTION__,cwsl_app.app_mode);
	if ((cwsl_app.app_mode != CWSL_APP_REC))
	{
		cwsl_reg_record_stop();
		cwsl_app_reset();
		cwsl_exit_reg_word();
		g_earn_error_number = 0;
		g_earn_successful_number = 0;
		cwsl_app.app_mode = CWSL_APP_REC;
		set_state_enter_wakeup();
		change_asr_normal_word();
		set_wakeup_time(EXIT_WAKEUP_TIME);
	}
}

// 退出删除模式
void cwsl_app_exit_delete_mode()
{
	if (cwsl_app.app_mode >= CWSL_APP_DEL_WAKE)
	{
		cwsl_app.app_mode = CWSL_APP_REC;
		//prompt_play_by_cmd_id(CWSL_EXIT_DELETE, -1,  false);
	}
}

// 删除指定类型模板
// cmd_id: 指定要删除的命令词ID, 传入-1为通配符，忽略此项
// group_id: 指定要删除的命令词分组号, 传入-1为通配符，忽略此项
// word_type: 指定要删除的命令词类型，传入-1为通配符，忽略此项
void cwsl_app_delete_word(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type)
{
	if ((cwsl_app.app_mode == CWSL_APP_DEL_WAKE)||(cwsl_app.app_mode == CWSL_APP_DEL_ALL)||(cwsl_app.app_mode == CWSL_APP_DEL_ONE))
	{
		mprintf("@@@@:%s,tpl_number:%d,cmd_id:%d\n",__FUNCTION__,cwsl_tm_get_reg_tpl_number(CMD_WORD),cmd_id);
		cwsl_delete_word(cmd_id, group_id, word_type);
	}
}

// cwsl_manage模块复位，用于系统退出唤醒状态时调用
int cwsl_app_reset()
{
	g_cur_learn_cmd_id = 0xff;
	g_earn_error_number = 0;
	g_earn_successful_number = 0;
	cwsl_manage_reset();
	return 0;
}

bool get_cmd_already_learn_by_cmd_id(uint16_t cmd_id)
{
	uint8_t cmd_word_tm_index[CWSL_CMD_NUMBER];
	int cmd_tpl_count = cwsl_tm_get_words_index(cmd_word_tm_index,CWSL_CMD_NUMBER, -1, CMD_WORD);
	int find_flag = 0;
	for (int j = 0; j < cmd_tpl_count; j++) 
	{
		if (cwsl_tm_get_tpl_cmd_id_by_index(cmd_word_tm_index[j]) == cmd_id) 
		{
			find_flag = 1; 
			break; 
		}
	} 
	if (find_flag)
	{
		return true;
	}
	else
	{ 
		return false;
	}
}
uint16_t  get_same_semantic_by_cmd_id(uint16_t cmd_id)
{
	cmd_handle_t cmd_handle = cmd_info_find_command_by_id(cmd_id);
	uint32_t semantic = cmd_info_get_semantic_id(cmd_handle);       
	uint32_t semantic1 = 0;
	for (int i = 0; i < CWSL_CMD_NUMBER; i++)
	{
		cmd_handle = cmd_info_find_command_by_id(reg_cmd_list[i].reg_cmd_id);
		semantic1 = cmd_info_get_semantic_id(cmd_handle);       
		if (semantic1 == semantic)
		{
			return reg_cmd_list[i].reg_cmd_id;
		}
	}
	return 0;
}

////cwsl process ASR message///////////////////////////////////////////////
/**
 * @brief 命令词自学习消息处理函数
 * 
 * @param asr_msg ASR识别结果消息
 * @param cmd_handle 命令词handle
 * @param cmd_id 命令词ID
 * @retval 1 数据有效,消息已处理
 * @retval 0 数据无效,消息未处理
 */

uint32_t cwsl_app_process_asr_msg(cmd_handle_t cmd_handle)
{
	uint16_t cmd_id = cmd_info_get_command_id(cmd_handle);
	uint16_t cmdid = 0;
	cmdid =  get_same_semantic_by_cmd_id(cmd_id);
	if(cmdid !=0)
	{
		cmd_id = cmdid ;
	}
	APPprintf("@@@@:%s,cmd_id:%d\n",__FUNCTION__,cmd_id);
	switch(cmd_id)
	{
		case CWSL_REGISTRATION_WAKE:
		{
			if((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
			{
				cmdid =  get_same_semantic_by_cmd_id(cmd_id);
				if(cmdid !=0)
				{
					cmd_id = cmdid ;
				}
				cwsl_reg_check_other_asrcmd(cmd_id,1);
			}
			if(cwsl_tm_get_reg_tpl_number(WAKEUP_WORD) !=0)
			{
				prompt_play_by_cmd_id(CWSL_ALREADY_REGISTRATION_WAKE, -1,  false);
				return SUPPORT_IN_LEARNING;
			}
			g_earn_error_number = 0;
			g_earn_successful_number = 0;
			cwsl_app_reg_word(WAKEUP_WORD);
			set_wakeup_time(CWSL_EXIT_WAKEUP_TIME);
			cwsl_reg_record_start(); 
			prompt_play_by_cmd_handle(cmd_handle, -1, false);
			return SUPPORT_IN_LEARNING;
		}
		case CWSL_REGISTRATION_CMD:
		{
			if((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
			{
				cmdid =  get_same_semantic_by_cmd_id(cmd_id);
				if(cmdid !=0)
				{
					cmd_id = cmdid ;
				}
				cwsl_reg_check_other_asrcmd(cmd_id,1);
			}
			if(cwsl_app.app_mode == CWSL_APP_REC)
			{
				cwsl_app.app_mode = CWSL_APP_REG_CMD_FLAG ;
				g_earn_error_number = 0;
				g_earn_successful_number = 0;
				set_wakeup_time(CWSL_EXIT_WAKEUP_TIME);
				prompt_play_by_cmd_handle(cmd_handle, -1, false);
			}
			return SUPPORT_IN_LEARNING;
		}
		case CWSL_CONTINUE_CMD:
		{
			if((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
			{
				cmdid =  get_same_semantic_by_cmd_id(cmd_id);
				if(cmdid !=0)
				{
					cmd_id = cmdid ;
				}
				cwsl_reg_check_other_asrcmd(cmd_id,1);
			}
			if(cwsl_app.app_mode == CWSL_APP_REG_CMD)
			{
				cwsl_app.app_mode = CWSL_APP_REG_CMD_FLAG ;
				g_earn_error_number = 0;
				g_earn_successful_number = 0;
				set_wakeup_time(CWSL_EXIT_WAKEUP_TIME);
				prompt_play_by_cmd_handle(cmd_handle, -1, false);
			}
			return SUPPORT_IN_LEARNING;
		}
		case CWSL_DELETE_WAKE:
		{
			if((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
			{
				cmdid =  get_same_semantic_by_cmd_id(cmd_id);
				if(cmdid !=0)
				{
					cmd_id = cmdid ;
				}
				cwsl_reg_check_other_asrcmd(cmd_id,1);
				return SUPPORT_IN_LEARNING;
			}
			if (cwsl_app.app_mode == CWSL_APP_REC)
			{
				if(cwsl_tm_get_reg_tpl_number(WAKEUP_WORD) ==0)
				{
					cwsl_app.app_mode = CWSL_APP_REC;
					cmd_info_change_cur_model_group(1);
					prompt_play_by_cmd_id(CWSL_NO_REGISTRATION_WAKE, -1,  false);
					return SUPPORT_IN_LEARNING;
				}
				else
				{
					set_wakeup_time(CWSL_EXIT_WAKEUP_TIME);
					if (cwsl_app.app_mode == CWSL_APP_REC)
					{
						cwsl_app.app_mode = CWSL_APP_DEL_WAKE_FLAG;
						prompt_play_by_cmd_handle(cmd_handle, -1, false);
					}		
				}
				return SUPPORT_IN_LEARNING;
			}
			else if (cwsl_app.app_mode == CWSL_APP_DEL_WAKE_FLAG)
			{
				cwsl_app.app_mode = CWSL_APP_DEL_WAKE;
				cwsl_app_delete_word((uint32_t)-1, (uint16_t)-1, WAKEUP_WORD);
			}
			return SUPPORT_IN_LEARNING;
		}
		case CWSL_DELETE_CMD:
		{
			if((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
			{
				cmdid =  get_same_semantic_by_cmd_id(cmd_id);
				if(cmdid !=0)
				{
					cmd_id = cmdid ;
				}
				cwsl_reg_check_other_asrcmd(cmd_id,1);
				break;
			}
			if (cwsl_app.app_mode == CWSL_APP_REC)
			{
				if(cwsl_tm_get_reg_tpl_number(CMD_WORD) ==0)
				{
					cwsl_app.app_mode = CWSL_APP_REC;
					cmd_info_change_cur_model_group(1);
					prompt_play_by_cmd_id(CWSL_NO_REGISTRATION_CMD, -1,  false);
					return SUPPORT_IN_LEARNING;
				}
				else
				{
					set_wakeup_time(CWSL_EXIT_WAKEUP_TIME);
					if (cwsl_app.app_mode == CWSL_APP_REC)
					{
						cwsl_app.app_mode = CWSL_APP_DEL_CMD;
						prompt_play_by_cmd_handle(cmd_handle, -1, false);
					}
					return SUPPORT_IN_LEARNING;
				}
				
			}
			return SUPPORT_IN_LEARNING;
		}
		case  CWSL_CMD_ID0:
		case  CWSL_CMD_ID1:
		case  CWSL_CMD_ID2:
		case  CWSL_CMD_ID3:
		case  CWSL_CMD_ID4:
		case  CWSL_CMD_ID5:
		case  CWSL_CMD_ID6:
		case  CWSL_CMD_ID7:
		case  CWSL_CMD_ID8:
		case  CWSL_CMD_ID9:
		case  CWSL_CMD_ID10:
		case  CWSL_CMD_ID11:
		case  CWSL_CMD_ID12:
		case  CWSL_CMD_ID13:
		case  CWSL_CMD_ID14:
		case  CWSL_CMD_ID15:
		case  CWSL_CMD_ID16:
		case  CWSL_CMD_ID17:
		case  CWSL_CMD_ID18:
		case  CWSL_CMD_ID19:
		{
			if ((cwsl_app.app_mode == CWSL_APP_REG_CMD_FLAG)||(cwsl_app.app_mode == CWSL_APP_DEL_CMD))
			{
				if (cwsl_app.app_mode == CWSL_APP_REG_CMD_FLAG)
				{
					int dret =  get_cmd_already_learn_by_cmd_id(cmd_id);
					if(dret == true)
					{
						cwsl_app.app_mode = CWSL_APP_REC;
						prompt_play_by_cmd_id(CWSL_ALREADY_REGISTRATION_CMD, -1,  false);
						return SUPPORT_IN_LEARNING;
					}             
					cwsl_app.word_id  =get_next_reg_cmd_word_index( cmd_id);
					if(cwsl_app.word_id!=-1)
					{
						cwsl_app.app_mode = CWSL_APP_REG_CMD;
						cwsl_app_reg_word(CMD_WORD);
						prompt_play_by_cmd_id(CWSL_MUTE_REGISTRATION_CMD, -1,  false);
						g_earn_error_number = 0;
						g_earn_successful_number = 0;
						cwsl_reg_record_start(); 
					}
					return SUPPORT_IN_LEARNING;
				}
				else if ((cwsl_app.app_mode == CWSL_APP_DEL_CMD))
				{
					int dret =  get_cmd_already_learn_by_cmd_id(cmd_id);
					if(dret == 0)
					{
						prompt_play_by_cmd_id(CWSL_DELETE_CMD_SUCCESSFUL, -1,  false);
						return SUPPORT_IN_LEARNING;
					}
					mprintf("@@@@:%s,tpl_number:%d\n",__FUNCTION__,cwsl_tm_get_reg_tpl_number(CMD_WORD));
					if(cwsl_tm_get_reg_tpl_number(CMD_WORD) ==0)
					{
						cwsl_app.app_mode = CWSL_APP_REC;
						cmd_info_change_cur_model_group(1);
						prompt_play_by_cmd_id(CWSL_NO_REGISTRATION_CMD, -1,  false);
						return SUPPORT_IN_LEARNING;
					}
					else 
					{
						int dret =  get_cmd_already_learn_by_cmd_id(cmd_id);
						if(dret == 0)
						{
							prompt_play_by_cmd_id(CWSL_DELETE_CMD_SUCCESSFUL, -1,  false);
							return SUPPORT_IN_LEARNING;
						}
						mprintf("@@@@:%s,cwsl_app.app_mode:%d\n",__FUNCTION__,cwsl_app.app_mode);
						cwsl_app.app_mode = CWSL_APP_DEL_ONE;
						cwsl_app_delete_word(cmd_id, (uint16_t)-1, CMD_WORD);
					}
					return SUPPORT_IN_LEARNING;
				}
			}
			else if(cwsl_app.app_mode == CWSL_APP_REG_CMD)
			{
    			if((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
    			{
    				cmdid =  get_same_semantic_by_cmd_id(cmd_id);
    				if(cmdid !=0)
    				{
    					cmd_id = cmdid ;
    				}
    				cwsl_reg_check_other_asrcmd(cmd_id,1);
    			}
				return SUPPORT_IN_LEARNING;
			}
			return SUPPORT_NOT_LEARN;
		}
		case CWSL_DELETE_ALL_CMD:
		{
			if((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
			{			
				cmdid =  get_same_semantic_by_cmd_id(cmd_id);
				if(cmdid !=0)
				{
					cmd_id = cmdid ;
				}
				cwsl_reg_check_other_asrcmd(cmd_id,1);
				break;
			}
			mprintf("@@@@111:%s,cwsl_app.app_mode:%d\n",__FUNCTION__,cwsl_app.app_mode);
			if(cwsl_app.app_mode < CWSL_APP_DEL_CMD)
				return SUPPORT_IN_LEARNING;
			cwsl_app.app_mode = CWSL_APP_DEL_ALL;
			cwsl_app_delete_word((uint16_t)-1, (uint16_t)-1, CMD_WORD);
			mprintf("@@@@222:%s,cwsl_app.app_mode:%d\n",__FUNCTION__,cwsl_app.app_mode);
			return SUPPORT_IN_LEARNING;
		}
		case CWSL_REGISTER_AGAIN:
		{
			if((cwsl_app.app_mode != CWSL_APP_REG_WAKE)&&(cwsl_app.app_mode != CWSL_APP_REG_CMD))
				return SUPPORT_IN_LEARNING;
			g_register_again_flag =1;
			cwsl_app_reg_word_restart();
			return SUPPORT_IN_LEARNING;
		}
		case CWSL_EXTI_REGISTRATION:
        case CWSL_EXTI_DELETE:
		{
			if (cwsl_app.app_mode == CWSL_APP_REC)
				return SUPPORT_IN_LEARNING;
			cwsl_app_exit_reg();
			prompt_play_by_cmd_handle(cmd_handle, -1, false);
			return SUPPORT_IN_LEARNING;
		}
		default:
		{
			if(cwsl_app.app_mode == CWSL_APP_REG_CMD_FLAG)
			{
				int ret = get_next_reg_cmd_word_index( cmd_id);
				if(ret == -1)
				{
					prompt_play_by_cmd_id(CWSL_INVALID_CMD, -1,  false);
					return SUPPORT_IN_LEARNING;
				}
			}
			if((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD))
			{
				cmdid =  get_same_semantic_by_cmd_id(cmd_id);
				if(cmdid !=0)
				{
					cmd_id = cmdid ;
				}
				cwsl_reg_check_other_asrcmd(cmd_id,1);
			}
			if((cwsl_app.app_mode == CWSL_APP_REG_WAKE)||(cwsl_app.app_mode == CWSL_APP_REG_CMD)||(cwsl_app.app_mode == CWSL_APP_REG_CMD)||(cwsl_app.app_mode == CWSL_APP_DEL_CMD))
			{
				send_nn_end_msg_to_cwsl(NULL, 0);
				int ret = get_next_reg_cmd_word_index( cmd_id);
			}
			break;
		}
	}
	return SUPPORT_NOT_LEARN;
}
#endif
