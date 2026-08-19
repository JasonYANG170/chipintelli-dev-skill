<!-- Source: https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B%E6%8C%87%E5%BC%95/ -->

# 语音AI平台产品开发流程指引

欢迎开发者使用语音AI平台，请查看下列产品开发流程指引。

---

## 入门篇

### 1、初识语音

如果您是第一次接触智能语音产品，可以先了解下 ☞[语音识别原理](https://document.chipintelli.com/%E6%96%B0%E6%89%8B%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%8E%9F%E7%90%86/ "点击查看")；

如果您已经了解了语音知识，接下来您可以登录语音AI平台，查看 ☞[语音识别在端侧的实现原理](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E7%AF%87/%E8%BD%AF%E4%BB%B6%E7%AF%8700%EF%BC%9A%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%9C%A8%E7%AB%AF%E4%BE%A7%E7%9A%84%E5%AE%9E%E7%8E%B0%E5%8E%9F%E7%90%86/ "点击查看")，让您对启英泰伦的语音芯片有所了解；

如果您对这些都有所了解，请直接查阅 ☞[语音AI平台账号如何获取](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E6%A6%82%E8%BF%B0/ "点击查看")，在这个平台，您可以获得更多的资料以及进行产品的开发，推荐您申请注册平台账号并使用，目前该平台对用户免费开放。

---

### 2、获取开发板套件

对于新用户，推荐您直接选取我们的开发板套件进行开发，我们为您提供了配套的使用说明，软件SDK，固件及各类工具，您可以更容易上手进行开发。我司的开发板套件和模块相关联，如果您还没有模块相关联的开发板套件，或者还不知道选用哪种模块及开发板套件，请您阅览下列内容: ☞[硬件选型指南](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E7%A1%AC%E4%BB%B6%E9%80%89%E5%9E%8B%E6%8C%87%E5%8D%97/ "点击查看")，选好后可以按 ☞[样品和批量采购](https://document.chipintelli.com/%E6%A0%B7%E5%93%81%E5%92%8C%E6%89%B9%E9%87%8F%E9%87%87%E8%B4%AD/ "点击查看") 中的说明购买该模块及开发板套件。

如果您已拥有了开发板套件，但还不知道如何体验开发板套件，请查看下面的初体验识别交互章节的内容，来亲自体验语音的魅力。

---

### 3、初体验识别交互

准备好您的开发板，接上电源，听见欢迎播报的时候，您就可以开始您的语音交互之旅了。

如果您体验完后，意犹未尽想制作一个自己的demo固件，那么请查阅下述进阶篇的内容。

---

## 进阶篇

### 1、开发环境准备

若您已经拥有我司开发板套件请直接查看下面内容，若目前还没有开发板套件请按照上文中 ☞[获取开发板套件](#jump3) 内容获取开发板套件。

#### 1.1 搭建可编程开发环境

进入【☞[语音AI平台](https://aiplatform.chipintelli.com/attachment)】后，点击“开发资料”，“文件节点菜单”可以勾选“SDK”，然后根据你使用的我司芯片型号系列进行SDK的选择和下载，下载对应芯片型号的SDK可以通过“标签搜索”框来进一步筛选；解压并根据【☞[IDE 搭建与使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)】指引安装可编程开发环境。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-1.png)

#### 1.2 开发板套件确认

收到购买的开发板套件后，需要参照以下步骤对开发板套件进行组装和功能确认：

**CI1306开发板套件确认步骤：**

1. 将麦克风按图中所示插入插座；（注意麦克风是有极性区分的，红线对应板极MIC+，黑线对应板极的MIC-）

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-2.png)

2. 喇叭插入对应插槽；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-3.png)

3. 将数据线type-c端如图连接到模块，另一端连接到电脑的USB接口；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-4.png)

4. 拨动电源开关，接通电源指示灯点亮后，会听到喇叭播报“欢迎使用智能管家,您可以使用智能管家唤醒我。语音处于关闭状态,如需语音请说打开语音”；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-5.png)

5. 用户以正常语速呼叫“智能管家”，会听到喇叭播报“你好”，这样就可以确认模块能正常工作。

**CI1303、CI1302、CI1122、CI1103开发板套件确认步骤：**

1. 将麦克风按图中所示插入插座；（注意麦克风是有极性区分的，红线对应板极MIC+，黑线对应板极的MIC-）  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-6.png)
2. 喇叭插入对应排针；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-7.png)
3. 将数据线type-c端如图连接到模块，另一端连接到电脑的USB接口；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-8.png)
4. 按下电源开关，接通电源指示灯点亮后，会听到喇叭播报“欢迎使用智能管家,您可以使用智能管家唤醒我。语音处于关闭状态,如需语音请说打开语音”；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-9.png)
5. 用户以正常语速呼叫“智能管家”，会听到喇叭播报“你好”，这样就可以确认模块能正常工作。

***如您购买的是其它开发板套件，也可以按照上述步骤进行组装和功能确认。***

---

### 2、在线开发产品固件

用户的“开发板套件”和“开发环境”准备完毕并确认基础功能正常之后，本章将接着带您详细了解语音固件的在线开发、烧录及测试，现在正式开始感受语音开发给您带来的乐趣吧。

#### 2.1 语音识别演示固件开发

用户可以使用该功能在十分钟内自动生成语音固件，让产品落地更快速。

1. 进入语音AI平台“功能开发”页面；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-10.png)

2. 点击“功能开发”页面下的“离线语音识别大模型应用”板块；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-102.png)

3. 再点击“语音识别演示固件开发”板块，进入固件快速制作流程页面；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-11.png)

4. 用户需要点击“新建项目”进入项目产品固件的快速开发流程。示例如下图：

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-87.png)

5. 根据页面提示，用户按照要开发的具体产品分别填写“项目名称、产品类型、语言类型、芯片选择、优化选项、选择板极配置、灵敏度、音色”等信息后，点击“继续”（如对某一项填写内容有疑问可点击旁边圆圈中的问号“？”查看注释）。其中“优化选项”为语言模型自动优化，目的是为了提升产品识别体验效果，对命令词和唤醒词的置信度等参数进行调优（目前仅支持单个唤醒词调优，如果你提交了多个唤醒词，则系统仅对第一个唤醒词进行优化，其他的唤醒词系统将视为命令词进行优化处理），因自动优化相对耗时，若你提交时选择了自动优化功能，请耐心等待模型优化完成。示例如下图：  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-12.png)
6. 选择“协议类型”即为选择串口通讯协议，目前有默认的有“串口协议V1”和用户“自定义”十六进制协议两种可选，新用户体验推荐选择默认的“☞[串口协议V1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%BA%94%E7%94%A8%E5%B1%82%E5%8D%8F%E8%AE%AE/CI13XX%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AEV1/)”。示例如下图：  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-13.png)
7. 根据提示编辑命令词等信息，编辑命令词有两种方式。  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-13.png)
8. 方式一：平台会根据产品类型推荐命令词模板可供下载编辑，编辑完成之后点击网页上的“点击这里上传”即可在数据区域显示命令词等信息,具体操作步骤如下：

①点击“附件样例下载”，将下载一个Excel文件;

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-35.png)

②打开下载好的Excel表；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-36.png)

③根据表中“填写说明”填写“命令词预处理”内容；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-37.png)

④填写好的“命令词预处理”内容如下示例（以空调产品为例）；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-38.png)  
⑤再到网页中上传该表格文件；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-39.png)

⑥上传成功后会显示表格名字，并在下方显示表格内容的预览并且支持修改调整；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-40.png)

9. 方式二：网页“新增行”实时编辑添加命令词和唤醒词等信息；具体操作步骤如下：

①点击“新增行”；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-88.png)

②在下方编辑框新增的行中进行唤醒词、命令词和播报音的编辑填入，如下图：

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-89.png)

10. 当页面所有信息填写或上传完毕后，点击“立即提交”；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-90.png)

11. 稍等片刻后，当看到“当前流程”显示为“已完成”，即可点击“下载文件”；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-42.png)

12. 将下载下来的压缩包文件进行解压；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-43.png)

13. 解压得到的文件包含“XXX.bin”产品固件、串口协议列表和《固件烧录步骤》文档，如下图中内容，请打开《固件烧录步骤》文档；根据其步骤将固件烧录到开发板即可开始体验测试语音识别效果。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-91.png)

#### 2.2 语音识别固件及SDK开发

本版块旨在快速完成串口透传、自学习等主流产品方案的固件开发与SDK生成，并且主要参数可配，协议可自定义，亦可获取SDK进行二次开发。

1. 下面我们以制作一个风扇类的“定制协议的单麦离线固件”为例，进入语音AI平台“功能开发”页面。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-10.png)

2. 点击“离线语音识别大模型应用”板块；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-102.png)

3. 再点击【语音识别固件及SDK开发】板块，进入开发流程。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-93.png)

4. 点击【+新建项目】填写“产品信息”，当开发者填写并选择好要开发的产品名称、类型以及描述，【应用场景】的选择则需要选择“单麦离线串口透传方案\_SDK\_x.x.x”；然后点击“创建”。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-94.png)

5. 然后进行该项目的版本创建，首先创建第一版，【版本信息】推荐填入“V0.0.1”；【语言类型】这里我们选择“中文”；【声学模型】这里可以输入产品类型进行最新版本的选择“中文普通话风扇V2\_1.1M\_V00587”（若没有该产品类型的声学模型，则可以选择含有通用版本的声学模型）；【优化选项】为非必选项，初次制作固件体验效果可不选，若选择此功能后，系统则会自动对唤醒词和命令词进行优化，优化后的识别体验效果更佳，但优化耗时相对较长，根据系统当前繁忙程度，优化耗时在一般20-120min不等，请耐心等待优化完成；【模块板选择】需要选择用户手上用于开发的模块板型号；填写完毕后点击“继续”。

备注

* 【模块板选择】选好后所生成的固件及SDK能运行在“开发板套件”上的对应列表如下：
  ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-103.png)

![image-20230131142755294](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-95.png)

6. 来到“固件参数配置”页，需要结合项目实际情况对本页内容进行选填；其中包含有【识别参数】、【算法参数】、【硬件参数】、【功能参数】、【打印串口配置】、【通讯串口配置】、【功放配置】，该页面各个参数均有详细说明，填选完毕后，点击“继续”。

![image-20230131143421770](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-96.png)

7. 然后是“编辑命令词”页面，需要选择或填写其中部分信息，检查无误后即可点击”立即提交“：

* 【音色选择】为选择识别后反馈回来的播报音音色，这里推荐使用带Ver.2的音色；
* 【语速】和【音量】均可调节还可进行【播报音试听】；
* 【播报音压缩比】播报音压缩比分为：低、中、高三档，压缩比越高，生成的音频文件越小，虽然播报音效果就会有所下降，但可以节省芯片Flash空间；
* 【播报配置】也需要按照页面中的说明进行填写；
* 【上传附件】则为用户填写的命令词、播报语或协议等内容的Excel表格文件，需要下载附件样例后进行填写，填写无误后续上传至本页，页面表格则会实时展示并支持页面微调；
* 【SDK下载】为勾选项，勾选后会得到对应固件的SDK开发包；
* 【自学习功能】默认为关闭状态，可在☞[视频教程](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/)中进行观看了解；
* 【唤醒词切换】功能默认为关闭状态，可在☞[视频教程](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/)中进行观看了解。

![image-20230131154306238](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-97.png)

8. 提交后，版本管理页面的【当前流程】显示“已提交”。

![image-20230131155210519](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-98.png)

9. 稍加等待之后，自动刷新后便会看到【当前流程】由“已提交”变为“已完成”，即可点击“下载”按钮进行固件及SDK的下载。

![image-20230131155700342](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-99.png)

10. 下载的压缩文件需要解压。

![无标题](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-100.png)

11. 解压得到的文件包含有SDK软件开发包、产品固件、串口协议列表及固件烧录步骤手册；

![image-20230131161221473](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-101.png)

12. 用户可根据解压得到的《固件烧录步骤.pdf》文档进行本“产品固件”的烧录操作和体验测试；
13. 最后有嵌入式开发能力的用户还可以使用解压出来的“SDK开发包”结合“☞[启英泰伦文档中心-软件开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E6%A6%82%E8%BF%B0/)”板块内容，进行产品的编程开发。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-31.png)

#### 2.3 语音识别固件调优

[★仅限企业用户](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/)

“语音识别固件调优”功能可用于优化与定制产品原有语音固件的语音识别性能，支持用户依照实际应用需求，灵活调整相关设置。用户可通过精细调控阈值和计数参数，显著提升产品的识别准确率与响应速度，优化交互体验。

同时，用户能便捷地修改唤醒词、命令词以及播报音内容，这些调整让设备更贴合个人使用习惯与所处环境的需求。凭借“语音识别固件调优”功能，用户不仅能够实现个性化语音识别能力的进阶，还能全方位增强产品的整体使用体验，提高工作效率。借助这一功能，您的产品将在多样化场景中大放异彩，实现更为智能的人机交互！

“语音识别固件调优”功能使用方法如下：

1. 进入“功能开发”板块后，点击“离线语音识别大模型应用”；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-102.png)

2. 再点击“语音识别固件调优”功能；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%9B%BA%E4%BB%B6%E8%B0%83%E4%BC%98-1.png)

3. 点击“新建项目”；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%9B%BA%E4%BB%B6%E8%B0%83%E4%BC%98-2.png)

4. 填入项目名称，并选择需要操作的类型：支持修改固件中各个命令词的阈值与count计数值、修改唤醒词和命令词、播报音播报内容；选择需要获取的下载结果：有“固件.bin”和“组件包”可供选择下载；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%9B%BA%E4%BB%B6%E8%B0%83%E4%BC%98-3.png)

5. 上传需要修改微调的.bin固件，上传完毕后点击“加载命令词信息”按钮；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%9B%BA%E4%BB%B6%E8%B0%83%E4%BC%98-4.png)

注

1、不能在已调优的固件上继续迭代，如要继续迭代，若需再修改请上传原始固件；

2、只能上传固件文件\*.bin，且不超过8Mb；

6. 上传固件后，可进行修改的信息会出现在页面下方表格中，对其中内容进行修改后，点击“生成新固件”。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%9B%BA%E4%BB%B6%E8%B0%83%E4%BC%98-5.png)

注

1、net\_之后的数字含义：0表示命令词网络，非0表示唤醒词网络，它对应于[60000]\*.xlsx的sheet中尖括号里的数字；

2、修改唤醒词时，唤醒词和命令词网络需要同时修改；如果仅修改命令词，则仅在命令网络中修改即可；

3、修改命令词时，命令词仅能用一种文字，不能多种文字混用，如：中文只能用中文，英文只能用英文，不支持中文+英文、英文+数字等；

4、原命令词存在多种文字混用时，不支持修改；

5、修改的唤醒词和命令词数量之和小于10；播报音内容增加会导致固件大小增加，须确保固件大小不会超过所用芯片flash大小；

7. 等待“当前流程”显示“已完成”，即可点击“下载文件”进行下载修改调整后的“.bin”固件或相关“组件”；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%9B%BA%E4%BB%B6%E8%B0%83%E4%BC%98-6.png)

8. 下载修改调整后的“.bin”固件如下图，无需解压直接烧录到模块板即可；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%9B%BA%E4%BB%B6%E8%B0%83%E4%BC%98-7.png)

9. 若是下载的组件则为压缩包形式，解压即可得到fw组件包，其中包含“asr“、”dnn”、“user\_file”、“voice”四个组件包；使用SDK开发的用户只需将组件替换到原有SDK中对应位置即可。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%9B%BA%E4%BB%B6%E8%B0%83%E4%BC%98-8.png)

---

### 3、开发固件组件

备注

* 也可以通过视频教程链接进行“开发固件”的学习【[基于SDK开发固件](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E7%AF%87/%E8%BD%AF%E4%BB%B6%E7%AF%8712%EF%BC%9A%E5%9F%BA%E4%BA%8ESDK%E5%BC%80%E5%8F%91%E5%9B%BA%E4%BB%B6%EF%BC%88%E7%AC%AC%E4%B8%80%E8%AE%B2%EF%BC%9A%E5%9F%BA%E4%BA%8ESDK%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%EF%BC%89/)】

用户在完成了第一章“开发环境准备”和第二章“在线开发产品固件”之后，本章将接着带您进行固件组件的开发。我司芯片所对应的固件，主要由“语言模型、声学模型、播报音、用户代码”四大组件组成；接下来，我们来一步一步制作各个组件去合成我们想要的固件。

#### 3.1 语言模型与声学模型获取

制作固件的语言模型与声学模型是一并生成获取的，详细步骤如下：

1. 进入语音AI平台“功能开发”板块；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-10.png)

2. 点击“语言模型开发”进入语言模型的制作流程；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-32.png)

3. 点击“新建项目”按钮；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-92.png)

4. 根据页面提示信息填入相关数据后，点击“继续”按钮前往下一步；（注：“操作类型”还可以选择“G.fst.txt”分词文本进行上传，并支持”语音模型自动优化“，但[★仅限企业用户](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/))

* 声学模型如何选择请参考☞[声学模型说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/%E5%A3%B0%E5%AD%A6%E6%A8%A1%E5%9E%8B%E8%AF%B4%E6%98%8E/)

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-33.png)

5. 声学模型是固件的重要组件，根据页面提示选择声学模型后请点击“声学模型下载”，解压后放入PC本地的SDK软件开发包中的.\firmware\dnn\目录下：

   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-34.png)
6. 点击“附件样例下载”，将下载一个Excel文件；

   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-35.png)
7. 打开下载好的Excel表；

   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-36.png)
8. 根据表中“填写说明”填写“命令词预处理”内容；

   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-37old.png)
9. 填写好的“命令词预处理”内容如下示例（以空调产品为例）：

   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-38old.png)
10. 再到网页中上传该表格文件；

    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-39.png)
11. 上传成功后会显示表格名字，并在下方显示表格内容的预览并且支持修改调整；

    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-40lm.png)
12. 点击“提交”，平台会自动跳转到【任务管理】界面，实时查看本次语言模型制作的进度，图中“当前流程”显示“已提交”，需要用户等待片刻；

    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-41.png)
13. 稍等片刻后，当看到“当前流程”则显示为“已完成”，即可点击“下载文件”；

    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-42.png)
14. 将下载下来的压缩包文件进行解压；

    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-43.png)
15. 解压得到的文件包含语言模型和声学模型，如下图中内容，请打开“使用说明.txt”文档；根据“使用说明”的步骤将各个文件内容拷贝到SDK相应路径即可。

    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-44.png)

#### 3.2 播报音合成

合成播报音是为了让设备识别到命令词之后能拥有播报反馈响应，增强交互体验；现在我们来制作播报音，并且来让我们的播报音与命令词一一对应起来。

1. 进入语音AI平台“功能开发”页面；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-10.png)

2. 合成播报音点击“播报音合成”板块；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-45.png)

3. 首先点击“新建项目”按钮，选择播报音的发音语种，拥有中文、英文、日文三种语言可以选择；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-46.png)

4. 然后，选择对应语种下的音色，有多种女声、男声、女童、男童等音色可供试听选择；（试听功能待开放）

* 提示：建议选择Ver.3及更高版本的播放音音色，这些播放音的情感流畅度和语调都更加自然。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-47.png)

5. 再选择播报音的语速和播报音初始音量；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-48.png)

6. 再点击“附件样例下载”，下载一个名为“播报词列表\_中文模板.xlsx”的Excel格式附件；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-49.png)

7. 打开该附件样例，根据表中的“填写说明”，将播报音等文本信息填写到“语言合成”sheet中保存备用；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-50.png)

8. 回到网页，将相应的Excel附件点击上传；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-51.png)

9. 上传成功后，下方表格会将Excel中的内容显示出来，用户可在其中进行预览和修改调整，确认无误即可点击“提交”；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-52.png)

10. 提交成功后，页面会自动跳转到任务管理页，可以看到播报音合成的当前状态为“进行中”；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-53.png)

11. 播报音根据制作音频数量请等待几分钟，“当前状态”便会显示为完成，即可点击“下载文件”按钮保存该压缩文件；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-54.png)

12. 解压下载好的压缩文件，得到如下多个文件；

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-55.png)

13. 根据《使用说明.txt》描述步骤将解压出来的音频文件放入PC本地的SDK中的.\firmware\voice\src目录下备用。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-56.png)

#### 3.3 用户代码

用户要开发自定义功能的代码，需要先根据芯片型号安装与之匹配的开发环境。还未安装开发环境的用户可以进行如下操作：

1. 进入☞[语音AI平台](https://aiplatform.chipintelli.com/)，点击【☞[开发资料](https://aiplatform.chipintelli.com/attachment)】板块；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-57.png)
2. 选择点击“开发工具”；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-58.png)
3. 点击“标签搜索”选择具体芯片型号，点击“搜索”后，可以看到不同芯片所对应的IDE开发工具下载链接；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-59.png)
4. 用户找到与自己手上芯片型号所对应的开发工具及手册，点击“下载文件”（这里以下载CI13XX系列芯片所对应的开发工具为例）；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-60.png)
5. 解压下载好的压缩文件；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-61.png)
6. 打开解压出的IDE安装包，打开文档中心【[IDE 搭建与使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/IDE%E6%90%AD%E5%BB%BA%E4%B8%8E%E4%BD%BF%E7%94%A8/)】，根据其中指引进行IDE开发环境的安装。  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-62.png)
7. 安装完毕后，用IDE工具打开放入了语言模型、声学模型、播报音及[60000]{cmd\_info}.xlsx表的SDK包。用户可以根据产品需求参照文档中心《[软件开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E6%A6%82%E8%BF%B0/)》进行应用代码的开发；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-63.png)
8. 代码编写完成后，可以在3代芯片的IDE工具内点下图所示AI图标，左上角选项框中可以看到该工程名，先点击清理按钮，再点击生成按钮进行代码编译；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-64.png)
9. 代码编译完成后，鼠标右键点击工程名，鼠标左键点击“合成分区bin文件”；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-65.png)
10. 合成所有分区bin文件后，点击启动打包升级工具；  
    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-66.png)
11. 第一次打开“打包升级工具”需要选择与用户开发板对应的芯片型号；  
    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-67.png)
12. 然后点击“固件升级”按钮，跳转到固件升级工具界面会看到我们开发板的串口号、下载进度、下载速率及选中框等信息。另外请注意开发时使用的是USB转串口方式和电脑通信，请提前在电脑中安装好USB转串口芯片的驱动，以方便固件升级工具获取到串口号；  
    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-68.png)
13. 点击“选择固件”选择已经准备好的固件；  
    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-69.png)
14. 再在串口号后面勾选我们的开发板；  
    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-70.png)
15. 开发板重新上电或复位，软件会自动开始烧录选好的固件。  
    ① CI1306开发板套件烧录时可以使用下图中的复位按键：  
    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-71.png)  
    ② CI1303、CI1302、CI1122、CI1103开发板套件烧录时需按下图中的电源开关断电后，再按电源开关上电：  
    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-72.png)
16. 固件升级工具烧录时会实时显示下载进度：  
    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-73.png)
17. 待固件升级工具显示“device:update success....”，这表示固件下载更新成功；去掉勾选再重新上电即可开始体验测试。  
    ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-74.png)

*注：烧录串口默认为芯片的UART0，波特率为115200（CI110X系列芯片）或921600（CI13XX系列芯片）*

#### 3.4 语言模型优化

[★仅限企业用户](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/%E4%BC%81%E4%B8%9A%E8%AE%A4%E8%AF%81/)

在经过之前的固件开发流程之后，相信开发者用户们已经体验到了语言产品项目优秀的识别效果。当然，如果在体验测试中发现有需要进一步提升“识别率”或“抗集外误识 ”情况时，则可以使用我们语音AI平台中的“语言模型优化”功能来进一步提升识别效果；只需要提交唤醒词或命令词，平台自动完成优化。

##### 唤醒词优化步骤

1、首先，我们将需要优化的项目SDK工程包准备好（可通过语音AI平台的“开发资料”板块或“产品开发”功能获取）；

![语言模型优化-20221116162506172](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221116162506172.png)

2、打开“启英泰伦语音AI平台”，点击功能开发板块中的“语言模型优化”功能；

![语言模型优化-20221112163636928](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112163636928.png)

3、点击“新建项目”；

![语言模型优化-20221112165328186](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112165328186.png)

4、这里我们以一个空调项目举例，”项目名称“填入“智能空调“；

![语言模型优化-20221112171123389](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171123389.png)

5、”产品类型“选择”空调“，没有的产品类型可点击”新增类别“进行新增；

![语言模型优化-20221112171132018](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171132018.png)

6、芯片型号选择开发者手上真实芯片型号，这里我以CI1306型号为例进行选择；

![语言模型优化-20221112171140849](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171140849.png)

7、首先我们以优化该项目的唤醒词为例，”优化操作类型“选择”唤醒词优化“；

![语言模型优化-20221112171150544](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171150544.png)

8、”灵敏度“的选择，可根据问号(?)中的提示以产品的具体使用场景来进行选择，这里我们选择默认的“中”等灵敏度；

![语言模型优化-20221112171213447](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171213447.png)

9、填选好以后，我们点击“继续”；

![语言模型优化-20221112171414423](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171414423.png)

10、我们可以看到页面下方新出现了几个选填项；继续，对“选择声学模型”进行选择，根据用户产品类型选择带有产品类型名的声学模型，若没有找到带有产品类型名的声学模型，就可以选择有“通用”字样的最新版声学模型。

![语言模型优化-20221112171225470](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171225470.png)

11、然后，我们填入需要优化的唤醒词“智能管家”；

![语言模型优化-20221112171234301](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171234301.png)

12、最后检查完唤醒词的拼音行准确无误后，点击“提交”；

![语言模型优化-20221112171329864](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171329864.png)

13、提交后，只需要等待片刻即可生成优化好的唤醒词网络语言模型；

![语言模型优化-20221112171501342](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171501342.png)

14、“唤醒词”优化完成后，点击“下载文件”，开发者可将压缩文件保存到某个文件夹目录，然后进行解压备用；

![语言模型优化-20221112171751367](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171751367.png)

15、将解压好的文件夹打开，重命名语言模型“[0]asr\_chinese\_…\_optimize.dat”中括号中的索引号，改为“[1]”；

![语言模型优化-20221116152851308](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221116152851308.png)

16、复制重命名完毕的语言模型“[1]asr\_chinese\_…*optimize.dat”覆盖掉CI130X\_SDK......\projects\offline\_asr\_pro\_sample\firmware\asr目录下的”[1]asr.....*.dat”文件；

![语言模型优化-20221116153100186](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221116153100186.png)

17、打开“confidence.txt”文件，可以看到一个数字，该数字为该唤醒词的“置信度”；

![语言模型优化-20221116155034694](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221116155034694.png)

18、紧接着，打开SDK工程包CI130X\_SDK......\projects\offline\_asr\_pro\_sample\firmware\user\_file\cmd\_info中的[60000]{......}.xls表格文件，将“<1>Sheet1”中的唤醒词置信度改为“confidence.txt”文件中的“37”；

![语言模型优化-20221116155607551](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221116155607551.png)

19、最后，用户使用编译工具VS\_Code打开SDK工程包；根据[固件烧录](https://document.chipintelli.com/软件开发/SDK/CI13XX芯片SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/快速入门/SDK快速开发/SDK快速开发/#4)部分文档，进行最终固件烧录体验即可。

![语言模型优化-20221115175728944](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221115175728944.png)

##### 命令词优化步骤

1、首先，我们将需要优化的项目SDK工程准备好（可通过语音AI平台的“开发资料”板块或“产品开发”功能获取）；

![语言模型优化-20221116162506172](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221116162506172.png)

2、打开“启英泰伦语音AI平台”，点击功能开发板块中的“语言模型优化”功能；

![语言模型优化-20221112163636928](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112163636928.png)

3、点击“新建项目”；

![语言模型优化-20221112165328186](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112165328186.png)

4、这里我们以一个空调项目举例，”项目名称“填入“智能空调“；

![语言模型优化-20221112171123389](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171123389.png)

5、”产品类型“选择”空调“，没有的产品类型可点击”新增类别“进行新增；

![语言模型优化-20221112171132018](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171132018.png)

6、芯片型号选择开发者手上真实芯片型号，这里我以CI1306型号为例进行选择；

![语言模型优化-20221112171140849](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171140849.png)

7、我们以优化该项目的所有命令词为例，”优化操作类型“选择”命令词优化“；

![语言模型优化-20221114103131858](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221114103131858.png)

8、”灵敏度“的选择，可根据问号(?)中的提示以产品的具体使用场景来进行选择，这里我们选择默认的“中”等灵敏度；

![语言模型优化-20221112171213447](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171213447.png)

9、填选好以后，我们点击“继续”；

![语言模型优化-20221114104224304](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221114104224304.png)

10、我们可以看到页面下方新出现了几个选填项；继续，对“选择声学模型”进行选择，根据用户产品类型选择带有产品类型名的声学模型，若没有找到带有产品类型名的声学模型，就可以选择有“通用”字样的最新版声学模型。

![语言模型优化-20221112171225470](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221112171225470.png)

11、上传附件即是上传需要优化的命令词列表，首先我们点击“附件样例”进行命令词Excel列表中文模板的下载。

![语言模型优化-20221114111207790](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221114111207790.png)

12、打开下载好的Excel模板，将所有命令词及唤醒词填入表中并保存。

![语言模型优化-20221115142057322](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221115142057322.png)

13、页面点击“点击这里上传”来上传保存好的“命令词列表\_中文模板.xlsx”，页面下方表格会自动显示出全部命令词，并且支持修改编辑。

![语言模型优化-20221115145346968](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221115145346968.png)

14、所有填入内容确认无误后，点击“提交”，只需要等待片刻即可生成优化好的语言模型；

![语言模型优化-20221115145734493](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221115145734493.png)

15、提交后，只需要等待片刻即可生成优化好的命令词网络语言模型；

![语言模型优化-20221115145941791](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221115145941791.png)

16、“命令词”优化完成后，点击“下载文件”，开发者可将压缩文件保存到某个文件夹目录，然后进行解压备用；

![语言模型优化-20221115153239184](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221115153239184.png)

17、将解压好的文件夹打开，复制优化完毕的语言模型“[0]asr\_chinese\_…*optimize.dat”覆盖掉CI130X\_SDK......\projects\offline\_asr\_pro\_sample\firmware\asr目录下的”[0]asr.....*.dat”文件；

![语言模型优化-20221115172350728](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221115172350728.png)

18、复制“[60000]{cmd\_info}.xlsx”表格文件中“<0>cmd”组内容覆盖掉CI130X\_SDK......\projects\offline\_asr\_pro\_sample\firmware\user\_file\cmd\_info中的[60000]{......}.xls文件中的“<0>......”组内容；

![语言模型优化-20221116161827182](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221116161827182.png)

19、最后，用户使用编译工具VS\_Code打开SDK工程包；根据[固件烧录](https://document.chipintelli.com/软件开发/SDK/CI13XX芯片SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/快速入门/SDK快速开发/SDK快速开发/#4)部分文档，进行最终固件烧录体验即可。

![语言模型优化-20221115175728944](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E8%AF%AD%E8%A8%80%E6%A8%A1%E5%9E%8B%E4%BC%98%E5%8C%96-20221115175728944.png)

---

### 4、硬件开发

硬件开发主要分为两种开发方式，第一：在启英泰伦官方标准开发板或模块的基础上进行扩展开发；第二：在启英泰伦芯片的基础上根据官方推荐外围器件布局设计自有的产品模块。接下来就这两种方式展开来说明，用户可根据开发能力和开发资源综合比较后选择其一。

#### 4.1 硬件模块选型

官方标准模块是我司根据各产品领域的常用功能，选用合适的芯片和优质外围物料设计的模块，具有性价比高、可靠性好等特点。用户可在【[产品选型工具](https://www.chipintelli.com/zh-cn/page/128.html)】中选取适合自身需求的一款模块，进行具体的产品开发。  
![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-75.png)

#### 4.2 硬件芯片选型

如用户需要直接采用语音芯片进行硬件开发，则需要先根据推荐选择适合自身产品的芯片型号。用户也可在【[硬件选型指南](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E7%A1%AC%E4%BB%B6%E9%80%89%E5%9E%8B%E6%8C%87%E5%8D%97/)】文档的芯片选型表中进行选择。  
![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-76.png)

#### 4.3 硬件设计

需要自行设计PCB的用户，可根据我司提供的【[参考原理图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E7%A1%AC%E4%BB%B6%E8%AE%BE%E8%AE%A1%E5%8F%82%E8%80%83/)】进行设计。启英泰伦提供了多种方案参考原理图，可以满足大多数通常应用场景。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-77.png)

硬件开发人员可从【[语音AI平台](http://aiplatform.chipintelli.com/)】获取参考原理图和PCB文件。具体获取步骤如下：

1. 第一步：进入语音AI平台“开发资料”页面；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-57.png)
2. 第二步：“分类”选项选择“硬件设计资料”；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-79.png)
3. 第三步：“文件名称”输入具体芯片型号后点击“查询”，例如“CI1306”；  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-80.png)
4. 第四步：找到与芯片对应的硬件开发资料，点击蓝色的“下载文件”则会下载包含对应芯片的“参考原理图”。  
   ![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-81.png)

#### 4.4 硬件测试

我司芯片和模块相关的测试报告可到【[测试报告](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E7%A1%AC%E4%BB%B6%E6%B5%8B%E8%AF%95%E6%8A%A5%E5%91%8A/)】进行下载。  
![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-82.png)

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-83.png)

客户若自行设计模块，可参考上述测试项进行送检测试。

---

### 5、产品测试

基于启英泰伦芯片或模块开发的产品方案有一套完备的测试方法，分别为“语音方案识别效果测试的方法”、“搭建自动化语音识别测试的方法”、“产品批量生产时功能测试的方法”。

#### 5.1 识别效果测试

文档中心有介绍语音方案识别效果测试的方法文档，请到【[识别效果测试](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%86%E5%88%AB%E6%95%88%E6%9E%9C%E6%B5%8B%E8%AF%95%E6%A0%87%E5%87%86/)】查看。  
![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-84.png)

#### 5.2 自动化识别测试

文档中心有介绍搭建自动化语音识别测试的方法文档，请到【[自动化识别测试](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/%E8%87%AA%E5%8A%A8%E5%8C%96%E8%AF%86%E5%88%AB%E6%B5%8B%E8%AF%95/)】查看。  
![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-85.png)

#### 5.3 生产测试

文档中心有介绍我司产品批量生产时功能测试的方法文档，请到【[生产测试](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E7%94%9F%E4%BA%A7%E6%B5%8B%E8%AF%95/)】查看。

![](https://document.chipintelli.com/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0/%E8%AF%AD%E9%9F%B3AI%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/img/%E5%B9%B3%E5%8F%B0%E5%BC%80%E5%8F%91%E6%8C%87%E5%BC%95-86.png)

---

### 6、产品方案开发

文档中心有介绍我司已完成的一些产品方案应用案例，用户可以点击【[产品方案开发](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/)】查阅对应文档浏览相关信息，用于指导开发和产品生产。