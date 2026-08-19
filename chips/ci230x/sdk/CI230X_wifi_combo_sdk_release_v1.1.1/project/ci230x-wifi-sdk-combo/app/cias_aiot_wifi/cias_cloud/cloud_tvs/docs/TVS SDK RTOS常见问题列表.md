## Q：SDK有适配过那些平台？

A：目前适配过FreeRTOS平台有：XR871，MTK7697c  以及Linux平台

## Q：SDK对硬件有什么要求？

A: CPU频率：单核196MHz以上 内存：200K以上， 需要SD卡或者Flash存储支持

## Q：接入SDK需要那些步骤？

A: 1.需要到腾讯云小微平台申请AppKey及Token

​	2.需要下载tvs rots sdk

​	3.根据 tvs sdk rtos接入指南 接入

​	4.根据硬件平台交叉编译工具编译

​	5.烧录到硬件平台，运行调试

## Q：如何选择返回的TTS音频格式？

A: 可以通过配置CONFIG_DECODE_TTS_IN_SDK来实现:

如果设置为1，则在SDK内部解码，并通过回调tvs_platform_adaptor_soundcard_pcm_write播放PCM

如果设置为0，SDK通过回调tvs_mediaplayer_adapter_tts_data, 将MP3数据传出，由上层解码并播放

## Q：如何切换到语义沙箱环境？

语义服务会把开发中的特性先发布到沙箱环境验证，验证通过后才会发到正式环境，因此如果需要和语义服务调试开发特性，需要切到语义的沙箱环境，在tvs_api_impl.c的tvs_api_impl_init中可以切换到测试环境：

```c
void tvs_api_impl_init() {
	...
	tvs_default_config config = {0};
	// 量产版本必须默认为正式环境
	config.def_env = TVS_API_ENV_TEST;
	// 量产版本必须默认为false
	config.def_sandbox_open = true;
	...
}
```

## Q：如何配置设备的QUA？

A：QUA中包含了设备端的各种信息，包含了设备的版本信息，设备的产品包名（用于区分不同产品的唯一标识），需要接入方在初始化SDK的填入。

使用举例：

```c
tvs_product_qua qua = {0};
// 设备端版本号
qua.version = "1.0.0.1000";
// 设备端包名
qua.package_name = "com.tencent.tvs.demo";
tvs_api_init(&api_callback, &config, &qua);
```

## Q：如何在Linux编译调试？

A：TVS SDK RTOS可以在Linxu系统的采用gcc编译，具体步骤如下(以Ununtu16.04为例)：

1.下载linux适配代码命名为：tvs_rtos_linux

2.下载tvs sdk代码命名为:tvs_rtos_sdk,必需与tvs_rtos_linux放在一个目录下

3.进入tvs_rtos_linux

4.运行: cmake ./

5.运行 make

注意在进行以上步骤前可能需要先安装系统工具:

sudo apt-get install cmake

sudo apt-get install lsb-core 

sudo apt-get install lib32ncurses5-dev



## 附录

### 问题反馈模板

如果遇到SDK的异常或者错误，请通过以下模板反馈给我们。

**【SDK版本号】**

所使用SDK的版本号（SDK的包名包含版本号）。

```
示例：
 1.11.200117.51.DD
```

**【测试机型及序列号】**

出现问题的设备（如果是手机请注明机型）以及传给SDK到序列号（DSN）。

```
示例：
硬件平台：MTK，DSN：12345678
```

**【AppKey】**

接入方从开放平台申请的AppKey

**【SessionId】（可选）**

如果能获取到，请反馈出现问题的请求所使用的SessionId。

> 备注：SessionId可以用来精确查找对应的请求流水，方便定位问题原因。目前可以从日志中搜索“SessionId”来查，如果无法获取可不提供。

**【问题出现时间】**

出现问题的时间，尽量精确，一般需要精确到十分钟以内。例如：2020年1月20日 16:32:23。

**【预置条件】**

出现问题所需的预置条件，例如：预先断开网络，或者其他操作。

```
示例：
1、连接网络可用的wifi
```

**【复现步骤】**

出现问题的操作步骤（描述点击了什么菜单，打开了什么功能，语音输入了什么query等）。

```
示例：
1、输入“叮当叮当”唤醒设备
2、输入“今天天气怎么样”，等待设备回复
```

**【预期结果】**

预期的正确结果是什么。

```
示例：
正确回复今天的天气信息
```

**【问题现象】**

简要描述与预期结果不符的问题现象。

```
示例：
回复“我没有听懂你在说什么，请稍后再试吧”
```

**【问题现象的图片或视频】（可选）**

提供问题现象的图片或者视频，可以帮助根据问题出现的场景更快找到问题的原因。

**【出现概率】**

概率出现 or 必现。

**【SDK日志】**

如果可以拿到请提供SDK的日志文件。