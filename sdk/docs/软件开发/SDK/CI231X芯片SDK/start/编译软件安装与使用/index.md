<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/ -->

# 编译软件安装与使用

---

## 1. 概述

* 本文为CI231X系列芯片软件开发包（SDK）环境安装与使用手册，旨在方便开发人员快速开发代码。

---

## 2、软件下载

* 官网 ☞[Visual Studio Code](https://code.visualstudio.com/) 下载 VSCodeUserSetup，必须 VSCodeUserSetup-x64-1.60.0.exe 或以上版本。

## 3、软件安装

* 双击安装 VSCodeUserSetup-x64-1.60.0.exe，选同意协议，再下一步

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-1.png)

图3-1

* 勾选下列配置，再点下一步

![2](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-2.png)

图3-2

* 点安装

![3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-3.png)

图3-3

* 等待安装完成即可。

### 3.1、打开工程的方法

先打开vscode软件，如图3-6通过菜单栏->文件->打开文件夹，再如图3-7选中CI231X\_SDK文件夹（.vscode文件夹的上一级）

![3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-36.png)

图3-6

![3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-34.png)

图3-7

## 4、编译步骤

### 4.1、方式一 快捷键

* 按快捷键 Ctrl + Shift + B，在下列弹窗中选择 compile（编译） 或 clean （清除）。

![4](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-7.png)

图4-1

### 4.2、方式二 点击图标

* 先安装插件，点击下图所示左侧软件商店图标，再点右上角三个省略号图标

![5](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-4.png)

图4-2

* 在点击省略号出来的选项中，选择“从VSIX中安装” 或 “Installed from VSIX”

![6](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-5.png)

图4-3

* 找到 SDK/tools/ci-tool-1.1.0.vsix 安装，等待安装完成即可，左上角会出现下列编译图标。

![7](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-6.png)

图4-4

* 安装gcc编译工具链， gcc编译工具链可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载 。

![8](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-8.png)

图4-5

* 选择编译器路径 ：

![77](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-9.png)

图4-6

* 选择编译器路径 ：

![77](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-10.png)

图4-7

* 点下图示AI图标，左上角选项框中可以看到所有工程名

![待补充图8](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-11.png)

图4-8

* 右击需要编译的工程名，选项功能介绍如下：
* **1: “生成”**：编译compile
* **2：“清理”**：清除clean
* **3: “设为活动工程”**：快捷键编译方式默认编译活动工程
* **4: “设置烧录串口号”**：输入串口号，如com3，不区分大小写，请确保输入正确且不被占用。
* **5: “合成分区bin文件”**：调用合成分区bin文件.bat，在vscode软件输出窗口命令行运行合成
* **6: “启动打包升级工具”**：启动串口升级工具.exe
* **7: “烧录user code”**：烧录user\_code.bin

## 5、添加源文件方法

说明

CI231X系列芯片的SDK中如果用户增加了新的源代码文件，必须找到工程文件SDK\projects\offline\_asr\_sample\project\_file\source\_file.prj，参照下列示例添加文件到工程编译。

### 5.1、添加.h头文件

* 找到//header file path.注释所在位置，参照示例填写.h头文件所在相对路径

```
//header file path.
include-path: driver/ci231x_chip_driver/inc
include-path: driver/boards
```

### 5.2、添加.c源文件

* 找到//source files.注释所在位置，参照示例填写.c源文件所在相对路径

```
//source files.
source-file: driver/ci231x_chip_driver/src/ci130x_core_eclic.c 
source-file: driver/ci231x_chip_driver/src/ci130x_core_timer.c
```

### 5.3、添加.a库文件

* 参照示例填写.a库文件所在相对路径

```
library-file: libs/libnewlib_port.a 
library-file: libs/libasr.a
```

## 6、固件烧录

* 初次烧录固件，需使用打包升级工具烧录合成的固件，方法参考☞[《CI231X系列芯片\_SDK\_Quick\_Start》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/CI231X_SDK_Quick_Start/)中的步骤4和步骤5，为了方便开发和调试，在此编译软件中集成了只烧录use code的功能，下面是调试过程中烧录use code的方法及注意事项：

### 6.1、步骤一 打包升级固件

* 启动打包升级工具，进入固件打包界面，为user code分区预留合适的大小，如图所示：

![固件打包页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-12.png)

图6-1

* 点击固件升级，烧录合成固件

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-13.png)

图6-2

### 6.2、步骤二 烧录use code

* 针对调试中只修改了use code分区的应用，可以直接在vscode编译器中烧录，无需重新对固件进行打包升级，升级方法如下：
* 右击工程名，选择烧录use code（或快捷键Ctrl+F11），按照如下提示进行硬件复位：

![烧录user code过程界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-14.png)

图6-3

* 升级完成界面如下：

![烧录user code完成界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-15.png)

图6-4

## 7、常见问题

### 7.1、升级use code失败

![11](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-19.png)

图7-1

* 原因是当前烧录的use\_code.bin的大小超出了固件中user分区预留的大小，解决办法，使用固件打包升级工具，重新打包固件，将use分区调大至合理值，参考章节6.1。