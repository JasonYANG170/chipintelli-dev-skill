<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E7%94%B5%E6%B0%94%E7%89%B9%E6%80%A7/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI2311&CI2312芯片数据手册.zip)

# 电气特性

CI231X系列芯片的电气特性参数见下表。

表E-1 电气参数表

| 符号 | 描述 | 最小值 | 典型值 | 最大值 | 单位 |
| --- | --- | --- | --- | --- | --- |
| VIN5V | PMU输入管脚电压，一般为5V | 3.6 | 5 | 5.5 | V |
| AVDD | 模拟和Codec供电电压 | 2.97 | 3.3 | 3.63 | V |
| VDD33 | 芯片IO供电电压 | 2.97 | 3.3 | 3.63 | V |
| VDD11 | 芯片内核供电电压 | 0.99 | 1.1 | 1.22 | V |
| VDDRF | 蓝牙供电电压 | 1.9 | 3.3 | 3.6 | V |
| VIH | 输入高电压，3.0V ≤ VDD33 ≤ 3.6V | 0.7\*VDD33 | - | - | V |
| VIL | 输入低电压，3.0V ≤ VDD33 ≤ 3.6V | - | - | 0.3\*VDD33 | V |
| VOL | 输出低电压 @IOL = 12mA | - | - | 0.4 | V |
| VOH | 输出高电压 @IOH = 20mA | 2.4 | - | - | V |
| I5VIO | IO（5V耐压）输出3.3V时驱动电流 | 5 | - | 23 | mA |
| I33VIO | IO（3.3V耐压）输出3.3V时驱动电流 | 12 | - | 26 | mA |
| ΣIVDD | 芯片所有IO总电流之和 | - | - | 180 | mA |
| Ta1 | 芯片工作环境温度 | -40 | - | +85 | ℃ |
| Tst | 芯片储存环境温度 | -55 | - | +150 | ℃ |

表E-2 蓝牙参数表

| No | Parameter | Symbol | Conditions | MIN | TYP | MAX | UNIT |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | Current in power down | Ipd | Register retention | - | 2 | - | uA |
| 2 | Current in standby | Istb | Crystal on | - | 40 | - | uA |
| 3 | Current in TX 0dBm | Itx | PA under 0dBm | - | 15 | - | mA |
| 4 | Current in RX 2Mbps | Irx | RX mode | - | 17 | - | mA |
| 5 | Operation frequency | Freq | - | 2400 | - | 2525 | MHz |
| 6 | PLL frequency step | Delta F | - | - | - | 1 | MHz |
| 7 | Freq deviation@1Mbps | Df | - | - | - | 250 | KHz |
| 8 | Freq deviation@2Mbps | Df | - | - | - | 320 | KHz |

##### 以上功耗数据为VDDRF等于3.3V时候测得

芯片进行SMT焊接时请控制炉温和时间，一个SMT焊接的温度曲线如下图所示。

![SMT焊接温度曲线](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI2312%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-24.png)

图E-1 芯片SMT焊接温度曲线

芯片的潮湿敏感度等级为MSL3级，使用前请按照MSL3级条件存储。如果开包装后存放时间超过MSL3级的要求，请在SMT焊接前先进行烘烤。