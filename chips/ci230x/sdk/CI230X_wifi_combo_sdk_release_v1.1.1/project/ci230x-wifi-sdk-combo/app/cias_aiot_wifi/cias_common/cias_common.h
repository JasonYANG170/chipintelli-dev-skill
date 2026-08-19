#ifndef __CIAS_COMMON_H__
#define __CIAS_COMMON_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cJSON.h"
#include "cias_user_config.h"
#include "cias_log.h"
#include "cias_freertos_task.h"
/* #define cias_malloc         os_malloc
#define cias_free(f)        if(f!=NULL){os_free(f);f=NULL;}
#define cias_free           os_free
#define cias_calloc         os_calloc
#define cias_zalloc         os_zalloc
#define cias_realloc        os_realloc
 */
#define bool int
#define true 1
#define false 0
#define cias_task_delay_ms(tick)    vTaskDelay((TickType_t)tick);
void cias_heap_info(void);
void cias_system_manage(void);
void cias_system_reboot(void);
#endif  //__CIAS_COMMON_H__