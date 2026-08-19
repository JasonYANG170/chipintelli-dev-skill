<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E7%A6%BB%E7%BA%BF%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# 离线命令词自学习使用说明

## 1 内容概述

本文档主要对CI13LC系列芯片的单麦离线SDK中离线命令词自学习功能进行说明。内容包括离线命令词自学习的概念、场景、使用说明以及注意事项。

## 2 离线命令词自学习基本概念

在非联网状态，用户通过语音对话的方式，更改默认的命令词；该种更改方式满足终端用户的个性化自定义的需求，可以给客户带来更好的用户体验。

## 3 离线命令词自学习适用场景

本方案适用于各种语音控制相关的智能家电，家居等产品，实现用户离线自定义命令词。

## 4 离线命令词自学习使用说明

离线命令词自学习底层代码在CI13LC\_SDK\components\ci\_cwsl下，应用层代码在CI13LC\_SDK\projects\cwsl\_sample\src下，见cwsl\_app\_sample1.c和cwsl\_app\_sample1.h，相关函数都有详细的注释，本文列出部分重要函数。

```
/* 
 * @brief 命令词自学习消息处理函数
 * @param asr_msg ASR识别结果消息
 * @param cmd_handle 命令词handle
 * @param cmd_id 命令词ID
 * @retval 1 数据有效,消息已处理
 * @retval 0 数据无效,消息未处理
 */
uint32_t cwsl_app_process_asr_msg(sys_msg_asr_data_t *asr_msg, cmd_handle_t *cmd_handle, uint16_t cmd_id)
```

```
/* 
 * @brief 模块初始化事件响应，命令词自学习参数初始函数，初始会调用该函数
 * @param cwsl_init_parameter 参数指针
 * @retval 1 必须返回可学习的模板数量
 */
int on_cwsl_init(cwsl_init_parameter_t *cwsl_init_parameter)
```

```
/* 
 * @brief 学习开始事件响应，每次学习开始调用该函数
 * @param cmd_id 命令词ID
 * @param group_id 组ID
 * @param word_type 学习的类型，CMD_WORD 命令词 ；WAKEUP_WORD唤醒词
 * @retval 0
 */
int on_cwsl_reg_start(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type)
```

```
/* 
 * @brief 学习停止事件响应
 * @param 无
 * @retval 0
 */
int on_cwsl_reg_abort()
```

```
/* 
 * @brief 录制开始事件响应
 * @param 无
 * @retval 0
 */
int on_cwsl_record_start()
```

```
/* 
 * @brief 录制结束事件响应
 * @param times 当前学习的次数
 * @param result 当前学习结果的状态：学习成功，学习失败,...
 * @retval 0
 */
int on_cwsl_record_end(int times, cwsl_reg_result_t result)
```

```
/* 
 * @brief 删除模板成功事件响应
 * @retval 0
 */
int on_cwsl_delete_successed()
```

```
/* 
 * @brief 识别成功事件响应
 * @param cmd_id 命令词ID 
 * @param distance 参数未使用，当前无效
 * @retval 0
 */
int on_cwsl_rgz_successed(uint16_t cmd_id, uint32_t distance)
```

```
/* 
 * @brief 开始启动学习
 * @param word_type 学习的命令词类型
 * @retval 0
 */
void cwsl_app_reg_word(cwsl_word_type_t word_type)
```

```
/* 
 * @brief 删除指定类型模板
 * @param cmd_id 指定要删除的命令词ID, 传入-1为通配符，忽略此项
 * @param group_id 指定要删除的命令词分组号, 传入-1为通配符，忽略此项
 * @param word_type 指定要删除的命令词类型，传入传入-1为通配符，忽略此项
 * @retval 无
 */
void cwsl_app_delete_word(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type)
```

### 4.1 离线命令词自学习相关宏定义

在CI13LC\_SDK\projects\cwsl\_sample\src下，打开user\_config.h文件。

```
#define USE_CWSL                   1 //1打开离线命令词自学习，0：关闭离线命令词自学习
#define CICWSL_TOTAL_TEMPLATE      10 //离线命令词自学习可存储模板数量，默认10个
```

### 4.2 离线命令词自学习唤醒词和命令词个数配置

在CI13LC\_SDK\projects\cwsl\_sample\src下，打开user\_config.h文件，设置可学习的唤醒词数量。

```
#define CWSL_WAKEUP_NUMBER         2 // 可学习的唤醒词数量
#define WAKE_UP_ID                 1 // 学习的唤醒词对应的命令词ID
```

在CI13LC\_SDK\projects\cwsl\_sample\src下，打开cwsl\_app\_sample1.c文件，设置可学习的命令词个数。

```
#define CWSL_CMD_NUMBER            ((sizeof(reg_cmd_list) / sizeof(reg_cmd_list[0]))) // 可学习的命令词数量
```

### 4.3 离线命令词自学习命令词学习播报音配置

**配置reg\_cmd\_list结构体数组**

在CI13LC\_SDK\projects\cwsl\_sample\src下，打开cwsl\_app\_sample1.c文件。

```
//左边一列表示的是要学习的命令词ID,右边一列表示需要播报的ID
const cwsl_reg_asr_struct_t reg_cmd_list[]=
{ //命令词ID       //学习提示播报音ID
    {2,             1001},
    {3,             1002},
    {4,             1003},
    {5,             1004},
    {6,             1005},
    {7,             1006},        
};
```

**配置命令词列表**

在CI13LC\_SDK\projects\cwsl\_sample\firmware\user\_file\cmd\_info下，打开[60000]{xxxx}cmd\_info.xls文件。

**说明**

下表中命令词一列目的是提示该条播报是哪个词，填写方式随意，无需按下表中格式填写。

| 命令词 | 命令词ID | 命令词语义ID | 置信度 | 唤醒词 | 组合词 | 期望词 | 不期望词 | 特殊词计数 | 播报音类型 | 播报音0ID | 播报音1ID |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| <请说打开空调的学习指令> | 1001 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 1001 |  |
| <请说关闭空调的学习指令> | 1002 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 1002 |  |
| <请说增大风速的学习指令> | 1003 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 1003 |  |
| <请说减小风速的学习指令> | 1004 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 1004 |  |
| <请说升高一度的学习指令> | 1005 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 1005 |  |
| <请说降低一度的学习指令> | 1006 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 1006 |  |

**增加对应的播报音**

添加到CI13LC\_SDK\projects\cwsl\_sample\firmware\voice\src文件夹下。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E7%A6%BB%E7%BA%BF%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-1.png)

### 4.4 其他参数配置

在CI13LC\_SDK\projects\cwsl\_sample\src下，打开user\_config.h文件。

```
#define CWSL_REG_TIMES              (1)                        // 学习时 每个词需说几遍，默认 1 遍即可,支持1、2遍,最大支持 2 遍;
#define CWSL_WAKEUP_THRESHOLD       (37)                       // 学习的唤醒词阈值门限，越小越灵敏，默认 37, 最小可配置到 32;
#define CWSL_CMD_THRESHOLD          (35)                       // 学习的命令词阈值门限，越小越灵敏，默认 35，最小可配置到 30；
```

在CI13LC\_SDK\projects\cwsl\_sample\src下，打开cwsl\_app\_sample1.c文件。

```
#define CWSL_TPL_MINWORD            (2)                        // 自学习模板的最小字数，默认 2 个字，可设置模板最小 2 、3 个字;
#define MAX_LEARN_REPEAT_NUMBER     ( CWSL_REG_TIMES + 2 )     // 学习时，重复的总次数, 建议范围 CWSL_REG_TIMES + 1、CWSL_REG_TIMES + 2、CWSL_REG_TIMES + 3
```

### 4.5 离线命令词自学习默认命令词

目前命令词自学习的demo针对唤醒词/命令词做了一套标准流程。如果用户需要使用命令词自学习功能，需要更新语言模型（ASR）、命令词表格和播报音。具体方法如下：

**语言模型（ASR）新增命令词列表：**

学习下一个

学习唤醒词

学习命令词

重新学习

退出学习

我要删除

删除唤醒词

删除命令词

退出删除

全部删除

**说明**

语言模型（ASR）目前只支持中文模型（其中低误识模型除外）。

**命令词表格增加的命令词：**

| 学习下一个 | 199 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 99 |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 学习唤醒词 | 200 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 200 |  |
| 学习命令词 | 201 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 201 |  |
| 重新学习 | 202 | 0x00 | 41 | NO | NO | NO | NO | 0 | 自定义 | 200 | 201 |
| 退出学习 | 203 | 0x00 | 41 | NO | NO | NO | NO | 0 | 自定义 | 203 |  |
| 我要删除 | 204 | 0x00 | 41 | NO | NO | NO | NO | 0 | 自定义 | 204 |  |
| 删除唤醒词 | 205 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 205 |  |
| 删除命令词 | 206 | 0x00 | 39 | NO | NO | NO | NO | 0 | 自定义 | 206 |  |
| 退出删除 | 207 | 0x00 | 41 | NO | NO | NO | NO | 0 | 自定义 | 207 |  |
| 全部删除 | 208 | 0x00 | 41 | NO | NO | NO | NO | 0 | 自定义 | 208 |  |

如果对CI13LC\_SDK\projects\cwsl\_sample\firmware\user\_file\cmd\_info下的命令词列表中\*\*命令词ID\*\*进行修改，还需要将CI13LC\_SDK\projects\cwsl\_sample\src\cwsl\_app\_sample1.h文件中枚举cicwsl\_func\_index进行修改，枚举如下：

```
typedef enum
{
    CWSL_REGISTRATION_WAKE          = 200,      ///< 命令词：学习唤醒词
    CWSL_REGISTRATION_CMD           = 201,      ///< 命令词：学习命令词
    CWSL_REGISTER_AGAIN             = 202,      ///< 命令词：重新学习
    CWSL_EXIT_REGISTRATION          = 203,      ///< 命令词：退出学习
    CWSL_DELETE_FUNC                = 204,      ///< 命令词：我要删除
    CWSL_DELETE_WAKE                = 205,      ///< 命令词：删除唤醒词
    CWSL_DELETE_CMD                 = 206,      ///< 命令词：删除命令词
    CWSL_EXIT_DELETE                = 207,      ///< 命令词：退出删除模式
    CWSL_DELETE_ALL                 = 208,      ///< 命令词：全部删除

    CWSL_REGISTRATION_NEXT          = 199,      ///< 播报：学习下一个
  //CWSL_DATA_ENTERY_SUCCESSFUL     = 209,      ///< 播报：录入成功
    CWSL_DATA_ENTERY_FAILED         = 210,      ///< 播报：学习失败
    CWSL_REGISTRATION_SUCCESSFUL    = 211,      ///< 播报：学习成功
    CWSL_TEMPLATE_FULL              = 212,      ///< 播报：学习模板超过上限
    CWSL_SPEAK_AGAIN                = 1007,     ///< 播报：请再说一次>
    CWSL_TOO_SHORT                  = 1008,     ///< 播报：语音长度不够，请再说一次>

    CWSL_DELETE_SUCCESSFUL          = 213,      ///< 播报：删除成功
    CWSL_REGISTRATION_ALL           = 217,      ///< 播报：学习完成
    CWSL_REG_FAILED                 = 218,      ///< 播报：学习失败
    CWSL_REG_FAILED_DEFAULT_CMD_CONFLICT = 1009, ///< 播报：与默认指令冲突，请换种说法
}cicwsl_func_index;
```

**需要增加播报音：**

![image-cwls](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E7%A6%BB%E7%BA%BF%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-2.png)

### 4.6 离线命令词自学习使用步骤

#### 4.6.1 学习功能

学习功能可设置一次学习和两次学习，功能逻辑如下。

**一次学习：**

唤醒词学习：系统唤醒状态下，用户通过念命令词“学习唤醒词”进入学习模式。根据播报语音提示，念一次自定义的唤醒词完成学习。学习完成后，语音播报“学习成功”。

命令词学习：系统唤醒状态下，用户通过念命令词“学习命令词”进入学习模式。根据播报语音提示，念一次自定义的命令词完成学习，系统会继续学习下一个命令词，直到学习完成。如果中途退出学习状态，下次再学习会接着上次退出时的位置继续学习。

**两次学习**：

唤醒词学习：系统唤醒状态下，用户通过念命令词“学习唤醒词”进入学习模式。根据播报语音提示，念第一遍自定义的唤醒词，学习成功后念第二遍，会和第一遍进行比对，如果相同则学习成功。如果不同会提示学习失败并重新学习该词。

命令词学习：系统唤醒状态下，用户通过念命令词“学习命令词”进入学习模式。根据播报语音提示，念第一遍自定义的命令词，学习成功后念第二遍，会和第一遍进行比对，如果相同则学习成功。如果不同会提示学习失败并重新学习该词。

#### 4.6.2 重新学习功能

**一次学习：**

当上一个词学习的不对或者被周围干扰人声误学习时，可以念命令词“重新学习”，系统会播报语音提示，重新学习上一个词。

**两次学习：**

当学习的第二遍与第一遍不同时，系统会自动重新学习该词，无需语音控制。

#### 4.6.3 退出学习功能

进入学习模式后，用户可以念命令词“退出学习”，即可退出学习模式。

#### 4.6.4 删除功能

用户通过念命令词“我要删除”进入删除模式，此时支持三种方式进行删除。方式一：全部删除，用户念该命令词时，会删除注册的所有唤醒词和命令词。方式二：删除唤醒词，用户念该命令词时，会删除注册的所有唤醒词。方式三：删除命令词，用户念该命令词时，会删除注册的所有命令词。

#### 4.6.5 退出删除功能

进入删除模式时，用户可以念命令词“退出删除”退出删除模式。

#### 4.6.6 学习下一个命令词功能

系统唤醒状态下，用户通过念命令词“学习命令词”进入学习模式。根据播报语音提示，中途可以念“学习下一个”，跳过不想学习的命令词，系统会继续学习下一个命令词，直到学习完成。如果中途退出学习状态，下次会从第1个未学习的命令词位置继续学习。

### 4.7 离线命令词自学习注意事项

**学习必须在安静环境下、正常语速、声音洪亮、距离1米内**。

**学习的词字数为2到5个**。

**语言模型（ASR）目前只支持中文模型（其中低误识模型除外），且模型节点数建议5000以下（例如下图开机打印中arcs 3584即节点数）**

![image-cwls](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E7%A6%BB%E7%BA%BF%E5%91%BD%E4%BB%A4%E8%AF%8D%E8%87%AA%E5%AD%A6%E4%B9%A0%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-3.jpg)

## 5 离线命令词自学习Q&A

Q1：如何进入学习模式？

A1：唤醒后（标准demo的唤醒词为“智能管家”），念命令词“学习唤醒词”或者“学习命令词”就可以进入学习模式。

Q2：如何退出学习模式？

A2：退出方式有两种，第一种用户主动退出，唤醒后，进入学习模式后，通过念命令词”退出学习“可以退出注册模式。第二种：每次学习成功后，系统自动退出学习模式，进入识别模式。

Q3：如果进入学习模式，念语音无任何反馈，怎么处理？

A3：原因一：可能是声音太小了，学习时提高了VAD的阈值，解决方式：保持和设备的距离在0.5米内，声音洪亮进行学习。原因二：系统退出唤醒状态了，解决方式：唤醒后重新进入学习模式就可解决。