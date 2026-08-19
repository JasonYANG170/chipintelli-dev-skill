/*
 * @FileName::
 * @Author: 
 * @Date: 2022-03-04 10:41:12
 * @LastEditTime: 2025-02-09 18:15:41
 * @Description: Network driven implementation 主要用于和wifi芯片通讯
 */
#include <stdint.h>
#include <string.h>
#include "sdk_default_config.h"
#include "ci130x_uart.h"
#include "ci130x_dma.h"
#include "cias_uart_protocol.h"
#include "system_msg_deal.h"
#include "ci_log.h"
#include "cias_audio_data_handle.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "codec_manager.h"
#include "audio_play_process.h"
#include "audio_play_device.h"
#include "audio_play_api.h"
#include "stream_buffer.h"

#if AUDIO_DATA_PLAY_BY_UART || AUDIO_DATA_UPLOAD_BY_UART
StreamBufferHandle_t network_msg_rcv_stream_buffer = NULL;  // WiFi数据消息接收缓存
extern audio_play_os_stream_t mp3_player;
extern audio_play_os_sem_t mp3_player_end;
uint32_t uart_rx_toatal = 0;
uint16_t uart1_index_total = 0;
extern SemaphoreHandle_t network_send_sem ;

bool network_port_recv_queue_init(void)
{
    network_msg_rcv_stream_buffer = xStreamBufferCreate(NETWORK_RECV_BUFF_MAX_SIZE, 1);
    if (network_msg_rcv_stream_buffer == NULL)
    {
        mprintf("network_msg_rcv_stream_buffer create fail\r\n");
        return false;
    }
    return true;
}
#if UART_NUM_SEND_PLAY_AUDIO_NUMBER == HAL_UART0_BASE
void UART0_IRQHandler(void)
#elif UART_NUM_SEND_PLAY_AUDIO_NUMBER == HAL_UART1_BASE
void UART1_IRQHandler(void)
#elif UART_NUM_SEND_PLAY_AUDIO_NUMBER == HAL_UART2_BASE
void UART2_IRQHandler(void)
#endif
{
    static int8_t uart_rx_temp = 0;
    UART_TypeDef *uart = (UART_TypeDef *)UART_NUM_SEND_PLAY_AUDIO_NUMBER;
    /*接受数据*/
    if ((uart->UARTMIS & (1UL << UART_RXInt)) || (uart->UARTMIS & (1UL << UART_RXTimeoutInt)))
    {
        while (!(uart->UARTFlag & (0x1 << 6))) // read fifo have data
        {
            if (network_msg_rcv_stream_buffer != NULL)
            {
                uart_rx_temp = (int8_t)UartPollingReceiveData(UART_NUM_SEND_PLAY_AUDIO_NUMBER);
                // mprintf("-%02x ", uart_rx_temp);
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                int tx_size = xStreamBufferSendFromISR(network_msg_rcv_stream_buffer, &uart_rx_temp, 1, &xHigherPriorityTaskWoken);
                if (tx_size != 1)
                {
                    mprintf("network send err, uxQueueSpacesAvailable = %d\r\n", xStreamBufferSpacesAvailable(network_msg_rcv_stream_buffer));
                    xStreamBufferReset(network_msg_rcv_stream_buffer);
                }
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        }
        UART_IntClear(uart, UART_RXInt);
    }
    uart->UARTICR = 0xFFF;
}

int8_t network_send_Byte(int8_t ch)
{
    // 发送FIFO满标志
    if (UART_FLAGSTAT(UART_NUM_SEND_PLAY_AUDIO_NUMBER, UART_TXFF))
    {
        return 0;
    }
    UartPollingSenddata(UART_NUM_SEND_PLAY_AUDIO_NUMBER, ch);
#if CI230X_AUDIO_DATA_OUT_BY_UART // 203x采音
    if (UART_FLAGSTAT(HAL_UART2_BASE, UART_TXFF))
    {
        return 0;
    }
    UartPollingSenddata(HAL_UART2_BASE, ch);
#endif
    return 1;
}

int32_t network_send(int8_t *str, uint32_t length)
{
    int32_t ret = 0;
    SemaphoreHandle_t xReturn = xSemaphoreTake(network_send_sem, pdMS_TO_TICKS(1000)); 
	if(!xReturn)
	{
	   return 0;
	}
    if (str && length)
    {
        while (length && (ret < NETWORK_SEND_BUFF_MAX_SIZE))
        {
            // ci_loginfo(LOG_USER, "%02x ", str[ret]);
            if (network_send_Byte(str[ret]))
            {
                ret++;
                length--;
            }
        }
    }
    xReturn = xSemaphoreGive( network_send_sem );//给出互斥量
    return ret;
}

/** 网络通讯初始化
 *  入口参数：void
 *  return： 0
 */
int8_t network_uart_port_init(void)
{
    UARTInterruptConfig(UART_NUM_SEND_PLAY_AUDIO_NUMBER, UART_NUM_SEND_PLAY_AUDIO_BAUDRATE);
#if CI230X_AUDIO_DATA_OUT_BY_UART
    UARTInterruptConfig(HAL_UART2_BASE, UART_BaudRate921600);
#endif
    return 0;
}
#endif