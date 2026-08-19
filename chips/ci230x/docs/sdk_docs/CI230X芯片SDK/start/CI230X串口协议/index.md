<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/CI230X%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE/ -->

# 编译软件安装与使用

---

## 1. 概述

* 本文为CI230X系列芯片软件开发包（SDK）环境安装与使用手册，旨在方便开发人员快速开发代码。

---

## 2. 软件下载

* 官网 ☞[Visual Studio Code](https://code.visualstudio.com/) 下载 VSCodeUserSetup，必须 VSCodeUserSetup-x64-1.60.0.exe 或以上版本。

## 3. 软件安装

* 双击安装 VSCodeUserSetup-x64-1.60.0.exe，选同意协议，再下一步

![1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-1.png)

图 3-1

* 勾选下列配置，再点下一步

![2](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-2.png)

图 3-2

* 点安装

![3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-3.png)

图 3-3

* 等待安装完成即可。

### 3.1 打开工程的方法

**方法一**：进入CI230X\_SDK文件夹（.vscode文件夹所在的目录），空白处右键“通过Code打开”：

![3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-34.png)

图 3-4

**方法二**：选中C230X\_SDK文件夹（.vscode文件夹的上一级），右键“通过Code打开”：

![3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-35.png)

图 3-5

**方法三**：先打开vscode软件，如图3-6通过菜单栏->文件->打开文件夹，再如图3-7选中C230X\_SDK文件夹（.vscode文件夹的上一级）

![3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-36.png)

图 3-6

![3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-37.png)

图 3-7

## 4. CI230X系列芯片 语音部分SDK编译步骤

### 4.1 方式一 快捷键

* 按快捷键 Ctrl + Shift + B，在下列弹窗中选择 compile（编译） 或 clean （清除）。

![4](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-7.png)

图 4-1

### 4.2 方式二 点击图标

* 先安装插件，点击下图所示左侧软件商店图标，再点右上角三个省略号图标

![5](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-4.png)

图 4-2

* 在点击省略号出来的选项中，选择“从VSIX中安装” 或 “Installed from VSIX”

![6](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-5.png)

图 4-3

* 找到 SDK/tools/ci-tool-1.1.0.vsix 安装，等待安装完成即可，左上角会出现下列编译图标。

![7](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-6.png)

图 4-4

* 请先到启英泰伦官网☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment)下载开发环境搭建软件包，软件包包括语音部分和wifi部分，工具包包含三部分，请完整下载后解压得到toolchain-audio和toolchain-wifi

![77](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/CI230X_SDK_%E5%BC%80%E5%8F%91%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA%E8%BD%AF%E4%BB%B6%E5%8C%85.png)

图 4-5

* 设置gcc编译工具链，解压ci230x\_gcc库.rar到任意目录，点工程下图处，设置编译器路径。

![77](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-9.png)

图4-6

* 选择编译器路径 ：

![77](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-10.png)

图4-7

* 点下图示AI图标，左上角选项框中可以看到所有工程名

![待补充图8](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-11.png)

图4-8

* 右击需要编译的工程名，选项功能介绍如下：
* **1: “生成”**：编译compile
* **2：“清理”**：清除clean
* **3: “设为活动工程”**：快捷键编译方式默认编译活动工程
* **4: “设置烧录串口号”**：输入串口号，如com3，不区分大小写，请确保输入正确且不被占用。
* **5: “合成分区bin文件”**：调用合成分区bin文件.bat，在vscode软件输出窗口命令行运行合成
* **6: “启动打包升级工具”**：启动串口升级工具.exe
* **7: “烧录user code”**：烧录user\_code.bin

## 5. CI230X系列芯片语音部分SDK添加源文件方法

说明

CI230X系列芯片的SDK中如果用户增加了新的源代码文件，必须找到工程文件SDK\projects\cias\_aiot\_audio\project\_file\source\_file.prj，参照下列示例添加文件到工程编译。

### 5.1 添加.h头文件

* 找到//header file path.注释所在位置，参照示例填写.h头文件所在相对路径

```
//header file path.
include-path: driver/ci130x_chip_driver/inc
include-path: driver/boards
```

### 5.2 添加.c源文件

* 找到//source files.注释所在位置，参照示例填写.c源文件所在相对路径

```
//source files.
source-file: driver/ci130x_chip_driver/src/ci130x_core_eclic.c 
source-file: driver/ci130x_chip_driver/src/ci130x_core_timer.c
```

### 5.3 添加.a库文件

* 参照示例填写.a库文件所在相对路径

```
library-file: libs/libnewlib_port.a 
library-file: libs/libasr.a
```

## 6. CI230X系列芯片 语音部分SDK固件烧录

请先到启英泰伦AI开发者平台 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载语音开发工具： 启英泰伦语音离线开发工具.zip ，CI230X系列芯片的wifi固件和语音固件都将通过该工具进行固件的制作与烧录。

![固件打包页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/CI230X_SDK_audio_offline_tool.png)

图 6-1

### 6.1 步骤一 打包升级固件

* 启动打包升级工具，进入固件打包界面，选择芯片->导入固件分区文件->自动计算分区文件地址->打包固件，如果固件生成太大，请改变flash大小，如图所示：

![固件打包页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-12.png)

图 6-2

![固件打包页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-12-1.png)

图 6-3

* 点击固件升级，烧录合成固件(如果用我司开发板CI-E05G02S\_MB，需要将拨码开关拨到VOICE进行烧录，开发板板图参考图6-5)

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-13.png)

图 6-4

* 开发板示意图，烧录语音固件需要将拨码开关拨到voice一侧

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E5%BC%80%E5%8F%91%E6%9D%BF%E7%83%A7%E5%BD%95%E8%AF%AD%E9%9F%B3%E5%9B%BA%E4%BB%B6.png)

图 6-5

### 6.2 步骤二 烧录use code

* 针对调试中只修改了use code分区的应用，可以直接在vscode编译器中烧录，无需重新对固件进行打包升级，升级方法如下：
* 右击工程名，选择烧录use code（或快捷键Ctrl+F11），按照如下提示进行硬件复位：

![烧录user code过程界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-14.png)

图 6-5

* 升级完成界面如下：

![烧录user code完成界面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-15.png)

图 6-6

## 7. CI230X系列芯片 语音部分SDK常见问题

### 7.1 升级use code失败

![11](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-19.png)

图 7-1

* 原因是当前烧录的use\_code.bin的大小超出了固件中user分区预留的大小，解决办法，使用固件打包升级工具，重新打包固件，将use分区调大至合理值，参考章节6.1。

## 8. CI230X系列芯片 WIFI部分SDK编译开发环境搭建

### 8.1. Python3安装 :

* 双击Python3安装包，以默认方式安装，目录可以自定义 eg;D:\works\TOOLS\LNxxxx\_tool;
* PATH 环境变量设置：打开环境变量编辑器，编辑用户或者系统的环境变量PATH，添加对应的 python3 安装路径，如图所示
* 进入到python3安装路径，复制 python.exe 并重新命名为 python3.exe

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-01.png)

图 8-1-1

* 确认python是否安装成功： python3 –version （重新打开命令行终端使用该命令）,安装成功会显示版本信息 ，如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-02.png)

图 8-1-2

### 8.2. ARM GCC工具链安装

* 安装gcc-arm-none-eabi-10-2020-q4-major-win32.exe 到指定目录即可；（注意：安装路径一定不要以数字开头，并且安装路径不要出现空格，不然编译打包时会报错），如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-03.png)

图 8-2-1

* 添加 工具链到系统环境变量PATH中：如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-04.png)

图 8-2-2

* 添加一个环境变量名：CROSS\_TOOLCHAIN\_ROOT，变量值：D:\GNU\_Arm\_Embedded\_Toolchain\2020-q4-major ，如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-05.png)

图 8-2-3

* 确认arm-none-eabi-gcc工具链是否安装成功： arm-none-eabi-gcc –version（重新打开命令行终端使用该命令）,安装成功会显示版本信息，如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-06.png)

图 8-2-4

### 8.3. ARM GCC工具链安装

* 安装 cmake-3.17.2win64-x64.msi 到指定目录
* 添加Cmake安装路径到环境变量中，如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-07.png)

图 8-3-1

* 确认Cmake是否安装成功： cmake –version （重新打开命令行终端使用该命令）,安装成功会显示版本信息，如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-08.png)

图 8-3-2

### 8.4. Ninja 安装

* 解压安装包EXE文件到指定目录：xxx\xxx\ninja-win，不需要安装。
* 添加Ninja安装路径到环境变量中，如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-09.png)

图 8-4-1

* 确认Ninja是否安装成功：Ninja –version （重新打开命令行终端使用该命令），安装成功会显示版本信息，如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-10.png)

图 8-4-2

## 9. CI230X系列芯片 WIFI部分SDK编译步骤（注意：在导入WIFI SDK时，尽量不要让导入路径太长，否则容易编译报错）

* 在vscode中新建终端，然后开始执行编译指令，参考下面编译指令，编译结果如图所示
* 清除结果： python3 start\_build.py clean
* 编译工程： python3 start\_build.py build
* 清除、编译二合一： python3 start\_build.py rebuild

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-11.png)

图 9-1

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-12.png)

图 9-2

## 10. CI230X WIFI部分SDK固件烧录

* 进入wifi烧录界面，然后点击编辑（选择固件，设置烧录地址（默认从0x00开始），设置波特率（一般设置为2Mbits/s），如图所示

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-13.png)

图 10-1

* 让设备进入烧录模式（按住BOOT按键，并重启设备，重启以后可以松开boot按键）
* 设备重启以后点击开始下载图标进行下载，开发板的拨码开关需要拨到wifi一侧

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/CI-E05GT02S_MB_V3.0.png)

图 10-2

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-14.png)

图 10-3

## 11.鉴权文件烧录

启英泰伦-语音开发工具，目前集成了腾讯IOT鉴权文件烧录，腾讯TVS云鉴权文件烧录，涂鸦云鉴权文件烧录功能，用户根据自己使用的云平台进行鉴权文件的烧录，同时也可以通过SDK 参考现有的鉴权文件烧录代码，进行扩展实现自己平台的鉴权文件烧录功能；需要注意：如果使用的是我司提供的开发板，需要将拨码开关拨到WIFI一侧才能进行鉴权文件烧录(请参考WIFI固件烧录说明)。

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E9%89%B4%E6%9D%83%E6%96%87%E4%BB%B6%E7%83%A7%E5%BD%95.png)

图 11-1

* 第一步：需要打开串口，串口使用语音固件烧录串口，波特率使用115200，如上图标注1
* 第二步：选取云平台类型，如上图标注2
* 第三步：设置/获取鉴权文件，如上图标注3

腾讯IOT鉴权文件烧录：鉴权文件组成字段(产品ID,产品名称,鉴权秘钥)，烧录结果如下图：

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E8%85%BE%E8%AE%AFIOT%E9%89%B4%E6%9D%83%E6%96%87%E4%BB%B6%E7%83%A7%E5%BD%95.png)

图 11-2