<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/SDK%E6%B3%A2%E7%89%B9%E7%8E%87%E6%A0%A1%E5%87%86%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E/ -->

请点击保存PDF文档

# SDK波特率校准功能

## 1、功能特点

* 只有SDK配置使用内部晶振时，波特率校准功能可选择开启，用于预防内部晶振可能出现的 *频偏*(频率偏差,ppm)现象。
* 开启波特率校准，同时开启SDK自带的串口协议，会定期向通信的客户端（以下称客户端）发送一条命令，通信客户端在超时时间内回复同样的命令，达到实时校准波特率的目的。SDK中自带的语音模块协议V1（一代协议）、V2（二代协议）均具备此功能。
* 波特率校准频次间隔时间可配置，等待回复的超时时间也可配置。
* 波特率校准的协议命令可修改，可自定义。
* 3代SDK V1.5.7及以上才支持此功能。

## 2、SDK配置及使用方法

### 2.1 开启内部RC功能

在“user\_config.h”中配置使用内部RC

```
#define USE_EXTERNAL_CRYSTAL_OSC             0       //0: 使用内部RC，1、使用外部晶振
```

### 2.2 开启波特率校准功能

在“user\_config.h”中配置开启波特率校准功能、发送间隔时间、接收回复的最长等待时间

```
#define UART_BAUDRATE_CALIBRATE         1           //是否使能波特率校准功能
#define BAUDRATE_SYNC_PERIOD            180000       // 波特率同步周期，单位毫秒（发送间隔时间）
#define BAUD_CALIBRATE_MAX_WAIT_TIME    400          // 等待反馈包的超时时间，单位毫秒（接收回复时间）
```

**注解**：上述配置，表示每隔180000毫秒（180秒），就会向通信客户端发送一次波特率校准的命令，通信客户端需在400毫秒内回复，超时则校准失败。

**注意**：按上述配置时，客户端若想收到完全正确的上电通知协议，请务必在400毫秒内回复校准反馈包。等待反馈包的时间尽量小于400ms，因为每次会发起多次校准请求，以免校准时间过长。

### 2.3 设置同步命令(SDK自带串口协议)

如果使用SDK自带的通信协议，已经实现了命令发送及接收相关功能，只需要在“命令词信息表”（$SDK/projects/oflline\_asr\_sample/frimware/user\_file/cmd\_info/[60000]xxx.xls）中添加或修改，如下图：

![命令词信息表](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/img/SDK%E6%B3%A2%E7%89%B9%E7%8E%87%E6%A0%A1%E5%87%86%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-3.png)

SDK自带串口协议会以语音识别结果命令发送此命令，做为波特率同步命令，客户端在解析到此命令词ID时，需将原命令数据返回，不能做任何修改。
如果使用SDK自带串口协议，到此波特率校准功能已经配置完成。如果使用自定义串口协议，请继续往下看。

### 2.4 自定义串口协议相关修改

#### 2.4.1 模块实始化

在自定义协议初始化时，需要调用波特率校准模块实始化函数:

```
//UARTx: 串口号。
//baudrate: 默认波特率。
//send_sync_req_func: 同步命令发送回调函数针，波特率校准模块通过调用此函数指针发送同步命令。
void baudrate_calibrate_init(UART_TypeDef *UARTx, uint32_t baudrate, send_sync_req_func_t send_sync_req_func);
```

#### 2.4.2 实现同步命令发送接口

实现同步命令发送接口，以做为“baudrate\_calibrate\_init”函数的最后一个参数传递给波特率校准模块。波特率校准模块通过调用此函数指针发送同步命令。
因为同步命令的生成、发送和接收都是由串口协议模块完成的，所以可以灵活的自定义。可以设计好同步命令，并以常量的形式写在代码中。

#### 2.4.3 在接收命令处理逻辑中添加同步命令处理

在做波特率校准时，还是需要串口协议模块来接收同步命令。当接收到波特率同步命令时，通过调用接口来通知波特率校准模块：

```
void baudrate_calibrate_set_ack();
```

至此，波特波率校准功能配置完成。

## 3、波特率自适应的电控配合

### 3.1、波特率自适应的流程图

![命令词信息表](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/img/SDK%E6%B3%A2%E7%89%B9%E7%8E%87%E6%A0%A1%E5%87%86%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-4.png)

### 3.2、如果产品的开机音根据电控确定，则上电时注意播放的流程如下框图

![命令词信息表](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/img/SDK%E6%B3%A2%E7%89%B9%E7%8E%87%E6%A0%A1%E5%87%86%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-5.png)