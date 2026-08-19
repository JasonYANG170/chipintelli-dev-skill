<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/ -->

# 命令词自学习方案开发

## 1 方案介绍

### 1.1 方案背景与目标

中国地域广阔，方言种类众多，主要分为七大类（如粤语、闽南语、吴语、湘语、赣语、客家话等），每种方言又细分为多个子方言和口音。由于这些方言在发音、词汇、语法上与普通话存在较大差异，传统的基于普通话训练的语音识别系统难以准确识别，为了解决这一痛点，启英泰伦为BNPU的发明者，采用BNPU的高效AI计算能力，推出了命令词自学习方案。

### 1.2 应用原理

将说话人语音特征通过深度神经网络进一步转化为深度声学特征，学习时，将该特征记录为模板，识别时提取的将深度声学特征与该模板采用DTW技术进行累积距离计算，从而进行识别。

### 1.3 学习流程

本方案中，启英泰伦AI语音芯片CI13XX或CI13LC通过语音指令或者其他主动指令开始进行自学习流程，学习流程如下：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_1.png)

图1.3 命令词自学习流程

参考视频链接：☞[离线语音灯头自学习方案演示（国内版）](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88/%E6%96%B9%E6%A1%8810%EF%BC%9A%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E7%81%AF%E5%A4%B4%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E6%BC%94%E7%A4%BA%EF%BC%88%E5%9B%BD%E5%86%85%E7%89%88%EF%BC%89/)

## 2 开发准备工作

开发前，请先准备好相关如下硬件，软件，如涉及量产，请从查看☞[生产测试](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E7%94%9F%E4%BA%A7%E6%B5%8B%E8%AF%95/)及☞[联系我们](https://document.chipintelli.com/%E8%81%94%E7%B3%BB%E6%88%91%E4%BB%AC/)启英泰伦官方获取生产支持：

### 2.1 开发硬件准备

2.1.1 本方案建议使用硬件模块D03GS01J或者F162GS02J进行：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_2.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_3.png)

2.1.2 下图是串口烧录工具

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_4.png)

图2.1.2 串口工具实物图

备注

开发者可以采用通用的串口工具进行固件烧录和查看日志，或进行点击[购买链接](http://mall.chipintelli.com/chip2?product_id=60)

特别注意：为了数据稳定，尽量选择带晶振的串口，推荐选择CH341 的串口调试工具（[启英泰伦AI语音uart串口调试工具](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%B5%84%E6%BA%90/UART%E4%B8%B2%E5%8F%A3%E8%B0%83%E8%AF%95%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)）

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_5.png)

### 2.2 软件开发准备

2.2.1 注册并登录AI开发平台：[https://aiplatform.chipintelli.com](https://aiplatform.chipintelli.com/)

2.2.2 获取对应芯片的含有自学习功能的SDK软件开发包：<https://aiplatform.chipintelli.com/attachment>

CI 13XX算法SDK：CI13XX\_SDK\_ASR\_ALG\_Vx.x.x

CI 13LC普通SDK：CI13LC\_SDK\_ASR\_Offline\_Vx.x.x(该功能在SDK中的cwsl\_sample工程中)

## 3 方案开发

### 3.1 方案一：语音识别固件及SDK开发

3.1.1 登陆启英泰伦语音AI平台，进入该组件。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_6.png)

3.1.2 选择语音识别固件及SDK开发。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_7.png)

3.1.3 新建一个工程，这边以1302芯片型号进行测试(需要选择13LC的自行更改芯片型号)。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_8.png)

3.1.4 创建一个工程，注意做自然说工程是没有自学习。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_9.png)

3.1.5 点击继续，该页面是对固件的参数进行配置，按照自己需求选择。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_10.png)

3.1.6 点击继续，下载附件样例表格，填写词条、播报音、协议。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_11.png)

3.1.7 同时在该页面勾选自学习选项。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_12.png)

3.1.8 根据提示填写自学习的词、播报音等，填写完毕后直接提交生成固件。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_13.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_14.png)

3.1.9 下载生成的固件，烧录到demo板，即可体验。

### 3.2 方案二：SDK+语言模型开发

该开发方案需要配合SDK和平台上的语言模型开发组件一起使用

3.2.1 软件准备

以下SDK可以支持命令词自学习功能：

CI 13XX算法SDK：CI13XX\_SDK\_ASR\_ALG\_Vx.x.x

CI 13LC普通SDK：CI13LC\_SDK\_ASR\_Offline\_Vx.x.x(该功能在SDK中的cwsl\_sample工程中)

3.2.2 生成带有自学习命令词的语音模型

①登陆AI平台，选择语音模型开发组件

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_15.png)

②新建一个项目，选择对应应用场景，芯片型号（13LC注意选择CI13LC系列）等。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_16.png)

③继续下一步，选择对应的模型，下载附件样例表格，填写对应的词

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_17.png)

④表格里面需要填写除了基本命令词外，还需要常规的自学习词条，如下图

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_18.png)

⑤填写完成后上传提交即可生语言模型，下载下来替换初始的文件

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_19.png)

3.2.3 生成对应的播报音

①登陆AI平台，选择播报音合成组件

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_20.png)

②新建一个项目，选好对应的音色，下载表格填写播报音

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_21.png)

③填写相关的命令词回复播报和自学习的播报

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_22.png)

④填写完成后上传提交即可生成对应的播报音文件，下载下来替换初始的文件。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_23.png)

3.2.4 语音模型文件和播报音移植（以130x为例）

①移植dnn文件

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_24.png)

②移植asr文件

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_25.png)

③移植表格cmd\_info文件

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_26.png)

④移植播报音voice

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_27.png)

⑤所有文件移植完成，下面是关于cmd\_info表格和播报对应关系说明，如图

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_28.png)

⑥注意表格修改完成后，还有另一个表格需要同步一下修改的内容，如图表格

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_29.png)

3.2.5 软件配置

①130x算法SDK：CI13XX\_SDK\_ASR\_ALG\_Vx.x.x，根据下图配置

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_30.png)

②13LC普通sdk：直接使用CI13LC\_SDK\_ASR\_Offline\_Vx.x.x(该功能在SDK中的cwsl\_sample工程中)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_31.png)

3.2.6 软件调试

调试以1302算法sdk：CI13XX\_SDK\_ASR\_ALG\_Vx.x.x为例子，同样的13LC调试步骤也是一样的。

①SDK相关自学习的配置在user\_config.h里面，如图：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_32.png)

开发者可以根据上图注释，调整一些自学习相关配置，如可以学习唤醒词数量，灵敏度，学习的次数等。

②唤醒词就一个，不用调整，只需要调整对应的命令词即可

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_33.png)

③匹配学习流程对应的命令词和播报音

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91_34.png)

④整个自学习修改完成，把修改的sdk程序编译打包即可烧录demo板体验