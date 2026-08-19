/**
 * @file status_share.c
 * @brief 示例程序
 * @version 1.0.0
 * @date 2021-12-16
 *
 * @copyright Copyright (c) 2021  Chipintelli Technology Co., Ltd.
 *
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h" 
#include "event_groups.h"
#include "status_share.h"



#ifdef STATUS_SHARE_MODULE
volatile static status_t __attribute__((__section__(".share_mem"))) status_list[EM_STATUS_NUM] = {0};
#endif


/**
 * @brief 信息共享模块初始化.
 */
void ciss_init(void)
{
    //void other_func_rpmsg_init(void);
    //other_func_rpmsg_init();
    ci_loginfo(LOG_STATUS_SHARE, "status_list addr = %08x\n",(uint32_t)status_list);
}

/**
 * @brief 读取状态信息.
 * 
 * @param id 状态信息标识，指定要读取的状态.
 * @return 要读取的状态值.
 */
status_t ciss_get(status_id_t id)
{
    if (id < EM_STATUS_NUM)
    {
        return status_list[id];
    }
    else
    {
        return INVALID_STATUS;
    }
}

/**
 * @brief 设置状态信息，如果状态有变化且是可等待状态，发送状态等待事件标志位.
 * 
 * @param id 状态信息标识，指定要设置的状态.
 * @param value 要设置的状态值.
 */
void ciss_set(status_id_t id, status_t value)
{
    if (id < EM_STATUS_NUM)
    {
        if (status_list[id] != value)
        {
            status_list[id] = value;
        }
    }
}




