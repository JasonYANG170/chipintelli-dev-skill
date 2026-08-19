<!-- Source: https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/%E7%A6%BB%E7%BA%BF%E8%87%AA%E7%84%B6%E8%AF%B4%E6%96%B9%E6%A1%88%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B%E6%8C%87%E5%BC%95/ -->

# 离线自然说方案平台开发流程指引

[★仅限企业用户](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/)

离线自然说，用户无需记忆固定词条，只需知道功能和唤醒词，即可语音控制设备，真正做到了自然、方便的人机交互，标志着离线语音识别2.0时代的到来。

开发自然说项目的固件及SDK，可在启英泰伦语音AI平台上进行无代码快捷开发；下面详细介绍整个自然说固件及SDK的开发步骤及流程。

## 一、注册登录

### 1.1、打开启英泰伦语音AI平台

首先，在浏览器中打开并登录“启英泰伦语音AI平台”开发页面☞“[aiplatform.chipintelli.com](https://aiplatform.chipintelli.com/)”;

![image-20240507142700284](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240507142700284.png)

### 1.2、注册账号

还未注册的用户点击页面“登录注册”按钮，填写基础信息及手机验证即可注册成功，注册成功后登录语音AI平台即可；

![image-20240507154428597](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240507154428597.png)

### 1.3、企业用户认证

开发自然说项目需要企业用户权限，故需要点击右上角头像，在弹出菜单中选择企业认证。详细操作请参考“[企业用户认证流程](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/)”；

![image-20240507154741328](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240507154741328.png)

## 二、开发自然说固件及SDK

### 2.1、产品固件及SDK深度开发功能

注册成为企业用户后，点击“功能开发”板块中的“产品固件及SDK深度开发”功能；

![image-20240507142124441](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240507142124441.png)

### 2.2、新建项目

点击“+ 新建项目“；

![image-20240507142531534](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240507142531534.png)

### 2.3、填写产品信息

2.3.1、填写该自然说项目的“产品信息”，填入自定义产品名称；在选择“产品方案”时，需选择“单麦离线自然说方案”；

![image-20240506192154135](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506192154135.png)

2.3.2、“产品类型”选择所需要开发的产品类型（可进行搜索选择），若无对应产品类型则可点击“新增类别”进行产品类型的添加；

![image-20240506193111850](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193111850.png)

2.3.3、“芯片型号”与“描述”填写好后，点击“创建”进入后续参数及基本信息的填写；

![image-20240506193155141](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193155141.png)

### 2.4、填写基本信息

基本信息需要填写版本号，版本号可从V1.0.0开始向上增加；“语言类型”选择中文；“选择声学模型”可根据问号(?)内提示选择较新版本的声学模型；“制作类型”需要选择“自然说”；“模块板选择”中需要选择项目所使用的开发板型号。

![image-20240506193247286](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193247286.png)

### 2.5、固件参数配置

固件参数配置均有默认配置，可根据项目需求并参考说明进行修改；还可打开“引脚配置”对未使用的引脚进行初始化操作，生成到本项目的SDK中；然后点击“继续”。

![image-20240506193321051](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193321051.png)

### 2.6、编辑命令词

2.6.1、首先，需要选择识别到指令后播报音的音色，有很多音色可供试听选择，可根据产品特点进行选择；播报声音的语速及音量可以先保持默认值；播报音压缩比在flash空间充足的情况下也可选择默认以保证音质。

![image-20240506193358368](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193358368.png)

2.6.2、继续向下滑动页面，本步骤也是最关键的一个步骤，就是编辑我们的自然说指令；这里可直接点击“最小功能词推荐”；

![image-20240506193428858](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193428858.png)

2.6.3、点击“最小功能词推荐”按钮会弹出勾选框，可按照项目产品的具体功能进行勾选，勾选完毕点击“确定”；

![image-20240506193441194](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193441194.png)

2.6.4、确定完最小功能后，则会显示在下方表格中；然后需要填入唤醒词、播报语句、播报模式、发送协议与接收协议等信息；

（备注：当然，若你的产品没有最小功能词推荐，则可以下载“附件样例”并根据其中规则填写后上传，或直接根据规则在页面表格中进行填写。）

![image-20240506193533544](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193533544.png)

2.6.5、最后，可勾选SDK选项，点击“立即提交”，平台会跳转到固件版本管理页面；

![image-20240506193608347](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193608347.png)

### 2.7、提交生成

在版本管理页，此时“操作”流程中会显示“暂无下载”状态，只需耐心等待。

![image-20240506193628216](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240506193628216.png)

## 三、固件及SDK下载烧录

耐心等待十分钟左右，即可看到“操作”流程的“暂无下载”变为了“下载文件”，即可点击下载进行固件的烧录及SDK工程包的开发工作。

### 3.1、点击“下载文件”；

点击“下载文件”按钮，将其保存到本地电脑中；

![image-20240507093818215](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240507093818215.png)

### 3.2、烧录到开发板进行验证

下载得到.zip压缩文件，解压后的文件夹中包含SDK开发包、“XXX.bin”产品固件、串口协议列表和《固件烧录步骤》文档，如下图中内容，请打开《固件烧录步骤》文档；根据其步骤将固件烧录到开发板即可开始体验测试启英泰伦自然说语音识别效果。

![image-20240508105815499](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91/img/image-20240508105815499.png)