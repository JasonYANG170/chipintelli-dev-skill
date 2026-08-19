#ifndef __CIAS_WORKMODE_H__
#define __CIAS_WORKMODE_H__

#include "cias_common.h"
#endif  //__CIAS_WORKMODE_H__


//工作模式
typedef enum
{
  WORKING_FACTORY_MODE_LEVEL1  = 0xff,
  WORKING_FACTORY_MODE_LEVEL2  = 0xfe,
  WORKING_APPLICATION_MODE   = 0x01,
  WORKING_FAIL_MODE
  
};

typedef struct 
{
  uint8_t word_mode;   //工作模式
  uint8_t send_elc_count;
  uint8_t light_led_interval_time;   //灯控测试间隔时间
}SystemWorkModeInfo;

SystemWorkModeInfo get_system_work_mode_info(void);