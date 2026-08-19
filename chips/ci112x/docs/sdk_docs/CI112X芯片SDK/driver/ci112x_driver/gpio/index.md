<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/driver/ci112x_driver/gpio/ -->

# 通用输入输出(GPIO)

---

## 简介

GPIO(通用IO端口)每个GPIO端口都有相应的控制寄存器和配置寄存器。API分为两类：一类是单独操作一个pin，另一类是可同时操作一个或多个pin。提供IO输入输出状态查询接口，中断屏蔽接口、中断屏蔽查询接口、中断清除接口、中断状态查询接口、中断触发方式配置接口（可配置为：低电平触发、高电平触发、上升沿触发、下降沿触发、双边沿触发）等，以满足不同应用的要求。

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

## 示例

以下代码控制GPIO1组的pin1输出模式

```
Scu_SetDeviceGate((unsigned int)GPIO1,ENABLE);
Scu_SetIOReuse(PWM0_PAD,FIRST_FUNCTION);
Scu_SetIOPull(PWM0_PAD,DISABLE);
gpio_set_output_mode(GPIO1,gpio_pin_1);
gpio_set_output_level_single(GPIO1,gpio_pin_1,0);   //输出低电平
gpio_set_output_level_single(GPIO1,gpio_pin_1,1);   //输出高电平
```

以下代码配置GPIO1组的pin1为输入模式

```
Scu_SetDeviceGate((unsigned int)GPIO1,ENABLE);
Scu_SetIOReuse(PWM0_PAD,FIRST_FUNCTION);
Scu_SetIOPull(PWM0_PAD,DISABLE);
gpio_set_input_mode(GPIO1,gpio_pin_1);
if(0 == gpio_get_input_level_single(GPIO1,gpio_pin_1))//获取IO状态
{
    //INFO:输入为低电平
}
else
{
    //INFO:输入为高电平
}
```

以下代码配置GPIO1组的pin1的中断为双边沿触发

```
Scu_SetDeviceGate((unsigned int)GPIO1,ENABLE);
Scu_SetIOReuse(PWM0_PAD,FIRST_FUNCTION);
Scu_SetIOPull(PWM0_PAD,DISABLE);
gpio_set_input_mode(GPIO1,gpio_pin_1);
gpio_irq_trigger_config(GPIO1,gpio_pin_1,both_edges_trigger);//中断触发方式
eclic_irq_enable(GPIO1_IRQn);
```

警告

IO芯片内部上下拉功能，只有一种状态（上拉/下拉），根据实际需求通过Scu\_SetIOPull()接口进行控制。
eg:
Scu\_SetIOPull(PWM0\_PAD,ENABLE);//打开内部上下拉
Scu\_SetIOPull(PWM0\_PAD,DISABLE);//关闭内部上下拉