<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# 小程序微信开发者工具使用说明

小程序☞[开发工具](https://developers.weixin.qq.com/miniprogram/dev/devtools/devtools.html)说明

\*\*启英BLE小程序\_Release\*\*代码请到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中搜索下载

## 1. 微信开发者工具下载安装

微信开者☞[工具下载链接](https://developers.weixin.qq.com/miniprogram/dev/devtools/stable.html)

Windows7系统请选择1.05版本工具，其余可下载1.06版本的工具。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-1.png)

下载完安装包后，选择目标文件夹直接安装即可。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-2.png)

## 2. 小程序项目新建和导入

点击打开微信开发者工具，选择小程序项目，进行 **新建\导入** 小程序项目。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-3.png)

### 2.1 新建项目

* 需要一个小程序的☞[AppID](https://developers.weixin.qq.com/miniprogram/dev/framework/quickstart/getstart.html#申请账号)；如没有 AppID，可以选择申请使用☞[测试号](https://developers.weixin.qq.com/miniprogram/dev/devtools/sandbox.html)。
* 登录的微信号需要是该 AppID 的开发者；
* 需要选择一个空目录，或者选择的非空目录下存在 ☞[`app.json`](https://developers.weixin.qq.com/miniprogram/dev/framework/config.html) 或者 `project.config.json`。当选择空目录时，可以选择是否在该目录下生成一个简单的项目。
* 开发者工具提供非常丰富的模块，也可以选择一个模版来创建一个项目。
* 模板选择推荐：JS-基础模板。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-4.png)

### 2.2 导入小程序项目

直接导入已有的小程序项目或者启英泰伦sample小程序，需输入AppID或者使用测试号。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-5.png)

## 3. 微信开发者工具使用

开发者工具主界面，从上到下，从左到右，分别为：菜单栏、工具栏、模拟器、目录树、编辑区、调试器 六大部分。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-6.png)

### 3.1 菜单栏

* **界面** 选项设置主界面想要展示的内容。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-7.png)

### 3.2 工具栏

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-8.png)

* **模拟器、编辑器、调试器**：打开\关闭该功能界面的显示。
* **可视化**：打开模拟器和编译器功能，点击模拟器上的视图或组件，编译器上会显示对应的代码。
* **编译**：进行代码的编译。
* **真机调试**：小程序App在手机或电脑上进行运行调试。
* **清缓存**：可选择性清除小程序的数据缓存、文件缓存、授权数据、登录状态等信息，可灵活操作便于调试。
* **上传**：上传代码到☞[微信公众平台](https://mp.weixin.qq.com/)，平台上可进行审核发布等操作。
* **详情**：查看小程序项目的基本信息、性能质量、本地设置、项目配置。

### 3.3 模拟器

模拟器可以模拟小程序在微信客户端的表现，小程序的代码通过编译后可以在模拟器上直接运行。

**推荐机型尺寸宽度为375 px**， 这是因为在微信小程序中，rpx（responsive pixel）是一种根据屏幕宽度自适应的单位，规定屏幕基准宽度为750rpx。 例如，在iPhone X上， 屏幕宽度为375px，共有750个物理像素，1rpx约等于0.5px，这样设计时可以直接使用rpx单位，无需担心不同设备上的显示差异‌。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-10.png)

### 3.4 调试器

* 调试器 **Console**，开发者可以在此输入和调试代码，小程序的错误输出，会显示在此处。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-11.png)

* 调试器 **AppData**，显示当前项目运行时小程序AppData的具体数据，实时反映项目数据情况，用户可以在此处编辑数据，并及时反馈到界面上，用于查看页面数据与预期的正确性。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-12.png)

* 调试器 **Storage**，用于显示当前项目的使用 wx.setStorage 或者 wx.setStorageSync 后的数据存储情况。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%BE%AE%E4%BF%A1%E5%BC%80%E5%8F%91%E8%80%85%E5%B7%A5%E5%85%B7%E8%AF%B4%E6%98%8E-13.png)