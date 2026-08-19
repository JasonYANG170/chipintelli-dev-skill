<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/driver/ci230x_driver/pwm/ -->

# 脉冲宽度调制输出(PWM)

---

## 简介

PWM(Pulse Width Modulation，脉冲宽度调制)是一种对模拟信号电平进行数字编码的方式，通过不同频率的脉冲，以及使用不同占空比的方波来对一个具体的模拟信号进行电平编码，使输出端得到一系列幅值相等的脉冲，用这些脉冲来代替所需要波形的设备。

---

## API列表

| 函数名 | 描述 |
| --- | --- |
| pwm\_init | 初始化PWM设备 |
| pwm\_start | 启动PWM设备 |
| pwm\_stop | 停止PWM设备 |
| pwm\_set\_duty | 设置PWM占空比 |
| pwm\_set\_restart\_md | 重新计数生效模式的选择 |

---

## 示例

(1) 以下代码配置并启动PWM0，P占空比，1000频率

```
scu_set_device_gate(HAL_PWM5_BASE,ENABLE);
dpmu_set_io_reuse(PB4,SECOND_FUNCTION);                //设置引脚功能复用
dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_OUTPUT); //设置引脚功能为输出模式
dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);          //设置关闭上下拉
pwm_init_t init;
init.clk_sel = 0;
init.freq = 1000;
init.duty = 50;
init.duty_max = 100;
pwm_init(PWM5,init);
pwm_stop(PWM5);
pwm_start(PWM5);
```

(2) 该芯片IP增加了重新计数生效模式的选择，例如初始化后，需要在不同占空比之间切换，希望前一个占空比的PWN波完整输出后，后一个占空比重新计数才生效，可以参考下列示例代码。

```
scu_set_device_gate(HAL_PWM5_BASE,ENABLE);
dpmu_set_io_reuse(PB4,SECOND_FUNCTION);                //设置引脚功能复用
dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_OUTPUT); //设置引脚功能为输出模式
dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);          //设置关闭上下拉
pwm_init_t init;
init.clk_sel = 0;
init.freq = 1000;
init.duty = 50;
init.duty_max = 100;
pwm_init(PWM5,init);
pwm_set_restart_md(PWM5,1);        //配置等待正在进行的PWM波完整输出后重新计数才生效
pwm_stop(PWM5);
pwm_start(PWM5);

//延时1s后
pwm_set_duty(PWM5,30,100)          //切换到占空比30%
```

配置等待正在进行的PWM波完整输出后重新计数才生效，波形结果如下：

![PWM波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/driver/ci230x_driver/img/pwm-1.png)

配置重新计数立即生效，pwm\_set\_restart\_md(PWM0,0)，波形结果如下：

![PWM波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/driver/ci230x_driver/img/pwm-2.png)

---

(3) 2组PWM实现互补输出功能

```
scu_set_device_gate(HAL_PWM5_BASE,ENABLE);
dpmu_set_io_reuse(PB4,SECOND_FUNCTION);                //设置引脚功能复用
dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_OUTPUT);   //设置引脚功能为输出模式
dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);            //设置关闭上下拉

pwm_init_t init;
init.clk_sel = 0;
init.freq = 16000;
init.duty = 50;
init.duty_max = 100;
pwm_init(PWM5,init);
pwm_stop(PWM5);
scu_set_device_gate(HAL_PC_BASE,ENABLE);
scu_set_device_gate(HAL_PWM0_BASE,ENABLE);
dpmu_set_io_reuse(PC4,FORTH_FUNCTION);                //设置引脚功能复用
dpmu_set_io_direction(PC4,DPMU_IO_DIRECTION_OUTPUT);   //设置引脚功能为输出模式
dpmu_set_io_pull(PC4,DPMU_IO_PULL_DISABLE);            //设置关闭上下拉
init.clk_sel = 0;
init.freq = 16000;
init.duty = 50;
init.duty_max = 100;
pwm_init(PWM0,init);
pwm_stop(PWM0);
{
    volatile uint32_t i = 0;
    taskENTER_CRITICAL();
    pwm_start(PWM5);
    for (i = 0;i < 0x210;i++);
    pwm_start(PWM0);
    taskEXIT_CRITICAL();
}
```

## 晶振IO示例

(1) PA0是连接晶振的IO，默认是模拟功能，以下代码配置并启动PA0复用成PWM5，P占空比，1000频率

```
dpmu_osc_pad_for_gpio(ENABLE);                       //PA0用作PWM，必须把晶振功能关闭

scu_set_device_gate(HAL_PWM5_BASE,ENABLE);
dpmu_set_io_reuse(PA0,SECOND_FUNCTION);              //设置引脚功能复用
dpmu_set_adio_reuse(PA0,DIGITAL_MODE);               //初始化为数字功能，默认是模拟功能
dpmu_set_io_direction(PA0,DPMU_IO_DIRECTION_OUTPUT); //设置引脚功能为输出模式
dpmu_set_io_pull(PA0,DPMU_IO_PULL_DISABLE);          //设置关闭上下拉
pwm_init_t init;
init.clk_sel = 0;
init.freq = 1000;
init.duty = 50;
init.duty_max = 100;
pwm_init(PWM5,init);
pwm_stop(PWM5);
pwm_start(PWM5);
```

## 常见问题

PWM模块基础时钟为主频的一半，例如CLK\_S = 100000000Hz（100M）。

PWM频率与PWM最大占空比的关系：

* CLK\_S 大于等于（freq \* duty\_max）
* 当freq为10000000Hz（10M）时，此时duty\_max最大可配置为10
* 当freq为1000000Hz（1M）时，此时duty\_max最大可配置为100
* 当freq为100000Hz（100K）时，此时duty\_max最大可配置为1000

最大频率支持：

* freq = 50000000Hz（50M）
* duty\_max = 2;