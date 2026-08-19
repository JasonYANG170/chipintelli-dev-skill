<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E9%A3%8E%E6%89%87%E8%AE%BE%E5%A4%87%E7%A4%BA%E4%BE%8B%E8%AF%B4%E6%98%8E/ -->

# 小程序风扇设备示例说明

本文档通过风扇设备的示例，了解小程序页面的生成制作、小程序蓝牙数据收发的处理、启英泰伦CI23LC SDK蓝牙设备制作、CI23LC SDK设备蓝牙数据收发的处理，最终达到用户能使用启英CI23LC语音蓝牙芯片制作微信蓝牙小程序的目的。

## 1. 小程序风扇页面制作

**启英BLE小程序\_Release** 代码请到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中搜索下载

使用微信开发者工具，导入启英泰伦微信ble小程序sample代码

☞[微信开者工具下载地址](https://developers.weixin.qq.com/miniprogram/dev/devtools/stable.html)

### 1.1 页面配置

开发者通过在 app.json的 `Pages` 或`subPackages` 字段中进行页面配置，本次使用`subPackages`字段来配置页面。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E9%A3%8E%E6%89%87%E8%AE%BE%E5%A4%87%E7%A4%BA%E4%BE%8B-1.png)

分包成功后，可以看到在control根目录下生成了Fan分包文件，在Fan分包文件中配置了electricFan和electricFan2两个页面。

### 1.2 视图层页面制作

**首先打开模拟器、编辑器、可视化功能**，在资源管理器下点击要制作页面的 **.wxml页面结构** 文件，模拟器会实时显示该文件的页面， **小程序页面是通过组件和容器搭配样式来制作的**‌。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E9%A3%8E%E6%89%87%E8%AE%BE%E5%A4%87%E7%A4%BA%E4%BE%8B-2.png)

* #### 组件

微信官方☞[组件说明文档](https://developers.weixin.qq.com/miniprogram/dev/platform-capabilities/miniapp/component/component.html)

**视图容器：** 视图容器是用于容纳其他视图组件的容器，通常作为布局的基础。

**组件：** 组件是用户可以直接与之交互的UI元素，如按钮、文本框等。

**用户交互方式‌：** 组件通过事件监听等方式与用户交互，而容器则主要负责布局和空间管理。

**常用的视图容器和组件列表如下：**

| 视图容器 | 容器功能说明 | 组件 | 组件功能说明 |
| --- | --- | --- | --- |
| view | 视图容器 | button | 按钮 |
| cover-view | 覆盖在原生组件之上的文本视图 | checkbox | 多选项目 |
| scroll-view | 可滚动视图区域 | input | 输入框 |
| swiper | 滑块视图容器 | picker | 从底部弹起的滚动选择器 |
| swiper-item | 仅可放置在swiper组件中，宽高自动设置为100% | slider | 滑动选择器 |
|  |  | image | 图片 |

用以下图示例进行说明，最外围放置了view的视图容器，其中有嵌套view后再放置了image、text、picker组件。视图容器大小与视图里组件的 **排列布局方式**，可通过 **class属性** 和 **style属性** 去实现具体 **样式**。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E9%A3%8E%E6%89%87%E8%AE%BE%E5%A4%87%E7%A4%BA%E4%BE%8B-3.png)

* #### 组件样式

微信官网☞[组件样式说明文档](https://developers.weixin.qq.com/miniprogram/dev/platform-capabilities/extended/component-plus/wxml-to-canvas.html#%E6%A0%B7%E5%BC%8F)

组件中设置 **class属性** 和 **style属性** 去实现具体的样式。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E9%A3%8E%E6%89%87%E8%AE%BE%E5%A4%87%E7%A4%BA%E4%BE%8B-4.png)

在页面对应的.wxss页面样式文件中，可以增删改查当前页面的所有局部样式。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E9%A3%8E%E6%89%87%E8%AE%BE%E5%A4%87%E7%A4%BA%E4%BE%8B-5.png)

### 1.3 视图层与逻辑层的交互

* #### 逻辑层.js文件说明

微信官方☞[Page逻辑层页面说明文档](https://developers.weixin.qq.com/miniprogram/dev/reference/api/Page.html)

页面逻辑层的.js文件使用Page(Object object)来注册页面， 接受一个 `Object` 类型参数，其指定页面的初始数据、生命周期回调、事件处理函数等，代码如下。

```
Page({

  /**
   * 页面的初始数据
   */
  data: {

  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {

  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady() {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow() {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide() {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload() {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh() {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom() {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage() {

  }
})
```

参数 **data** 是页面第一次渲染使用的初始数据，页面加载时，data 将会以JSON字符串的形式由逻辑层传至渲染层，因此data中的数据必须是可以转成JSON的类型：字符串，数字，布尔值，对象，数组。

渲染层可以通过 WXML 对数据进行绑定。

```
<view>{{text}}</view>
<view>{{array[0].msg}}</view>
```

```
Page({
  data: {
    text: 'init data',
    array: [{msg: '1'}, {msg: '2'}]
  }
})
```

* #### 组件事件及处理函数

微信官方☞[事件说明文档](https://developers.weixin.qq.com/miniprogram/dev/framework/view/wxml/event.html)

* 事件是视图层到逻辑层的通讯方式。
* 事件可以将用户的行为反馈到逻辑层进行处理。
* 事件可以绑定在组件上，当达到触发事件，就会执行逻辑层中对应的事件处理函数。

在页面结构.wxml文件中，可以给组件绑定事件及其事件处理函数，示例代码如下：

image组件bind绑定了 **tab** 点击事件及事件处理函数是 **wind\_sub**。

```
<image bind:tap="wind_sub"  class="box1_view2-img"/>
```

在页面逻辑.js文件中，当达到触发事件，就会执行逻辑层中对应的事件处理函数，示例代码如下：

```
 /* 风速控制 */
  wind_sub(){
    let tmp = this.data.fanInfo.wind_slider_value
    if(tmp <= this.data.fanInfo.wind_min){
      wx.showToast({
        title: '已达最低档位',
        duration: 1000,
        icon: 'none'
      })
      return
    }else{
      tmp -= 1
    }
    this.setData({      //setData，将 data 从逻辑层传输到视图层
      'fanInfo.wind_slider_value' : tmp,
    })
    app.ble_send_data(风扇,设备编号,属性设置,数据类型,风速,[tmp])   //发送蓝牙协议
  },
```

* #### 逻辑层数据页面渲染

微信官方☞[逻辑层数据渲染setData方法说明](https://developers.weixin.qq.com/miniprogram/dev/framework/performance/tips/runtime_setData.html)

`setData`函数用于将数据从逻辑层发送到视图层（异步），同时改变对应的 `this.data` 的值（同步），直 **接修改 this.data 而不调用 this.setData 是无法改变页面的状态的，还会造成数据不一致。**

`setData` 的过程，大致可以分成几个阶段：

* 逻辑层虚拟 DOM 树的遍历和更新，触发组件生命周期和 observer 等；
* 将 data 从逻辑层传输到视图层；
* 视图层虚拟 DOM 树的更新、真实 DOM 元素的更新并触发页面渲染更新。

示例代码如下：

```
//页面js代码
const app = getApp()    //获取全局方法
//启英协议部分，根据自身协议进行自定义
const 风扇 = 0x06
const 设备编号 = 0x07
const 属性设置 = 0x01
const 状态查询 = 0x03
const 数据类型 = 0x01

const 开关 = 0x01 //“开关”功能ID

const 关闭 = 0x01
const 开启 = 0x02

//Page注册页面
Page({
  data: {
    fanInfo:{     //风扇功能信息数据
      fanPower:false,       //风扇开关，true：开启  false：关闭
      wind_slider_value:1,  //风速一档
      fanTimer:false,       //定时开关  true：开启  false：关闭
      timer_slider_value:0, //定时0小时
      fanUpAndDown:false,   //上下摇头  true：开启  false：关闭
      fanLiftAndRight:true, //上下摇头  true：开启  false：关闭
    },
  },
  //开关image组件，点击事件处理函数
  fanPowerChange(){
    this.setData({      //设置数据，并将数据输到视图层
      ['fanInfo.fanPower']:!this.data.fanInfo.fanPower,//开关设置
      ['fanInfo.wind_slider_value']:1,      //风速一档
      ['fanInfo.fanTimer']:false,           //关闭定时
      ['fanInfo.timer_slider_value']:0,     //定时值0
      ['fanInfo.fanUpAndDown']:false,       //关闭上下摇头
      ['fanInfo.fanLiftAndRight']:false,    //关闭左右摇头
    })
    app.ble_send_data(风扇,设备编号,属性设置,数据类型,开关,[tmp?开启:关闭])     //发送蓝牙设备控制数据
  },
})
```

```
<!-- 页面wxml代码 -->
    <view class="power_view">
      <image bindtap="fanPowerChange" class="power_icon" src="../images/electric_fan2/{{fanInfo.fanPower == false ? 'powerOff':'powerOn'}}.png" mode="aspectFit"/>
    </view>

      <text style="color: {{fanInfo.fanPower?'#03a7fe':''}};" class="box1_view2-txt">{{fanInfo.fanPower?fanInfo.wind_slider_value:0}}<text class="box1_view2-txt2">档风</text></text>

    <view class="control6" bindtap="fanUpAndDownChange">
      <image class="icon_image" src="{{fanInfo.fanPower&&fanInfo.fanUpAndDown == true ? '../images/electric_fan2/sapu_fanUpAndDownOn.png' : '../images/electric_fan2/sapu_fanUpAndDownOff.png'}}"></image>
      <text class="control6Txt">上下摇头</text>
    </view>

    <view class="control6" bindtap="fanLiftAndRightChange">
      <image class="icon_image" src="{{fanInfo.fanPower&&fanInfo.fanLiftAndRight == true ? '../images/electric_fan2/sapu_fanLiftAndRightOn.png' : '../images/electric_fan2/sapu_fanLiftAndRightOff.png'}}"></image>
      <text class="control6Txt">左右摇头</text>
    </view>
```

## 2. 小程序设备蓝牙连接的数据收发代码说明

使用微信蓝牙相关的API，结合启英泰伦蓝牙小程序sample代码进行说明。

微信☞[蓝牙低功耗介绍](https://developers.weixin.qq.com/miniprogram/dev/framework/device/ble.html)

微信小程序☞[蓝牙通用API](https://developers.weixin.qq.com/miniprogram/dev/api/device/bluetooth/wx.stopBluetoothDevicesDiscovery.html)

### 2.1 建立蓝牙连接数据通信

建立蓝牙通信主要流程：初始化蓝牙模块 —> 搜索蓝牙 —> 监听搜索到新设备 —> 连接蓝牙 —> 获取蓝牙服务 —> 获取蓝牙服务特征 —> 启动蓝牙特征notify监听 —> 监听蓝牙特征值变化事件 。

微信官方☞[蓝牙使用流程说明](https://developers.weixin.qq.com/miniprogram/dev/framework/device/ble.html#_4-%E4%B8%AD%E5%BF%83%E8%AE%BE%E5%A4%87%E7%9A%84%E4%BD%BF%E7%94%A8%E6%B5%81%E7%A8%8B)

启英小程序sample代码中，建立蓝牙连接数据通信代码入口如下。

```
// 在device.js文件中

/* 蓝牙初始化，开始蓝牙设备搜索 */
bluetooth_init()

/* 监听新蓝牙设备，并进行界面展示 */
onBluetoothDeviceFound()

/* 点击界面蓝牙设备，建立蓝牙连接、获取服务监听等 */
get(e)
```

```
// 在index.js文件中

/* 首页中点击已添加的蓝牙设备，会初始化蓝牙模块、建立连接、获取服务监听等 */
device_click(e)
```

**在给新设备建立蓝牙连接数据通信的过程中，小程序会把蓝牙设备的关键属性信息保存记录下来，存入本地缓存并记入app.js的globalData全局变量中，方便后续调用微信API接口等功能使用。**

```
// 在device.js文件中，添加设备时，会把蓝牙设备关键信息记录在app.globalData.ind_device_arr结构体数组中

if (storage_id_flag){
          num = app.globalData.ind_device_arr.length      //往ind_device_arr数组里新设备数据的索引
          app.globalData.ind_device_arr.push(JSON.parse(JSON.stringify(device_info)))
        }
        app.globalData.ind_device_arr[num].id = that.data.blue_id[that.data.blue_id_num]//蓝牙设备ID
        app.globalData.ind_device_arr[num].blue_devices = that.data.blue_devices[that.data.blue_id_num]//蓝牙设备编号（启英设备广播中补充的信息）
        app.globalData.ind_device_arr[num].config_type = that.data.config_type[that.data.blue_id_num]//蓝牙设备功能（启英设备广播中补充的信息）
        app.globalData.ind_device_arr[num].blue_version = that.data.blue_version[that.data.blue_id_num]//蓝牙设备版本（启英设备广播中补充的信息）
        app.globalData.ind_device_arr[num].sn = that.data.sn[that.data.blue_id_num]//未使用(忽略)
        app.globalData.ind_device_arr[num].serviceId = app.globalData.serviceId//蓝牙设备服务的UUID
        app.globalData.ind_device_arr[num].characteristicsId_write = app.globalData.notifyCharacteristicsId_write//蓝牙设备写特征的UUID
        app.globalData.ind_device_arr[num].characteristicsId_read = app.globalData.notifyCharacteristicsId_read//蓝牙设备读特征的UUID
        app.globalData.ind_device_arr[num].CharacteristicsId_notify = app.globalData.notifyCharacteristicsId_notify//蓝牙设备监听特征的UUID
        app.globalData.ind_device_arr[num].online = true      //蓝牙连接状态，不一定为true
        app.globalData.ind_device_arr[num].name = that.data.dev_name_value//设备名字
        app.globalData.ind_device_arr[num].image = '/images/device_icom/device.png'//设备图片
```

### 2.2 小程序蓝牙数据发送

向蓝牙低功耗设备特征值中写入二进制数据，需调用 **wx.writeBLECharacteristicValue** 接口，在蓝牙MTU未知的情况下，建议使用 20 字节为单位进行数据传输。

启英小程序sample代码中，已封装好蓝牙数据发送函数 **ble\_send\_data**，当一包数据长度大于20字节时会分包发送，**用户可根据项目协议，自定义该函数。**

```
// 在app.js文件中

/* 启英小程序发送设备控制蓝牙数据 */
ble_send_data(device_type,device_number,fuc_type,data_type,fuc_id,datas)
```

**注意：调用wx.writeBLECharacteristicValue接口，必须设备的特征支持 write属性 。**

#### 示例：风扇设备发送蓝牙数据

当用户点击’风扇开关’组件时，触发对应的事件处理函数fanPowerChange，在函数中调用小程序全局方法app.ble\_send\_data发送蓝牙数据。

```
// 在electricFan2.js文件中

const app = getApp()            //获取小程序app全局对象的数据或方法
//'风扇开关'组件的点击事件处理函数
fanPowerChange(){
    let tmp = this.data.fanInfo.fanPower    //获取当前风扇开关状态
    if(!tmp){
      tmp = true
    }
    else{
      tmp = false
    }

    if(this.data.fanInfo.Active_Show_EN)    //是否渲染判断
    this.setData({      //进行界面渲染
      ['fanInfo.fanPower']:tmp,
      ['fanInfo.wind_slider_value']:1,
      ['fanInfo.fanTimer']:false,
      ['fanInfo.timer_slider_value']:0,
      ['fanInfo.fanUpAndDown']:false,
      ['fanInfo.fanLiftAndRight']:false,
      ['fanInfo.fanModeIndex']:0,
      ['fanInfo.fanHumidity']:false,
      ['fanInfo.fanCold']:false,
      ['fanInfo.fanAnion']:false,
      ['fanInfo.fanTmp']:false,
      ['fanInfo.fanScreen']:false,
      ['fanInfo.fanAlarm']:false,
    })
    let i = tmp?2:1
    app.ble_send_data(风扇,设备编号,属性设置,数据类型,开关,[i])     //发送蓝牙数据
    util.vibrateShort()     //手机震动
  },
```

### 2.3 小程序蓝牙数据接收

在蓝牙设备成功连接并获取服务后，调用 **wx.notifyBLECharacteristicValueChange** 接口对蓝牙服务特征进行监听。调用 **wx.notifyBLECharacteristicValueChange** 接口，当监听的蓝牙服务特征值变化事件 ，会接收到设备推送的 notification。

启英小程序sample代码中，已封装好蓝牙数据接收函数 **ble\_rcv\_data**，当小程序存在多个设备页面时，会根据同蓝牙设备 **deviceId** 绑定的 **蓝牙设备编号blue\_devices(启英蓝牙设备补充的广播信息)** 去判断把蓝牙数据分配给对应界面进行逻辑处理，**用户根据自己小程序应用，可自定义该函数。**

```
// 在app.js文件中

/* 小程序接收启英语音蓝牙设备状态上报的蓝牙数据 */
ble_rcv_data(deviceId,datas)
```

**注意：调用wx.notifyBLECharacteristicValueChange接口，必须设备的特征支持 notify 或者 indicate 才可以成功调用。**

#### 示例：风扇设备处理蓝牙数据

通过getApp()获取小程序app全局对象的数据或方法，把风扇页面接收处理蓝牙数据的函数赋给app全局对象，当小程序监听到蓝牙设备特征值改变时，直接通过app去调用风扇的蓝牙数据接收处理函数，实现风扇界面处理协议渲染界面功能。

```
// 在electricFan2.js文件中

const app = getApp()            //获取小程序app全局对象的数据或方法

  /**
   * 生命周期函数--监听页面加载执行
   */
  onLoad(options) {
    let that = this
    app.globalData.fanFunc2.fanDataDeal = that.fanRecvData  //在风扇界面加载时,设备的蓝牙数据      接收处理函数赋给app全局对象
  },

 /**
  * 蓝牙数据接收处理函数，根据协议去渲染界面
  */
fanRecvData(id,e){
    .........
}

-------------------------------------------------------------------------------

// 在app.js文件中
  globalData: {     //小程序的全局数据
    //风扇接收蓝牙数据的处理函数
    fanFunc2:{
      fanDataDeal:function(){},
    },
  },

/* 小程序接收蓝牙数据时，使用app全局的特性，直接调用风扇的蓝牙数据接收处理函数 */
ble_rcv_data(deviceId,datas){
    let that = this
    that.globalData.fanFunc2.fanDataDeal(deviceId,datas)
}
```

### 2.4 小程序示例

#### 2.4.1 页面背景更换

* ##### 页面背景颜色修改

在页面的.wxss样式文件中，通过设置page{ **background:#f1e2d6;** }属性，修改本页面的背景颜色。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E7%A4%BA%E4%BE%8B-1.png)

在页面的.wxss样式文件中，通过设置page{ **background: linear-gradient(to right, #ffffff, #000000 );** /\* 用于设定线性渐变背景色 \*/}属性，其中颜色代码参数是需要用逗号隔开的多种颜色代码参数，每个参数代表一个色值。以下是线性渐变的基本语法示例：

```
background:linear-gradient(方向, 色值1, 色值2, ... ,色值n);
```

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E7%A4%BA%E4%BE%8B-2.png)

* ##### 使用image组件作为图片背景

使用image组件，通过绝对定位方式把该组件放置在页面最底层，作为图片背景界面。

```
<image class="imagebackground" src="../images/electric_fan2/sapu_background.jpg"></image>
```

```
.imagebackground{
  width: 100%;        /* 设置宽度 */
  height: 100%;       /* 设置高度 */
  position: absolute;/* 绝对位置 */
  top: 0;         /* 绝对定位 */
  left: 0;            /* 绝对定位 */
  z-index: -999;  /* 堆叠顺序，大的在上，小的在下 */
}
```

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E7%A4%BA%E4%BE%8B-3.png)

#### 2.4.2 界面新增一个“开关”控制图标

* ##### 新增图标的界面代码

利用 **视图容器view** 给“开关”图标位置布局，在该容器中使用媒体 **image组件** 显示了一个长款120rpx大小“开关”图片。

```
<!-- 页面wxml代码 -->
<view class="center_view">
    <image class="power_icon" src="../images/electric_fan2/powerOff.png"                   mode="aspectFit"/>
</view>
```

```
/* 页面wxss代码 */
.center_view{
  width: 100%;
  display: flex;
  position: absolute;
  justify-content: center;
  align-items: center;
  margin-top: 23%;
  height: 500rpx;
}

.power_icon{
  width: 120rpx;
  height: 120rpx;
}
```

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E7%A4%BA%E4%BE%8B-4.png)

* ##### “开关”图标逻辑代码（发送蓝牙协议，接收蓝牙协议处理）

在页面wxml代码里给 **“开关”image组件** bind绑定点击事件，触发事件时，其事件处理函数为 **fanPowerChange**。

视图层通过 **大括号{{}}**（Mustache语法）能获取到当前页面的数据，用到三目运算符判断页面数据fanInfo.fanPower的值进行“开机”和“关机”**2个状态图的图片切换**。

```
<!-- 页面wxml代码 -->
<view class="center_view">
  <image bindtap="fanPowerChange" class="power_icon" src="../images/electric_fan2/{{fanInfo.fanPower == false ? 'powerOff':'powerOn'}}.png" mode="aspectFit"/>
</view>
```

**发送“开关”蓝牙数据协议**，当点击“开关”组件时点击事件触发，调用 **fanPowerChange** 函数，该函数中回调用小程序全局 **ble\_send\_data** 函数去发送蓝牙数据，**用户可根据自身协议修改该函数**。

```
// 页面js代码
const app = getApp()    //获取全局方法
//启英协议部分，根据自身协议进行自定义
const 风扇 = 0x06
const 设备编号 = 0x07
const 属性设置 = 0x01
const 状态查询 = 0x03
const 数据类型 = 0x01

const 开关 = 0x01   //“开关”功能ID

const 关闭 = 0x01
const 开启 = 0x02

//Page注册页面
Page({
  data: {
    fanInfo:{     //风扇功能信息数据
      fanPower:false, //风扇开关，true：开启  false：关闭
    },
  },

  /**
   * 生命周期函数--监听页面加载执行
   */
  onLoad(options) {
    app.globalData.fanFunc2.fanDataDeal = this.fanRecvData    //在风扇界面加载时,设备的蓝牙数据的接收处理函数赋给app全局对象
  },

  //开关image组件，点击事件处理函数
  fanPowerChange(){
    let tmp = !this.data.fanInfo.fanPower
    this.setData({
      ['fanInfo.fanPower']:tmp,   //新状态立即渲染到界面
    })
    app.ble_send_data(风扇,设备编号,属性设置,数据类型,开关,[tmp?开启:关闭])   //启英协议，用户自定义该部分
  },

  //蓝牙数据接收函数，id：deviceId  ，ble_data：接收蓝牙设备的数据
  fanRecvData(id, ble_data){
    let tmp_id = id   //存在多个相同设备时，接收蓝牙数据时需要用id判断，要不要渲染界面
    let recvData = ble_data       //接受的蓝牙数据
    let dealType = recvData[7]    //功能类型，启英协议第七个字节判断功能
    let dealData = recvData[10]   //功能状态，启英协议第十个字节判断功能状态

    //启英蓝牙数据处理如下，用户根据自身协议完善该部分
    switch (dealType) {
      case 开关:{
        if(dealData == 关闭){
          console.log('关闭风扇')
          this.setData({
            ['fanInfo.fanPower']:false,       //修改页面data值，并渲染界面
          })
        }
        else{
          console.log('打开风扇')
          this.setData({
            ['fanInfo.fanPower']:true,        //修改页面data值，并渲染界面
          })
        }
      }
      break;
      default:{
        console.log("other data")
      }
        break;
    }
  }
})
```

**语音设备发送“开关”协议的小程序处理**，在小程序成功跟蓝牙设备建立连接和特征是监听后，设备发送蓝牙数据给小程序，小程序 **ble\_rcv\_data** 会接收蓝牙数据，会通过小程序getApp()全局方法的特性，把蓝牙数据交给 **fanRecvData** 函数进行处理，fanRecvData函数会根据协议去处理渲染小程序界面。

```
// 在electricFan2.js文件中
const app = getApp()          //获取小程序app全局对象的数据或方法
Page({
  /**
   * 生命周期函数--监听页面加载执行
   */
  onLoad(options) {
  app.globalData.fanFunc2.fanDataDeal = this.fanRecvData  //在风扇界面加载时,设备的蓝牙数据      接收处理函数赋给app全局对象
  },

     /**
      * 蓝牙数据接收处理函数，根据协议去渲染界面 id：deviceId  ，ble_data：接收蓝牙设备的数据
      */
    fanRecvData(id,ble_data){
        .........
    }
)}
---------------------------------------------------------
// 在app.js文件中
  globalData: {       //小程序的全局数据
    //风扇接收蓝牙数据的处理函数
    fanFunc2:{
      fanDataDeal:function(){},
    },
  },

/* 小程序接收蓝牙数据时，使用app全局的特性，直接调用风扇的蓝牙数据接收处理函数 */
ble_rcv_data(deviceId,datas){
    let that = this
    that.globalData.fanFunc2.fanDataDeal(deviceId,datas)
}
```

## 3. CI23LC语音蓝牙SDK代码部分说明

本节主要以 CI23LC 蓝牙SDK 添加蓝牙风扇设备为示例，讲述如何新建开发蓝牙设备并与”启英物联”小程序进行蓝牙数据交互。

启英\*\*CI23LC\_SDK\_BLE\*\*代码可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment) 资料库中搜索下载并使用。

CI23LC SDK开发入门可以到 ☞[启英泰伦文档中心](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/start/CI23LC_SDK_Quick_Start/)  查看相关资料。

### 3.1 蓝牙宏配置

**打开sample工程下src\user\_config.h文件，蓝牙宏配置如下：**

```
//----蓝牙相关用户配置------
#define USE_BLE_MOUDLE                      1           //使能蓝牙功能
#define USE_CI_APPLET_ENABEL                1           //使用启英物联小程序1-使能 0-关闭，客户如果用自己的小程序务必关闭
#define BLE_ADV_NAME_APPEND_FLASH_ID        1           //蓝牙广播名称自动追加2字节flash id, 保证每个设备广播名称不一样,flash_id会占用5字节，自定义长度最长为24字节
#define BLE_USER_DEFINE_ADV_NAME_CONTENT   "CI_BLE"     //用户可自定义,最长不能超过29字节，中文一个字符占3字节;
                                                        //注意如果使用启英物联小程序，最大长度不能超过18字节(厂商信息占用了字节数)

//服务定义-如需修改该下面三个值，小程序也要做同步修改(不可用0x2900之前的值不可用)
#define BLE_UUID_CIAS_SERVICE              0XAE3A     //服务入口
#define BLE_UUID_CIAS_WRITE                0XAE3B     //手机发送数据到蓝牙芯片特征值
#define BLE_UUID_CIAS_NOTIFY               0XAE3C     //蓝牙芯片发送数据到手机特征值
#define BLE_UUID_CIAS_CMD_NOTIFY           0XAE3D     //获取词条信息特征值
```

### 3.2 蓝牙广播数据设置

根据产品有不同领域和不同规格的概念，CI23LC SDK代码中定义了产品和功能等信息的宏，并通过/蓝牙把该信息广播出去，小程序可通过蓝牙广播信息去区别该蓝牙产品的应用领域和应用功能。

在app\_ble\demo\cias\_demo\_config.h文件中，已对一些领域的ID进行定义，若代码中有多个蓝牙设备数据的处理代码(如：空调、风扇等)，用户可对DEV\_DRIVER\_EN\_ID宏进行修改，使SDK执行其中一个蓝牙数据的设备处理代码，本示例中把DEV\_DRIVER\_EN\_ID定义未风扇设备类型。

```
//DEV_DRIVER_EN_ID  设备类型ID
//DEV_TYPE_ID       设备主类型ID
//DEV_NUMBER_ID     设备次编号ID
//CONFIG_TYPE       设备功能类型，0:蓝牙连接; 1:iot设备+ble配网; 2:蓝牙连接; 3:蓝牙+红外方案;                                    4:蓝牙广播;

#define DEV_DRIVER_EN_ID                    DEV_FAN_MAIN_ID
// //主设备ID定义
#define DEV_IR_CONTROL_MAIN_ID              0x01   //红外遥控器
#define DEV_AIRCONDITION_MAIN_ID            0X02   //空调
#define DEV_LIGHT_CONTROL_MAIN_ID           0x03   //灯控
#define DEV_SOUND_MAIN_ID                   0x04   //音响设备
#define DEV_TEA_BAR_MAIN_ID                 0x05   //茶吧机
#define DEV_FAN_MAIN_ID                     0x06   //风扇
#define DEV_HEATTABLE_MAIN_ID               0x07   //取暖桌
#define DEV_WARMER_MAIN_ID                  0x08   //取暖器
#define DEV_WATERHEATED_MAIN_ID             0x09   //水暖毯

#if(DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)    
    #define DEV_TYPE_ID       DEV_AIRCONDITION_MAIN_ID
    #define DEV_NUMBER_ID     1   
    #define CONFIG_TYPE       0  
#elif(DEV_DRIVER_EN_ID == DEV_LIGHT_CONTROL_MAIN_ID)               
    #define DEV_TYPE_ID        DEV_LIGHT_CONTROL_MAIN_ID
    #define DEV_NUMBER_ID      DEV_LIGHT_CONTROL_RGB_SUB_ID
    #define CONFIG_TYPE       0
    #if CIAS_BLE_ADV_GROUP_MODE_ENABEL
    #define CONFIG_TYPE       4     //小程序区别广播设备
    #endif
#elif (DEV_DRIVER_EN_ID == DEV_TEA_BAR_MAIN_ID)  
    #define DEV_TYPE_ID       DEV_TEA_BAR_MAIN_ID
    #define DEV_NUMBER_ID     1   
    #define CONFIG_TYPE       0 
#elif(DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID)  
    #define DEV_TYPE_ID       DEV_FAN_MAIN_ID
    #define DEV_NUMBER_ID     7 
    #define CONFIG_TYPE       0   
#elif(DEV_DRIVER_EN_ID == DEV_HEATTABLE_MAIN_ID)
    #define DEV_TYPE_ID       DEV_HEATTABLE_MAIN_ID
    #define DEV_NUMBER_ID     1   
    #define CONFIG_TYPE       0 
#elif(DEV_DRIVER_EN_ID == DEV_WARMER_MAIN_ID)
    #define DEV_TYPE_ID       DEV_WARMER_MAIN_ID
    #define DEV_NUMBER_ID     1 
    #define CONFIG_TYPE       0 
#elif(DEV_DRIVER_EN_ID == DEV_WATERHEATED_MAIN_ID)
    #define DEV_TYPE_ID       DEV_WATERHEATED_MAIN_ID
    #define DEV_NUMBER_ID     1 
    #define CONFIG_TYPE       0 
#endif
```

在app\_ble\demo\cias\_ble\_msg\_deal.c文件中，设置该蓝牙设备的广播数据。

```
/**
 * @brief 初始化adv_ind数组里面的广播数据,同样启英物联小程序连接,用户可以自行修改
 */
void ci_ble_adv_data_init(uint8_t *adv_data, char *adv_name)
{
    int adv_name_len = strlen(adv_name);
    adv_data[0] = adv_name_len + 1;
    adv_data[1] = 0x09;
    sprintf(&adv_data[2], "%s", adv_name);
    adv_data[2 + adv_name_len] = 0x02;
    adv_data[2 + adv_name_len + 1] = 0x01;
    adv_data[2 + adv_name_len + 2] = 0x06;

    adv_data[2 + adv_name_len + 3] = 0x07;   //启英小程序协议
    adv_data[2 + adv_name_len + 4] = 0xFF;
    //广播中增添相关信息，小程序扫描到可进行区别设备
    adv_data[2 + adv_name_len + 5] = DEV_MTU_TYPE;  //mtu类型，CI23LC蓝牙连接支持一包240字节
    adv_data[2 + adv_name_len + 6] = CONFIG_TYPE;   //设备功能类型，给小程序区别是ble/iot设备
    adv_data[2 + adv_name_len + 7] = DEV_TYPE_ID;   //设备主类型ID
    adv_data[2 + adv_name_len + 8] = DEV_NUMBER_ID; //设备次编号ID
    unsigned short crc_cal = crc16_ccitt(0, &adv_data[7 + adv_name_len], 4);//crc校验
    adv_data[2 + adv_name_len + 9] = crc_cal >> 8;
    adv_data[2 + adv_name_len + 10] = crc_cal & 0xFF;

}
```

### 3.3 风扇设备的代码文件和蓝牙数据相关函数补充

#### 3.3.1 新建风扇设备代码文件

新建风扇设备的.h和.c代码文件，并在运行工程project\_file\source\_file.prj文件中把新建的.c的文件路径放进。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/SDK%E6%96%B0%E6%B7%BB%E9%A3%8E%E6%89%87%E7%A4%BA%E4%BE%8B-1.png)

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/SDK%E6%96%B0%E6%B7%BB%E9%A3%8E%E6%89%87%E7%A4%BA%E4%BE%8B-2.png)

#### 3.3.2 风扇设备代码文件完善

* ##### **完善风扇设备.h头文件代码**

定义设备词条ID宏，根据自身协议去定义设备功能ID，定义常用的功能状态数据，定义设备状态结构体等信息，用户可参考app\_ble\demo\cias\_fan\_msg\_deal.h文件，代码如下。

```
#ifndef __CIAS_FAN_MSG_H__
#define __CIAS_FAN_MSG_H__

/* 命令词cmd_id或命令词semantic_id，用宏定义方便.c代码处理 */
#define TURN_ON                2//开机
#define TURN_OFF               3//关机
#define SPEED_ONE              4//一档风
#define SPEED_TWO              5//二档风
#define SPEED_THREE            6//三档风
#define SPEED_FOUR             7//四档风
#define SPEED_FIVE             8//五档风
#define SPEED_SIX              9//六档风
#define SPEED_REDUCE           10//风速减小
#define SPEED_RAISE            11//风速增大
#define SPEED_MIN              12//风速最小
#define SPEED_MAX              13//风速最大 
#define SHAKE_ON               14//打开摇头 
#define SHAKE_OFF              15//关闭摇头 
#define SHAKE_LR_ON            16//打开左右摇头   
#define SHAKE_LR_OFF           17//关闭左右摇头   
#define SHAKE_HD_ON            18//打开上下摇头   
#define SHAKE_HD_OFF           19//关闭上下摇头   
#define NATURAL_ON             20//打开自然风    
#define SLEEP_ON               21//打开睡眠风    
#define NORMAL_ON              22//打开正风
#define ANION_ON               23//开负离子
#define ANION_OFF              24//关闭负离子
#define TIMMING_1H             25//定时一小时    
#define TIMMING_2H             26//定时二小时
#define TIMMING_3H             27//定时三小时    
#define TIMMING_4H             28//定时四小时    
#define TIMMING_5H             29//定时五小时    
#define TIMMING_6H             30//定时六小时
#define TIMMING_7H             31//定时七小时    
#define TIMMING_8H             32//定时八小时    
#define TIMMING_9H             33//定时九小时    
#define TIMMING_10H            34//定时十小时    
#define TIMMING_11H            35//定时十一小时   
#define TIMMING_12H            36//定时十二小时   
#define TIMING_OFF             37//关闭定时
#define VOICE_UP               38//音量增大 
#define VOICE_DOWN             39//音量减小
#define VOICE_MAX              40//最大音量 
#define VOICE_MIN              41//最小音量

/* 微信启英小程序蓝牙协议数据 */
//function_id   功能id
#define FAN_POWER              0x01    //开关
#define FAN_SPEED              0x02    //风速
#define FAN_SHAKE              0x03    //风向
#define FAN_MODE               0x04    //模式
#define FAN_ANION              0x07    //负离子
#define FAN_TIMING             0x08    //定时
#define FAN_SPEAKER            0x09    //播报音量
#define FAN_ASR                0x0A    //语音识别
//function_data  功能状态数据
#define DEV_SPEED_MAX          6       //最大风档位
#define DEV_SPEED_MIN          1       //最小风档位
#define DEV_TIMING_MAX         0xAC    //最大定时(定时十二小时)
#define DEV_TIMING_MIN         0xA1    //最小定时(定时一小时)

#define SHAKE_LR_ON_DATA       0x12    //打开左右摇头
#define SHAKE_LR_OFF_DATA      0x11    //关闭左右摇头
#define SHAKE_HD_ON_DATA       0x22    //打开上下摇头
#define SHAKE_HD_OFF_DATA      0x21    //关闭上下摇头

#define VOICE_UP_DATA          0xF1    //音量增大   
#define VOICE_DOWN_DATA        0xF2    //音量减小
#define VOICE_MAX_DATA         0xF3    //最大音量   
#define VOICE_MIN_DATA         0xF4    //最小音量

#define FAN_MODE_NORMAL        0x03    //正常模式
#define FAN_MODE_SLEEP         0x04    //睡眠模式
#define FAN_MODE_NATURAL       0x05    //自然模式

/* 风扇设备状态 */
typedef struct
{
    uint8_t power;      //电源
    uint8_t speed;      //风速
    uint8_t shake;      //摇头
    uint8_t mode;       //模式
    uint8_t anion;      //负离子
    uint8_t timing;     //定时
    uint8_t asr_status; //语音识别
}fan_dev_t;

#endif
```

* ##### **完善风扇设备.c文件代码**

设备.c文件代码主要包含，设备的初始化状态、识别词条设备状态上报处理、接收小程序数据处理、小程序查询设备状态处理、定时器处理等相关函数，用户可参考app\_ble\demo\cias\_fan\_msg\_deal.c代码完善自己设备的.c代码文件。

**注意：可根据设备实际功能去设计跟小程序的蓝牙数据协议内容**

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/SDK%E6%96%B0%E6%B7%BB%E9%A3%8E%E6%89%87%E7%A4%BA%E4%BE%8B-3.png)

当设备处理小程序蓝牙协议时，设备端若要发协议给主控，CI23LC SDK代码中已预留uart\_send\_asr(uint16\_t cmd\_id)函数，在app\_ble\demo\cias\_ble\_msg\_deal.c文件中，可根据自己的协议实现该部分内容。

```
void uart_send_asr(uint16_t cmd_id)
{
/* #if (UART_PROTOCOL_VER == 1)
    uart_send_AsrResult(cmd_id, 0);
#elif (UART_PROTOCOL_VER == 2)
    vmup_send_asr_result_cmd(cmd_id, 0);
#elif (UART_PROTOCOL_VER == 255)
    usr_send_asr_result(cmd_id);
#endif // 0 */
}
```

#### 3.3.3 CI23LC跟小程序蓝牙数据处理的文件完善

在app\_ble\demo\cias\_ble\_msg\_deal.h文件中，声明风扇设备蓝牙数据的处理函数方便后面调用，**定义的蓝牙协议格式用户可修改。**

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/SDK%E6%96%B0%E6%B7%BB%E9%A3%8E%E6%89%87%E7%A4%BA%E4%BE%8B-4.png)

在app\_ble\demo\cias\_ble\_msg\_deal.c文件中，把风扇已声明四个函数写在代码对应位置中，示例代码如下：

* **设备功能属性初始化函数**

```
/**
 * @brief 设备功能状态初始化，记录各功能状态，供小程序同步状态
 *
 */
void dev_state_init(void)
{
#if (DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)  //代码里存在多个设备时判断
    aircondition_init();    //空调设备各功能状态初始化
#elif (DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID) //代码里存在多个设备时判断
    fan_init();     //风扇设备各功能状态初始化
#endif
}
```

* **接收小程序蓝牙数据处理函数**

```
/**
 * @brief 接收并处理蓝牙消息任务，按照蓝牙小程序协议或自定义蓝牙协议，解析设备端收到的手机APP消息(该函数只适用于和启英物联APP交互使用)
 *
 */
void ci_ble_recv_task()
{
    BaseType_t err = pdPASS;
    ble_msg_V1_t recv_ble_msg;
    uint16_t recv_ble_type;
    ble_msg_queue = xQueueCreate(10, sizeof(ble_msg_V1_t));
    while (1)
    {
        /* 阻塞接收系统消息 */
        if (xQueueReceive(ble_msg_queue, &recv_ble_msg, portMAX_DELAY) != pdPASS)
        {
            mprintf("ble_msg_queue rcv error ...\r\n");
        }
        else
        {
            recv_ble_type = recv_ble_msg.dev_type;
            mprintf("ble_recv_msg type = 0x%x\r\n", recv_ble_type);
            switch (recv_ble_type)
            {
#if (DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID) //代码里存在多个设备时判断
            case AIRCONDITION_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)  //控制协议类型判断
                {
                    aircondition_callback(recv_ble_msg);    //小程序下发空调控制协议数据
                }
                else if (recv_ble_msg.function_type == STATE_QUERY) //查询协议类型判断
                {
                    aircondition_query(recv_ble_msg);       //小程序下发查询空调状态协议数据
                }
            }
            break;
#elif (DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID) //代码里存在多个设备时判断
            case FAN_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)  //控制协议类型判断
                {
                    fan_callback(recv_ble_msg); //小程序下发查询风扇状态协议数据
                }
                else if (recv_ble_msg.function_type == STATE_QUERY) //查询协议类型判断
                {
                    fan_query(recv_ble_msg);    //小程序下发查询风扇状态协议数据
                }
            }
            break;
#endif
            default:
                break;
            }
        }
    }
}
```

**注意：ble\_msg\_queue队列若未收到数据，请在ci\_ble\_recv\_data\_handle中查看数据的协议匹配并且crc校验通过。**

* **识别词条数据处理函数**

```
/**
 * @brief 组装蓝牙设备发送的消息，按照蓝牙小程序协议或自定义协议，上报本地IOT事件到手机APP端
 *
 */
void deal_ble_send_msg(uint16_t cmd_id)
{
    switch (cmd_id)
    {
    default:
    {
#if (DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)  //代码里存在多个设备时判断
        aircondition_report(cmd_id);        //空调设备识别词条数据处理
#elif (DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID) //代码里存在多个设备时判断
        fan_report(cmd_id);     //风扇设备识别词条数据处理
#endif
        break;
    }
    }
}
```

最后编译烧录CI23LC固件，即可通过微信小程序，搜索添加蓝牙设备进行蓝牙控制。

**注意事项：设备端设置的蓝牙服务跟微信小程序与设备建立通信获取的服务要一致。**