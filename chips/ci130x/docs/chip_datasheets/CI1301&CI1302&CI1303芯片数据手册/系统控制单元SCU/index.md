<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E7%B3%BB%E7%BB%9F%E6%8E%A7%E5%88%B6%E5%8D%95%E5%85%83SCU/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1301&CI1302&CI1303芯片数据手册.zip)

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
| 0x88 | CLKDIV\_PARAM2\_CFG | 32 | R/W | 0x03C0F03C | 分频参数寄存器2 |
| 0xB0 | CLK\_DIV\_PARAM\_EN\_CFG | 32 | R/W | 0x00000000 | 分频参数使能寄存器 |
| 0xC0 | SRC0\_MCLK\_CFG | 32 | R/W | 0x00000001 | MCLK来源时钟0配置寄存器 |
| 0xC4 | SRC1\_MCLK\_CFG | 32 | R/W | 0x00000001 | MCLK来源时钟1配置寄存器 |
| 0xC8 | SRC2\_MCLK\_CFG | 32 | R/W | 0x00000001 | MCLK来源时钟2配置寄存器 |
| 0xD0 | MCLK0\_CFG | 32 | R/W | 0x00000001 | IIS0 MCLK配置寄存器 |
| 0xE0 | IIS0\_CLK\_SEL\_CFG | 32 | R/W | 0x00000001 | IIS0时钟选择配置寄存器 |
| 0xF0 | PAD\_CLK\_SEL\_CFG | 32 | R/W | 0x00000009 | IIS引脚时钟选择配置寄存器 |
| 0xF8 | PDM\_CLK\_SEL\_CFG | 32 | R/W | 0x00000001 | PDM时钟选择配置寄存器 |
| 0x11C | SYS\_CLKGATE\_CFG0 | 32 | R/W | 0x00000FFC | 系统时钟门控配置寄存器 |
| 0x124 | AHB\_CLKGATE\_CFG | 32 | R/W | 0x0000007F | AHB总线模块时钟门控配置寄存器 |
| 0x128 | APB0\_CLKGATE\_CFG | 32 | R/W | 0x00007FFF | APB0总线模块时钟门控配置寄存器 |
| 0x12C | APB1\_CLKGATE\_CFG | 32 | R/W | 0x000001FF | APB1总线模块时钟门控配置寄存器 |
| 0x178 | SCU\_STATE\_REG | 32 | R/W | 0x00000001 | SCU状态寄存器 |
| 0x190 | AHB\_RESET\_CFG | 32 | R/W | 0x0000007E | AHB总线模块软件复位配置寄存器 |
| 0x194 | APB0\_RESET\_CFG | 32 | R/W | 0x00000FFF | APB0总线模块软件复位配置寄存器 |
| 0x198 | APB1\_RESET\_CFG | 32 | R/W | 0x000001FF | APB1总线模块软件复位配置寄存器 |
| 0x1DC | WAKEUP\_MASK\_CFG | 32 | R/W | 0x00000000 | 唤醒Mask配置寄存器 |
| 0x1E4 | EXT0\_FILTER\_CFG | 32 | R/W | 0x0000FFFF | 外部中断0滤波使能配置寄存器 |
| 0x1E8 | EXT1\_FILTER\_CFG | 32 | R/W | 0x0000FFFF | 外部中断1滤波使能配置寄存器 |
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
| 31:18 | Reserved | 0x0 | RW | Reserved |
| 17:12 | UART2\_DIV | 0x8 | RW | UART2模块的时钟分频参数 |
| 11:6 | UART1\_DIV | 0x8 | RW | UART1模块的时钟分频参数 |
| 5:0 | UART0\_DIV | 0x8 | RW | UART0模块的时钟分频参数 |

## 分频参数寄存器2（CLKDIV\_PARAM2\_CFG）

偏移量：0x88

复位值：0x03C0F03C

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:30 | Reserved | 0x0 | RW | Reserved |
| 29:20 | SRC2\_MCLK\_DIV | 0x3C | RW | MCLK来源时钟2的分频参数 |
| 19:10 | SRC1\_MCLK\_DIV | 0x3C | RW | MCLK来源时钟1的分频参数 |
| 9:0 | SRC0\_MCLK\_DIV | 0x3C | RW | MCLK来源时钟0的分频参数 |

## 分频参数使能寄存器（CLK\_DIV\_PARAM\_EN\_CFG）

偏移量：0xB0

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:11 | Reserved | 0x0 | RW | Reserved |
| 10 | SRC2\_MCLK\_DIV\_EN | 0x0 | RW | 分频参数SRC2\_MCLK\_DIV的更新使能： 1：使能 0：不使能 |
| 9 | SRC1\_MCLK\_DIV\_EN | 0x0 | RW | 分频参数SRC1\_MCLK\_DIV的更新使能： 1：使能 0：不使能 |
| 8 | SRC0\_MCLK\_DIV\_EN | 0x0 | RW | 分频参数SRC0\_MCLK\_DIV的更新使能： 1：使能 0：不使能 |
| 7 | UART2\_DIV\_EN | 0x0 | RW | 分频参数UART2\_DIV的更新使能： 1：使能 0：不使能 |
| 6 | UART1\_DIV\_EN | 0x0 | RW | 分频参数UART1\_DIV的更新使能： 1：使能 0：不使能 |
| 5 | UART0\_DIV\_EN | 0x0 | RW | 分频参数UART0\_DIV的更新使能： 1：使能 0：不使能 |
| 4 | TIMER\_GPWM\_DIV\_EN | 0x0 | RW | 分频参数TIMER\_GPWM\_DIV的更新使能： 1：使能 0：不使能 |
| 3 | ST\_DIV\_EN | 0x0 | RW | 分频参数ST\_DIV的更新使能： 1：使能 0：不使能 |
| 2 | DTR\_RAM\_DIV\_EN | 0x0 | RW | 分频参数DTR\_RAM\_DIV的更新使能： 1：使能 0：不使能 |
| 1 | DTR\_DIV\_EN | 0x0 | RW | 分频参数DTR\_DIV的更新使能： 1：使能 0：不使能 |
| 0 | ADC\_DIV\_EN | 0x0 | RW | 分频参数ADC\_DIV的更新使能： 1：使能 0：不使能 |

## MCLK来源时钟0配置寄存器（SRC0\_MCLK\_CFG）

偏移量：0xC0

复位值：0x00000001

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:3 | Reserved | 0x0 | RW | Reserved |
| 2:1 | SRC0\_MCLK\_SEL | 0x0 | RW | MCLK的来源0的时钟： 0：系统内核时钟 1：外部晶振时钟 2：内部RC晶振时钟 3：芯片MCLK引脚输入时钟 |
| 0 | SRC0\_MCLK\_CKEN | 0x1 | RW | MCLK的来源0的时钟门控： 0：关闭该时钟 1：打开该时钟 |

## MCLK来源时钟1配置寄存器（SRC1\_MCLK\_CFG）

偏移量：0xC4

复位值：0x00000001

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:3 | Reserved | 0x0 | RW | Reserved |
| 2:1 | SRC1\_MCLK\_SEL | 0x0 | RW | MCLK的来源1的时钟： 0：系统内核时钟 1：外部晶振时钟 2：内部RC晶振时钟 3：芯片MCLK引脚输入时钟 |
| 0 | SRC1\_MCLK\_CKEN | 0x1 | RW | MCLK的来源1的时钟门控： 0：关闭该时钟 1：打开该时钟 |

## MCLK来源时钟2配置寄存器（SRC2\_MCLK\_CFG）

偏移量：0xC8

复位值：0x00000001

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:3 | Reserved | 0x0 | RW | Reserved |
| 2:1 | SRC2\_MCLK\_SEL | 0x0 | RW | MCLK的来源2的时钟： 0：系统内核时钟 1：外部晶振时钟 2：内部RC晶振时钟 3：芯片MCLK引脚输入时钟 |
| 0 | SRC2\_MCLK\_CKEN | 0x1 | RW | MCLK的来源2的时钟门控： 0：关闭该时钟 1：打开该时钟 |

## IIS0 MCLK配置寄存器（MCLK0\_CFG）

偏移量：0xD0

复位值：0x00000001

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:6 | Reserved | 0x0 | RW | Reserved |
| 5:4 | MCLK0\_FS\_SEL | 0x0 | RW | 配置IIS0的MCLK的过采样率： 0：128FS 1：192FS 2：256FS 3：384FS |
| 3 | MCLK0\_WID\_SEL | 0x0 | RW | 配置SCK和LRCK的频率关系比值： 1：SCK/LRCK=64 0：SCK/LRCK=32 |
| 2:1 | MCLK0\_SEL | 0x0 | RW | 配置IIS0的MCLK来源： 0：MCLK0 1：MCLK1 2：MCLK2 3：芯片MCLK引脚输入时钟 |
| 0 | MCLK0\_CKEN | 0x1 | RW | IIS0的MCLK的时钟门控： 0：关闭该时钟 1：打开该时钟 |

## IIS0时钟选择配置寄存器（IIS0\_CLK\_SEL\_CFG）

偏移量：0xE0

复位值：0x00000001

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:4 | Reserved | 0x0 | RW | Reserved |
| 3:1 | IIS0\_CLK\_SEL | 0x0 | RW | SCK/LRCK输出来源： 0：MCLK0产生的SCK/LRCK 1：MCLK1产生的SCK/LRCK 2：MCLK2产生的SCK/LRCK 3：芯片引脚输入的外部SCK/LRCK 4：内部Codec的ADC输入时钟 5：内部Codec的DAC输入时钟 6：PDM的输入时钟 7：PDM的输入时钟（和6功能相同） |
| 0 | IIS0\_CLK\_MUX\_CKEN | 0x1 | RW | IIS0模块选择上述时钟来源后的时钟门控： 0：关闭该时钟 1：打开该时钟 |

## IIS引脚时钟选择配置寄存器（PAD\_CLK\_SEL\_CFG）

偏移量：0xF0

复位值：0x00000009

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:9 | Reserved | 0x0 | RW | Reserved |
| 8 | PAD\_SCK\_LRCK\_PAD\_OEN | 0x0 | RW | SCK和LRCK的芯片引脚的方向选择： 0：输出 1：输入 |
| 7 | PAD\_MCLK\_PAD\_OEN | 0x0 | RW | MCLK的芯片引脚的方向选择： 0：输出 1：输入 |
| 6:4 | PAD\_IIS\_CLK\_SEL | 0x0 | RW | SCK/LRCK引脚输出来源： 0：MCLK0产生的SCK/LRCK 1：MCLK1产生的SCK/LRCK 2：MCLK2产生的SCK/LRCK 3：Reserved 4：内部Codec的ADC输入时钟 5：内部Codec的DAC输入时钟 6：PDM的输入时钟 7：PDM的输入时钟（和6功能相同） |
| 3 | PAD\_IIS\_CLK\_MUX\_CKEN | 0x1 | RW | SCK和LRCK选择上述时钟来源后的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 2:1 | PAD\_MCLK\_SEL | 0x0 | RW | MCLK引脚输出来源： 0：MCLK0 1：MCLK1 2：MCLK2 3：低电平 |
| 0 | PAD\_MCLK\_MUX\_CKEN | 0x1 | RW | MCLK选择上述时钟来源后的时钟门控： 0：关闭该时钟 1：打开该时钟 |

## PDM时钟选择配置寄存器（PDM\_CLK\_SEL\_CFG）

偏移量：0xF8

复位值：0x00000001

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:3 | Reserved | 0x0 | RW | Reserved |
| 2:1 | PDM\_MCLK\_SEL | 0x0 | RW | PDM模块的MCLK来源： 0：MCLK0 1：MCLK1 2：MCLK2 3：MCLK硬件输入的时钟 |
| 0 | PDM\_MCLK\_MUX\_CKEN | 0x1 | RW | PDM模块的MCLK选择上述时钟来源后的时钟门控： 0：关闭该时钟 1：打开该时钟 |

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
| 2 | ADC\_CKEN | 0x1 | RW | ADC模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
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
| 6 | GPWM3\_CKEN | 0x1 | RW | PWM3模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 5 | GPWM2\_CKEN | 0x1 | RW | PWM2模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 4 | GPWM1\_CKEN | 0x1 | RW | PWM1模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 3 | GPWM0\_CKEN | 0x1 | RW | PWM0模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 2 | PDM\_CKEN | 0x1 | RW | PDM模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 1 | IIC\_CKEN | 0x1 | RW | IIC模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 0 | WWDG\_CKEN | 0x1 | RW | WWDG模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |

## APB1总线模块时钟门控配置寄存器（APB1\_CLKGATE\_CFG）

偏移量：0x12C

复位值：0x000001FF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:9 | Reserved | 0x3 | RW | Reserved |
| 8:6 | Reserved | 0x7 | RW | Reserved |
| 5 | IIS0\_CKEN | 0x1 | RW | IIS0模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
| 4 | UART2\_CKEN | 0x1 | RW | UART2模块的时钟门控： 0：关闭该时钟 1：打开该时钟 |
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
| 2 | ADC\_RSTEN | 0x1 | RW | ADC模块软件复位控制： 0：复位 1：不复位 |
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
| 7 | GPWM23\_RSTEN | 0x1 | RW | PWM2和PWM3模块软件复位控制： 0：复位 1：不复位 |
| 6 | Reserved | 0x1 | RW | Reserved |
| 5 | GPWM01\_RSTEN | 0x1 | RW | PWM0和PWM1模块软件复位控制： 0：复位 1：不复位 |
| 4 | Reserved | 0x1 | RW | Reserved |
| 3 | CODEC\_RSTEN | 0x1 | RW | CODEC模块软件复位控制： 0：复位 1：不复位 |
| 2 | PDM\_RSTEN | 0x1 | RW | PDM模块软件复位控制： 0：复位 1：不复位 |
| 1 | IIC\_RSTEN | 0x1 | RW | IIC模块软件复位控制： 0：复位 1：不复位 |
| 0 | WWDG\_RSTEN | 0x1 | RW | 窗口看门狗WWDG模块软件复位控制： 0：复位 1：不复位 |

## APB1总线模块软件复位配置寄存器（APB1\_RESET\_CFG）

偏移量：0x198

复位值：0x000001FF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:6 | Reserved | 0x0 | RW | Reserved |
| 5 | IIS0\_RSTEN | 0x1 | RW | IIS0模块软件复位控制： 0：复位 1：不复位 |
| 4 | UART2\_RSTEN | 0x1 | RW | UART2模块软件复位控制： 0：复位 1：不复位 |
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
| 16 | ADC\_INT | 0x0 | RW | ADC模块中断唤醒使能： 0：禁止 1：使能 |
| 15 | Reserved | 0x0 | RW | Reserved |
| 14 | VDT\_INT | 0x0 | RW | VDT模块中断唤醒使能： 0：禁止 1：使能 |
| 13 | IIS\_INT | 0x0 | RW | IIS模块中断唤醒使能： 0：禁止 1：使能 |
| 12 | GPIO2\_INT | 0x0 | RW | GPIO2模块中断唤醒使能： 0：禁止 1：使能 |
| 11 | GPIO1\_INT | 0x0 | RW | GPIO1模块中断唤醒使能： 0：禁止 1：使能 |
| 10 | GPIO0\_INT | 0x0 | RW | GPIO0模块中断唤醒使能： 0：禁止 1：使能 |
| 9 | UART2\_INT | 0x0 | RW | UART2模块中断唤醒使能： 0：禁止 1：使能 |
| 8 | UART1\_INT | 0x0 | RW | UART1模块中断唤醒使能： 0：禁止 1：使能 |
| 7 | UART0\_INT | 0x0 | RW | UART0模块中断唤醒使能： 0：禁止 1：使能 |
| 6 | TIMER1\_INT | 0x0 | RW | TIMER1模块中断唤醒使能： 0：禁止 1：使能 |
| 5 | TIMER0\_INT | 0x0 | RW | TIMER0模块中断唤醒使能： 0：禁止 1：使能 |
| 4 | WWDG\_INT | 0x0 | RW | 窗口看门狗WWDG模块中断唤醒使能： 0：禁止 1：使能 |
| 3 | IWDG\_INT | 0x0 | RW | 独立看门狗IWDG模块中断唤醒使能： 0：禁止 1：使能 |
| 2 | EXT\_INT1 | 0x0 | RW | 外部中断1模块中断唤醒使能： 0：禁止 1：使能 |
| 1 | EXT\_INT0 | 0x0 | RW | 外部中断0模块中断唤醒使能： 0：禁止 1：使能 |
| 0 | SCU\_INT | 0x0 | RW | SCU模块中断唤醒使能： 0：禁止 1：使能 |

## 外部中断0滤波使能配置寄存器（EXT0\_FILTER\_CFG）

偏移量：0x1E4

复位值：0x0000FFFF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:21 | Reserved | 0x0 | RW | Reserved |
| 20 | FILTER0\_EN | 0x0 | RW | 外部中断0输入信号滤波功能使能： 0：禁止 1：使能 |
| 19:0 | EXT0\_FILTER | 0xFFFF | RW | 外部中断0滤波参数 |

## 外部中断1滤波使能配置寄存器（EXT1\_FILTER\_CFG）

偏移量：0x1E8

复位值：0x0000FFFF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:21 | Reserved | 0x0 | RW | Reserved |
| 20 | FILTER1\_EN | 0x0 | RW | 外部中断1输入信号滤波功能使能： 0：禁止 1：使能 |
| 19:0 | EXT1\_FILTER | 0xFFFF | RW | 外部中断1滤波参数 |

***注：外部中断0和外部中断1可以进行数字滤波，滤波时以一个晶振时钟周期进行计数，累计计数值大于等于EXT0\_FILTER/EXT1\_FILTER的值时，才会触发对应的外部中断。EXT0\_FILTER/EXT1\_FILTER的值越大，需要外部中断保持有效触发电平的时间越长。***

## 中断状态寄存器（INT\_STATE\_REG）

偏移量：0x1F4

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:17 | Reserved | 0x0 | W1C | Reserved |
| 16 | ADC\_INT\_WAKE | 0x0 | W1C | ADC模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 15 | Reserved | 0x0 | W1C | Reserved |
| 14 | VDT\_INT\_WAKE | 0x0 | W1C | VDT模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 13 | IIS\_INT\_WAKE | 0x0 | W1C | IIS模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 12 | GPIO2\_INT\_WAKE | 0x0 | W1C | GPIO2模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 11 | GPIO1\_INT\_WAKE | 0x0 | W1C | GPIO1模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 10 | GPIO0\_INT\_WAKE | 0x0 | W1C | GPIO0模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 9 | UART2\_INT\_WAKE | 0x0 | W1C | UART2模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 8 | UART1\_INT\_WAKE | 0x0 | W1C | UART1模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 7 | UART0\_INT\_WAKE | 0x0 | W1C | UART0模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 6 | TIMER1\_INT\_WAKE | 0x0 | W1C | TIMER1模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 5 | TIMER0\_INT\_WAKE | 0x0 | W1C | TIMER0模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 4 | WWDG\_INT\_WAKE | 0x0 | W1C | 窗口看门狗WWDG模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 3 | IWDG\_INT\_WAKE | 0x0 | W1C | 独立看门狗IWDG模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 2 | EXT\_INT1\_WAKE | 0x0 | W1C | 外部中断1模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 1 | EXT\_INT0\_WAKE | 0x0 | W1C | 外部中断0模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |
| 0 | SCU\_INT\_WAKE | 0x0 | W1C | SCU模块中断唤醒状态： 0：中断未引起系统唤醒 1：中断引起系统唤醒，向该位写1清除该状态 |