<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/CI13XX%E7%B3%BB%E5%88%97%E5%8F%8CMIC%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91V1.0/ -->

# 双麦语音识别项目开发-CI1306

## 一、方案介绍

### 1.1 方案背景与目标

随着人工智能与声学技术的飞速发展，传统单麦克风语音识别系统正面临越来越多的应用瓶颈。受限于单声道拾音的物理特性，单麦克风难以有效分离目标语音与环境噪声，在复杂声场环境（如多人嘈杂场景、混响严重空间）中容易出现识别准确率骤降的问题，且无法实现声源定位、波束成形等进阶功能。而双麦克风阵列通过构建声学差分模型，可利用双声道的时间差与相位差特性，实现精准的声源定向捕捉，配合自适应滤波算法，能有效抑制以上的环境噪声，并通过波束成形技术聚焦目标语音信号。同时，双麦克风阵列的空间立体声场感知能力，为远场语音交互、多模态人机协作等前沿应用提供了技术基础，已成为突破单麦克风性能天花板的关键解决方案。启英泰伦作为 BNPU 的发明者，凭借先进的架构与卓越的运算能力，BNPU 为实现双麦克风阵列算法的各类应用提供了坚实有力的支持。

### 1.2 硬件方案框架

双麦语音识别方案实现，可以采用启英泰伦AI语音芯片☞[CI130X 系列（CI1303/CI1302/CI1306）](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/) 。本文方案开发采用CI-D06GT01D开发板进行开发演示。开发板采用主芯片型号为CI1306，集成了启英泰伦自研的脑神经网络处理器BNPU V3和CPU内核，系统主频可达240MHz，内置高达640KByte的SRAM，集成PMU电源管理单元和RC振荡器，双通道高性能低功耗Audio Codec和多路UART、IIC、IIS、PWM、GPIO、PDM等外围控制接口。芯片仅需少量电阻电容等外围器件就可以实现各类智能语音产品硬件方案，性价比极高。

CI1306芯片应用框图如下图所示：

![CI1306芯片数据手册-1](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-1.png)

图1 CI1306 应用框图

### 1.3双麦语音识别算法简介

目前已经实现的有语音深度降噪（DENOISE）、语音增强（DSS）、声源定位（DOA）等：

**1.3.1 双麦语音深度降噪（DENOISE）**

语音深度降噪算法能有效抑制稳态噪声，在保证语音失真度的同时具有良好的噪声抑制效果。开启该功能后，可以通过降低稳态噪声的方式进行识别效果的提升。

应用原理应用原理示意框图如下，人声为目标语音信号A，噪音和信号A在应用环境进行复杂的混合后进入芯片，语音降噪算法可抑制噪音信号，提高信号A的信噪比，从而提升识别效果。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-3.png)

**注意事项：目前启英泰伦提供的语音降噪算法仅适用于稳态噪声及半稳态噪声。**

**1.3.2 语音增强（DSS）**

语音增强算法主要用于噪声环境下，抑制噪声和干扰。可以过滤稳态噪声，瞬态噪声及远距离的背景人声，提供给云端干净的人声。

该方案的典型应用环境示意图如下图所示，图a、b中，麦克风阵列为线性双麦阵列，推荐麦间距为4cm，其结构方向与实际应用环境中的噪音源方向及目标人声方向相适应。目标人声在语音增强范围内，噪音信号在抑制范围内。语音增强算法可以起到在嘈杂环境中抑制噪声与增强目标人声的作用，开启后可以提升语音识别的效果。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-4.png)

图a麦克风阵列横向结构噪声和目标信号示意图

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-5.png)

图b麦克风阵列竖向结构噪声和目标信号示意图

**注意事项：**

（1）噪声距离麦克风阵列1米内效果较好；

（2）使用过程中无需设置初始目标声源角度和mic间距（建议麦克风间距40-120mm），但请尽量避免噪声和人声同方向，此情况下算法性能会降低，噪声与人声夹角越大，降噪效果越好；

（3）目前sdk默认关闭alc，麦克alc固定增益设置为20；

（4）为提升算法稳定性，低音量环境和70dB以上噪音环境会尽可能进入单麦识别流程，此时与单麦sdk识别效果相当；

**1.3.3 声源定位（DOA）**

声源定位采用波达方向（Direct of Arrival,DOA）估计算法，主要用于声源方位角度估计，以获取目标声源位置或者为语音增强算法提供先验信息。

该方案应用场景示意图如下图所示。图a,b中麦克风阵列为线性双麦阵列，推荐麦间距为4cm。设置角度从左麦克到右麦克为0度到180度变化，利用DOA算法可以获取目标人声的方位角度。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-6.png)

图 (a) 麦克风阵列横向结构与角度设置示意图

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-7.png)

图(b) 麦克风阵列竖向结构与角度设置示意图

**注意事项：**

（1）DOA默认使用唤醒词模式，即识别到唤醒词后会输出对应角度信息。

## 二、开发准备工作

开发前，请先准备好相关如下硬件，软件，如涉及量产，请从查看☞[生产测试](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E7%94%9F%E4%BA%A7%E6%B5%8B%E8%AF%95/)及联系启英获取生产支持：

### 2.1 开发硬件准备

**2.1.1 开发板简介**

本方案开发演示使用硬件模块☞[CI-D06GT01D](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI1306%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/)进行，该开发板集成一颗USB总线的转接芯片CH340G，可通过USB接口直接为芯片下载程序。同时该开发板具有众多的测试接口和测试电路，可以帮助用户快速开发产品。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-8.png)

图2.1 CI-D06GT01D 实物图

备注：开发板原理图：☞[D06GT01D原理图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/img/CI-D06GT01D%E5%8E%9F%E7%90%86%E5%9B%BE.pdf)、点击获取☞[购买链接](http://mall.chipintelli.com/chip2?product_id=74&brd=1)

**2.1.2 固件烧录与调试**

1、可以直接通过TYPE-C数据线连接板子与终端进行固件烧录。若配置日志口为UART0，可以直接查看日志。

2、也可以采用通用的串口工具进行固件烧录（连接UART0）和查看日志，或进行点击☞[购买链接](http://mall.chipintelli.com/chip2?product_id=60)

下图是串口烧录工具:

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-9.png)

图2.2 串口工具实物图

特别注意：为了数据稳定，尽量选择带晶振的串口，推荐选择CH341 的串口（☞[CH340官方驱动下载](https://www.wch.cn/products/CH340.html)）

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-10.png)

**2.1.3 采音板**

用于分析产品的底噪及声音情况，实物如下图：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-11.png)

图2.3 采音板实物图

本采音板主要是通过IIS采音和uart 对被测试语音模块进行采音分析（需要特定的软件配置编译的固件），☞[使用方法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%B5%84%E6%BA%90/%E9%87%87%E9%9F%B3%E6%9D%BF%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)参考

### 2.2 开发软件准备

1、开发环境搭建：☞[VScode下载及使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%B5%84%E6%BA%90/Visual%20Studio%20Code/)：

2、SDK 代码包获取: ☞[语音AI平台 - 资料库](https://aiplatform.chipintelli.com/attachment)

3、☞[启英泰伦AI平台账号](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E6%A6%82%E8%BF%B0/)：用于命令词模型的生成，播放音的生成等。

**特别注意：需要注册后认证企业用户，才可以开发双麦项目可以获取更多功能**

4、录音及分析工具：☞[Audacity](https://pc.qq.com/detail/0/detail_640.html)或其他工具

5、串口日志工具：☞[SSCOM](https://soft.3dmgame.com/down/247967.html)或者☞[串口调试工具](https://apps.microsoft.com/detail/9nblggh43hdm?launch=true&hl=zh-cn&gl=cn) 或其他开发者习惯的串口工具主要用于debug 查看日志

## 三、开发验证准备

### 3.1 硬件搭建

开发板搭建如下图：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-12.png)

使用说明：

（1）串口使用说明：管脚不与其他功能冲突的前提下，UART0，UART1，UART2，均可以配置成日志打印口或者通讯口；烧录升级固件只能采用UART0。图中UART0通过板载CH340，转换成USB 直接与PC端相连；USB-TTL工具通过杜邦线连接UART1，芯片TX1接工具RX，芯片RX1接TX。

（2）麦克风：板子分左右两路麦克风，支持两种接口形式。分别满足不同的麦克风接口形式。麦克风的”+”对应接口的”+”，避免接反影响识别效果。同时特别注意，多麦克风阵列，麦克风拥有较好的一致性，才可以获得更准确的结果信息

（3）其他：喇叭对应连接到SPK+，SPK-接口处。1306带有RESET功能。可以通过拨动电源开关进行通断电，或按reset键使芯片复位后让系统重启。

### 3.2 软件开发环境搭建

☞[SDK 下载和环境搭建介绍：](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)

## 四、双麦方案硬件参考设计

### 4.1 基于CI1306硬件参考设计

本应用大部分为电池类供电，需要考虑功耗，故需要使用电池+dcdc进行方案供电

4.1.1 CI1306芯片及外围电路

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-13.png)

主控部分原理图

4.1.2 模组供电电路

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-14.png)

4.1.3 麦克风的单端线路和差分线路

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-15.png)

麦克风的单端线路图

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-16.png)

麦克风的差分线路图

单端线路，麦克”+”信号接语音芯片的MICP\_L麦克的”-“接GND，此线路适合A、硬件干扰小 B、硬件干扰大同时使用屏蔽线麦克的应用场景；

差分线路（推荐），麦克的”+”接语音芯片的MICP\_L，麦克的”-“接语音芯片的MICP\_N，在语音芯片内进行差分运算，可以消除常见的一些共模干扰（例如一个信号干扰到麦克”+”和”-“，差分运算后就将该干扰信号消除了）,此线路+双绞线麦克，可以解决绝大部分的通用外部干扰

注意事项：两种方式麦克风线路在语音芯片内接收到的信号幅度存在差异，软件为保证识别效果，需要调整宏定义；（user\_config.h文件）

```
//麦克风电路模式配置

#define MIC_DIFF_SINGLE                0   //1,单端。0，差分
```

4.1.4 UART0 预留升级线路

UART0 为升级和debug的日志口，需要预留便于升级和调试

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-17.png)

4.1.5 AEC 参考线路

双麦工程中使用回声消除算法，若配置了其他任意双麦算法，需外置codec，本方案选用立体声音频ADC芯片ES7243E进行设计，参考电路如下图：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-18.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-19.png)

### 4.2 硬件验证

硬件PCB做好后，需要进行整机底噪测试和AEC效果测试，依此保证产品为最佳效果状态

4.2.1 硬件底噪测试

为了保证识别效果，做好的硬件安装在产品结构中后，需要检查底噪是否正常；底噪正常意味着整个产品的麦克灵敏度正常及有较好的声学结构；

底噪主要采集如下场景：

1. 整机供电无麦克，
2. 整机供电安静环境（40~50dB）
3. 整机供电60dB稳态噪音，

   详细的采音方法和底噪判断链接：

4.2.2 AEC效果测试：

将PCB安装到结构中，安装固定好麦克和喇叭，组成整个产品后，再来测试AEC效果；

定性测试：安静环境3米，设备播放云端内容时麦克处的分贝不超过85db情况下，正常讲话音量70db左右，唤醒率为90%以上，麦克分贝接近90db情况，大声讲话可以正常打断；

声音判断：在正常播放云端内容时，录制参考声音和AEC后的声音，分析是否AEC后的声音能消除掉产品本身的播放音；

## 五、结构注意事项及重要物料选配

### 5.1 麦克风结构注意

> 麦克风类似是人的耳朵，本质是将一个微小声压信号进行放大，然后进入语音芯片进行识别，所以麦克风在安装时候，要特别产品开孔和注意远离噪音避免干扰,☞[详情查看连接](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/)

采用双麦设计，双麦克风推荐距离为4CM(两麦克风的中心距离）；

麦克规格（推荐）：灵敏度-32dB，信噪比65dB以上。

### 5.2 喇叭注意事项

喇叭是设备发音的器件，很多时候为了成本选择不带音腔的喇叭，此时设计上需要特别注意密封和开孔；

喇叭的结构注意事项见☞[文档中心链接](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E5%96%87%E5%8F%AD%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E5%96%87%E5%8F%AD%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/)

### 5.3 影响AEC效果的结构注意事项

AEC为在设备播放声音时，用户仍可以通过唤醒词将播放状态打断，在本方案中是个特别重要的功能，在产品设计时需要特别注意：

> 1、麦克和喇叭严格按照建议进行相关设计,注意好密封
>
> 2、麦克和喇叭尽量远离，背向，使喇叭到达mic的声音尽量小，最好不大于85db
>
> 3、麦克和喇叭的相对空间位置不能变化；【AEC 会自适应估计喇叭位置、喇叭发音特性等情况】
>
> 4、参考信号按启英给出的分压参考，如果喇叭声音变大，需要适当调整分压电阻，保证最大音量下参考分压电路后的电压幅值范围为100-150mv；

## 六、双麦方案固件开发

> 双麦方案固件开发可以通过算法SDK直接开发，也可以通过启英泰伦语音AI平台进行开发。

### 6.1 基于算法SDK开发双麦方案固件

6.1 .1 获取算法SDK

登录启英泰伦AI平台-->选择开发资料-->选择130X离线SDK-->选择带ALG字眼的算法SDK版本，下载文件到本地。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-20.png)

6.1 .2 公共配置

1、板级配置：

> 本文档示例采用CI-D06GT01D 开发板，需要在..\projects\offline\_asr\_alg\_pro\_sample\app\app\_main\user\_config.h中修改板级配置如下：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-21.png)

2、修改配置完成后，在VScode中按如下操作进行代码编译和和合成分区文件 ：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-22.png)

3、通过上图step5的打包固件工具按如下步骤打包固件；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-23.png)

固件打包界面

4、打包固件成功后，将硬件串口接到PC后，再给 开发板进行供电；下图为固件升级成功的界面；更多细节☞[请查看文档中心链接](https://document.chipintelli.com/软件开发/SDK/CI13XX芯片SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API指南/语音识别深度开发进阶/命令词和固件制作指南/#433)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-24.png)

固件打包升级界面

6.1.3 语音深度降噪算法配置

深度降噪法算抑制设备本身产生的高噪声，以提升低信噪比环境下的识别率，针对不同应用领域需使用对应的领域模型，当前版本默认提供了烟机和窗帘的降噪模型。

1、打开offline\_asr\_alg\_pro\_sample\project\_file\makefile文件，对CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_DENOISE\_NN)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-25.png)

2. 深度降噪算法把external\model\nn\_denoise(深度降噪)中[60003]nn\_denoise\_xx.bin算法模型，复制到projects\offline\_asr\_alg\_pro\_sample\firmware\dnn文件夹中

   注意：

   （1）深度降噪需搭配该算法的前端算法模型使用。

   （2）深度降噪在AI平台制作ASR与DNN模型请选择深度降噪专用模型。

   ![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-26.png)
3. 在 ..\projects\offline\_asr\_alg\_pro\_sample\app\app\_main\ci\_ssp\_config.c 文件，可以对降噪算法参数进行调整

```
//denoise模块配置

const denoise_config_t denoise_config =

{

    .alg_enable = true,

    .start_Hz = 0,            //降噪起始频率 单位Hz

    .end_Hz = 8000,           //降噪结束频率 单位Hz

    .fre_resolution = 31.25f, //频率分辨率 单位Hz

    .aggr_mode = 1,//算法处理的效果等级:0，1，2，处理效果依次增强，失真也会变大

    .set_denoise_threshold = 7000.0f,     //默认帧平均幅值\>=7000起效

    .set_denoise_thr_window_size = 20 //门限判断窗长

};
```

6.1.4 语音增强算法配置

双麦语音增强算法主要用于噪声环境下，抑制噪声和干扰，增强目标语音，只支持双mic，双mic位置需处于同一平面相同朝向，推荐麦间距为4cm~6cm。

1、打开offline\_asr\_alg\_pro\_sample\project\_file\makefile文件，对CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_BF)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-27.png)

2. 该算法参数宏说明在projects\offline\_asr\_alg\_pro\_sample\app\app\_main\user\_config.h文件中

```
#define BF_DEEPSE_MODE                  1   //1:全深度分离更耗内存(单双网络都可以用) 0：半深度分离(唤醒词做深度分离，命令词不做，只能用双网络)

#define BF_ASR_VALID_MODE               0   //1:开启ASR打分是否有效判断功能 0：关闭ASR打分是否有效判断功能 该功能只针对全深度分离和半深度分离
```

3、在 ..\projects\offline\_asr\_alg\_pro\_sample\app\app\_main\ci\_ssp\_config.c 文件，可以对算法参数进行调整

```
//bf模块配置

const bf_config_t bf_config =

{

    .alg_enable = true,

    .distance = 40,

    .angle = 90,

    .freq = 20,

    .frame_wkup = 100,

    .frame_rt = 40,

    .wkup_result_thr = 45,     //ASR分值的阈值，值设置的越大，误识会降低(肯能也会影响识别)，根据具体项目去调整

    .bf_deepse_mode = BF_DEEPSE_MODE,

    .bf_asr_valid_mode = BF_ASR_VALID_MODE

};
```

参数说明：

distance：麦克风阵列麦间距，int类型，默认值40

angle：设置角度，int型，范围为[0 180]，默认值90

freq：设置频点，int型，范围为[0 255]，默认值20

frame\_wkup：唤醒词时间帧长，int型，默认值90

frame\_rt：响应时间帧长，int型，默认值40

wkup\_result\_thr：唤醒词打分阈值，int型，默认值35

bf\_new\_method：通道选取模式，0为旧模式，1为新模式

注意事项：

（1）噪声距离麦克风阵列1米内效果较好

（2）使用过程中无需设置初始目标声源角度和MIC间距（建议麦克风间距40-120mm），但请尽量避免噪声和人声同方向，此情况下算法性能会降低，噪声与人声夹角越大，降噪效果越好

（3）目前SDK默认关闭ALC，麦克风固定增益设置为20

（4）为提升算法稳定性，低音量环境和70dB以上噪音环境会尽可能进入单麦识别流程，此时与单麦识别效果相当

**6.1.5 双麦声源定位算法配置**

双麦声源定位算法为双mic算法可进行声源方位角度估计，当前版本支持0-180度检测范围，分辨率为10°，双mic位置需处于同一平面相同朝向，推荐麦间距为4cm~6cm。

1、打开offline\_asr\_alg\_pro\_sample\project\_file\makefile文件，对CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_AI\_DOA)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-28.png)

2、该算法参数宏说明在projects\offline\_asr\_alg\_pro\_sample\app\app\_main\user\_config.h文件中

```
#define BF_DEEPSE_MODE                  1   //1:全深度分离更耗内存(单双网络都可以用) 0：半深度分离(唤醒词做深度分离，命令词不做，只能用双网络)

#define BF_ASR_VALID_MODE               0   //1:开启ASR打分是否有效判断功能 0：关闭ASR打分是否有效判断功能 该功能只针对全深度分离和半深度分离
```

3. 在 ..\projects\offline\_asr\_alg\_pro\_sample\app\app\_main\ci\_ssp\_config.c 文件，可以对算法参数进行调整

```
//doa模块配置

const doa_config_t doa_config =

{

    .alg_enable = true,

    .distance = 40,

    .min_frebin = 40,

    .max_frebin = 130,

    .samplerate = 16000,

    .doa_resolut = 5

};
```

参数说明：

distance：麦克风阵列麦间距，int类型，默认40mm

min\_frebin：进行DOA计算的最低频点，范围为[0 255]，默认40

max\_frebin：进行DOA计算的最高频点，范围为[0 255]，默认130

samplerate：输入音频采样率，默认16000

doa\_resolut：DOA输出分辨率，只能选择3、5、10。分别表示输出为3个角度、5个角度和10个角度。

3个角度的输出为45°、90°和135°；

5个角度的输出为0°、45°、90°、135°和180°；

10个角度的输出为0到180°，每间隔20°输出一个角度。

备注：DOA默认使用唤醒词模式，即识别到唤醒词后会输出对应角度信息。

**6.1.6 双麦降混响算法配置**

1、打开offline\_asr\_alg\_pro\_sample\project\_file\makefile文件，对CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_DEREVERB)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-29.png)

2、该算法参数宏说明在projects\offline\_asr\_alg\_pro\_sample\app\app\_main\user\_config.h文件中

```
#if USE_DEREVERB_MODULE
#define DEREVERB_FREQ_RANGE_INDEX       0  //默认0:算法起效频率160HZ-4800HZ 消耗28KB内存  1: 算法起效频率0-8000HZ 消耗49KB内存    
#endif
```

3、在 ..\projects\offline\_asr\_alg\_pro\_sample\app\app\_main\ci\_ssp\_config.c 文件，可以对算法参数进行调整

```
/dr模块配置,startHZ:算法起效的起始频率  endHZ:算法起效的结束频率  范围:0-8KHZ,调大会增加一定的算法力和内存消耗

//160HZ-4800HZ 消耗28KB内存  0-8000HZ 消耗49KB内存

const dereverb_config_t dereverb_config =

{

    .alg_enable = true,

#if DEREVERB_FREQ_RANGE_INDEX

    .startHz = 0.0f,

    .endHz = 8000.0f

#else

    .startHz = 160.0f,

    .endHz = 4800.0f

#endif

};
```

**6.1.7 双麦组合算法配置**

☞[参考算法开发流程描述](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)

### 6.2 使用AI平台制作固件

登录启英泰伦AI平台。注意只有企业级以上权限客户，才可以开发双麦算法方案。

1. 选择产品开发下，你先语音识别大模型应用功能模块

   ![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-30.png)
2. 点击进入语音识别固件及SDK开发

   ![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-31.png)
3. 新建项目

   ![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-32.png)
4. 填写基本信息

   ![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-33.png)
5. 固件参数配置

   ![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-34.png)
6. 播报音色选择

   ![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-35.png)
7. 命令词制作

   ![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-36.png)

   具体参考：☞[命令词固件制作流程](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/%E5%91%BD%E4%BB%A4%E8%AF%8D%E5%92%8C%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%E6%8C%87%E5%8D%97/)
8. SDK下载

   ![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-37.png)