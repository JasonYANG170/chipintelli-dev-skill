<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/IIC/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1306_datasheet V1.5_chs_20251205.pdf)

# IIC

## 功能介绍

芯片内置IIC（Inter-Integrated Circuit，集成电路总线），支持标准传输模式速率100Kbit/s和快速传输模式速率400Kbit/s。其主要特征如下：

* 支持IIC Master模式，master时支持7位和10位寻址
* 支持IIC transmitter和receiver功能
* IIC总线速率可配置，支持Standard-100Kbps/Fast-400Kbps速率
* 支持多master总线仲裁功能
* 支持SCL总线时钟同步和握手机制
* 支持中断和查询操作方式操作

## 寄存器映射

IIC寄存器映射的基地址为0x40011000，详细的寄存器映射见表IIC-1。

表IIC-1 IIC寄存器映射

| 偏移量 | 名称 | 位宽 | 类型 | 复位值 | 描述 |
| --- | --- | --- | --- | --- | --- |
| 0x00 | IIC\_SCLDIV | 32 | R/W | 0x00FA00FA | IIC SCL分频参数寄存器 |
| 0x04 | IIC\_SRHLD | 32 | R/W | 0x00FA00FA | IIC Start条件hold time |
| 0x08 | IIC\_DTHLD | 32 | R/W | 0x00040004 | IIC SDA Data time |
| 0x0C | IIC\_GLBCTRL | 32 | R/W | 0x00040080 | IIC全局控制寄存器 |
| 0x10 | IIC\_CMD | 32 | R/W | 0x00000000 | IIC命令寄存器 |
| 0x14 | IIC\_INTEN | 32 | R/W | 0x00000000 | IIC中断使能控制寄存器 |
| 0x18 | IIC\_INTCLR | 32 | WO | 0x00000000 | IIC中断清除寄存器 |
| 0x20 | IIC\_TXDR | 32 | R/W | 0x00000000 | IIC发送数据寄存器 |
| 0x24 | IIC\_RXDR | 32 | RO | 0x00000000 | IIC接收数据寄存器 |
| 0x28 | IIC\_TIMEOUT | 32 | R/W | 0x05F5E100 | IIC Timeout寄存器 |
| 0x2C | IIC\_STATUS | 32 | RO | 0x00001004 | IIC状态寄存器 |

## SCL分频参数寄存器（IIC\_SCLDIV）

偏移量：0x00

复位值：0x00FA00FA

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:16 | IIC\_SCLHWID | 0x00FA | R/W | SCL高电平宽度，仅master有效，以PCLK为时钟进行计数。默认按50MHz-100Kbps设置5us。 |
| 15:0 | IIC\_SCLLWID | 0x00FA | R/W | SCL低电平宽度，以PCLK为时钟进行计数。默认按50MHz-100Kbps设置5us。 1. Master时，用于SCL时钟产生； 2. 数据传输完后硬件会自动拉低SCL等待TB为高，当TB为高后会继续拉低SCL此寄存器设置的时间，然后再输出SCL高电平，用此确保数据在SCL高电平时稳定。 |

## START/STOP HOLD TIME寄存器（IIC\_SRHLD）

偏移量：0x04

复位值：0x00FA00FA

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:16 | IIC\_SPHLD | 0x00FA | R/W | stop条件的hold time时间（即stop条件到确认为空闲状态的这段时间），以PCLK时钟计数，仅master有效。默认按50MHz-100kbps处理，5us。 |
| 15:0 | IIC\_SRHLD | 0x00FA | R/W | Start/repeat-start条件的hold time时间，以PCLK时钟计数，仅master有效。默认按50MHz-100kbps处理，5us。 |

## DATA Sample/HOLD TIME寄存器（IIC\_DHLD）

偏移量：0x08

复位值：0x00040004

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:16 | IIC\_DTSAMPLE | 0x0004 | R/W | SCL上升沿后间隔此配置时间后才采样SDA信号。因SDA在SCL高电平有效，所以设置此寄存器用来控制采样时机，最快在SCL上升沿处采样数据。 |
| 15:0 | IIC\_DHLD | 0x0004 | R/W | Data hold time。作为transmitter时，SCL下降沿发生时，等待此时间后才发送新的SDA到总线。最快SCL下降沿时发送数据。 |

## 全局控制寄存器（IIC\_GLBCTRL）

偏移量：0x0C

复位值：0x00040080

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:24 | Reserved | 0x00 | R/W | 保留 |
| 23:16 | BUS\_FILTWID | 0x04 | R/W | I2C总线信号滤波宽度，最大支持256个PCLK周期的SCL和SDA滤波。 |
| 7 | SW\_RSTn | 0x1 | R/W | 模块软复位，低有效。先写0，后写1完成复位。 |
| 3 | TIMEOUT\_EN | 0 | R/W | Timeout功能使能，高有效。 |
| 2 | Reversed | 0 | R/W | 保留 |
| 1 | GLB\_EN | 0 | R/W | 模块全局使能，高有效。 |
| 0 | MSTSLV | 0 | R/W | Master模式选择 0：保留 1：Master模式，软件需等到I2C总线处于IDLE状态才能进入master模式。 |

## 命令寄存器（IIC\_CMD）

偏移量：0x10

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:5 | Reserved | 0x00000000 | R/W | 保留 |
| 4 | START | 0 | R/W | 产生start/re-start条件，仅在master模式时有效，需等到I2C总线IDLE时才能发起此命令。Start条件产生后硬件会自动清零。 0：不产生start条件； 1：产生start条件。 |
| 3 | STOP | 0 | R/W | 产生stop条件，仅在master时有效，stop条件产生后硬件会自动清零。 0：不产生stop条件； 1：产生stop条件。 |
| 2 | ACK | 0 | R/W | 作为receiver时，当前byte数据传输完后给transmitter的响应控制。软件需在读当前接收完的1byte数据时配置下一byte数据的ACK。 0：发送ACK给transmitter； 1：发送NOACK给transmitter。 |
| 0 | TB | 0 | R/W | 命令配置有效。 0：在1byte数据传输完后硬件自动清零，表示命令寄存器中的值失效，即数据传输完成等待CPU响应； 1：命令寄存器的配置有效，可以进行新的数据传输。 |

## 中断使能控制寄存器（IIC\_INTEN）

偏移量：0x14

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:7 | Reserved | 0x0000000 | R/W | 保留 |
| 6 | SLVADIE | 0 | R/W | Slave地址被寻中中断使能。 0：disable此种中断； 1：当此slave被寻中或有general call时向CPU产生中断。 |
| 5 | ARBLSTIE | 0 | R/W | 仲裁丢失中断使能。 0：禁止仲裁丢失中断产生； 1：master模式当仲裁丢失总线控制权时向CPU产生中断。 |
| 4 | SSTOPIE | 0 | R/W | 停止条件检测中断。 0：禁止停止条件检测中断产生； 1：slave模式当检测到有stop条件时向CPU产生中断。 |
| 3 | BEIE | 0 | R/W | 总线错误中断使能。 0：disable此中断； 1：当总线出错时向CPU产生中断。在1Byte data+ACK期间产生了start/stop条件视为总线错误。 |
| 2 | TXDEPTIE | 0 | R/W | 发送寄存器空中断使能。 0：disable此中断； 1：当完成1byte数据发送（包括ACK位）后向CPU产生中断。 |
| 1 | RXDFULIE | 0 | R/W | 接收寄存器满中断使能。 0：disable此中断； 1：当完成1byte数据接收（包括ACK位）后向CPU产生中断，通知CPU将数据读走。 |
| 0 | TIMEOUTIE | 0 | R/W | Timeout中断使能。 0：禁止此种中断； 1：当总线在start和stop之间的高/低电平时间长度超过预设值后产生timeout中断给CPU，由CPU处理此模块的行为。 |

## 中断/状态清除寄存器（IIC\_INTCLR）

偏移量：0x18

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:7 | Reserved | 0x0000000 | W | 保留 |
| 6 | CLR\_SLVAD | 0 | W | 当向此位写1时清除slave被寻中/general call状态/中断。 |
| 5 | CLR\_ARBLST | 0 | W | 当向此位写1时清除仲裁丢失状态/中断。 |
| 4 | CLR\_SSTOP | 0 | W | 当向此位写1时清除slave检测到stop条件状态/中断。 |
| 3 | CLR\_BE | 0 | W | 当向此位写1时清除总线错误状态/中断。 |
| 2 | CLR\_TXDEPT | 0 | W | 当向此位写1时清除发送数据寄存器空状态/中断。 |
| 1 | CLR\_RXDFUL | 0 | W | 当向此位写1时清除接收数据寄存器满状态/中断。 |
| 0 | CLR\_TIMEOUT | 0 | W | 当向此位写1时清除timeout状态/中断。 |

## 发送数据寄存器（IIC\_TXDR）

偏移量：0x20

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:8 | Reserved | 0x0000000 | R/W | 保留 |
| 7:0 | IIC\_TXDR | 0 | R/W | 需向IIC总线发送的数据。 [0]：master时在start后作为R/nW位，其余时候作为数据的最低位； [7:1]：master时在start后作为要寻址的slave地址，其余时候作为数据的[7:1]。 |

## 接收数据寄存器（IIC\_RXDR）

偏移量：0x24

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:8 | Reserved | 0x0000000 | R | 保留 |
| 7:0 | IIC\_RXDR | 0 | R | 从IIC总线接收到的数据 |

## 总线TIMEOUT寄存器（IIC\_TIMEOUT）

偏移量：0x28

复位值：0x05F5E100

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:0 | TIMEOUT\_VALUE | 0x05F5E100 | R/W | Timeout时长预设值。当总线SCL高电平时长超过此设置则发生timeout，以PCLK作为参考时钟进行计数。 |

## 状态寄存器（IIC\_STATUS）

偏移量：0x2C

复位值：0x00001004

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:16 | Reserved | 0x0000 | R | 保留 |
| 15 | I2CBUS\_BUSY | 0 | R | I2C总线IDLE/BUSY状态。 0：I2C bus处于IDLE状态； 1：I2C bus处于busy状态，在start和stop条件之间为高。 |
| 14 | ACK\_STAT | 0 | R | ACK周期的响应状态。 0：receiver向transmitter发送了ACK； 1：receiver向transmitter发送了NOACK。 |
| 13 | REWR | 0 | R | 读写状态，为slave地址后的R/nW位，stop后自动清零。 0：此模块作为master-transmitter或slave-receiver； 1：此模块作为master-receiver或slave-transmitter。 |
| 12 | TBCMPLT | 1 | R | 1byte数据传输完成状态，在IIC\_CMD[TB]有效后自动清零。 0：未完成1个byte的数据传输（发送/接收）； 1：完成1个byte的数据传输（发送/接收）； |
| 11 | TRANMITTER | 0 | R | Transmitter标志，内部逻辑根据总线上的R/nW位及master/slave模式自动生成的transmitter标志，Stop后自动清零。 1：此模块作为master/slave-transmitter； 0：此模块作为master/slave-receiver； |
| 10 | MST\_SLV | 0 | R | 当前模块工作的master/slave模式状态，由于存在总线竞争，所以模块不一定工作在master模式下。 1：此模块工作在master模式下； 0：此模块工作在slave模式下。 |
| 9:8 | Reserved | 0 | R | 保留 |
| 7 | GENCALL | 0 | R | 广播呼叫检测状态。 0：无广播呼叫； 1：总线有广播呼叫。 |
| 6 | SLVAD | 0 | R | Slave被寻中状态。 0：此slave未被寻中； 1：此slave被总线上其他master寻中。 |
| 5 | ARBLST | 0 | R | Master总线仲裁丢失状态。 0：总线仲裁未丢失； 1：总线仲裁丢失了总线控制权。 |
| 4 | SSTOP | 0 | R | 停止条件检测状态，master/slave均能使用。 0：总线无stop条件发生； 1：总线有stop条件发生。 |
| 3 | BERR | 0 | R | 总线错误状态。 0：总线正常； 1：总线发生错误，有不符合IIC协议的行为发生。 |
| 2 | TXDEPT | 1 | R | 发送完1byte数据状态。 0：未发送完1byte数据； 1：发送完1byte数据。 |
| 1 | RXDFUL | 0 | R | 接收完1byte数据状态。 0：数据接收寄存器空； 1：数据接收寄存器满。 |
| 0 | TIMEOUT | 0 | R | Timeout状态。 0：无timeout； 1：SCL高电平宽度超过预设值，timeout发生。 |