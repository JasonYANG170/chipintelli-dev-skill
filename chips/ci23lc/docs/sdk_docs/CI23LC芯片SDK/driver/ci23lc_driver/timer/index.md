<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/timer/ -->

# 通用定时器(TIMER)

---

## 简介

TIMER(定时器)功能是在指定的时间间隔内反复触发指定窗口的定时器事件。

---

## API列表

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

## TIMER相关说明

| TIMER名 | 中断名 | 中断服务函数 | 描述 |
| --- | --- | --- | --- |
| TIMER0 | TIMER0\_IRQn | TIMER0\_IRQHandler |  |
| TIMER1 | TIMER1\_IRQn | TIMER1\_IRQHandler |  |

---

## 示例

TIMER中断服务函数是wake类型，可以在其他任意位置定义该函数：

```
//TIMER0中断服务函数定义
void TIMER0_IRQHandler(void)
{
    mprintf("timer0 irq\n");
    timer_clear_irq(TIMER0);
}

//TIMER1中断服务函数参考TIMER0
```

注意

配置中断宽度为 timer\_iqr\_width\_f 时，如果使能了中断，必须调用接口 timer\_clear\_irq 清除中断。其他中断宽度，硬件会自清中断。

以下代码配置并启动TIMER0，1S来一次定时器中断：

```
void timer_s(uint32_t s)
{
    eclic_irq_enable(TIMER0_IRQn);
    scu_set_device_gate(HAL_TIMER0_BASE,ENABLE);
    timer_init_t init;
    init.mode = timer_count_mode_auto;
    init.div = timer_clk_div_0;
    init.width = timer_iqr_width_f;
    init.count = get_apb_clk() * s;
    timer_init(TIMER0,init);
    timer_start(TIMER0);
}
```

以下代码配置并启动TIMER0，1ms来一次定时器中断：

```
void timer_ms(uint32_t ms)
{
    eclic_irq_enable(TIMER0_IRQn);
    scu_set_device_gate(HAL_TIMER0_BASE,ENABLE);
    timer_init_t init;
    init.mode = timer_count_mode_auto;
    init.div = timer_clk_div_0;
    init.width = timer_iqr_width_f;
    init.count = (get_apb_clk() / 1000) * ms;
    timer_init(TIMER0,init);
    timer_start(TIMER0);
}
```

以下代码配置并启动TIMER0，1us来一次定时器中断：

```
void timer_us(uint32_t us)
{
    eclic_irq_enable(TIMER0_IRQn);
    scu_set_device_gate(HAL_TIMER0_BASE,ENABLE);
    timer_init_t init;
    init.mode = timer_count_mode_auto;
    init.div = timer_clk_div_0;
    init.width = timer_iqr_width_f;
    init.count = (get_apb_clk() / 1000000) * us;
    timer_init(TIMER0,init);
    timer_start(TIMER0);
}
```

警告

CI23LC系列芯片内置32位计数器，计数范围：0x0 - 0xFFFFFFFF；如果需要更长时间的定时设置，需要对定时器进行分频操作，或使用级联模式。

---

## 常见问题

* TIMER定时范围(TIMER模块基础时钟CLK\_S = 120000000Hz)

分频系数：DIV（取值范围：1，2，4，16）

计数时钟：CLK = CLK\_S / DIV 单位：Hz

最小定时：MIN = 1s / CLK 单位：S

最大定时：MAX = 0xFFFFFFFF \* MIN 单位：S

以上做法，最大定时9分钟左右，若需要更长时间定时，请使用定时器级联模式。以下对级联模式进行描述：

TODO:

| 基础定时器 | 级联定时器 |
| --- | --- |
| TIMER0 | TIMER1 |

举个例子：
我们现在将TIMER0作为基础定时器，将时钟分频系数配置为16（这里追求定时最大化），TIMER1配置为级联模式，且TIMER1的计数值配置为最大值（0xFFFFFFFF），那么每一个TIMER定时是多久呢？

| 定时器 | 定时时间 |
| --- | --- |
| TIMER0 | 13分钟左右 |
| TIMER1 | 0xFFFFFFFF \* 13分钟左右 |