/**
 * @file ci13lc_timer.h
 * @brief  Timer驱动文件
 * @version 0.1
 * @date 2019-04-03
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */

#ifndef _CI13LC_TIMER_H_
#define _CI13LC_TIMER_H_

#include "ci_system.h"

/**
 * @ingroup ci13lc_chip_driver
 * @defgroup ci13lc_timer 
 * @brief CI13LC芯片TIMER驱动
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#define TIMER_S_COUNT  (get_apb_clk())         /*计时1s的count值*/
#define TIMER_MS_COUNT (get_apb_clk() / 1000)  /*计时1ms的count值*/

/**
 * @brief TIMER控制器定义
 */
typedef enum
{
	TIMER0     = HAL_TIMER0_BASE,/*!< TIMER0控制器 */
	TIMER1     = HAL_TIMER1_BASE,/*!< TIMER1控制器 */
}timer_base_t;

/**
 * @brief 计数模式定义
 */
typedef enum
{
	timer_count_mode_single = 0,/*!< 单周期模式 */
	timer_count_mode_auto = 1,	/*!< 自动重新计数模式 */
	timer_count_mode_free = 2,	/*!< 自由计数模式 */
	timer_count_mode_event = 3,	/*!< 事件计数模式 */
}timer_count_mode_t;

/**
 * @brief 分频系数定义
 */
typedef enum
{
	timer_clk_div_0 = 0,	/*!< 不分频 */
	timer_clk_div_2 = 1,	/*!< 2分频 */
	timer_clk_div_4 = 2,	/*!< 4分频 */
	timer_clk_div_16 = 3,	/*!< 16分频 */
}timer_clock_div_t;

/**
 * @brief 中断信号宽度定义
 */
typedef enum
{
	timer_iqr_width_f = 0,/*!< 由 TIMER_CFG1[CT]清除 */
	timer_iqr_width_2 = 1,/*!< 2 个时钟周期 */
	timer_iqr_width_4 = 2,/*!< 4 个时钟周期 */
	timer_iqr_width_8 = 3,/*!< 8 个时钟周期 */
}timer_iqr_width_t;

/**
 * @brief 时钟选择
 */
typedef enum
{
	timer_clk_pclk = 0,/*!< pclk  */
	timer_clk_exit = 1,/*!< exit  */
}timer_clk_sel_t;

/**
 * @brief timer配置结构体定义
 */
typedef struct
{
	timer_count_mode_t mode;/*!< 计数模式 */
	timer_clock_div_t div;/*!< 时钟分频系数 */
	timer_iqr_width_t width;/*!< 中断信号宽度 */
	unsigned int count;/*!< 计数值 */
	timer_clk_sel_t clk_sel;
}timer_init_t;

//函数接口声明
void timer_init(timer_base_t base,timer_init_t init);
void timer_set_mode(timer_base_t base,timer_count_mode_t mode);
void timer_start(timer_base_t base);
void timer_stop(timer_base_t base);
void timer_event_start(timer_base_t base);
void timer_set_count(timer_base_t base,unsigned int count);
void timer_get_count(timer_base_t base,unsigned int* count);
void timer_cascade_set(timer_base_t base,unsigned int count);
void timer_clear_irq(timer_base_t base);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
