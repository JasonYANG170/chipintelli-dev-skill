<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/components/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%A1%E6%81%AF%E8%A1%A8/ -->

# 命令词信息管理模块(command\_info)

---

## 1. 概述

用于读取命令词相关的信息。也就是访问保存在资源文件”[60000]{xxx}xxx.xls”中的信息。

---

## 2. 使用说明

### 2.1. 模块初始化

```
uint32_t cmd_info_init(
        uint32_t cmd_file_addr_in_flash, 
        uint32_t voice_patition_addr, 
        uint8_t model_group_id);
```

必须在调用本模块所有其他接口之前调用此接口初始化，可以参考SDK示例工程。

### 2.2. 查找命令词

这里提供三种查找命令词的方式：

* 通过字符串查找：

```
cmd_handle_t cmd_info_find_command_by_string(const char * cmd_string);
```

* 通过命令词ID查找：

```
cmd_handle_t cmd_info_find_command_by_id(uint16_t cmd_id);
```

* 通过命令词语义ID查找：

```
cmd_handle_t cmd_info_find_command_by_semantic_id(uint32_t semantic_id);
```

以上三个接口返回指向查找到的命令词的句柄，用于本模块的其他接口获取命令词相关的信息。

### 2.3. 获取命令词信息

* 获取命令词字符串：

```
char * cmd_info_get_command_string(cmd_handle_t cmd_handle);
```

* 获取命令词ID：

```
uint16_t cmd_info_get_command_id(cmd_handle_t cmd_handle);
```

* 获取命令词语义ID：

```
uint32_t cmd_info_get_semantic_id(cmd_handle_t cmd_handle);
```

* 获取命令词最小识别置信度：

```
uint8_t cmd_info_get_cmd_score(cmd_handle_t cmd_handle);
```

此接口一般被语音识别模块调用，用户一般用不到。

提示

置信度:可以用于调整命令词的识别灵敏度，数值越小越容易被识别，但也越容易被误识别。

* 查询命令词是否特殊词：

```
uint32_t cmd_info_is_special_word(cmd_handle_t cmd_handle);
```

特殊词主要用于某些短命令词是其他长命令词的子字符串时，会影响长命令词的识别效果。比如，有两个命令词: “加热”、”加热两分钟”，当用户说”加热两分钟”的时候，肯定会先识别到前面的”加热”，造成误识别。为了解决这个问题，我们可以把”加热”设置为特殊词，并设置一个等待时间。语音模块在识别到”加热”后，不会立即输出结果，而是等待设定的时间。如果在等待时间内，又识别到了更长的命令词，就把之前的特殊词忽略掉。只有超过设定的等待时间都没有识别到其他命令词，才把该特殊词作为有效结果输出。

* 查询特殊词的等待时间：

```
int32_t cmd_info_get_special_wait_count(cmd_handle_t cmd_handle);
```

* 查询命令词是否唤醒词：

```
uint32_t cmd_info_is_wakeup_word(cmd_handle_t cmd_handle);
```

* 查询命令词是否组合词：

```
uint32_t cmd_info_is_combo_word(cmd_handle_t cmd_handle);
```

组合词的功能是可以和唤醒词连起来说，用户可以一次性说出唤醒词和命令词，减少等待。比如，智能管家的打开空调交互流程为：

```
用户：      智能管家
语音模块：  我在
用户：      打开空调
语音模块：  好的，打开空调
```

如果设置”打开空调”为组合词，交互流程可以是：

```
用户：      智能管家打开空调
语音模块：  好的，打开空调
```

注意

当前SDK还未实现此功能。

* 查询命令词关联的提示音的选择类型：

```
voice_select_type_t get_voice_select_type(cmd_handle_t cmd_handle);
```

只对关联了多个选择提示音的命令词有用。结果只有两个可能：”随机选择”,”用户选择”。

### 2.4. 多模型分组切换

CI112X SDK增加了模型分组的概念，一个分组可以是一个应用场景，也可以是一种识别模式，对应一套语音资源（声学模型，语言模型，命令词表，提示音），也就是命令词信息文件《[60000]{xxx}xxx.xls》中的一个工作表（sheet）。比如两个场景”英文识别”和”中文识别”，这中文和英文所需要的资源是不一样的，所以场景切换时，要切换响应的资源。
切换接口为:

```
uint32_t cmd_info_change_cur_model_group(uint8_t model_group_id);
```