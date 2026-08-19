<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E7%B3%BB%E7%BB%9F%E6%8E%A7%E5%88%B6%E5%8D%95%E5%85%83SCU/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1311&CI1312芯片数据手册.zip)

# 系统控制单元SCU

系统控制单元主要负责芯片的时钟来源、部分时钟信号的产生与控制、中断控制等功能，该模块和DPMU模块一起提供芯片的时钟控制等基本功能。

## SCU寄存器映射

系统控制单元寄存器映射基地址为0x40000000，详见表SCU-1。

表SCU-1 系统控制单元寄存器映射

| 偏移量 | 名称 | 位宽 | 类型 | 复位值 | 描述 |
| --- | --- | --- | --- | --- | --- |
| 0x00 | SYS\_CTRL\_CFG | 32 | R/W | 0x00000401 | 系统控制寄存器 |
| 0x0C | EXT\_INT\_CFG | 32 | R/W | 0x00000000 | 外部中断配置寄存器 |
| 0x50 | SYSCFG\_LOCK\_CFG | 32 | R/W | 0x00000000 | 系统锁定配置寄存器 |
| 0x58 | CKCFG\_LOCK\_CFG | 32 | R/W | 0x00000000 | 时钟配置锁定配置寄存器 |
| 0x80 | CLKDIV\_PARAM0\_CFG | 32 | R/W | 0x1001808C | 分频参数寄存器0 |
| 0x84 | CLKDIV\_PARAM1\_CFG | 32 | R/W | 0x00008208 | 分频参数寄存器1 |
| 0xB0 | CLK\_DIV\_PARAM\_EN\_CFG | 32 | R/W | 0x00000000 | 分频参数使能寄存器 |
| 0x11C | SYS\_CLKGATE\_CFG0 | 32 | R/W | 0x00000FFC | 系统时钟门控配置寄存器 |
| 0x124 | AHB\_CLKGATE\_CFG | 32 | R/W | 0x0000007F | AHB总线模块时钟门控配置寄存器 |
| 0x128 | APB0\_CLKGATE\_CFG | 32 | R/W | 0x00007FFF | APB0总线模块时钟门控配置寄存器 |
| 0x12C | APB1\_CLKGATE\_CFG | 32 | R/W | 0x000001FF | APB1总线模块时钟门控配置寄存器 |
| 0x178 | SCU\_STATE\_REG | 32 | R/W | 0x00000001 | SCU状态寄存器 |
| 0x190 | AHB\_RESET\_CFG | 32 | R/W | 0x0000007E | AHB总线模块软件复位配置寄存器 |
| 0x194 | APB0\_RESET\_CFG | 32 | R/W | 0x00000FFF | APB0总线模块软件复位配置寄存器 |
| 0x198 | APB1\_RESET\_CFG | 32 | R/W | 0x000001FF | APB1总线模块软件复位配置寄存器 |
| 0x1DC | WAKEUP\_MASK\_CFG | 32 | R/W | 0x00000000 | 唤醒Mask配置寄存器 |
| 0x1F4 | INT\_STATE\_REG | 32 | R/W | 0x00000000 | 中断状态寄存器 |

## 系统控制寄存器（SYS\_CTRL\_CFG）

偏移量：0x00

复位值：0x00000401

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:11 | Reserved | 0x0 | RW | Reserved |
| 10 | DTR\_CLK\_SEL | 0x1 | RW | DTR控制器时钟来源： 0：PLL倍频前的时钟 1：PLL时钟 |
| 9 | RUN\_IN\_FLASH\_EN | 0x1 | RW | 控制系统程序在FLASH中运行（使能Flash XIP功能）： 1：在FLASH中运行 0：不在FLASH中运行 |
| 8:5 | Reserved | 0x1 | RW | Reserved |
| 4:1 | NMI\_INT\_CTRL | 0x0 | RW | 将CPU快速中断配置为如下中断源： 0：Reserved 1：INT\_IWDG 2：INT\_WWDG 3：INT\_EXT0 4：INT\_EXT1 5：INT\_TIMER0 6：INT\_TIMER1 7：INT\_UART0 8：INT\_UART1 9：INT\_UART2 10：INT\_GPIO0 11：INT\_GPIO1 12：INT\_GPIO2 13：INT\_VDT 14：Reserved 15：INT\_ADC |
| 0 | SPI\_BOOT | 0x1 | RW | Flash对应的QSPI控制器BOOT模式使能： 0：Flash对应的QSPI控制器为正常模式，非启动模式，用于读写Flash中的数据 1：Flash对应的QSPI控制器为BOOT模式，从Flash中读取代码并执行 芯片上电后会通过上电时采集的相关引脚的高低电平状态，控制系统的BOOT方式，若设定了当前系统从Flash中启动，则当系统完成启动后，需要把该SPI\_BOOT位配置为0，Flash对应的QSPI控制器才能恢复到正常模式，才能正常读写Flash中的数据 |

## 外部中断配置寄存器（EXT\_INT\_CFG）

偏移量：0x0C

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:4 | Reserved | 0x0 | RW | Reserved |
| 3 | EXT1\_INT\_EN | 0x0 | RW | 外部中断1中断使能： 1：发生外部中断请求时产生对应中断 0：中断不使能 |
| 2 | EXT0\_INT\_EN | 0x0 | RW | 外部中断0中断使能： 1：发生外部中断请求时产生对应中断 0：中断不使能 |
| 1 | EXT1\_INT\_STATE | 0x0 | RW | 外部中断1状态位： 1：发生外部中断请求 0：未发生外部中断请求 该位写1清除 |
| 0 | EXT0\_INT\_STATE | 0x0 | RW | 外部中断0状态位： 1：发生外部中断请求 0：未发生外部中断请求 该位写1清除 |

## 系统锁定配置寄存器（SYSCFG\_LOCK\_CFG）

偏移量：0x50

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:0 | SYSCFG\_LOCK | 0x0 | RW | 软件配置系统时需先向此寄存器写0x51AC0FFE解锁，才能写入系统控制单元各寄存器的配置。读此寄存器的值有以下含义: 1：本寄存器已解锁，可以写入 0：本寄存器未解锁，不能写入 |

## 时钟配置锁定配置寄存器（CKCFG\_LOCK\_CFG）

偏移量：0x58

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:0 | CKCFG\_LOCK | 0x0 | RW | 软件配置PLL和时钟门控相关寄存器时需要先向此寄存器写0x51AC0FFE解锁，然后才能进行配置，写其他任意值锁定。读此寄存器的值有以下含义： 1：本寄存器已解锁，可以写入 0：本寄存器未解锁，不能写入。 |

## 分频参数寄存器0（CLKDIV\_PARAM0\_CFG）

偏移量：0x80

复位值：0x1001808C

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31 | Reserved | 0x0 | RW | Reserved |
| 30:24 | TIMER\_GPWM\_DIV | 0x10 | RW | TIMER和PWM模块的时钟分频参数 |
| 23:12 | ST\_DIV | 0x18 | RW | CPU内核滴答(SysTick)时钟的分频参数 |
| 11:9 | DTR\_RAM\_DIV | 0x0 | RW | DTR Flash模块中RAM的时钟分频参数 |
| 8:6 | DTR\_DIV | 0x2 | RW | DTR Flash模块的时钟分频参数 |
| 5:0 | ADC\_DIV | 0xC | RW | ADC模块的时钟分频参数 |

## 分频参数寄存器1（CLKDIV\_PARAM1\_CFG）

偏移量：0x84

复位值：0x00008208

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:12 | Reserved | 0x0 | RW | Reserved |
| 11:6 | UART1\_DIV | 0x8 | RW | UART1模块的时钟分频参数 |
| 5:0 | UART0\_DIV | 0x8 | RW | UART0模块的时钟分频参数 |

## 分频参数使能寄存器（CLK\_DIV\_PARAM\_EN\_CFG）

偏移量：0xB0

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:11 | Reserved | 0x0 | RW | Reserved |
| 10 | SRC2\_MCLK\_DIV\_EN | 0x0 | RW | 分频参数SRC2\_MCLK\_DIV的更新使能： 1：使能 0：不使能 |
| 9 | SRC1\_MCLK\_DIV\_EN | 0x0 | RW | 分频参数SRC1\_MCLK\_DIV的更新使能： 1：使能 0：不使能 |
| 8 | SRC0\_MCLK\_DIV\_EN | 0x0 | RW | 分频参数SRC0\_MCLK\_DIV的更新使能： 1：使能 0：不使能 |
| 7 | Reserved | 0x0 | RW | Reserved |
| 6 | UART1\_DIV\_EN | 0x0 | RW | 分频参数UART1\_DIV的更新使能： 1：使能 0：不使能 |
| 5 | UART0\_DIV\_EN | 0x0 | RW | 分频参数UART0\_DIV的更新使能： 1：使能 0：不使能 |
| 4 | TIMER\_GPWM\_DIV\_EN | 0x0 | RW | 分频参数TIMER\_GPWM\_DIV的更新使能： 1：使能 0：不使能 |
| 3 | ST\_DIV\_EN | 0x0 | RW | 分频参数ST\_DIV的更新使能： 1：使能 0：不使能 |
| 2 | DTR\_RAM\_DIV\_EN | 0x0 | RW | 分频参数DTR\_RAM\_DIV的更新使能： 1：使能 0：不使能 |
| 1 | DTR\_DIV\_EN | 0x0 | RW | 分频参数DTR\_DIV的更新使能： 1：使能 0：不使能 |
| 0 | Reserved | 0x0 | RW | Reserved |

## 系统时钟门控配置寄存器（SYS\_CLKGATE\_CFG0）

偏移量：0x11C

复位值：0x00000FFC

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:12 | Reserved | 0x0 | RW | Reserved |
| 11 | ROM\_CKEN | 0x1 | RW | ROM模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 10 | SRAM6\_CLKEN | 0x1 | RW | SRAM6模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 9 | SRAM5\_CLKEN | 0x1 | RW | SRAM5模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 8 | SRAM4\_CLKEN | 0x1 | RW | SRAM4模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 7 | SRAM3\_CLKEN | 0x1 | RW | SRAM3模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 6 | SRAM2\_CLKEN | 0x1 | RW | SRAM2模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 5 | SRAM1\_CLKEN | 0x1 | RW | SRAM1模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 4 | SRAM0\_CLKEN | 0x1 | RW | SRAM0模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 3 | STCLK | 0x1 | RW | 系统滴答时钟STCLK模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 2 | CPU\_CORECLK | 0x1 | RW | CPU内核时钟模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 1 | SLEEPDEEP | 0x0 | RW | CPU处于深度睡眠时的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 0 | SLEEPING | 0x0 | RW | CPU睡眠时的时钟门控： 0：关闭该时钟 1：打开该时钟 |

\*\*\*注1：上述SRAM0到SRAM6共同组成芯片内部的640KB SRAM，正常使用时请全部设置时钟为打开状态 \*\*\*
\*\*\*注2：上述深度睡眠和睡眠是CPU的两种休眠模式，可以通过直接写CPU内置的寄存器实现，使用该模式时需提前打开对应的时钟，用户可自行查阅CPU的相关资料进行设置 \*\*\*

## AHB总线模块时钟门控配置寄存器（AHB\_CLKGATE\_CFG）

偏移量：0x124

复位值：0x0000007F

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:5 | Reserved | 0x3 | RW | Reserved |
| 4 | DTR\_CKEN | 0x1 | RW | DTR Flash模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 3 | Reserved | 0x1 | RW | Reserved |
| 2 | Reserved | 0x1 | RW | Reserved |
| 1 | GDMA\_CKEN | 0x1 | RW | DMA模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 0 | Reserved | 0x1 | RW | Reserved |

## APB0总线模块时钟门控配置寄存器（APB0\_CLKGATE\_CFG）

偏移量：0x128

复位值：0x00007FFF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:14 | Reserved | 0x3 | RW | Reserved |
| 13 | WWDG\_CPU\_HALT\_CKEN | 0x1 | RW | 窗口看门狗WWDG模块在CPU处于HALT状态时的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 12 | CODEC\_DA\_CKEN | 0x1 | RW | CODEC模块DAC的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 11 | CODEC\_AD\_CKEN | 0x1 | RW | CODEC模块ADC的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 10 | TIMER3\_CKEN | 0x1 | RW | TIMER3模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 9 | TIMER2\_CKEN | 0x1 | RW | TIMER2模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 8 | TIMER1\_CKEN | 0x1 | RW | TIMER1模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 7 | TIMER0\_CKEN | 0x1 | RW | TIMER0模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 6 | Reserved | 0x1 | RW | Reserved |
| 5 | GPWM2\_CKEN | 0x1 | RW | PWM2模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 4 | GPWM1\_CKEN | 0x1 | RW | PWM1模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 3 | GPWM0\_CKEN | 0x1 | RW | PWM0模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 2 | Reserved | 0x1 | RW | Reserved |
| 1 | IIC\_CKEN | 0x1 | RW | IIC模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 0 | WWDG\_CKEN | 0x1 | RW | WWDG模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |

## APB1总线模块时钟门控配置寄存器（APB1\_CLKGATE\_CFG）

偏移量：0x12C

复位值：0x000001FF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:9 | Reserved | 0x3 | RW | Reserved |
| 8:6 | Reserved | 0x7 | RW | Reserved |
| 5 | Reserved | 0x1 | RW | Reserved |
| 4 | Reserved | 0x1 | RW | Reserved |
| 3 | UART1\_CKEN | 0x1 | RW | UART1模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 2 | UART0\_CKEN | 0x1 | RW | UART0模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 1 | GPIO1\_CKEN | 0x1 | RW | GPIO1模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 0 | GPIO0\_CKEN | 0x1 | RW | GPIO0模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |

## SCU状态寄存器（SCU\_STATE\_REG）

偏移量：0x178

复位值：0x00000001

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:5 | Reserved | 0x0 | RW | Reserved |
| 4 | CPU\_DEEPSLEEP | 0x0 | RW | CPU的深度睡眠状态查询： 0：不处于深度睡眠状态 1：处于深度睡眠状态 |
| 3 | CPU\_SLEEP | 0x0 | RW | CPU的睡眠状态查询： 0：不处于睡眠状态 1：处于睡眠状态 |
| 2 | PLL\_LOCK\_STATE | 0x0 | RW | PLL的锁定状态查询： 0：不处于锁定状态 1：处于锁定状态 |
| 1 | BOOT\_MODE | 0x0 | RW | 系统启动模式查询： 0：片内ROM启动 1：片内SRAM启动 |
| 0 | Reserved | 0x1 | RW | Reserved |

## AHB总线模块软件复位配置寄存器（AHB\_RESET\_CFG）

偏移量：0x190

复位值：0x0000007E

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:5 | Reserved | 0x3 | RW | Reserved |
| 4 | DTR\_RSTEN | 0x1 | RW | DTR Flash模块软件复位控制： 0：复位 1：不复位 |
| 3 | Reserved | 0x1 | RW | Reserved |
| 2 | Reserved | 0x1 | RW | Reserved |
| 1 | GDMA\_RSTEN | 0x1 | RW | DMA模块软件复位控制： 0：复位 1：不复位 |
| 0 | Reserved | 0x1 | RW | Reserved |

## APB0总线模块软件复位配置寄存器（APB0\_RESET\_CFG）

偏移量：0x194

复位值：0x00000FFF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:12 | Reserved | 0x0 | RW | Reserved |
| 11 | TIMER23\_RSTEN | 0x1 | RW | TIMER2和TIMER3模块软件复位控制： 0：复位 1：不复位 |
| 10 | Reserved | 0x1 | RW | Reserved |
| 9 | TIMER01\_RSTEN | 0x1 | RW | TIMER0和TIMER1模块软件复位控制： 0：复位 1：不复位 |
| 8 | Reserved | 0x1 | RW | Reserved |
| 7 | GPWM23\_RSTEN | 0x1 | RW | PWM2模块软件复位控制： 0：复位 1：不复位 |
| 6 | Reserved | 0x1 | RW | Reserved |
| 5 | GPWM01\_RSTEN | 0x1 | RW | PWM0和PWM1模块软件复位控制： 0：复位 1：不复位 |
| 4 | Reserved | 0x1 | RW | Reserved |
| 3 | CODEC\_RSTEN | 0x1 | RW | CODEC模块软件复位控制： 0：复位 1：不复位 |
| 2 | Reserved | 0x1 | RW | Reserved |
| 1 | IIC\_RSTEN | 0x1 | RW | IIC模块软件复位控制： 0：复位 1：不复位 |
| 0 | WWDG\_RSTEN | 0x1 | RW | 窗口看门狗WWDG模块软件复位控制： 0：复位 1：不复位 |

## APB1总线模块软件复位配置寄存器（APB1\_RESET\_CFG）

偏移量：0x198

复位值：0x000001FF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:6 | Reserved | 0x0 | RW | Reserved |
| 5 | Reserved | 0x1 | RW | Reserved |
| 4 | Reserved | 0x1 | RW | Reserved |
| 3 | UART1\_RSTEN | 0x1 | RW | UART1模块软件复位控制： 0：复位 1：不复位 |
| 2 | UART0\_RSTEN | 0x1 | RW | UART0模块软件复位控制： 0：复位 1：不复位 |
| 1 | GPIO1\_RSTEN | 0x1 | RW | GPIO1模块软件复位控制： 0：复位 1：不复位 |
| 0 | GPIO0\_RSTEN | 0x1 | RW | GPIO0模块软件复位控制： 0：复位 1：不复位 |

## 唤醒Mask配置寄存器（WAKEUP\_MASK\_CFG）

偏移量：0x1DC

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:17 | Reserved | 0x0 | RW | Reserved |
| 16 | Reserved | 0x0 | RW | Reserved |
| 15 | Reserved | 0x0 | RW | Reserved |
| 14 | VDT\_INT | 0x0 | RW | VDT模块中断唤醒使能： 0：禁止 1：使能 |
| 13 | Reserved | 0x0 | RW | Reserved |
| 12 | Reserved | 0x0 | RW | Reserved |
| 11 | GPIO1\_INT | 0x0 | RW | GPIO1模块中断唤醒使能： 0：禁止 1：使能 |
| 10 | GPIO0\_INT | 0x0 | RW | GPIO0模块中断唤醒使能： 0：禁止 1：使能 |
| 9 | Reserved | 0x0 | RW | Reserved |
| 8 | UART1\_INT | 0x0 | RW | UART1模块中断唤醒使能： 0：禁止 1：使能 |
| 7 | UART0\_INT | 0x0 | RW | UART0模块中断唤醒使能： 0：禁止 1：使能 |
| 6 | TIMER1\_INT | 0x0 | RW | TIMER1模块中断唤醒使能： 0：禁止 1：使能 |
| 5 | TIMER0\_INT | 0x0 | RW | TIMER0模块中断唤醒使能： 0：禁止 1：使能 |
| 4 | WWDG\_INT | 0x0 | RW | 窗口看门狗WWDG模块中断唤醒使能： 0：禁止 1：使能 |
| 3 | IWDG\_INT | 0x0 | RW | 独立看门狗IWDG模块中断唤醒使能： 0：禁止 1：使能 |
| 2 | Reserved | 0x0 | RW | Reserved |
| 1 | Reserved | 0x0 | RW | Reserved |
| 0 | SCU\_INT | 0x0 | RW | SCU模块中断唤醒使能： 0：禁止 1：使能 |

## 中断状态寄存器（INT\_STATE\_REG）

偏移量：0x1F4

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:17 | Reserved | 0x0 | W1C | Reserved |
| 16 | Reserved | 0x0 | W1C | Reserved |
| 15 | Reserved | 0x0 | W1C | Reserved |
| 14 | Reserved | 0x0 | W1C | Reserved |
| 13 | Reserved | 0x0 | W1C | Reserved |
| 12 | Reserved | 0x0 | W1C | Reserved |
| 11 | GPIO1\_INT\_WAKE | 0x0 | W1C | GPIO1模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 10 | GPIO0\_INT\_WAKE | 0x0 | W1C | GPIO0模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 9 | Reserved | 0x0 | W1C | Reserved |
| 8 | UART1\_INT\_WAKE | 0x0 | W1C | UART1模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 7 | UART0\_INT\_WAKE | 0x0 | W1C | UART0模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 6 | TIMER1\_INT\_WAKE | 0x0 | W1C | TIMER1模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 5 | TIMER0\_INT\_WAKE | 0x0 | W1C | TIMER0模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 4 | WWDG\_INT\_WAKE | 0x0 | W1C | 窗口看门狗WWDG模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 3 | IWDG\_INT\_WAKE | 0x0 | W1C | 独立看门狗IWDG模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 2 | Reserved | 0x0 | W1C | Reserved |
| 1 | Reserved | 0x0 | W1C | Reserved |
| 0 | SCU\_INT\_WAKE | 0x0 | W1C | SCU模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |