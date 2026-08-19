<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/driver/ci112x_driver/pwm/ -->

# 脉冲宽度调制输出(PWM)

---

## 简介

PWM(Pulse Width Modulation,脉冲宽度调制)是一种对模拟信号电平进行数字编码的方式，通过不同频率的脉冲，以及使用不同占空比的方波来对一个具体的模拟信号进行电平编码，使输出端得到一系列幅值相等的脉冲，用这些脉冲来代替所需要波形的设备。

---

## API列表

| 函数名 | 描述 |
| --- | --- |
| pwm\_init | 初始化PWM设备 |
| pwm\_start | 启动PWM设备 |
| pwm\_stop | 停止PWM设备 |
| pwm\_set\_duty | 设置PWM占空比 |

---

## 示例

以下代码配置并启动PWM0，P占空比，1000频率

```
Scu_SetDeviceGate(HAL_PWM0_BASE,ENABLE);
Scu_SetIOReuse(PWM0_PAD,SECOND_FUNCTION);
Scu_SetIOPull(PWM0_PAD,DISABLE);//关闭IO内部上下拉
pwm_init_t init;
init.clk_sel = 0;
init.freq = 1000;
init.duty = 50;
init.duty_max = 100;
pwm_init(PWM0,init);
pwm_stop(PWM0);
pwm_start(PWM0);
```

---

## 常见问题

PWM模块基础时钟为主频的一半，例如CLK\_S = 82000000Hz（82M），分频系数取值范围（1，2，4，16），默认分频值为16。

PWM频率与PWM最大占空比的关系：

* CLK\_S 大于等于（freq \* duty\_max）
* 当freq为8200000Hz（8.2M）时，此时duty\_max最大可配置为10
* 当freq为820000Hz（820K）时，此时duty\_max最大可配置为100
* 当freq为82000Hz（82K）时，此时duty\_max最大可配置为1000

最大频率支持：

* freq = 41000000Hz（41M）
* duty\_max = 2;

如何设置支持最大频率：

```
//先配置分频参数为1
Scu_SetDeviceGate(HAL_PWM0_BASE,DISABLE);
Scu_Setdevice_Reset(HAL_PWM0_BASE);
Scu_Setdiv_Parameter(HAL_PWM0_BASE,1);
Scu_SetDeviceGate(HAL_PWM0_BASE,ENABLE);
Scu_Setdevice_ResetRelease(HAL_PWM0_BASE);

//再参照示例配置PWM占空比、最大占空比、频率等
```