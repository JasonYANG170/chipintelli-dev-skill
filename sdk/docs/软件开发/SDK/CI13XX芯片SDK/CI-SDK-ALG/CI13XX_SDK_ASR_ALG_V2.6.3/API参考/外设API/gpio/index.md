<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/gpio/ -->

# 通用输入输出接口(GPIO)

---

## 1、简介

* GPIO是一种通用的输入输出接口，允许芯片与外围硬件进行电平信号交互，其既可以作为输入接收外部信号，也可以作为输出控制外围硬件。

---

## 2、特性

* CI13XX支持多个可编程的输入/输出管脚，每个GPIO端口都有相应的控制寄存器和配置寄存器，可单独打开或关闭每个GPIO管脚，实现对外围硬件的精准控制和状态监测。CI13XX支持4组GPIO(GPIO0、GPIO1、GPIO2、GPIO3)，其中GPIO0对应的是芯片PA口，GPIO1对应的是芯片PB口，GPIO2对应的是芯片PC口，GPIO3对应的是芯片PD口。
* GPIO支持输入输出状态查询、中断屏蔽、中断清除、中断状态查询，支持中断触发方式可配置（低电平触发、高电平触发、上升沿触发、下降沿触发、双边沿触发），以满足不同的应用场景需求。

---

## 3、API列表

* 以下API可同时设置一个或多个PIN

| 函数名 | 描述 |
| --- | --- |
| gpio\_set\_output\_mode | GPIO设备输出模式配置 |
| gpio\_set\_input\_mode | GPIO设备输入模式配置 |
| gpio\_get\_direction\_status | GPIO设备获取IO方向 |
| gpio\_irq\_mask | GPIO设备中断屏蔽 |
| gpio\_irq\_unmask | GPIO设备取消中断屏蔽 |
| gpio\_irq\_trigger\_config | GPIO设备设置中断触发方式 |
| gpio\_set\_output\_high\_level | GPIO设备输出高电平 |
| gpio\_set\_output\_low\_level | GPIO设备输出低电平 |
| gpio\_get\_input\_level | GPIO设备获取输入电平 |

* 以下API用于单独设置一个PIN

| 函数名 | 描述 |
| --- | --- |
| gpio\_get\_direction\_status\_single | GPIO设备获取一个管脚输入输出方向 |
| gpio\_get\_irq\_raw\_status\_single | GPIO设备获取一个管脚中断屏蔽前状态 |
| gpio\_get\_irq\_mask\_status\_single | GPIO设备获取一个管脚中断屏蔽后状态 |
| gpio\_clear\_irq\_single | GPIO设备控制一个管脚清除中断 |
| gpio\_set\_output\_level\_single | GPIO设备控制一个管脚输出 |
| gpio\_get\_input\_level\_single | GPIO设备获取一个管脚输入 |

---

## 4、特别说明

* PA0、PA1这两个引脚是连接晶振的，默认是模拟功能。
* PC1、PC2、PC3、PC4这4个引脚默认是模拟功能。
* 上述模拟功能引脚，当作GPIO时需要特殊配置。以下分【普通IO示例】、【晶振IO示例】、【模拟IO示例】说明。

## 5、普通IO示例

### 5.1、普通IO输出模式

* 以下代码控制PB组的pin4输出模式

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"

void gpio_output_test()
{
    /*PB组GPIO控制器时钟配置*/
    scu_set_device_gate((unsigned int)PB,ENABLE);        //开启PB时钟

    /*PB4引脚初始化*/
    dpmu_set_io_reuse(PB4,FIRST_FUNCTION);               //设置引脚功能复用为GPIO
    dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_OUTPUT); //设置引脚功能为输出模式
    dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);          //设置关闭上下拉

    /*PB的pin_4，配置GPIO输出模式*/
    gpio_set_output_mode(PB,pin_4);                      //GPIO的pin脚配置成输出模式

    /*PB的pin_4，输出电平*/
    gpio_set_output_level_single(PB,pin_4,0);            //输出低电平
    gpio_set_output_level_single(PB,pin_4,1);            //输出高电平
}
```

### 5.2、普通IO开漏输出模式

* 以下代码配置PB组的pin4为开漏输出模式

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"

void gpio_open_drain_output_test()
{
    /*PB组GPIO控制器时钟配置*/
    scu_set_device_gate((unsigned int)PB,ENABLE);       //开启PB时钟

    /*PB4引脚初始化，使能开漏功能*/
    dpmu_set_io_reuse(PB4,FIRST_FUNCTION);              //设置引脚功能复用为GPIO
    dpmu_set_io_open_drain(PB4,ENABLE);                 //配置引脚开漏功能，支持外部上拉5V
    dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);         //关闭上拉
    dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_OUTPUT);//设置引脚功能为输出模式

    /*PB的pin_4，配置GPIO输出模式*/
    gpio_set_output_mode(PB,pin_4);                     //GPIO的pin脚配置成输出模式

    /*PB的pin_4，输出电平*/
    gpio_set_output_level_single(PB,pin_4,0);            //输出低电平
    gpio_set_output_level_single(PB,pin_4,1);            //输出高电平
}
```

### 5.3、普通IO输入模式

* 以下代码配置PB组的pin4为输入模式

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"

void gpio_input_test()
{
    /*PB组GPIO控制器时钟配置*/
    scu_set_device_gate((unsigned int)PB,ENABLE);       //开启PB时钟

    /*PB4引脚初始化*/
    dpmu_set_io_reuse(PB4,FIRST_FUNCTION);              //设置引脚功能复用为GPIO
    dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_INPUT); //设置引脚功能为输入模式
    dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);         //设置关闭上下拉

    /*PB的pin_4，配置GPIO输入模式*/
    gpio_set_input_mode(PB,pin_4);                      //GPIO的pin脚配置成输入模式

    /*检测PB的pin_4，输入电平*/
    if(0 == gpio_get_input_level_single(PB,pin_4))      //获取IO电平
    {
        //INFO:输入为低电平
    }
    else
    {
        //INFO:输入为高电平
    }
}
```

### 5.4、普通IO中断模式

* 以下代码配置PB组的pin4的中断为双边沿触发

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"
#include "ci130x_core_eclic.h"

void gpio_interrupt_test()
{
    /*PB组GPIO控制器时钟配置*/
    scu_set_device_gate((unsigned int)PB,ENABLE);          //开启PB时钟

    /*PB4引脚初始化*/
    dpmu_set_io_reuse(PB4,FIRST_FUNCTION);                 //设置引脚功能复用为GPIO
    dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_INPUT);    //设置引脚功能为输入模式
    dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);            //设置关闭上下拉

    /*PB的pin_4，配置GPIO输入模式，开启中断*/
    gpio_set_input_mode(PB,pin_4);                         //GPIO的pin脚配置成输入模式
    gpio_irq_trigger_config(PB,pin_4,both_edges_trigger);  //中断触发方式
    eclic_irq_enable(PB_IRQn);                             //使能中断
}

/*PB的中断服务函数已在ci130x_gpio.c中定义，名为*/
//void PB_IRQHandler(void);
```

警告

PA、PB、PC组的IO有输入、输出、中断功能，
PD组只有输入、输出功能，无中断功能。

## 6、晶振IO示例

PA0、PA1这两个引脚是连接晶振的，默认是模拟功能，用作GPIO需要先关闭晶振功能，再配置成数字功能

### 6.1、晶振IO输出模式

* 以下代码控制PA组的PA0为输出模式，PA1可以参考PA0的示例配置

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"

void gpio_output_test()
{
    /*使能晶振IO用作GPIO功能*/
    dpmu_osc_pad_for_gpio(ENABLE);              //PA0和PA1用作GPIO，必须把晶振功能关闭

    /*PA组GPIO控制器时钟配置*/
    scu_set_device_gate(PA,ENABLE);             //开启PA时钟

    /*PA0引脚初始化*/
    dpmu_set_io_reuse(PA0,FIRST_FUNCTION);      //初始化为GPIO功能
    dpmu_set_adio_reuse(PA0,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
    dpmu_set_io_direction(PA0,DPMU_IO_DIRECTION_OUTPUT);  //初始化引脚为输出模式

    /*PA的pin_0，配置GPIO输出模式*/
    gpio_set_output_mode(PA,pin_0);             //初始化PA的pin_0为输出模式

    /*PA的pin_0，输出电平*/
    gpio_set_output_high_level(PA,pin_0);       //输出高电平
    gpio_set_output_low_level(PA,pin_0);        //输出低电平
}
```

### 6.2、晶振IO输入模式

* 以下代码控制PA组的PA0为输入模式，PA1可以参考PA0的示例配置

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"

void gpio_input_test()
{
    /*使能晶振IO用作GPIO功能*/
    dpmu_osc_pad_for_gpio(ENABLE);              //PA0和PA1用作GPIO，必须把晶振功能关闭

    /*PA组GPIO控制器时钟配置*/
    scu_set_device_gate(PA,ENABLE);             //开启PA时钟

    /*PA0引脚初始化*/
    dpmu_set_io_reuse(PA0,FIRST_FUNCTION);      //初始化为GPIO功能
    dpmu_set_adio_reuse(PA0,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
    dpmu_set_io_direction(PA0,DPMU_IO_DIRECTION_INPUT);  //初始化引脚为输入模式

    /*PA的pin_0，配置GPIO输入模式*/
    gpio_set_input_mode(PA,pin_0);              //初始化PA的pin_0为输入模式

    /*检测PA的pin_0，输入电平*/
    if(0 == gpio_get_input_level_single(PA,pin_0)) //获取IO电平
    {
        //INFO:输入为低电平
    }
    else
    {
        //INFO:输入为高电平
    }
}
```

### 6.3、晶振IO中断模式

以下代码控制PA组的PA0的中断为双边沿触发，PA1可以参考PA0的示例配置

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"
#include "ci130x_core_eclic.h"

void gpio_interrupt_test()
{
    /*使能晶振IO用作GPIO功能*/
    dpmu_osc_pad_for_gpio(ENABLE);              //PA0和PA1用作GPIO，必须把晶振功能关闭

    /*PA组GPIO控制器时钟配置*/
    scu_set_device_gate(PA,ENABLE);             //开启PA时钟

    /*PA0引脚初始化*/
    dpmu_set_io_reuse(PA0,FIRST_FUNCTION);      //初始化为GPIO功能
    dpmu_set_adio_reuse(PA0,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
    dpmu_set_io_direction(PA0,DPMU_IO_DIRECTION_INPUT);  //初始化引脚为输入模式

    /*PA的pin_0，配置GPIO输入模式，开启中断*/
    gpio_set_input_mode(PA,pin_0);              //初始化PA的pin_0为输入模式
    gpio_irq_trigger_config(PA,pin_0,both_edges_trigger);  //中断触发方式
    eclic_irq_enable(PA_IRQn);                  //使能中断

    /*PA的中断服务函数已在ci130x_gpio.c中定义，名为*/
    //void PA_IRQHandler(void);
}
```

## 7、模拟IO示例

PC1、PC2、PC3、PC4这4个引脚默认是模拟功能，用作GPIO需要配置成数字功能

### 7.1、模拟IO输出模式

* 以下代码控制PC组的PC1为输出模式

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"

void gpio_output_test()
{
    /*PC组GPIO控制器时钟配置*/
    scu_set_device_gate(PC,ENABLE);             //开启PC时钟

    /*PC1引脚初始化*/
    dpmu_set_io_reuse(PC1,SECOND_FUNCTION);      //初始化为GPIO功能
    dpmu_set_adio_reuse(PC1,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
    dpmu_set_io_direction(PC1,DPMU_IO_DIRECTION_OUTPUT);  //初始化引脚为输出模式

    /*PC的pin_1，配置GPIO输出模式*/
    gpio_set_output_mode(PC,pin_1);             //初始化GPIOC的pin_1为输出模式

    /*PC的pin_1，输出电平*/
    gpio_set_output_high_level(PC,pin_1);       //输出高电平
    gpio_set_output_low_level(PC,pin_1);        //输出低电平
}
```

### 7.2、模拟IO输入模式

* 以下代码控制PC组的PC1为输入模式

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"

void gpio_input_test()
{
    /*PC组GPIO控制器时钟配置*/
    scu_set_device_gate(PC,ENABLE);             //开启PC时钟

    /*PC的pin_1，配置GPIO输入模式*/
    dpmu_set_io_reuse(PC1,SECOND_FUNCTION);      //初始化为GPIO功能
    dpmu_set_adio_reuse(PC1,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
    dpmu_set_io_direction(PC1,DPMU_IO_DIRECTION_INPUT);  //初始化引脚为输入模式

    /*PC的pin_1，配置GPIO输入模式*/
    gpio_set_input_mode(PC,pin_1);              //初始化PC的pin_1为输入模式

    /*检测PC的pin_1，输入电平*/
    if(0 == gpio_get_input_level_single(PC,pin_1))       //获取IO电平
    {
        //INFO:输入为低电平
    }
    else
    {
        //INFO:输入为高电平
    }
}
```

### 7.3、模拟IO中断模式

* 以下代码控制PC组的PC1的中断为双边沿触发

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"
#include "ci130x_core_eclic.h"

void gpio_interrupt_test()
{
    /*PC组GPIO控制器时钟配置*/
    scu_set_device_gate(PC,ENABLE);             //开启PC时钟

    /*PC1引脚初始化*/
    dpmu_set_io_reuse(PC1,SECOND_FUNCTION);      //初始化为GPIO功能
    dpmu_set_adio_reuse(PC1,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
    dpmu_set_io_direction(PC1,DPMU_IO_DIRECTION_INPUT);  //初始化引脚为输入模式

    /*PC的pin_1，配置GPIO输入模式，开启中断*/
    gpio_set_input_mode(PC,pin_1);              //初始化PC的pin_1为输入模式
    gpio_irq_trigger_config(PC,pin_1,both_edges_trigger);  //中断触发方式
    eclic_irq_enable(PC_IRQn);                  //使能中断

    /*PC的中断服务函数已在ci130x_gpio.c中定义，名为*/
    //void AON_PC_IRQHandler(void);
}
```

## 8、API 参考