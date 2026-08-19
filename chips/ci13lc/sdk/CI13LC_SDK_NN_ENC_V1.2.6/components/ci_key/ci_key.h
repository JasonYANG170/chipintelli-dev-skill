#ifndef __CI_KEY_H
#define __CI_KEY_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include <string.h>
#include "ci13lc_system.h"
#include "ci13lc_core_eclic.h"
#include "ci13lc_scu.h"
#include "ci13lc_dpmu.h"
#include "system_msg_deal.h"

#ifdef __cplusplus
extern "C"
{
#endif

//!按键消抖时间为2*10ms
#define KEY_SHORTPRESS_TIMER    (2)
//!按键长按间隔时间为150*10ms
#define KEY_LONGPRESS_TIMER     (100)

/**
 * @addtogroup key
 * @{
 */
void ci_key_gpio_init(void);
void ci_key_io_init(void);
void ci_key_init(void);
void key_info_show(sys_msg_key_data_t *key_msg);
void system_start_time_test(void);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif

