<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/CI13XX%E4%BD%8E%E5%8A%9F%E8%80%97%E6%96%B9%E6%A1%88%E8%AF%B4%E6%98%8E/ -->

# CI13XX低功耗方案说明

## 概述

CI13XX芯片支持低功耗方案，低功耗方案需要从硬件和软件两个方面一起协同工作实现。以下从硬件和软件两个方面详细说明实现方法。

---

## 低功耗硬件方案

CI13XX系列芯片支持低功耗方案的硬件方案是采用效率更高的DCDC电源替换芯片内部的LDO电源，DCDC电源效率可以达到95%以上，而LDO的效率只有30%，因此采用DCDC电源可以有效降低功耗。DCDC电源采用5v输入，1.1v输出接芯片的VDD11管脚输入。
低功耗硬件参考图如下：

![CI13XX低功耗方案说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/img/CI13XX%E4%BD%8E%E5%8A%9F%E8%80%97%E6%96%B9%E6%A1%88%E8%AF%B4%E6%98%8E-1.png)

图1 1.1v外部DCDC供电

## 低功耗软件方案

#### CI13XX系列芯片采用DCDC电源降低功耗，软件上需要配合关闭内部1.1v的LDO。具体配置方法如下所示：

```
#define USE_INNER_LDO3                      0
```

将SDK中的宏USE\_INNER\_LDO3配置为0, 即可关闭内部1.1V的LDO。注意，如果有修改，修改后需要清理并重新编译工程。

#### CI13XX系列芯片低功耗方案还包括降低系统主频，具体配置方法为：

```
define MAIN_FREQUENCY          180000000    //主频率，单位为赫兹(Hz)
```

可以适当减小系统主频的宏定义值。主频率越低，功耗越小；但系统主频率太低了会影响识别效果，比如识别响应速度变慢，识别率降低等。不同的识别模型的影响也不一样，所以需要根据具体项目来实际测试看能降到多少。注意，如果有修改，修改后需要清理并重新编译工程。