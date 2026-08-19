<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/%E6%9D%BF%E7%BA%A7%E9%85%8D%E7%BD%AE%E8%AF%B4%E6%98%8E/ -->

# 板级配置使用说明

---

## 1. 概述

本文为CI13LC系列芯片 SDK 板级适配文档，旨在介绍当开发人员使用为非SDK标准板时，如何修改SDK，使其适配新的板子。

---

## 2. 使用说明

板级配置代码路径：SDK\driver\boards

### 2.1. 代码结构

| 源文件 | 说明 |
| --- | --- |
| board.c board.h | 板级配置接口 |
| CI-F1316XGS0XJ-V10.h | CI标准板F1316XGS0XJ配置头文件 |
| CI-F1324XGS0XJ-V10.h | CI标准板F1324XGS0XJ配置头文件 |

### 2.2. 新板添加步骤

当开发者需要使用SDK没有对应配置头文件的新板时，需要按照如下步骤进行添加，若SDK中已有配置源文件，只需要按照步骤二关联当前板子即可。

#### 2.2.1. 步骤一：添加源文件

添加新板头文件，例如CI-xxx.h，包含16系列芯片，添加方法：复制CI-F1316XGS0XJ-V10.h于SDK\driver\boards中，并将其重命名为CI-xxx.h，并修改源文件中对应的接口中的内容，若需新增接口查看步骤三。

#### 2.2.2. 步骤二：关联新板源文件

在SDK\driver\boards下添加好新板的源文件之后，需要将新板源文件与当前SDK关联起来，关联方法：打开工程代码，修改user\_config.h中的BOARD\_PORT\_FILE宏定义，且修改板子芯片型号，使新的板级配置源文件被关联。

```
#define CI_CHIP_TYPE              13162         //修改板子的芯片型号
#define BOARD_PORT_FILE           "CI-xxx.h"    //板级配置源文件关联到CI-xxx.h
```

#### 2.2.3. 步骤三：新增接口添加说明

(1)在board.c文件中添加接口，图中以示例源文件为例。

![板级配置使用方法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/img/boards-1.png)
  

图2-1 新增板级配置接口步骤一

(2)在boards.h文件中添加函数声明，如下图所示。

![板级配置使用方法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/img/boards-2.png)
  

图2-2 新增板级配置接口步骤二