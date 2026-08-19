<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/uart/ -->

# 通用异步收发传输器(UART)

---

## 1、简介

* UART是一种通用异步串行通信数据接口，实现两个设备之间数据的接收和发送，支持全双工通讯。接收端和发送端之间没有共享时钟信号，为保障通信可靠性，通讯的两个设备需要设置相同的波特率和数据帧格式。
* CI13XX支持3个UART控制器：UART0、UART1和UART2。

## 2、特性

* 支持标准的UART协议，数据帧格式由起始位、数据位（长度可配置）、奇偶校验位(可选)和停止位（宽度可配置）四部分组成，支持波特率可配置；
* 总线处于空闲状态时，信号线为高电平状态；
* 起始位：用于标识传输数据帧的开始，每个数据帧以发送端输出一个比特宽度的低电平开始，通知接收端数据传输已启动；
* 数据位：传输的数据长度通过API(UART\_LCRConfig)可配置为5~8位，常用8位，位传输顺序为低位优先，先发送最低位LSB最后发送最高位MSB；
* 奇偶校验位：通过API(UART\_LCRConfig)配置是否开启奇偶校验功能，通过API(UART\_LCRConfig)配置奇校验或偶校验。奇偶校验功能开启状态、数据位传输完成后，可通过奇/偶校验检测数据传输是否错误；
* 停止位：用于标识传输数据帧的结束，每个数据帧以高电平结束，长度通过API(UART\_LCRConfig)可配置为1、1.5、2位；

## 3、时序图

* UART数据传输1个数据帧（8位数据位、奇偶校验位、1bit停止位）的时序图如下：

![UART波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/uart-3.png)

* UART数据传输1个数据帧（7位数据位、奇偶校验位、1.5bit停止位）的时序图如下：

![UART波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/uart-4.png)

* UART数据传输1个数据帧（8位数据位、无奇偶校验位、2bit停止位）的时序图如下：

![UART波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/uart-5.png)

* UART波特率设置越高，数据传输速度越快，但也会增加干扰和误码率。在设置波特率时，需要考虑通信双方串口硬件是否支持该波特率。若波特率设置过高，可能会导致数据传输不稳定。
* 支持最大3Mbps波特率，波特率可通过API(UART\_BAUDRATEConfig)配置。

---

## 4、API列表

| 函数名 | 描述 |
| --- | --- |
| UARTPollingConfig | UART 轮询模式初始化配置 |
| UARTInterruptConfig | UART 中断模式初始化配置 |
| UARTDMAConfig | UART DMA模式初始化配置 |
| UART\_IntMaskConfig | UART 中断屏蔽设置 |
| UART\_IntClear | UART 中断标志清除 |
| UartPollingReceiveData | 轮询方式接收一个Byte数据 |
| UartPollingSenddata | 轮询方式发送一个Byte数据 |
| UART\_LCRConfig | 配置奇偶校验位、停止位、数据位 |
| UART\_BAUDRATEConfig | 配置波特率 |

---

## 5、示例

* 在板级配置文件里修改UART的引脚配置，例如CI-D06GT01D.c：

```
/*UART的引脚初始化接口*/
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
```

### 5.1、轮询模式

* 以下代码配置UART0轮询模式,发送和接收数据，收发完毕后进行数据对比验证；

```
#include "ci130x_uart.h"

void uart_polling_test(void)
{
    //初始化发送接收数据数组
    unsigned char buf_send[32] = {0x0};
    unsigned char buf_recv[32] = {0x0};
    for(int i = 0;i < 32;i++)          
    {
        buf_send[i] = i;
    }

    //初始化UART0(轮询模式)，波特率115200
    UARTPollingConfig(UART0, UART_BaudRate115200);

    //发送数据：每次发送一个byte
    for(int i=0;i<32;i++)
    {
        UartPollingSenddata(UART0,buf_send[i]);
    }

    //接收数据：每次接收一个byte
    for(int i=0;i<32;i++)
    {
        buf_recv[i] = UartPollingReceiveData(UART0);
    }

    //比较接收发送数据是否相等
    for(int i=0;i<32;i++)
    {
        if(buf_send[i] != buf_recv[i])
        {
            mprintf("Comparison of the Data Fail\n");
            break;
        }
    }
    else
    {
        mprintf("Comparison of the Data Successful\n");
    }
}
```

### 5.2、中断模式

* 以下代码配置UART0中断模式并使用轮询发送数据，中断接收数据，收发完毕后进行数据对比验证；

```
#include "ci130x_uart.h"
unsigned char buf_send[32] = {0x0};
unsigned char buf_recv[32] = {0x0};

void uart_interrupt_test(void)
{
    //初始化UART0(中断模式)
    UARTInterruptConfig(UART0,UART_BaudRate115200);

    //初始化发送接收数据数组
    for(int i = 0;i < 32;i++)
    {
        buf_send[i] = i;
    }

    //发送数据：每次发送一个byte
    for(int i=0;i<32;i++)
    {
        UartPollingSenddata(UART0,buf_send[i]);
    }

    //比较接收发送数据是否相等
    for(int i = 0;i < 32;i++)
    {
        if(buf_send[i] != buf_recv[i])
        {
            mprintf("Comparison of the Data Fail\n");
            break;
        }
    }
    else
    {
        mprintf("Comparison of the Data Successful\n");
    }
}
```

* UART中断服务函数的处理如下

```
#include "ci130x_uart.h"
extern unsigned char buf_recv[32];
int length = 0;

/*UART的中断服务函数*/
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

### 5.2、DMA模式

* 以下代码配置UART0 DMA模式并使用DMA模式发送和接收数据，收发完毕后进行数据对比验证；

```
#include "ci130x_uart.h"
#include "ci130x_dma.h"
unsigned char buf_send[2048] = {0};
unsigned char buf_recv[2048] = {0};

void uart_dma_test(void)
{   
    //配置DMA数据传输宽度
    TRANSFERWIDTHx trans_width = TRANSFERWIDTH_8b;
    //DMA传输数据长度(单位：Byte)
    int bytesize = 2048;

    scu_set_dma_mode(DMAINT_SEL_CHANNELALL);

    //初始化发送接收数据数组
    for(int i = 0;i < bytesize;i++)
    {
        buf_send[i] = i;
    }

    //初始化UART0(DMA模式)
    UARTDMAConfig(UART0, UART_BaudRate115200);

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
    DMAC_M2P_P2M_advance_config(DMACChannel1,
                            DMAC_Peripherals_UART0_TX,
                            M2P_DMA,
                            (unsigned int)buf_send,
                            UART0_DMA_ADDR,
                            bytesize,
                            trans_width,
                            BURSTSIZE1,
                            DMAC_AHBMaster1);

    //等待DMA Channel1发送完成
    if(RETURN_ERR == wait_dma_translate_flag(DMACChannel1,0xffffff))
    {
        mprintf("send dma irq err\n");
    }

    //等待DMA Channel0接收完成
    if(RETURN_ERR == wait_dma_translate_flag(DMACChannel0,0xffffff))
    {
        mprintf("recv dma irq err\n");
    }

    //比较接收发送数据是否相等
    for(int i = 0;i < bytesize;i++)
    {
        if(buf_send[i] != buf_recv[i])
        {
            mprintf("Comparison of the Data Fail\n");
            break;
        }
    }
    else
    {
        mprintf("Comparison of the Data Successful\n");
    }
}
```

* DMA中断服务函数的处理如下

```
#include "ci130x_uart.h"
#include "ci130x_dma.h"

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
    DMAC->DMACIntTCClear = reg;
}
```

---

## 6、其它

* *常用的波特率范围如下所示*

| UARTx | 波特率:Bps |
| --- | --- |
| UART0 | 2400,4800,9600,1920038400,57600,115200230400,380400,460800921600,1M,2M,3M |
| UART1 | 2400,4800,9600,1920038400,57600,115200230400,380400,460800921600,1M,2M,3M |
| UART2 | 2400,4800,9600,1920038400,57600,115200230400,380400,460800921600,1M,2M,3M |

## 7、API 参考