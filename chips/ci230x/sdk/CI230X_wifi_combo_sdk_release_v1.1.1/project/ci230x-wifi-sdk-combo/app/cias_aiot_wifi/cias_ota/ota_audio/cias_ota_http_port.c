
#include "cias_common.h"
#include "cias_log.h"
#include "cias_olcp_export.h"
#include "cias_ota_http_port.h"
#include "cias_slave_message_handle.h"
#include "cias_freertos_task.h"
#include "cias_common.h"

extern cias_task_t recv_slave_msg_deal_handle;

/**
 * @brief ota olcp硬件发送函数
 * 
 * @param buff: 发送数据
 * @param len: 数据长度
 */
void http_ota_olcp_send(unsigned char *buff, unsigned int len)
{
#define ONE_PACKET_SZ 1024 //限制一包数据大小
    int remain_len = len;
    while (remain_len > 0)
    {
        if (remain_len > ONE_PACKET_SZ)
        {
            cias_message_send_interface(CIAS_OTA_DATA, DEF_FILL, ONE_PACKET_SZ, buff);
            remain_len -= ONE_PACKET_SZ;
            buff += ONE_PACKET_SZ;
        }
        else
        {
            cias_message_send_interface(CIAS_OTA_DATA, DEF_FILL, remain_len, buff);
            remain_len = 0;
            break;
        }
    }
}
/**
 * @brief oclp硬件接收函数，通过接收到的数据进行通信协议解析
 * 
 * @param buff: 原始数据
 * @param len: 数据长度
 * @retval 0: 
 */
int http_ota_olcp_recv(unsigned char *buff, unsigned int len)
{
    cias_olcp_t *ota_olcp_handle = NULL;

    ota_olcp_handle = cias_ota_get_olcp_handle();
    CIAS_ASSERT_RETURN(ota_olcp_handle != NULL);
    cias_olcp_decode(ota_olcp_handle, buff, len);

    return 0;
}

/**
 * @brief OTA 任务
 * 
 * @param parameter: NULL
 */
void _http_ota_port_task(void *parameter)
{

    
}
/**
 * @brief 创建OTA任务
 * 
 * @retval -1: error
 * @retval 0: successful
 */
#include "data_template_client.h"
int cias_http_ota_port_task_create(void)
{

    
    return 0;
}

/**
 * msh />http_ota [url]
*/
//wifi w0 join AI-Cloud Aicloud123.
