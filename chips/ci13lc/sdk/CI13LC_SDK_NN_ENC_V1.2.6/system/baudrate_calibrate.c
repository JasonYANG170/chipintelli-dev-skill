
#include "sdk_default_config.h"
#include "ci_log.h"
#include "ci_uart.h"
#include "ci_log_config.h"
#include "ci_gpio.h"
#include "ci_dpmu.h"
#include "ci_timer.h"
#include "FreeRTOS.h"
#include "system_msg_deal.h"
#include "baudrate_calibrate.h"
#include "board.h"




#if UART_BAUDRATE_CALIBRATE


void baudrate_calibrate_init(UART_TypeDef *UARTx, uint32_t baudrate)
{
    // UartBaudIntMask(UARTx, 0, ENABLE);
    // UartBaudSampleRateSet(UARTx, (baudrate < 460800) ? UART_BAUD_SAMPLERATE_128:UART_BAUD_SAMPLERATE_64);
    // UartBaudSampleRateSet(UARTx, UART_BAUD_SAMPLERATE_64);
    // UartBaudCheckEnable(UARTx,ENABLE);
}

void baudrate_calibrate(UART_TypeDef *UARTx)
{
    // mprintf("Enter BaudRate calibrate\r\n");
    if (UartBaudIntRawStatus(UARTx, UART_BAUD_INT_DTCT_VAILD)) 
    {
        // mprintf("RawStatus:%x\r\n", UARTx->BAUD_RIS);
        
        uint32_t dir = UartBaudStatusRead(UARTx);
        uint32_t offset = UartBaudSample0ReadCount(UARTx);
        // mprintf("BaudRate offset count:%d\r\n", offset);
        uint32_t total_count = UartBaudSample0ReadBit(UARTx)*UartBaudSampleRateGet(UARTx);
        // mprintf("total count:%d\r\n", total_count);
        float offset_percent = ((float)offset)/((float)total_count);
        float percent = (dir == UART_BAUD_STATUS_HIGH) ? (offset_percent + 1.0f) : (1.0f - offset_percent);
        // mprintf("BaudRate offset:%d\r\n", (uint32_t)(percent*1000));
        if (percent > 1.01f || percent < 0.99f)
        {
            uint32_t UART_CLK = get_ahb_clk()/2;
            uint32_t cur_bd = UART_CLK/((UARTx->UARTIBrd*16 + (UARTx->UARTFBrd>>2)));
            mprintf("Cur BD:%d\r\n", cur_bd);
            uint32_t new_bd = (uint32_t)((float)cur_bd/percent);
            mprintf("New BD:%d\r\n", new_bd);
            UARTInterruptConfig(UARTx, new_bd);
        }
    }
}





#endif


