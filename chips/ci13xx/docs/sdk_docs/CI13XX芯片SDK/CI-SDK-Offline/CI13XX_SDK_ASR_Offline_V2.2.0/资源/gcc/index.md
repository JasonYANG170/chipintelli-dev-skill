<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%B5%84%E6%BA%90/gcc/ -->

# GCC编译工具链

## 1. 简介

GCC(GNU Compiler Collection)是由GNU开发的编程语言编译器，以GPL许可证所发行的自由软件，也是GNU计划的关键部分。GCC的初衷是为GNU操作系统专门编写一款编译器，现已被大多数类Unix操作系统(Linux、BSD、MacOS等)采纳为标准的编译器，支持包括riscv在内的多种核心架构的编译工具链，包含编译器、汇编器、链接器等组件，用于将源代码转换为可执行文件。

## 2. 下载

启英泰伦CI13XX系列芯片编译器下载链接☞[点击下载(riscv-nuclei-elf-gcc)](http://chipintelli.com/zh-cn/page/153.html)

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%B5%84%E6%BA%90/img/gcc-1.png)

备注

CI13XX系列芯片所使用的GCC版本格式为riscv-nuclei-elf-gcc-x.x.x

## 3. 安装

* 从上面的链接下载下来应该是一个压缩包文件。将此压缩包文件解压到任意文件夹(最好路径名中没有空格和中文字符)。
* 打开Visual Studio Code, 如果还没有安装，请参考文档☞《[Visual Studio Code](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%B5%84%E6%BA%90/Visual%20Studio%20Code/)》下载安装。
* 请参考文档☞《[IDE 搭建与使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)》中的 “为CI工程管理插件设置编译器路径” 一节进行设置。