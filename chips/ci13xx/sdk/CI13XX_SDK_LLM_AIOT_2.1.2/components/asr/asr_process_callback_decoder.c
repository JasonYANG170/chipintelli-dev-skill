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
#include "platform_config.h"
#include "ci_log_config.h"
#include "asr_process_callback_decoder.h"
#include "system_msg_deal.h"
#include "status_share.h"
#include "cias_network_msg_protocol.h"
// #include "voiceprint_port.h"

#include "ci_nlp.h"
#include "ci_log.h"
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

/* #if !USE_CWSL
void get_cwsl_threshold(unsigned char *wakeup_threshold,unsigned char *cmdword_threshold )
{}
#endif */ 

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
    
    #if USE_VPR || USE_WMAN_VPR
    extern void get_asr_start_end_frm(int start,int valid);
    get_asr_start_end_frm(asr->voice_start_frame,asr->vocie_valid_frame_len);
    #endif
#if (!DEBUG_ASR_NOT_PLAY)
    if (INVALID_HANDLE != asr->cmd_handle)
    {
        
    #if (AEC_INTERRUPT_TYPE == 0)      //只有唤醒词词打断
        #if (USE_AEC_MODULE && !ONE_SHOT_ENABLE)
            if (!(cmd_info_is_wakeup_word(asr->cmd_handle)))
            {
                if (ciss_get(CI_SS_INTERCEPT_ASR_OUT))
                {
                    return 1;
                }
            }
        #endif
    #elif (AEC_INTERRUPT_TYPE == 1)    //只有命令词打断
        #if (USE_AEC_MODULE && !ONE_SHOT_ENABLE)
            if ((cmd_info_is_wakeup_word(asr->cmd_handle)))
            {
                if (ciss_get(CI_SS_INTERCEPT_ASR_OUT))
                {
                    return 1;
                }
            }
        #endif
    #endif
        #if (MULT_INTENT < 2)
           #if USE_AI_DOA
            #if USE_CWSL
            if (!ciss_get(CI_SS_CWSL_IN_REG))  //学习状态不进行doa流程
            #endif
            {
                   // mprintf("--asr->voice_start_frame = %d\r\n", asr->voice_start_frame);
                   // mprintf("asr->vocie_valid_frame_len = %d\r\n", asr->vocie_valid_frame_len);
                    ciss_set(CI_SS_WAKE_UP_START_INDEX_FOR_DOA,  asr->voice_start_frame);
                    ciss_set(CI_SS_WAKE_UP_VALID_FRAME_LEN_FOR_DOA,  asr->vocie_valid_frame_len);
                }
            #endif
        #endif
        #if USE_PWK
        //mprintf("--asr->voice_start_frame = %d\r\n", asr->voice_start_frame);
        //mprintf("--asr->vocie_valid_frame_len = %d\r\n", asr->vocie_valid_frame_len);
        ciss_set(CI_SS_WAKE_UP_START_INDEX_FOR_PWK,  asr->voice_start_frame);
        ciss_set(CI_SS_WAKE_UP_VALID_FRAME_LEN_FOR_PWK,  asr->vocie_valid_frame_len);
        #endif
        #if (MULT_INTENT < 2)
        // 如果识别结果是唤醒词 "你好，方太"，就发个无效指令给s3，让其通知服务器忽略掉
        #if CIAS_AIOT_DEMO_ENABLE
        if (0 == strcmp(asr->cmd_word, "小爱小爱"))
        #endif
        {
            cias_send_cmd(SKIP_INVAILD_SPEAK, DEF_FILL);
            mprintf("检测到唤醒词\n");
        }
        mprintf("send result:%s %d\n", asr->cmd_word, asr->confidence);
        sys_msg_t send_msg;
        send_msg.msg_type = SYS_MSG_TYPE_ASR;
        send_msg.msg_data.asr_data.asr_status = MSG_ASR_STATUS_GOOD_RESULT;
        send_msg.msg_data.asr_data.asr_cmd_handle = asr->cmd_handle;
        mprintf("asr->cmd_handle is %x\n",(asr->cmd_handle));
		
        send_msg.msg_data.asr_data.asr_score = asr->confidence;
        send_msg.msg_data.asr_data.asr_pcm_base_addr = asr->asrvoice_ptr;
        send_msg.msg_data.asr_data.asr_frames = asr->vocie_valid_frame_len;
        send_msg_to_sys_task(&send_msg, NULL);
        ret = 1;
        #else
        //mprintf("decoded_frames=%d \n", asr->frm);
        //mprintf("*************sil=%d******** \n", asr->sil_cfd);
        //mprintf("nlp asr->voice_start_frame = %d\r\n", asr->voice_start_frame);
       // mprintf("nlp asr->vocie_valid_frame_len = %d\r\n", asr->vocie_valid_frame_len);
        ret = asr_result_callback_nlp(asr->cmd_word, asr->confidence, asr->frm, asr->sil_cfd, asr->path_node_cfd, asr->voice_start_frame, asr->vocie_valid_frame_len);
        
        #endif
    }
    //else
    {
        //mprintf("asr->cmd_handle is INVALID_HANDLE\n");
        ret = 1;
    }
#endif
    return ret;
}


int asr_lite_result_callback(char * words,short cfd)
{
    //mprintf("out2 :%s %d\n",words,cfd);
    /*add code for app*/
    
    return 0;
}


/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
