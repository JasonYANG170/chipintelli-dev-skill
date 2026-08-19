/*
 * @FileName:: 
 * @Author:
 * @Date: 2022-03-10 15:24:25
 * @LastEditTime: 2022-04-09 16:52:03
 * @Description: 底层的通讯端口实现（audio and wifi）
 */
#include "ln882h.h"
#include "proj_config.h"
#include "utils/debug/log.h"
#include "serial.h"
#include "stddef.h"
#include "hal/hal_gpio.h"

#include "cias_log.h"
#include "cias_user_config.h"
#include "cias_slave_message_handle.h"
#include "cias_communication.h"
#include "cias_freertos_task.h"
#include "cias_freertos_common.h"
#include "cias_freertos_queue.h"
#include "cias_user_config.h"
#include "cias_system_manage.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
// #include "queue.h"

#define UART1_USE_MUTEX_XSEMAPHORE 1
#define RECV_BUFF_LEGTH 1024

static Serial_t g_audioSerial;
//int8_t recv_buff[RECV_BUFF_LEGTH];

#if UART1_USE_MUTEX_XSEMAPHORE
xSemaphoreHandle UART1_T_xSemaphore = NULL;

extern cias_system_manage_param_t g_cias_system_manage_param;
/******************************************************************************
      函数说明：串口1任务锁
      入口数据：无  
      返回值：  无
******************************************************************************/
void uart1_tx_mutex_init(void)
{
    UART1_T_xSemaphore = xSemaphoreCreateMutex();
}
#endif

cias_queue_t cias_send_slave_msg_queue = {
    .handle = NULL,
};
cias_queue_t cias_recv_slave_msg_queue = {
    .handle = NULL,
};

int communication_port_init(void)
{
    //memset(recv_buff, 0, RECV_BUFF_LEGTH);
     gpio_init_t_def gpio_init;

    hal_gpio_pin_afio_en(GPIOB_BASE, GPIO_PIN_3, HAL_ENABLE);
    memset(&gpio_init , 0 ,sizeof(gpio_init_t_def));
    gpio_init.dir   = GPIO_INPUT;                 //配置GPIO方向，输入或者输出
    gpio_init.pin   = GPIO_PIN_3;                  //配置GPIO引脚号
    gpio_init.speed = GPIO_HIGH_SPEED;             //设置GPIO速度
    gpio_init.mode  = GPIO_MODE_DIGITAL;           //
    gpio_init.pull  = GPIO_PULL_NONE;                //设置引脚为上拉
    hal_gpio_init(GPIOB_BASE, &gpio_init);          //初始化GPIO

//配置和语音通信串口
    serial_init(&g_audioSerial, AIOT_AUDIO_UART_NUM, AIOT_AUDIO_UART_BAND_RATE, NULL);
    return CIAS_OK;
}

void communication_rate_change(unsigned long rate)
{
    // serial_init(&g_audioSerial, AIOT_AUDIO_UART_NUM, rate, NULL);
    serial_setbaudrate(&g_audioSerial, rate);
}

int32_t communication_recv(uint8_t *addr, int32_t length)
{
    uint32_t ret = 0;
    uint8_t  flage = 1;
    if (length > RECV_BUFF_LEGTH)
    {
        LOG(LOG_LVL_INFO, "error length!!!\r\n");
        return -1;
    }
    if (fifo_len(&g_audioSerial.rxfifo))   
    {
        ret = serial_read(&g_audioSerial, addr, length);
    }
    return ret;
}

#if !UART1_USE_MUTEX_XSEMAPHORE
int32_t communication_send(int8_t *str, int32_t length)
{
    int ret = 0;
    ret = serial_write(&g_audioSerial, (const void *)str, length);
    return ret;
}
#else
int32_t communication_send(int8_t *str, int32_t length)
{
    int ret = 0;
    ret = serial_write(&g_audioSerial, (const void *)str, length);
    return ret;
}
#endif

static void cias_slave_message_send_task(void)
{
    int32_t ret = 0;
    cias_send_msg_t send_msg;
    while (1)
    {
        ret = cias_queue_receive(&cias_send_slave_msg_queue, &send_msg, CIAS_WAIT_FOREVER);
        if (ret == CIAS_OK)
        {
            #if TEST_PLAYER
                ret = communication_send(send_msg.data+16, send_msg.length-16);
                if (!ret)
                {
                    CIAS_LOG_ERR("communication_send  fail...\r\n");                
                }

            #else
                ret = communication_send(send_msg.data, send_msg.length);
                if (!ret)
                {
                    CIAS_LOG_ERR("communication_send  fail...\r\n");
                }
            #endif

            
        }
    }
}



static void cias_slave_message_recv_task(void)
{
    int32_t ret = 0;
    uint16_t temp = 0;
    uint32_t data_len = 0;
    int8_t test_gpio = 0;
    cias_standard_head_t *recv_headpart = NULL;
    static slave_recv_state_t slave_msg_state = MSG_FIND_HEAD;

    uint8_t recv_slave_buf[SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE];

    memset(recv_slave_buf, 0, SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE);
    vTaskDelay(1000);  

    while (1)
    {     
       
        if (slave_msg_state == MSG_FIND_HEAD)
        {
            ret = communication_recv(recv_slave_buf+data_len, sizeof(cias_standard_head_t)-data_len);
            if (ret >=0 && ret <= sizeof(cias_standard_head_t))
            {
                data_len += ret; 
                if(data_len < sizeof(cias_standard_head_t) ) 
                {
                    cias_delay(5);
                    continue;
                }                    
                else
                {
                    recv_headpart = (cias_standard_head_t *)recv_slave_buf;
                    data_len = 0;
                    if (recv_headpart->magic == CIAS_STANDARD_MAGIC)
                    {
                        if(recv_headpart->len == 0)
                            slave_msg_state = MSG_VERIFY;
                        else
                            slave_msg_state = MSG_RECV_MSG;
                    }
                    else
                    {                       
                        CIAS_LOG_ERR("recv_headpart magic err1\n");	
                        slave_msg_state = MSG_FIND_HEAD;       
                        memset(recv_slave_buf,0,SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE);
                    }
                }                
            }
            else
            {                
                CIAS_LOG_ERR("recv_headpart magic err2\n");	
                slave_msg_state = MSG_FIND_HEAD;       
                memset(recv_slave_buf,0,SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE);
            }								
        }
        if (slave_msg_state == MSG_RECV_MSG)
        {
            ret = communication_recv(recv_slave_buf+sizeof(cias_standard_head_t)+data_len,(recv_headpart->len - data_len));
            if (ret>=0 && ret <=recv_headpart->len)
            {
                data_len += ret; 
                if(data_len < recv_headpart->len )
                {
                    cias_delay(2);
                    continue;
                }                           
                else
                {
                    data_len = 0;
                    slave_msg_state = MSG_VERIFY;
                }
                    
            }
            else
            {
                
                CIAS_LOG_ERR("recv_headpart data err2\n");	
                data_len = 0;
                memset(recv_slave_buf,0,SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE);
                slave_msg_state = MSG_FIND_HEAD;
            }
            
        }
        if (slave_msg_state == MSG_VERIFY)
        {
            slave_msg_state = MSG_FIND_HEAD;           
            if (cias_queue_send(&cias_recv_slave_msg_queue, recv_slave_buf, pdMS_TO_TICKS(10)) != CIAS_OK)
            {
                CIAS_LOG_ERR("cias_recv_slave_msg_queue send fail...\r\n");
            }          
            data_len = 0;  
            memset(recv_slave_buf,0,SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE);
        }

        ret = 0;       
        cias_delay(5);
         
    }
}

int communication_task_init(void)
{
    if (cias_queue_create(&cias_recv_slave_msg_queue, SLAVE_DATA_RECV_QUEUE_ITEAM_COUNT, SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE) != CIAS_OK)
    {
        CIAS_LOG_ERR("cias_recv_slave_msg_queue create fail...\r\n");
    }

    if (cias_queue_create(&cias_send_slave_msg_queue, SLAVE_DATA_SEND_QUEUE_ITEAM_COUNT, SLAVE_DATA_SEND_QUEUE_ITEAM_SIZE) != CIAS_OK)
    {
        CIAS_LOG_ERR("cias_send_slave_msg_queue create fail...\r\n");
    }


    cias_task_create(&g_cias_system_manage_param.send_slave_msg_task_handle,
                     SEND_SLAVE_DATA_TASK_NAME,
                     (cias_task_function_t)cias_slave_message_send_task,
                     (void *)0,
                     SEND_SLAVE_DATA_TASK_PRIO,
                     (unsigned short)SEND_SLAVE_DATA_TASK_SIZE);
    
    cias_task_create(&g_cias_system_manage_param.recv_slave_msg_task_handle,
                     RECV_SLAVE_DATA_TASK_NAME,
                     (cias_task_function_t)cias_slave_message_recv_task,
                     (void *)0,
                     RECV_SLAVE_DATA_TASK_PRIO,
                     (unsigned short)RECV_SLAVE_DATA_TASK_SIZE);
}