#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "sdk_default_config.h"
#include "voice_module_uart_protocol.h"
#include "system_msg_deal.h"
#include "command_info.h"
#include "ci130x_spiflash.h"
#include "ci130x_dpmu.h"
#include "prompt_player.h"
#include "audio_play_api.h"
// #include "flash_rw_process.h"
#include "asr_api.h"
#include "firmware_updater.h"
#include <string.h>
#include <stdlib.h>
#include "romlib_runtime.h"
#include "flash_manage_outside_port.h"
#include "baudrate_calibrate.h"
#include "ota_config.h"
#include "user_config.h"
#define VMUP_MSG_HEAD_LOW  (0xA5)
#define TIMEOUT_ONE_PACKET_INTERVAL (1000)/*ms, in this code, it should be bigger than portTICK_PERIOD_MS */
#define MAX_DATA_RECEIVE_PER_PACKET (80)/*???*/
TickType_t last_ota_time;
static bool vmup_port_timeout_one_packet(void)
{
    TickType_t now_time;
    TickType_t timeout;
    
    now_time = xTaskGetTickCountFromISR();

    timeout = (now_time - last_ota_time);/*uint type, so overflow just used - */

    last_ota_time = now_time;

    if(timeout > TIMEOUT_ONE_PACKET_INTERVAL/portTICK_PERIOD_MS) /*also as timeout = timeout*portTICK_PERIOD_MS;*/
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void vump_uart_irq_ota_handler(void)
{
    /*发送数据*/
    if (((UART_TypeDef*)UART_NUM_SEND_PLAY_AUDIO_NUMBER)->UARTMIS & (1UL << UART_TXInt))
    {
        UART_IntClear((UART_TypeDef*)UART_NUM_SEND_PLAY_AUDIO_NUMBER,UART_TXInt);
    }
    /*接受数据*/
    if (((UART_TypeDef*)UART_NUM_SEND_PLAY_AUDIO_NUMBER)->UARTMIS & (1UL << UART_RXInt))
    {
        //here FIFO DATA must be read out
        vmup_receive_ota_packet(UART_RXDATA((UART_TypeDef*)UART_NUM_SEND_PLAY_AUDIO_NUMBER));
        UART_IntClear((UART_TypeDef*)UART_NUM_SEND_PLAY_AUDIO_NUMBER,UART_RXInt);
    }

    UART_IntClear((UART_TypeDef*)UART_NUM_SEND_PLAY_AUDIO_NUMBER,UART_AllInt);
}

int vmup_port_hw_ota_init(void)
{
#if HAL_UART0_BASE == UART_NUM_SEND_PLAY_AUDIO_NUMBER
    __eclic_irq_set_vector(UART0_IRQn, (int32_t)vump_uart_irq_ota_handler);
#elif (HAL_UART1_BASE == UART_NUM_SEND_PLAY_AUDIO_NUMBER)
    __eclic_irq_set_vector(UART1_IRQn, (int32_t)vump_uart_irq_ota_handler);
#else
    __eclic_irq_set_vector(UART2_IRQn, (int32_t)vump_uart_irq_ota_handler);
#endif
    UARTInterruptConfig((UART_TypeDef *)UART_NUM_SEND_PLAY_AUDIO_NUMBER, UART_NUM_SEND_PLAY_AUDIO_BAUDRATE);
    return RETURN_OK;
}

/********************************************************************
                     receive function
********************************************************************/
/*receive use state machine method, so two char can arrive different time*/
typedef enum 
{
    REV_STATE_HEAD0   = 0x00,
    REV_STATE_HEAD1   = 0x01,
    REV_STATE_LENGTH0 = 0x02,
    REV_STATE_LENGTH1 = 0x03,
    REV_STATE_TYPE    = 0x04,
    REV_STATE_CMD     = 0x05,
    REV_STATE_SEQ     = 0x06,
    REV_STATE_DATA    = 0x07,
    REV_STATE_CKSUM0  = 0x08,
    REV_STATE_CKSUM1  = 0x09,
    REV_STATE_TAIL    = 0x0a,
}vmup_receive_state_t;

static uint8_t rev_state = REV_STATE_HEAD0;
static uint16_t length0 = 0, length1 = 0;
static uint16_t chk_sum0 = 0, chk_sum1 = 0;
static uint16_t data_rev_count = 0;

static uint16_t ota_payload_len = 0;
extern cias_ota_pack_t ota_uart_pack; 
bool ota_new_message = false;
bool get_new_ota_message(void)
{
    return ota_new_message;
}
void clear_ota_new_message(void)
{
    ota_new_message = false;
}

void vmup_receive_ota_packet(uint8_t receive_char)
{
    if(true == vmup_port_timeout_one_packet())
    {
        rev_state = REV_STATE_HEAD0;
    }
    //mprintf("%02x ",receive_char);
    switch(rev_state)
    {
        case REV_STATE_HEAD0:
            if(VMUP_MSG_HEAD_LOW == receive_char)
            {
                rev_state = REV_STATE_HEAD1;
            }
            else
            {
                rev_state = REV_STATE_HEAD0;
            }
            break;
        case REV_STATE_HEAD1:
            if(MSG_OTA_HEAD1 == receive_char)
            {
                rev_state = REV_STATE_LENGTH0;
                ota_uart_pack.head0 = MSG_OTA_HEAD0;
                ota_uart_pack.head1 = MSG_OTA_HEAD1;
            }
            else
            {
                if(VMUP_MSG_HEAD_LOW != receive_char)
                {
                    rev_state = REV_STATE_HEAD0;
                }
            }
            break;
        case REV_STATE_LENGTH0:
            ota_uart_pack.len0 = receive_char;
            rev_state = REV_STATE_LENGTH1;
            break;
        case REV_STATE_LENGTH1:
            ota_uart_pack.len1 = receive_char;
            rev_state = REV_STATE_TYPE;
            data_rev_count = 0;
            ota_payload_len = get_ota_pack_payload_len(&ota_uart_pack);
            break;
        case REV_STATE_TYPE:
            ota_uart_pack.msg_type = receive_char;
            if (!ota_payload_len)
                rev_state = REV_STATE_CKSUM0;
            else
                rev_state = REV_STATE_DATA;
            break;
        case REV_STATE_DATA:
            //mprintf("%02x ",(uint8_t)receive_char);
            ota_uart_pack.data[data_rev_count++] = receive_char;
            if(data_rev_count == ota_payload_len)
            {
                rev_state = REV_STATE_CKSUM0;
            }
            break;
        case REV_STATE_CKSUM0:
            ota_uart_pack.crc0 = receive_char;
            rev_state = REV_STATE_CKSUM1;
            break;
        case REV_STATE_CKSUM1:
            ota_uart_pack.crc1 = receive_char;
            rev_state = REV_STATE_TAIL;
            break;
        case REV_STATE_TAIL:
            ota_uart_pack.tail = receive_char;
            rev_state = REV_STATE_HEAD0;
            if (ota_uart_pack.tail == MSG_OTA_TAIL)
            {
                ota_new_message = true;
                //send_ota_recv_msg(&ota_uart_pack, NULL);
            }
            break;
        default:
            rev_state = REV_STATE_HEAD0;
            break;
	}
}

extern int8_t network_send_Byte(int8_t ch);
//ota数据发送的串口函数，默认使用sdk的原始协议串口，客户可以自己修改发送串口函数
int vmup_send_ota_ack_packet(cias_ota_pack_t * msg)
{
    uint8_t *buf = (uint8_t*)msg;
    int i;
    uint16_t payload_len = get_ota_pack_payload_len(msg);
    if(msg == NULL)
    {
        return RETURN_ERR;
    }        
    /*header*/ 
    //network_send(buf, 5);
     for(i = 0; i < 5; i++)
    {
        network_send_Byte(buf[i]);
    } 
    /* data */
    //network_send(msg->data[i], payload_len);
    for(i = 0;i < payload_len; i++)
    {
        network_send_Byte(msg->data[i]);
    } 
    buf += 9;
    /*tail*/
    //network_send(buf, 3);
     for(i = 0;i < 3; i++)
    {
        network_send_Byte(buf[i]);
    }
    return RETURN_OK;
}

/* void vmup_send_ota()
{
    sys_msg_com_data_t msg;

    msg.header = VMUP_MSG_HEAD;
    msg.data_length = 0;
    msg.msg_type = VMUP_MSG_TYPE_CMD_DOWN;
    msg.msg_cmd = VMUP_MSG_CMD_ENTER_OTA_MODE;
    msg.msg_seq = 0;
    vmup_send_packet(&msg);
}
 */

