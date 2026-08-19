<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/iwdg/ -->

# 独立看门狗(IWDG)

---

## 1、简介

* IWDG是一种硬件定时电路，主要用于监测系统由于工作异常而引发的故障并从故障中进行恢复。

## 2、特性

* CI13XX支持1个IWDG模块，IWDG是基于一个32位递减计数器，计数器从装载值开始递减，计数值计数到0时，产生超时中断，计数器重新加载装载值，计数值再次计数到0时，若超时中断未被清除，则IWDG将产生复位请求。
* 可通过API(dpmu\_iwdg\_reset\_none\_config、dpmu\_iwdg\_reset\_system\_config、dpmu\_iwdg\_reset\_bus\_config)配置复位域的范围。

---

## 3、API列表

| 函数名 | 描述 |
| --- | --- |
| iwdg\_init | 初始化IWDG设备 |
| iwdg\_open | 启动IWDG设备 |
| iwdg\_close | 停止IWDG设备 |
| iwdg\_feed | IWDG设备喂狗 |

---

## 4、示例

以下代码配置并启动IWDG喂狗时间为1S：

```
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_iwdg.h"
#include "ci130x_core_eclic.h"

void iwdg_test()
{
    /* IWDG控制器时钟配置 */
    scu_set_device_gate(HAL_IWDG_BASE, ENABLE);

    /* 开启IWDG中断 */
    eclic_irq_enable(IWDG_IRQn);

    /* 打开IWDG的复位配置 */
    dpmu_iwdg_reset_system_config();

    /* IWDG初始化 */
    iwdg_init_t init;
    init.irq = iwdg_irqen_enable;         //中断使能
    init.res = iwdg_resen_enable;         //复位使能
    init.count = get_src_clk() / 16;      //IWDG计数时钟从src时钟经过   16分频得到
    iwdg_init(IWDG,init);

    /* IWDG启动 */
    iwdg_open(IWDG);

    /* 喂狗 */
    iwdg_feed(IWDG);
}
```

## 5、API 参考