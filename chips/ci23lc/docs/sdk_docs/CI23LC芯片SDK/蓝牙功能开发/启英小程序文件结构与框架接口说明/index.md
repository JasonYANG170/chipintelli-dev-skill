<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E5%90%AF%E8%8B%B1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E6%96%87%E4%BB%B6%E7%BB%93%E6%9E%84%E4%B8%8E%E6%A1%86%E6%9E%B6%E6%8E%A5%E5%8F%A3%E8%AF%B4%E6%98%8E/ -->

# 启英小程序文件结构与框架接口说明

## 1. 启英小程序文件结构与列表

使用微信发开者工具导入启英的小程序sample代码，资源管理器中查看文件目录。

**启英BLE小程序\_Release** 代码请到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中搜索下载。

### 1.1 启英小程序文件结构

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E6%96%87%E6%A1%A3%E7%BB%93%E6%9E%84-1.png)

**components**：存放自定义组件文件。

**control**：存放设备控制界面文件。

**images**：主包界面中的图片资源文件。

**pages**：小程序主包界面文件。

**utils**：工具函数和辅助方法。

**app.js**：小程序核心逻辑文件，小程序实例的注册和全局功能的定义。

**app.json**：全局配置文件，可在此添加配置界面。

**app.wsss**：小程序的主样式表文件，可定义全局样式。

**其余文件**：.eslintrc.js、.gitignore、project.config.json、project.private.config.json、sitemap.json，涉及小程序协议，工具版本，工具规则等信息，**无需过多关注**。

### 1.2 启英小程序主要修改的文件列表

* #### 小程序全局文件

**app.js**：小程序的逻辑文件，提供全局函数和数据，方便其他页面调用，减少重复代码，提高开发效率。

**app.json**：小程序的全局配置文件，确保小程序的所有页面和组件能够正确加载和显示， 包括了小程序的所有页面路径、界面表现、顶部导航栏、底部 tab 等。

**app.wxss**：小程序的全局样式文件， 定义全局样式，确保所有页面在视觉上保持一致，提升用户体验。

* #### 小程序界面文件

启英小程序sample代码中，界面文件主要包含 **：首页界面**、**蓝牙搜索添加界面**、**风扇控制界面1**、**风扇控制界面2**，四个界面。

小程序每个界面通常包含四个文件：.js（页面逻辑）、.json（页面配置）、.wxml（页面结构）和.wxss（页面样式）。

| 界面 | 界面文件路径 |
| --- | --- |
| 首页界面 | pages\index\index |
| 蓝牙搜索添加界面 | pages\device\device |
| 风扇控制界面1 | control\Fan\electricFan\electricFan |
| 风扇控制界面2 | control\Fan\electricFan2\electricFan2 |

## 2. 微信小程序常用框架接口

微信小程序☞[框架接口说明](https://developers.weixin.qq.com/miniprogram/dev/reference/api/App.html)

### 2.1 小程序 App框架接口：

**App(Object object)：注册小程序，接受一个 `Object` 参数，其指定小程序的生命周期回调等。**

App() 必须在 `app.js` 中调用，必须调用且只能调用一次。不然会出现无法预期的后果。

Object object参数：

| 属性 | 类型 | 默认值 | 必填 | 说明 | 最低版本 |
| --- | --- | --- | --- | --- | --- |
| onLaunch | function |  | 否 | 生命周期回调——监听小程序初始化。 |  |
| onShow | function |  | 否 | 生命周期回调——监听小程序启动或切前台。 |  |
| onHide | function |  | 否 | 生命周期回调——监听小程序切后台。 |  |
| onError | function |  | 否 | 错误监听函数。 |  |
| onPageNotFound | function |  | 否 | 页面不存在监听函数。 | 1.9.90 |
| onUnhandledRejection | function |  | 否 | 未处理的 Promise 拒绝事件监听函数。 | 2.10.0 |
| onThemeChange | function |  | 否 | 监听系统主题变化 | 2.11.0 |
| 其他 | any |  | 否 | 开发者可以添加任意的函数或数据变量到 `Object` 参数中，用 `this` 可以访问 |  |

**示例代码**：

```
App({
  globalData: {
    // Global data
  },
  onLaunch (options) {
    // Do something initial when launch.
  },
  onShow (options) {
    // Do something when show.
  },
  onHide (options) {
    // Do something when hide.
  },
  onError (msg) {
    console.log(msg)
  },
})
```

在页面中可通过 `getApp()` 获取到小程序全局唯一的 `App` 实例。

### 2.2 页面 Page框架接口：

**Page(Object object)： 注册小程序中的一个页面， 接受一个 `Object` 类型参数，其指定页面的初始数据、生命周期回调、事件处理函数等。**

Object object参数：

| 属性 | 类型 | 默认值 | 必填 | 说明 |
| --- | --- | --- | --- | --- |
| data | Object |  |  | 页面的初始数据 |
| options | Object |  |  | 页面的组件选项，同 Component`构造器中的`options` ，需要基础库版本 2.10.1 |
| behaviors | String Array |  |  | 类似于mixins和traits的组件间代码复用机制，参见☞ [behaviors](https://developers.weixin.qq.com/miniprogram/dev/framework/custom-component/behaviors.html)，需要基础库版本 2.9.2 |
| onLoad | function |  |  | 生命周期回调—监听页面加载 |
| onShow | function |  |  | 生命周期回调—监听页面显示 |
| onReady | function |  |  | 生命周期回调—监听页面初次渲染完成 |
| onHide | function |  |  | 生命周期回调—监听页面隐藏 |
| onUnload | function |  |  | 生命周期回调—监听页面卸载 |
| onRouteDone | function |  |  | 生命周期回调—监听路由动画完成 |
| onPullDownRefresh | function |  |  | 监听用户下拉动作 |
| onReachBottom | function |  |  | 页面上拉触底事件的处理函数 |
| onShareAppMessage | function |  |  | 用户点击右上角转发 |
| onShareTimeline | function |  |  | 用户点击右上角转发到朋友圈 |
| onAddToFavorites | function |  |  | 用户点击右上角收藏 |
| onPageScroll | function |  |  | 页面滚动触发事件的处理函数 |
| onResize | function |  |  | 页面尺寸改变时触发，详见 ☞[响应显示区域变化](https://developers.weixin.qq.com/miniprogram/dev/framework/view/resizable.html#在手机上启用屏幕旋转支持) |
| onTabItemTap) | function |  |  | 当前是 tab 页时，点击 tab 时触发 |
| onSaveExitState | function |  |  | 页面销毁前保留状态回调 |
| 其他 | any |  |  | 开发者可以添加任意的函数或数据到 `Object` 参数中，在页面的函数中用 `this` 可以访问。**这部分属性会在页面实例创建时进行一次深拷贝**。 |

**示例代码**：

```
//index.js
Page({
  data: {
    text: "This is page data."
  },
  onLoad: function(options) {
    // Do some initialize when page load.
  },
  onShow: function() {
    // Do something when page show.
  },
  onReady: function() {
    // Do something when page ready.
  },
  onHide: function() {
    // Do something when page hide.
  },
  onUnload: function() {
    // Do something when page close.
  },
  onPullDownRefresh: function() {
    // Do something when pull down.
  },
  onReachBottom: function() {
    // Do something when page reach bottom.
  },
  onShareAppMessage: function () {
    // return custom share data when user share.
  },
  onPageScroll: function() {
    // Do something when page scroll
  },
  onResize: function() {
    // Do something when page resize
  },
  onTabItemTap(item) {
    console.log(item.index)
    console.log(item.pagePath)
    console.log(item.text)
  },
  // Event handler.
  viewTap: function() {
    this.setData({
      text: 'Set some data for updating view.'
    }, function() {
      // this is setData callback
    })
  },
  customData: {
    hi: 'MINA'
  }
})
```

### 2.3 基础功能

#### 2.3.1 console

向调试面板中打印日志。console 是一个全局对象，可以直接访问。在微信客户端中，向 vConsole 中输出日志。

| 方法 | 功能 |
| --- | --- |
| console.debug() | 向调试面板中打印 debug 日志 |
| console.log() | 向调试面板中打印 log 日志 |
| console.info() | 向调试面板中打印 info 日志 |
| console.warm() | 向调试面板中打印 warn 日志 |
| console.error() | 向调试面板中打印 error 日志 |
| console.group(string lable) | 在调试面板中创建一个新的分组。随后输出的内容都会被添加一个缩进，表示该内容属于当前分组。调用 console.groupEnd之后分组结束。 |
| console.groupEnd() | 结束由 console.group 创建的分组 |

**示例代码**：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E6%A1%86%E6%9E%B6%E6%8E%A5%E5%8F%A3-1.png)

#### 2.3.2 定时器

微信小程序中的定时器主要有两种： 一次性定时器setTimeout和周期性定时器setInterval，其接口列表如下。

| 定时器接口 | 功能 |
| --- | --- |
| number setTimeout(function callback, number delay, any rest) | 设定一个定时器。在定时到期以后执行注册的回调函数 |
| clearTimeout(number timeoutID) | 取消由 setTimeout 设置的定时器。 |
| number setInterval(function callback, number delay, any rest) | 设定一个定时器。按照指定的周期（以毫秒计）来执行注册的回调函数 |
| clearInterval(number intervalID) | 取消由 setInterval 设置的定时器。 |

**示例代码**：

```
onLoad(options) {
    let now_time = new Date()   //获取当前时间
    console.log('开始定时器时间：',now_time);

    let Timeout = setTimeout(() => {    //定时五秒后执行
      now_time = new Date()
      console.log('Timeout 时间：',now_time);
    }, 5000);

    let Interval = setInterval(() => {  //周期每秒执行
      now_time = new Date()
      console.log('Interval 时间：',now_time);
    }, 1000);

    setTimeout(() => {  //十秒后关闭定时器
      now_time = new Date()
      clearTimeout(Timeout)
      clearInterval(Interval)
      console.log('结束定时器打印，当前时间：',now_time)
    }, 10000);
  },
```

小程序运行结果如下。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E6%A1%86%E6%9E%B6%E6%8E%A5%E5%8F%A3-2.png)

## 3. 微信小程序 WXML 语法参考

WXML（WeiXin Markup Language）是框架设计的一套标签语言，结合☞[基础组件](https://developers.weixin.qq.com/miniprogram/dev/component/index.html)、☞[事件系统](https://developers.weixin.qq.com/miniprogram/dev/framework/view/wxml/event.html)，可以构建出页面的结构。

用以下一些简单的例子来看看 WXML 具有什么能力：

### 3.1 ☞[数据绑定](https://developers.weixin.qq.com/miniprogram/dev/reference/wxml/data.html)

```
<!--wxml-->
<view> {{message}} </view>
// page.js
Page({
  data: {
    message: 'Hello MINA!'
  }
})
```

### 3.2 ☞[列表渲染](https://developers.weixin.qq.com/miniprogram/dev/reference/wxml/list.html)

```
<!--wxml-->
<view wx:for="{{array}}"> {{item}} </view>
// page.js
Page({
  data: {
    array: [1, 2, 3, 4, 5]
  }
})
```

### 3.3 ☞[条件渲染](https://developers.weixin.qq.com/miniprogram/dev/reference/wxml/conditional.html)

```
<!--wxml-->
<view wx:if="{{view == 'WEBVIEW'}}"> WEBVIEW </view>
<view wx:elif="{{view == 'APP'}}"> APP </view>
<view wx:else="{{view == 'MINA'}}"> MINA </view>
// page.js
Page({
  data: {
    view: 'MINA'
  }
})
```

### 3.4 ☞[模板](https://developers.weixin.qq.com/miniprogram/dev/reference/wxml/template.html)

```
<!--wxml-->
<template name="staffName">
  <view>
    FirstName: {{firstName}}, LastName: {{lastName}}
  </view>
</template>

<template is="staffName" data="{{...staffA}}"></template>
<template is="staffName" data="{{...staffB}}"></template>
<template is="staffName" data="{{...staffC}}"></template>
// page.js
Page({
  data: {
    staffA: {firstName: 'Hulk', lastName: 'Hu'},
    staffB: {firstName: 'Shang', lastName: 'You'},
    staffC: {firstName: 'Gideon', lastName: 'Lin'}
  }
})
```

具体的能力以及使用方式在以下章节查看：

☞[数据绑定](https://developers.weixin.qq.com/miniprogram/dev/reference/wxml/data.html)、☞[列表渲染](https://developers.weixin.qq.com/miniprogram/dev/reference/wxml/list.html)、☞[条件渲染](https://developers.weixin.qq.com/miniprogram/dev/reference/wxml/conditional.html)、☞[模板](https://developers.weixin.qq.com/miniprogram/dev/reference/wxml/template.html)、☞[引用](https://developers.weixin.qq.com/miniprogram/dev/reference/wxml/import.html)

## 4. 微信小程序 WXS 语法参考

WXS（WeiXin Script）是小程序的一套脚本语言，结合 `WXML`，可以构建出页面的结构。

WXS 与 JavaScript 是不同的语言，有自己的语法，并不和 JavaScript 一致。

* ☞[WXS 模块](https://developers.weixin.qq.com/miniprogram/dev/reference/wxs/01wxs-module.html)
* ☞[变量](https://developers.weixin.qq.com/miniprogram/dev/reference/wxs/02variate.html)
* ☞[注释](https://developers.weixin.qq.com/miniprogram/dev/reference/wxs/03annotation.html)
* ☞[运算符](https://developers.weixin.qq.com/miniprogram/dev/reference/wxs/04operator.html)
* ☞[语句](https://developers.weixin.qq.com/miniprogram/dev/reference/wxs/05statement.html)
* ☞[数据类型](https://developers.weixin.qq.com/miniprogram/dev/reference/wxs/06datatype.html)
* ☞[基础类库](https://developers.weixin.qq.com/miniprogram/dev/reference/wxs/07basiclibrary.html)