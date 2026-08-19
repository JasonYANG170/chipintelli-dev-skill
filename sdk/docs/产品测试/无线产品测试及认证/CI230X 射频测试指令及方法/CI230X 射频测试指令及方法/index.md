<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95/CI230X%20%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95/ -->

# CI230X系列芯片 射频测试指令及方法

## 测试环境搭建

![测试环境搭建](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95/img/CI230X%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95-1.png)

图1 测试环境搭建

◆ 待测 DUT 为基于 CI230X系列芯片的模块或者开发板。  
◆ 测试时，PC 发送相应 ATE 测试命令，PC 与 DUT 之间通过 UART 进行交互，进行各种测试模式的配置。  
◆ 测试仪器为 IQ/Itest 极致汇仪/CMW 综测仪/频谱仪等可以测试射频性能指标的仪器即可。

## 测试准备

在进行此测试前，请先查看☞[CI-E05GT02S\_MB开发板套件说明](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI-E05GT02S_MB%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/)。我们所有射频测试基于CI-E05GT02S\_MB开发板套件实现。

### 硬件测试准备

◆ CI-E05GT02S\_MB开发板套件 1 PCS

◆ CI-E0XGT02S模块 1 PCS

◆ USB Type-C 数据线一根

◆ USB转串口工具

◆ 杜邦线若干

### 软件测试准备

◆ 烧录测试固件：基本射频测试烧录《CI230X系列芯片定频测试固件》。

◆ 安装USB转串口工具驱动：确保串口驱动已正确安装，若不能识别串口，建议根据USB转串口工具芯片型号自行安装。

### 传导测试硬件说明

![传导硬件连接图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95/img/CI230X%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95-2.png)

图2 传导硬件连接图

◆ 割线断开天线和匹配电路的连接，将 cable 线连接至匹配电路后，确保 cable 线接地良好。  
◆ 若基于 CI-E05GT02S\_MB 开发板套件进行传导测试，已经板载了USB转串口电路，只需短接PE2、PE3处跳线帽，且将串口选择开关切换至 “Wi-Fi”处，通过 USB Type-C 数据线把开发板连接至电脑即可。  
◆ 若基于 CI-E0XGT02S 模块单独进行传导测试，须把USB转串口工具连接至模块的 PE2、PE3 脚，并连接 3.3V 电源、GND 至模块。  
◆ 打开串口工具，推荐使用 SSCOM ,也可使用其他串口工具。波特率设置 115200，选择对应的 COM 口，然后根据本文后续命令配置测试命令，从而进行各个模式的测试。

## CI230X系列芯片 AT 产测命令说明

### 命令格式

命令 与 参数 之间使用 空格 或者 英文半角逗号 隔开，每条命令以 \r\n 结尾。

必须要的参数使用 < > 标识，可选的参数使用 [ ] 标识，如：

命令 A <必选参数 1> [可选参数 2]

示例：

AT+PVTCMD=EVM,TX,N,7,1,1000\r\n

或者

AT+PVTCMD=EVM TX N 7 1 1000\r\n

### Wi-Fi RF TX/RX 控制命令

#### 命令作用

通过发送这类命令，进行 RF 的发送，接收控制。

#### 变更 channel 命令

命令：AT+PVTCMD=set\_rf\_channel <Chan>\r\n

注：

Chan 是必选参数，取值范围为 1~14 。

成功返回：+set\_rf\_channel OK ch:6

失败返回：+set\_rf\_channel FAIL

#### 设置 TX 相关参数

命令：AT+PVTCMD=EVM,TX,Mode,Rate,Channel,PackageLength\r\n

◆ Mode: BL/BS/G/N  
◆ Rate：  
11B 1,2,5.5,11  
11G 6,9,12,18,24,36,48,54  
11N 0,1,2,3,4,5,6,7  
◆ Channel:1~14  
◆ PackageLength:1000,1024

成功返回：+evm OK  
失败返回：+evm FAIL

示例：

11B 1M CH1: AT+PVTCMD=EVM,TX,B,1,1,1000\r\n

#### 设置 RX（连续模式）相关参数

命令：AT+PVTCMD=EVM,RX,Channel\r\n

成功返回：+evm OK  
失败返回：+evm FAIL

#### 单次 RX 测试

◆ Step1：启动 RX Test

命令：AT+PVTCMD=EVM,RXS,1,CH\r\n

成功返回：+evm OK  
失败返回：+evm FAIL

◆ Step2: 停止测试

命令：AT+PVTCMD=EVM,RXS,0,CH\r\n

成功返回：+evm OK  
失败返回：+evm FAIL

◆ Step3：读取数据

命令：AT+PVTCMD=EVM,RXS,INFO\r\n

成功返回：+evm OK 并显示收包数和 PER  
失败返回：+evm FAIL

#### 设置 evm tx 发送间隔

命令：AT+PVTCMD=evm\_tx\_interval <interval>\r\n

参数：interval 取值为 100 作为 evm tx 测试需要占空比接近满包（>�）分析发射电流的时候使用；interval 取值 10000 （程序默认）为正常的 EVM 指标测试。

说明：每次启动后恢复默认值 10000。

必须先使用 AT+PVTCMD=evm\_tx\_interval <interval> 命令设置 tx 发送间隔，然后使用 EVM 命令进行测试。

### 蓝牙 RF TX/RX 控制命令

#### 接收测试指令

–CMD(H\_CMD\_LE\_RX\_TEST)，指令最后一个字节表示 Rx Frequency = 0x00 ，示例如下

<UART>TX:[01 1D 20 01 00]

注：前 4 个字节是固定的，最后一个字节表示测试的频点（0x00~0x27）。

对应关系如下：

0x00=ch37, 0x01=ch0, 0x02=ch1.....

![蓝牙频道图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95/img/CI230X%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95-3.png)

图2 蓝牙频道图

接收测试指令的响应，注：关注响应的最后一个字节，正确返回必须为 0。

<UART>RX:[04:0E:04:05:1D:20:00]

参数解释：

CMD\_COMPLETE\_EVT(H\_CMD\_LE\_RX\_TEST) :

Length = 0x04

Nb Hci Command Packets = 0x05

Command Opcode = 0x201D (cmd\_le\_rx\_test)

Status = 0x00 (success)

#### 发送测试指令

–CMD(H\_CMD\_LE\_TX\_TEST)：包含如下 3 个参数

Tx Frequency = 0x00 （0x00~0x27）（此处信道需转换为 16 进制，即 00 对应 CH0,13 对应 CH19,27 对应 CH39）

Data Length = 0x25

Packet Payload = 0x00 (0:PRBS9)(0x01:11110000 Packet )(0x02:10101010 Packet Payload)

示例如下：

<UART>TX:[01 1E 20 03 00 25 00]

注：前 4 个字节是固定的，最后三个字节表示测试的频点、数据包长度、数据包内容。

发送测试指令的响应，注：关注响应的最后一个字节，正确返回必须为 0。

<UART>RX:[04:0E:04:05:1E:20:00]

CMD\_COMPLETE\_EVT(H\_CMD\_LE\_TX\_TEST)：

参数解释：

Length = 0x04

Nb Hci Command Packets = 0x05

Command Opcode = 0x201E (cmd\_le\_tx\_test)

Status = 0x00 (success)

#### 测试结束指令

–CMD(H\_CMD\_LE\_TEST\_END) 注：固定指令，无参数

<UART>TX:[01 1F 20 00]

测试结束指令的响应

<UART>RX:[04:0E:06:05:1F:20:00:00:00]

参数解释：

CMD\_COMPLETE\_EVT(H\_CMD\_LE\_TEST\_END)

Length = 0x06

Nb Hci Command Packets = 0x05

Command Opcode = 0x201F (cmd\_le\_test\_end)

Status = 0x00 (success)

Nb Packets = 0x0000 (d0)

注：如果之前发送的 TX\_TEST\_CMD,则响应内容的最后 2 个字节都是 0；如果之前发送的RX\_TEST\_CMD,则响应内容的最后 2 个字节表示实际接收到的数据包数目；

#### 增强型接收测试指令

–CMD(H\_CMD\_LE\_ENH\_RX\_TEST) ：包含如下 3 个参数

Rx Channel = 0x00 （0x00~0x27）

Phy = 0x01 (0x01：1 Mbps PHY)【0x02：2M、0x03：coded】

调制模式 Modulation Index = 0x00 (0：Standard Modulation， 1：Stable Modulation)

示例如下：

<UART>TX:[01 33 20 03 00 01 00]

增强型接收测试指令的响应

<UART>RX:[04:0E:04:05:33:20:00]

CMD\_COMPLETE\_EVT(H\_CMD\_LE\_ENH\_RX\_TEST) :

Length = 0x04

Nb Hci Command Packets = 0x05

Command Opcode = 0x2033 (cmd\_le\_enh\_rx\_test)

Status = 0x00 (success)

#### 增强型发送测试指令

–CMD(H\_CMD\_LE\_ENH\_TX\_TEST) ：包含如下 4 个参数

Tx Channel = 0x00

Length = 0xFB

Payload = 0x00 (0x00:PRBS9)(0x01:11110000 Packet )(0x02:10101010 Packet Payload)

Phy = 0x01 (0x01:1Mbps PHY)【0x02：2M、0x03：coded】

示例如下：

<UART>TX:[01 34 20 04 00 FB 00 01]

增强型发送测试指令的响应

<UART>RX:[04:0E:04:05:34:20:00]

CMD\_COMPLETE\_EVT(H\_CMD\_LE\_ENH\_TX\_TEST)：

Length = 0x04

Nb Hci Command Packets = 0x05

Command Opcode = 0x2034 (cmd\_le\_enh\_tx\_test)

Status = 0x00 (success)

#### 复位指令

<UART>TX:[01 03 0C 00]

#### 发送命令流程

先发复位命令，然后发测试命令。

发送/接收测试结束需要发送测试结束命令

即：

复位命令 -> 发送/接收测试命令 -> 测试结束命令

#### 常用命令

表1 常用命令说明

| 命令 | 作用 |
| --- | --- |
| BLE\_接收 | 01 1D 20 01 00 |
| BLE\_发送 | 01 1E 20 03 00 25 00 |
| BLE\_结束测试 | 01 1F 20 00 |
| BLE\_准备进入边带测试 | 01 1E 20 03 40 00 00 |
| BLE\_退出边带测试 | 01 1E 20 03 50 00 00 |

### 晶体补偿电容

#### 命令作用

模块产线晶体补偿电容 offset 值设置，有正值和负值，offset 取值范围边界：-20~+20。

命令参数设置完毕，模块频偏补偿立即生效，但不保存到 flash，可在测试仪器上观察到频偏变化。

#### 命令演示

AT+PVTCMD=XTAL\_CAP,3\r\n

AT+PVTCMD=XTAL\_CAP,-5\r\n

成功返回：+xtal\_cap OK offset:3

失败返回：+xtal\_cap FAIL

note：+1 = -1ppm

这里设置的是晶体偏置，不是差值；

### Wi-Fi 发射功率补偿

#### 命令作用

模块产线 RF 发射功率微调 offset 值设置，有正值和负值，offset 取值范围边界：-24~+24。

命令参数设置完毕，模块 TX 功率补偿立即生效，但不保存到 flash，可在测试仪器上观察到功率变化。

#### 命令演示

AT+PVTCMD=TX\_POWER,7\r\n

AT+PVTCMD=TX\_POWER,-4\r\n

成功返回：+tx\_power OK offset:7

失败返回：+tx\_power FAIL

note：+4=0.5db

这里设置的是功率偏置，不是差值

### Wi-Fi 产线测试结果

#### 命令作用

模块产线测试完毕，向模块写入测试结果，1=成功，0=失败。模块调用相关接口将测试结果（全部测试成功/失败，晶体补偿， 发射功率）写入 flash。  
注意：这个命令是产线测试的最后一个命令，命令返回“OK\r\n”表示已经将结果写入 flash，仪器要在收到这条命令后再下电模块进入下一工位，以确保 flash 写入成功。

#### 命令演示

AT+ATE\_OK=1\r\n

AT+ATE\_OK=0\r\n

成功返回：OK\r\n