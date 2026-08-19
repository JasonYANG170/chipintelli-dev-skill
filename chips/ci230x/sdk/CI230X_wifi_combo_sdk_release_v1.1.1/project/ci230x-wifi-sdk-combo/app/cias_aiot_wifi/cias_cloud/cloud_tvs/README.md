### 什么是TVS SDK RTOS

TVS SDK RTOS是FreeRTOS平台上基于TVS（Tencent Voice Service）云端API协议的软件开发包，可以帮助开发者快速接入TVS，以实现智能语音功能和语音合成功能。

### TVS SDK RTOS有那些功能


SDK提供如下的功能：

 - 语音语义识别功能
 - 语音合成功能
 - 流媒体和TTS音频数据的播放和控制功能；(依赖接入方实现)
 - 语音录制功能；(依赖接入方实现)
 - 系统闹钟的设置、振铃等功能；(依赖接入方实现)

由于FreeRTOS系统API本身没有提供这些功能，而在不同的芯片方案中，这些功能的接口各不相同。
所以，SDK抽象了一套适配层接口，由各个芯片方案的方案商来实现，作为TVS Adapter层，提供给SDK所需要的能力。需要第三方库mbedtls、lwip、libSpeex、Mongoose和cJSON等库的支持

更详细请参考《[TVS SDK RTOS功能清单](./docs/TVS%20SDK%20RTOS功能清单.md)》

### 硬件要求

1.CPU频率建议单核196MHz以上

2.内存建议预留200k以上（多核方案，前端音频模块和TTS、网络流媒体解码和播放模块均工作在另一个核上的情况，可以放宽到最低100k左右）

3.需要SD卡或者内部Flash等存储模块支持

### 版本说明

版本更新和说明详见《[TVS SDK RTOS更新日志](./docs/TVS%20SDK%20RTOS更新日志.md)》

### 开始接入

1.编译源代码可以参考《[TVS SDK RTOS编译指南](./docs/TVS%20SDK%20RTOS编译指南.md)》

2.各接口及参数、常量、枚举的定义，在《[TVS SDK TROS API Doc](./docs/html/index.html)》中有详细说明。

3.接入适配硬件平台可以参考《[TVS SDK RTOS接入指南](./docs/TVS%20SDK%20RTOS接入指南.md)》

4.接入过程中，如果出现问题可以参考《[TVS SDK RTOS常见问题列表](./docs/TVS%20SDK%20RTOS常见问题列表.md)》

5.如果问题难以解决，可以按照《[TVS SDK RTOS问题反馈模板](./docs/TVS%20SDK%20RTOS问题反馈模版.md)》，向我们反馈您遇到的问题。

### 源代码架构说明

源码架构及各模块说明详见《[TVS SDK RTOS源码架构说明](./docs/TVS%20SDK%20RTOS源码架构说明.md)》

### 如何验收

当您接完SDK后，可以参考《[TVS SDK RTOS 验收标准](./docs/TVS%20SDK%20RTOS验证标准.md)》的操作步骤和预期效果，对每一个功能进行验收。






