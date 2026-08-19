#ifndef __CWSL_APP_SAMPLE1_H__
#define __CWSL_APP_SAMPLE1_H__

#include "system_msg_deal.h"
#include "cwsl_manage.h"

#define SUPPORT_IN_LEARNING   1
#define SUPPORT_NOT_LEARN 2

typedef enum
{
	CWSL_APP_REC, // 识别模式
	CWSL_APP_REG_WAKE, // 注册模式
	CWSL_APP_REG_CMD_FLAG, // 注册模式
	CWSL_APP_REG_CMD, // 注册模式
	CWSL_APP_DEL_WAKE_FLAG, // 删除模式
	CWSL_APP_DEL_WAKE, // 删除模式
	CWSL_APP_DEL_CMD, // 删除模式
	CWSL_APP_DEL_ALL, // 删除模式
	CWSL_APP_DEL_ONE, // 删除模式
	CWSL_APP_MAX, // 识别模式
} cwsl_app_mode_t;

typedef enum
{
	CWSL_CMD_ID0           = 23,    
	CWSL_CMD_ID1           = 24,    
	CWSL_CMD_ID2           = 39,    
	CWSL_CMD_ID3          = 40,    
	CWSL_CMD_ID4           = 47,    
	CWSL_CMD_ID5          = 48,    
	CWSL_CMD_ID6          = 63,    
	CWSL_CMD_ID7           = 64,    
	CWSL_CMD_ID8          = 68,    
	CWSL_CMD_ID9          = 71,    
	CWSL_CMD_ID10          = 79,    
	CWSL_CMD_ID11          = 80,    
	CWSL_CMD_ID12          = 82,    
	CWSL_CMD_ID13          = 83,    
	CWSL_CMD_ID14          = 85,    
	CWSL_CMD_ID15          = 86,    
	CWSL_CMD_ID16          = 87,    
	CWSL_CMD_ID17          = 88,    
	CWSL_CMD_ID18          = 89,    
	CWSL_CMD_ID19         = 91,    

	CWSL_REGISTRATION_WAKE          = 700,      ///< 命令词：学习唤醒词
	CWSL_REGISTRATION_CMD           = 701,      ///< 命令词：学习命令词
	CWSL_REGISTER_AGAIN             = 702,      ///< 命令词：重新学习
	CWSL_CONTINUE_CMD             = 703,      ///< 命令词：重新学习
	CWSL_DELETE_WAKE                = 704,      ///< 命令词：删除唤醒词
	CWSL_DELETE_CMD                 = 705,      ///< 命令词：删除命令词
	CWSL_REGISTRATION_FAILED_EXIT         = 706,      ///< 播报：学习失败
	CWSL_REGISTRATION_FINSH         = 707,      ///< 播报：学习失败
	CWSL_ALREADY_REGISTRATION_WAKE          = 708,      ///< 命令词：学习唤醒词
	CWSL_DELETE_WAKE_SUCCESSFUL                = 709,      ///< 命令词：删除唤醒词
	CWSL_REGISTRATION_CMD_FINSH_CONTINUE_EXIT    = 710,      ///< 播报：学习入成功
	CWSL_DELETE_CMD_SUCCESSFUL            = 711,      ///< 命令词：学习命令词
	CWSL_REGISTRATION_SUCCESSFUL_AGAIN     = 712,      ///< 播报：学习成功
	CWSL_REGISTRATION_FAILE_AGAIN     = 713,      ///< 播报：学习入成功
	CWSL_ALREADY_REGISTRATION_CMD          = 714,      ///< 命令词：学习唤醒词
	CWSL_MUTE_REGISTRATION_CMD           = 715,      ///< 命令词：学习命令词
	CWSL_OTHER_CMD_AGAIN           = 716,      ///< 命令词：学习命令词
	CWSL_NO_REGISTRATION_WAKE          = 717,      ///< 命令词：学习唤醒词
	CWSL_NO_REGISTRATION_CMD           = 718,      ///< 命令词：学习命令词
	CWSL_DELETE_ALL_CMD                 = 719,      ///< 命令词：删除命令词
	CWSL_EXTI_REGISTRATION          = 720,      ///< 命令词：学习命令词
	CWSL_EXTI_DELETE         = 721,      ///< 命令词：学习命令词
	CWSL_REGISTRATION_FULL          = 722,      ///< 命令词：学习命令词
	CWSL_WANT_LEARN          = 725,      ///< 命令词：学习唤醒词

  	 CWSL_CMD_MAX_ID,    

}cicwsl_func_index;


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
uint32_t cwsl_app_process_asr_msg(cmd_handle_t cmd_handle);
uint32_t cwsl_app_process_com_msg(cmd_handle_t cmd_handle);

int cwsl_play_done_check_status(cmd_handle_t cmd_handle);
void cwsl_app_delete_word(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type);

// cwsl_manage模块复位，用于系统退出唤醒状态时调用
int cwsl_app_reset();
int get_cwsl_app_mode(void);
bool adjust_other_cmd_cur_learn_by_cmd_id(uint16_t cmd_id);
void cwsl_app_exit_reg(void);

#endif  // __CWSL_APP_SAMPLE1_H__
