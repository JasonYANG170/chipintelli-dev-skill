<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1311%26CI1312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/DTR%20Flash/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI1311&CI1312芯片数据手册.zip)

# DTR Flash

芯片内置了支持以DTR、单线、四线方式工作的Nor Flash控制器，可以支持各类Nor Flash器件。芯片已内置有Nor Flash，用户使用时可以直接调用SDK中的相应驱动代码，来实现Flash烧写和读取等功能。

## 功能介绍

DMA控制器主要特征如下：

* 数据总线接口：
  + 支持BIT/HALF-WORD/WORD读写操作
  + 支持SINGLE/BURST4/BURST8/BUSRT16操作
* SPI接口：
  + 支持4线SPI
  + 支持SPI（单线）/OCTAL（4线）/DTR（4线双沿）三种传输模式
  + 兼容FLASH各种命令操作
  + 支持发送操作的相位调整
  + 支持采集数据时的采集时钟相位调整及delayline微调
  + 支持DQS方式采集数据
* 三种控制器操作模式：
  + 预取读操作模式（XIP）
  + 通用操作模式
* 传输支持寄存器或FIFO缓存
  + 一个读操作异步FIFO，数据宽度32BIT，深度64，触发阈值可配置
  + 一个写操作异步FIFO，数据宽度32BIT，深度64，触发阈值可配置
  + 两个32BIT的数据配置寄存器
  + 两个32BIT的数据读取寄存器
* 支持的中断类型：
  + 通用操作模式完成标志中断源
  + 错误的写操作中断源
  + FIFO underflow\overflow中断源
  + 复位期间操作中断源
* 支持DMA传输

## 寄存器映射

DTR Flash控制器的寄存器映射基地址为0x40004000，详见表F-1。

表F-1 DTR Flash寄存器映射

| 偏移量 | 名称 | 位宽 | 类型 | 复位值 | 描述 |
| --- | --- | --- | --- | --- | --- |
| 0x00 | GLOBE\_CFG | 32 | R/W | 0x00000000 | 全局配置寄存器 |
| 0x04 | GLOBE1\_CFG | 32 | R/W | 0x00000000 | 全局配置寄存器1 |
| 0x08 | RX\_CLK\_CFG | 32 | R/W | 0x00000010 | 接收时钟配置寄存器 |
| 0x0C | ADDR\_MASK\_CFG | 32 | R/W | 0x00000FFF | 地址屏蔽配置寄存器 |
| 0x10 | GM\_CFG | 32 | R/W | 0x00000000 | 通用模式配置寄存器 |
| 0x14 | GM1\_CFG | 32 | R/W | 0x00000000 | 通用模式配置寄存器1 |
| 0x18 | GM\_ADDR\_CFG | 32 | R/W | 0x00000000 | 通用模式读写地址配置寄存器 |
| 0x1C | GM\_DATA\_SIZE\_CFG | 32 | R/W | 0x00000000 | 通用模式数据大小配置寄存器 |
| 0x20 | RD\_CFG | 32 | R/W | 0x00000000 | 预取读模式配置寄存器 |
| 0x24 | RD1\_CFG | 32 | R/W | 0x00000000 | 预取读模式配置寄存器1 |
| 0x30 | FIFO\_LEVEL\_CFG | 32 | R/W | 0x00000000 | FIFO级别配置寄存器 |
| 0x34 | R/W\_DATA0\_CFG | 32 | R/W | 0x00000000 | 读写缓存寄存器0 |
| 0x38 | R/W\_DATA1\_CFG | 32 | R/W | 0x00000000 | 读写缓存寄存器1 |
| 0x3C | INT\_CTRL\_CFG | 32 | R/W | 0x00000000 | 中断控制配置寄存器 |
| 0x40 | STATE\_REG | 32 | R/W | 0x00000040 | 状态寄存器 |

## 全局配置寄存器（GLOBE\_CFG）

偏移量：0x000

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:14 | Reserved | 0 | R/W | Reserved |
| 13 | flash\_clk\_bypass | 0x0 | R/W | Flash器件时钟旁路，可以使控制器核Flash采用同一个外部时钟，配置为1有效。当使能此功能时，需要tx\_data\_shift发送数据相位配置为0、rx\_nege\_en和rx\_nege\_sample配置为1。 |
| 12 | dpm\_en | 0x0 | R/W | 发送的dpm模式使能，配置为1有效 |
| 11 | dtr\_tx\_nege\_en | 0x0 | R/W | DTR模式下发送数据时钟下降沿相位调整，配置为1有效 |
| 10:8 | dtr\_tx\_data\_shift | 0x0 | R/W | DTR模式下发送数据时钟相位平移的周期数 |
| 7 | dtr\_tx\_nege\_en | 0x0 | R/W | 正常模式下发送数据时钟下降沿相位调整，配置为1有效 |
| 6:4 | dtr\_tx\_data\_shift | 0x0 | R/W | 正常模式下发送数据时钟相位平移的周期数 |
| 3:2 | ram\_clk\_md | 0x0 | R/W | fifo的flash侧时钟与内部sram时钟频率关系: 0：同频 1：2分频 2：4分频 |
| 1:0 | flash\_clk\_div | 0x0 | R/W | Flash器件时钟的频率： 0：内核时钟的2分频 1：内核时钟的4分频 2：内核时钟的6分频 3：内核时钟的8分频 |

## 全局配置寄存器1（GLOBE1\_CFG）

偏移量：0x004

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:11 | Reserved | 0 | R/W | Reserved |
| 10 | flash\_R/Wdata\_maxnum\_en | 0x0 | R/W | 单次传输限制使能，配置为1有效 |
| 9:2 | flash\_R/Wdata\_maxnum | 0x0 | R/W | 单次传输最大的读写数据个数，其值为该寄存器配置值再加1个Word |
| 1 | flash\_hw\_reset | 0x0 | R/W | flash硬件复位，配置为1有效 |
| 0 | ecc\_dect\_en | 0x0 | R/W | ECC信号检测使能，配置为1有效 |

## 接收时钟配置寄存器（RX\_CLK\_CFG）

偏移量：0x08

复位值：0x00000010

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:22 | Reserved | 0 | R/W | Reserved |
| 21 | rx\_nege\_sample | 0x0 | R/W | 非DTR模式下，接收时钟使用下降沿采样，配置为1有效 |
| 20 | rx\_dqs\_en | 0x0 | R/W | 接收时钟选择外部dps信号，配置为1有效 |
| 19:4 | rx\_clk\_delay\_sel | 0x1 | R/W | delay\_line选择： [19]RX\_CLK时域，rx\_wptr相位选择 0x0:下降沿采样 0x1:上升沿采样 [18]RX\_CLK时域，rx\_wptr输出选择 0x0:原始rx\_wptr输出 0x1:相位调整rx\_wptr输出 [17]core\_clk时域，rx\_wptr相位选择 0x0:下降沿采样 0x1:上升沿采样 [16]core\_clk时域，rx\_wptr输出选择 0x0:原始rx\_wptr输出 0x1:相位调整rx\_wptr输出 [15:12]保留 [11:4]delay\_line的级数范围（1-32）级 |
| 3 | rx\_nege\_en | 0x0 | R/W | 接收时钟下降沿相位调整使能，配置为1有效 |
| 2:0 | rx\_clk\_shift | 0x0 | R/W | 接收时钟相位平移的周期数 |

## 地址屏蔽配置寄存器（ADDR\_MASK\_CFG）

偏移量：0x0C

复位值：0x00000FFF

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:12 | Reserved | 0 | R/W | Reserved |
| 11:0 | addr\_mask | 0xFFF | R/W | 配置地址的屏蔽，系统默认Flash的低16bit地址位（15:0）为有效，该寄存器每一位顺序对应Flash从第16bit地址的对应位，如bit0对应的是Flash地址的第16bit位，对该位写1为屏蔽，0为不屏蔽 例： 器件大小2MB，器件有效地址共21位，addr\_mask = 0xFE0； 器件大小4MB，器件有效地址共22位，addr\_mask = 0xFC0； 器件大小16MB，器件有效地址共24位，ddr\_mask = 0xF00； 器件大小32MB，器件有效地址共25位,addr\_mask = 0xE00。 |

## 通用模式配置寄存器（GM\_CFG）

偏移量：0x10

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:29 | Reserved | 0 | R/W | Reserved |
| 28:24 | gm\_dummy\_cycle | 0x0 | R/W | Dummy等待flash的周期数 |
| 23 | gm\_data\_store\_md | 0x0 | R/W | 读写数据存放方式： 0：fifo 1：rw\_data\_reg(不支持dma模式） |
| 22 | gm\_dma\_wreq\_md | 0x0 | R/W | DMA写请求发送方式： 0：写fifo amempty（快空状态）时发送写请求 1：写fifo empty（空状态）时发送写请求 |
| 21 | gm\_dma\_en | 0x0 | R/W | DMA模式使能，配置为1有效 |
| 20 | Reserved | 0x0 | R/W | Reserved |
| 19 | gm\_crm\_en | 0x0 | R/W | 连续读模式使能，配置为1有效 |
| 18:15 | gm\_data\_md\_sel | 0x0 | R/W | 读写数据的模式选择： 4’b0000：单线模式 4’b0001：4线模式 4’b0100：单线merge模式 4’b0101：4线merge模式 4’b1000：DTR单线模式 4’b1001：DTR4线模式 4’b1100：DTR单线merge模式 4’b1101：DTR4线merge模式 其它：Reseverd |
| 14 | gm\_write\_en | 0x0 | R/W | 写数据使能，配置为1有效 |
| 13 | gm\_read\_en | 0x0 | R/W | 读数据使能，配置为1有效 |
| 12 | gm\_dummy\_en | 0x0 | R/W | Dummy等待使能，配置为1有效 |
| 11 | gm\_crb\_en | 0x0 | R/W | 连续读bit发送使能，配置为1有效 |
| 10:7 | gm\_data\_md\_sel | 0x0 | R/W | 发送地址的模式选择： 4’b0000：单线模式 4’b0001：4线模式 4’b0100：单线merge模式 4’b0101：4线merge模式 4’b1000：DTR单线模式 4’b1001：DTR4线模式 4’b1100：DTR单线merge模式 4’b1101：DTR4线merge模式 其它：Reseverd |
| 6 | gm\_addr\_en | 0x0 | R/W | 发送地址使能，配置为1有效 |
| 5:2 | gm\_cmd\_md\_sel | 0x0 | R/W | 发送命令的模式选择： 4’b0000：单线模式 4’b0001：4线模式 4’b0100：单线merge模式 4’b0101：4线merge模式 4’b1000：DTR单线模式 4’b1001：DTR4线模式 4’b1100：DTR单线merge模式 4’b1101：DTR4线merge模式 其它：Reseverd |
| 1 | gm\_cmd\_en | 0x0 | R/W | 发送命令使能，配置为1有效 |
| 0 | gm\_en | 0x0 | R/W | 通用模式总使能，配置为1有效，写1后生效后该bit位自清除为0 |

## 通用模式配置寄存器1（GM1\_CFG）

偏移量：0x14

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:27 | Reserved | 0 | R/W | Reserved |
| 26 | gm\_cmd\_size | 0x0 | R/W | 命令的数据长度，该长度为该位的值+1个Byte |
| 25:24 | gm\_addr\_size | 0x0 | R/W | 地址的数据长度，该长度为该位的值+1个Byte |
| 23:16 | gm\_crb\_code | 0x0 | R/W | 发送的连续读bit数据 |
| 15:8 | gm\_cmd\_code1 | 0x0 | R/W | 发送的命令编码数据1 |
| 7:0 | gm\_cmd\_code0 | 0x0 | R/W | 发送的命令编码数据0 |

## 通用模式读写地址配置寄存器（GM\_ADDR\_CFG）

偏移量：0x18

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:0 | gm\_address | 0 | R/W | 通用模式读写地址 |

## 通用模式数据大小配置寄存器（GM\_DATA\_SIZE\_CFG）

偏移量：0x1C

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:20 | Reserved | 0 | R/W | Reserved |
| 19:0 | rw\_data\_size | 0 | R/W | 读写的数据字节个数，单位为Byte |

## 预取读模式配置寄存器（RD\_CFG）

偏移量：0x20

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:23 | Reserved | 0 | R/W | Reserved |
| 22 | prefetch\_en | 0 | R/W | 预取读模式总使能，配置为1有效 |
| 21:17 | rd\_dummy\_cycle | 0 | R/W | Dummy等待flash的周期 |
| 16 | Reserved | 0 | R/W | Reserved |
| 15:12 | rd\_data\_md\_sel | 0x0 | R/W | 读数据的模式选择： 4’b0000：单线模式 4’b0001：4线模式 4’b0100：单线merge模式 4’b0101：4线merge模式 4’b1000：DTR单线模式 4’b1001：DTR4线模式 4’b1100：DTR单线merge模式 4’b1101：DTR4线merge模式 其它：Reseverd |
| 11 | rd\_dummy\_en | 0 | R/W | Dummy等待使能，配置为1有效 |
| 10 | rd\_crb\_en | 0 | R/W | 连续读bit发送使能，配置为1有效 |
| 9:6 | rd\_addr\_md\_sel | 0x0 | R/W | 发送地址的模式选择： 4’b0000：单线模式 4’b0001：4线模式 4’b0100：单线merge模式 4’b0101：4线merge模式 4’b1000：DTR单线模式 4’b1001：DTR4线模式 4’b1100：DTR单线merge模式 4’b1101：DTR4线merge模式 其它：Reseverd |
| 5:2 | rd\_cmd\_md\_sel | 0x0 | R/W | 发送命令的模式选择： 4’b0000：单线模式 4’b0001：4线模式 4’b0100：单线merge模式 4’b0101：4线merge模式 4’b1000：DTR单线模式 4’b1001：DTR4线模式 4’b1100：DTR单线merge模式 4’b1101：DTR4线merge模式 其它：Reseverd |
| 1 | rd\_cmd\_en | 0 | R/W | 发送命令使能，配置为1有效 |
| 0 | rd\_en | 0 | R/W | 预取读模式总使能，配置为1有效 |

## 预期读模式配置寄存器1（RD1\_CFG）

偏移量：0x24

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:27 | Reserved | 0 | R/W | Reserved |
| 26 | rd\_cmd\_size | 0x0 | R/W | 命令的数据长度，该长度为该位的值+1个Byte |
| 25:24 | rd\_addr\_size | 0x0 | R/W | 地址的数据长度，该长度为该位的值+1个Byte |
| 23:16 | rd\_crb\_code | 0x0 | R/W | 发送的连续读bit数据 |
| 15:8 | rd\_cmd\_code1 | 0x0 | R/W | 发送的命令编码数据1 |
| 7:0 | rd\_cmd\_code0 | 0x0 | R/W | 发送的命令编码数据0 |

## FIFO级别配置寄存器（FIFO\_LEVEL\_CFG）

偏移量：0x30

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:19 | Reserved | 0 | R/W | Reserved |
| 18:13 | rfifo\_amfull\_level | 0x0 | R/W | 配置读FIFO中，amfull（几乎满）的数据个数阈值标准，达到时即触发对应条件 |
| 12:7 | rd\_arfifo\_amempty\_levelddr\_size | 0x0 | R/W | 配置读FIFO中，amempty（几乎空）的数据个数阈值标准，达到时即触发对应条件 |
| 6:0 | wfifo\_amfull\_level | 0x0 | R/W | 配置写FIFO中，amfull（几乎满）的数据个数阈值标准，达到时即触发对应条件 |

## 读写缓存寄存器0（RW\_DATA0\_CFG）

偏移量：0x34

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:0 | rw\_data\_reg0 | 0 | R/W | 读写缓存，选择数据储存为寄存器时的第一个WORD数据 |

## 读写缓存寄存器1（RW\_DATA1\_CFG）

偏移量：0x38

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:0 | rw\_data\_reg1 | 0 | R/W | 读写缓存，选择数据储存为寄存器时的第二个WORD数据 |

## 中断控制配置寄存器（INT\_CTRL\_CFG）

偏移量：0x3C

复位值：0x00000000

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:6 | Reserved | 0 | R/W | Reserved |
| 5 | hready\_error\_int\_en | 0 | R/W | hready错误中断使能，配置为1有效 |
| 4 | ecc\_error\_int\_en | 0 | R/W | ECC错误中断使能，配置为1有效 |
| 3 | gm\_done\_int\_en | 0 | R/W | 通用模式处理完成中断使能，配置为1有效 |
| 2 | prefetch\_md\_close\_int\_en | 0 | R/W | 预取模式命令完成中断使能，配置为1有效 |
| 1 | rfifo\_under\_flow\_int\_en | 0 | R/W | 读FIFO低于预设值中断使能，配置为1有效 |
| 0 | wfifo\_overflow\_int\_en | 0 | R/W | 写FIFO高于预设值中断使能，配置为1有效 |

## 状态寄存器（STATE\_REG）

偏移量：0x40

复位值：0x00000040

| 位域 | 名称 | 复位值 | 类型 | 描述 |
| --- | --- | --- | --- | --- |
| 31:9 | Reserved | 0 | R/W | Reserved |
| 8 | main\_ctrl\_busy | 0 | R | 主控制忙状态，只读，1为忙 |
| 7 | arbt\_busy | 0 | R | arbt忙状态，只读，1为忙 |
| 6 | data\_if\_hready | 1 | R | 数据处于hready错状态，只读，1为该状态 |
| 5 | hready\_error\_int | 0 | R/W | hready错误中断状态，1为触发中断，该位写1清除 |
| 4 | ecc\_error\_int | 0 | R/W | ECC错误中断状态，1为触发中断，该位写1清除 |
| 3 | gm\_done\_int | 0 | R/W | 通用模式处理完成中断状态，1为触发中断，该位写1清除 |
| 2 | prefetch\_md\_close\_int | 0 | R/W | 预取模式命令完成中断状态，1为触发中断，该位写1清除 |
| 1 | rfifo\_under\_flow\_int | 0 | R/W | 读FIFO低于预设值中断状态，1为触发中断，该位写1清除 |
| 0 | wfifo\_overflow\_int | 0 | R/W | 写FIFO高于预设值中断状态，1为触发中断，该位写1清除 |