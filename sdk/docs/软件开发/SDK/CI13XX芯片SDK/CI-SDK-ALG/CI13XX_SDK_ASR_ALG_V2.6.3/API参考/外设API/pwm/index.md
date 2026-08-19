<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/pwm/ -->

# 脉冲宽度调制输出(PWM)

---

## 1、简介

* PWM（Pulse Width Modulation）是一种通过调节数字脉冲的占空比（高电平时间占整个周期的比例）来等效模拟信号电平的技术，广泛应用于电机控制、电源管理、LED调光等领域。
* CI13XX支持6个专用PWM：PWM0~PWM5。
* 每个PWM输出信号的频率通过API(pwm\_init)进行配置，每个PWM输出信号的占空比通过API(pwm\_init、pwm\_set\_duty)进行配置，不支持100%占空比(常高)，若需使用100%占空比，通过配置GPIO来实现。

---

## 2、特性

* 计数时钟分频，支持1、2、4、16分频，通过API(pwm\_init)进行配置；
* 支持两个32位递减计数器；
* 可变占空比PWM脉冲宽度波形输出；

## 3、API列表

| 函数名 | 描述 |
| --- | --- |
| pwm\_init | 初始化PWM设备 |
| pwm\_start | 启动PWM设备 |
| pwm\_stop | 停止PWM设备 |
| pwm\_set\_duty | 设置PWM占空比 |
| pwm\_set\_restart\_md | 重新计数生效模式的选择 |

---

## 4、普通IO示例

(1) 以下代码配置并启动PWM5，50%占空比，1000频率

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_pwm.h"

void pwm_test()
{
    /*PWM5控制器时钟配置*/
    scu_set_device_gate(HAL_PWM5_BASE,ENABLE);

    /*PWM5的引脚初始化*/
    dpmu_set_io_reuse(PB4,SECOND_FUNCTION);              //设置引脚功能复用
    dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_OUTPUT); //设置引脚功能为输出模式
    dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);          //设置关闭上下拉

    /*PWM5频率、占空比初始化*/
    pwm_init_t init;
    init.clk_sel = 0;    //计数时钟来源于PCLK
    init.freq = 1000;    //频率为1K
    init.duty = 50;      //占空比50%的分子
    init.duty_max = 100; //占空比50%的分母
    pwm_init(PWM5,init);

    /*PWM5启动*/
    pwm_stop(PWM5);      //PWM关闭
    pwm_start(PWM5);     //PWM打开
}
```

(2) 该芯片IP增加了重新计数生效模式的选择，例如初始化后，需要在不同占空比之间切换，希望前一个占空比的PWN波完整输出后，后一个占空比重新计数才生效，可以参考下列示例代码。

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_pwm.h"

void pwm_test()
{
    /*PWM5控制器时钟配置*/
    scu_set_device_gate(HAL_PWM5_BASE,ENABLE);

    /*PWM5的引脚初始化*/
    dpmu_set_io_reuse(PB4,SECOND_FUNCTION);              //设置引脚功能复用
    dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_OUTPUT); //设置引脚功能为输 出模式
    dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);          //设置关闭上下拉

    /*PWM5频率、占空比初始化*/
    pwm_init_t init;
    init.clk_sel = 0;      //计数时钟来源于PCLK
    init.freq = 1000;      //频率为1K
    init.duty = 50;        //占空比50%的分子
    init.duty_max = 100;   //占空比50%的分母
    pwm_init(PWM5,init);
    pwm_set_restart_md(PWM5,1);   //配置等待正在进行的PWM波完整输出后重新计数才生效

    /*PWM5启动*/
    pwm_stop(PWM5);        //PWM关闭
    pwm_start(PWM5);       //PWM打开

    /*PWM5中途切换占空比*/
    pwm_set_duty(PWM5,30,100)    //切换到占空比30%
}
```

配置等待正在进行的PWM波完整输出后重新计数才生效，波形结果如下：

![PWM波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/pwm-1.png)

配置重新计数立即生效，pwm\_set\_restart\_md(PWM0,0)，波形结果如下：

![PWM波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/pwm-2.png)

---

(3) 2组PWM实现互补输出功能

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_pwm.h"
#include "task.h"

void pwm_test()
{
    /*PWM5控制器时钟配置*/
    scu_set_device_gate(HAL_PWM5_BASE,ENABLE);

    /*PWM5的引脚初始化*/
    dpmu_set_io_reuse(PB4,SECOND_FUNCTION);                //设置引脚功  能复用
    dpmu_set_io_direction(PB4,DPMU_IO_DIRECTION_OUTPUT);   //设置引脚功  能为输出模式
    dpmu_set_io_pull(PB4,DPMU_IO_PULL_DISABLE);            //设置关闭上  下拉

    /*PWM5频率、占空比初始化*/
    pwm_init_t init;
    init.clk_sel = 0;     //计数时钟来源于PCLK
    init.freq = 16000;    //频率为1.6K
    init.duty = 50;       //占空比50%的分子
    init.duty_max = 100;  //占空比50%的分母
    pwm_init(PWM5,init);
    pwm_stop(PWM5);       //PWM关闭

    /*PWM0控制器时钟配置*/
    scu_set_device_gate(HAL_PC_BASE,ENABLE);

    /*PWM0的引脚初始化*/
    scu_set_device_gate(HAL_PWM0_BASE,ENABLE);
    dpmu_set_io_reuse(PC4,FORTH_FUNCTION);                 //设置引脚功能复用
    dpmu_set_io_direction(PC4,DPMU_IO_DIRECTION_OUTPUT);   //设置引脚功能为输出模式
    dpmu_set_io_pull(PC4,DPMU_IO_PULL_DISABLE);            //设置关闭上下拉

    /*PWM5频率、占空比初始化*/
    init.clk_sel = 0;    //计数时钟来源于PCLK
    init.freq = 16000;   //频率为1.6K
    init.duty = 50;      //占空比50%的分子
    init.duty_max = 100; //占空比50%的分母
    pwm_init(PWM0,init);
    pwm_stop(PWM0);      //PWM关闭

    /*PWM0和PWM5错开时间开启，实现互补输出*/
    {
        volatile uint32_t i = 0;
        taskENTER_CRITICAL();
        pwm_start(PWM5);
        for (i = 0;i < 0x210;i++);
        pwm_start(PWM0);
        taskEXIT_CRITICAL();
    }
}
```

## 5、晶振IO示例

(1) PA0是连接晶振的IO，默认是模拟功能，以下代码配置并启动PA0复用成PWM5，50%占空比，1000频率

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_pwm.h"

void pwm_test()
{
    /*使能晶振IO用作普通IO功能*/
    dpmu_osc_pad_for_gpio(ENABLE);                       //PA0用作PWM，必须把晶振功能关闭

    /*PWM5控制器时钟配置*/
    scu_set_device_gate(HAL_PWM5_BASE,ENABLE);

    /*PWM5的引脚初始化*/
    dpmu_set_io_reuse(PA0,SECOND_FUNCTION);              //设置引脚功能复用
    dpmu_set_adio_reuse(PA0,DIGITAL_MODE);               //初始化为数字功能，默认是模拟功能
    dpmu_set_io_direction(PA0,DPMU_IO_DIRECTION_OUTPUT); //设置引脚功能为输出模式
    dpmu_set_io_pull(PA0,DPMU_IO_PULL_DISABLE);          //设置关闭上下拉

    /*PWM5频率、占空比初始化*/
    pwm_init_t init;
    init.clk_sel = 0;     //计数时钟来源于PCLK
    init.freq = 1000;     //频率为1K
    init.duty = 50;       //占空比50%的分子
    init.duty_max = 100;  //占空比50%的分母
    pwm_init(PWM5,init);

    /*PWM5启动*/
    pwm_stop(PWM5);    //PWM关闭
    pwm_start(PWM5);   //PWM打开
}
```

## 6、模拟IO示例

（1）PC1、PC2、PC3、PC4这4个引脚默认是模拟功能，以下代码配置并启动PC1复用成PWM3，50%占空比，1000频率

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_pwm.h"

void pwm_test()
{
    /*PWM3控制器时钟配置*/
    scu_set_device_gate(HAL_PWM3_BASE,ENABLE);

    /*PWM3的引脚初始化*/
    dpmu_set_io_reuse(PC1,FORTH_FUNCTION);               //设置引脚功能复用
    dpmu_set_adio_reuse(PC1,DIGITAL_MODE);               //初始化为数字功能，默认是模拟功能
    dpmu_set_io_direction(PC1,DPMU_IO_DIRECTION_OUTPUT); //设置引脚功能为输出模式
    dpmu_set_io_pull(PC1,DPMU_IO_PULL_DISABLE);          //设置关闭上下拉

    /*PWM3频率、占空比初始化*/
    pwm_init_t init;
    init.clk_sel = 0;     //计数时钟来源于PCLK
    init.freq = 1000;     //频率为1K
    init.duty = 50;       //占空比50%的分子
    init.duty_max = 100;  //占空比50%的分母
    pwm_init(PWM3,init);

    /*PWM3启动*/
    pwm_stop(PWM3);    //PWM关闭
    pwm_start(PWM3);   //PWM打开
}
```

## 7、常见问题

PWM模块基础时钟为主频的一半，例如CLK\_S = 100000000Hz（100M）。

PWM频率与PWM最大占空比的关系：

* CLK\_S 大于等于（freq \* duty\_max）
* 当freq为10000000Hz（10M）时，此时duty\_max最大可配置为10
* 当freq为1000000Hz（1M）时，此时duty\_max最大可配置为100
* 当freq为100000Hz（100K）时，此时duty\_max最大可配置为1000

最大频率支持：

* freq = 50000000Hz（50M）
* duty\_max = 2;

## 8、API 参考