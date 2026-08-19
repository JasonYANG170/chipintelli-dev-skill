<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/SDK%E6%B3%A2%E7%89%B9%E7%8E%87%E6%A0%A1%E5%87%86%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E/ -->

# SDK波特率校准功能

## 1、功能特点

* 只有SDK配置使用内部晶振时，波特率校准功能可选是否开启。
* 开启波特率校准，同时开启SDK自带的串口协议，上电会向通信客户端（以下称客户端）发送一条命令，通信客户端回复同样的命令，芯片在接收数据时检测波特率偏差，达到校准波特率的目的，SDK中自带的语音模块协议V1（一代协议）、V2（二代协议）均具备此功能。
* 波特率校准的协议命令可修改，可自定义。
* CI13LC支持此功能。

## 2、SDK配置及使用方法

### 2.1 开启内部RC功能

先参考文档 ☞[《板级配置使用说明》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/%E6%9D%BF%E7%BA%A7%E9%85%8D%E7%BD%AE%E8%AF%B4%E6%98%8E/)修改板级配置文件。假如
user\_config.h中配置的BOARD\_CONFIG\_FILE宏如下：

```
#define BOARD_CONFIG_FILE           "CI-F1316XGS0XJ-V10.h"
```

先打开SDK\driver\boards\CI-F1316XGS0XJ-V10.h文件，修改使用内部晶振

```
#define USE_EXTERNAL_CRYSTAL_OSC    0  //是否使用外部晶振。0：不使用外部晶振时钟；1：使用外部晶振时钟；
```

### 2.2 开启波特率校准功能

在SDK\driver\boards\board.h中配置开启波特率校准功能

```
#define UART_BAUDRATE_CALIBRATE         1           //是否使能波特率校准功能
```

### 2.3 设置同步命令(SDK自带串口协议)

使用SDK自带的通信协议，已经实现了命令发送及接收相关功能，只需要在“命令词信息表”（$SDK/projects/oflline\_asr\_sample/frimware/user\_file/cmd\_info/[60000]xxx.xls）中添加或修改，如下图：

![命令词信息表](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/img/SDK%E6%B3%A2%E7%89%B9%E7%8E%87%E6%A0%A1%E5%87%86%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-3.png)

SDK自带串口协议只会上电发送1次此命令，客户端在解析到此命令词ID时，需将原命令数据返回，不能做任何修改。
如果使用SDK自带串口协议，到此波特率校准功能已经配置完成。如果使用自定义串口协议，请继续往下看。

### 2.4 自定义串口协议相关修改

#### 2.4.1 模块实始化

在自定义协议初始化时，开启波特率校准宏，参考SDK自带协议初始化串口，且从表格中读取发送同步命令。

```
cmd_handle_t cmd_handle = cmd_info_find_command_by_string("<baud_sync_req>");
baud_sync_req = cmd_info_get_command_id(cmd_handle);
ci_loginfo(CI_LOG_INFO, "baud_sync_req:%08x\n", baud_sync_req);
```

#### 2.4.2 实现命令发送接口

调用下面的函数发送同步命令。同步命令的生成、发送和接收都是由串口协议模块完成的，所以可以灵活的自定义。可以设计好同步命令，并以常量的形式写在代码中。

```
void send_baudrate_sync_req(void);
```

#### 2.4.3 接收到数据时调用波特率校准接口

波特率偏了，可能数据会收错，所以一般在收到错误数据时，进行波特率校准。

```
void baudrate_calibrate(UART_TypeDef *UARTx);
```

至此，波特波率校准功能配置完成。

## 3、波特率自适应的电控配合

### 3.1、波特率自适应的流程图

![命令词信息表](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/img/SDK%E6%B3%A2%E7%89%B9%E7%8E%87%E6%A0%A1%E5%87%86%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-4.png)

### 3.2、如果产品的开机音根据电控确定，则上电时注意播放的流程如下框图

![命令词信息表](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/img/SDK%E6%B3%A2%E7%89%B9%E7%8E%87%E6%A0%A1%E5%87%86%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E-5.png)