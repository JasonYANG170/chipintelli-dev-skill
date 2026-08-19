/**
 * @file   ln_at_transfer.c
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018-2020 LightningSemi Technology Co., Ltd. All rights reserved.
 * 
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-28     MurphyZhao   the first version
 */

#include "ln_at_transfer.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "serial/serial.h"
#include "proj_config.h"
#ifdef CFG_UART_BAUDRATE_CONSOLE
  #define CONSOLE_PORT_BAUDRATE       CFG_UART_BAUDRATE_CONSOLE
#else
  #define CONSOLE_PORT_BAUDRATE       (115200)
#endif

#if (defined(AT_LOG_MERGE_TO_UART0) && (AT_LOG_MERGE_TO_UART0 == 1))
extern Serial_t m_LogSerial;
#endif // !AT_LOG_MERGE_TO_UART0

static Serial_t       g_at_serial;
static ln_at_transfer_t g_at_transfer;

static void ln_serial_rx_callbcak(void);
int  ln_transfer_init(void);
static int  ln_transfer_deinit(void);
static int  ln_transfer_write(const char *buf, size_t size);
static int  ln_transfer_putc(char ch);
static char ln_transfer_getc(void);

static int ln_transfer_write(const char *buf, size_t size)
{
    int ret = 0;
	#if (defined(AT_LOG_MERGE_TO_UART0) && (AT_LOG_MERGE_TO_UART0 == 1))
		ret = serial_write(&m_LogSerial, (const void *)buf, size);
	#else
		ret = serial_write(&g_at_serial, (const void *)buf, size);
	#endif // !AT_LOG_MERGE_TO_UART0
    
    return ret;
}

static int ln_transfer_putc(char ch)
{
    int ret = 0;
	#if (defined(AT_LOG_MERGE_TO_UART0) && (AT_LOG_MERGE_TO_UART0 == 1))
		serial_putchar(&m_LogSerial, ch);
	#else
		serial_putchar(&g_at_serial, ch);
	#endif // !AT_LOG_MERGE_TO_UART0
    return ret;
}

static char ln_transfer_getc(void)
{
    return 0;
}

static void ln_transfer_task(void *arg)
{
    ln_at_transfer_t *transfer = &g_at_transfer;
    Serial_t *port = (Serial_t *)arg;
    int len = 0;
    uint8_t ch;

    if (transfer->status == 0)
    {
        LN_AT_LOG_E("Transfer is not created!\r\n");
        return;
    }

    while(1)
    {
        if (0 == ln_at_sem_wait(transfer->sem, LN_AT_WAIT_FOREVER))
        {
            while (!fifo_isempty(&port->rxfifo))
            {
                len = serial_read(port, &ch, 1);
                if(len > 0)
                {
                    ln_at_preparser(ch);
                }
            }
        }
    }
}

static void ln_serial_rx_callbcak(void)
{
    ln_at_transfer_t *transfer = &g_at_transfer;
    ln_at_sem_release(transfer->sem);
}

int ln_transfer_init(void)
{
    ln_at_transfer_t *transfer = &g_at_transfer;
    Serial_t *fd = NULL;
    if (transfer->status == 0)
    {
        LN_AT_LOG_E("Transfer is not created!\r\n");
        return -1;
    }

    /*
     *  Init Serial.
     *  Console use SER_PORT_UART0/SER_PORT_UART1.
     */
	#if (defined(AT_LOG_MERGE_TO_UART0) && (AT_LOG_MERGE_TO_UART0 == 1))
		fd = &m_LogSerial;
		serial_init(fd, SER_PORT_UART0, CONSOLE_PORT_BAUDRATE, ln_serial_rx_callbcak);
	#else
		fd = &g_at_serial;
		serial_init(fd, SER_PORT_UART1, CONSOLE_PORT_BAUDRATE, ln_serial_rx_callbcak);
	#endif // !AT_LOG_MERGE_TO_UART0
    
    transfer->sem = ln_at_sem_create(0, 1024);
    if (transfer->sem == NULL) {
        return -1;
    }

    if(ln_at_task_create("AT_Transfer",   \
                (void *)ln_transfer_task, \
                fd,                       \
                LN_AT_TRANSFER_TASK_PRI,  \
                LN_AT_TRANSFER_TASK_STACK_SIZE) == NULL)
    {
        return -1;
    }

    serial_write(fd, "Console init ok!\r\n", strlen("Console init ok!\r\n"));

    return 0;
}

static int ln_transfer_deinit(void)
{
    memset(&g_at_transfer, 0x0, sizeof(ln_at_transfer_t));
    return 0;
}

ln_at_transfer_t *ln_transfer_create(void)
{
    ln_at_transfer_t *transfer = &g_at_transfer;
    memset(transfer, 0x0, sizeof(ln_at_transfer_t));

    transfer->init   = ln_transfer_init;
    transfer->deinit = ln_transfer_deinit;
    transfer->getc   = ln_transfer_getc;
    transfer->putc   = ln_transfer_putc;
    transfer->write  = ln_transfer_write;
    transfer->status = 1;
    return transfer;
}
