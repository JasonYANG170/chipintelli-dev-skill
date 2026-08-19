<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E5%B0%8F%E7%A8%8B%E5%BA%8F%E4%BF%A1%E6%81%AF%E8%A1%A5%E5%85%85%E4%B8%8E%E6%9D%83%E9%99%90%E7%94%B3%E8%AF%B7%E8%AF%B4%E6%98%8E/ -->

# 小程序信息补充与权限申请说明

使用已注册的小程序账号或微信号，登录☞[微信公众平台](https://mp.weixin.qq.com/)

## 1. 小程序账号信息补充

新注册的账号需补充小程序信息、小程序类目

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E4%BF%A1%E6%81%AF%E8%A1%A5%E5%85%85-1.png)

补充小程序信息，如名称、图标、描述等。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E4%BF%A1%E6%81%AF%E8%A1%A5%E5%85%85-2.png)

补充小程序类目信息，并设置主营类目。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E4%BF%A1%E6%81%AF%E8%A1%A5%E5%85%85-3.png)

若小程序采集用户隐私 ，在小程序账号设置->基本设置-> 服务内容声明中，补充 用户隐私保护指引 。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E4%BF%A1%E6%81%AF%E8%A1%A5%E5%85%85-4.png)

## 2. 小程序定位接口权限申请

考虑到蓝牙功能可以间接进行定位，安卓 6.0 及以上版本，无定位权限或定位开关未打开时，无法进行设备搜索。确保用户使用小程序能正常搜索到蓝牙设备，需要在申请小程序位置权限。

在管理->开发管理->接口设置->接口权限下，申请开通 **wx.getLocation** 或 **wx.getFuzzyLocation** 权限。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E6%9D%83%E9%99%90%E7%94%B3%E8%AF%B7-1.png)

根据小程序功能及应用场景业务等方面填写 **申请接口理由** 和 **使用场景截图**，以增大审核通过率。

可参考☞[接口申请案例](https://dldir1.qq.com/WechatWebDev/mp/product_assets/MP接口申请-申请成功案例参考.pdf)

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E6%9D%83%E9%99%90%E7%94%B3%E8%AF%B7-2.png)