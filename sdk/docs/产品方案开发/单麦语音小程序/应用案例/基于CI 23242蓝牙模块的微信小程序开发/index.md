<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E5%9F%BA%E4%BA%8ECI%2023242%E8%93%9D%E7%89%99%E6%A8%A1%E5%9D%97%E7%9A%84%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91/ -->

# 基于CI 23242蓝牙模块的微信小程序开发

## 1. 微信小程序平台操作

### 1.1. 小程序微信开发者工具使用说明

小程序工具官方说明：<https://developers.weixin.qq.com/miniprogram/dev/devtools/devtools.html>

启英BLE小程序\_Release代码请到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中下载

**1.1.1. 微信开发者工具下载安装**

微信开者工具下载链接：<https://developers.weixin.qq.com/miniprogram/dev/devtools/stable.html>

Windows7系统请选择1.05版本工具，其余可下载1.06版本的工具。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-1.png)

下载完安装包后，选择目标文件夹直接安装即可。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-2.png)

**1.1.2. 小程序项目新建和导入**

点击打开微信开发者工具，选择小程序项目，进行 **新建\导入** 小程序项目。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-3.png)

**1.1.2.1. 新建项目**

* 需要一个小程序的 AppID；如没有 AppID，可以选择申请使用测试号。
* 登录的微信号需要是该 AppID 的开发者；
* 需要选择一个空目录，或者选择的非空目录下存在app.json 或者 project.config.json。当选择空目录时，可以选择是否在该目录下生成一个简单的项目。
* 开发者工具提供非常丰富的模块，也可以选择一个模版来创建一个项目。
* 模板选择推荐：JS-基础模板。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-4.png)

**1.1.2.2. 导入小程序项目**

直接导入已有的小程序项目或者启英泰伦sample小程序，需输入AppID或者使用测试号。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-5.png)

**1.1.3. 微信开发者工具使用**

开发者工具主界面，从上到下，从左到右，分别为：菜单栏、工具栏、模拟器、目录树、编辑区、调试器 六大部分。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-6.png)

**1.1.3.1. 菜单栏**

* 界面 选项设置主界面想要展示的内容。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-7.png)

**1.1.3.2. 工具栏**

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-8.png)

* 模拟器、编辑器、调试器：打开\关闭该功能界面的显示。
* 可视化：打开模拟器和编译器功能，点击模拟器上的视图或组件，编译器上会显示对应的代码。
* 编译：进行代码的编译。
* 真机调试：小程序App在手机或电脑上进行运行调试。
* 清缓存：可选择性清除小程序的数据缓存、文件缓存、授权数据、登录状态等信息，可灵活操作便于调试。
* 上传：上传代码到[微信公众平台](https://mp.weixin.qq.com/)，平台上可进行审核发布等操作。
* 详情：查看小程序项目的基本信息、性能质量、本地设置、项目配置。

**1.1.3.3. 模拟器**

模拟器可以模拟小程序在微信客户端的表现，小程序的代码通过编译后可以在模拟器上直接运行。

**推荐机型尺寸宽度为375 px**， 这是因为在微信小程序中，rpx（responsive pixel）是一种根据屏幕宽度自适应的单位，规定屏幕基准宽度为750rpx。 例如，在iPhone X上， 屏幕宽度为375px，共有750个物理像素，1rpx约等于0.5px，这样设计时可以直接使用rpx单位，无需担心不同设备上的显示差异‌。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-9.png)

**1.1.3.4. 调试器**

* 调试器 **Console**，开发者可以在此输入和调试代码，小程序的错误输出，会显示在此处。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-10.png)

* 调试器 **AppData**，显示当前项目运行时小程序AppData的具体数据，实时反映项目数据情况，用户可以在此处编辑数据，并及时反馈到界面上，用于查看页面数据与预期的正确性。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-11.png)

* 调试器 **Storage**，用于显示当前项目的使用 wx.setStorage 或者 wx.setStorageSync 后的数据存储情况。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-12.png)

### **1.2. 注册小程序**

**1.2.1. 个人主体注册**

访问微信公众平台（<https://mp.weixin.qq.com/>），点击 “立即注册”，选择 “小程序” 类型。注册时需填写个人邮箱、设置密码，并进行个人信息登记，包括姓名、身份证号等，完成实名认证。注册成功后，可设置小程序名称、头像、简介等基础信息，但个人主体小程序在功能上存在一定限制，例如无法使用支付等部分商业功能。

**1.2.2. 企业主体注册**

同样在微信公众平台注册，企业注册需准备营业执照、企业对公账户等资料。注册流程中，除填写基本信息外，还需进行企业认证，通过微信认证需支付 300 元认证费用。认证后，企业小程序可解锁更多高级功能，如微信支付、微信登录等，更适合开展商业活动。

### **1.3. 认证与备案**

个人小程序无需备案，但企业小程序若涉及特定业务（如互联网金融、医疗等），需根据相关法规进行备案。认证方面，企业小程序通过微信认证后，可获得更丰富的接口权限和功能，提升小程序的可信度和服务能力。

### **1.4. 相关信息完善**

登录小程序管理后台，在 “设置” 中完善小程序的各类信息。包括基本信息（名称、头像、简介）、服务类目（根据小程序实际功能选择合适的类目，不同类目对应不同的审核标准和功能权限）、开发设置（配置服务器域名、消息推送等）。

### **1.5. 项目成员管理**

在管理后台的 “成员管理” 模块，管理员可添加项目成员，包括开发者、体验者等。开发者拥有代码上传、调试等权限，体验者可体验小程序的体验版。添加成员时，需输入对方的微信号，并设置相应权限。

### **1.6. 上传项目**

使用微信开发者工具，在项目开发完成后，点击 “上传” 按钮，将代码上传至小程序管理后台。上传时需填写版本号和版本描述，方便后续管理和审核。

### **1.7. 体验版设置**

上传代码后，在管理后台的 “开发管理 - 开发版本” 中，可将上传的版本设置为体验版。生成体验版二维码后，可分享给项目成员或测试人员进行体验和测试。体验版可设置权限，限制访问人员范围。

### **1.8. 提交项目审核与发布项目**

当小程序测试无误后，在管理后台点击 “提交审核”，微信官方将对小程序的功能、内容等进行审核。审核通过后，即可点击 “发布” 按钮，将小程序正式上线，供用户搜索和使用。若审核未通过，需根据审核意见修改代码后重新提交审核。

## **2. 小程序开发核心知识**

### **2.1. 代码文件目录结构**

小程序项目的目录结构主要包含以下几类文件和文件夹：

> **app.js**：小程序的入口文件，用于注册小程序，定义全局数据和生命周期函数。app.js 作为小程序的入口文件，默认生成的代码用于注册小程序，并定义全局数据和生命周期函数，是小程序运行的起点和全局逻辑的管理者。
>
> **app.json**：小程序的全局配置文件，用于配置小程序的页面路径、窗口样式、tabBar 等。
>
> **app.wxss**：小程序的全局样式表，可设置页面的公共样式。
>
> **pages 文件夹**：存放小程序的页面文件，每个页面由同名的.wxml（页面结构）、.wxss（页面样式）、.js（页面逻辑）、.json（页面配置）四个文件组成。
>
> **utils 文件夹**：用于存放公共的工具函数，方便在多个页面中调用。
>
> **components**：存放自定义组件文件。
>
> **control**：存放设备控制界面文件，例如风扇、取暖器和空调等设备控制相关文件就放在该文件夹里面。
>
> **images**：主包界面中的图片资源文件。·
>
> **pages**：小程序主包界面文件。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-13.png)

```
// .js文件初始内容含义：

Page({
  data: {
    message: 'Hello, World!'
  },

  // 在页面加载时触发，通常用于获取页面初始化数据，如从服务器请求数据；
  onLoad: function (options) {
  },

  // 函数在页面初次渲染完成后触发，此时页面的视图已经生成，
  // 适合进行一些依赖视图的操作，如操作画布、初始化地图等；
  onReady: function () {
  },

  // 函数在页面显示时触发，可用于更新页面状态；
  onShow: function () {
  },

  // 函数在页面隐藏时触发，可进行数据清理等操作；
  onHide: function () {
  },

  // 函数在页面卸载时触发，可用于释放资源、取消事件监听等；
  onUnload: function () {
  }
});
```

### **2.2. 小程序页面创建**

在 app.json 的 “pages” 字段中添加新页面的路径，如 “pages/newPage/newPage”，微信开发者工具会自动生成对应的页面文件。然后在页面文件中编写页面结构、样式和逻辑代码。

### **2.3. 页面分包处理**

小程序主包大小限制为 2M，若项目过大，可进行分包处理。在 app.json 中配置 “subPackages” 字段，定义分包的路径和页面。主包中包含启动页面和一些公共资源，分包中存放特定功能的页面。分包可以按需加载，提高小程序的启动速度和运行效率。

**“subPackages”是一个数组，数组里包含一个对象，这个对象描述了一个子包的相关属性。**

* “root”属性指定了该子包的根目录为”control/lamp”，表示该子包在项目中的文件路径位置。
* “name”属性为”lamp”，给这个子包起了个名字。
* “pages”属性是一个数组，里面包含了”rgb/rgb”，代表这个子包内包含的页面路径。
* “independent”属性值为”false”，表明该子包不独立。\*\*

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-14.png)

### **2.4. 页面文件结构**

每个页面由.wxml、.wxss、.js、.json 四个文件组成：

> **wxml**：类似于 HTML，用于描述页面的结构，通过组件和标签构建页面布局。
>
> **wxss**：类似于 CSS，用于设置页面的样式，可对组件进行样式定制。
>
> **js**：用于编写页面的逻辑代码，处理数据和事件。
>
> **json**：用于配置页面的局部属性，如导航栏标题、是否启用下拉刷新等。

### **2.5. 页面制作绘制与布局排版**

使用微信小程序提供的组件（如 view、text、image 等）在对应WXML文件中进行页面布局，通过 flex 布局、grid 布局等方式实现灵活的排版。结合 wxss 样式设置，可调整组件的大小、位置、颜色等样式属性。

页面UI最好在一张背景图片上设计，将图片放在最底层。避免出现未知错误。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-15.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-16.png)

### **2.6. 页面的渲染**

小程序通过数据绑定和事件驱动来实现页面渲染。在.js 文件中定义数据，通过 setData 方法将数据更新到页面，页面会自动重新渲染展示最新数据。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-17.png)

### **2.7. 页面逻辑结构与事件处理**

页面逻辑主要在.js 文件中编写，包括数据的获取、处理和展示。事件处理方面，小程序支持多种事件类型，如点击事件（bindtap）、滑动事件（bindtouchmove）、选择器事件（bindchange）等。为组件绑定事件后，编写对应的事件处理函数，在函数中实现相应的业务逻辑。

首先在页面文件.wxml上设计好对应的UI，然后在.js文件中绑定事件，编写对应的事件处理函数，在函数中实现相应的业务逻辑。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-18.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-19.png)

## **3. 小程序蓝牙功能实现**

蓝牙设备控制与协议解析概要：

总结：关于小程序控制蓝牙设备的开发细节，包括协议传输、界面控制、权限管理及数据处理等技术问题。

1. 服务通过特定协议进行通信，SDK中可以修改设备组编号和字编号以控制不同设备如空调、风扇等。
2. 小程序分包对应不同的界面选择，主设备编号和number ID需要根据需求进行调整。
3. 蓝牙3.5支持更大的数据包传输，用于区分不同设备的重要数据，并通过结构体保存设备信息。
4. 设备搜索和连接过程中涉及定位权限的检查，确保蓝牙和定位功能开启。
5. 搜索到的设备信息存储在全局变量中，避免重复显示并实时刷新搜索结果。
6. 蓝牙设备广播数据包含厂商特定信息，通过截取数据段获取设备版本和功能信息。
7. 连接蓝牙设备后获取服务和特征值，用于后续的数据通信和状态监听。
8. 数据加密和解密通过固定密钥完成，防止客户直接使用小程序代码。
9. 接收到蓝牙数据后解析协议，更新设备状态并渲染至控制界面。
10. 支持通过一条协议同步设备所有状态，提高数据传输效率。
11. 设备功能显示与隐藏可通过协议控制，实现界面定制化。
12. 控制指令通过蓝牙发送，数据格式需符合协议要求并进行加密处理。
13. CRC校验确保数据完整性，广播通信适用于不需连接的场景。
14. 界面设计依赖美工提供的图片和布局指导，适配不同屏幕尺寸。
15. 使用第三方组件库简化开发流程，引入图表库实现数据可视化。

蓝牙相关功能在app.js实现，包括获取蓝牙服务、蓝牙复连、蓝牙连接心跳、获取蓝牙特征值、蓝牙监听数据等操作。

### **3.1. 蓝牙初始化**

在页面加载时，onLoad函数会调用bluetooth\_init函数进行蓝牙初始化操作。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-20.png)

### **3.2. 检查蓝牙状态与搜索设备**

getBluetoothAdapterState函数用于检查本机蓝牙状态，并根据状态进行设备搜索操作。

如果蓝牙正在搜索设备（res.discovering为true），直接调用onBluetoothDeviceFound获取设备信息；若蓝牙可用但未在搜索，则调用startBluetoothDevicesDiscovery启动搜索。

startBluetoothDevicesDiscovery函数通过wx.startBluetoothDevicesDiscovery开始搜索蓝牙设备，设置allowDuplicatesKey: true允许重复获取设备，搜索成功后调用onBluetoothDeviceFound处理搜索到的设备。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-21.png)

### **3.3. 获取蓝牙设备信息​**

onBluetoothDeviceFound函数用于处理搜索到的蓝牙设备信息，对设备进行筛选、解析和页面渲染。

该函数遍历搜索到的设备，先进行设备名称和广播数据校验，然后解析设备广播数据中的版本、功能类型和设备标识等信息，将设备相关信息存储到页面数据中，并根据设备类型设置设备名称、图标等，最后通过setData更新页面，展示搜索到的蓝牙设备列表。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-22.png)

### **3.4. 连接蓝牙设备**

当用户点击设备列表中的设备时，get函数获取点击设备的信息，并调用bluetooth\_connection函数进行连接。

get函数首先判断当前是否允许连接（避免重复连接操作），对于不同类型设备进行不同处理。对于普通蓝牙设备，弹出确认连接提示框，用户确认后调用bluetooth\_connection通过wx.createBLEConnection连接指定设备。连接成功后，调用bluetooth\_services\_get获取设备服务；连接失败时，若在重试次数内则开启定时器进行重试，否则提示用户连接失败并进行相关清理操作，同时停止蓝牙设备搜索。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-23.png)

### **3.5. 获取蓝牙服务与特征值**

连接设备成功后，bluetooth\_services\_get函数用于获取蓝牙设备的服务。

该函数通过wx.getBLEDeviceServices获取设备服务，获取成功后将第一个服务的 UUID 存储到全局数据app.globalData.serviceId中，并调用bluetooth\_character\_get获取服务的特征值。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-24.png)

### **3.6. 蓝牙数据传输**

该部分是用来通过蓝牙传输数据。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-25.png)

### **3.7. 设备信息本地保存添加**

将搜索到的蓝牙设备信息或连接成功后的设备信息，通过小程序的本地存储接口（如 wx.setStorageSync 或 wx.setStorage）保存到本地，方便下次快速连接或查询。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-26.png)

### **3.8. 小程序与设备间蓝牙通信协议**

不同领域的语音设备其蓝牙通信协议不同，小程序需根据设备的协议进行开发。协议通常规定了数据的格式、指令的定义、交互的流程等。在开发过程中，需与设备厂商密切沟通，了解协议细节，确保小程序与语音设备之间能够准确、稳定地进行数据交互和功能控制。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-27.png)

### **3.9. CI23LC 系列 BLE 蓝牙开发说明**

CI23LC 系列 BLE 蓝牙设备在开发时，需根据官方文档了解设备的服务 UUID、特征值 UUID 以及数据交互协议。在小程序中，按照上述蓝牙功能实现步骤，结合设备特定的参数和协议进行开发，确保与 CI23LC 系列设备的正确通信和功能实现。

“设备的服务 UUID” 指的是设备在相关服务体系中，用于唯一标识特定服务的通用唯一识别码（UUID）。

## **4. 小程序扩展**

### **4.1. Vant 组件库**

vant 是一款轻量、可靠的小程序组件库，提供了丰富的组件，如按钮、表单、导航等。在小程序项目中引入 vant 组件库，可快速搭建美观、功能强大的界面，提高开发效率。引入方式可参考官方文档，通过 npm 安装或直接下载组件代码进行配置。详情见[Vant安装流程](https://blog.csdn.net/weixin_58667126/article/details/128706961?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522434ac6f4256f1f7ce297b17fa5ca933f%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=434ac6f4256f1f7ce297b17fa5ca9)。

### **4.2. E-charts 图表插件**

e - charts 是一款常用的图表插件，可在小程序中实现各种图表展示，如折线图、柱状图、饼图等。通过在小程序中集成 e - charts 插件，可将数据以直观的图表形式呈现，方便用户理解和分析数据。[E-Charts官网](https://echarts.apache.org/zh/index.html) [配置手册](https://echarts.apache.org/zh/option.html#title)

### **4.3. 小程序广播（blerf）**

blerf 是一种用于小程序蓝牙广播相关的技术或工具。在小程序开发中，若涉及蓝牙设备的广播发现和处理，可使用 blerf 相关功能实现对蓝牙广播数据的解析、过滤和处理，帮助小程序更高效地与蓝牙广播设备进行交互。

## **5. 小程序示例添加流程**

### 5.1. 小程序端

如果我们想要在取暖器功能区添加一个测试功能按钮，并实现小程序和设备背后的逻辑与数据传递，即小程序代码部分和SDK代码部分。如下图所示。我们需要按照以下步骤来操作。

![IMG_283](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-28.png)

5.1.1. 在Warmer.js文件中添加测试按钮的初始化数据（初始化为false，默认关闭测试按钮）和添加取暖器测试功能ID为0x21。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-30.png)

5.1.2. 先在想要添加的Warmer.wxml文件中添加对应UI元素，将该组件定义为icon\_image，将背后的点击逻辑事件（bind:tap）定义为CeShiChang。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-31.png)

5.1.3. 在对应的Warmer.wxss文件中定义该组件父容器（view）和子容器（此处为image图片）的具体样式，例如字体大小、高度、视觉效果等。

此处请注意：同一功能区的功能图标样式应该保持一致，避免图标的大小等不一致造成UI界面的混乱，图标应尽量整齐排列。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-32.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-33.png)

5.1.4. 在Warmer.js文件中定义背后的事件逻辑。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-34.png)

注意：小程序可以选择真机调试。步骤：真机调试->编译并自动调试。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-35.png)

在调试窗口可以观察小程序通过蓝牙发送数据的具体过程。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-36.png)

### 5.2. SDK设备端

**5.2.1. 在对应的SDK cias\_demo\_config.h中修改对应设备控制页面**

如果还没有对应设备要根据蓝牙设备-小程序交互协议修改两个地方，因为同一类型设备可能包括多个客户的不同设备，所以不仅要修改DEV\_DRIVE\_EN\_ID为对应的设备主设备ID，还要修改次设备ID，具体主设备ID和次设备ID见文档中心CI23LC系列小程序交互协议与功能说明。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-37.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-38.png)

**5.2.2. 在对应设备.h文件中添加功能ID宏定义**

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-39.png)

**5.2.3. 设置功能初始化值**

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-40.png)

**5.2.4. 在功能选择逻辑区添加测试功能的打开和关闭状态**

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-41.png)

**5.2.5. 宏定义测试功能的两种状态ID，即打开和关闭**

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-42.png)

**5.2.6. 添加判断测试功能的代码**

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-43.png)

**5.2.7. 添加发送暖风机设备测试功能的属性信息**

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-44.png)

**5.2.8. 关于SDK中测试功能相关代码添加完毕后进行代码的烧录**

步骤：清理->生成->合成分区bin文件->启动打包升级工具。
![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-45.png)

烧录工具打开后选择对应硬件参数，选择好后点击固件打包。之后步骤：自动分配->打包固件->固件升级。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-46.png)

固件升级步骤：选择固件->选择日志输出打勾，选择对应串口号->选择对应串口打勾。烧录进度100%即完成烧录。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-47.png)

注意

1.可以在烧录工具中看日志，观察发送或接收到的数据是否正确。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-48.png)

## **6. 微信小程序如何添加一个新设备**

微信小程序如何添加一个新设备

特别需要注意的是SDK和小程序端必须规定统一交互协议，否则小程序无法正确运行！！！

本文档仅适用于启英泰伦公版小程序代码。

### 6.1. 在app.json中添加分包，在对应主包下面添加分包

例如此处是水暖毯的一个次设备，在对应水暖毯主包下面的pages创建对应的次设备分包，保存（Ctrl+S）之后会自动生成分包页面。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-49.png)

### 6.2. 在app.js中添加对应处理该设备数据的方法

将设备 ID（deviceId）和原始数据（datas）传入进行处理。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-50.png)

添加对应处理该设备数据的函数。

注意

此处的数据函数接口需要和设别页面js文件中的接口函数相同

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-56.png)

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-62.png)

### 6.3. 在index.js文件中添加对应控制页面跳转逻辑

在主页面点击已连接设备图标后跳转到对应设备控制页面。

注意

设备号需要统一，避免点击设备跳转错误控制页面或者显示无此设备控制面板。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-51.png)

### 6.4. 在user.js文件中添加修改对于逻辑代码。

注意

小程序显示图标和默认名称是固定的，用户在user.js文件中只需要在jump\_devices函数中根据分配的设别号添加对应设备页面的url地址。

user.js文件介绍：主要用于用户修改小程序主页面的设备图标和名字显示（Info\_conver函数）以及关联设备页面url地址（jump\_devices函数)。

DeviceArray：用于存储设备名字，会显示在小程序主页面以及设备查找页面。

![image-20251231101750674](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-57.png)

DeviceImageArray：用于存储设备搜索页面和小程序主页面的设备图标的url地址。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-58.png)

function jump\_devices(deviceInfo)：用于返回设备文件地址。

![image-20251231101836221](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-59.png)

function Info\_conver(DeviceId, Type)：用于返回设备名称和设备图标地址字符串。

![image-20251231101923459](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-60.png)

示例：以添加一个风扇页面为准，在jump\_devices函数中添加对应代码。

![image-20251231103931219](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-61.png)

### 6.5. 在对应分包文件夹中添加设备对应代码。

注意：需要根据具体协议来进行代码开发。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-53.png)

### 6.6. 在SDK端我们需要修改对应的设备类型和次设备号

（1）修改主设备号。根据下面宏定义直接引用对应设备宏定义。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-54.png)

（2）根据具体子设备号修改子设备号，小程序端和SDK端代码必须保证设备信息一致才能成功进行蓝牙连接。

![](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91-55.png)

总结：经过以上操作我们才能实现小程序端和设备端的连接与交互，相关逻辑设计请自行设计开发或参考文档中心相关代码进行相关设备具体开发。

参考资料：

[小程序风扇设备示例说明 - 启英泰伦文档中心](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E9%A3%8E%E6%89%87%E8%AE%BE%E5%A4%87%E7%A4%BA%E4%BE%8B%E8%AF%B4%E6%98%8E/)

[基于CI23242蓝牙模块的微信小程序开发 - 启英泰伦文档中心](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%B0%8F%E7%A8%8B%E5%BA%8F/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E5%9F%BA%E4%BA%8ECI%2023242%E8%93%9D%E7%89%99%E6%A8%A1%E5%9D%97%E7%9A%84%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91/)

## **7. 注意事项**

**7.1. 自动保存：设置->编辑器设置->更多编辑器设置->搜索Files:Auto Save->选中afterDelay.**

**7.2. 更改Tab大小为四个空格：设置->编辑器设置**

**7.3. 开启代码自动热重载：详情->本地设置**

**7.4. wxml标签中回车没有自动缩进：在下方选择语言模式中先将”WXML”更换为”HTML”，再重新切换回”自动检测”（AIT+Z)。但是会出现关闭后重新打开还是没有自动缩进的清空，重启后需要再次作上述操作。**

**7.5. 在标签中间的数据为变量时键入{，不出现}，在Vue中也遇到过vscode的设置：**

**设置->编辑器设置->更多编辑器设置，搜索bracket**

**7.6. 只刷新模拟器的页面，不重新编译。点击模拟器上的刷新按钮。**

**7.7. 在使用API发送请求出现域名问题，可以先将微信开发者工具设置为不校验合法域名：详情->本地设置->勾选”不校验合法域名、web-view（业务域名）、TLS版本以及HTTPS证书”。**

**7.8. 优先级：style（内联样式）>class**

**7.9. 设置局部编译，避免编译全部。**