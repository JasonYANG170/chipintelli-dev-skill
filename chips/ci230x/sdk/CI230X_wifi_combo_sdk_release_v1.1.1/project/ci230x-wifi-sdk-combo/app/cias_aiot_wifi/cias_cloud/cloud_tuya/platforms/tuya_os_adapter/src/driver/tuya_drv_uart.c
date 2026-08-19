/*============================================================================
*                                                                            *
* Copyright (C) by Tuya Inc                                                  *
* All rights reserved                                                        *
*                                                                            *
*                                                                            *
=============================================================================*/

/*============================ INCLUDES ======================================*/
#include "tuya_uart.h"
#include "tuya_os_adapter_error_code.h"
#include "proj_config.h"
#include "ln_types.h"
#include "utils/debug/ln_assert.h"
#include "utils/debug/log.h"

#include "hal/hal_uart.h"
#include "hal/hal_gpio.h"
// #include "hal/hal_sleep.h"

#include "serial_hw.h"
#include "serial.h"


#define UART_DEV_NUM            2
#define UART_RX_FIFO_LEN        128

/**
 * @brief UART_HANDLE_GET
 * @param uart: TY_UART_PORT_E
 */
#define UART_HANDLE_GET(uart) ((uart <= TUYA_UART1) ? &s_uart_dev[uart] : NULL)


typedef enum {
    TY_HAL_UART_CLOSED = 0,
    TY_HAL_UART_OPENED,
}TY_HAL_UART_STATUS_T;

typedef enum {
    UART_FIFO_EMPTY = 0,
    UART_FIFO_FULL,
    UART_FIFO_OTHER,
}UART_FIFO_STATUS_T;

typedef struct {
    tuya_uart_t                 dev;
    uint32_t                    uart_base;
    tuya_uart_isr_cb            isr_cb;
    TY_HAL_UART_STATUS_T        status;
} uart_dev_t;

typedef struct {
    int                 head;
    int                 tail;
    UART_FIFO_STATUS_T  status;
    unsigned char       buff[UART_RX_FIFO_LEN];
} uart_rx_fifo_t;

/*============================ PROTOTYPES ====================================*/
static int uart_dev_init        (tuya_uart_t *uart, tuya_uart_cfg_t *cfg);
static int uart_dev_write_byte  (tuya_uart_t *uart, uint8_t byte);
static int uart_dev_read_byte   (tuya_uart_t *uart, uint8_t *byte);
static int uart_dev_control     (tuya_uart_t *uart, uint8_t cmd, void *arg);
static int uart_dev_deinit      (tuya_uart_t *uart);


/***********************************************************
*************************variable define********************
***********************************************************/
static uart_dev_t s_uart_dev[UART_DEV_NUM]     = {0}; // tuya_uart_port_t as array index
static Serial_t   s_serial_array[UART_DEV_NUM] = {0}; // tuya_uart_port_t as array index

static const tuya_uart_ops_t  uart_dev_ops = {
    .init       = uart_dev_init,
    .read_byte  = uart_dev_read_byte,
    .write_byte = uart_dev_write_byte,
    .control    = uart_dev_control,
    .deinit     = uart_dev_deinit,
};

static uart_rx_fifo_t s_uart_rx_fifo;

/***********************************************************
*************************function define********************
***********************************************************/

static void serial0_rx_default_cb(void)
{
    size_t len = 0;
    unsigned char ch = 0;
    uart_rx_fifo_t *rx_fifo = &s_uart_rx_fifo;
    Serial_t *port = &s_serial_array[TUYA_UART0];
    uart_dev_t *pdrv = UART_HANDLE_GET(TUYA_UART0);
    // LOG(LOG_LVL_INFO, "[%s, %d]\r\n", __func__, __LINE__);

    if(rx_fifo->status == UART_FIFO_FULL){
        LOG(LOG_LVL_INFO, "[%s, %d]UART_FIFO_FULL\r\n", __func__, __LINE__);
        goto out;
    }

    while (!fifo_isempty(&port->rxfifo))
    {
        len = serial_read(port, &ch, 1);
        LOG(LOG_LVL_INFO, "%02x ", ch);
        if(len > 0)
        {
            rx_fifo->buff[rx_fifo->tail] = ch;
            rx_fifo->tail = (rx_fifo->tail + 1) % UART_RX_FIFO_LEN;
            if(rx_fifo->status == UART_FIFO_EMPTY){
                rx_fifo->status = UART_FIFO_OTHER;
            }else if(rx_fifo->status == UART_FIFO_OTHER && rx_fifo->tail == rx_fifo->head){
                rx_fifo->status = UART_FIFO_FULL;
            }
        }
    }

out:
    if(pdrv->isr_cb){
        pdrv->isr_cb(&pdrv->dev, TUYA_UART_INT_RX_EVENT);
    }
}

static void serial1_rx_default_cb(void)
{
    size_t len = 0;
    unsigned char ch = 0;
    uart_rx_fifo_t *rx_fifo = &s_uart_rx_fifo;
    Serial_t *port = &s_serial_array[TUYA_UART1];
    uart_dev_t *pdrv = UART_HANDLE_GET(TUYA_UART1);
    // LOG(LOG_LVL_INFO, "[%s, %d]\r\n", __func__, __LINE__);

    if(rx_fifo->status == UART_FIFO_FULL){
        LOG(LOG_LVL_INFO, "[%s, %d]UART_FIFO_FULL\r\n", __func__, __LINE__);
        goto out;
    }

    while (!fifo_isempty(&port->rxfifo))
    {
        len = serial_read(port, &ch, 1);
        LOG(LOG_LVL_INFO, "%02x ", ch);
        if(len > 0)
        {
            rx_fifo->buff[rx_fifo->tail] = ch;
            rx_fifo->tail = (rx_fifo->tail + 1) % UART_RX_FIFO_LEN;
            if(rx_fifo->status == UART_FIFO_EMPTY){
                rx_fifo->status = UART_FIFO_OTHER;
            }else if(rx_fifo->status == UART_FIFO_OTHER && rx_fifo->tail == rx_fifo->head){
                rx_fifo->status = UART_FIFO_FULL;
            }
        }
    }

out:
    if(pdrv->isr_cb){
        pdrv->isr_cb(&pdrv->dev, TUYA_UART_INT_RX_EVENT);
    }
}

/**
 * @brief 
 * 
 * int   (*rx_notify)  (tuya_uart_t *uart, uint16_t size);
 * int   (*tx_finish)  (tuya_uart_t *uart, void *buffer);
 * 
 * @param uart 
 * @param cfg 
 * @return int 
 */
int uart_dev_init(tuya_uart_t *uart, tuya_uart_cfg_t *cfg)
{
    uart_dev_t *pdrv = NULL;
    IRQn_Type uart_irq_num;

    if(uart == NULL || (uart->node.port > TUYA_UART1) || cfg == NULL) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    LOG(LOG_LVL_INFO, "[%s, %d]#### port=%d, baudrate=%d, rx cb:%08x\r\n",
            __func__, __LINE__, uart->node.port, cfg->baudrate, uart->cb.rx_notify);

    pdrv = UART_HANDLE_GET(uart->node.port);
    if (NULL == pdrv)
    {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (pdrv->status == TY_HAL_UART_CLOSED) {
        LOG(LOG_LVL_ERROR, "[%s:%d]\r\n", __func__, __LINE__);
        if(uart->node.port == TUYA_UART0) {
            pdrv->uart_base = UART1_BASE;
            uart_irq_num = UART1_IRQn;
        }else if(uart->node.port == TUYA_UART1) {
            pdrv->uart_base = UART0_BASE;
            uart_irq_num = UART0_IRQn;
        }

        NVIC_DisableIRQ(uart_irq_num);
        hal_uart_it_disable(pdrv->uart_base, USART_IT_TXE);
        hal_uart_it_disable(pdrv->uart_base, USART_IT_RXNE);

        if (uart->node.port == TUYA_UART0) {
            serial_init(&s_serial_array[TUYA_UART0], SER_PORT_UART1, (uint32_t)cfg->baudrate, serial0_rx_default_cb);
        } else if (uart->node.port == TUYA_UART1) {
            serial_init(&s_serial_array[TUYA_UART1], SER_PORT_UART0, (uint32_t)cfg->baudrate, serial1_rx_default_cb);
        }

        LOG(LOG_LVL_ERROR, "[%s:%d]\r\n", __func__, __LINE__);

        pdrv->status = TY_HAL_UART_OPENED;
        return OPRT_OS_ADAPTER_OK;
    }

    return OPRT_OS_ADAPTER_UART_INIT_FAILED;

}

int uart_dev_deinit(tuya_uart_t *uart)
{
    uart_dev_t *pdrv = NULL;
    LOG(LOG_LVL_INFO, "[%s, %d]\r\n", __func__, __LINE__);
    if(uart == NULL) {
        return  OPRT_OS_ADAPTER_INVALID_PARM;
    }

    pdrv = UART_HANDLE_GET(uart->node.port);
    if (NULL == pdrv)
    {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    LOG(LOG_LVL_INFO, "[%s, %d]#### \r\n", __func__, __LINE__);

    if (pdrv->status == TY_HAL_UART_OPENED) {
        serial_deinit(&s_serial_array[uart->node.port]);
        pdrv->status = TY_HAL_UART_CLOSED;
    }
    return OPRT_OS_ADAPTER_OK;
}

int uart_dev_write_byte(tuya_uart_t *uart, uint8_t byte)
{
    uart_dev_t *pdrv = NULL;
    // LOG(LOG_LVL_INFO, "[%s, %d]\r\n", __func__, __LINE__);
    if(uart == NULL) {
        return  OPRT_OS_ADAPTER_INVALID_PARM;
    }

    pdrv = UART_HANDLE_GET(uart->node.port);
    if (NULL == pdrv)
    {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (pdrv->status == TY_HAL_UART_OPENED) {
        serial_putchar(&s_serial_array[uart->node.port], (int)byte);
        return OPRT_OS_ADAPTER_OK;
    }
    return OPRT_OS_ADAPTER_UART_SEND_FAILED;
}

int uart_dev_read_byte(tuya_uart_t *uart, uint8_t *byte)
{
    uart_dev_t *pdrv = NULL;
    uart_rx_fifo_t *rx_fifo = &s_uart_rx_fifo;

    if(uart == NULL || byte == NULL) {
        LOG(LOG_LVL_INFO, "[%s, %d]\r\n", __func__, __LINE__);
        return  OPRT_OS_ADAPTER_INVALID_PARM;
    }

    pdrv = UART_HANDLE_GET(uart->node.port);
    if (NULL == pdrv)
    {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (pdrv->status == TY_HAL_UART_OPENED) {
        if (rx_fifo->status == UART_FIFO_EMPTY) {
            return OPRT_OS_ADAPTER_UART_READ_FAILED;
        }

        *byte = rx_fifo->buff[rx_fifo->head];
        rx_fifo->head = (rx_fifo->head + 1) % UART_RX_FIFO_LEN;

        if (rx_fifo->status == UART_FIFO_FULL) {
            rx_fifo->status = UART_FIFO_OTHER;
        } else if(rx_fifo->status == UART_FIFO_OTHER && rx_fifo->tail == rx_fifo->head) {
            rx_fifo->status = UART_FIFO_EMPTY;
        }

        return OPRT_OS_ADAPTER_OK;
    }

    return OPRT_OS_ADAPTER_UART_READ_FAILED;
}

int uart_dev_control(tuya_uart_t *uart, uint8_t cmd, void *arg)
{
    int result = OPRT_OK;
    uart_dev_t *pdrv = NULL;
    uint8_t *isr_mode = NULL;

    LOG(LOG_LVL_INFO, "[%s, %d]\r\n", __func__, __LINE__);
    if(uart == NULL) {
        return  OPRT_OS_ADAPTER_INVALID_PARM;
    }

    pdrv = UART_HANDLE_GET(uart->node.port);
    if (NULL == pdrv)
    {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    LOG(LOG_LVL_INFO, "[%s, %d]#### cmd=%d, port=%d, cmd:%d\r\n", __func__, __LINE__, cmd, uart->node.port, cmd);

    switch (cmd) {
        case TUYA_DRV_SET_INT_CMD:
            break;
        case TUYA_DRV_CLR_INT_CMD:
            break;
        case TUYA_DRV_SET_ISR_CMD:
            pdrv->isr_cb = (tuya_uart_isr_cb)arg;
            break;
        case TUYA_DRV_ISR_MODE_CMD:
            isr_mode = (uint8_t *)arg;
            *isr_mode = TUYA_UART_RX_ISR_FIFO_MODE;
            break;
    }

    return result;
}

int platform_uart_init(void)
{
#define TY_SQ_UART_NUM    TUYA_UART0 // For tuya "ShaoLuShouQuan"

    int ret = 0;
    uart_dev_t *pdrv = UART_HANDLE_GET(TY_SQ_UART_NUM);
    if (pdrv == NULL)
    {
        LOG(LOG_LVL_ERROR, "[%s:%d]\r\n", __func__, __LINE__);
        return -1;
    }

    memset(&pdrv->dev, 0x0, sizeof(tuya_uart_t));
    pdrv->dev.ops = &uart_dev_ops;

    TUYA_UART_8N1_CFG(&(pdrv->dev), TUYA_UART_BAUDRATE_9600, 256, 0);

    ret = tuya_driver_register(&pdrv->dev.node, TUYA_DRV_UART, TY_SQ_UART_NUM);
    LOG(LOG_LVL_INFO, "[%s, %d] port:%d; type:%d; ret:%d\r\n", 
            __func__, __LINE__, pdrv->dev.node.port, pdrv->dev.node.type, ret);

    {
        void *node = tuya_driver_find(TUYA_DRV_UART, TY_SQ_UART_NUM);
        LOG(LOG_LVL_INFO, "[%s, %d] find drv:%p\r\n", 
                __func__, __LINE__, node);
    }

    return 0;
}
