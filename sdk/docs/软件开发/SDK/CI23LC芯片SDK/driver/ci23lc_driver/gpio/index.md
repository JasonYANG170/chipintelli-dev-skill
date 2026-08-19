<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/gpio/ -->

# 通用输入输出(GPIO)

---

## 简介

GPIO(通用IO端口)每个GPIO端口都有相应的控制寄存器和配置寄存器。API分为两类：一类是单独操作一个pin，另一类是可同时操作一个或多个pin。提供IO输入输出状态查询接口，中断屏蔽接口、中断屏蔽查询接口、中断清除接口、中断状态查询接口、中断触发方式配置接口（可配置为：低电平触发、高电平触发、上升沿触发、下降沿触发、双边沿触发）等，以满足不同应用的要求。
CI23LC系列芯片现在有PA、PB、PC共3组GPIO，PA、PB、PC每组的pin脚详见ci13642.h。

---

## API列表

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

## 特别说明

* PA0、PA1这两个引脚是连接晶振的，默认是模拟功能，配置会和其他IO不一样，以下分【普通IO示例】和【晶振IO示例】说明。

## 普通IO示例

以下代码控制PB组的pin4输出模式

```
scu_set_device_gate((unsigned int)PA,ENABLE);        //开启PB时钟
dpmu_set_io_reuse(PA7,FIRST_FUNCTION);               //设置引脚功能复用为GPIO
dpmu_set_io_direction(PA7,DPMU_IO_DIRECTION_OUTPUT); //设置引脚功能为输出模式
dpmu_set_io_pull(PA7,DPMU_IO_PULL_DISABLE);          //设置关闭上下拉
gpio_set_output_mode(PA,pin_7);                      //GPIO的pin脚配置成输出模式
gpio_set_output_level_single(PA,pin_7,0);            //输出低电平
gpio_set_output_level_single(PA,pin_7,1);            //输出高电平
```

以下代码配置PB组的pin4为开漏输出模式

```
scu_set_device_gate((unsigned int)PA,ENABLE);       //开启PB时钟
dpmu_set_io_reuse(PA7,FIRST_FUNCTION);              //设置引脚功能复用为GPIO
dpmu_set_io_open_drain(PA7,ENABLE);                 //配置引脚开漏功能，支持外部上拉5V
dpmu_set_io_pull(PA7,DPMU_IO_PULL_DISABLE);         //关闭上拉
dpmu_set_io_direction(PA7,DPMU_IO_DIRECTION_OUTPUT);//设置引脚功能为输出模式
gpio_set_output_mode(PA,pin_7);                     //GPIO的pin脚配置成输出模式
```

以下代码配置IIC口（PA2/PA3）为开漏输出模式

```
dpmu_set_io_reuse(PA2,THIRD_FUNCTION);              //设置引脚功能复用为IIC
dpmu_set_io_reuse(PA3,THIRD_FUNCTION);              //设置引脚功能复用为IIC
dpmu_set_io_open_drain(PA2,ENABLE);                 //配置引脚开漏功能，支持外部上拉5V
dpmu_set_io_open_drain(PA3,ENABLE);                 //配置引脚开漏功能，支持外部上拉5V
dpmu_set_io_pull(PA2,DPMU_IO_PULL_DISABLE);         //关闭上拉
dpmu_set_io_pull(PA3,DPMU_IO_PULL_DISABLE);         //关闭上拉
dpmu_set_io_direction(PA3,DPMU_IO_DIRECTION_OUTPUT);//SCL配置成输出
```

以下代码配置PB组的pin4为输入模式

```
scu_set_device_gate((unsigned int)PA,ENABLE);       //开启PB时钟
dpmu_set_io_reuse(PA7,FIRST_FUNCTION);              //设置引脚功能复用为GPIO
dpmu_set_io_direction(PA7,DPMU_IO_DIRECTION_INPUT); //设置引脚功能为输入模式
dpmu_set_io_pull(PA7,DPMU_IO_PULL_DISABLE);         //设置关闭上下拉
gpio_set_input_mode(PA,pin_7);                      //GPIO的pin脚配置成输入模式
if(0 == gpio_get_input_level_single(PA,pin_7))      //获取IO电平
{
    //INFO:输入为低电平
}
else
{
    //INFO:输入为高电平
}
```

以下代码配置PB组的pin4的中断为双边沿触发

```
scu_set_device_gate((unsigned int)PA,ENABLE);          //开启PB时钟
dpmu_set_io_reuse(PA7,FIRST_FUNCTION);                 //设置引脚功能复用为GPIO
dpmu_set_io_direction(PA7,DPMU_IO_DIRECTION_INPUT);    //设置引脚功能为输入模式
dpmu_set_io_pull(PA7,DPMU_IO_PULL_DISABLE);            //设置关闭上下拉
gpio_set_input_mode(PA,pin_7);                         //GPIO的pin脚配置成输入模式
gpio_irq_trigger_config(PA,pin_7,both_edges_trigger);  //中断触发方式
eclic_irq_enable(PA_IRQn);    //使能中断
```

## 晶振IO示例

以下代码控制PA组的PA0、PA1为输出模式

```
dpmu_osc_pad_for_gpio(ENABLE);              //PA0和PA1用作GPIO，必须把晶振功能关闭

//PA0初始化
scu_set_device_gate(PA,ENABLE);             //开启GPIOA时钟
dpmu_set_io_reuse(PA0,FIRST_FUNCTION);      //初始化为GPIO功能
dpmu_set_adio_reuse(PA0,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
dpmu_set_io_direction(PA0,DPMU_IO_DIRECTION_OUTPUT);  //初始化引脚为输出模式
gpio_set_output_mode(PA,pin_0);             //初始化GPIOA的pin_0为输出模式
gpio_set_output_high_level(PA,pin_0);       //输出高电平
gpio_set_output_low_level(PA,pin_0);        //输出低电平

//PA1初始化
scu_set_device_gate(PA,ENABLE);             //开启GPIOA时钟
dpmu_set_io_reuse(PA1,FIRST_FUNCTION);      //初始化为GPIO功能
dpmu_set_adio_reuse(PA1,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
dpmu_set_io_direction(PA1,DPMU_IO_DIRECTION_OUTPUT);  //初始化引脚为输出模式
gpio_set_output_mode(PA,pin_1);             //初始化GPIOA的pin_1为输出模式
gpio_set_output_high_level(PA,pin_1);       //输出高电平
gpio_set_output_low_level(PA,pin_1);        //输出低电平
```

以下代码控制PA组的PA0、PA1为输入模式

```
dpmu_osc_pad_for_gpio(ENABLE);              //PA0和PA1用作GPIO，必须把晶振功能关闭

//PA0初始化
scu_set_device_gate(PA,ENABLE);             //开启GPIOA时钟
dpmu_set_io_reuse(PA0,FIRST_FUNCTION);      //初始化为GPIO功能
dpmu_set_adio_reuse(PA0,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
dpmu_set_io_direction(PA0,DPMU_IO_DIRECTION_INPUT);  //初始化引脚为输入模式
gpio_set_input_mode(PA,pin_0);              //初始化GPIOA的pin_0为输入模式
if(0 == gpio_get_input_level_single(PA,pin_0))       //获取IO电平
{
    //INFO:输入为低电平
}
else
{
    //INFO:输入为高电平
}

//PA1初始化
scu_set_device_gate(PA,ENABLE);             //开启GPIOA时钟
dpmu_set_io_reuse(PA1,FIRST_FUNCTION);      //初始化为GPIO功能
dpmu_set_adio_reuse(PA1,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
dpmu_set_io_direction(PA1,DPMU_IO_DIRECTION_INPUT);  //初始化引脚为输入模式
gpio_set_input_mode(PA,pin_1);              //初始化GPIOA的pin_1为输入模式
if(0 == gpio_get_input_level_single(PA,pin_1))       //获取IO电平
{
    //INFO:输入为低电平
}
else
{
    //INFO:输入为高电平
}
```

以下代码控制PA组的PA0、PA1的中断为双边沿触发

```
dpmu_osc_pad_for_gpio(ENABLE);              //PA0和PA1用作GPIO，必须把晶振功能关闭

//PA0初始化
scu_set_device_gate(PA,ENABLE);             //开启GPIOA时钟
dpmu_set_io_reuse(PA0,FIRST_FUNCTION);      //初始化为GPIO功能
dpmu_set_adio_reuse(PA0,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
dpmu_set_io_direction(PA0,DPMU_IO_DIRECTION_INPUT);  //初始化引脚为输入模式
gpio_set_input_mode(PA,pin_0);              //初始化GPIOA的pin_0为输入模式
gpio_irq_trigger_config(PA,pin_0,both_edges_trigger);  //中断触发方式
eclic_irq_enable(PA_IRQn);                  //使能中断

//PA1初始化
scu_set_device_gate(PA,ENABLE);             //开启GPIOA时钟
dpmu_set_io_reuse(PA1,FIRST_FUNCTION);      //初始化为GPIO功能
dpmu_set_adio_reuse(PA1,DIGITAL_MODE);      //初始化为数字功能，默认是模拟功能
dpmu_set_io_direction(PA1,DPMU_IO_DIRECTION_INPUT);  //初始化引脚为输入模式
gpio_set_input_mode(PA,pin_1);              //初始化GPIOA的pin_1为输入模式
gpio_irq_trigger_config(PA,pin_1,both_edges_trigger);//中断触发方式
eclic_irq_enable(PA_IRQn);                  //使能中断
```