<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/%E6%8F%90%E7%A4%BA%E9%9F%B3%E6%92%AD%E6%94%BE%E5%99%A8/ -->

# 提示音播放器(prompt\_player)

---

## 1. 概述

此模块建立在音频播放器之上，用于播放命令词对应的提示音。封装了从命令词查找提示音信息的过程，并提供了组合播报，选择播报，队列播报等功能。

---

## 2. 功能说明

### 2.1. 命令词内组合播报

由于音频文件都较大，占用较多flash空间，为了节省flash空间，我们可以把不同提示音中共有的词语提取出来，在播放的时候再通过多个音频连续播放的方式组合出所需的提示音。例如，SDK中的标准示例“智能管家”的提示音中，很多提示音都有一个相同的前缀”好的…”，”好的，打开空调”、”好的，二十五度”、”好的,打开台灯”。我们可以把”好的”提取出来，做成一个单独的音频文件。然后在命令词信息表中通过组合的方式关联到命令词。用户程序在调用播报的时候，只需要传入对应命令词ID或字符串就行，组合逻辑已经在内部实现。

支持的接口:

* prompt\_play\_by\_cmd\_handle
* prompt\_play\_by\_cmd\_id
* prompt\_play\_by\_semantic\_id
* prompt\_play\_by\_cmd\_string

#### 2.1.1 命令词内组合播报示例

* 假设音频内容如下：

![提示音播放器](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/img/%E6%8F%90%E7%A4%BA%E9%9F%B3%E6%92%AD%E6%94%BE%E5%99%A8%28prompt_player%29-1.png)

* 修改SDK目录（SDK\projects\sample\_xxx\firmware\user\_file\cmd\_info\）下的文件[60000]{智能管家}V2.xls，填写组合播报的音频ID，用加号连接（最多支持16个音频组合）。
* 例如，命令词 **“打开空调”** 的 **“播报音0ID”** 栏，填写 **4+5+6**

![提示音播放器](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/img/%E6%8F%90%E7%A4%BA%E9%9F%B3%E6%92%AD%E6%94%BE%E5%99%A8%28prompt_player%29-2.png)

* 代码中调用方式如下，播放的内容均为 **“好的，主人，已为您打开空调”** ，即将3个音频组合起来播报：

```
//可用命令词handle方式播报：句柄由识别结果结构体得到
prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,true);

//或命令词id方式播报：传参“打开空调”的命令词ID
prompt_play_by_cmd_id(2, -1, default_play_done_callback,true);

//或命令词语义id方式播报：传参“打开空调”的语义ID
prompt_play_by_semantic_id(0x2BC1943, -1, default_play_done_callback,true);

//或命令词字符串方式播报：传参“打开空调”的字符串
prompt_play_by_cmd_string("打开空调", -1, default_play_done_callback,true);
```

### 2.2. 选择播报

* 为了播报的多样性，避免每个命令词只有固定的一个提示音，提升产品的体验感。比如，对于唤醒词的响应可以是”您好”,或者是”我在”,也可以是”请讲”等等。每当识别到用户说出的唤醒词后，可以几个词中任意选取一个播报。
* 为了根据不同的环境情况选择不同的提示音播报。比如，用户说命令词”增大音量”，可以根据当前音量值得不同选择不同的播报。当前音量已经是最大音量，提示音可以是”好的，已最大音量”，如果当前音量还没有达到最大，则可以播报”好的，已增大音量”。

支持的接口:

* prompt\_play\_by\_cmd\_handle
* prompt\_play\_by\_cmd\_id
* prompt\_play\_by\_semantic\_id
* prompt\_play\_by\_cmd\_string

#### 2.2.1 选择播报示例

* 假设音频内容如下：

![提示音播放器](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/img/%E6%8F%90%E7%A4%BA%E9%9F%B3%E6%92%AD%E6%94%BE%E5%99%A8%28prompt_player%29-1.png)

* 修改SDK目录（SDK\projects\sample\_xxx\firmware\user\_file\cmd\_info\）下的文件 **[60000]{智能管家}V2.xls** ，填写选择播报的音频ID，从播报音1ID栏往后填写，最多可以填写127栏。

![提示音播放器](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/img/%E6%8F%90%E7%A4%BA%E9%9F%B3%E6%92%AD%E6%94%BE%E5%99%A8%28prompt_player%29-3.png)

* 代码中调用方式如下，第2个参数用于选择播报：

  ```
  //第2个参数-1，表示默认选择播报音0ID：播放内容“好的”
   prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,true);
   prompt_play_by_cmd_id(2, -1, default_play_done_callback,true);
   prompt_play_by_semantic_id(0x2BC1943, -1, default_play_done_callback,true);
   prompt_play_by_cmd_string("打开空调", -1, default_play_done_callback,true);

  //第2个参数0表示选择播报音0ID：播放内容“好的”
   prompt_play_by_cmd_handle(cmd_handle, 0, default_play_done_callback,true);
   prompt_play_by_cmd_id(2, 0, default_play_done_callback,true);
   prompt_play_by_semantic_id(0x2BC1943, 0, default_play_done_callback,true);
   prompt_play_by_cmd_string("打开空调", 0, default_play_done_callback,true);

  //第2个参数1表示选择播报音1ID：播放内容“主人”
   prompt_play_by_cmd_handle(cmd_handle, 1, default_play_done_callback,true);
   prompt_play_by_cmd_id(2, 1, default_play_done_callback,true);
   prompt_play_by_semantic_id(0x2BC1943, 1, default_play_done_callback,true);
   prompt_play_by_cmd_string("打开空调", 1, default_play_done_callback,true);

  //第2个参数2表示选择播报音2ID：播放内容“已为您打开空调”
   prompt_play_by_cmd_handle(cmd_handle, 2, default_play_done_callback,true);
   prompt_play_by_cmd_id(2, 2, default_play_done_callback,true);
   prompt_play_by_semantic_id(0x2BC1943, 2, default_play_done_callback,true);
   prompt_play_by_cmd_string("打开空调", 2, default_play_done_callback,true);
  ```

### 2.3. 多命令词组合播报

前面解释了”命令词内组合播报”和”选择播报”，两个一起用可以实现先组合再选择，也就是可以实现选择不同的组合方式播报。某些应用可能还需要更复杂的组合功能，组合不同的选择，简单的说就是先选择再组合。本模块提供把多个命令词的提示音组合到一起播报的功能，从而可以实现先组合，再选择，再组合的功能。

支持的接口:

* prompt\_play\_by\_multi\_cmd\_id

#### 2.3.1 多命令词组合播报示例

* 假设音频内容如下：

![提示音播放器](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/img/%E6%8F%90%E7%A4%BA%E9%9F%B3%E6%92%AD%E6%94%BE%E5%99%A8%28prompt_player%29-4.png)

* 需求假设是命令词“灯全开”、“内灯开”、“外灯开”。
* 修改SDK目录（SDK\projects\sample\_xxx\firmware\user\_file\cmd\_info\）下的文件 **[60000]{智能管家}V2.xls** ，填写播报的音频ID

![提示音播放器](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/components/img/%E6%8F%90%E7%A4%BA%E9%9F%B3%E6%92%AD%E6%94%BE%E5%99%A8%28prompt_player%29-5.png)

代码中调用方式如下：

```
prompt_play_info_t p_play_info_all[9] =
{
    //内灯
    {65, -1},    //打开主卧灯
    {69, -1},    //打开客房灯
    {71, -1},    //打开餐厅灯
    {73, -1},    //打开厕所灯

    //外灯
    {67, -1},    //打开花园灯
    {75, -1},    //打开阳台灯

    {62, -1},    //绿色模式
    {63, -1},    //蓝色模式
    {64, -1},    //彩色模式
};

void deal_asr_msg_by_multi_cmd_id()
{
    //第2个参数表示，组合p_play_info_all[0] ~ p_play_info_all[5]，6个命令词的音频播放
    prompt_play_by_multi_cmd_id(p_play_info_all, 6, default_play_done_callback);
    //绿色模式
    prompt_play_by_multi_cmd_id(&p_play_info_all[6], 1, default_play_done_callback);

    //第2个参数表示，组合p_play_info_all[0] ~ p_play_info_all[3]，4个命令词的音频播放
    prompt_play_by_multi_cmd_id(p_play_info_all, 4, default_play_done_callback);
    //彩色模式
    prompt_play_by_multi_cmd_id(&p_play_info_all[8], 1, default_play_done_callback);

    //第2个参数表示，组合p_play_info_all[4] ~ p_play_info_all[6]，3个命令词的音频播放
    prompt_play_by_multi_cmd_id(&p_play_info_all[4], 3, default_play_done_callback);
    //蓝色模式
    prompt_play_by_multi_cmd_id(&p_play_info_all[7], 1, default_play_done_callback);
}
```

### 2.4. 抢占播报

打断正在播放的提示音，并清空之前的播报队列，立即开始新的提示音播报。

支持的接口:

* prompt\_play\_by\_cmd\_handle
* prompt\_play\_by\_cmd\_id
* prompt\_play\_by\_semantic\_id
* prompt\_play\_by\_cmd\_string

代码中调用方式如下，第4个参数true用于抢占播报：

```
 //第4个参数true，打断正在播放的提示音，并清空之前的播报队列，立即开始新的提示音播报。
 prompt_play_by_cmd_handle(cmd_handle, 0, default_play_done_callback,true);
 prompt_play_by_cmd_id(2, 0, default_play_done_callback,true);
 prompt_play_by_semantic_id(0x2BC1943, 0, default_play_done_callback,true);
 prompt_play_by_cmd_string("打开空调", 0, default_play_done_callback,true);
```

### 2.5. 队列播报

与抢占播报相反，只是把新的提示音放到播报队列的尾部，等待播放器依次播报。注意，SDK中播报队列满个数有限，需很多个音频连续队列播报时，建议组合播报+队列播报两种方式结合。

支持的接口:

* prompt\_play\_by\_cmd\_handle
* prompt\_play\_by\_cmd\_id
* prompt\_play\_by\_semantic\_id
* prompt\_play\_by\_cmd\_string

代码中调用方式如下，第4个参数false用于队列播报：

```
 //第4个参数false，只是把新的提示音放到播报队列的尾部，等待播放器依次播报。
 prompt_play_by_cmd_handle(cmd_handle, 0, default_play_done_callback,false);
 prompt_play_by_cmd_id(2, 0, default_play_done_callback,false);
 prompt_play_by_semantic_id(0x2BC1943, 0, default_play_done_callback,false);
 prompt_play_by_cmd_string("打开空调", 0, default_play_done_callback,false);
```

### 2.6. 回调函数

prompt\_player所有的播放接口都是非阻塞的，不会等到提示音播放结束才返回。但应用的很多逻辑可能需要知道播放结束的时机。

所以，所有的播放接口都提供了回调功能。在调用播报接口时，传入一个回调函数指针，播放器在播放结束的时候，会调用传入的回调函数，让调用者处理结束后的相关逻辑。

支持的接口:

* prompt\_play\_by\_cmd\_handle
* prompt\_play\_by\_cmd\_id
* prompt\_play\_by\_semantic\_id
* prompt\_play\_by\_cmd\_string
* prompt\_play\_by\_multi\_cmd\_id

代码中调用方式如下，第3个参数default\_play\_done\_callback用于指定播放完成后的回调函数：

```
 //第3个参数default_play_done_callback，为播放接口的回调函数
 prompt_play_by_cmd_handle(cmd_handle, 0, default_play_done_callback,true);
 prompt_play_by_cmd_id(2, 0, default_play_done_callback,true);
 prompt_play_by_semantic_id(0x2BC1943, 0, default_play_done_callback,true);
 prompt_play_by_cmd_string("打开空调", 0, default_play_done_callback,true);

 //播放接口回调函数
 void default_play_done_callback(cmd_handle_t cmd_handle)
 {
    resume_voice_in();

    //添加播放完成后的处理逻辑
 }
```

### 2.7. 播报状态

播报器状态查询与控制，使能控制。
相关的接口：

* prompt\_player\_enable
* prompt\_is\_playing
* prompt\_stop\_play

代码中调用方式如下：

```
 //开启语音播报
 prompt_player_enable(ENABLE);

 //关闭语音播报
 prompt_player_enable(DISABLE);

 //检查提示音播放状态
 if(prompt_is_playing() == true)
 {
      //正在播放
 }
 else
 {
      //播放完成
 }

 //停止提示音播放
 prompt_stop_play();
```