#ifndef __CWSL_APP_HANDLE_H__
#define __CWSL_APP_HANDLE_H__

#include "system_msg_deal.h"
#include "cwsl_manage.h"
typedef enum
{
    CWSL_APP_REC, // 识别模式
    CWSL_APP_REG, // 学习模式
    CWSL_APP_DEL, // 删除模式
} cwsl_app_mode_t;

typedef struct
{
    int word_id;                    // 正在学习的命令词ID
    cwsl_word_type_t word_type;     // 正在学习的命令词类型
    cwsl_app_mode_t app_mode;       // 当前工作模式
    uint8_t continus_flag;          // 是否连续学习，用于简化连续学习命令词时的提示音,0:非连续学习; 1:连续学习
} cwsl_app_t;

typedef enum
{
    CWSL_REGISTRATION_WAKE          = 200,      ///< 命令词：学习唤醒词
    CWSL_REGISTRATION_CMD           = 201,      ///< 命令词：学习命令词
    CWSL_REGISTER_AGAIN             = 202,      ///< 命令词：重新学习
    CWSL_EXIT_REGISTRATION          = 203,      ///< 命令词：退出学习
    CWSL_DELETE_FUNC                = 204,      ///< 命令词：我要删除
    CWSL_DELETE_WAKE                = 205,      ///< 命令词：删除唤醒词
    CWSL_DELETE_CMD                 = 206,      ///< 命令词：删除命令词
    CWSL_EXIT_DELETE                = 207,      ///< 命令词：退出删除模式
    CWSL_DELETE_ALL                 = 208,      ///< 命令词：全部删除

    CWSL_REGISTRATION_NEXT          = 199,      ///< 播报：学习下一个
  //CWSL_DATA_ENTERY_SUCCESSFUL     = 209,      ///< 播报：录入成功
    CWSL_DATA_ENTERY_FAILED         = 210,      ///< 播报：学习失败
    CWSL_REGISTRATION_SUCCESSFUL    = 211,      ///< 播报：学习成功
    CWSL_TEMPLATE_FULL              = 212,      ///< 播报：学习模板超过上限
    CWSL_SPEAK_AGAIN                = 219,      ///< 播报：请再说一次>
    CWSL_TOO_SHORT                  = 220,      ///< 播报：语音长度不够，请再说一次>

    CWSL_DELETE_SUCCESSFUL          = 213,      ///< 播报：删除成功
    CWSL_REGISTRATION_ALL           = 217,      ///< 播报：学习完成
    CWSL_REG_FAILED                 = 218,      ///< 播报：学习失败
    CWSL_REG_FAILED_DEFAULT_CMD_CONFLICT = 221, ///< 播报：与默认指令冲突，请换种说法
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
uint32_t cwsl_app_process_asr_msg(sys_msg_asr_data_t *asr_msg, cmd_handle_t *cmd_handle, uint16_t cmd_id);

// cwsl_manage模块复位，用于系统退出唤醒状态时调用
int cwsl_app_reset();

#endif  // __CWSL_APP_HANDLE_H__
