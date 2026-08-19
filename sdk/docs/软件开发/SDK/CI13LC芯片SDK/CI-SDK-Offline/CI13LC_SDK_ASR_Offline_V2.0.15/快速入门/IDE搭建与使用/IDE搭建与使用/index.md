<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/ -->

# IDE 搭建与使用

## 1 简介

CI13LC系列芯片的SDK主要是使用C语言开发，VSCode（Visual Studio Code）及其插件市场的插件对C语言的支持很友好，强大的插件功能支持自定义插件。所以被选为CI13LC系列芯片的SDK的默认编辑器，加上启英泰伦的编译插件，以及一系列工具，组成了CI13LC系统芯片的SDK的IDE。

备注

**IDE（启英泰伦集成开发环境，Integrated Development Environment ）** 是用于提供程序开发环境的应用程序，一般包括代码编辑器、编译器、调试器和图形用户界面等工具。开发者需要通过官方推荐的集成开发环境 (IDE) 对启英泰伦软件开发包（离线语音识别SDK）进行编辑、编译、链接、调试等操作。

## 2 下载Visual Studio Code

Microsoft Visual Studio Code 是适用于 Windows、macOS 和 Linux 的免费、功能强大的轻型代码编辑器，基于开源项目vscode构建。该编辑器集成了所有一款现代编辑器所应该具备的特性，包括语法高亮（syntax high lighting），可定制的热键绑定（customizable keyboard bindings），括号匹配（bracket matching）以及代码片段收集（snippets），以及拥有对 Git 的开箱即用的支持。

* Visual Studio Code☞[免费下载链接](https://code.visualstudio.com)

## 3 安装Visual Studio Code

* 双击运行下载好的安装文件，在弹出的许可协议界面选同意协议，再点击下一步。

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-1.png)

* 勾选下列配置，再点下一步。

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-2.png)

* 点击安装按钮。

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-3.png)

* 等待安装完毕即可。

## 4 打开CI13LC系列芯片SDK工程的方法

如果还没有下载CI13LC系列的SDK, 可以到☞[启英泰伦语音AI平台-开发资料](https://aiplatform.chipintelli.com/attachment)中进行搜索下载。
方法一：进入该SDK文件夹（.vscode文件夹所在的目录），空白处右键“通过Code打开”：

![1760498099731](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-099731.png)

方法二：选中CI13LC\_SDK文件夹（.vscode文件夹的上一级），右键“通过Code打开”：

![1760498271375](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-271375.png)

方法三：先打开vscode软件，如图3-6通过菜单栏->文件->打开文件夹，再如图3-7选中CI13LC\_SDK文件夹（.vscode文件夹的上一级）

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-36.png)

![1760498299595](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-299595.png)

## 5 安装CI工程管理插件

* 点击下图所示左侧软件商店图标，再点右上角三个省略号图标。

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-4.png)

* 在弹出菜单中，选择“从VSIX中安装” 或 “Installed from VSIX”。

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-5.png)

* 通过文件选择对话框找到(SDK根目录)/tools/ci-tool-1.1.0.vsix文件，并选择安装，等待安装完成即可。

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-6.png)

* 安装完成后，在Visual Studio Code左侧边栏会显示安装好的插件，点击可以打开插件界面。

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-100.png)

## 6 为CI工程管理插件设置编译器路径

* 选择[GCC编译工具链](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E8%B5%84%E6%BA%90/gcc/)的路径，当鼠标移动到插件界面内部时，右上角会显示“设置编译器路径”，点击“设置编译器路径”:

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-9.png)

* 选择编译器路径：

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-10.png)

* 设置完成后，就可以通过工程名后面的编译按钮进行编译了。

![img](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/img/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8-11.png)