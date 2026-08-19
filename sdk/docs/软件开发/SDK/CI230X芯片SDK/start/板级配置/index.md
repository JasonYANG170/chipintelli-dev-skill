<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/%E6%9D%BF%E7%BA%A7%E9%85%8D%E7%BD%AE/ -->

# 板级配置使用方法

---

## 1. 概述

本文为C230X 语音SDK 板级适配文档，旨在介绍当开发人员使用为非SDK标准板时，如何修改SDK，使其适配新的板子。

---

## 2. 使用说明

板级配置代码路径：语音SDK\driver\boards

### 2.1. 代码结构

| 源文件 | 说明 |
| --- | --- |
| board\_default.c | 板级配置弱定义接口 |
| board.c board.h | 板级配置接口 |
| board\_port\_template.c | 板级配置示例 |
| CI-E05GT02S.c | IOT标准板E05GT02S配置源文件 |

### 2.2. 新板添加步骤

当开发者需要使用SDK没有对应配置源文件的新板时，需要按照如下步骤进行添加，若SDK中已有配置源文件，只需要按照步骤二关联当前板子即可。

#### 2.2.1. 步骤一：添加源文件

添加新板源文件，例如CI-E05GT02S.c，添加方法：复制board\_port\_template.c于SDK\driver\boards中，并将其重命名，并修改源文件中对应的接口中的内容，若需新增接口查看步骤三。

#### 2.2.2. 步骤二：关联新板源文件

在SDK\driver\boards下添加好新板的源文件之后，需要将新板源文件与当前SDK关联起来，关联方法：打开工程代码，修改user\_config.h中的BOARD\_PORT\_FILE宏定义，使新的板级配置源文件被关联。

```
#define BOARD_PORT_FILE           "CI-E05GT02S.c"    //板级配置源文件关联到CI-E05GT02S_V1.0.c
```

#### 2.2.3. 步骤三：新增接口添加说明

(1)在新添加的配置源文件中添加接口，图中以示例源文件为例。

![板级配置使用方法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/boards-1.png)
  

图2-1 新增板级配置接口步骤一

(2)在boards.h文件中添加函数声明，如下图所示。

![板级配置使用方法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/boards-2.png)
  

图2-2 新增板级配置接口步骤二

(3)在borads\_default.c中添加同名虚函数，如下图所示。

![板级配置使用方法](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/img/boards-3.png)
  

图2-3 新增板级配置接口步骤三