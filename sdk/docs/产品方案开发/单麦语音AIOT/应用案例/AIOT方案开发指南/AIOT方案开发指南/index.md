<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/AIOT%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/AIOT%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/ -->

# CI230X系列芯片-AIOT方案开发指南

​ **CI230X系列芯片-AIOT方案开发指南主要包括：方案概述、方案涉及的专有名词介绍、方案开发介绍、方案软硬件开发流程介绍、并例举一个腾讯IOT灯控方案详细开发步骤。**

## **一、AIOT 方案概述：**

​ **语音识别是自然的交互方式，越来越多的被用户所接受，物联网wifi IOT 也在快速发展，不断的渗透日常的生活；针对于此，我司专门开发了CI230X系列芯片，通过单芯片用于离线语音识别+IOT控制，减少开发者的开发周期，为产品提升体验感，可以将我们的模块通过串口接入原有产品，快速增加离线语音和IOT控制的功能，该方案有以下特点：**

* **离线识别**：不依赖网络，10米远场识别，识别速度0.2~0.8s，识别率大于95%，BNPU3.0 抗噪性更好。
* **IOT 控制**：通过微信小程序或者APP远程监控及对设备进行控制。
* **隐私保护**：收集设备的使用习惯，同时不侵犯客户的隐私。
* **单芯片方案**：减少多方沟通对接，配合，联调，生产测试等大量的研发时间成本，快速为产品增加语音和IOT功能。
* **完善的解决方案**：使用百度、腾讯、阿里、涂鸦方案，无需用户开发wifi代码，直接使用启英泰伦开发好的WiFi固件即可接入IOT云，减少开发周期，加速产品项目落地。
* **场景联动**：接入到客户原有的智能家居系统，进行场景联动和控制。

## **二、 专业术语介绍**

​ **在基于CI230X系列芯片进行AIOT方案开发、测试、认证等过程中会有相关的专有名词术语需要用户了解，如：三元组、微信小程序、APP、语音固件、WIFI固件、入网license、MAC地址、AP配网、BLE配网、物模型JSON文件、SRC认证、CE认证、OTA，下面对这些专有名词进行详细说明：**

**1.1 三元组**

​ 是物联网平台颁发的设备身份识别ID。设备出厂的时候会将三元组信息烧录到设备中，在设备入网到IOT平台时首先将三元组信息发送到平台进行身份认证，认证通过后设备就可以和云端进行正常数据交互；腾讯IOT云平台发布的三元组信息由产品ID（ProductID），设备名称（DeviceName）和设备秘钥（DeviceSecret）组成。

**1.2 微信小程序 Wechat Mini Program**

​ 是一种不需要下载安装即可使用的应用程序，在微信内被便捷地获取和传播。

**1.3 APP Application**

​ 基于安卓或IOS系统的智能手机的第三方应用程序。

**1.4 语音固件**
烧写在芯片语音识别端的固件，一般负责设备的本地语音识别，硬件控制，语音播放等功能。

**1.5 WIFI固件**

​ 烧写在芯片WIFI端的固件，一般负责设备的联网、和IOT平台数据交互等功能。

**1.6 IOT入网License**

​ 进入IOT平台的许可证证书，主要功能是平台对设备身份的进行鉴权认证，防止非法设备接入平台

**1.7 MAC地址**

​ 芯片对外展示的地址，也称为局域网地址 ( LAN Address )、以太网地址 ( Ethernet Address ) 或物理地址 ( Physical Address), 用来确认网络设备位置。

**1.8 AP配网**

​ 设备热点配网，设备硬件WIFI处于AP模式（类似路由器，组成局域网）生成热点。手机先连接设备的热点，之后发送路由器名称和密码到设备，设备根据收到的名称和密码连接路由器，连接成功以后表示本次配网成功。

**1.9 BLE配网**

​ 蓝牙配网，设备运行低功耗蓝牙协议栈，作为蓝牙服务器端。手机通过搜索设备蓝牙广播，并建立连接，之后发送路由器名称和密码到设备，设备根据收到的名称和密码连接路由器，连接成功以后表示本次配网成功。

**1.10 物模型JSON文件**

​ 物模型是设备实体在云端的一个抽象。设备物模型通常通过属性、事件和动作这三种功能元素来定义，一般这些功能元素和对应的值定义为JSON(JavaScript对象标记)格式文件。通过物模型，可以将每一种品类的IOT设备在云端进行标准化和规模化，解决物联网设备碎片化的问题。

**1.11 SRRC认证**

​ 无线电型号核准认证。SRRC认证的目的是对无线电频谱参数进行技术管理，确保有限的频谱资源的正常使用，维持无线电信号的秩序和安全。

**1.12 CE认证 CONFORMITE EUROPEENNE**

​ CE认证，即只限于产品不危及人类、动物和货品的安全方面的基本安全要求。CE代表欧洲统一（CONFORMITE EUROPEENNE）。在欧盟市场“CE”标志属强制性认证标志，不论是欧盟内部企业生产的产品，还是其他国家生产的产品，要想在欧盟市场上自由流通，就必须加贴“CE”标志。

**1.13 OTA Over-The-Air Technology**

​ 通过外部的方式（有线 / 无线）对产品软件进行更新升级，满足用户对产品功能进行实时定义修改的需求。

## **三、AIOT-灯控方案介绍**

#### 3.1 基于CI230X系列芯片 IOT芯片可以开发 IOT灯控方案、IOT风扇方案、IOT红外空调等方案：下面介绍IOT灯控离线+在线控制方案，用户开发其他离线+IOT方案框架类似，可参考该灯控方案进行设计开发。

![IOT空调](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/AIOT%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/img/IOT%E7%81%AF%E6%8E%A7%E5%8A%9F%E8%83%BD%E6%A1%86%E5%9B%BE.png)

​

**方案支持如下功能：**

* 离线识别功能。
* 语音控制功能。
* IOT控制功能。
* 多设备场景联动功能。
* 设备在线OTA功能。
* 收集对设备使用习惯功能。
* 支持百度IOT、腾讯IOT、涂鸦IOT、阿里飞燕云功能

#### 3.2 下面介绍基于CI230X系列芯片开发的IOT灯控离线+在线控制方案：在原有电控的基础上，增加语音AIOT模块，仅需对接串口，即可为产品更增加语音、手机控制、远程监控、场景联动等功能。

![IOT灯控](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/AIOT%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/img/IOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88%E6%A1%86%E5%9B%BE.png)

**语音控制流程：**

1. 语音模块识别到结果通过串口发送到电控
2. 电控根据自身状态进行控制，同时将控制结果同步给模块
3. CI230X系列芯片模块根据反馈结果进行语音播放，并把结果发送到IOT平台，平台再将结果同步到手机APP/小程序

**手机控制流程：**

1. 在手机APP操作发送控制指令到CI230X系列芯片模块
2. CI230X系列芯片模块收到控制指令，将指令传给电控，电控结合自身状态进行控制，将控制结果反馈到模块
3. CI230X系列芯片模块根据反馈结果进行语音播放，并把结果发送到IOT平台，平台再将结果同步手机APP/小程序

## **四、AIOT-产品开发流程**

#### **产品开发流程中会涉及到硬件开发、软件开发、结构设计、生产测试等环节，如下图所示：**

![IOT空调](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/AIOT%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/img/IOT%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B.png)

#### 整套产品的固件有下面4个部分：

* **语音固件**

1. 使用启英的语音AI平台☞ [启英泰伦语音AI开发平台](https://aiplatform.chipintelli.com/begin)进行开发语音固件。
2. 将物联网JSON文件对应的功能，在代码中实现，将语音和手机的界面联动进行对应起来。
3. 对接电控协议。

* **WIFI固件**

​ 如果连接到腾讯连连/启英的微信小程序，启英提供标准固件，无需代码开发，☞ [腾讯IOT-CJSON透传WIFI固件](https://aiplatform.chipintelli.com/attachment)，连接其他IOT平台需要针对性开发代码，启英已提供开发实例。

![IOT空调](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/AIOT%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/img/ci230x-cjson%20WiFi%E6%A0%87%E5%87%86%E5%9B%BA%E4%BB%B6.png)

* **手机小程序**

​ 腾讯连连小程序，无需代码开发（创建界面方式连接），定制APP/微信小程序等需要开发代码进行联动，开发时建议产生于腾讯 物联网类似的json文件，方便语音固件对接。

* **电控固件**

​ 和语音固件联调对接协议，注意语音和电控的协议需要包含产品的状态，这些状态会同步小程序进行显示

### 4.1 下面详细介绍CI230X系列芯片 软件开发流程：包括语音SDK开发、WIFI SDK开发、小程序开发，固件烧录，如下图所示：

![IOT空调](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/AIOT%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/img/IOT%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B%E5%9B%BE.png)

* #### 软件开发环境搭建

​ CI230X系列芯片sdk开发环境搭建：包括WIFI开发环境搭建和语音开发环境搭建，详细步骤可以参考启英泰伦文档中心☞ [CI230X系列芯片开发环境搭建与应用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8/)

* #### CI230X系列芯片 语音 SDK和WIFI SDK 框架介绍

​ CI230X系列芯片SDK分为两部分，Part1：语音SDK， Part2：WIFI SDK，两部分SDK框架说明请分别参考启英泰伦文档中心☞ [SDK Quick Start](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/CI230X_SDK_Quick_Start/) 第3章节和第4章节。

* #### CI230X系列芯片 SDK组件介绍

​ CI230X系列芯片现有语音组件包括：命令词信息管理、音频播放器、用户数据管理、日志管理等模块，详细说明请分别参考启英泰伦文档中心☞ [语音部分SDK组件说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%A1%E6%81%AF%E8%A1%A8/)

* #### CI230X系列芯片驱动介绍

​ CI230X系列芯片驱动包括语音部分驱动和WIFI部分驱动，语音部分驱动主要包括GPIO、UART、PWM、IIS、IWDG、DOEDC等驱动模块，详细说明请分别参考启英泰伦文档中心☞ [语音部分外设驱动开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/driver/ci230x_driver/gpio/) ；WIFI部分驱动主要包括SPI ADV\_Timer WDT CLOCK RTC UART AES等驱动模块，详细说明请分别参考启英泰伦文档中心☞ [WIFI部分外设驱动开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E8%8A%AF%E7%89%87%E5%A4%96%E8%AE%BE%E9%A9%B1%E5%8A%A8/chip_driver/)

* #### CI230X系列芯片语音算法介绍

​ CI230X系列芯片算法包括语音识别算法、语音降噪算法、回升消除等算法，详细算法说明请分别参考启英泰伦文档中心☞ [算法使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/)

* #### CI230X系列芯片外部串口协议与内部串口协议介绍

​ 外部串口协议主要提供的是和电控进行通信的标准协议，用户也可以使用私有协议，请参考启英泰伦文档中心☞ [CI230X系列芯片外部串口协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/CI230X%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AEV2/)

内部串口协议主要是芯片内部语音Part和WIFI Part 通过串口通信，请参考启英泰伦文档中心☞ [CI230X系列芯片内部串口协议](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/CI230X-%E8%AF%AD%E9%9F%B3%E5%92%8CWiFi%E5%BA%95%E5%B1%82%E4%B8%B2%E5%8F%A3%E4%BC%A0%E8%BE%93%E5%8D%8F%E8%AE%AE/)

* #### CI230X系列芯片板级配置说明介绍

​ CI230X系列芯片在使用非标准开发板开发时，需要修改板级配置，适配用户自有的板载资源，具体操作说明请参考启英泰伦文档中心☞ [CI230X系列芯片板级配置说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E6%9D%BF%E7%BA%A7%E9%85%8D%E7%BD%AE/)

* #### CI230X系列芯片语音功能开发介绍

​ 用户在使用CI230X系列芯片开发语音功能时，请参考启英泰伦文档中心☞ [命令词和固件制作指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E5%91%BD%E4%BB%A4%E8%AF%8D%E5%92%8C%E5%9B%BA%E4%BB%B6%E5%88%B6%E4%BD%9C%E6%8C%87%E5%8D%97/)

* #### CI230X系列芯片 WIFI IOT云端接入介绍，目前WIFi SDK已经接入了百度IOT云、腾讯IOT云、腾讯TVS云小微、涂鸦IOT云，详细接入说明如下：

​ 腾讯IOT云接入：请参考启英泰伦文档中心☞ [腾讯IOT云接入](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/tencent_iot/)

​ 腾讯TVS云小微接入：请参考启英泰伦文档中心☞ [腾讯TVS云接入](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/tencent_tvs/)

​ 腾讯TVS云小微接入：请参考启英泰伦文档中心☞ [涂鸦IOT云接入](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-WIFI-SDK/tuya_iot/)

* #### 小程序开发介绍，小程序开发有三种情况

使用启英物联小程序，用户无需开发对小程序进行开发，只需要按照SDK现有和小程序通信协议开发具体功能。

使用腾讯连连小程序/涂鸦智能智能APP，用户可以在IOT云平台，对小程序功能和面板进行配置，然后发布产品即可使用。

用户在自己的小程序/APP上进行开发，配合CI230X系列芯片模组，定制私有通信协议进行开发，小程序开发和发布流程请参考启英泰伦文档中心☞ [小程序开发发布流程](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91/)

### 4.2 下面详细介绍CI230X系列芯片硬件开发流程，主要包括：需求评估、原理图设计、PCB设计、功能调测、射频匹配及调测、天线设计等方面, 如下图所示：

![IOT空调](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/AIOT%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/img/IOT%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B%E5%9B%BE.png)

* #### 需求评估

  1. 根据产品形态及应用场景，合理评估产品对IO口、UART、PWM、IIC、IIS、PDM等接口的需求数量，以判断芯片是否满足需求或调整需求。
  2. 根据产品形态及应用场景，合理评估单麦或双麦是否满足产品需求。
  3. 根据产品形态及应用场景，合理评估产品对音频功放规格的需求。可根据产品是否有功耗要求、是否有EMI要求、需要驱动的喇叭规格等方面来进行音频功放选型。
  4. 根据产品形态及应用场景，合理评估产品对AEC功能的需求。可根据是否需要播音打断判断是否需要AEC功能。同时值得注意的是，单麦+AEC、双麦+AEC、AB类功放、D类功放，不同的硬件配置，所对应的电路也不同。
  5. 根据产品形态及应用场景，合理评估产品电源供电方案。需要注意的是，CI2305、CI2306芯片需要5V和3.3V两路电源分别供电。可结合喇叭功率综合评估5V电源的电流需求，另一方面，3.3V为Wi-Fi供电，Wi-Fi在工作时，瞬时峰值电流较大，建议3.3V供电能力不小于1A。
* #### 原理图设计

  ​ 需求分析后，可根据产品形态，参照我司参考设计绘制原理图，需重点关注电源电路、晶体电路、射频电路、音频电路。详情可参考文档中心☞ [应用参考电路图](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E7%A1%AC%E4%BB%B6%E8%AE%BE%E8%AE%A1/)
* #### PCB设计

  ​ 针对Wi-Fi及音频的特殊性，PCB设计需要重点关注电源的稳定性、阻抗匹配、音频防干扰等方面，建议参照我司参考设计绘制PCB，详情可参考文档中心☞ [PCB Layout设计](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E7%A1%AC%E4%BB%B6%E8%AE%BE%E8%AE%A1/)
* #### 基本功能调测

  ​ 基本功能调测须先确认PCBA基本电气参数正常，然后再烧录语音固件及Wi-Fi固件，从而调试各IO口或外设功能，详情可参考文档中心☞ [CI-E05GT02S\_MB开发板套件说明](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI-E05GT02S_MB%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/)
* #### 射频匹配及调测

  ​ 芯片RFIO射频匹配的好坏，直接影响着传导和OTA指标的优劣以及Wi-Fi的稳定性，建议由专业射频工程师调测或在PCBA基本功能调测正常后寄回我司调测，详情可参考文档中心☞ [CI230X系列芯片射频测试指令及方法](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E6%97%A0%E7%BA%BF%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/CI230X%20%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95/CI230X%20%E5%B0%84%E9%A2%91%E6%B5%8B%E8%AF%95%E6%8C%87%E4%BB%A4%E5%8F%8A%E6%96%B9%E6%B3%95/)
* #### 天线设计

  建议专业天线工程师参与设计及调试或委托天线供应商设计

## **五、AIOT-灯控方案完整开发流程：**

​ **通过上述四个章节对CI230X系列芯片的资源、功能、开发流程等熟悉后，下面将介绍如何制作一个完整的IOT灯控Demo,用户可以参考该Demo开发流程，快速完成离线+IOT产品开发，开发流程如下图所示：**

![IOT空调](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/AIOT%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/img/iot-%E8%85%BE%E8%AE%AF%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B.png)

详细开发流程说明，可参考文档中心☞ [离线+腾讯IOT灯控方案开发](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/)