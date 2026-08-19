<!-- Source: https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E5%BD%95%E9%9F%B3%E6%96%B9%E6%B3%95%E5%8F%8A%E6%B5%81%E7%A8%8B/ -->

# 小语种录音方法及流程

用户需要开发支持中国地方方言或者外国小语种的语音识别方案产品时，可以在启英泰伦[语音AI平台](https://aiplatform.chipintelli.com/)上进行相关语种的语言模型开发，平台当前已支持超过11国小语种，并在持续更新中；

![image-20230505140946122](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D4.jpg)

若开发平台中暂时没有您所需的地方方言或者外国小语种时，就可以用到“小语种录音方法及流程”，让您所开发的产品识别方案拥有识别中国地方方言或外国小语种的能力。

## 1、词条确定

当前规定制作小语种语言模型时，所使用的唤醒词和命令词的总词条数之和不能大于20条（唤醒词数+命令词数≤20条），命令词的制定规则可参照[《命令词播报词列表V3\_英文模板》](https://document.chipintelli.com/语音AI平台/语音AI平台使用指南/语音AI平台上传附件填写规范/#33_);

### 1.1、小语种命令词词条确定

开发小语种语音产品，需先在电脑上创建一个cmd.txt文本，将拟定的唤醒词及命令词填入其中（若有空格必须用中横线代替），并保存备用，下面以俄语小语种为例：

![image-20230505145054235](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D5.jpg)

## 2、录音设备选择

### 2.1、官方录音设备

推荐采用我司“开发板”配合“专用采音板”连接PC电脑进行小语种音频的录制；

![image-20230505153129362](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D6.jpg)

#### 2.1.1、开发板

开发板我们选用搭载启英泰伦三代智能语音识别芯片的开发板CI-D06GT01D三代离线语音开发板，可在我司官网进行[样品购买](http://mall.chipintelli.com/0?product_id=74&brd=1)。

![image-20230505153736317](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D7.jpg)

#### 2.1.2、采音板

采音板，是专用来配套开发板通过IIS传输对音频进行实时采集的重要外设，可在我司官网进行[样品购买](http://mall.chipintelli.com/chip2?product_id=28&brd=1)。（注意：以启英商城当下售卖的型号款式为准）

![image-20230505154211906](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D8.jpg)

#### 2.1.3、组装并连接电脑

①将采音板连接开发板，CI-D06GT01D三代离线语音开发板可直接将采音板对应排针插入排母中。最后将开发板与采音板分别使用type-c数据线与micro-usb数据线连接到电脑。

![image-20230505161913152](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D12.jpg)

②开发板及采音板均连接好windows7及以上版本的PC电脑后，需要对电脑进行简单的录音前配置。首先打开“控制面板”，点击“声音”；

![img](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D1.jpg)

③分别点击“录制”、采音板对应的“麦克风”和“属性”；（录音麦克风设为默认设备）

![img](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D2.jpg)

④在麦克风属性中，点击“高级 ”，在默认格式中选择“2通道，16位，16000Hz…”，最后点击“确定”即可完成配置。

![img](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D3.jpg)

### 2.2、其他录音设备（使用官方录音设备可忽略）

当然除了我司推荐的录音设备外，还可根据实际情况选用以下高保真设备进行录制，效果会略逊一筹；

![image-20230505154823060](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D9.jpg)

## 3、录音软件安装

### 3.1、下载“音频录制及处理工具包”

在启英泰伦语音AI平台中的开发资料中，输入“音频录制及处理工具包”到文件名称进行查询，然后下载对应工具包到电脑中。

![image-20230505154823060](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D13.jpg)

### 3.2、安装PC端录音软件

客户需要解压“音频录制及处理工具包”，首先打开“1、安装音频采集工具”文件夹，安装开源录音工具软件Audacity，这里我们以该录音软件为例进行讲解。

![image-20230505164527946](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D14.jpg)

![image-20230505154947496](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D10.jpg)

### 3.3、为WPS安装VBA库（MS office无需安装此库）

在使用“记录录音时间标签工具”时需要用到office工具，若用户电脑上安装的是WPS请安装该文件夹中的vba库，若用户电脑上安装的是MS office可跳过此步骤。

![image-20230505164556616](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D15.jpg)

### 3.4、安装记录录音时间标签工具

打开“3、安装记录录音时间标签工具”文件夹解压其中的“记录录音时间标签工具\_V2.3.zip”压缩包即可得到名为“release\_standor”的记录录音时间标签工具，其中包含有“Excel录音工具使用说明-2020-09-09.docx”可供使用时学习。

![image-20230505165052418](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D16.jpg)![image-20230505165041828](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D17.jpg)

### 3.5、安装音频切分处理工具

打开“4、安装音频切分处理工具”文件夹解压其中的“AudioProcessing.zip”压缩包即可得到名为“AudioProcessing”的音频切分处理工具，其中包含有“音频处理工具使用说明.pdf”可供使用时学习。

![image-20230505165320324](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D18.jpg)

### 3.6、开发板下载录音专用固件

#### 3.6.1、开发板下载口连接

将两个跳帽插入J2排针；

![image-20230506104123920](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D29.jpg)

#### 3.6.2、固件烧录下载

打开“5、开发板录音固件”文件夹，根据其中“固件烧录步骤.pdf”文档的第3节、第4节步骤，用”PACK\_UPDATE\_TOOL.exe”工具对“CI-D06GT01D开发板录音固件\_V101.bin”固件进行烧录。

![image-20230506102920816](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D31.jpg)![image-20230506104748798](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D28.jpg)

## 4、录音条件及要求

### 4.1 、参与音频录制人数

参与音频录制人数需满足以下公式，并且男女比例需均衡尽量满足1:1；

![image-20230505150828715](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D32.jpg)

### 4.2、录音环境

录音设备需要搭建在相对安静的房间环境中，混响小无明显回声的房间为宜。参与音频录制的人员与采音设备麦克风之间的距离应为3-5m。

![image-20230505152139045](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D30.jpg)

## 5、开始录制小语种语料

录音人员准备进入房间录音前，需要进行如下操作：

### 5.1、配置“记录录音时间标签工具”

①打开工具包中的记录录音时间标签工具“release\_standor”文件夹，双击“record.xlsm”，（需要启用宏）

![image-20230505170611733](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D19.jpg)

②填入之前拟定的唤醒词及命令词到Excel表格第一列中，并填入ENG结束；

![image-20230505171012245](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D20.jpg)

### 5.2、配置“Audacity”录音软件

①打开“Audacity”录音软件，点击“音频设置”，进入“音频设置…”

![image-20230506163143992](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D22.jpg)

②设置默认采样格式（16 位 ）和采样率（16000Hz），点击“确定”：

![image-20230506163333654](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D36.jpg)

### 5.3、录音人员开始录音

①1名操作员专职操作设备，1号录音人员进入房间坐在距离录音设备3~5m的位置，操作员在电脑上操作，点击”录制“按钮；

![image-20230505174453014](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D33.jpg)

②点击Excel表中的start record按钮，手动按下键盘上的↓箭头，录音人员在听到报提示音（现在开始测试）后，即可念出Excel表中对应的命令词；念完一个命令词后继续按下键盘上的↓箭头，念错后可按键盘↑箭头回到上一个命令词重新念。

![image-20230505175115416](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D23.jpg)

③该录音人员念完全部命令词后，操作员点击停止按钮停止录音；

![image-20230505175939897](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D24.jpg)

④删除提示音（哒哒哒......现在开始测试）之前的全部无效音频；

![image-20230506152821956](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D35.jpg)

⑤然后，导出音频为wav格式，命名为“录音人员姓名.wav”；

![image-20230505180132873](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D26.jpg)

⑥1号录音人员的音频录制完毕后，即可更换下一位录音人员进入房间重复之前步骤进行录制，Excel表同时需要进行ID+操作。

![image-20230505195544778](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D25.jpg)

## 6、音频切分处理

所有录音人员的音频录制完毕后，就需要把所有音频拆分成按命令词分类的文件夹；

### 6.1、音频切分

使用“AudioProcessing”工具将“记录录音时间标签工具”生成的时间标签与录制好的“录音人员姓名.wav”文件按要求放入其中，工具会自动切分出各个录音人员的音频文件；（具体步骤按照“音频处理工具使用说明.pdf”操作，音频务必转为单声道）

![image-20230505200422740](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E5%B0%8F%E8%AF%AD%E7%A7%8D%E8%AE%AD%E7%BB%83/img/%E5%B0%8F%E8%AF%AD%E7%A7%8D34.jpg)