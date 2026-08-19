<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E7%B3%BB%E7%BB%9F%E6%8F%8F%E8%BF%B0/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1311&CI1312芯片数据手册.zip)

# 系统描述

芯片系统框图如图S-1所示，其内部由多个模块组成，包含脑神经网络处理器BNPU等。下面分别针对各个模块进行描述。

![CI1303系统框图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI1311%26CI1312%E8%8A%AF%E7%89%87%E7%B3%BB%E7%BB%9F%E6%A1%86%E5%9B%BE.png)

图S-1 系统框图

## 系统架构

芯片系统包含了BNPU、CPU、ROM、SRAM、DMA和各类外设接口。各功能模块通过支持多核并行处理架构的总线进行通信和控制，其架构如图S-2所示。

![CI1303系统架构](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI1311%26CI1312%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84%E5%9B%BE.png)

图S-2 系统架构

## 寄存器映射

芯片寄存器映射如图S-3所示，内部ROM起始地址从0x00000000开始；SRAM起始地址从0x1FF00000开始到0x1FF7FFFF结束，共640Kbyte。其余是各外设接口的起始地址。

![CI1303寄存器映射](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI1311%26CI1312%E5%AF%84%E5%AD%98%E5%99%A8%E6%98%A0%E5%B0%84%E5%9B%BE.png)

图S-3 寄存器映射

## 中断

芯片集成了内核中断控制器，可进行高效的中断处理。该控制器功能描述如下：

* 支持软件中断、计时器中断和外部中断；
* 32路可编程外部中断；
* 3 bits中断优先级配置，即8个优先级等级；
* 支持软件动态可编程修改中断级别和中断优先级的数值；
* 支持基于中断级别的中断嵌套；
* 支持快速向量中断处理机制；
* 支持快速中断咬尾机制；
* 支持 NMI（Non-Maskable Interrupt）。

中断向量表如下表所示，发生相应中断后，CPU会从对应的中断入口地址执行指令。

表S-1 芯片中断向量表

| IRQ号 | 中断源 | 说明 |
| --- | --- | --- |
| 0 | INT\_WWDG | 窗口看门狗中断 |
| 1 | INT\_SCU | SCU中断 |
| 2 | Reserved | 保留 |
| 3 | Reserved | 保留 |
| 4 | Reserved | 保留 |
| 5 | INT\_TIMER0 | 定时器0中断 |
| 6 | INT\_TIMER1 | 定时器1中断 |
| 7 | INT\_TIMER2 | 定时器2中断 |
| 8 | INT\_TIMER3 | 定时器3中断 |
| 9 | INT\_IIC | IIC中断 |
| 10 | INT\_GPIO0 | GPIO0中断 |
| 11 | INT\_GPIO1 | GPIO1中断 |
| 12 | INT\_UART0 | UART0中断 |
| 13 | INT\_UART1 | UART1中断 |
| 14 | Reserved | 保留 |
| 15 | Reserved | 保留 |
| 16 | Reserved | 保留 |
| 17 | Reserved | 保留 |
| 18 | Reserved | 保留 |
| 19 | Reserved | 保留 |
| 20 | Reserved | 保留 |
| 21 | INT\_DTR | DTR Flash控制器中断 |
| 22 | Reserved | 保留 |
| 23 | INT\_VDT | 低电压检测指示中断 |
| 24 | Reserved | 保留 |
| 25 | Reserved | 保留 |
| 26 | INT\_IWDG | 独立看门狗中断 |
| 27 | Reserved | 保留 |
| 28 | Reserved | 保留 |
| 29 | INT\_EFUSE | EFUSE控制器中断 |
| 30 | Reserved | 保留 |

## 模块概述

本文档会详细描述用户经常使用到的模块及寄存器说明，列举如下：

* 系统控制单元SCU
* DMA
* 通用定时器和PWM输出
* 独立看门狗（IWTD）
* 窗口看门狗（WWTD）
* DTR\_FLASH
* IIC
* UART
* GPIO
* EFUSE

其它如BNPU、CODEC、电源管理和PLL、EFUSE等模块的配置和使用已包含在CI13XX系列芯片SDK提供的基础组件内，不建议用户直接修改驱动或者直接操作寄存器，以避免导致基础组件工作异常，建议直接使用CI13XX系列芯片SDK内提供的标准驱动接口，如果确实有较特殊需求请联系我司技术支持人员进行支持。