#include <stdlib.h>
#include <stdint.h>
#include "system_msg_deal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ci_log.h"
#include "sdk_default_config.h"
#include "cias_network_msg_protocol.h"
#include "cias_network_msg_send_task.h"
#include "ci130x_uart.h"
#include "ci130x_core_misc.h"
#include "cias_uart_protocol.h"
#include "cias_audio_data_handle.h"
#include "cias_common.h"
#include "user_config.h"

uint8_t broad_link_buf[NETWORK_SEND_BUFF_NUM * NETWORK_SEND_BUFF_MAX_SIZE] = {0}; // 
static QueueHandle_t network_msg_queue = NULL;
SemaphoreHandle_t network_send_sem =NULL;
bool network_send_task_init(void)
{
#if NET_AUDIO_PLAY_BY_PCM && AUDIO_COMPRESS_RECORD_DISABLE   //精简播放播放pcm需要更大的消息队列，不然播放会卡顿
    network_msg_queue = xQueueCreate(8, sizeof(sdio_task_msg_t));
#else
    network_msg_queue = xQueueCreate(4, sizeof(sdio_task_msg_t));
#endif
    if(!network_msg_queue)
    {
        mprintf("network_msg_queue creat fail\r\n");
        return false;
    }
	network_send_sem = xSemaphoreCreateMutex();
    if (NULL == network_send_sem)
    {
        mprintf("network_MuxSem creat fail\r\n");
        return false;
    }
    return true;
}



/**
 * @brief A simple code for other component send system message to this module, just wrap freertos queue function
 * 
 * @param flag_from_isr : 0 call this function not from isr, other call this from isr
 * @param send_msg : system message
 * @param xHigherPriorityTaskWoken : if call this not from isr set NULL
 * @return BaseType_t 
 */
BaseType_t send_msg_to_network_task(sdio_task_msg_t *send_msg, BaseType_t *xHigherPriorityTaskWoken)
{
    //if(0 != __get_IPSR())
    if(0 != check_curr_trap())
    {
        return xQueueSendFromISR(network_msg_queue,send_msg, xHigherPriorityTaskWoken);
    }
    else
    {
        return xQueueSend(network_msg_queue, send_msg, 100);
    }
}


int32_t broadlink_frame_create(uint32_t type, const uint8_t *input, uint32_t len, uint8_t *output, uint32_t maxsize, uint32_t fill_data, uint16_t index)
{
    cias_standard_head_t *head = (cias_standard_head_t *)output;
    static uint16_t index_packg = 0;
#if AUDIO_SEND_WITH_PROTOCOL_HEADER
    /* Check param */
    if (output == NULL || len + sizeof(cias_standard_head_t) > maxsize)
    {
        ci_logerr(LOG_MEDIA,"broadlink_frame_create err\n");
        return -1;
    }
    /* DNA standard header initialization */
    memset(head, 0, sizeof(cias_standard_head_t));
    head->magic = CIAS_STANDARD_MAGIC;  
    head->type = type;
    head->len = len;
    head->version = 0x100;   
    for(int i = 0; i < len; i++)
    {
        head->checksum += input[i];
    }
    if(type == PCM_MIDDLE)
    {
        head->version = index_packg;
        index_packg++;
    }
    else
        index_packg = 0;
    
    head->fill_data = fill_data;
    /* Fill data part */
    memcpy(output + sizeof(cias_standard_head_t), input, len);
    return head->len + sizeof(cias_standard_head_t);
#else
    memcpy(output, input, len);
    return head->len;
#endif
}


int cais_send_msg_flag_to_network(int8_t *send_buffer)
{
    network_send(send_buffer,strlen(send_buffer));
}

int32_t cias_send_cmd(uint32_t cmd, uint32_t fill_data)
{
#if !AUDIO_SEND_WITH_PROTOCOL_HEADER
    return 0;
#endif
    uint8_t send_msg_buf[16] = {0};
    sdio_task_msg_t send_msg;
    broadlink_frame_create(cmd, NULL, 0, send_msg_buf, 16, fill_data, 0);
    memcpy((void *)send_msg.network_data.cmd, send_msg_buf, 16);
    send_msg.network_data.type = ONLY_CMD;
    send_msg.msg_type = SDIO_DMA_SEND;
    send_msg.network_data.length = 16;
    send_msg.network_data.flag = 0;
    if (pdPASS != send_msg_to_network_task(&send_msg, NULL))
    {
        mprintf("cias_send_cmd err:%d\n", __LINE__);
    }
    return cmd;
}

/**
 * @brief
 *
 * @param packet_type 数据包类型
 * @param voice_data 音频数据
 * @param packet_len 数据长度
 * @return int -1 错误， 返回包长度
 */
int32_t cias_send_cmd_and_data(uint32_t cmd_type, uint8_t *buf, int32_t len, uint32_t fill_data)
{
#if !AUDIO_SEND_WITH_PROTOCOL_HEADER
    return 0;
#endif
    static int32_t buff_count = 0;
    int32_t package_length = 0;
    sdio_task_msg_t send_msg;
    package_length = broadlink_frame_create(cmd_type, buf, len, (broad_link_buf + buff_count * NETWORK_SEND_BUFF_MAX_SIZE), NETWORK_SEND_BUFF_MAX_SIZE, fill_data, 0);
    send_msg.network_data.payload = (uint32_t)(broad_link_buf + buff_count * NETWORK_SEND_BUFF_MAX_SIZE);
    send_msg.msg_type = SDIO_DMA_SEND;
    send_msg.network_data.type = CMD_AND_DATA;
    send_msg.network_data.length = package_length;
    if (cmd_need_ack(cmd_type) == true)
    {
        send_msg.network_data.flag = 1; /*需要ACK*/
    }
    else
    {
        send_msg.network_data.flag = 0; /*不需要ACK*/
    }
    if (pdPASS != send_msg_to_network_task(&send_msg, NULL))
    {
        mprintf("sdio err3\n");
    }
    buff_count++;
    if (buff_count >= NETWORK_SEND_BUFF_NUM)
    {
        buff_count = 0;
    }
    return package_length;
}


/**
 * @brief 打包发送音频数据
 * 
 * @param packet_type 数据包类型
 * @param voice_data 音频数据
 * @param packet_len 数据长度
 * @return int -1 错误， 返回包长度
 */
int voice_data_packet_and_send(uint32_t packet_type,uint8_t *voice_data, int32_t packet_len)
{
    static int32_t buff_count = 0;
    sdio_task_msg_t send_msg;
    
    broadlink_frame_create(packet_type, voice_data, packet_len, (broad_link_buf + buff_count * NETWORK_SEND_BUFF_MAX_SIZE), NETWORK_SEND_BUFF_MAX_SIZE, 0x12345678,0);
    send_msg.network_data.payload = (uint32_t)(broad_link_buf + buff_count * NETWORK_SEND_BUFF_MAX_SIZE);
    send_msg.msg_type = SDIO_DMA_SEND;
    send_msg.network_data.type = AUDIO_MIDDLE;
#if AUDIO_SEND_WITH_PROTOCOL_HEADER
    send_msg.network_data.length = packet_len + 16;
#else
    send_msg.network_data.length = packet_len;
#endif
    send_msg.network_data.flag = 0;
    if(pdPASS != send_msg_to_network_task(&send_msg,NULL))
    {
        mprintf("sdio err3\n");
    }
    buff_count++;
    if(buff_count >= NETWORK_SEND_BUFF_NUM)
    {
        buff_count = 0;
    }

    return 0;
}

/**
 * @brief 打包发送cjson
 * 
 * @param packet_type   数据包类型
 * @param data          cjson数据
 * @param packet_len    数据长度
 * @return int -1 错误，返回包长度
 */
int cias_cjson_packet_and_send(uint32_t packet_type,uint8_t *data, int32_t packet_len)
{
    static int32_t buff_count = 0;
    sdio_task_msg_t send_msg;
    
    broadlink_frame_create(CIAS_CJSON_DATA, data, packet_len, (broad_link_buf + buff_count * NETWORK_SEND_BUFF_MAX_SIZE), NETWORK_SEND_BUFF_MAX_SIZE, 0x12345678,0);
    send_msg.network_data.payload = (uint32_t)(broad_link_buf + buff_count * NETWORK_SEND_BUFF_MAX_SIZE);
    send_msg.msg_type = SDIO_DMA_SEND;
    send_msg.network_data.type = AUDIO_MIDDLE;
    send_msg.network_data.length = packet_len + 16;
    send_msg.network_data.flag = 0;
    if(pdPASS != send_msg_to_network_task(&send_msg,NULL))
    {
        ci_logdebug(LOG_MEDIA,"sdio err3\n");
    }
    
    buff_count++;
    if(buff_count >= NETWORK_SEND_BUFF_NUM)
    {
        buff_count = 0;
    }
    return 0;
}


/**
 * @brief 串口发送任务
 * 
 * @param p_arg 
 */
void network_send_data_task(void *p_arg)
{
    sdio_task_msg_t rev_msg;
    BaseType_t ret = pdPASS;
    network_msg_data_t *data;
    int16_t i = 0;
    while(1)
    {
        ret = xQueueReceive(network_msg_queue, &rev_msg, portMAX_DELAY);   
        if(pdPASS == ret)
        {
           // ci_loginfo(LOG_USER,"rev_msg.msg_type = %d\r\n", rev_msg.msg_type); 
            int8_t *p =  (int8_t *)data->payload;   
            switch (rev_msg.msg_type)
            {
                case SDIO_DMA_SEND:
                {                    
                    data = &rev_msg.network_data;
                    //mprintf("data->length = %d, data->type = %d\r\n", data->length, data->type);
                    if(data->type == CMD_AND_DATA)
                    {
                        ret = network_send((int8_t *)data->payload, data->length);
                    }
                    else if(data->type == ONLY_CMD)
                    {
                        ret = network_send((int8_t *)data->cmd, data->length);
                    }
                    else
                    {
                    //    mprintf("data->length = %d\r\n", data->length);
                    //    uint8_t *p = (int8_t *)data->payload;
                    //    for(int i = 0; i < data->length; i++)
                    //    {
                    //         mprintf("%02x", (uint8_t)p[i]);
                    //    }
                       #if AUDIO_SEND_WITH_PROTOCOL_HEADER
                       ret = network_send((int8_t *)data->payload, data->length);  
                       #else
                       ret = network_send((int8_t *)data->payload + 16, data->length - 16);   
                       #endif   
                    }
                    break;
                } 
                default:
                {
                 
                    break;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));    //发送加延时，防止接收端串口buffer溢出
    }
}

