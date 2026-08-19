<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/%E5%8D%95%E6%80%BB%E7%BA%BF/ -->

# 单总线(ONE WIRE)

---

## 1. 概述

SDK内置组件中单总线组件为基础组件，软件实现通用单总线协议，使用硬件资源一个硬件TIMER一个GPIO口；方便应用开发人员快速对接单总线设备，单总线要求外接一个约5K的上拉电阻，这样当总线空闲时，总线状态为高电平。

---

## 2. 使用说明

* 相关宏配置说明如下：

```
/*-----------------------------------------------------------------------------
                            define
-----------------------------------------------------------------------------*/
/* 用到的硬件TIMER定义 */
#define ONE_WIRE_TIMER (TIMER0)
#define ONE_WIRE_TIMER_IRQ (TIMER0_IRQn)
#define ONE_WIRE_TIMER_US (get_apb_clk()/1000000)
/* 发送复位信号延时定义 */
#define ONE_WIRE_SEND_RESET_TIMER_US (480)/* 发送复位时间 */
#define ONE_WIRE_READ_RESPOND_TIMER_US (70)/* 读取响应时间 */
#define ONE_WIRE_RESPOND_CONTINUE_TIMER_US (480)/* 响应持续时间 */
/* 写数据信号延时定义 */
#define ONE_WIRE_WRITE_PREPARE_TIMER_US (15)/* 写数据准备时间 */
#define ONE_WIRE_WRITE_TIMER_US (30)/* 写数据时间 */
#define ONE_WIRE_WRITE_CONTINUE_TIMER_US (65)/* 写数据持续时间 */
/* 读数据信号延时定义 */
#define ONE_WIRE_READ_PREPARE_TIMER_US (3)/* 读数据准备时间 */
#define ONE_WIRE_READ_TIMER_US (13)/* 读数据时间 */
#define ONE_WIRE_READ_CONTINUE_TIMER_US (63)/* 读数据持续时间 */
```

* 相关API接口如下：

| 函数 | 描述 |
| --- | --- |
| one\_wire\_init | 单总线初始化 |
| one\_wire\_write | 单总线写数据 |
| one\_wire\_read | 单总线读数据 |
| one\_wire\_timer\_handler | 单总线timer中断处理函数 |