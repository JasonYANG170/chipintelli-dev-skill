<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E5%A3%B0%E9%9F%B3%E4%BA%8B%E4%BB%B6%E6%A3%80%E6%B5%8B/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B9%E6%A3%80%E6%B5%8B%E6%96%B9%E6%A1%88/ -->

# 男女声纹检测方案

## 1、方案介绍

男女声纹识别是一种通过深度学习方法对说话人的声音特征来区分说话者性别（男性或女性）的技术。它基于男女声音在生理和声学特性上的差异，通过算法模型自动判断说话者的性别。

---

## 2、硬件方案选型

（1）支持的芯片有1303 、1306等4M的flash的芯片，可以看☞[芯片规格说明](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/)；

（2）支持的模块CI-D02GS01J（芯片选1302）、CI-D03GS01J（芯片1303），☞[模块资料](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13XX%E7%B3%BB%E5%88%97/CI-D0XGS01J%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/)。

---

## 3、固件开发

### 3.1 编译环境搭建

如果是第一次用启英泰伦130X的SDK进行开发，则需要配置固件编译环境，如下☞[IDE 搭建与使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)

### 3.2 熟悉固件开发流程

如果是第一次用启英泰伦130X的SDK进行开发，则需要看一下☞[SDK如何快速开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/)。包括命令词添加，语言模型添加，播报音添加等。我们也提供☞[视频教程链接](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E7%AF%87/%E8%BD%AF%E4%BB%B6%E7%AF%8712%EF%BC%9A%E5%9F%BA%E4%BA%8ESDK%E5%BC%80%E5%8F%91%E5%9B%BA%E4%BB%B6%EF%BC%88%E7%AC%AC%E4%B8%80%E8%AE%B2%EF%BC%9A%E5%9F%BA%E4%BA%8ESDK%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%EF%BC%89/)

### 3.3男女声纹固件基础配置

（1）首先把CI130X\_SDK\_ALG\_PRO\_2.X.X\external\model\wman\_vpr(男女声纹)下的[60008]VGR\_model\_1027\_v8.bin文件（男女声纹模型）（图1所示）复制到工程目录下的firmware目录下的dnn目录下面（图2所示）。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E5%A3%B0%E9%9F%B3%E4%BA%8B%E4%BB%B6%E6%A3%80%E6%B5%8B/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B9%E6%A3%80%E6%B5%8B%E6%96%B9%E6%A1%88-1.png)

图1

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E5%A3%B0%E9%9F%B3%E4%BA%8B%E4%BB%B6%E6%A3%80%E6%B5%8B/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B9%E6%A3%80%E6%B5%8B%E6%96%B9%E6%A1%88-2.png)

图2

（2）然后在makefile中配置CI\_ALG\_TYPE := $(USE\_WMAN\_VPR)，如图3所示。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E5%A3%B0%E9%9F%B3%E4%BA%8B%E4%BB%B6%E6%A3%80%E6%B5%8B/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B9%E6%A3%80%E6%B5%8B%E6%96%B9%E6%A1%88-3.png)

图3

完成如上两个步骤以后，工程需要先点击清理再生成。如图4所示

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E5%A3%B0%E9%9F%B3%E4%BA%8B%E4%BB%B6%E6%A3%80%E6%B5%8B/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B9%E6%A3%80%E6%B5%8B%E6%96%B9%E6%A1%88-4.png)

图4

### 3.4男女声纹算法配置和对外接口

（1）算法配置如图6所示，可以根据宏的具体描述确认是否要调整

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E5%A3%B0%E9%9F%B3%E4%BA%8B%E4%BB%B6%E6%A3%80%E6%B5%8B/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B9%E6%A3%80%E6%B5%8B%E6%96%B9%E6%A1%88-5.png)

图5

（2）对外接口

调用vpr\_run\_one\_recognition();执行一次男女声纹识别，根据vp\_buffer\_identify[0]的大小来判断男女声学特征，如图7所示。也可以根据这个接口来做应用层逻辑。