<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/driver/ci110x_driver/uart/ -->

# 通用异步收发传输器(UART)

---

## 简介

UART(通用异步收发传输器)是一种通用串行数据总线，用于异步通信；可以实现全双工传输和接收。

---

## API列表

| 函数名 | 描述 |
| --- | --- |
| UARTPollingConfig | UART 轮询模式初始化配置 |
| UARTInterruptConfig | UART 中断模式初始化配置 |
| UARTDMAConfig | UART DMA模式初始化配置 |
| UART\_IntMaskConfig | UART 中断屏蔽设置 |
| UART\_IntClear | UART 中断标志清除 |
| UartPollingReceiveData | 轮询方式接收一个Byte数据 |
| UartPollingSenddata | 轮询方式发送一个Byte数据 |

---

## 示例

1) 以下代码配置UART0轮询模式,发送和接收数据，收发完毕后进行数据对比验证；

```
#include "ci110x_uart.h"
void main(void)
{
    //初始化发送接收数据数组
    int i;
    unsigned char buf_send[32] = {0x0};
    unsigned char buf_recv[32] = {0x0};
    for(i = 0;i < 32;i++)
    {
        buf_send[i] = i;
    }
    //初始化UART0(轮询模式)
    UARTPollingConfig(UART0);
    //发送数据：每次发送一个byte
    for(i=0;i<32;i++)
    {
        UartPollingSenddata(UART0,buf_send[i]);
    }
    //接收数据：每次接收一个byte
    for(i=0;i<32;i++)
    {
        buf_recv[i] = UartPollingReceiveData(UART0);
    }
    //比较接收发送数据是否相等
    for(i = 0;i < 32;i++)
    {
        if(buf_send[i] != buf_recv[i])
        {
            mprintf("Comparison of the Data Fail\n");
            while(1);
        }
    }
    mprintf("Comparison of the Data Successful\n");
    while(1);
}
```

2) 以下代码配置UART0中断模式并使用轮询发送数据，中断接收数据，收发完毕后进行数据对比验证；

```
#include "ci110x_uart.h"
unsigned char buf_send[32] = {0x0};
unsigned char buf_recv[32] = {0x0};
void main(void)
{
    int i = 0;
    //初始化UART0(中断模式)
    UARTInterruptConfig(UART0,UART_BaudRate115200);
    //初始化发送接收数据数组
    for(i = 0;i < 32;i++)
    {
        buf_send[i] = i;
    }
    //发送数据：每次发送一个byte
    for(i=0;i<32;i++)
    {
        UartPollingSenddata(UART0,buf_send[i]);
    }
    //比较接收发送数据是否相等
    for(i = 0;i < 32;i++)
    {
        if(buf_send[i] != buf_recv[i])
        {
            mprintf("Comparison of the Data Fail\n");
            while(1);
        }
    }
    mprintf("Comparison of the Data Successful\n");

    while(1);
}
```

```
#include "ci110x_uart.h"
extern unsigned char buf_recv[32];
int length = 0;
void UART0_IRQHandler(void)
{
    /*发送数据*/
    if (UART0->UARTMIS & (1UL << UART_TXInt))
    {
        ;
    }
    /*接收数据*/
    if (UART0->UARTMIS & (1UL << UART_RXInt))
    {
        //here FIFO DATA must be read out
        buf_recv[length] = UartPollingReceiveData(UART0);
        length ++;
    }
    UART_IntClear(UART0,UART_AllInt);
}
```

3) 以下代码配置UART0 DMA模式并使用DMA模式发送和接收数据，收发完毕后进行数据对比验证；

```
#include "ci110x_uart.h"
#include "ci110x_dma.h"
unsigned char buf_send[2048] = {0};
unsigned char buf_recv[2048] = {0};

void main(void)
{
    int i = 0;
    //初始化发送接收数据数组
    for(i = 0;i < 2048;i++)
    {
        buf_send[i] = i;
    }

    //初始化UART0(DMA模式)
    UARTDMAConfig(UART0);
    //配置DMA数据传输宽度
    TRANSFERWIDTHx trans_width = TRANSFERWIDTH_8b;
    //DMA传输数据长度(单位：Byte)
    int bytesize = 2048;
    //DMA接收数据
    DMAC_M2P_P2M_advance_config(DMACChannel0,
                            DMAC_Peripherals_UART0_RX,
                            P2M_DMA,
                            UART0_DMA_ADDR,
                            (unsigned int)buf_recv,
                            bytesize,
                            trans_width,
                            BURSTSIZE1);
    //DMA发送数据
    DMAC_M2P_P2M_advance_config(DMACChannel1,
                            DMAC_Peripherals_UART0_TX,
                            M2P_DMA,
                            (unsigned int)buf_send,
                            UART0_DMA_ADDR,
                            bytesize,
                            trans_width,
                            BURSTSIZE1);
    //等待DMA Channel1发送完成
    if(RETURN_ERR == wait_dma_translate_flag(DMACChannel1,0xffffff))
    {
        mprintf("send dma irq err\n");
        while(1);
    }
    //等待DMA Channel0接收完成
    if(RETURN_ERR == wait_dma_translate_flag(DMACChannel0,0xffffff))
    {
        mprintf("recv dma irq err\n");
        while(1);
    }

    //比较接收发送数据是否相等
    for(i = 0;i < 2048;i++)
    {
        if(buf_send[i] != buf_recv[i])
        {
            mprintf("Comparison of the Data Fail\n");
            while(1);
        }
    }
    mprintf("Comparison of the Data Successful\n");

    while(1);
}
```

```
#include "ci110x_uart.h"
#include "ci110x_dma.h"
void DMA_IRQHandler(void)
{
    int reg = DMAC->DMACIntTCStatus;
    if (reg & (1 << DMACChannel0))
    {
        CALL_CALLBACK(g_dma_channel0_callback);
    }
    if (reg & (1 << DMACChannel1))
    {
        CALL_CALLBACK(g_dma_channel1_callback);
    }
    if (reg & (1 << DMACChannel2))
    {
        CALL_CALLBACK(g_dma_channel2_callback);
    }
    DMAC->DMACIntTCClear = reg;
}
```

---

## 常见问题

* 波特率范围

| UARTx | 波特率:Bps |
| --- | --- |
| UART0 | 2400,4800,9600,1920038400,57600,115200230400,380400,460800921600,1M,2M,3M |
| UART1 | 2400,4800,9600,1920038400,57600,115200230400,380400,460800921600,1M |
| UART2 | 2400,4800,9600,1920038400,57600,115200230400,380400,460800921600,1M,2M,3M |