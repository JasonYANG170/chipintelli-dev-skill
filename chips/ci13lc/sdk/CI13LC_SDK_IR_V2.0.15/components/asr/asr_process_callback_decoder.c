/**
  ******************************************************************************
  * @文件    asr_process_callbak.c
  * @版本    V1.0.1
  * @日期    2019-3-15
  * @概要  asr 回调函数，VAD相关
  ******************************************************************************
  * @注意
  *
  * 版权归chipintelli公司所有，未经允许不得使用或修改
  *
  ******************************************************************************
  */
#include <stdlib.h>
#include <stdio.h>

#include "sdk_default_config.h"
#include "ci_log_config.h"
#include "asr_process_callback_decoder.h"
#include "system_msg_deal.h"
#include "status_share.h"
// #include "voiceprint_port.h"

#include "ci_log.h"

#if (MULTI_INTENTS > 1)
int nlp_cmd_cnt_default()
{
    return NLP_CMD_CNT_DEFAULT;
}
int nlp_cmd_cnt_end()
{
    return NLP_CMD_CNT_END;
}
int nlp_stl_len()
{
    return NLP_STL_LEN;
}
int get_nlp_mult_intent()
{
    return MULTI_INTENTS;
}
int nlp_mult_intent_equal_status(short status)
{
    if(MULTI_INTENTS == status)
        return 1;
    else
        return 0;
}
int nlp_cmd_nodes_cfd_times()
{
    return NLP_NODES_CFD_TIMES;
}
int get_power_on_wait_times()
{
    return NLP_POWER_ON_WAIT_TIMES;
}
int get_power_off_wait_times()
{
    return NLP_POWER_OFF_WAIT_TIMES;
}
int get_cwsl_status()
{
    return USE_CWSL;
}
int nlp_mult_intent_greater_than_status(short status)
{
    if(MULTI_INTENTS >= status)
        return 1;
    else
        return 0;
}
extern int asr_result_callback_nlp(char* cmd, int cfd, int decoded_frames, short sli_cfd, short path_node_cfd, short start_frame, short frame_len);
#endif
  //luqoiang
int send_asr_prediction_msg(uint32_t cmd_handle)
{
    return 0;
}

int send_asr_pre_cancel_msg(void)
{
    return 0;
}

int send_asr_pre_confirm_msg(uint32_t cmd_handle)
{
    return 0;
}

#if !USE_CWSL
void get_cwsl_threshold(unsigned char *wakeup_threshold,unsigned char *cmdword_threshold )
{}
#endif 

int asr_result_callback(callback_asr_result_type_t *asr)
{
    #if USE_CWSL
    extern int deal_cwsl_cmd(char * cmd_word,cmd_handle_t *cmd_handle,short *confidence,int unwakeup_flag,int *cwsl_flag);
    int unwakeup_flag = 0;
    if(SYS_STATE_UNWAKEUP == get_wakeup_state())
    {
        unwakeup_flag = 1;
    }
    int cwsl_flag ;
    int exit_flag = deal_cwsl_cmd(asr->cmd_word,&asr->cmd_handle,&asr->confidence,unwakeup_flag,&cwsl_flag);
    asr->cwsl_flag = cwsl_flag;
    if(exit_flag)
    {
        return 1;
    }
    #endif
    short ret = 0;
    
    #if (MULTI_INTENTS < 2)
    mprintf("send result:%s %d\n", asr->cmd_word, asr->confidence);
    #endif

#if 1
    if ((cmd_handle_t)INVALID_HANDLE != asr->cmd_handle)
    {
        #if USE_AEC_MODULE
        if (!(cmd_info_is_wakeup_word(asr->cmd_handle)))
        {
            if (ciss_get(CI_SS_INTERCEPT_ASR_OUT))
            {
                return 1;
            }
        }
        #endif
        
        #if (MULTI_INTENTS < 2) 
        sys_msg_t sys_msg;
        sys_msg.msg_type = SYS_MSG_TYPE_ASR;
        sys_msg_asr_data_t *msg_data = (sys_msg_asr_data_t*)sys_msg.msg_data; 
        msg_data->asr_status = MSG_ASR_STATUS_GOOD_RESULT;
        msg_data->asr_cmd_handle = (uint32_t)asr->cmd_handle;
        ci_loginfo(LOG_ASR, "asr->cmd_handle is %x\n",(asr->cmd_handle));
        msg_data->asr_score = asr->confidence;
        msg_data->asr_pcm_base_addr = asr->asrvoice_ptr;
        msg_data->asr_frames = asr->vocie_valid_frame_len;
        send_sys_msg_inner(&sys_msg, sizeof(sys_msg), NULL);
        ret = 1;
        #else
        #if USE_CWSL
        if (ciss_get(CI_SS_CWSL_IN_REG))
        {
            sys_msg_t sys_msg;
            sys_msg.msg_type = SYS_MSG_TYPE_ASR;
            sys_msg_asr_data_t *msg_data = (sys_msg_asr_data_t*)sys_msg.msg_data; 
            msg_data->asr_status = MSG_ASR_STATUS_GOOD_RESULT;
            msg_data->asr_cmd_handle = (uint32_t)asr->cmd_handle;
            mprintf("send result:%s %d\n", asr->cmd_word, asr->confidence);
            ci_loginfo(LOG_ASR, "asr->cmd_handle is %x\n",(asr->cmd_handle));
            msg_data->asr_score = asr->confidence;
            msg_data->asr_pcm_base_addr = asr->asrvoice_ptr;
            msg_data->asr_frames = asr->vocie_valid_frame_len;
            send_sys_msg_inner(&sys_msg, sizeof(sys_msg), NULL);
            ret = 1; 
        }
        else
        #endif
        {
            asr->cmd_word = cmd_info_get_command_string(asr->cmd_handle);
            if(!asr->cmd_word)
            {
                ci_logerr(LOG_ASR, "cmd_word & cmd_handle is INVALID_HANDLE\n");
            }
            ret = asr_result_callback_nlp(asr->cmd_word, asr->confidence, asr->frm, asr->sil_cfd, asr->path_node_cfd, asr->voice_start_frame, asr->vocie_valid_frame_len);
        }
        #endif
    }
    else
    {
        #if (!(MULTI_INTENTS > 0))
        ci_logerr(LOG_ASR, "asr->cmd_handle is INVALID_HANDLE\n");
        #endif
        ret = 1;
    }
#endif
    return ret;
}


int asr_lite_result_callback(char * words,short cfd)
{
    //ci_logdebug(LOG_ASR, "out2 :%s %d\n",words,cfd);
    /*add code for app*/
    
    return 0;
}


/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
