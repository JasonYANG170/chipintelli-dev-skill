<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/JLINK%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97/ -->

# JLINK 调试指南

## 1 概述

本文档为CI110X系列芯片SDK使用JLINK工具的调试说明，旨在帮助开发人员快速开发和调试。

## 2 调试环境

以下是调试过程中使用到的软件和硬件：

* IAR 8.30 版本及以上；
* J-link V9 版本及以上；
* CI110X系列芯片SDK。

以上软件及硬件版本等需求请参考文档☞[SDK Quick Start](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/CI110X_SDK_Quick_Start/)。

### 2.1 硬件准备

![JLINK调试准备](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/JLINK%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97-1.png)

1. 供电：输入电源5V/GND；
2. 短接开发板SEL和EN：进入调试模式；
3. 连接调试器：连接J-LINK调试器,连接TMS、TCK、GND三根线，并短接J-LINK的1、2脚。

### 2.2 软件准备

在IAR中打开CI110X系列芯片SDK工程，配置软件环境。首先点击上方priject,再点击Options,进入配置界面如下图：
点击debugger,在setup选项卡的Driver中选择J-link/J-Trace：
![JLINK调试准备](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/JLINK%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97-2.png)

同样在配置界面中选中J-Link/J-Trace，在setup选项卡的Reset中选择Core：

![JLINK调试准备](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/JLINK%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97-3.png)

然后在Connection选项卡的Interface中选中SWD,配置成功，关闭配置界面，回到IAR主界面。

![JLINK调试准备](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/JLINK%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97-4.png)

## 3 调试方法

准备好硬件接线和软件配置，就可以使用J-link进行CI110X系列芯片SDK调试了，在IAR的主界面中分别点击下面的编译和调试按钮就可以进入调试界面：

![JLINK调试准备](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/JLINK%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97-5.png)

调试界面如下图所示，在调试过程中主要使用到的是右上角的调试按钮：

![JLINK调试准备](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/JLINK%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97-6.png)

在调试的过程中，在代码左边的位置双击即可设置断点，在主程序窗口中可以观察c语言在调试过程中的执行顺序，在汇编查看窗口中，可以查看当前汇编代码的执行顺序，在LOG查看窗口中，可以观察Debugger log:

![JLINK调试准备](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/JLINK%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97-7.png)

调试方法：

1. Make & Restart Debugger : 编译并重新开始调试;
2. Restart Debugger : 重新开始调试;
3. Stop Debugging : 停止调试;
4. Step over : 单步执行，不进入子程序;
5. Step Into ：单步执行，跟踪进入子程序;
6. Step Out : 启动C函数或汇编语言子程序从当前位置开始执行，并返回到调用该函数或子程序的下一语句;
7. Next Statement：直接运行到下一条语句;
8. Run to Cursor：从当前位置运行到光标指定处;
9. Go : 运行;
10. Break：终止运行;