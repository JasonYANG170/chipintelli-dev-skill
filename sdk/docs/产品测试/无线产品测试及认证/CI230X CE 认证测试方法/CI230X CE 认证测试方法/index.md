<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20CE%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/CI230X%20CE%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/ -->

# CI230X系列芯片 CE 认证测试方法

## 测试准备

在进行此测试前，请先查看☞[CI-E05GT02S\_MB开发板套件说明](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI-E05GT02S_MB%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/)。我们所有射频测试基于CI-E05GT02S\_MB开发板套件实现。

### 硬件测试准备

◆ CI-E05GT02S\_MB开发板套件 2 PCS

◆ CI-E0XGT02S模块 2 PCS

◆ USB Type-C 数据线一根

◆ USB转串口工具

◆ 杜邦线若干

### 软件测试准备

◆ 烧录测试固件：基本射频测试烧录《CI230X系列芯片定频测试固件》。

◆ 安装USB转串口工具驱动：确保串口驱动已正确安装，若不能识别串口，建议根据USB转串口工具芯片型号自行安装。

### 测试方法

◆ 待测 DUT 为基于 CI230X系列芯片的模块或者开发板。

◆ 测试时，PC 发送相应 ATE 测试命令，PC 与 DUT 之间通过 UART 进行交互，进行各种测试模式的配置。

◆ 若基于 CI-E05GT02S\_MB 开发板套件进行测试，已经板载了USB转串口电路，只需短接PE2、PE3处跳线帽，且将串口选择开关切换至 “Wi-Fi”处，通过 USB Type-C 数据线把开发板连接至电脑即可。

◆ 若基于 CI-E0XGT02S 模块单独进行测试，须把USB转串口工具连接至模块的 PE2、PE3 脚，并连接 3.3V 电源、GND 至模块。

◆ 需要准备 1PCS 完整样机用于辐射测试；1PCS 割线断开天线和匹配电路的连接，将 cable 线连接至匹配电路后，用于传导测试。

◆ 打开串口工具，推荐使用 SSCOM ,也可使用其他串口工具。波特率设置 115200，选择对应的 COM 口，然后根据本文后续命令配置测试命令，从而进行各个模式的测试。

## 定频测试

### Wi-Fi 定频测试

表1 Wi-Fi 命令说明

| 命令 | 作用 |
| --- | --- |
| 设置占空比 | |
| AT+PVTCMD=evm\_tx\_interval 100\r\n | 要先设置占空比，再下发其他 AT 命令 |
| TX 发射指令 | |
| AT+PVTCMD=EVM,TX,B,1,1,1000\r\n | 11B 1M CH1 |
| AT+PVTCMD=EVM,TX,B,1,7,1000\r\n | 11B 1M CH7 |
| AT+PVTCMD=EVM,TX,B,1,13,1000\r\n | 11B 1M CH13 |
| AT+PVTCMD=EVM,TX,G,6,1,1000\r\n | 11G 6M CH1 |
| AT+PVTCMD=EVM,TX,G,6,7,1000\r\n | 11G 6M CH7 |
| AT+PVTCMD=EVM,TX,G,6,13,1000\r\n | 11G 6M CH13 |
| AT+PVTCMD=EVM,TX,G,54,1,1000\r\n | 11G 54M CH1 |
| AT+PVTCMD=EVM,TX,G,54,7,1000\r\n | 11G 54M CH7 |
| AT+PVTCMD=EVM,TX,G,54,13,1000\r\n | 11G 54M CH13 |
| AT+PVTCMD=EVM,TX,N,0,1,1000\r\n | 11N MCS0 CH1 |
| AT+PVTCMD=EVM,TX,N,0,7,1000\r\n | 11N MCS0 CH7 |
| AT+PVTCMD=EVM,TX,N,0,13,1000\r\n | 11N MCS0 CH13 |
| AT+PVTCMD=EVM,TX,N,7,1,1000\r\n | 11N MCS7 CH1 |
| AT+PVTCMD=EVM,TX,N,7,7,1000\r\n | 11N MCS7 CH7 |
| AT+PVTCMD=EVM,TX,N,7,13,1000\r\n | 11N MCS7 CH13 |
| RX 接收指令 | |
| AT+PVTCMD=EVM,RX,1\r\n | CH1 |
| AT+PVTCMD=EVM,RX,7\r\n | CH7 |
| AT+PVTCMD=EVM,RX,13\r\n | CH13 |

![Wi-Fi 命令执行图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20CE%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/img/CI230XCE%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95-1.png)

图2 Wi-Fi 命令执行图

### 蓝牙定频测试

表1 蓝牙命令说明

| 命令 | 作用 |
| --- | --- |
| 01 1E 20 03 00 25 00 | BLE\_发射 1M(00-CH0,13-CH19,27-CH39) |
| 01 34 20 04 00 FB 00 02 | BLE\_发射 2M(00-CH0,13-CH19,27-CH39) |
| 01 1D 20 01 00 | BLE\_接收 1M(00-CH0,13-CH19,27-CH39) |
| 01 33 20 03 00 02 00 | BLE\_接收 2M(00-CH0,13-CH19,27-CH39) |
| 01 1F 20 00 | BLE\_结束测试 |
| 01 1E 20 03 40 00 00 | BLE\_准备进入边带测试 |
| 01 1E 20 03 50 00 00 | BLE\_退出边带测试 |
| 01 03 0C 00 | BLE\_reset |

![蓝牙命令执行图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20CE%20%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95/img/CI230XCE%E8%AE%A4%E8%AF%81%E6%B5%8B%E8%AF%95%E6%96%B9%E6%B3%95-2.png)

图2 蓝牙命令执行图

## 自适应测试

通过 UART 下达测试命令给模块，模块和综测仪相连。一般认证实验室使用的综测仪为：CMW500。

输入命令：

AT+CWJAP=”CMW SSID”,”CMW 密码”

例：AT+CWJAP=”CMW500”,”12345678”

## Blocking 测试

CE 认证中的 Blocking 测试（接收阻塞测试）分两部分：Wi-Fi Blocking 测试和蓝牙 Blocking 测试，蓝牙 Blocking 测试使用 BQB 来测试。

### Wi-Fi blocking

WiFi blocking 认证准备同 自适应测试。

### 蓝牙 blocking（同 BQB 认证）

蓝牙 BQB 测试需要烧录：CI230X系列芯片 BQB 测试固件。

蓝牙 BQB 认证一般为传导方式。断开天线，将测样机 RF cable 线先连接到测试设备的同轴线，一般测试设备为 CMW500，设置蓝牙 HCI 模式，连接到仪器，进行后续 BQB 测试。