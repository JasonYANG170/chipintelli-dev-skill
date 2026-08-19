<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/iwdg/ -->

# 独立看门狗(IWDG)

---

## 简介

IWDG(独立看门狗)看门狗定时器是一种硬件定时电路，主要用于检测系统是否发生故障。独立看门狗模块同步时钟为PCLK，基于一个32bit的递减计数器，使用SRC时钟计数，当计数值达到0时，产生中断请求计数器重载初值再次进行递减计数，再次当计数值到0时若中断未被清除，将产生复位请求，同时计数器停止计数。

---

## API列表

| 函数名 | 描述 |
| --- | --- |
| iwdg\_init | 初始化IWDG设备 |
| iwdg\_open | 启动IWDG设备 |
| iwdg\_close | 停止IWDG设备 |
| iwdg\_feed | IWDG设备喂狗 |

---

## 示例

以下代码配置并启动IWDG喂狗时间为1S：

```
scu_set_device_gate(HAL_IWDG_BASE, ENABLE);
eclic_irq_enable(IWDG_IRQn);
/* 打开IWDG的复位配置 */
dpmu_iwdg_reset_system_config();
/* 配置IWDG并启动 */
iwdg_init_t init;
init.irq = iwdg_irqen_enable;
init.res = iwdg_resen_enable;
init.count = get_src_clk() / 16;      //IWDG计数时钟从src时钟经过16分频得到
iwdg_init(IWDG,init);
iwdg_open(IWDG);
```