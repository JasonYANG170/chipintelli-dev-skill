<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/timer/ -->

# 通用定时器(TIMER)

---

## 1、简介

* TIMER(通用定时器)是一个基于可配置分频器和多种计数方式32位递减计数器，在计数值达到0时触发一个定时事件，常用于在指定的时间间隔内反复触发指定窗口的定时器事件，可作为周期性中断发生器、事件计数器使用。
* CI13XX支持4个专用TIMER：TIMER0~TIMER3，支持TIMER级联，PWM4和PWM5也可以当作TIMER功能使用。

## 2、特性

* 支持三种计数模式，通过API(timer\_init、timer\_set\_mode)进行配置：单周期计数模式、自动重装载计数模式、自由运行计数模式；
* 单周期计数模式：定时器仅计数一个计数周期；
* 自动重装载计数模式：计数器在每个计数结束时重新初始化；
* 自由运行计数模式：计数值在每次计数结束时从0xFFFFFFFF循环到0x00000000；
* 支持计数时钟分频，支持1、2、4、16分频，通过API(timer\_init)进行配置；
* 支持32位递减计数器，可读取计数器的实时值，通过API(timer\_get\_count)进行读取；
* 支持级联模式配置，通过API(timer\_cascade\_set)进行配置；
* 支持计数完成上报中断，中断清除方式具体参见API(timer\_clear\_irq)；

---

## 3、API列表

| 函数名 | 描述 |
| --- | --- |
| timer\_init | 初始化TIMER设备 |
| timer\_start | 启动TIMER设备 |
| timer\_stop | 停止TIMER设备 |
| timer\_set\_mode | 设置TIMER设备计数模式 |
| timer\_event\_start | TIMER设备触发事件计数 |
| timer\_set\_count | TIMER设置计时周期 |
| timer\_get\_count | TIMER设备获取计数值 |
| timer\_cascade\_set | TIMER设备设置级联模式 |
| timer\_clear\_irq | TIMER中断清除 |

## 4、TIMER相关说明

| TIMER名 | 中断名 | 中断服务函数 | 描述 |
| --- | --- | --- | --- |
| TIMER0 | TIMER0\_IRQn | TIMER0\_IRQHandler |  |
| TIMER1 | TIMER1\_IRQn | TIMER1\_IRQHandler |  |
| TIMER2 | TIMER2\_IRQn | TIMER2\_IRQHandler |  |
| TIMER3 | TIMER3\_IRQn | TIMER3\_IRQHandler |  |
| AON\_TIMER0 | AON\_TIM\_INT0\_IRQn | AON\_TIM\_INT0\_IRQHandler | HAL\_PWM4\_BASE 可当做 AON\_TIMER0 |
| AON\_TIMER1 | AON\_TIM\_INT1\_IRQn | AON\_TIM\_INT0\_IRQHandler | HAL\_PWM5\_BASE 可当做 AON\_TIMER1 |

---

## 5、示例

TIMER中断服务函数是wake类型，可以在其他任意位置定义该函数：

```
#include "ci130x_timer.h"
#include "ci_log.h"

//TIMER0中断服务函数定义
void TIMER0_IRQHandler(void)
{
    mprintf("timer0 irq\n");
    timer_clear_irq(TIMER0);
}

//TIMER1中断服务函数参考TIMER0
//TIMER2中断服务函数参考TIMER0
//TIMER3中断服务函数参考TIMER0
//AON_TIMER0中断服务函数参考TIMER0
//AON_TIMER1中断服务函数参考TIMER0
```

注意

配置中断宽度为 timer\_iqr\_width\_f 时，如果使能了中断，必须调用接口 timer\_clear\_irq 清除中断。其他中断宽度，硬件会自清中断。

以下代码配置并启动TIMER0，传参1时，表示1S来一次定时器中断：

```
#include "ci130x_time.h"
#include "ci130x_timer.h"
#include "ci130x_core_eclic.h"

void timer_s(uint32_t s)
{
    /*TIMER0控制器时钟配置*/
    scu_set_device_gate(HAL_TIMER0_BASE,ENABLE);
    /*使能TIMER0中断*/
    eclic_irq_enable(TIMER0_IRQn);
    /*TIMER0初始化*/
    timer_init_t init;
    init.mode = timer_count_mode_auto;      //自动重新计数模式
    init.div = timer_clk_div_0;             //不分频
    init.width = timer_iqr_width_f;         //该中断宽度，需调用timer_clear_irq清除中断
    init.count = get_apb_clk() * s;         //计时多少s的count值
    timer_init(TIMER0,init);
    /*TIMER0启动计时*/
    timer_start(TIMER0);
}
```

以下代码配置并启动TIMER0，传参1时，表示1ms来一次定时器中断：

```
#include "ci130x_time.h"
#include "ci130x_timer.h"
#include "ci130x_core_eclic.h"

void timer_ms(uint32_t ms)
{
    /*TIMER0控制器时钟配置*/
    scu_set_device_gate(HAL_TIMER0_BASE,ENABLE);
    /*使能TIMER0中断*/
    eclic_irq_enable(TIMER0_IRQn);
    /*TIMER0初始化*/
    timer_init_t init;
    init.mode = timer_count_mode_auto;          //自动重新计数模式
    init.div = timer_clk_div_0;                 //不分频
    init.width = timer_iqr_width_f;             //该中断宽度，需调用timer_clear_irq清除中断
    init.count = (get_apb_clk() / 1000) * ms;   //计时多少ms的count值
    timer_init(TIMER0,init);
    /*TIMER0启动计时*/
    timer_start(TIMER0);
}
```

以下代码配置并启动TIMER0，传参1时，表示1us来一次定时器中断：

```
#include "ci130x_time.h"
#include "ci130x_timer.h"
#include "ci130x_core_eclic.h"

void timer_us(uint32_t us)
{
    /*TIMER0控制器时钟配置*/
    scu_set_device_gate(HAL_TIMER0_BASE,ENABLE);
    /*使能TIMER0中断*/
    eclic_irq_enable(TIMER0_IRQn);
    /*TIMER0初始化*/
    timer_init_t init;
    init.mode = timer_count_mode_auto;            //自动重新计数模式
    init.div = timer_clk_div_0;                   //不分频
    init.width = timer_iqr_width_f;               //该中断宽度，需调用timer_clear_irq清除中断
    init.count = (get_apb_clk() / 1000000) * us;  //计时多少us的count值
    timer_init(TIMER0,init);
    /*TIMER0启动计时*/
    timer_start(TIMER0);
}
```

警告

CI13XX系列芯片内置32位计数器，计数范围：0x0 - 0xFFFFFFFF；如果需要更长时间的定时设置，需要对定时器进行分频操作，或使用级联模式。

---

## 6、常见问题

* TIMER定时范围(TIMER模块基础时钟CLK\_S = 120000000Hz)

分频系数：DIV（取值范围：1，2，4，16）

计数时钟：CLK = CLK\_S / DIV 单位：Hz

最小定时：MIN = 1s / CLK 单位：S

最大定时：MAX = 0xFFFFFFFF \* MIN 单位：S

以上做法，最大定时9分钟左右，若需要更长时间定时，请使用定时器级联模式。以下对级联模式进行描述：

TODO:

| 基础定时器 | 级联定时器 |
| --- | --- |
| TIMER0 | TIMER1\2\3 |
| TIMER1 | TIMER2\3 |
| TIMER2 | TIMER3 |
| AON\_TIMER0 | AON\_TIMER1 |

举个例子：
我们现在将TIMER0作为基础定时器，将时钟分频系数配置为16（这里追求定时最大化），TIMER1、TIMER2、TIMER3都配置为级联模式，且TIMER1、TIMER2、TIMER3的计数值都配置为最大值（0xFFFFFFFF），那么每一个TIMER定时是多久呢？

| 定时器 | 定时时间 |
| --- | --- |
| TIMER0 | 13分钟左右 |
| TIMER1 | 0xFFFFFFFF \* 13分钟左右 |
| TIMER2 | 0xFFFFFFFF \* 0xFFFFFFFF \* 13分钟左右 |
| TIMER3 | 0xFFFFFFFF \* 0xFFFFFFFF \* 0xFFFFFFFF \* 13分钟左右 |
| AON\_TIMER0 | 13分钟左右 |
| AON\_TIMER1 | 0xFFFFFFFF \* 13分钟左右 |

## 7、API 参考