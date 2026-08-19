<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E7%A5%9E%E7%BB%8F%E7%BD%91%E7%BB%9C%E9%99%8D%E5%99%AA%2B%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91-CI1303/ -->

# 单麦深度神经网络降噪+语音识别方案开发-CI1303

## 一. 方案介绍

### 1.1 方案背景

有的设备运行时，设备本身会产生较大的噪声，此时如果人声又比较小，信噪比不够会导致识别效果下降。为了解决这一痛点，启英泰伦推出了深度神经网络降噪算法方案。

### 1.2 方案原理

深度神经网络降噪算法通过抑制设备本身产生的高噪声，以提升低信噪比环境下的识别率。

### 1.3 方案实例

针对不同应用领域需使用对应领域的深度神经网络降噪模型，当前启英泰伦已推出了烟机和窗帘这两个领域的深度神经网络降噪模型。下面教程将以烟机领域为例，进行实战开发讲解。

## **二. 开发准备**

### **2.1 硬件准备**

**开发模块**：启英泰伦 CI1303模块板 (推荐型号 [CI-D03GS01J](http://mall.chipintelli.com/chip2?product_id=67&brd=1))

**烧录工具**：[USB转TTL串口调试工具](http://mall.chipintelli.com/chip2?product_id=60&brd=1)（用于固件烧录、实时通信验证、日志LOG打印等，可5V/3V3供电）

**杜邦线**：[杜邦线](http://mall.chipintelli.com/chip2?product_id=122)（用于固件烧录、实时通信验证、日志LOG打印等）

**麦克风、喇叭**：启英商城购买[麦克风、喇叭](http://mall.chipintelli.com/chip?product_category=18&brd=1)与模块板匹配，批量购买具体参数可参考：[麦克风兼容列表](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%A4%96%E5%9B%B4%E5%99%A8%E4%BB%B6%E5%85%BC%E5%AE%B9%E5%88%97%E8%A1%A8/)

**测试设备**：个人电脑（建议Windows 7及以上系统）

### **2.2 软件准备**

**开发环境搭建**：[快速入门](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/)

**软件SDK下载**：[CI13XX\_SDK\_ASR\_ALG\_V2.5.28](https://aiplatform.chipintelli.com/attachment)(*若有新版本，请使用最新版本的SDK*)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE1SDK%E4%B8%8B%E8%BD%BD.png)

### **2.3 资料获取**

1. [启英泰伦语音AI平台](https://aiplatform.chipintelli.com/)
2. [CI1303 芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI1301%26CI1302%26CI1303%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/)
3. [CI-D03GS01J 模块数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13XX%E7%B3%BB%E5%88%97/CI-D0XGS01J%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/)
4. [SDK 软件开发手册](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/)

## **三. 软件开发**

### 3.1 修改板级配置

打开CI13XX\_SDK\_ASR\_ALG\_Vx.x.x.x\projects\offline\_asr\_alg\_pro\_sample\app\app\_main\user\_config.h文件，手动新增CI-D03GS01J模块对应的板级配置文件。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE4%E6%9D%BF%E7%BA%A7%E9%85%8D%E7%BD%AE.png)

### 3.2 修改makefile

打开CI13XX\_SDK\_ASR\_ALG\_Vx.x.x.x\projects\offline\_asr\_alg\_pro\_sample\project\_file\makefile 文件，将CI\_ALG\_TYPE 修改为 CI\_ALG\_TYPE := $(USE\_DENOISE\_NN)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE2%E4%BF%AE%E6%94%B9makefile.png)

### 3.3 清理编译生成

修改makefile文件后，先clean清理后，再进行build生成。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE3%E6%B8%85%E7%90%86%E7%BC%96%E8%AF%91%E5%B7%A5%E7%A8%8B.png)

### 3.4 引用前端降噪算法模型

使用深度神经网络降噪算法时需要将CI13XX\_SDK\_ASR\_ALG\_Vx.x.x.x\external\model\nn\_denoise(深度降噪)\烟机\ [60003]nn\_denoise\_m34.bin 前端降噪算法模型，复制到CI13XX\_SDK\_ASR\_ALG\_Vx.x.x.x\projects\offline\_asr\_alg\_pro\_sample\firmware\dnn文件夹中。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE5%E5%89%8D%E7%AB%AF%E9%99%8D%E5%99%AA%E6%A8%A1%E5%9E%8B.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE6%E5%A4%8D%E5%88%B6%E5%89%8D%E7%AB%AF%E9%99%8D%E5%99%AA%E6%A8%A1%E5%9E%8B%E5%88%B0DNN%E6%96%87%E4%BB%B6%E5%A4%B9.png)

### 3.5 下载深度降噪声学模型

1、进入AI平台组件开发页面，选择语音模型开发；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE8%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E5%BC%80%E5%8F%91%E6%8E%A5%E5%8F%A3.png)

2、选择新建项目；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE9%E9%80%89%E6%8B%A9%E6%96%B0%E5%BB%BA%E9%A1%B9%E7%9B%AE.png)

3、选择深度降噪专用声学模型；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE7%E4%B8%8B%E8%BD%BD%E7%83%9F%E6%9C%BA%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E6%A8%A1%E5%9E%8B.png)

4、下载深度降噪声学模型；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE10%E4%B8%8B%E8%BD%BD%E5%A3%B0%E5%AD%A6%E6%A8%A1%E5%9E%8B.png)

5、复制深度降噪声学模型压缩包并解压到CI13XX\_SDK\_ASR\_ALG\_Vx.x.x.x\projects\offline\_asr\_alg\_pro\_sample\firmware\dnn文件夹中；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE11%E5%A4%8D%E5%88%B6%E8%A7%A3%E5%8E%8B%E5%A3%B0%E5%AD%A6%E6%A8%A1%E5%9E%8B.png)

### 3.6 生成语言模型

1、选择命令词合成语言模型，下载命令词样例表格；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE12%E4%B8%8B%E8%BD%BD%E6%A0%B7%E4%BE%8B%E8%A1%A8%E6%A0%BC.png)

2、根据产品需求，自定义唤醒词及命令词；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE13%E8%87%AA%E5%AE%9A%E4%B9%89%E5%91%BD%E4%BB%A4%E8%AF%8D.png)

3、上传命令词表格，生成asr语言模型和cmd\_info表格；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE14%E4%B8%8A%E4%BC%A0%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%A1%A8%E6%A0%BC.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE15%E6%8F%90%E4%BA%A4%E8%A1%A8%E6%A0%BC.png)

4、下载asr语言模型及cmd\_info表格；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE16%E4%B8%8B%E8%BD%BD%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B.png)

5、将[60000]{cmd\_info}表格复制到CI13XX\_SDK\_ASR\_ALG\_Vx.x.x.x\projects\offline\_asr\_alg\_pro\_sample\firmware\user\_file\cmd\_info文件夹下；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE17%E5%A4%8D%E5%88%B6cmdinfo%E8%A1%A8%E6%A0%BC.png)

6、将命令词和唤醒词网络语言模型复制到CI13XX\_SDK\_ASR\_ALG\_Vx.x.x.x\projects\offline\_asr\_alg\_pro\_sample\firmware\asr文件夹下；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE18%E5%A4%8D%E5%88%B6asr%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B.png)

### 3.7 TTS播报音合成

1、功能开发主页选择播报音合成；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE22%E6%92%AD%E6%8A%A5%E9%9F%B3%E5%90%88%E6%88%90.png)

2、新建播报音合成；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE23%E6%96%B0%E5%BB%BA%E6%92%AD%E6%8A%A5%E9%9F%B3.png)

3、选择合适的音色、语速及音量；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE24%E9%9F%B3%E8%89%B2%E9%80%89%E6%8B%A9.png)

4、下载播报音样例表格，自定义播报内容，提交开始合成播报音；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE25%E4%B8%8B%E8%BD%BDTTS%E6%A0%B7%E4%BE%8B%E8%A1%A8%E6%A0%BC.png)

5、下载播报音，并将原始音频复制到CI13XX\_SDK\_ASR\_ALG\_Vx.x.x.x\projects\offline\_asr\_alg\_pro\_sample\firmware\voice\src文件夹下；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE26%E5%A4%8D%E5%88%B6%E6%92%AD%E6%8A%A5%E9%9F%B3.png)

### 3.8 打包合成固件

1、点击合成分区bin文件，会自动合成asr.bin、dnn.bin、cmd\_info.bin、voice.bin、user\_code.bin；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE19%E8%87%AA%E5%AE%9A%E7%94%9F%E6%88%90%E5%88%86%E5%8C%BAbin%E6%96%87%E4%BB%B6.png)

2、打包合成固件；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE20%E6%89%93%E5%8C%85%E5%9B%BA%E4%BB%B6.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE21%E5%9B%BA%E4%BB%B6%E6%89%93%E5%8C%85.png)

### 3.9 固件升级及测试验证

1、点击固件升级按钮，进入固件升级界面；

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE27%E7%82%B9%E5%87%BB%E5%9B%BA%E4%BB%B6%E5%8D%87%E7%BA%A7.png)

2、使用杜邦线连接串口烧录工具和语音模块烧录口的5V、GND、TX、RX这几个引脚（注意TX接RX、RX接TX），将串口烧录工具与电脑连接，勾选正确的COM口，然后重新给语音模块上电(拔插一下5V)，语音模块将自动开始升级固件。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%9B%BE28%E5%8D%87%E7%BA%A7%E4%B8%8B%E8%BD%BD.png)

3、固件烧录完成后，将语音模块接上麦克风和喇叭，然后可以将语音模块放在真实烟机环境下进行效果体验测试。

### 3.10 注意事项

1、深度降噪需搭配该算法的前端算法模型使用。

2、深度降噪在AI平台制作ASR与DNN模型请选择深度降噪专用模型。