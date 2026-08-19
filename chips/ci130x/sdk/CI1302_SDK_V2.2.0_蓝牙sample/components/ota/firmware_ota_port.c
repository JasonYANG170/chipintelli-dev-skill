#include "firmware_ota_port.h"
#include "FreeRTOS.h" 
#include "task.h"
#include "queue.h"
#include "ci130x_system.h"
#include "ci_log.h"

#if FIRMWARE_OTA_ENABEL
volatile int irq_disable = 0;
static QueueHandle_t msg_recv_queue = NULL;
int8_t uart_temp = 0;
#define SUPPORT_OTA_PACKAGE_DATA_LEN   128

//ota接收数据队列初始化
bool ota_port_recv_queue_init(void)
{
    msg_recv_queue = xQueueCreate(64*3, sizeof(int8_t));
    if (msg_recv_queue == NULL)
    {
        mprintf("msg_recv_queue create fail\r\n");
        return false;
    }
    return true;
}

//处理串口接收任务指令
void ota_recv_cmd_task(void *parameter)
{
    int32_t recv_package_length = 0;
    int32_t package_length = 0;
    int8_t  msg_state = NET_MSG_IDE;
    BaseType_t err;
    cias_data_standard_head_t *data_header;
    int8_t package_data[SUPPORT_OTA_PACKAGE_DATA_LEN] = {0};
    while (1)
    {
        package_length = 0;
        while(pdPASS == xQueueReceive(msg_recv_queue, &package_data[package_length], portMAX_DELAY))
        {
            //mprintf("package_length = %d\r\n", package_length);
            package_length++;
            if(msg_state == NET_MSG_IDE) 
                msg_state = NET_MSG_HEAD;
            else if ((msg_state == NET_MSG_HEAD) && (package_length > (sizeof(cias_data_standard_head_t)-2)))
            {
                data_header = (cias_data_standard_head_t *)package_data;
                if(data_header->magic == 0x5a5aa5a5)
                    msg_state = NET_MSG_DATE; 
                else
                    msg_state = NET_MSG_ERR;
            }
            else if(msg_state == NET_MSG_DATE && package_length > (data_header->len + sizeof(cias_data_standard_head_t) - 1))
            {
                data_header = (cias_data_standard_head_t *)package_data;
                recv_package_length = data_header->len + sizeof(cias_data_standard_head_t); 
                mprintf("data_header->type = %x, \r\n\r\n", data_header->type);                
                if(data_header->type == 0x0501)
                {
                    irq_disable = 1;
                    mprintf("start reset system...");
                    vTaskDelay(pdMS_TO_TICKS(10));   
                    dpmu_software_reset_system_config();
                }
                package_length = 0;
                memset(package_data,0,SUPPORT_OTA_PACKAGE_DATA_LEN);
                msg_state = NET_MSG_IDE; 
            }
            if(msg_state == NET_MSG_ERR)
            {
                ci_logdebug(LOG_USER, "ci_standard_head_t error\r\n");
                package_length = 0;
                memset(package_data,0,SUPPORT_OTA_PACKAGE_DATA_LEN);
                msg_state = NET_MSG_IDE; 
            }
        }
        if(msg_state != NET_MSG_IDE)
        {
            ci_logdebug(LOG_USER, "recv data timeout error(%d)\r\n",package_length);
            package_length = 0;
            memset(package_data, 0, SUPPORT_OTA_PACKAGE_DATA_LEN);
            msg_state = NET_MSG_IDE; 
        }
    }
}
//ota任务初始化
void firmware_ota_task_init(void)
{
    UARTInterruptConfig(OTA_CMD_PORT, UART_BaudRate115200);
    if(ota_port_recv_queue_init())
    {
        xTaskCreate(ota_recv_cmd_task, "ota_recv_cmd_task", 480, NULL, 4, NULL);
    }
    else
    {
        mprintf("firmware_ota_task_init fail ...\r\n");
    }
}
#endif//CIAS_FLASH_IMAGE_UPGRADE_ENABLE