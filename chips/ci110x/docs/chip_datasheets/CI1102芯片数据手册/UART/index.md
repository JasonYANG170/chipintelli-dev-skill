<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1102%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/UART/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1102芯片数据手册.pdf)

# UART

## 功能介绍

CI1102有三路标准UART：UART0 - UART2。UART模块的主要特征如下：

* 独立的发送FIFO和接收FIFO
* 波特率可编程，支持DMA接口
* 支持标准的UART协议
* 开始bit错误检测
* 支持奇偶校验
* 数据帧可以配置为5，6，7，8bits
* stop位可配置为1bit，1.5bit，2bit
* 支持Timeout中断机制，且Timeout大小可配置
* FIFO大小为64 \* 8 bit，支持FIFO上溢出下溢出错误检测
* 支持FIFO空满中断和传输错误中断
* 最高可达3M波特率

## 寄存器映射

UART0 / 1 / 2寄存器映射的基地址分别为0x40049000，0x4004a000，0x4004b000，详细的寄存器映射见表18。

表18 UART0 / 1 / 2寄存器映射

| 偏移量 | 名称 | 位宽 | 类型 | 复位值 | 描述 |
| --- | --- | --- | --- | --- | --- |
| 0x00 | UART\_RdD\_R | 32 | RO | 0x00000000 | 读数据寄存器 |
| 0x04 | UART\_WrD\_R | 32 | WO | 0x00000000 | 写数据寄存器 |
| 0x08 | UART\_Rx\_Er\_R | 32 | R/W | 0x00000000 | 接收错误标志寄存器 |
| 0x0C | UART\_Flag\_R | 32 | RO | 0x0000034F | 标志寄存器 |
| 0x10 | UART\_I\_BRD | 32 | R/W | 0x00000000 | 波特率分频计数器整数部分寄存器 |
| 0x14 | UART\_F\_BRD | 32 | R/W | 0x00000000 | 波特率分频计数器小数部分寄存器 |
| 0x18 | UART\_LCR | 32 | RO | 0x00000000 | 线性控制寄存器 |
| 0x1C | UART\_CR | 32 | RO | 0x00000300 | 控制寄存器 |
| 0x20 | UART\_FLS | 32 | R/W | 0x00000012 | FIFO触发深度配置寄存器 |
| 0x24 | UART\_Mask\_Int | 32 | R/W | 0x00000FFF | 中断屏蔽寄存器 |
| 0x28 | UART\_RIS | 32 | RO | 0x00000020 | 原始的中断状态寄存器 |
| 0x2C | UART\_MIS | 32 | RO | 0x00000000 | 屏蔽后的中断状态寄存器 |
| 0x30 | UART\_ICR | 32 | WO | 0x00000000 | 中断清零寄存器 |
| 0x34 | UART\_DMA\_CR | 32 | R/W | 0x00000000 | DMA控制寄存器 |
| 0x38 | UART\_TIMEOUT\_R | 32 | R/W | 0x00000020 | 接收时延寄存器 |
| 0x50 | UART\_DMA\_BYTE\_EN | 32 | R/W | 0x00000000 | DMA模式支持byte传输 |

## 读数据寄存器（UART\_RdD\_R）

偏移量：0x00

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:8 | Reserved | 0x00000000 | R | 保留 |
| 7:0 | DATA | 0x00 | R | 读数据 |

## 写数据寄存器（UART\_WrD\_R）

偏移量：0x04

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:0 | WDATA | 0x00000000 | W | 32位写数据 |

## 接收错误标志寄存器（UART\_Rx\_Er\_R）

偏移量：0x08

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:4 | Reserved | 0x00000000 | R/W | 保留 |
| 3 | OE | 0 | R/W | Overrun错误标志 |
| 2 | BE | 0 | R/W | Break错误标志 |
| 1 | PE | 0 | R/W | 奇偶校验错误标志 |
| 0 | FE | 0 | R/W | 传输Frame错误标志 |

## 标志寄存器（UART\_Flag\_R）

偏移量：0x0C

复位值：0x0000034F

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:11 | Reserved | 0x00000 | R | 保留 |
| 10 | Error Data Flag(EDF) | 0 | R | 为1时表示当前FIFO中错误数据还没有被读出，CPU应继续出RXFIFO中的数据 |
| 9 | End of current trans(EOC) | 1 | R | 完成当前传输的标志信号 |
| 8 | Transmit FIFO Empty (TXFE) | 1 | R | 发送FIFO空标志位 |
| 7 | Transmit FIFO Full(TXFF) | 0 | R | 发送FIFO满标志位 |
| 6 | Receive FIFO Empty(RXFE) | 1 | R | 接收FIFO空标志位 |
| 5 | Receive FIFO Full(RXFF) | 0 | R | 接收FIFO满标志位 |
| 4 | UART Busy(BUSY) | 0 | R | UART忙标志，当TXFIFO不空时该信号为1 |
| 3:1 | Reversed | - | R | 保留 |
| 0 | Clear To Send(CTS) | 1 | R | 当外部modem的CTS信号有效时，该bit位为1 |

## 波特率分频计数器整数部分寄存器（UART\_I\_BRD）

偏移量：0x10

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:0 | Baud Rate Integer | 0x00000000 | R/W | 波特率分频寄存器整数部分 |

## 波特率分频计数器小数部分寄存器（UART\_F\_BRD）

偏移量：0x14

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:6 | Reserved | 0x0000000 | R/W | 保留 |
| 5:0 | Baud Rate Integer | 0x00 | R/W | 波特率分频寄存器小数部分 |

## 线性控制寄存器（UART\_LCR）

偏移量：0x18

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:6 | Reserved | 0x000000 | R/W | 保留 |
| 8 | Byte\_select(BS) | 0 | R/W | 向给bit写1，表示APB和AHB以byte方式向TXFIFO发送数据。为0时表示以word的方式向TXFIFO发送数据。 |
| 7 | Stick Parity Select (SPS) | 0 | R/W | 固定奇偶校验位 |
| 6:5 | Word length [1:0] (WLEN) | 0x0 | R/W | 每帧中有效数据的个数 00=5bit 01=6bit 10=7bit  11=8bit |
| 4 | FIFOs Clear (FIFO\_CLR) | 0 | R/W | 向该bit写1，FIFO将清零。 |
| 3:2 | Two Stop Bits Select (STP) | 0x0 | R/W | 停止位的个数 00=1bit 01=1.5bit 10=2bit 11=reserved |
| 1 | Even Parity Select (EPS) | 0 | R/W | 偶校验选择，为1时为偶校验，为0时为奇校验 |
| 0 | Parity Enable (PEN) | 0 | R/W | 奇偶检验enable信号 |

## 控制寄存器（UART\_CR）

偏移量：0x1C

复位值：0x00000300

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:16 | Reserved | 0x0000 | R/W | 保留 |
| 15 | CTS Hardware Flow Control Enable (CTSEn) | 0 | R/W | 该bit位写1，由硬件判断CTS信号：采样到CTS有效则继续向外发送数据。 |
| 14 | RTS Hardware Flow Control Enable (RTSEn) | 0 | R/W | 该bit位写1，由硬件产生RTS信号，当RXFIFO没有达到域值时RTS信号就一直有效，请求外部继续发送数据。 |
| 13 | Out2 | 0 | R/W | 当该bit写1时，在输出端口nUARTOUT2上输出0。在用作modem时，该端口可作为响铃信号RI |
| 12 | Out1 | 0 | R/W | 当该bit写1时，在输出端口nUARTOUT1上输出0。在用作modem时，该端口可作为数据载波检测信号DCD |
| 11 | Request to Send (RTS) | 0 | R/W | 该位是UART请求发送（nUARTRTS）调制解调器状态输出的补充。当该位编程为1时，输出为0 |
| 10 | Data Transmit Ready (DTR) | 0 | R/W | 该位是UART数据传输就绪（nUARTDTR）调制解调器状态输出的补充。当该位编程为1时，输出为0 |
| 9 | Receive Enable (RXE) | 1 | R/W | 当该bit位写1，表示允许接收，如果在一帧传输的中间disable，要先完成当前的传输然后再停止接收 |
| 8 | Transmit Enable (TXE) | 1 | R/W | 当该bit位写1，表示允许发送，如果在一帧传输的中间disable，要先完成当前的传输然后再停止发送 |
| 7:2 | Reversed | 0 | R/W | 保留 |
| 1 | Don’t care error data(NCED) | 0 | R/W | 该bit位写1时，不管RXFIFO是否有错误数据（奇偶校验错误、帧错误、break错误和overrun错误），只要RXFIFO达到域值就发送DMA请求或CPU接收中断。 |
| 0 | UART Enable(UARTEN) | 0 | R/W | UART enable信号。当该bit写1时，表示enabled。如果在一次传输的中间disable UART，要等当前传输完成后，UART才停止工作。 |

## FIFO触发深度配置寄存器（UART\_FLS）

偏移量：0x20

复位值：0x00000012

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:6 | Reserved | 0x000000 | R/W | 保留 |
| 5:3 | Receive Interrupt FIFO Level Select (RXIFLSEL) | 0x2 | R/W | 接收FIFO触发深度选择 000 = Receive FIFO becomes >= ⅛ full 001 = Receive FIFO becomes >= ¼ full 010 = Receive FIFO becomes >= ½ full 011 = Receive FIFO becomes >= ¾ full 100 = Receive FIFO becomes >= ⅞ full 101 = Receive FIFO 中只要有>=1个byte数据就触发 110 = Receive FIFO 中只要有>=2个byte数据就触发 111 = reserved. |
| 2:0 | Transmit Interrupt FIFO Level Select (TXIFLSEL) | 0x2 | R/W | 发送FIFO触发深度选择 000 = Transmit FIFO becomes < ⅛ full(有大于⅞的空间为空) 001 = Transmit FIFO becomes < ¼ full(有大于¾的空间为空) 010 = Transmit FIFO becomes < ½ full(有大于½的空间为空) 011 = Transmit FIFO becomes < ¾ full(有大于¼的空间为空) 100 = Transmit FIFO becomes < ⅞ full(有大于⅛的空间为空) 101:111 = reserved. |

## 中断屏蔽寄存器（UART\_Mask\_Int）

偏移量：0x24

复位值：0x00000FFF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:12 | Reserved | 0x00000 | R/W | 保留 |
| 11 | Error data interrupt Mask (EDIM) | 1 | R/W | 该bit写1表示屏蔽Error Data Interrupt |
| 10 | Overrun Error Interrupt Mask (OEIM) | 1 | R/W | 该bit写1表示屏蔽Overrun Error Interrupt |
| 9 | Break Error Interrupt Mask (BEIM) | 1 | R/W | 该bit写1表示屏蔽Break Error Interrupt |
| 8 | Parity Error Interrupt Mask (PEIM) | 1 | R/W | 该bit写1表示屏蔽Parity Error Interrupt |
| 7 | Framing Error Interrupt Mask (FEIM) | 1 | R/W | 该bit写1表示屏蔽Framing Error Interrupt |
| 6 | Receive Timeout Interrupt Mask (RTIM) | 1 | R/W | 该bit写1表示屏蔽Receive Timeout Interrupt |
| 5 | Transmit Interrupt Mask (TXIM) | 1 | R/W | 该bit写1表示屏蔽Transmit Interrupt |
| 4 | Receive Interrupt Mask (RXIM) | 1 | R/W | 该bit写1表示屏蔽Receive Interrupt |
| 3:2 | Reversed | 0x3 | R/W | 保留 |
| 1 | nUARTCTS Modem Interrupt Mask (CTSMIM) | 1 | R/W | 该bit写1表示屏蔽nUARTCTS Modem Interrupt |
| 0 | nUARTRI Modem Interrupt Mask (RIMIM) | 1 | R/W | 该bit写1表示屏蔽nUARTRI Modem Interrupt |

## 原始的中断状态寄存器（UART\_RIS）

偏移量：0x28

复位值：0x00000020

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:12 | Reserved | 0x00000 | R/W | 保留 |
| 11 | Error data interrupt Status(EDRIS) | 0 | R/W | 原始的Error Data Interrupt状态 |
| 10 | Overrun Error Interrupt Status (OERIS) | 0 | R/W | 原始的Overrun Error Interrupt状态 |
| 9 | Break Error Interrupt Status (BERIS) | 0 | R/W | 原始的Break Error Interrupt状态 |
| 8 | Parity Error Interrupt Status (PERIS) | 0 | R/W | 原始的Parity Error Interrupt状态 |
| 7 | Framing Error Interrupt Status (FERIS) | 0 | R/W | 原始的Framing Error Interrupt状态 |
| 6 | Receive Timeout Interrupt Status (RTRIS) | 0 | R/W | 原始的Receive Timeout Interrupt状态 |
| 5 | Transmit Interrupt Status (TXRIS) | 1 | R/W | 原始的Transmit Interrupt状态 |
| 4 | Receive Interrupt Status (RXRIS) | 0 | R/W | 原始的Receive Interrupt状态 |
| 3 | nUARTDSR Modem Interrupt Status (DSRRMIS) | 0 | R/W | 原始的nUARTDSR Modem Interrupt状态 |
| 2 | nUARTDCD Modem Interrupt Status (DCDRMIS) | 0 | R/W | 原始的nUARTDCD Modem Interrupt状态 |
| 1 | nUARTCTS Modem Interrupt Status (CTSRMIS) | 0 | R/W | 原始的nUARTCTS Modem Interrupt状态 |
| 0 | nUARTRI Modem Interrupt Status (RIRMIS) | 0 | R/W | 原始的nUARTRI Modem Interrupt状态 |

## 屏蔽后的中断状态寄存器（UART\_MIS）

偏移量：0x2C

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:12 | Reserved | 0x00000 | R | 保留 |
| 11 | Error data Masked interrupt Status（EDMIS） | 0 | R | 屏蔽后的Error Data Interrupt状态 |
| 10 | Overrun Error Masked Interrupt Status (OEMIS) | 0 | R | 屏蔽后的Overrun Error Interrupt状态 |
| 9 | Break Error Masked Interrupt Status (BEMIS) | 0 | R | 屏蔽后的Break Error Interrupt状态 |
| 8 | Parity Error Masked Interrupt Status (PEMIS) | 0 | R | 屏蔽后的Parity Error Interrupt状态 |
| 7 | Framing Error Masked Interrupt Status (FEMIS) | 0 | R | 屏蔽后的Framing Error Interrupt状态 |
| 6 | Receive Timeout Masked Interrupt Status (RTMIS) | 0 | R | 屏蔽后的Receive Timeout Interrupt状态 |
| 5 | Transmit Masked Interrupt Status (TXMIS) | 0 | R | 屏蔽后的Transmit Interrupt状态 |
| 4 | Receive Masked Interrupt Status (RXMIS) | 0 | R | 屏蔽后的Receive Interrupt状态 |
| 3 | nUARTDSR Modem Masked Interrupt Status (DSRMMIS) | 0 | R | 屏蔽后的nUARTDSR Modem Interrupt状态 |
| 2 | nUARTDCD Modem Masked Interrupt Status (DCDMMIS) | 0 | R | 屏蔽后的nUARTDCD Modem Interrupt状态 |
| 1 | nUARTCTS Modem Masked Interrupt Status (CTSMMIS) | 0 | R | 屏蔽后的nUARTCTS Modem Interrupt状态 |
| 0 | nUARTRI Modem Masked Interrupt Status (RIMMIS) | 0 | R | 屏蔽后的nUARTRI Modem Interrupt状态 |

## 中断清零寄存器（UART\_ICR）

偏移量：0x30

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:12 | Reserved | 0x00000 | W | 保留 |
| 11 | Error data interrupt Clear（EDIC） | 0 | W | 向该bit写1清除Error Data Interrupt |
| 10 | Overrun Error Interrupt Clear (OEIC) | 0 | W | 向该bit写1清除Overrun Error Interrupt |
| 9 | Break Error Interrupt Clear (BEIC) | 0 | W | 向该bit写1清除Break Error Interrupt |
| 8 | Parity Error Interrupt Clear (PEIC) | 0 | W | 向该bit写1清除Parity Error Interrupt |
| 7 | Framing Error Interrupt Clear (FEIC) | 0 | W | 向该bit写1清除Framing Error Interrupt |
| 6 | Receive Timeout Interrupt Clear (RTIC) | 0 | W | 向该bit写1清除Receive Timeout Interrupt |
| 5 | Transmit Interrupt Clear (TXIC) | 0 | W | 向该bit写1清除Transmit Interrupt |
| 4 | Receive Interrupt Clear (RXIC) | 0 | W | 向该bit写1清除Receive Interrupt |
| 3 | nUARTDSR Modem Interrupt Clear (DSRMIC) | 0 | W | 向该bit写1清除nUARTDSR Modem Interrupt |
| 2 | nUARTDCD Modem Interrupt Clear (DCDMIC) | 0 | W | 向该bit写1清除nUARTDCD Modem Interrupt |
| 1 | nUARTCTS Modem Interrupt Clear (CTSMIC) | 0 | W | 向该bit写1清除nUARTCTS Modem Interrupt |
| 0 | nUARTRI Modem Interrupt Clear (RIMIC) | 0 | W | 向该bit写1清除nUARTRI Modem Interrupt |

## DMA控制寄存器（UART\_DMA\_CR）

偏移量：0x34

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:12 | Reserved | 0x00000000 | R/W | 保留 |
| 1 | Transmit DMA Enable (TXDMAE) | 0 | R/W | 如果此位设置为1，则启用传输FIFO的DMA请求 |
| 0 | Receive DMA Enable (RXDMAE) | 0 | R/W | 如果此位设置为1，则启用接收FIFO的DMA请求 |

## 接收时延寄存器（UART\_timeout\_R）

偏移量：0x38

复位值：0x00000020

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:10 | Reserved | 0x000000 | R/W | 保留 |
| 9:0 | Timeout size (TS) | 0x020 | R/W | Timeout延时的大小寄存器，默认为32个波特bit的大小，最大支持1023个波特bit的大小 |

## DMA传输模式控制寄存器（UART\_DMA\_BYTE\_EN）

偏移量：0x50

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:1 | Reserved | 0x00000000 | R/W | 保留 |
| 0 | EN | 0x00 | R/W | 使用DMA模式传输时，使能支持最小传输单位为BYTE，否则最小单位为WORD |