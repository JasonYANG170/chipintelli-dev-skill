<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/epwm/ -->

# 增强型脉冲宽度调制输出(EPWM)

---

## 1、简介

EPWM有2路PWM输出，分别为EPWMA和EPWMB，对应引脚PWMN和PWMP，用于工业及消费类电子领域常用的电源控制器件，例如电机控制等。可当作普通PWM使用，或输出互补PWM波，或自定义的PWM输出。

---

## 2、工作原理

#### 2.1、配置值说明

* TBPRD：计数周期值 > 0，配置宽度最大为16位，可配置计数到TBPRD值时产生什么动作，拉高、拉低或不变
* ZERO：计数周期值0，以下简称0，可配置计数到ZERO值时产生什么动作，拉高、拉低或不变
* CMPA：相位CMPA的值，配置宽度最大为16位，可配置计数到CMPA值时产生什么动作，拉高、拉低或不变
* CMPB：相位CMPB的值，配置宽度最大为16位，可配置计数到CMPB值时产生什么动作，拉高、拉低或不变

#### 2.2、计数模式

* 递增模式：每个周期均是，计数从0向TBPRD递增。1个TBPRD周期就能输出1周期PWM
* 递减模式：每个周期均是，计数从TBPRD向0递减。1个TBPRD周期就能输出1周期PWM
* 增减模式：奇数周期，计数从0向TBPRD递增；偶数周期，计数从TBPRD向0递减。2个TBPRD周期就能输出1周期PWM。

![EPWM模式选择](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/epwm-1.png)

EPWM模式选择

#### 2.3、使用方法

首先配置TBPRD、CMPA、CMPB的值，指定计数模式，再配置计数到TBPRD、ZERO、CMPA、CMPB值时分别产生什么动作，就能输出自己想要的波形。允许配置CMPA = CMPB，或CMPA > CMPB，或CMPA < CMPB。

---

## 3、示例说明

#### 3.1、如何输出50%占空比PWM波？

* 假设主时钟为40M，TBCLK分频参数为1，配置相位CMPA，EPWMA通道输出频率为10000，50%占空比PWM波，按照下列公式计算配置值。

```
   频率 = 主时钟 / TBCLK分频参数 / TBPRD周期 / 2

        频率 10000 = 主时钟 40M / TBCLK分频参数 1 / TBPRD周期 / 2

        TBPRD周期 = 20M / 10000 = 2000

        //增减模式时，2个TBPRD周期才能输出1个周期PWM，所以除2，递增模式或递减模式不需要除2

    占空比 = CMPA 相位 / TBPRD 周期

        CMPA相位 = 50% 占空比 * TBPRD 周期 = 1000
```

* 按下图输出50%占空比PWM波，计数到0和TBPRD时保持不变，递增时计数到CMPA拉高，递减时计数到CMPA拉低

![50%占空比PWM波](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/epwm-2.png)

#### 3.2、如何输出20%低电平占空比PWM波？

* 假设主时钟为40M，TBCLK分频参数为1，配置相位CMPB、EPWMA通道输出频率为50000，输出低电平25%占空比PWM，按照下列公式计算配置值。

```
    频率 = 主时钟 / TBCLK分频参数 / TBPRD周期 / 2

        频率 50000 = 主时钟 40M / TBCLK分频参数 1 / TBPRD周期 / 2

        TBPRD周期 = 20M / 50000 = 400

        //增减模式时，2个TBPRD周期才能输出1个周期PWM，所以除2，递增模式或递减模式不需要除2

    低电平占空比 = CMPB 相位 / TBPRD 周期

        CMPB相位 = 25% 低电平占空比 * TBPRD 周期 = 100
```

* 按下图输出20%低电平占空比PWM波，计数到0和TBPRD时保持不变，递增时计数到CMPB拉高，递减时计数到CMPB拉低

![20%低电平占空比PWM波](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/epwm-3.jpg)

#### 3.3、如何输出互补功能（带死区的PWM波）？

* 互补：两根线，输出的PWM，只有一端导通，和死区概念类似。
* 死区时间：指在这段时间，上下都没有输出
* 带死区的PWM波防止上下两个器件同时导通。当一个器件导通后关闭，再经过一段死区，才能让另一个导通。

```
增减模式时，频率 frequency = 主时钟 / TBCLK分频参数 / TBPRD周期 / 2

EPWMA通道高电平占空比 = (TBPRD - CMPA) / TBPRD = 1 - （CMPA / TBPRD）

EPWMB通道高电平占空比 = CMPB / TBPRD

死区相位 = （CMPA - CMPB）
```

* 按按下图输出带死区的pwm波，EPWMA通道用相位CMPA控制，EPWMB通道用相位CMPB控制

![带死区的PWM波](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/epwm-4.png)

---

## 4、API

| 函数名 | 描述 |
| --- | --- |
| epwm\_tbprd\_init | 周期配置 |
| epwm\_tbctl\_init | 分频参数、模式等初始化 |
| epwm\_cmpa\_config | 相位CMPA的值 |
| epwm\_cmpb\_config | 相位CMPB的值 |
| epwm\_cmpctrl\_config | 相位控制 |
| epwm\_aqctla\_config | EPWMA通道动作配置 |
| epwm\_aqctlb\_config | EPWMB通道动作配置 |
| epwm\_start | EPWM开始输出 |
| epwm\_stop | EPWM停止输出 |
| epwm\_set\_duty\_decrease | EPWM更改需要的占空比 |

---

## 5、示例代码

#### 5.1、以下代码配置双通道输出互补pwm波（带死区）

```
//配置PA2为PWMP功能、配置PA3为PWMN功能
dpmu_set_io_reuse(PA2, SIXTH_FUNCTION);
dpmu_set_io_reuse(PA3, SIXTH_FUNCTION);

//打开时钟，复位，复位释放
scu_set_device_gate(HAL_EPWM_BASE,ENABLE);
scu_set_device_reset(HAL_EPWM_BASE);        
scu_set_device_reset_release(HAL_EPWM_BASE);

//计数周期
epwm_tbprd_config(EPWM,100);

//分频参数、模式配置，除了配置的，其他均为默认值
epwm_tbctl_init_t tbctl_init = {0};
tbctl_init.CTRMODE = EPWM_CTRMODE_INC_DEC; /*增减模式*/
tbctl_init.PRDLD = EPWM_PRDLD_FREE; /*任何时候周期控制直接寄存器都可以获取TBPRD的值*/
tbctl_init.CNTEN = EPWM_CNTEN_STOP; /* 计数器计数使能，初始化时先关闭 */
tbctl_init.CLKDIV = EPWM_CLKDIV1; /*1分频*/
tbctl_init.FREESOFT = EPWM_FREESOFT_CONTINUE; /*继续运行*/
epwm_tbctl_config(EPWM,&tbctl_init);

//cmpa相位配置
epwm_cmpa_config(EPWM,60);
//cmpb相位配置
epwm_cmpb_config(EPWM,30);

//配置何时获取CMPA或CMPB的值
epwm_cmpctrl_init_t cmpctrl_init = {0};
cmpctrl_init.LOADAMODE = EPWM_LOADABMODE_PRD; /*在counter = PRD时*/
cmpctrl_init.LOADBMODE = EPWM_LOADABMODE_PRD;
cmpctrl_init.SHDWAMODE = EPWM_SHDWABMODE_DIRECT; /*间接模式*/
cmpctrl_init.SHDWBMODE = EPWM_SHDWABMODE_DIRECT;
epwm_cmpctrl_config(EPWM,&cmpctrl_init);

//PWMP通道动作配置，除了配置的，其他均输出无动作
epwm_aqctlx_init_t aqctla_init = {0};
aqctla_init.CAU = EPWM_OUT_HIGH; /*当计数器counter=CMPA，递增阶段时EPWMx输出高*/
aqctla_init.CAD = EPWM_OUT_LOW;  /*当计数器counter=CMPA，递减阶段时EPWMx输出低*/
epwm_aqctla_config(EPWM,&aqctla_init);
//PWMN通道动作配置，除了配置的，其他均输出无动作
epwm_aqctlx_init_t aqctlb_init = {0};
aqctlb_init.CBU = EPWM_OUT_LOW;  /*当计数器counter=CMPB，递增阶段时EPWMx输出低*/
aqctlb_init.CBD = EPWM_OUT_HIGH; /*当计数器counter=CMPB，递减阶段时EPWMx输出高*/
epwm_aqctlb_config(EPWM,&aqctlb_init);

//开始计数
epwm_start(EPWM);
_delay_10us(10);

//中途同比例修改PWMP、PWMN的占空比（一条线高电平占空比为90%，另一条线高电平占空比为45%）
epwm_set_duty_decrease(EPWM,90, 0.5f);
_delay_10us(10);

//中途同比例修改PWMP、PWMN的占空比（一条线高电平占空比为50%，另一条线高电平占空比为25%）
epwm_set_duty_decrease(EPWM,50, 0.5f);
_delay_10us(10);

//中途同比例修改PWMP、PWMN的占空比（一条线高电平占空比为25%，另一条线高电平占空比为45%）
epwm_set_duty_decrease(EPWM,45, 2.0f);
_delay_10us(10);

//停止计数
epwm_stop(EPWM);
```

* 开始计数后，输出波形如下，一条线的高电平占空比为40%（周期100，相位60），另一条线的高电平占空比为30%（周期100，相位30）。

![带死区的PWM波](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/epwm-5.png)