/*!
\file tuya_ir_control_internal.h
Copyright(C),2017, 涂鸦科技 www.tuya.comm
*/

#ifndef _TUYA_IR_CONTROL_INTERNAL_H
#define _TUYA_IR_CONTROL_INTERNAL_H

#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TY_IR_SEND_DELAY_TM_MS          300
#define TY_IR_KEY_NUM_MAX               4
#define TY_IR_DATA_LEN_MAX              (1024 * 2)
#define TY_IR_FZ_DECPMPRESS_MAX_LEN     (600*2)
#define TY_IR_STUDY_FEQ                 38000
#define TY_IR_STUDY_FRAME_COUNT         1
#define TY_IR_LEARN_USE_TYLIB           0           /* 学习code时采用tuya码库的方式(转化存储count值)                */
#define TY_MAX_STUDY_END_TIME           100000      /* 最长的帧（1个bit）为1s */
#define TY_STUDY_CODE_MAX               65000
#define TY_MAX_STUDY_CODE_LEN           (1024)
#define TY_MAX_STUDY_FZ_CODE_LEN        (1024*2)
#define TY_ERR_CODE_LEN                 9

typedef CHAR_T TY_IR_CODE_TYPE_S;
#define    TY_IR_TYPE_SELF_LIB       0
#define    TY_IR_TYPE_KK_LIB         3         /* 酷控码 */



typedef CHAR_T TY_IR_CODE_MODE_S;
#define     TY_IR_CODE_MODE_LIB         48
#define     TY_IR_CODE_MODE_STUDY       49
#define     TY_IR_FZCODE_MODE_STUDY     50


typedef struct {
    TY_IR_CODE_TYPE_S   type;
    USHORT_T            delay;
    CHAR_T              *p_head;
    UCHAR_T             key_num;
    CHAR_T              *key[TY_IR_KEY_NUM_MAX];
    
}TY_IR_SEND_CONFIG_S;

OPERATE_RET tuya_ir_control(IN CONST ty_cJSON *json);


#ifdef __cplusplus
}
#endif

#endif  

