#include <stdbool.h>
#include "ci_log.h"
#include "cwsl_app_sample1.h"
#include "cwsl_manage.h"
#include "cwsl_template_manager.h"
#include "prompt_player.h"

extern void set_state_enter_wakeup(uint32_t exit_wakup_ms);


#define CWSL_WAKEUP_NUMBER 2                    // 可注册的唤醒词数量
#define WAKE_UP_ID 1                            // 注册的唤醒词对应的命令词ID
#define CWSL_CMD_NUMBER ((sizeof(reg_cmd_list) / sizeof(reg_cmd_list[0])))     // 可注册的命令词数量

#define MAX_LEARN_REPEAT_NUMBER     6           // 学习时，重复的最大次数


typedef struct cwsl_reg_asr_struct
{
    int reg_cmd_id;         // 命令词ID
    int reg_play_id;        // 注册提示播报音ID
}cwsl_reg_asr_struct_t;

const cwsl_reg_asr_struct_t reg_cmd_list[]=
{   //命令词ID     //注册提示播报音ID
    {2,             1001},
    {3,             1002},
    {4,             1003},
    {5,             1004},
};

typedef enum
{
    CWSL_APP_REC, // 识别模式
    CWSL_APP_REG, // 注册模式
    CWSL_APP_DEL, // 删除模式
} cwsl_app_mode_t;

typedef struct
{
    int word_id;             // 正在注册的命令词ID
    cwsl_word_type_t word_type; // 正在注册的命令词类型
    cwsl_app_mode_t app_mode;       // 当前工作模式
    uint8_t continus_flag;          // 是否连续注册，用于简化连续学习命令词时的提示音,0:非连续学习; 1:连续学习
} cwsl_app_t;

cwsl_app_t cwsl_app;

static int get_next_reg_cmd_word_index();

////cwsl 事件响应函数////////////////////////////////////////////////

// cwsl模块初始化事件响应
// 必须返回可注册的模板数量
int on_cwsl_init()
{    
    cwsl_app.app_mode = CWSL_APP_REC;
    CI_ASSERT(CICWSL_TOTAL_TEMPLATE >= (CWSL_WAKEUP_NUMBER + CWSL_CMD_NUMBER), "not enough template space\n");
    return CWSL_WAKEUP_NUMBER + CWSL_CMD_NUMBER;
}

// 播放回调处理
static void cwsl_play_done_callback_default(cmd_handle_t cmd_handle)
{
}

// 播放回调处理, 开始录制模板
static void cwsl_play_done_callback_with_start_record(cmd_handle_t cmd_handle)
{
    cwsl_reg_record_start(); // 播放完 "开始学习" 提示音后，开始录制模板
}

// 注册开始事件响应
int on_cwsl_reg_start(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type)
{    
    set_state_enter_wakeup(EXIT_WAKEUP_TIME); // 更新退出唤醒时间
    ci_logdebug(LOG_CWSL, "==on_cwsl_reg_start\n");
    cwsl_reg_record_stop(); // 在提示音播放期间，关闭模板录制功能

    if (word_type == WAKEUP_WORD)
    {
        // 播放提示音 "开始学习唤醒词"
        prompt_play_by_cmd_id(CWSL_REGISTRATION_WAKE, -1, cwsl_play_done_callback_with_start_record, false);
    }
    else
    {
        if (!cwsl_app.continus_flag) // 如果是连续注册，就不播报“开始注册”
        {
            prompt_play_by_cmd_id(CWSL_REGISTRATION_CMD, -1, cwsl_play_done_callback_default, false);
        }

        // int next_cmd_index = get_next_reg_cmd_word_index();
        prompt_play_by_cmd_id(reg_cmd_list[cwsl_app.word_id].reg_play_id, -1, cwsl_play_done_callback_with_start_record, false);
    }
    return 0;
}

// 注册停止事件响应
int on_cwsl_reg_abort()
{
    ci_logdebug(LOG_CWSL, "==on_cwsl_reg_abort\n");
    prompt_play_by_cmd_id(CWSL_EXIT_REGISTRATION, -1, cwsl_play_done_callback_default, true);
    cwsl_app.app_mode = CWSL_APP_REC;
    return 0;
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
    set_state_enter_wakeup(EXIT_WAKEUP_TIME); // 更新退出唤醒时间
    ci_logdebug(LOG_CWSL, "==on_cwsl_record_end %d,%d\n", times, result);

    if (cwsl_app.app_mode == CWSL_APP_REG)
    {
        if (CWSL_RECORD_SUCCESSED == result)
        {
            if (MAX_LEARN_REPEAT_NUMBER > times)
            {
                prompt_play_by_cmd_id(CWSL_DATA_ENTERY_SUCCESSFUL, -1, cwsl_play_done_callback_with_start_record, true);
            }
            else
            {
                // 注册次数超过上限，自动退出
                prompt_play_by_cmd_id(CWSL_REG_FAILED, -1, cwsl_play_done_callback_with_start_record, true);
                cwsl_recognize_start(ALL_WORD);
                cwsl_app.app_mode = CWSL_APP_REC; // 转回识别模式
            }
        }
        else if (CWSL_RECORD_FAILED == result)
        {
            prompt_play_by_cmd_id(CWSL_DATA_ENTERY_FAILED, -1, cwsl_play_done_callback_with_start_record, true);
        }
        else if (CWSL_REG_FINISHED == result)
        {
            prompt_play_by_cmd_id(CWSL_REGISTRATION_SUCCESSFUL, -1, cwsl_play_done_callback_default, true);

            if (CMD_WORD == cwsl_app.word_type && CWSL_CMD_NUMBER > cwsl_tm_get_reg_tpl_number(CMD_WORD))
            {
                int next_cmd_index = get_next_reg_cmd_word_index();
                cwsl_app.word_id = next_cmd_index;
                cwsl_app.continus_flag = 1;                     // 指定是连续注册，用于简化播报提示音
                cwsl_reg_word(reg_cmd_list[next_cmd_index].reg_cmd_id, 0, CMD_WORD);
            }
            else
            {
                cwsl_recognize_start(ALL_WORD);
                cwsl_app.app_mode = CWSL_APP_REC; // 转回识别模式
            }
        }
    }
    return 0;
}

// 删除模板成功事件响应
int on_cwsl_delete_successed()
{
    prompt_play_by_cmd_id(CWSL_DELETE_SUCCESSFUL, -1, cwsl_play_done_callback_default, true);
    cwsl_app.app_mode = CWSL_APP_REC; // 删除成功后，转回识别模式
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
static int get_next_reg_cmd_word_index()
{
    int ret = 0;

    // 查找注册的命令词
    uint8_t cmd_word_tm_index[CWSL_CMD_NUMBER];
    int cmd_tpl_count = cwsl_tm_get_words_index(cmd_word_tm_index, CWSL_CMD_NUMBER, -1, CMD_WORD);
    for (int i = 0; i < CWSL_CMD_NUMBER; i++)
    {
        int find_flag = 0;
        for (int j = 0; j < cmd_tpl_count; j++)
        {
            if (cwsl_tm_get_tpl_cmd_id_by_index(cmd_word_tm_index[j]) == reg_cmd_list[i].reg_cmd_id)
            {
                find_flag = 1;
                break;
            }
        }
        if (!find_flag)
        {
            ret = i;
            return ret;
        }
    }
    return ret;
}

////cwsl API///////////////////////////////////////////////

// 学习唤醒词
void cwsl_app_reg_word(cwsl_word_type_t word_type)
{
    if (cwsl_app.app_mode == CWSL_APP_REC)
    {
        if (WAKEUP_WORD == word_type)
        {
            if (CWSL_WAKEUP_NUMBER > cwsl_tm_get_reg_tpl_number(WAKEUP_WORD))
            {
                cwsl_app.word_type = WAKEUP_WORD;
                cwsl_reg_word(WAKE_UP_ID, 0, WAKEUP_WORD);
                cwsl_app.app_mode = CWSL_APP_REG;
            }
            else
            {
                // 唤醒词已经注册满了
                prompt_play_by_cmd_id(CWSL_TEMPLATE_FULL, -1, cwsl_play_done_callback_default, true);
            }
        }
        else if (CMD_WORD == word_type)
        {
            if (CWSL_CMD_NUMBER > cwsl_tm_get_reg_tpl_number(CMD_WORD))
            {
                int next_cmd_index = get_next_reg_cmd_word_index();
                cwsl_app.word_id = next_cmd_index;
                cwsl_app.word_type = CMD_WORD;
                cwsl_reg_word(reg_cmd_list[next_cmd_index].reg_cmd_id, 0, CMD_WORD);
                cwsl_app.app_mode = CWSL_APP_REG;
                cwsl_app.continus_flag = 0;
            }
            else
            {
                // 命令词已经注册满了
                prompt_play_by_cmd_id(CWSL_TEMPLATE_FULL, -1, cwsl_play_done_callback_default, true);
            }
        }
    }
}

// 重新学习
void cwsl_app_reg_word_restart()
{
    if (cwsl_app.app_mode == CWSL_APP_REG)
    {
        cwsl_reg_restart();
    }
}

// 播放回调处理, 退出注册
static void cwsl_play_done_callback_with_exit_reg(cmd_handle_t cmd_handle)
{
    cwsl_exit_reg_word();
}

// 退出注册
void cwsl_app_exit_reg()
{
    if (cwsl_app.app_mode == CWSL_APP_REG)
    {
        cwsl_reg_record_stop();
        cwsl_app.app_mode = CWSL_APP_REC;
        prompt_play_by_cmd_id(CWSL_EXIT_REGISTRATION, -1, cwsl_play_done_callback_with_exit_reg, true);
    }
}

// 进入删除模式
void cwsl_app_enter_delete_mode()
{
    if (cwsl_app.app_mode == CWSL_APP_REC)
    {
        cwsl_app.app_mode = CWSL_APP_DEL;
        prompt_play_by_cmd_id(CWSL_DELETE_FUNC, -1, cwsl_play_done_callback_default, true);
    }
}

// 退出删除模式
void cwsl_app_exit_delete_mode()
{
    if (cwsl_app.app_mode == CWSL_APP_DEL)
    {
        cwsl_app.app_mode = CWSL_APP_REC;
        prompt_play_by_cmd_id(CWSL_EXIT_DELETE, -1, cwsl_play_done_callback_default, true);
    }
}

// 删除指定类型模板
// cmd_id: 指定要删除的命令词ID, 传入-1为通配符，忽略此项
// group_id: 指定要删除的命令词分组号, 传入-1为通配符，忽略此项
// word_type: 指定要删除的命令词类型，传入-1为通配符，忽略此项
void cwsl_app_delete_word(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type)
{
    if (cwsl_app.app_mode == CWSL_APP_DEL)
    {
        cwsl_delete_word(cmd_id, group_id, word_type);
    }
}

// cwsl_manage模块复位，用于系统退出唤醒状态时调用
int cwsl_app_reset()
{
    cwsl_app.app_mode = CWSL_APP_REC;
    cwsl_manage_reset();
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

uint32_t cwsl_app_process_asr_msg(sys_msg_asr_data_t *asr_msg, cmd_handle_t *cmd_handle, uint16_t cmd_id)
{
    uint32_t ret = 0;

    switch(cmd_id)
    {
    case CWSL_REGISTRATION_WAKE:
        cwsl_app_reg_word(WAKEUP_WORD);
        ret = 2;
        break;
    case CWSL_REGISTRATION_CMD:
        cwsl_app_reg_word(CMD_WORD);
        ret = 2;
        break;
    case CWSL_REGISTER_AGAIN:
        cwsl_app_reg_word_restart();
        ret = 2;
        break;
    case CWSL_EXIT_REGISTRATION:
        cwsl_app_exit_reg();
        ret = 2;
        break;
    case CWSL_DELETE_FUNC:
        cwsl_app_enter_delete_mode();
        ret = 2;
        break;
    case CWSL_EXIT_DELETE:
        cwsl_app_exit_delete_mode();
        ret = 2;
        break;
    case CWSL_DELETE_WAKE:
        cwsl_app_delete_word((uint32_t)-1, (uint16_t)-1, WAKEUP_WORD);
        ret = 2;
        break;
    case CWSL_DELETE_CMD:
        cwsl_app_delete_word((uint32_t)-1, (uint16_t)-1, CMD_WORD);
        ret = 2;
        break;
    case CWSL_DELETE_ALL:
        cwsl_app_delete_word((uint32_t)-1, (uint16_t)-1, ALL_WORD);
        ret = 2;
        break;
    default:
        if (cwsl_app.app_mode == CWSL_APP_REG || cwsl_app.app_mode == CWSL_APP_DEL)
        {
            send_nn_end_msg_to_cwsl();
            ret = 2;
        }
        break;
    }
    return ret;
}
