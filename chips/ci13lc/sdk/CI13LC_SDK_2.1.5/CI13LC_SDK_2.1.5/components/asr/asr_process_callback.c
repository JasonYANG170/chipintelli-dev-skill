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

#include <stdio.h> 
#include "sdk_default_config.h"
#include "ci_log_config.h"
#include "asr_process_callback.h"
#include "FreeRTOS.h" 
#include "task.h"
#include "semphr.h"
#include "sdk_default_config.h"
#include "ci_flash_data_info.h"
#include "flash_rw_process.h"
#include "system_msg_deal.h"
#include "ci_log.h"
#include "status_share.h"
#include "alg_preprocess.h"
#include "flash_rw_process.h"
#if USE_CWSL 
#include "cwsl_manage.h"
#endif

#define VAD_PCM_MARK_EN (1)


void asr_process_init(void)
{
    extern void ci_set_fe_is_reduce_mem(int is_8bit);
    #if ASR_FE_REDUCE_MEM
    ci_set_fe_is_reduce_mem(1);
    #else
    ci_set_fe_is_reduce_mem(0);
    #endif
    flash_ctl_init();
    ci_flash_data_info_init(DEFAULT_MODEL_GROUP_ID);
    
    ciss_set(CI_SS_DECODER_WAIT_NPU_CLEAR,false);
    ciss_set(CI_SS_NPU_WAIT_DECODER_CLEAR,false);

    uint32_t lg_model_addr;
    uint32_t lg_model_size;
    uint32_t ac_model_addr;
    uint32_t ac_model_size;
    get_current_model_addr(&ac_model_addr, &ac_model_size, &lg_model_addr, &lg_model_size);

    #if USE_CWSL
    cwsl_set_vad_alc_config(1);
    cwsl_init();
    #endif

    ciss_set(CI_SS_AC_MODEL_ADDR,ac_model_addr);
    ciss_set(CI_SS_AC_MODEL_SIZE,ac_model_size);
    ci_logdebug(LOG_ASR, "lg_model_addr = %x\n",lg_model_addr);
    extern void serial_flow_decoder_init(uint32_t lg_model_addr);
    serial_flow_decoder_init(lg_model_addr); 

    extern void serial_asr_flow_fe_vad_nn_init(uint32_t lut_table_addr);
    uint32_t lut_table_addr = ciss_get(CI_SS_LUT_ADDR);
    ci_logdebug(LOG_ASR, "lut_table_addr = %x\n",lut_table_addr);
    serial_asr_flow_fe_vad_nn_init(lut_table_addr);

    //同步语音输入的开始时序
    ciss_set(CI_SS_ASR_SYS_STATE,CI_SS_ASR_SYS_STARTED_UP);
    /* 语音输入任务 */
    //打印中间结果
    extern int ctr_asr_detail_result(int p);
    ctr_asr_detail_result(0);
    extern void audio_in_preprocess_mode_task(void* p);
    xTaskCreate(audio_in_preprocess_mode_task,"audio_in_preprocess",640,NULL,4,NULL);


    extern void config_adpt_cnt(int enable);
    config_adpt_cnt(ADAPTIVE_CNT_ENABLE);

    extern void config_max_stop_cfd(int enable,int nocnt_max_stop_cfd,int cnt_max_stop_cfd);
    config_max_stop_cfd(MAX_STOP_CFD_ENABLE,MAX_STOP_CFD_NOCNT,MAX_STOP_CFD_CNT);

    extern void config_max_vad_end_frm(int max_vad_end_frm);
    config_max_vad_end_frm(MAX_STOP_VAD_FRM);

    extern int config_base_confidence_count(short base_confidence,unsigned char valid_count);
    config_base_confidence_count(DEFAULT_CONFIDENCE,DEFAULT_CNT);

    extern void config_recover_result(int enable,int mode,int max_frm);
    config_recover_result(RECOVER_RESULT_ENABLE,RECOVER_RESULT_MODE,RECOVER_RESULT_MAX_FRM);
    
    extern void config_silprob_cnt(float base_silprob,int base_silcnt );
    config_silprob_cnt(DEFAULT_STOP_SILPROB,DEFAULT_STOP_SILCNT);
}


int set_pcm_vad_mark_flag(short *pcm_data,int frame_len)
{
    #if VAD_PCM_MARK_EN
    static int vad_start_marked = 0;//vad三帧数一算，此处为避免重复输出3次标签
    static int vad_end_marked = 0;
    
    status_t vad_state = ciss_get(CI_SS_VAD_STATE_MARK_USE);//asr_vad 获取vad_start时刻

    if((CI_SS_VAD_START == vad_state)&&(0 == vad_start_marked))
    {
        vad_start_marked = 1;
    	for(int i = 0;i < frame_len;i++)
        {
            pcm_data[i] = 30000;        
        }
        // ci_logdebug(LOG_ASR, "set vad start flag\n");
        vad_end_marked = 0;
        ciss_set(CI_SS_VAD_STATE_MARK_USE,CI_SS_VAD_ON);
    }
    
    if((CI_SS_VAD_END == vad_state)&&(0 == vad_end_marked))
    {
        vad_end_marked = 1;
        
    	for(int i = 0;i < frame_len;i++)
        {
            pcm_data[i] = -30000;        
        }
        vad_start_marked = 0;
        // ci_logdebug(LOG_ASR, "set vad end flag\n");
        ciss_set(CI_SS_VAD_STATE_MARK_USE,CI_SS_VAD_IDLE);
    }
    #endif

    return 0;
}


//语音处理完一帧的回调函数，里面的执行过程一定要尽可能短
void audio_deal_one_frm_callback(void* para)
{
    #if 0
    uint32_t* data = (uint32_t*)para;
    int16_t* voice = (int16_t*)data[0];
    ci_logdebug(LOG_ASR, "num = %d  %d %d %d\n", data[1], voice[0], voice[1], voice[2]);
    #endif
}


#if 0
int set_pcm_vad_mark_flag_set_vad_state(short *pcm_data,int frame_len,int vad_state)
{
    #if VAD_PCM_MARK_EN
    static int vad_start_marked = 0;//vad三帧数一算，此处为避免重复输出3次标签
    static int vad_end_marked = 0;
    
    if((1 == vad_state)&&(0 == vad_start_marked))
    {
        vad_start_marked = 1;
    	for(int i = 0;i < frame_len;i++)
        {
            pcm_data[i] = 30000;        
        }
        // ci_logdebug(LOG_ASR, "set vad start flag\n");
        vad_end_marked = 0;
    }
    
    if((2 == vad_state)&&(0 == vad_end_marked))
    {
        vad_end_marked = 1;
        
    	for(int i = 0;i < frame_len;i++)
        {
            pcm_data[i] = -30000;        
        }
        vad_start_marked = 0;
        // ci_logdebug(LOG_ASR, "set vad end flag\n");
    }
    #endif

    return 0;
}



/*******************************************************************************
    *功能：vad start 函数调用
    *参数：pdata 数据指针  pdata[0]:vadstart-addr,pdata[1]:帧数
    *返回：0
    *注意：none
*******************************************************************************/
int vadstart_callback(unsigned int *pdata, int line)
{
    ciss_set(CI_SS_CWSL_OUTPUT_FLAG, 0);     //清除上异常CWSL识别输出标记。
    return 0;
}

/*******************************************************************************
    *功能：vad process 函数调用
    *参数：pdata 数据指针 pdata[0]:vadcur-addr,pdata[1]:frms
    *返回：0
    *注意：none
*******************************************************************************/
int vadprocess_callback(unsigned int *pdata, int line)
{
    return 0;
}


extern void vad_light_off(void);
/*******************************************************************************
    *功能：vad end 函数调用
    *参数：pdata 数据指针  pdata[0]:vadend-addr
            pdata[1]:vad end结束类型 0 硬件 vad end, 1 超时  vad end, 2 asr
             3,系统(释放asr,暂停asr等)
    *返回：0
    *注意：none
*******************************************************************************/
int vadend_callback(unsigned int *pdata, int line)
{
    // vad_light_off();
    return 0;
}


int computevad_callback(int asrpcmbuf_addr, int pcm_byte_size, short asrfrmshift, unsigned int asrpcmbuf_start_addr, unsigned int asrpcmbuf_end_addr)
{
    return 0;
}
#endif


/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
