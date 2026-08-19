<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E9%AB%98%E5%99%AA%E3%80%81%E9%9C%87%E5%8A%A8%E3%80%81%E8%BF%91%E8%B7%9D%E7%A6%BB%E4%BA%A7%E5%93%81%E7%9A%84%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91-CI1302/ -->

# 高噪、震动、近距离产品的语音方案开发

## 一、方案介绍

### 1.1 方案背景与目标

一些使用场景如厨房，浴室等背景噪声比较大，对传统语音识别方案有这比较大的挑战。有些机器运行时产生的机械震动可能导致麦克风拾音失真，甚至硬件损坏。近距离语音交互用户需贴近设备发声（如头盔通讯、手持工具等），需解决近场识别等问题。

启英泰伦在语音识别领域深耕多年，对以上痛点有着行之有效的解决方案，接下来就为大家介绍启英泰伦语音识别方案是如何解决以上痛点的。

## 二、开发准备工作

开发前，请先准备好相关如下硬件，软件，如涉及量产，请从查看☞[生产测试](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E7%94%9F%E4%BA%A7%E6%B5%8B%E8%AF%95/)及联系启英获取生产支持：

### 2.1 开发硬件准备

1、本方案建议使用硬件模块D02GS01J进行：

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E9%AB%98%E5%99%AA%E9%9C%87%E5%8A%A8%E8%BF%91%E8%B7%9D%E7%A6%BB%E4%BA%A7%E5%93%81%E7%9A%84%E8%AF%AD%E9%9F%B3%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91-1.png)

图2.1 CI-D02GS01J 实物图

备注：本模块贴不同芯片，对应的模块编号不同，总共3中型号：CI-D01GS01J（贴1301），CI-D02GS01J（1302），CI-D03GS01H（1303），点击获取：☞[CI-D02GS01J模块板说明](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI13XX%E7%B3%BB%E5%88%97/CI-D0XGS01J%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/)、点击获取[购买链接](http://mall.chipintelli.com/chip2?product_id=85&brd=1)

2、下图是串口烧录工具

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E9%AB%98%E5%99%AA%E9%9C%87%E5%8A%A8%E8%BF%91%E8%B7%9D%E7%A6%BB%E4%BA%A7%E5%93%81%E7%9A%84%E8%AF%AD%E9%9F%B3%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91-2.png)

图2.2 串口工具实物图

备注：开发者可以采用通用的串口工具进行固件烧录和查看日志，或进行点击☞[购买链接](http://mall.chipintelli.com/chip2?product_id=60)

特别注意：为了数据稳定，尽量选择带晶振的串口，推荐选择CH341 的串口（☞[CH340官方驱动下载](https://www.wch.cn/products/CH340.html)）

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E9%AB%98%E5%99%AA%E9%9C%87%E5%8A%A8%E8%BF%91%E8%B7%9D%E7%A6%BB%E4%BA%A7%E5%93%81%E7%9A%84%E8%AF%AD%E9%9F%B3%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91-3.png)

### 2.2 软件准备

1、开发环境搭建：☞[VScode下载及使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)：

2、SDK 代码包获取: ☞[语音AI平台 - 资料库](https://aiplatform.chipintelli.com/attachment)

## 三、高噪、震动、近距离产品的语音方案开发

### 3.1 将动态增益改为固定增益

在一些类似按摩仪，头盔等使用距离较近的产品，麦克风的拾音效果可能会出现削顶现象，如下图红色箭头标记处，这种情况我们可以将动态增益改为固定增益，另外麦克风位置距离噪声源很近并且噪声源主要是稳态噪声的情况下比如风扇，油烟机等产品，若识别不好也可以尝试采用此算法进行优化。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E9%AB%98%E5%99%AA%E9%9C%87%E5%8A%A8%E8%BF%91%E8%B7%9D%E7%A6%BB%E4%BA%A7%E5%93%81%E7%9A%84%E8%AF%AD%E9%9F%B3%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91-4.png)

图3.1拾音出现削顶现象

以下解释几个专业名词：

\*\*ALC\*\*叫自动增益调整，当语音信号幅度过小时，则自动提高增益，把信号自动调大，便于识别。语音信号幅度过大时，则自动降低增益，把信号自动调小，便于识别。上电是默认增益为28.5，之后不同环境下，增益是自动变化的。

动态ALC即是动态控制ALC的开和关，在ALC调整放大过大时（ALC提高或降低就那么几个档位，有可能一下子把信号放得过大了削頂），会自动把ALC关掉，改变为固定增益，从而降低信号幅度，避免削頂，保障基本的识别。之后当外部信号变小时，又会自动打开ALC，不同环境下，增益是自动变化的，所以称为动态增益。

固定增益就是关闭动态ALC以及关闭ALC，然后把增益固定为某一值。不同环境下，增益都是固定不变的。

CI130X芯片单麦方案的固定增益设置方法，先关闭动态ALC，后在audio\_in\_manage\_inner.c的audio\_in\_manage\_inner\_task()函数开始位置添加下面两行代码，设置固定增益并配置参数，参考如下图代码所示。

cm\_set\_codec\_alc(HOST\_MIC\_RECORD\_CODEC\_ID,CM\_CHA\_LEFT,DISABLE);

cm\_set\_codec\_adc\_gain(HOST\_MIC\_RECORD\_CODEC\_ID,CM\_CHA\_LEFT,20); //关闭ALC后，设置左声道固定增益为20dB；增益参数定义范围 -18~28dB，增益建议调节范围：15~28dB；麦克风处噪声小向28方向调，麦克风处噪声大向15方向调。

![识别效果优化-8](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E9%AB%98%E5%99%AA%E9%9C%87%E5%8A%A8%E8%BF%91%E8%B7%9D%E7%A6%BB%E4%BA%A7%E5%93%81%E7%9A%84%E8%AF%AD%E9%9F%B3%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91-5.png)

图3.2

CI130X芯片双麦方案的固定增益设置方法，一般情况下，需要左右MIC都要设置初始化增益，参考如下：

cm\_set\_codec\_alc(HOST\_MIC\_RECORD\_CODEC\_ID,CM\_CHA\_LEFT,DISABLE); //关闭左声道ALC后，

cm\_set\_codec\_adc\_gain(HOST\_MIC\_RECORD\_CODEC\_ID,CM\_CHA\_LEFT,20); //设置左声道固定增益初始化为20dB；

cm\_set\_codec\_alc(HOST\_MIC\_RECORD\_CODEC\_ID,CM\_CHA\_RIGHT,DISABLE); //关闭右声道ALC后，

cm\_set\_codec\_adc\_gain(HOST\_MIC\_RECORD\_CODEC\_ID,CM\_CHA\_RIGHT,20); //设置右声道固定增益初始化为20dB；

CI13LC芯片单麦方案的固定增益设置方法如下图九代码所示，先关闭动态ALC，后在main.c的task\_init函数里的”audio\_in\_codec\_registe();"后面添加下面两行代码，设置固定增益并配置参数，参考如下所示：

cm\_set\_codec\_alc(HOST\_MIC\_RECORD\_CODEC\_ID,CM\_CHA\_LEFT, DISABLE);

cm\_set\_codec\_adc\_gain(HOST\_MIC\_RECORD\_CODEC\_ID, CM\_CHA\_LEFT, 15);//关闭ALC后，设置左声道固定增益为15dB；增益参数定义范围 -18~28dB，增益建议调节范围：15~28dB；MIC处噪声小向28方向调，MIC处噪声大向15方向调；目前80db~83db情况，设置到15比较合适。

![识别效果优化-9](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E9%AB%98%E5%99%AA%E9%9C%87%E5%8A%A8%E8%BF%91%E8%B7%9D%E7%A6%BB%E4%BA%A7%E5%93%81%E7%9A%84%E8%AF%AD%E9%9F%B3%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91-6.png)

图3.3

通过该算法的处理，我们可以看到采用固定增益后，声音信号幅值明降低了。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E9%AB%98%E5%99%AA%E9%9C%87%E5%8A%A8%E8%BF%91%E8%B7%9D%E7%A6%BB%E4%BA%A7%E5%93%81%E7%9A%84%E8%AF%AD%E9%9F%B3%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91-7.png)

图3.4

### 3.2 使用深度降噪算法（CI130X芯片）

一些产品在使用过程中会产生较大的噪音，比如油烟机以及电动窗帘和电动晾衣机工作时麦克风处结构噪音比较大，此类产品在开发过程中如果识别效果不好可采用深度降噪算法进行优化。

深度降噪算法抑制设备本身产生的高噪声，以提升低信噪比环境下的识别率，针对不同应用领域需使用对应的领域模型，当前只提供了烟机和窗帘的降噪模型。

1.CI130X\_SDK\_ALG\_PRO\_X.X.XX\projects\offline\_asr\_alg\_pro\_sample\project\_file\makefile文件中，将CI\_ALG\_TYPE := $(USE\_NULL)修改为CI\_ALG\_TYPE := $(USE\_DENOISE\_NN)

并且请把CI130X\_SDK\_ALG\_PRO\_X.X.XX\external\model\nn\_denoise(深度降噪)中的[60003]nn\_denoise\_xx.bin深度降噪算法模型，复制到CI130X\_SDK\_ALG\_PRO\_X.X.XX\projects\offline\_asr\_alg\_pro\_sample\firmware\dnn文件夹中，具体请参考：☞[算法SDK2.4.18开发流程](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E6%B7%B1%E5%BA%A6%E9%99%8D%E5%99%AA%E7%AE%97%E6%B3%95/)请注意，深度降噪需使用CI130X芯片搭配该算法的前端算法模型使用，如下图所示，深度降噪在AI平台制作ASR与DNN模型请选择深度降噪专用模型。

## 结构注意事项及重要物料选配

用于我司语音芯片的麦克风参数规格有两点需要注意的，灵敏度必须为-32±3dB，信噪比必须为≥65dB；推荐用户直接采用我司验证后的麦克风设计方案，可参考我司☞[外围器件兼容列表](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%A4%96%E5%9B%B4%E5%99%A8%E4%BB%B6%E5%85%BC%E5%AE%B9%E5%88%97%E8%A1%A8/)选择合适的麦克风。

另外，麦克风处有明显震动或者机器的结构噪音过大，麦克风处噪声达到75dB就会对识别有明显影响，这时需要进行结构上的优化，关于结构优化请参考☞[产品结构设计](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/)对机器结构进行优化。