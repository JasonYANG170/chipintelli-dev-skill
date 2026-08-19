<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E5%BA%93%E4%B8%8E%E6%A1%86%E6%9E%B6/SDK%E8%BD%AF%E4%BB%B6%E7%BB%93%E6%9E%84/ -->

# SDK软件结构

---

## 1. SDK软件结构图

![SDK软件结构](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E5%BA%93%E4%B8%8E%E6%A1%86%E6%9E%B6/img/SDK%E8%BD%AF%E4%BB%B6%E7%BB%93%E6%9E%84-1.png)

图1-1 SDK软件结构

---

## 2. SDK目录结构

```
--components                 // 功能组件
----asr                      // 语音识别
----alg                      // 语音前处理算法库
------denoise                // 降噪
----assist                   // 辅助函数，例如：测试算法的计时函数等
----audio_in_manage          // 音频采集实时任务
----audio_pre_rslt_iis_out   // 语音前处理结果输出组件
----ci_nvdm                  // 用户数据管理
----cmd_info                 // 固件信息解析
----codec_manager            // codec管理器
----ci_cwsl                  // 自学习
----status_share             // 状态共享
----fft                      // fft管理器
----flash_control            // flash管理器
----flash_encrypt            // flash加密策略
----freertos                 // 操作系统
----led                      // 三色灯控管理器
----log                      // 日志打印
----msg_com                  // 串口协议
----ota                      // OTA升级
----player                   // 播放器
----RISCV                    // RISCV相关
----sys_monitor              // 系统监控器
--projects                   // 应用示例代码
----nn_denoise_rtc_sample       // ci13XX系列芯片模板工程
--driver                     // 驱动
----boards                   // 板级支持，例如1302/1306
----ci130x_chip_driver       // 芯片内部驱动，如IIC驱动
----third_device_driver      // 外部驱动，如外部codec驱动ES8388
--startup                    // 启动代码
--system                     // 统一使用的头定义等
--tools                      // 固件构建工具
--utils                      // 调试工具集
```

---

## 3. SDK用户代码说明

SDK用户代码区域如下图3-1所示：

![SDK用户代码区域](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E5%BA%93%E4%B8%8E%E6%A1%86%E6%9E%B6/img/SDK%E8%BD%AF%E4%BB%B6%E7%BB%93%E6%9E%84-2.png)

图3-1 SDK用户代码区域

下列文件所在目录：CI-SDK-NN-ENC\_Vx.x.x\projects\nn\_denoise\_rtc\_sample\src\

| 文件名 | 描述 |
| --- | --- |
| main.c | 主函数所在文件：包含任务创建、平台初始化、系统启动代码 |
| system\_hook.c | 事件钩子接口c文件：系统启动、唤醒、退出唤醒、语音识别事件钩子函数 |
| system\_hook.h | 事件钩子接口h文件 |
| system\_msg\_deal.c | 系统消息处理任务c文件 |
| system\_msg\_deal.h | 系统消息处理任务h文件 |
| user\_config.h | 用户配置宏定义.h文件 |
| user\_msg\_deal.c | 用户代码.c文件：串口协议、IIC协议、按键消息等用户处理 |
| user\_msg\_deal.h | 用户代码.h文件 |
| ci\_uart\_eq\_param.c | EQ通过串口配置参数源文件 |
| ci\_uart\_eq\_param.h | EQ通过串口配置参数.h文件 |

---

## 4. SDK代码逻辑分析

由于语音识别的系统的特定需要，因此SDK中程序已经包含了大量初始化工作，为了帮助用户更快的熟悉代码结构流程，现将程序启动流程和工作状态做以简单说明。

![SDK程序启动流程和工作状态](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E5%BA%93%E4%B8%8E%E6%A1%86%E6%9E%B6/img/SDK%E7%A8%8B%E5%BA%8F%E6%B5%81%E7%A8%8B.png)

图5-1 SDK程序启动流程和工作状态

如上图所示，系统上电启动后进入main函数初始化相关硬件，然后创建一个vTaskCreate线程后启动FreeRTOS系统，vTaskCreate线程中会创建识别和播报的相关线程，之后系统进入sleep状态并采集语音输入。

当正确的语音输入给识别线程后，识别线程将通过消息队列的机制将消息发送到用户线程中进行处理，在SDK的用户线程中，已经完成了一部分操作比如播放对应的播报语音，切换系统状态，发送串口协议等工作，用户增加代码时一般只需在用户线程中增加代码，因此理解用户线程的中的消息处理机制就可以轻松扩展功能。

当外部通讯串口发送串口协议控制语音板时，将在通讯串口中断中解析串口协议，成功解析后将在中断中调用vmup\_port\_send\_packet\_rev\_msg函数发送串口消息给用户线程，用户线程收到该消息后将在依次调用deal\_userdef\_msg>>userapp\_deal\_com\_msg>>userapp\_deal\_cmd最终在此处同步或异步（大多数为同步执行，但播放播报词为异步请求）执行功能，完成后发回ACK到通讯串口。