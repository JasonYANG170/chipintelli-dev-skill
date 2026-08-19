<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F%E5%88%86%E5%8C%85%E8%AF%B4%E6%98%8E/ -->

# 微信小程序分包说明

小程序☞[分包说明文档](https://developers.weixin.qq.com/miniprogram/dev/framework/subpackages.html)

概述： 某些情况下，开发者需要将小程序划分成不同的子包，在构建时打包成不同的分包，用户在使用时按需进行加载。

目前小程序分包大小有以下限制：

* 整个小程序所有分包大小不超过 30M（服务商代开发的小程序不超过 20M）
* 单个分包/主包大小不能超过 2M

对小程序进行分包，可以优化小程序首次启动的下载时间，以及在多团队共同开发时可以更好的解耦协作。

## 1. 小程序进行分包

开发者通过在 app.json `subPackages` 字段声明项目分包结构：

```
{
  "pages": [
    "pages/index/index",
    "pages/device/device",
    "pages/device/deviceinfo/deviceinfo",
    "pages/device/deviceinfo/cmdinfo/cmdinfo"
  ],
  "subPackages": [
    {
      "root": "control/Fan",
      "name": "Fan",
      "pages": [
        "electricFan/electricFan",
        "electricFan2/electricFan2"
      ],
      "independent": false
    },
    {
      "root": "control/air",
      "name": "air",
      "pages": [
        "air/air"
      ],
      "independent": false
    }
  ]
}
```

`pages` 中为主包页面信息。

`subPackages` 中，每个分包的配置有以下几项：

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| root | String | 分包根目录 |
| name | String | 分包别名，☞[分包预下载](https://developers.weixin.qq.com/miniprogram/dev/framework/subpackages/preload.html)时可以使用 |
| pages | StringArray | 分包页面路径，相对于分包根目录 |
| independent | Boolean | 分包是否是☞[独立分包](https://developers.weixin.qq.com/miniprogram/dev/framework/subpackages/independent.html) |
| entry | String | 分包入口文件 |

## 2. 打包原则

* 声明 `subPackages` 后，将按 `subPackages` 配置路径进行打包，`subPackages` 配置路径外的目录将被打包到主包中
* 主包也可以有自己的 pages，即最外层的 pages 字段。
* `subPackages` 的根目录不能是另外一个 `subPackages` 内的子目录
* `tabBar` 底部导航栏页面必须在主包内

## 3. 引用原则

* `packageA` 无法 require `packageB` JS 文件，但可以 require 主包、`packageA` 内的 JS 文件；使用 ☞[分包异步化](https://developers.weixin.qq.com/miniprogram/dev/framework/subpackages/async.html) 时不受此条限制
* `packageA` 无法 import `packageB` 的 template，但可以 require 主包、`packageA` 内的 template
* `packageA` 无法使用 `packageB` 的资源，但可以使用主包、`packageA` 内的资源

---

## 4. 分包信息查看

分包成功后在目录列表中可看到生成的文件，例如：**air分包** 和 **Fan分包** 文件

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%88%86%E5%8C%85%E8%AF%B4%E6%98%8E-1.png)

在工具栏，打开详情，在详情的基本信息中点击查看代码依赖分析，可查看小程序总体积大小和分包所占体积大小

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/%E8%93%9D%E7%89%99%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/img/%E5%88%86%E5%8C%85%E8%AF%B4%E6%98%8E-2.png)