<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8E1.12.16%E8%BF%81%E7%A7%BB%E5%88%B02.0.10/ -->

# 从1.12.16迁移到2.0.10

CI130X\_SDK从1.11.7迁移到1.12.6, 先把工程目录从旧SDK中 “CI13XX\_SDK\_ASR\_Offline\_V1.12.16\projects\xxx” 直接拷贝到新的SDK目录 “CI13XX\_SDK\_ASR\_Offline\_V2.0.10\projects\”。然后还需要做下文所描述的修改。

## Offline\_asr\_sample工程迁移指南

### 脚本修改和同步

* 工程管理文件修改  
  文件路径：“CI130X\_SDK\projects\\*\project\_files\source\_file.prj”   
  添加内容：

  ```
  //在文件最前面添加以下三行
  define-macro: ASR_CODE_VERSION=0
  define-macro: CI_NN_V2_EN=0
  build-config: USE_MORE_WORDS_LIBRARY=0
  ```
* makefile更新   
  文件路径：“CI13XX\_SDK\_ASR\_Offline\_V2.0.10\projects\\*\project\_files\makefile”   
  同步方式：直接拷贝新版本SDK中示例工程下的文件覆盖旧版本文件。

### 代码逻辑修改

* system\_msg\_deal.c
  文件路径：“CI130X\_SDK\projects\\*\system\_msg\_deal.c”
  同步方式：直接拷贝新SDK中示例工程下的system\_msg\_deal.c覆盖。
* system\_msg\_deal.h
  文件路径：“CI130X\_SDK\projects\\*\system\_msg\_deal.h”
  同步方式：直接拷贝新SDK中示例工程下的system\_msg\_deal.h覆盖。

## 自学习工程迁移指南

### 脚本修改和同步

* 工程管理文件修改  
  文件路径：“CI130X\_SDK\projects\\*\project\_files\source\_file.prj”   
  添加内容：

  ```
  //在文件最前面添加以下两行
  define-macro: ASR_CODE_VERSION=0
  define-macro: CI_NN_V2_EN=0
  ```
* makefile更新  
  文件路径：“CI13XX\_SDK\_ASR\_Offline\_V2.0.10\projects\\*\project\_files\makefile”   
  同步方式：直接拷贝新版本SDK中示例工程下的文件覆盖旧版本文件。

### 代码逻辑修改

* 自学习应用示例改进  
  文件路径：“CI130X\_SDK\projects\cwsl\_sample\cwsl\_app\_sample1.c”
  同步方式：同步宏 USE\_AEC\_MODULE 管控的部分。
* system\_msg\_deal.c
  文件路径：“CI130X\_SDK\projects\\*\system\_msg\_deal.c”
  同步方式：直接拷贝新SDK中示例工程下的system\_msg\_deal.c覆盖。
* system\_msg\_deal.h
  文件路径：“CI130X\_SDK\projects\\*\system\_msg\_deal.h”
  同步方式：直接拷贝新SDK中示例工程下的system\_msg\_deal.h覆盖。

## One Shot工程迁移指南

* 工程管理文件修改  
  文件路径：“CI130X\_SDK\projects\\*\project\_files\source\_file.prj”   
  添加内容：

  ```
  //在文件最前面添加以下两行
  define-macro: ASR_CODE_VERSION=0
  define-macro: CI_NN_V2_EN=0
  ```
* makefile更新   
  文件路径：“CI13XX\_SDK\_ASR\_Offline\_V2.0.10\projects\\*\project\_files\makefile”   
  同步方式：直接拷贝新SDK中示例工程下的makefile覆盖。

### 代码逻辑修改

* system\_msg\_deal.c
  文件路径：“CI130X\_SDK\projects\\*\system\_msg\_deal.c”
  同步方式：直接拷贝新SDK中示例工程下的system\_msg\_deal.c覆盖。
* system\_msg\_deal.h
  文件路径：“CI130X\_SDK\projects\\*\system\_msg\_deal.h”
  同步方式：直接拷贝新SDK中示例工程下的system\_msg\_deal.h覆盖。
* main.c
  文件路径：“CI130X\_SDK\projects\\*\main.c”
  修改内容：

  ```
  //删除下面这一行
  ciss_set(CI_SS_AEC_MUTE_STATE,CI_SS_AEC_MUTE_ON);
  ```

## 配置宏默认值修改

* ADAPTIVE\_THRESHOLD   

  ```
  //迁移前
  #define ADAPTIVE_THRESHOLD                  1   //ASR 自适应阈值    =1 开启  =0 关闭
  ```

  ```
  //迁移后
  #define ADAPTIVE_THRESHOLD                  0   //ASR 自适应阈值    =1 开启  =0 关闭
  ```

## 废弃API接口

* pause\_voice\_in
* resume\_voice\_in