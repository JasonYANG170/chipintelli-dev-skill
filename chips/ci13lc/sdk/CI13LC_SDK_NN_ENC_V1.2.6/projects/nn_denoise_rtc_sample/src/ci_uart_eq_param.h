#ifndef _CI_UART_EQ_TEST_H_
#define _CI_UART_EQ_TEST_H_

#include <stdint.h>
#include "ci_eq.h"
#include "ci_drc.h"
#define EQ_TEST_ENABLE      1
#define EQ_DATA_MAX_LEN     512
#define EQ_TASK_MAX_LEN     512     //最小分配-256

#define EQ_CONFIG_MAX_CH      9
#define EQ_DRC_NV_ENABEL_FLAG 0xAC
typedef struct gain_val
{
    int32_t gain[9];
    int32_t bw[9];              //带宽
    int32_t fc[9];              //中心频率
}gain_val_t;

typedef struct drc_val
{
    int32_t attack_time;
    int32_t release_time;
    int32_t ratio;
    int32_t knee;
    int32_t thr_comp;
    int32_t thr_exp;
    int32_t thr_gate;
}drc_val_t;


typedef struct eq_parme
{
    gain_val_t uart_eq_config;
    drc_val_t  uart_drc_config;
    uint32_t   param_enable_flag;   //参数生效标志
}eq_param_t;

typedef struct 
{
    float* filtered_out;
    float* gain;
    float* filter_low;
    float* filter_high;
    float* input_temp;
    float* output_temp;
    int frame_cnt;
    int16_t* pcm_tmp;
    int16_t* last_ifft;
    eq_config_t eq_param;
}npu_eq_param_t;
typedef struct 
{
    float* state;
    float* gain_out;
    drc_config_t drc_param;
}npu_drc_param_t;

void uart_eq_param_cfg_task(void * ptr);

int uart_eq_param_cfg_init(void);


#endif //_CI_UART_EQ_TEST_H_