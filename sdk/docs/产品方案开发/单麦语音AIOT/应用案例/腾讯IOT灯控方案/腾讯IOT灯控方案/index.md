<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/ -->

# 腾讯IOT灯控方案开发实例

**新用户建议先查看 ☞[新手指南](https://document.chipintelli.com/%E6%96%B0%E6%89%8B%E6%8C%87%E5%8D%97/%E6%96%B0%E6%89%8B%E6%8C%87%E5%8D%97/)，了解开发的基础知识，再阅读该方案文档**

## **一、硬件环境**

​ 该方案使用启英泰伦标准Demo开发板CI-E05GT02S\_MB实现，开发板集成了CI2305智能语音IOT模组，开发板相关资料可以到启英文档中心查看☞ [CI-E05GT02S\_MB开发板套件说明](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/CI-E05GT02S_MB%E5%BC%80%E5%8F%91%E6%9D%BF%E5%A5%97%E4%BB%B6%E8%AF%B4%E6%98%8E/)，CI2305 智能语音芯片资料可以到启英文档中心查看☞ [CI230X系列芯片数据手册](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E8%8A%AF%E7%89%87%E6%A6%82%E8%BF%B0/)

## **二、IOT云端开发**

### **2.1 云端接入**

#### **2.1.1 注册登录腾讯云开放平台**

登录 ☞ [腾讯云开放平台](https://cloud.tencent.com/login?s_url=https://cloud.tencent.com/)，点击左下角 **立即注册**，注册一个腾讯云开放平台开发者账号。

![腾讯云开发者](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-1.png)

> **注意**：通过以上步骤注册的账号拥有最高权限。请妥善保存您的账号，避免您创建的硬件产品、App、设备运行或者用户数据受到安全风险。

#### **2.1.2 项目、产品、设备创建说明**

在腾讯云首页按照如下图所示步骤进入到 **物联网开发平台**。

![进入物联网开发平台](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-2.png)

#### **2.1.3 进入产品控制台**

![进入物联网开发平台](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-3.png)

#### **2.1.4 开发调试时使用公共实例**

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-4.png)

#### **2.1.5 新建项目或选择已有项目进行开发**

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-5.png)

#### **2.1.6 新建产品，可以选择标准品类，也可以选择自定义品类（自己定义产品功能）**

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E4%BA%A7%E5%93%81%E5%88%9B%E5%BB%BA.png)

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-6.png)

#### **2.1.7 新增自定义功能，生成物模型(物模型跟进新增功能生成)，可以点击查看物模型获取详细信息**

![1679367739537](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/%E7%89%A9%E6%A8%A1%E5%9E%8B.png)

![1679367739537](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E5%8A%9F%E8%83%BD%E5%AE%9A%E4%B9%89.png)

#### **2.1.8 设备开发类型选择，这里选择基于SDK开发**

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E8%AE%BE%E5%A4%87%E9%80%89%E6%8B%A9.png)

#### **2.1.9 小程序配置，重点注意配置面板，配置完以后发布产品，腾讯连连小程序会同步显示配置内容**

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E4%BA%A4%E4%BA%92%E5%BC%80%E5%8F%91%E9%85%8D%E7%BD%AE.png)

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E9%85%8D%E7%BD%AE%E4%BA%A7%E5%93%81%E5%B1%95%E7%A4%BA.png)

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E5%BF%AB%E6%8D%B7%E5%85%A5%E5%8F%A3%E9%85%8D%E7%BD%AE.png)

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E9%9D%A2%E6%9D%BF%E9%85%8D%E7%BD%AE.png)

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E9%85%8D%E7%BD%AE%E7%81%AF%E6%8E%A7%E9%A2%9C%E8%89%B2%E9%9D%A2%E6%9D%BF.png)

#### **2.1.10 配网方式选择：选择AP配网(用户可以根据需求选择BLE配网的方式)**

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E9%85%8D%E7%BD%91%E6%96%B9%E5%BC%8F%E9%80%89%E6%8B%A9.png)

#### **2.1.11 配置AP热点名称，用户配网时需要连接CI230X系列芯片的热点**

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E9%85%8D%E7%BD%AE%E8%AE%BE%E5%A4%87%E7%83%AD%E7%82%B9%E5%90%8D%E7%A7%B0.png)

#### **2.1.12 设备配网二维码，小程序和设备绑定需要扫描该二维码**

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E8%AE%BE%E5%A4%87%E9%85%8D%E7%BD%91%E4%BA%8C%E7%BB%B4%E7%A0%81.png)

#### **2.1.13 新建设备，设备名称用户自定义，不能重复**

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E6%96%B0%E5%BB%BA%E8%AE%BE%E5%A4%87.png)

#### **2.1.14点击设备名称，查看三元组信息，三元组信息需要通过语音开发工具烧录到设备中**

#### **2.1.15 设备测试通过以后，上传检测报告审核通过以后发布产品**

![三元组](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E8%AE%BE%E5%A4%87%E9%87%8F%E4%BA%A7.png)

#### **2.1.16 用户如果想要了解更多腾讯IOT开发资料，可以在[腾讯云文档中心](https://cloud.tencent.com/document/product/1081/34739)查看**

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/%E8%85%BE%E8%AE%AFiot%E6%96%B9%E6%A1%88%E8%AE%BE%E8%AE%A1%E6%8C%87%E5%8D%97-7.png)

## 三、语音和WIIF固件开发

### **2.1 使用启英提供的WIFI透传固件，下载启英WIFI标准透传固件进行烧录[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment)**

![1679295041189](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/%E4%B8%8B%E8%BD%BDwifi%E9%80%8F%E4%BC%A0%E5%9B%BA%E4%BB%B6Json%E7%89%88%E6%9C%AC.png)

### 2.2 语音固件开发

* **下载语音SDK[CI230X系列芯片语音SDK](https://aiplatform.chipintelli.com/attachment)**

![1679363906695](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot-sdk%E4%B8%8B%E8%BD%BD.png)

* **物模型接口定义：cias\_qcloud\_parse\_iot\_light.c**

```
void iot_light_init_data_template(void)
```

使用方式如下：

![1679369958904](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E8%AF%AD%E9%9F%B3%E5%9B%BA%E4%BB%B6%E7%89%A9%E6%A8%A1%E5%9E%8B%E6%8E%A5%E5%8F%A3%E5%AE%9A%E4%B9%89.png)

![1679367739537](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7%E5%8A%9F%E8%83%BD%E5%AE%9A%E4%B9%89.png)

* **云端指令处理和状态上报接口**

1. 对IOT云端下发的物模型指令进行解析接口：

```
int cias_qcloud_down_raw_json_iot_light(char *control_str)
```

2. 对指令解析后，进行电控和播报音处理接口：

```
int iot_light_deal_down_stream(void)
```

3. 将指令执行结果上报到IOT平台

```
void cias_sync_status_to_iot(void)
```

语音相关的基础知识请参考[新手指南](https://document.chipintelli.com/%E6%96%B0%E6%89%8B%E6%8C%87%E5%8D%97/%E6%96%B0%E6%89%8B%E6%8C%87%E5%8D%97/)

## **四、固件烧录**

### 4.1 语音固件烧录

* 启动打包升级工具，进入固件打包界面，选择芯片->导入固件分区文件->自动计算分区文件地址->打包固件，如果固件生成太大，请改变flash大小，如图所示：

![固件打包页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-12.png)

![固件打包页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-12-1.png)

* 点击固件升级，烧录合成固件(如果用我司开发板CI-E05G02S\_MB，需要将拨码开关拨到VOICE进行烧录，开发板板图参考图6-5)

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-13.png)

* 开发板示意图，烧录语音固件需要将拨码开关拨到voice一侧

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E5%BC%80%E5%8F%91%E6%9D%BF%E7%83%A7%E5%BD%95%E8%AF%AD%E9%9F%B3%E5%9B%BA%E4%BB%B6.png)

### 4.2 WIFi固件烧录

1. 进入wifi烧录界面，然后点击编辑（选择固件，设置烧录地址（默认从0x00开始），设置波特率（一般设置为2Mbits/s），如图所示：

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-13.png)

2. 拨码开关需要拨到wifi一侧， 让设备进入烧录模式（按住BOOT按键，并重启设备，重启以后可以松开boot按键）

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/CI-E05GT02S_MB_V3.0.png)

* 烧录完成

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E7%BC%96%E8%AF%91%E8%BD%AF%E4%BB%B6%E5%AE%89%E8%A3%85%E4%B8%8E%E4%BD%BF%E7%94%A8-wifi-14.png)

### 4.3 鉴权文件烧录

​ 启英泰伦-语音开发工具，目前集成了腾讯IOT鉴权文件烧录，腾讯TVS云鉴权文件烧录，涂鸦云鉴权文件烧录功能，用户根据自己使用的云平台进行鉴权文件的烧录，同时也可以通过SDK 参考现有的鉴权文件烧录代码，进行扩展实现自己平台的鉴权文件烧录功能；需要注意：**如果使用的是我司提供的开发板，需要将拨码开关拨到WIFI一侧才能进行鉴权文件烧录(请参考WIFI固件烧录说明)**

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E9%89%B4%E6%9D%83%E6%96%87%E4%BB%B6%E7%83%A7%E5%BD%95.png)

图 11-1

1. 需要打开串口，串口使用语音固件烧录串口，波特率使用115200，如上图标注1
2. 选取云平台类型，如上图标注2
3. 设置/获取鉴权文件，如上图标注3

腾讯IOT鉴权文件烧录：鉴权文件组成字段(产品ID,产品名称,鉴权秘钥)，烧录结果如下图：

![固件升级页面](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/%E8%85%BE%E8%AE%AFIOT%E9%89%B4%E6%9D%83%E6%96%87%E4%BB%B6%E7%83%A7%E5%BD%95.png)

## **五、腾讯连连小程序使用**

* ​ **小程序具有配网和与云端产品进行交互功能。用户不需要进行二次开发，只需要参考上面2.1.9中对小程序完成配置即可**
* **小程序可使用微信扫描下面的二维码进入**

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7demo%E9%85%8D%E7%BD%91%E4%BA%8C%E7%BB%B4%E7%A0%81.png)

* **进入微信小程序后的操作步骤如下：**

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7demo%E9%85%8D%E7%BD%912.png)![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7demo%E9%85%8D%E7%BD%913.png)![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3AIOT/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E8%85%BE%E8%AE%AFIOT%E7%81%AF%E6%8E%A7%E6%96%B9%E6%A1%88/img/iot%E7%81%AF%E6%8E%A7demo%E9%85%8D%E7%BD%914.png)

1. 进入到小程序配网界面，勾选“我已完成上述操作”，点击“下一步”
2. 输入待连接wifi的名称和密码，然后按照步骤连接设备热点“CI\_network\_config”，密码为12345678，连接成功后返回到小程序等待配网完成，（注意：该设备不支持5G频段wifi）
3. 为避免小程序和云平台配网消息迟滞问题，显示配网超时失败后也可返回至腾讯连连小程序先添加设备至“我的设备”，试试能否控制设备来判断是否已配网成功。设备重新上电后默认连接上次配网的wifi名称和密码，连接超时会自动进入配网模式以便再次通过小程序控制配网

配网和使用可参考视频：[离线语音&腾讯连连小程序方案](https://document.chipintelli.com/%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88/%E6%96%B9%E6%A1%8804%EF%BC%9A%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%26%E8%85%BE%E8%AE%AF%E8%BF%9E%E8%BF%9E%E5%B0%8F%E7%A8%8B%E5%BA%8F%E6%96%B9%E6%A1%88/)