<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/uart/ -->

# 通用异步收发传输器(UART)

---

## 1、简介

UART(通用异步收发传输器)是一种通用串行数据总线，用于异步通信；可以实现全双工传输和接收。

---

## 2、时序

### 2.1、 基础简介

* UART工作原理是将数据的二进制位一位一位的进行传输。在UART通讯协议中信号线上的状态位高电平代表’1’低电平代表’0’
* 硬件连接比较简单，仅需要3条线，TX、RX、GND
* TX（发送数据端，要接对面设备的RX）
* RX（接收数据端，要接对面设备的TX）
* 空闲位（当总线处于空闲状态时信号线的状态为‘1’即高电平）
* 起始位（开始传输时要先发出一个低电平’0’来表示传输字符的开始）
* 数据位（起始位之后就是要传输的数据，数据一般都是8位。先发送最低位最后发送最高位）
* 奇偶校验位（数据位传送完成后，要进行奇偶校验）
* 停止位（数据结束标志，可以是1位，1.5位，2位的高电平）
* 波特率（传输一个比特需要的时间是（1/波特率）秒）

### 2.2、时序图分析

* （1）、传输1个字节的时序图，起始位、8位数据、停止位。

![UART波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/img/uart-1.png)

* （2）、波特率计算，找到发送1位数据的宽度，例如下图，时间是104 us

![UART波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/img/uart-2.png)

* 根据计算，1/波特率 = 104 us = 1.04e-4 s（1.04的负4次方秒） ，得出波特率约为9600

---

## 3、API列表

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

## 4、示例

1) 以下代码配置UART0轮询模式,发送和接收数据，收发完毕后进行数据对比验证；

```
#include "ci13lc_uart.h"
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
    //初始化UART0(轮询模式)，波特率115200
    UARTPollingConfig(UART0, UART_BaudRate115200);
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
#include "ci13lc_uart.h"
unsigned char buf_send[32] = {0x0};
unsigned char buf_recv[32] = {0x0};

void pad_config_for_uart(UART_TypeDef *UARTx)
{
    if (UARTx == UART0)
    {
        dpmu_set_io_reuse(PB5,SECOND_FUNCTION);
        dpmu_set_io_reuse(PB6,SECOND_FUNCTION);
        #if 1
        //如果外部有上拉电阻到5V，这样配
        dpmu_set_io_open_drain(PB5,ENABLE);      //配置引脚开漏功能，支持外部上拉5V
        dpmu_set_io_open_drain(PB6,ENABLE);      //配置引脚开漏功能，支持外部上拉5V
        dpmu_set_io_pull(PB5,DPMU_IO_PULL_DISABLE);   //关闭上拉
        dpmu_set_io_pull(PB6,DPMU_IO_PULL_DISABLE);   //关闭上拉
        #else
        //如果外部没有上拉
        dpmu_set_io_pull(PB5,DPMU_IO_PULL_UP);   //开启上拉
        dpmu_set_io_pull(PB6,DPMU_IO_PULL_UP);   //开启上拉
        #endif
    }
    else if (UARTx == UART1)
    {
        dpmu_set_io_reuse(PB7,SECOND_FUNCTION);
        dpmu_set_io_reuse(PC0,SECOND_FUNCTION);
        dpmu_set_io_pull(PC0,DPMU_IO_PULL_UP);  //RX需开启上拉
    }
    else if (UARTx == UART2)
    {
        dpmu_set_io_reuse(PB1,THIRD_FUNCTION);
        dpmu_set_io_reuse(PB2,THIRD_FUNCTION);
        dpmu_set_io_pull(PB2,DPMU_IO_PULL_UP);  //RX需开启上拉
    }
}

void main(void)
{
    int i = 0;
    pad_config_for_uart(UART0);
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
#include "ci13lc_uart.h"
extern unsigned char buf_recv[32];
int length = 0;
void UART0_IRQHandler(void)
{
    /*发送数据*/
    if (UART0->UARTMIS & (1UL << UART_TXInt))
    {
        ;
    }
    /*接受数据*/
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
#include "ci13lc_uart.h"
#include "ci13lc_dma.h"
unsigned char buf_send[2048] = {0};
unsigned char buf_recv[2048] = {0};

void pad_config_for_uart(UART_TypeDef *UARTx)
{
    if (UARTx == UART0)
    {
        dpmu_set_io_reuse(PB5,SECOND_FUNCTION);
        dpmu_set_io_reuse(PB6,SECOND_FUNCTION);
        #if UART_PAD_OPENDRAIN_MODE_EN
        dpmu_set_io_pull(PB5,DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        dpmu_set_io_pull(PB6,DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        dpmu_set_io_open_drain(PB5);
        #else
        dpmu_set_io_pull(PB6,DPMU_IO_PULL_UP);  //RX需开启上拉
        #endif
    }
    else if (UARTx == UART1)
    {
        dpmu_set_io_reuse(PB7,SECOND_FUNCTION);
        dpmu_set_io_reuse(PC0,SECOND_FUNCTION);
        #if UART_PAD_OPENDRAIN_MODE_EN
        dpmu_set_io_pull(PB7,DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        dpmu_set_io_pull(PC0,DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        dpmu_set_io_open_drain(PB7);
        #else
        dpmu_set_io_pull(PC0,DPMU_IO_PULL_UP);  //RX需开启上拉
        #endif
    }
    else if (UARTx == UART2)
    {
        dpmu_set_io_reuse(PB1,THIRD_FUNCTION);
        dpmu_set_io_reuse(PB2,THIRD_FUNCTION);
        #if UART_PAD_OPENDRAIN_MODE_EN
        dpmu_set_io_pull(PB1,DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        dpmu_set_io_pull(PB2,DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        dpmu_set_io_open_drain(PB1);
        #else
        dpmu_set_io_pull(PB2,DPMU_IO_PULL_UP);  //RX需开启上拉
        #endif
    }
}

void main(void)
{   
    scu_set_dma_mode(DMAINT_SEL_CHANNELALL);

    int i = 0;
    pad_config_for_uart(UART0);
    //初始化发送接收数据数组
    for(i = 0;i < 2048;i++)
    {
        buf_send[i] = i;
    }

    //初始化UART0(DMA模式)
    UARTDMAConfig(UART0, UART_BaudRate115200);
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
                            BURSTSIZE1,
                            DMAC_AHBMaster1);
    //DMA发送数据
    DMAC_M2P_P2M_advance_config(DMACChannel0,
                            DMAC_Peripherals_UART0_TX,
                            M2P_DMA,
                            (unsigned int)buf_send,
                            UART0_DMA_ADDR,
                            bytesize,
                            trans_width,
                            BURSTSIZE1,
                            DMAC_AHBMaster1);
    //等待DMA Channel1发送完成
    if(RETURN_ERR == wait_dma_translate_flag(DMACChannel0,0xffffff))
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
#include "ci13lc_uart.h"
#include "ci13lc_dma.h"
void DMA_IRQHandler(void)
{
    int reg = DMAC->DMACIntTCStatus;
    if (reg & (1 << DMACChannel0))
    {
        CALL_CALLBACK(g_dma_channel0_callback);
    }
    DMAC->DMACIntTCClear = reg;
}
```

---

## 5、其它

* *常用的波特率范围如下所示*

| UARTx | 波特率:Bps |
| --- | --- |
| UART0 | 2400,4800,9600,1920038400,57600,115200230400,380400,460800921600,1M,2M,3M |
| UART1 | 2400,4800,9600,1920038400,57600,115200230400,380400,460800921600,1M,2M,3M |
| UART2 | 2400,4800,9600,1920038400,57600,115200230400,380400,460800921600,1M,2M,3M |