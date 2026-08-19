<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/CI110X_SDK%E5%86%85%E5%AD%98%E5%88%86%E9%85%8D%28icf%29%E8%B0%83%E6%95%B4%E6%8C%87%E5%8D%97/ -->

# CI110X系列芯片SDK内存分配(icf)调整指南

---

## 1. CI1102

### 1.1. 内存区域说明

* *define symbol ROM\_SIZE*：代码和常量占用的空间
  异常表现：编译报错
* *define symbol FHEAP\_SIZE*：FreeRTOS的堆空间，任务堆栈，操作系统资源，很多模块使用到的资源，命令词excel 表格加大后，可能该区域会不够。
  异常表现：运行中可能出现错误警告
* *define symbol RAM\_SIZE*：全局变量，全局数组
  异常表现：编译报错
* *define symbol ASR\_USED\_SIZE*：识别解码相关，词条占用该部分空间，词条过多，该部分在程序运行中可能不够。
  异常表现：运行中无法识别，【请在人声干扰噪音下测试1小时，如不出现无法识别情况，则表示正常】
* *define symbol STACK\_SIZE*：中断栈，不建议修改
  异常表现：无法预测，所以强烈不建议修改。

### 1.2. 调整基本规则

* 在调整过程中，始终要保证 **ROM\_SIZE+FHEAP\_SIZE+RAM\_SIZE+ASR\_USED\_SIZE = 528\*1024**。
* ***RAM\_SIZE*** 不能小于16\*1024。
* ***ROM\_SIZE*** ，***RAM\_SIZE*** 刚够就好，剩余的空间可以分给 ***ASR\_USED\_SIZE*** 。

### 1.3. 代码空间(ROM\_SIZE)不够

#### 1.3.1. 现象

* 编译时，在从0x1ffb0000开始的区域中分配不到内存：

Error[Lp011]: section placement failed
unable to allocate space for sections/blocks with a total estimated minimum size of 0x1’d4b8 bytes (max align 0x4) in **<[0x1ffb‘0000-0x1ffb‘9fff]>** (total uncommitted space 0x9f00).

#### 1.3.2. 调整策略

增大 ***ROM\_SIZE***, 减小 ***ASR\_USED\_SIZE***，不要调整太多，刚好够就行，尽量把空间留给 ***ASR\_USED\_SIZE***。

### 1.4. 全局变量区(RAM\_SIZE)不够

#### 1.4.1. 现象

* 编译时，在从0x20020000后面的区域中分配不到内存：
  Error[Lp011]: section placement failed
  unable to allocate space for sections/blocks with a total estimated minimum size of 0x5298 bytes (max align 0x8) in <[0x2002’f000-0x2003‘3fff]> (total uncommitted space 0x5000).

#### 1.4.2. 调整策略

增大 ***RAM\_SIZE***, 减小 ***ASR\_USED\_SIZE***，不要调整太多，刚好够就行，尽量把空间留给 ***ASR\_USED\_SIZE***。

### 1.5. FreeRTOS 堆不够

#### 1.5.1. 现象

* 运行时，使用接口 ***pvPortMalloc*** 申请内存失败。
* 运行时，log日志：**“asr init fail”**。
* 运行时，log日志：**“not enough memory”**。
* 运行时，log日志：**“alloc error”**。
* 运行时，log日志：**“malloc err!”**。
* 运行时，log日志：**“malloc audio source data error”**。

#### 1.5.2. 调整策略

* 先调整 ***ROM\_SIZE***，调整到刚好够就行，剩余的加到 ***FHEAP\_SIZE***。
* 再调整 ***RAM\_SIZE***，调整到刚好够就行，剩余的加到 ***FHEAP\_SIZE***。
* 如果还不够，可以试着从 ***ASR\_USED\_SIZE*** 调整部分空间到 ***FHEAP\_SIZE***。

### 1.6. iar堆不够

#### 1.6.1. 现象

* 运行时，使用接口 ***malloc*** 申请内存失败。
* 运行时，log日志：**“decoder\_manage.c,L: xxx”**，然后卡住不动或重启。

#### 1.6.2. 调整策略

* 先调整 ***ROM\_SIZE***，调整到刚好够就行，剩余的加到 ***ASR\_USED\_SIZE***。
* 再调整 ***RAM\_SIZE***，调整到刚好够就行，剩余的加到 ***ASR\_USED\_SIZE***。
* 如果还不够，可以试着从 ***FHEAP\_SIZE*** 调整部分空间到 ***ASR\_USED\_SIZE***。

---

## 2. CI1103

### 2.1. 内存区域说明

* ***define symbol ROM\_SIZE***：代码和常量占用的空间，通常增加代码会导致对该区域内存大小需求增加，此区域size建议以sdk发布时为准。
  异常表现：编译报错
* ***define symbol DECODER\_CODE\_RW\_SIZE***：对应语音识别系统代码段和全局变量内存需要，此区域size建议以sdk发布时为准。
  异常表现：编译报错
* ***define symbol RAM\_SIZE***：全局变量，全局数组，对应用户数据区，通常增加全局变量会导致对该区域内存大小需求增加。
  异常表现：编译报错
* ***define symbol DECODER\_USED\_SIZE***：对应sdk中语音识别、aec、denoise等算法申请数据区大小，此区域通常情况是不需要变更的，但是注意在裁剪功能时可以对此区域进行减小，例如不需要使用aec、denoise、deforming、doa等算法时，且其他用户代码对内存要求较多时可以适量减小此区域大小。
  异常表现：详见6.1
* ***define symbol RODATA\_SIZE***：为只读数据段，通常增加const变量会导致对该区域内存大小需求增加。
  异常表现：编译报错
* ***define symbol FHEAP\_SIZE***：用户堆区，通常用户使用pvPortMalloc函数申请动态内存即从此区域申请，同时freertos中的相关接口也从该区域申请内存，例如创建任务、信号量、队列、事件组等，若用户程序增加了以上内容则需要适量增加该区域大小。
  异常表现：运行中可能出现错误警告
* ***define symbol NO\_CACHE\_PSRAM\_SIZE***：
  异常表现：仅用于语音识别系统内dnn输出使用，此区域size以sdk发布时为准不可修改。
* ***define symbol STACK\_SIZE***：中断栈，不建议修改
  异常表现：无法预测，所以强烈不建议修改。

### 2.2. 调整基本规则

在调整过程中，始终要保证以下规则：

#### 2.2.1. 规则（一）

* **ROM\_SIZE+DECODER\_CODE\_RW\_SIZE+DECODER\_USED\_SIZE+RAM\_SIZE+RODATA\_SIZE+FHEAP\_SIZE+NO\_CACHE\_PSRAM\_SIZE = （2 \* 1024 \* 1024 + 1 \* 512 \* 1024）**

#### 2.2.2. 规则（二）

* **DECODER\_CODE\_RW\_SIZE+DECODER\_USED\_SIZE+RAM\_SIZE = （512 \* 1024）**

#### 2.2.3. 规则（三）

* **ROM\_SIZE**，**RAM\_SIZE** 刚够就好。

### 2.3. 代码空间(ROM\_SIZE)不够

#### 2.3.1. 现象

编译时，在 0x1ff30000 ~ 0x1ffaffff 之间的区域中分配不到内存：

* Error[Lp011]: section placement failed  
  unable to allocate space for sections/blocks with a total estimated minimum size of 0x2’ac8b bytes (max align 0x4) in <[0x1ff8‘9000-0x1ffa’ffff]> (total uncommitted space 0x2‘6ec4).

计算方法：

* 0x2ac8b - 0x26ec4 = 0x3C00 ，0x3C00转换为十进制是15360， 15360/1024 = 15，能整除，意味着大概还需要增加16K（1K为1024）的ROM\_SIZE空间，根据步骤2的规则（一），可以相应减少FHEAP\_SIZE的空间大小。

例如这样修改：

```
define symbol ROM_SIZE                       = 1024*(256+16);
define symbol FHEAP_SIZE                     = 1024*(1506-16);
```

修改过后，再出现下列错误，说明ROM空间还不够，再依次增加1K，FHEAP\_SIZE减少1K，直到编译不报错。

* Error[Lp015]: section placement failure: overcommitted content in [0x1ff8‘5000-0x1ffa’ffff]

#### 2.3.2. 调整策略

* 增大 **ROM\_SIZE**，减小 **FHEAP\_SIZE**，不要调整太多，刚好够就行，以免用用户堆区不够。
* 依照步骤2的规则（一）调整。

### 2.4. 语音识别系统代码段和全局变量区(DECODER\_CODE\_RW\_SIZE)不够

#### 2.4.1. 现象

编译时，在 0x1ffb0000 后的区域中分配不到内存：

* Error[Lp011]: section placement failed  
  unable to allocate space for sections/blocks with a total estimated minimum size of 0x4138 bytes (max align 0x4) in <[0x1ffb‘0000-0x1ffb‘3fff]> (total uncommitted space 0x3f00).

计算方法：

* 0x4138 - 0x3f00 = 0x238 ，0x238转换为十进制是568，568/1034 = 0，意味着大概还需要增加1K的DECODER\_CODE\_RW\_SIZE空间，根据步骤2的规则（二），可以相应减少DECODER\_USED\_SIZE的空间大小。

例如这样修改：

```
define symbol DECODER_CODE_RW_SIZE           = 1024*(16+1);
define symbol DECODER_USED_SIZE              = 1024*(419-3-2-6-1);
```

#### 2.4.2. 调整策略

* 增大 **DECODER\_CODE\_RW\_SIZE**，减小 **DECODER\_USED\_SIZE**，不要调整太多，刚好够就行，尽量把空间留给 **DECODER\_USED\_SIZE**。
* 依照步骤2的规则（二）调整。

### 2.5. 全局变量区(RAM\_SIZE)不够

#### 2.5.1. 现象

编译时，在 0x1ffb4c00 后面的区域中分配不到内存：

* Error[Lp011]: section placement failed  
  unable to allocate space for sections/blocks with a total estimated minimum size of 0x1‘53f8 bytes (max align 0x8) in <[0x1ffb‘4c00-0x1ffc‘87ff]> (total uncommitted space 0x1‘3c00).

计算方法：

* 0x153f8 - 0x13c00 = 0x1400 ，0x1400转换为十进制是5120，5120/1024 = 5，能整除，意味着大概还需要增加 6K 的 RAM\_SIZE 空间，根据步骤2的规则（二），可以相应减少 DECODER\_USED\_SIZE 的空间大小。

例如这样修改：

```
define symbol RAM_SIZE                       = 1024*(74+3+2+6);
define symbol DECODER_USED_SIZE              = 1024*(419-3-2-6);
```

#### 2.5.2. 调整策略

* 增大 **RAM\_SIZE**， 减小 **DECODER\_USED\_SIZE**，不要调整太多，刚好够就行，尽量把空间留给 **DECODER\_USED\_SIZE**。
* 依照步骤2的规则（二）调整。

### 2.6. asr堆(DECODER\_USED\_SIZE)不够

#### 2.6.1. 现象

* 运行时，使用接口 **malloc** 申请内存失败。
* 运行时，log日志：**“decoder\_manage.c,L: xxx”**，然后卡住不动或重启。

#### 2.6.2. 调整策略

* 先调整 **DECODER\_CODE\_RW\_SIZE**，调整到刚好够就行，剩余的加到 **DECODER\_USED\_SIZE**。
* 再调整 **RAM\_SIZE**，调整到刚好够就行，剩余的加到 **DECODER\_USED\_SIZE**。
* 依照步骤2的规则（二）调整。

### 2.7. 只读数据段(RODATA\_SIZE)不够

#### 2.7.1. 现象

编译时，在 0x20030000 后面的区域中分配不到内存：

* Error[Lp011]: section placement failed  
  unable to allocate space for sections/blocks with a total estimated minimum size of 0x3194 bytes (max align 0x4) in <[0x2003‘0000-0x2003‘27ff]> (total uncommitted space 0x2800).

计算方法：

* 0x3194 - 0x2800 = 0x994 ，0x1400转换为十进制是2452，5120/1024 = 2.39，不能整除，意味着大概还需要增加 3K 的 RODATA\_SIZE 空间，根据步骤2的规则（一），可以相应减少 FHEAP\_SIZE 的空间大小。

例如这样修改：

```
define symbol RODATA_SIZE                    = 1024*(10+3);
define symbol FHEAP_SIZE                     = 1024*(1506+20-3);
```

#### 2.7.2. 调整策略

* 增大 **RODATA\_SIZE**，减小 **FHEAP\_SIZE**，不要调整太多，刚好够就行。
* 依照步骤2的规则（一）调整。

### 2.8. FreeRTOS 堆（FHEAP\_SIZE）不够

#### 2.8.1. 现象

* 运行时，使用接口 ***pvPortMalloc*** 申请内存失败。
* 运行时，log日志：**“asr init fail”**。
* 运行时，log日志：**“not enough memory”**。
* 运行时，log日志：**“alloc error”**。
* 运行时，log日志：**“malloc err!”**。
* 运行时，log日志：**“malloc audio source data error”**。

#### 2.8.2. 调整策略

* 先调整 **ROM\_SIZE**，调整到刚好够就行，剩余的加到 **FHEAP\_SIZE**。
* 再调整 **RODATA\_SIZE**，调整到刚好够就行，剩余的加到 **FHEAP\_SIZE**。
* 依照步骤2的规则（一），规则（二）调整。