/**
 * @file ci_system.h
 * @brief chip级定义
 * @version 1.0
 * @date 2024-05-30
 * 
 * @copyright Copyright (c) 2024 Chipintelli Technology Co., Ltd.
 * 
 */
#ifndef _CI_SYSTEM_H_
#define _CI_SYSTEM_H_

#include <stdint.h>
#include <stdbool.h>
#include "user_config.h"
#include "sdk_default_config.h"
#include "ci13lc_system.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define CONCAT(a,b) a##b
#define INCLUDE_BOARD_HEADER(a,b) TOSTRING(CONCAT(a,b).h)
#define PREFEX  ci
#include INCLUDE_BOARD_HEADER(PREFEX, CI_CHIP_TYPE)

/*******function return defines******/
#define INT32_T_MAX (0x7fffffff)
#define INT32_T_MIN (0x80000000)

enum _retval
{
    RETURN_OK = 0,       RET_SUCCESS = RETURN_OK,
    PARA_ERROR = -1,     RET_INVALIDARGMENT = PARA_ERROR,
    RETURN_ERR = -2,     RET_FAIL = RETURN_ERR,
                         RET_MOMEM = -3,
                         RET_READONLY = -4,
                         RET_OUTOFRANGE = -5,
                         RET_TIMEOUT = -6,
                         RET_NOTRANSFEINPROGRESS = -7,

                         RET_UNKNOW = INT32_T_MIN,
};


typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))


#ifndef NULL
#define NULL 0
#endif

void _delay_10us(uint32_t cnt);
uint32_t get_ipcore_clk(void);
uint32_t get_ahb_clk(void);
uint32_t get_apb_clk(void);
uint32_t get_systick_clk(void);
uint32_t get_osc_clk(void);
uint32_t get_src_clk(void);

void set_ipcore_clk(uint32_t clk);
void set_ahb_clk(uint32_t clk);
void set_apb_clk(uint32_t clk);
void set_systick_clk(uint32_t clk);
void set_osc_clk(uint32_t clk);
void set_src_clk(uint32_t clk);
void maskrom_lib_init(void);

void init_platform(void);
void init_clk_div(void);
void init_irq_pri(void);
void pa_switch_io_init(void);
bool get_pa_control_level_flag(void);
int vad_start_mark(void);
int vad_end_mark(void);
void init_dma_channel0_mutex(void);

float get_freq_factor();


#endif

/************************ (C) COPYRIGHT chipintelli *****END OF FILE****/











