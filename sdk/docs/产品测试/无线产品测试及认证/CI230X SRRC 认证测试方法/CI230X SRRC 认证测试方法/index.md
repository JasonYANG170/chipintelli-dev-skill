<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/ -->

# CI230X系列芯片 SRRC 认证测试方法

## 测试环境搭建

![测试环境搭建](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/img/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95-1.png)

图1 测试环境搭建

### 硬件测试准备

◆ 待测 DUT 为基于 CI230X系列芯片 芯片的模块或者开发板。

◆ 连接串口工具3.3V、GND、UART-TX、UART-RX 至模块管脚 VDD33、GND、PE2、PE3。

◆ 测试时，PC 发送相应 ATE 测试命令，PC 与 DUT 之间通过 UART 进行交互，进行各种测试模式的配置。

◆ 须准备模块 5 PCS，其中 4 PCS断开天线和匹配电路的连接并焊接射频测试线，不带屏蔽罩；1 PCS 带屏蔽罩，为完整样品

◆ 打开串口工具，推荐使用 SSCOM ,勾选”加回车换行“。波特率设置 115200，选择对应的 COM 口，然后根据本文后续命令配置测试命令，从而进行各个模式的测试。

◆ USB转串口工具

◆ 杜邦线若干

### 软件测试准备

◆ 烧录测试固件：基本定频测试烧录《CI230X系列芯片定频测试固件》。

◆ 安装USB转串口工具驱动：确保串口驱动已正确安装，若不能识别串口，建议根据USB转串口工具芯片型号自行安装。

## 定频测试

### Wi-Fi 定频测试

表1 Wi-Fi 命令说明

| 命令 | 作用 |
| --- | --- |
| 设置占空比 | |
| AT+PVTCMD=evm\_tx\_interval 100 | 要先设置占空比，再下发其他 AT 命令 |
| TX 发射指令 | |
| AT+PVTCMD=EVM,TX,B,1,1,1000 | 发射：11B 1M CH1 |
| AT+PVTCMD=EVM,TX,B,1,7,1000 | 发射：11B 1M CH7 |
| AT+PVTCMD=EVM,TX,B,1,13,1000 | 发射：11B 1M CH13 |
| AT+PVTCMD=EVM,TX,G,6,1,1000 | 发射：11G 6M CH1 |
| AT+PVTCMD=EVM,TX,G,6,7,1000 | 发射：11G 6M CH7 |
| AT+PVTCMD=EVM,TX,G,6,13,1000 | 发射：11G 6M CH13 |
| AT+PVTCMD=EVM,TX,G,54,1,1000 | 发射：11G 54M CH1 |
| AT+PVTCMD=EVM,TX,G,54,7,1000 | 发射：11G 54M CH7 |
| AT+PVTCMD=EVM,TX,G,54,13,1000 | 发射：11G 54M CH13 |
| AT+PVTCMD=EVM,TX,N,0,1,1000 | 发射：11N MCS0 CH1 |
| AT+PVTCMD=EVM,TX,N,0,7,1000 | 发射：11N MCS0 CH7 |
| AT+PVTCMD=EVM,TX,N,0,13,1000 | 发射：11N MCS0 CH13 |
| AT+PVTCMD=EVM,TX,N,7,1,1000 | 发射：11N MCS7 CH1 |
| AT+PVTCMD=EVM,TX,N,7,7,1000 | 发射：11N MCS7 CH7 |
| AT+PVTCMD=EVM,TX,N,7,13,1000 | 发射：11N MCS7 CH13 |
| RX 接收指令 | |
| AT+PVTCMD=EVM,RX,1 | 接收：CH1 |
| AT+PVTCMD=EVM,RX,7 | 接收：CH7 |
| AT+PVTCMD=EVM,RX,13 | 接收：CH13 |

Wi-Fi 命令执行效果如下：

![Wi-Fi 命令执行图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/img/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95-2.png)

图2 Wi-Fi 命令执行图

### 蓝牙定频测试

蓝牙定频测试须先切换至蓝牙模式，命令如下：

表2 切换至蓝牙命令说明

| 命令 | 作用 |
| --- | --- |
| AT+BLE\_START | 切换至BLE测试 |

切换至蓝牙命令执行效果如下：

![切换至蓝牙命令执行效果图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/img/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95-3.png)

图3 切换至蓝牙命令执行效果图

切换至蓝牙测试后，须勾选”HEX显示“和”HEX发送“，命令如下：

表3 蓝牙命令说明

| 命令 | 作用 |
| --- | --- |
| 01 1E 20 03 00 25 00 | BLE\_发射 1M(00 → CH0,13 → CH19,27 → CH39) |
| 01 34 20 04 00 FB 00 02 | BLE\_发射 2M(00 → CH0,13 → CH19,27 → CH39) |
| 01 1D 20 01 00 | BLE\_接收 1M(00 → CH0,13 → CH19,27 → CH39) |
| 01 33 20 03 00 02 00 | BLE\_接收 2M(00 → CH0,13 → CH19,27 → CH39) |
| 01 1F 20 00 | BLE\_结束测试 |
| 01 1E 20 03 40 00 00 | BLE\_准备进入边带测试 |
| 01 1E 20 03 50 00 00 | BLE\_退出边带测试 |
| 01 03 0C 00 | BLE\_reset |

蓝牙命令执行效果如下：

![蓝牙命令执行效果图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/img/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95-4.png)

图4 蓝牙命令执行效果图

### 补充说明

本文中所述所有指令均已在随本文提供的 SSCOM 软件中配置好，使用时请打开”扩展”按钮，按需点击相应的命令。

SSCOM 命令展现如下：

![SSCOM 命令图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/img/CI230X%20SRRC%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95-5.png)

图5 SSCOM 命令图