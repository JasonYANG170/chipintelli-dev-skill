<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E7%94%B5%E6%B0%94%E7%89%B9%E6%80%A7/ -->

[请点击下载PDF文档](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/PDF/CI230X_datasheet V0.3_chs_20220902.pdf)

# 电气参数

## 电气特性

表E-1 电气参数表

| 符号 | 参数 | 最小值 | 典型值 | 最大值 | 单位 |
| --- | --- | --- | --- | --- | --- |
| 5V\_IN | PMU 输入管脚电压 一般为 5V | 3.6 | 5 | 5.5 | V |
| VDD33 | Wi-Fi 供电电压 | 2.6 | 3.3 | 3.6 | V |
| VIH | 输入高电压 3.0V ≤ VDD33 ≤ 3.6V | VDD33-0.6 | / | VDD33+0.3 | V |
| VIL | 输入低电压 3.0V ≤ VDD33 ≤ 3.6V | / | / | 0.6 | V |
| VOL | 输出低电压 @IOL = 12mA | / | / | 0.4 | V |
| VOH | 输出高电压 @IOL =20mA | VDD33-0.5 | VDD33 | VDD33+0.3 | V |
| I3.3V | 模块3.3V电源工作电流 | 90 | / | 500 | mA |
| I5V | 模块5V电源工作电流 | 55 | / | 250 | mA |

## Wi-Fi特性

### 支持频段

表E-2 频段表

| Parameter | Min | Typ | Max | Unit |
| --- | --- | --- | --- | --- |
| Receive frequency rang 2.4Ghz | 2412 | / | 2484 | Mhz |

### 接收特性

表E-3 接收特性表

| Parameter | Condition | Min | Typ | Max | Unit |
| --- | --- | --- | --- | --- | --- |
| Sensitivity | | | | | |
| 11b，1M | FER < 8%，1024 bytes | / | -94 | / | dBm |
| 11b，11M | FER < 8%，1024 bytes | / | -87 | / | dBm |
| 11g，6M | FER < 10%，1024 bytes | / | -90 | / | dBm |
| 11g，54M | FER < 10%，1024 bytes | / | -74 | / | dBm |
| 11n，MCS0 | FER < 10%，1024 bytes | / | -90 | / | dBm |
| 11n，MCS7 | FER < 10%，1024 bytes | / | -71 | / | dBm |
| Maximum input level | | | | | |
| 11b | FER < 8%，1024 bytes | / | 4 | / | dBm |
| 11g | FER < 10%，1024 bytes | / | -10 | / | dBm |
| 11n | FER < 10%，1024 bytes | / | -10 | / | dBm |
| power consumption | | | | | |
| 11b | / | / | 80 | / | mA |
| 11g | / | / | 82 | / | mA |
| 11n | / | / | 82 | / | mA |

**以上功耗数据为VDDA33为3.3V时测得电流。**

### 发射特性

表E-4 发射特性表

| Parameter | Condition | Min | Typ | Max | Unit |
| --- | --- | --- | --- | --- | --- |
| Output power | | | | | |
| 11b，1M DSSS | Maximum Burst power | / | 18 | / | dBm |
| 11g，54M OFDM | Maximum Burst power | / | 16 | / | dBm |
| 11n，MCS7 | Maximum Burst power | / | 14 | / | dBm |
| Power consumption | | | | | |
| 11b | 100% Duty Cycle @ 17dBm | / | 320 | / | mA |
| 11g | 100% Duty Cycle @ 14dBm | / | 290 | / | mA |
| 11n | 100% Duty Cycle @ 13dBm | / | 270 | / | mA |

**以上功耗数据为VDDA33为3.3V时测得电流。**

**芯片内置的RC振荡器会随环境温度变化产生一定的温漂，环境温度范围变化越大，温漂越大。该时钟温漂可能对需要高精度时钟的应用，或者与上位机串口通信的准确率带来影响。**

**应用方案需要高精度时钟的，或者需要进行串口通信且环境温度范围超过-10到70℃的，建议采用外部晶振作为时钟源，工作环境温度可以达到或超过工业标准规格。如采用内部RC振荡器作为时钟源，串口通信波特率必须小于或等于115200bps，同时与上位机串口波特率之间总偏差不得超过4%，以保证良好通信。工作环境温度为-10到70℃的，配合的上位机串口波特率偏差在该温区须不超过±1.5%。如工作环境温度为-20到85℃的，上位机串口波特率偏差在该温区须不超过±1% 。**

**当上位机为免晶振设计时，需要尽量减小通讯误差。启英泰伦可提供串口波特率自适应方案，该方案需要在串口协议中增加一个握手指令，并且上位机保证在收到该握手指令的50ms内会按照协议要求回复。增加该自适应方案后，产品可以用于工作环境温度为-20到85℃的场景。**

CI230X系列芯片进行SMT焊接时请控制炉温和时间，一个SMT焊接的温度曲线如下图所示。

![CI230X系列芯片 SMT焊接温度曲线](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/img/CI230X%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C-24.png)

图E-1 CI230X系列芯片SMT焊接温度曲线

CI230X系列芯片的潮湿敏感度等级为MSL3级，使用前请按照MSL3级条件存储。如果开包装后存放时间超过MSL3级的要求，请在SMT焊接前先进行烘烤。