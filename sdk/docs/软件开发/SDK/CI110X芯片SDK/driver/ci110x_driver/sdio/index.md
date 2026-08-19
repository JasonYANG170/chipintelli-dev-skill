<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/driver/ci110x_driver/sdio/ -->

# 安全数字输入\输出接口(SDIO)

---

## SDIO简介

SDIO(Secure Digital Input and Output)是在SD标准上定义了一种外设接口，目前来讲主要应用于可移动外部存储设备卡和无线wifi或蓝牙模块上。在CI110X系列芯片上包含一组SDIO外设，支持标准SD3.0协议，针对sd卡的应用，SDK内已实现ci110x\_sdc.c驱动和fatfs移植，将不再赘述。本文着重说明SDIO用于对接wifi模块等设备时的注意事项。需要注意的是，CI110X系列芯片中的sdio同一时间仅能支持一个设备，作为sd存储设备底层驱动为ci110x\_sdc.c,作为sdio设备驱动为ci110x\_sdio.c。

---

## 移植指南

sdio卡在协议中需要支持命令主要包括

| 命令 | 说明 |
| --- | --- |
| CMD0 | 复位总线命令 |
| CMD5 | sdio卡电压识别 |
| CMD3 | 请求从机RCA |
| CMD7 | 选中从机 |
| CMD52 | 寄存器fn读写 |
| CMD53 | 数据读写 |

在驱动中包括以下函数，调用即可完成sdio总线的卡枚举过程（默认情况下会开启四线模式）：

* void SDIOInit(void)

完成上述总线初始化过程后，往往不同的wifi模块针对不同fn功能区有不同的初始化要求，驱动提供了以下函数供与wifi模块数据交换：

* uint8\_t SDIO\_send\_CMD52(uint8\_t func, uint32\_t addr, uint8\_t value, sdio\_cmd\_type flag)
* uint8\_t SDIO\_send\_CMD53(uint8\_t func, uint32\_t addr, void \*data, uint32\_t size, uint16\_t block\_num, sdio\_cmd\_type flag)

大多数sdio卡需要支持d1中断响应，在本驱动内提供了以下函数用于中断使能控制和中断服务函数注册：

* void set\_sdio\_int(int8\_t enable)
* void SDIO\_reg\_dev\_irq\_handler(SDIO\_DEV\_IRQ\_HANDLER handler)

---

## 使用示例

在SDK内的third\_device\_driver目录内容提供了 MARVELL 88w8801 sdio wifi驱动 和 Realtek 8189FTV sdio wifi驱动，其中包括上述说明中的函数移植示例，可供参考。

---

## 常见问题

* CI110X系列芯片中的SDC和SDIO属于同一外设，同一时间仅能做为其中一种功能；
* 目前SDIO总线仅支持挂载一个设备，不支持多设备挂载。