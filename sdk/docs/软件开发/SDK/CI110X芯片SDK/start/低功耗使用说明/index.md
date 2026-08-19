<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/%E4%BD%8E%E5%8A%9F%E8%80%97%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# 低功耗使用说明

---

## 1. 概述

目前CI110X系列芯片SDK中支持以下两种低功耗模式:

* 降频模式；
* 低功耗模式。

启用这两种模式后，当系统处于非唤醒状态则进入对应模式，已达到降低功耗的目的，而处于唤醒状态下则不会受到影响。

---

## 2. 降频模式

降频模式是一种将系统主频降低为原频率一半的做法，由于保留了语音识别全流程所需外设的所有时钟，对系统流程影响极小，通过该模式可以降低接近一半的功耗。

使用方法：

* 在user\_config.h中定义USE\_LOWPOWER\_DOWN\_FREQUENCY宏为1；
* 在user\_config.h中定义USE\_SEPARATE\_WAKEUP\_EN宏为1；
* 打包固件时添加一个仅用于唤醒的模型(这种模型对芯片硬件资源消耗较少，适合在低频模式下工作)。

按照上述操作即可在非唤醒状态下启用降频模式降低功耗。

目前芯片和模组正常状态的功耗数据为（以下功耗为在5V供电时整体功耗）：

| 芯片 | 正常工作时功耗 | 安静状态下功耗 |
| --- | --- | --- |
| 1102 | 0.34W | 0.26W |

---

## 3. 低功耗模式

目前语音芯片的低功耗模式需要配合硬件进行修改，无法直接通过软件调整达到稳定使用状态，如有产品需要进入低功耗工作模式请联系原厂技术人员。

使用方法：

* 在user\_config.h中定义USE\_LOWPOWER\_OSC\_FREQUENCY宏为1；
* 在user\_config.h中定义USE\_SEPARATE\_WAKEUP\_EN宏为1；
* 打包固件时添加一个仅用于唤醒的模型(这种模型对芯片硬件资源消耗较少，适合在低频模式下工作)。

按照上述操作即可在非唤醒状态下启用降频模式降低功耗。

警告

注意定义USE\_LOWPOWER\_OSC\_FREQUENCY宏后，无论USE\_LOWPOWER\_DOWN\_FREQUENCY宏为0还是1均会处于低功耗模式，而非降频模式。

---

## 4. 功耗模式对比

![低功耗模式对比](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/%E4%BD%8E%E5%8A%9F%E8%80%97%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-1.png)

---

## 5. 低功耗模式用户扩展

在进入退出低功耗时，往往需要添加自己的操作代码，sdk中增加了一个注册接口用于注册低功耗用户hook函数，用法如下：

```
extern void register_lowpower_user_fn(void * enter_lowpower_fn,void * exit_lowpower_fn);

void user_enter_lowpower(void)
{
    //进入低功耗
    uint32_t clk;
    clk = get_ipcore_clk(); //可以通过这一组函数获取进入低功耗模式后，当前时钟频率
    /* 省略处理函数 */
    clk = get_apb_clk();
    /* 省略处理函数 */
}

void user_exit_lowpower(void)
{
    //退出低功耗
    uint32_t clk;
    clk = get_ipcore_clk(); //可以通过这一组函数获取进入低功耗模式后，当前时钟频率
    /* 省略处理函数 */
    clk = get_apb_clk();
    /* 省略处理函数 */
}

int main(void)
{
    //注册用户扩展函数
    register_lowpower_user_fn((void*)user_enter_lowpower,(void*)user_exit_lowpower);
}
```