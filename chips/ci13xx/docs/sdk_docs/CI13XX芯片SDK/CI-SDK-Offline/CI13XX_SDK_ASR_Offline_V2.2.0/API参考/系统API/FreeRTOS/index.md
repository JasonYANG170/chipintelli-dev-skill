<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E7%B3%BB%E7%BB%9FAPI/FreeRTOS/ -->

# 实时操作系统(FreeRTOS)

---

## 1. 概述

FreeRTOS是一款著名的开源免费实时操作系统，目前已作为CI13XX系列芯片SDK的默认OS使用。其内核轻巧，稳定，接口丰富，市场占有率较高。CI13XX系列芯片集成了最新的FreeRTOSV10版本，相较以前的V8版本，新增了邮箱、数据缓冲流等新组件接口。

提示

更多FreeRTOS信息请查看 ☞[FreeRTOS官网](https://www.freertos.org/index.html) 获取帮助。

---

## 2. 使用说明

在使用FreeRTOS的APi函数时，需要包含FreeRTOS.h和对应的模块头文件，在CI13XX系列芯片SDK中应添加一级路径，如下所示：

```
#include "FreeRTOS.h"
#include "task.h"
```

FreeRTOSConfig.h文件内包含OS内核的一些配置项，这里针对具有开发需求配置做以说明：

## API 参考