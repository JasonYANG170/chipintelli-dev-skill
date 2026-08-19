#include "cias_system_manage.h"

//腾讯IOT
#include "qcloud_iot_import.h"
#include "data_template_client.h"
#include "cias_ota_main.h"
#include "cias_ln_hardware.h"
#include "cias_freertos_queue.h"
#include "Config.h"
#include "cias_ota_config.h"

cias_system_manage_param_t g_cias_system_manage_param;
static void cias_system_manage_task(void *parameter)
{
    g_cias_system_manage_param.recv_slave_msg_deal_flag = 0;
    g_cias_system_manage_param.qcloud_main_handle.handle = NULL;
    g_cias_system_manage_param.recv_slave_msg_deal_handle.handle = NULL;
#if CIAS_IOT_TENCENT_ENABLE
    
#elif CIAS_IOT_TUYA_ENABLE
    while (1)
    {
        if (g_cias_system_manage_param.tuya_ir_down_flag == 1) //涂鸦开始下载，删除多余的任务
        {
            g_cias_system_manage_param.tuya_ir_down_flag = 0;
            // g_cias_system_manage_param.recv_slave_msg_deal_flag = 1;
            // g_cias_system_manage_param.recv_slave_msg_task_flag = 1;
            // g_cias_system_manage_param.send_slave_msg_task_flag = 1;
            extern cias_queue_t cias_recv_slave_msg_queue;
            extern cias_queue_t cias_send_slave_msg_queue;
            cias_queue_delete(&cias_send_slave_msg_queue);   //删除队列
            cias_queue_delete(&cias_recv_slave_msg_queue);  //删除队列
            CIAS_LOG_HL("tuy ir download task start.....\r\n");
            if (g_cias_system_manage_param.recv_slave_msg_deal_handle.handle)
                cias_task_delete(&g_cias_system_manage_param.recv_slave_msg_deal_handle);
            if (g_cias_system_manage_param.recv_slave_msg_task_handle.handle)
                cias_task_delete(&g_cias_system_manage_param.recv_slave_msg_task_handle);
            if (g_cias_system_manage_param.send_slave_msg_task_handle.handle)
                 cias_task_delete(&g_cias_system_manage_param.send_slave_msg_task_handle);
        }
        cias_task_delay_ms(1000);
    }
#else
    while(1)
    {
        // CIAS_LOG_HL("\r\n cias_task_manage 7s\r\n");
        cias_task_delay_ms(7000);
    }
#endif
}

void cias_system_manage(void)
{
    cias_task_t qcloud_system_manage_handle = {
        .handle = NULL,
    };
    cias_task_create(&qcloud_system_manage_handle,
                     "cias_system_manage",
                     (cias_task_function_t)cias_system_manage_task,
                     (void *)0,
                     QCLOUD_IOT_TASK_PRIORITY,   //优先级减小
                     (unsigned short)1024*2);
}