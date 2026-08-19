<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BA%93%E4%B8%8E%E6%A1%86%E6%9E%B6/SDK%E8%BD%AF%E4%BB%B6%E7%BB%93%E6%9E%84/ -->

# SDK软件结构

---

## 1. SDK软件结构图

![SDK软件结构](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BA%93%E4%B8%8E%E6%A1%86%E6%9E%B6/img/SDK%E8%BD%AF%E4%BB%B6%E7%BB%93%E6%9E%84-1.png)

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
----codec_manage             // codec管理器
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
----offline_asr_sample       // ci13lc系列芯片模板工程
--driver                     // 驱动
----boards                   // 板级支持，例如CI13LC
----ci13lc_chip_driver         // 芯片内部驱动，如IIC驱动
----third_device_driver      // 外部驱动，如外部codec驱动ES8388
--startup                    // 启动代码
--system                     // 统一使用的头定义等
--tools                      // 固件构建工具
--utils                      // 调试工具集
```

---

## 3. SDK用户代码说明

SDK用户代码区域如下图3-1所示：

![SDK用户代码区域](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BA%93%E4%B8%8E%E6%A1%86%E6%9E%B6/img/SDK%E8%BD%AF%E4%BB%B6%E7%BB%93%E6%9E%84-2.png)

图3-1 SDK用户代码区域

下列文件所在目录：CI13LC\_SDK\projects\offline\_asr\_sample\src\

| 文件名 | 描述 |
| --- | --- |
| sample\_main.c | 主函数所在文件：包含任务创建、平台初始化、系统启动代码 |
| system\_hook.c | 事件钩子接口c文件：系统启动、唤醒、退出唤醒、语音识别事件钩子函数 |
| system\_hook.h | 事件钩子接口h文件 |
| system\_msg\_deal.c | 系统消息处理任务c文件 |
| system\_msg\_deal.h | 系统消息处理任务h文件 |
| user\_config.h | 用户配置宏定义.h文件 |
| user\_msg\_deal.c | 用户代码.c文件：串口协议、IIC协议、按键消息等用户处理 |
| user\_msg\_deal.h | 用户代码.h文件 |

---

## 4. 添加代码示例

（1）针对唤醒词添加协议等处理代码，找到CI13LC\_SDK\projects\offline\_asr\_sample\src，

* 先在user\_msg\_deal.h里添加处理接口声明，例如：

```
void wake_up_xxx_deal(void);
```

* 然后在user\_msg\_deal.c里添加处理接口定义，例如：

```
void wake_up_xxx_deal(void)
{
    /*处理接口逻辑代码*/
}
```

* 最后在system\_hook.c里调用接口，例如：

```
__WEAK void sys_weakup_hook(void)
{
    #if MSG_COM_USE_UART_EN
    vmup_send_notify(VMUP_MSG_DATA_NOTIFY_WAKEUPENTER);
    #endif

    /*此处添加处理接口调用*/
    wake_up_xxx_deal();
}
```

注意

系统其他状态（系统启动、系统退出唤醒、语音识别），也可参照上述方式添加代码，但需在对应的事件钩子函数中调用。

（2）根据命令词ID添加处理代码，找到CI13LC\_SDK\projects\offline\_asr\_sample\src\user\_msg\_deal.c的deal\_asr\_msg\_by\_cmd\_id函数。

命令词ID的对应命令词，由CI13LC\_SDK\projects\offline\_asr\_sample\firmware\user\_file\cmd\_info[60000]{xxx}cmd\_info.xls指定

```
uint32_t deal_asr_msg_by_cmd_id(sys_msg_asr_data_t *asr_msg， cmd_handle_t cmd_handle， uint16_t cmd_id)
{
    uint32_t ret = 1;
    int select_index = -1;
    switch(cmd_id)
    {
        case 2://“打开空调”
        {
            /*此处根据命令词ID添加处理代码*/
            break;
        }
        /*自行添加case处理命令词ID*/
        /*省略部分代码*/
        default:
            ret = 0;
            break;
    }
    /*省略部分代码*/
}
```

（3）根据语义ID添加处理代码，找到CI13LC\_SDK\projects\offline\_asr\_sample\src\user\_msg\_deal.c的deal\_asr\_msg\_by\_semantic\_id函数。

更多语义ID信息可以访问 ☞[《CI13LC系列芯片语义ID说明文档》](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/CI13LC%E8%AF%AD%E4%B9%89ID%E6%96%87%E6%A1%A3%E8%AF%B4%E6%98%8E/)页面

```
uint32_t deal_asr_msg_by_semantic_id(sys_msg_asr_data_t *asr_msg， cmd_handle_t cmd_handle， uint32_t semantic_id)
{
    uint32_t ret = 1;
    if (PRODUCT_GENERAL == get_product_id_from_semantic_id(semantic_id))
    {
        uint8_t vol;
        int select_index = -1;
        switch(get_function_id_from_semantic_id(semantic_id))
        {
        case VOLUME_UP:        //增大音量
            vol = vol_set(vol_get() + 1);
            select_index = (vol == VOLUME_MAX) ? 1:0;
            break;
        case XXX_XXX:
            /*此处根据语义ID添加处理代码*/
            break;
        /*自行添加case处理语义ID*/
        /*省略部分代码*/
        default:
            ret = 0;
            break;
        }
        /*省略部分代码*/
    }
    /*省略部分代码*/
}
```

## 5. SDK代码逻辑分析

由于语音识别的系统的特定需要，因此SDK中程序已经包含了大量初始化工作，为了帮助用户更快的熟悉代码结构流程，现将程序启动流程和工作状态做以简单说明。

![SDK程序启动流程和工作状态](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%BA%93%E4%B8%8E%E6%A1%86%E6%9E%B6/img/SDK%E8%BD%AF%E4%BB%B6%E7%BB%93%E6%9E%84-3.png)

图5-1 SDK程序启动流程和工作状态

如上图所示，系统上电启动后进入main函数初始化相关硬件，然后创建一个vTaskCreate线程后启动FreeRTOS系统，vTaskCreate线程中会创建识别和播报的相关线程，之后系统进入sleep状态并采集语音输入。

当正确的语音输入给识别线程后，识别线程将通过消息队列的机制将消息发送到用户线程中进行处理，在SDK的用户线程中，已经完成了一部分操作比如播放对应的播报语音，切换系统状态，发送串口协议等工作，用户增加代码时一般只需在用户线程中增加代码，因此理解用户线程的中的消息处理机制就可以轻松扩展功能。

当外部通讯串口发送串口协议控制语音板时，将在通讯串口中断中解析串口协议，成功解析后将在中断中调用vmup\_port\_send\_packet\_rev\_msg函数发送串口消息给用户线程，用户线程收到该消息后将在依次调用deal\_userdef\_msg>>userapp\_deal\_com\_msg>>userapp\_deal\_cmd最终在此处同步或异步（大多数为同步执行，但播放播报词为异步请求）执行功能，完成后发回ACK到通讯串口。