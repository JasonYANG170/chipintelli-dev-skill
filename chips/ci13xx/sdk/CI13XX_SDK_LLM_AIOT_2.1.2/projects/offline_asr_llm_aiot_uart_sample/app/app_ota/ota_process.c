#include <stdio.h> 
#include <string.h>
#include <malloc.h>
#include "FreeRTOS.h" 
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "ci_flash_data_info.h"
#include "user_config.h"
#include "ci_log.h"
#include "crc.h"
#include "ota_config.h"

TimerHandle_t ota_request_timer = NULL;
cias_ota_pack_t ota_uart_pack;  //在串口中断接收函数使用的ota接收结构体
cias_ota_pack_t ota_ack_pack;   //ack回复函数使用的结构体
void init_ota_ack_pack(cias_ota_pack_t *ota_pack)
{
    ota_pack->head0 = MSG_OTA_HEAD0;
    ota_pack->head1 = MSG_OTA_HEAD1;
    ota_pack->len0 = 0;
    ota_pack->tail = MSG_OTA_TAIL;
    ota_pack->data = pvPortMalloc(OTA_ACK_PACK_LEN);
}

uint16_t get_ota_pack_payload_len(cias_ota_pack_t *ota_pack)
{
    uint16_t payload_len = ((uint16_t)ota_pack->len0 << 8) | ota_pack->len1 - OTA_PACK_PALOAD_HEAD_LEN;
    return payload_len;
}

void get_ota_send_pack_crc(cias_ota_pack_t *ota_pack)
{
    uint16_t crc = 	crc16_ccitt(0, ota_pack, OTA_ACK_PACK_HEAD_LEN);
    crc = crc16_ccitt(crc, ota_pack->data, get_ota_pack_payload_len(ota_pack));
    ota_pack->crc0 = crc >> 8;
    ota_pack->crc1 = crc & 0xff;
}

void get_ota_ack_version(cias_ota_pack_t *ota_pack)
{
    ota_pack->len1 = OTA_PACK_ACK_VERSION_HEAD_LEN;
    ota_pack->msg_type = MSG_TYPE_OTA_VERSION;
    memset(ota_pack->data, 0, OTA_ACK_PACK_LEN);
    get_ota_version(ota_pack->data);
    get_ota_send_pack_crc(ota_pack);
}

void get_ota_ack_start(cias_ota_pack_t *ota_pack)
{
    ota_pack->len1 = OTA_PACK_ACK_START_HEAD_LEN;
    ota_pack->msg_type = MSG_TYPE_OTA_START;
    get_ota_send_pack_crc(ota_pack);
}

void get_ota_ack_firmware(cias_ota_pack_t *ota_pack, bool ret, uint16_t pkg_current, uint16_t pkg_next)
{
    ota_pack->len1 = OTA_PACK_ACK_FIRMWAER_HEAD_LEN;
    ota_pack->msg_type = MSG_TYPE_OTA_FIRMWARE;
    memset(ota_pack->data, 0, OTA_ACK_PACK_LEN);
    ota_pack->data[0] = ret;
    if(ret)
    {
        ota_pack->data[1] = pkg_current >> 8;
        ota_pack->data[2] = pkg_current & 0xff;
    }
    else
    {
        ota_pack->data[1] = pkg_current;
    }
    ota_pack->data[3] = pkg_next >> 8;
    ota_pack->data[4] = pkg_next & 0xff;
    get_ota_send_pack_crc(ota_pack);
}

void get_ota_ack_finish(cias_ota_pack_t *ota_pack, bool ret)
{
    ota_pack->len1 = OTA_ACK_PACK_HEAD_LEN;
    ota_pack->msg_type = MSG_TYPE_OTA_FINISH;
    memset(ota_pack->data, 0, OTA_ACK_PACK_LEN);
    ota_pack->data[0] = ret;
    get_ota_send_pack_crc(ota_pack);
}

void get_ota_ack_request(cias_ota_pack_t *ota_pack)
{
    ota_pack->len1 = OTA_PACK_ACK_REQUEST_HEAD_LEN;
    ota_pack->msg_type = MSG_TYPE_OTA_REQUEST;
    memset(ota_pack->data, 0, OTA_ACK_PACK_LEN);
    get_ota_version(ota_pack->data);
    get_ota_send_pack_crc(ota_pack);
}

bool verify_ota_pack(cias_ota_pack_t *ota_pack)
{
    uint16_t payload_len = get_ota_pack_payload_len(ota_pack);
	uint16_t crc = 	crc16_ccitt(0, ota_pack, OTA_ACK_PACK_HEAD_LEN);
    if(payload_len)
    {
        crc = crc16_ccitt(crc, ota_pack->data, payload_len);
    }
	if ((ota_pack->crc0 != (crc >> 8)) || (ota_pack->crc1 != (crc & 0xff)))
	{
		mprintf("verify_ota_ack erro %x, %x, %x\r\n", crc, ota_pack->crc0, ota_pack->crc1);
		return false;
	}
	else	
		return true;
}

void ota_request_timer_callback()
{
    get_ota_ack_request(&ota_ack_pack);
    vmup_send_ota_ack_packet(&ota_ack_pack);
}

void ota_main_task(void)
{
    /*创建ota数据接收通知消息队列，为了节约内存，消息队列只含有ota头消息，ota接收有效数据通过串口中断接收后
    直接放在定义的全局变量ota_uart_pack.data里面 */
    //ota_recv_msg = xQueueCreate(1, sizeof(cias_ota_pack_t)); 
    //cias_ota_pack_t ota_recv_pack;   //消息队列接收函数使用的收结构体
    bool start_ota_status = false;
    init_ota_ack_pack(&ota_ack_pack);  
    ota_uart_pack.data = pvPortMalloc(OTA_PACK_LENGTH + 2); //在串口中断接收函数使用的ota接收有效数据缓存buffer
    uint16_t payload_len;
    uint16_t pkg_len;
    uint16_t pkg_current;
    uint16_t pkg_count;
    uint16_t pkg_next;
    int8_t status;
    if (partition_check != PARTITION_OK)
    {
		ota_request_timer = xTimerCreate("ota_request_timer", pdMS_TO_TICKS(3000), pdTRUE, (void *)0, ota_request_timer_callback);
		xTimerStart(ota_request_timer, 0);
    }
    while (1)
    {
        //BaseType_t rst = xQueueReceive(ota_recv_msg, &ota_uart_pack, portMAX_DELAY);
        bool rst = get_new_ota_message();
        if (rst)
        {
            //mprintf("ota_recv_msg\r\n");
            clear_ota_new_message();
            if (!verify_ota_pack(&ota_uart_pack))
            {
                continue;
            }
            switch (ota_uart_pack.msg_type)
            {
                case MSG_TYPE_OTA_VERSION:
                    get_ota_ack_version(&ota_ack_pack);
                    vmup_send_ota_ack_packet(&ota_ack_pack);
                    break;

                case MSG_TYPE_OTA_START:
                    pkg_current = 0xffff;
                    pkg_count = ((uint16_t)ota_uart_pack.data[3] << 8) + ota_uart_pack.data[4];
                    pkg_len = ((uint16_t)ota_uart_pack.data[5] << 8) + ota_uart_pack.data[6];
                    mprintf("ota_start %d %d\r\n",pkg_count, pkg_len);
                    ota_start();
                    get_ota_ack_start(&ota_ack_pack);
                    vmup_send_ota_ack_packet(&ota_ack_pack);
                    start_ota_status = true;
                    break;
                case MSG_TYPE_OTA_FIRMWARE:
                    if (!start_ota_status)
                        continue;
                    if (pkg_current == (((uint16_t)ota_uart_pack.data[0] << 8) | ota_uart_pack.data[1])) //判断帧号是否重复
                    {
                        mprintf("ota_write_flash repeat %d\r\n", pkg_current);
                    }
                    else
                    {
                        payload_len = get_ota_pack_payload_len(&ota_uart_pack) - OTA_PACK_PALOAD_FIRMWAER_HEAD_LEN;
                        pkg_current = ((uint16_t)ota_uart_pack.data[0] << 8) | ota_uart_pack.data[1];
                        mprintf(" %d ", pkg_current);
                        ota_write_flash_data(&ota_uart_pack.data[2], payload_len);
                    }
                    status = get_ota_partition_status(&pkg_next, pkg_count, pkg_len);
                    if (status == 1)
                        get_ota_ack_firmware(&ota_ack_pack, true, pkg_current, pkg_next);  //分区写入到了结尾，获取一下分区的包序号
                    else if(status == 0)
                        get_ota_ack_firmware(&ota_ack_pack, true, pkg_current, pkg_current + 1);   //分区未写完，继续获取下一包
                    else
                        get_ota_ack_firmware(&ota_ack_pack, false, status, 0);    //分区写入有错误，直接返回失败                   
                    vmup_send_ota_ack_packet(&ota_ack_pack);
                    break;

                case MSG_TYPE_OTA_FINISH:
                    mprintf("ota finish\r\n");
                    get_ota_ack_finish(&ota_ack_pack, 1);
                    vmup_send_ota_ack_packet(&ota_ack_pack);
                    start_ota_status = false;
                    _delay_10us_240M(20000);
                    dpmu_software_reset_system_config();
                    break;

                default:
                    break;
            }
        }
        else
        {
            _delay_10us_240M(1000);
        }
            
    }
}