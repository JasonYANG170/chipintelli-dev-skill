<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/LOG%E6%97%A5%E5%BF%97/ -->

# 日志(LOG)

---

## 1. 概述

LOG日志输出组件目前仅可以通过串口打印调试信息，故目前仅用于调试，打印信息参考Andriod Logcat设置有6个等级，说明如下：

| 打印等级 | 说明 |
| --- | --- |
| CI\_LOG\_VERBOSE | 详细日志，最高打印等级，用于输出所有日志信息 |
| CI\_LOG\_DEBUG | 调试日志，该类型日志常用于产品开发阶段使用，输出调试信息 |
| CI\_LOG\_INFO | 信息日志，该类型日志用于输出关键信息以方便确认系统状态 |
| CI\_LOG\_WARN | 警告信息，该类型日志用于打印警告信息，表明系统可能遇到问题尝试修复 |
| CI\_LOG\_ERROR | 错误信息，该类型日志用于打印错误信息，报告系统遇到问题 |
| CI\_LOG\_ASSERT | 断言信息，该类型日志用于打印严重错误，即系统遇到严重问题完全无法运行 |

使用该组件在程序响应位置添加输出信息，可以更好的检测系统运行状态，这在产品开发阶段定位调试问题时帮助极大。

---

## 2. 使用说明

该组件源代码包括ci\_log.c、ci\_log.h、ci\_log\_config.h三个文件组成，其中ci\_log.c、ci\_log.h我们已经实现了相关串口输出打印代码，一般不需要修改，使用者仅使用ci\_log\_config.h进行配置，相关宏配置说明如下：

### 2.1. 配置

配置CONFIG\_CI\_LOG\_EN为1则使能该模块，如果配置为0则不会输出打印信息，所有打印代码由宏实现故不会加入编译。配置CONFIG\_CI\_LOG\_UART\_PORT可以选则由哪一个串口输出打印信息。

```
/* LOG模块开关 */
#define CONFIG_CI_LOG_EN            1
/* LOG模块输出串口 */
#define CONFIG_CI_LOG_UART_PORT     ((UART_TypeDef*)CONFIG_CI_LOG_UART)
```

### 2.2. 添加模块统一打印等级

在ci\_log\_config.h中可以自定义一个LOG\_\*的模块打印等级宏，如下所示，通过在相同模块填写该模块的宏等级，这样便可以针对不同模块选择不同的打印等级。

```
/* 打印等级 */
#define LOG_WIFI_EVENT              CI_LOG_DEBUG
#define LOG_DUERAPP                 CI_LOG_DEBUG
#define LOG_AUDIO_PLAY              CI_LOG_INFO
#define LOG_AUDIO_GET_DATA          CI_LOG_INFO
#define LOG_USER                    CI_LOG_DEBUG
#define LOG_COM_UART                CI_LOG_DEBUG
```

### 2.3. 打印函数

如api手册所述，打印函数包括如下（带参宏实现）：

```
/** @brief 日志打印--详细 */
#define ci_logverbose(comlevel, message, args...) 
/** @brief 日志打印--调试 */
#define ci_logdebug(comlevel, message, args...)  
/** @brief 日志打印--信息 */
#define ci_loginfo(comlevel, message, args...) 
/** @brief 日志打印--警告 */
#define ci_logwarn(comlevel, message, args...)  
/** @brief 日志打印--错误 */
#define ci_logerr(comlevel, message, args...)  
/** @brief 日志打印--断言 */
#define ci_logassert(comlevel, message, args...)
```

使用示例如下：

```
#define LOG_TEST_APP                  CI_LOG_DEBUG

int test_log(void)
{
    int test_index = 0;
    ci_logverbose(LOG_TEST_APP, "logverbose %d", test_index++);
    ci_logdebug(LOG_TEST_APP, "logdebug %d", test_index++);
    ci_loginfo(LOG_TEST_APP, "loginfo %d", test_index++);
    ci_logwarn(LOG_TEST_APP, "logwarn %d", test_index++);
    ci_logerr(LOG_TEST_APP, "logerr %d", test_index++);
    ci_logassert(LOG_TEST_APP, "logassert %d", test_index++);
}
```

注意

ci\_logxxx系列是通过宏实现，切勿将需要使用的函数作为参数以打印函数返回值，这将导致在宏被关闭时函数得不得调用。

---

## 3. 常见问题

* 由于日志输出组件需要使用串口，务必要在系统初始化时初始化相应串口方可使用。