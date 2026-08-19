/**
 * @file ci13lc_iwdg.h
 * @brief  看门狗驱动文件
 * @version 0.1
 * @date 2019-04-03
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */

#ifndef _CI13LC_IWDG_H_
#define _CI13LC_IWDG_H_

#include "ci_system.h"

/**
 * @ingroup ci13lc_chip_driver
 * @defgroup ci13lc_iwdg ci13lc_iwdg
 * @brief CI13LC芯片IWDG驱动
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#define IWDG_S_COUNT  (get_src_clk() / 16)    /*计时1s的count值*/
#define IWDG_MS_COUNT (IWDG_S_COUNT / 1000)   /*计时1ms的count值*/

/**
 * @brief 控制器定义
 */
typedef enum
{
	IWDG = HAL_IWDG_BASE,/*!< IWDG控制器 */
}iwdg_base_t;

/**
 * @brief 中断开关定义
 */
typedef enum
{
	iwdg_irqen_enable = 1,/*!< 中断使能 */
    iwdg_irqen_disable = 0,/*!< 中断禁止 */
}iwdg_irqen_t;

/**
 * @brief 复位开关定义
 */
typedef enum
{
	iwdg_resen_enable = 1,/*!<复位使能 */
    iwdg_resen_disable = 0,/*!<复位禁止 */
}iwdg_resen_t;

/**
 * @brief iwdg配置结构体
 */
typedef struct
{
	unsigned int count;/*!< 计数值 */
    iwdg_irqen_t irq;/*!< 中断开关 */
    iwdg_resen_t res;/*!< 复位开关 */
}iwdg_init_t;

/*!<函数接口声明 */
void iwdg_init(iwdg_base_t base,iwdg_init_t init);
void iwdg_open(iwdg_base_t base);
void iwdg_close(iwdg_base_t base);
void iwdg_feed(iwdg_base_t base);
void iwdg_irqhander(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif