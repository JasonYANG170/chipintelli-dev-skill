<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3/ -->

# 多意图语音识别开发文档

多意图使用宏开关控制 MULT\_INTENT

设置为1表示单意图，设置为2表示双意图，设置为3表示三意图，最多3个意图，超出无效，默认三意图。

文件位置：projects/offline\_asr\_alg\_pro\_sample/project\_file/makefile

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3-CI13XX-1.png)

## **一、多意图处理开发流程：**

### 1. **做词**

上传[语音AI平台](https://aiplatform.chipintelli.com/) 制作语言模型，选择好对应的dnn后，自然说多意图复选框要勾选上，如图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3-CI13XX-2.png)

### 2. **cmd\_info文件修改**

a. 找到命令词ID一列，设置命令词属性类别，同一个操作对象属性设置相同，如空调领域，唤醒词”小艾小艾”设置为1，命令词”打开屏显”，”关闭屏显”等屏显”打开”属性和”关闭”属性的所有命令词均设置为4，这里特别注意空调的”打开”属性和”关闭”属性要设置成不同的值，且空调”关闭”属性相关的命令词类别ID **必须设置为3**，如图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3-CI13XX-3.png)

其他的如”风速调大”、”风速调小”、”风速最大”、”风速最小”等同一操作对象的命令词类别ID设为同一个值，其他功能词依次类推。

另外需要注意的时，如果有特别要求，如客户要求必须输出”风速调大”、”风速调小”，那么在cmd-info表格中，这两类功能词的命令词ID要设置成不同。

b. 以往的防误词通常不出现在cmd\_info表格中，但是多意图需要将带”防误”的词条补充进表格中，且设置一个较高的置信度和较大的count值。如图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3-CI13XX-4.png)

c，多意图功能中命令词的打分阈值较自然说偏高，所以表格中设置置信度略微调高一点，通常两字的命令词置信度120左右，三字的置信度110左右，四字的置信度90~100，五字的置信度80左右，命令词长度依次类似减小；另外多意图中命令词count不在cmd-info表格中填写

### 3. **文件中数组修改**

找到文件projects/offline\_asr\_alg\_pro\_sample/app\_main/ci\_nlp\_user.c 这里有几个数组需要特别注意一下

a. 数组 pNlpSpecialCmdParamTemp1 用于如”开低档”，”开一档”等极易识别为”开机-开低档”，其中”开机”属于多识别出来词，该数组用于容易会多识别”开机”的词做优化处理，例如：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3-CI13XX-5.png)

b. 数组 pNlpSpecialCmdParamTemp4 用于有关闭属性，且也可以单独说的命令词，如指令词”负离子”，它有关闭属性，可以说指令”关闭负离子”，需要注意存在包含关系的命令词，被包含的词放在包含词的后面，例如：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3-CI13XX-6.png)

c. 数组pNlpSpecialCmdParamTemp5、pNlpSpecialCmdParamTemp5Real

这两个数组用于相近易误识的词处理，如”打开等离子风”识别成”打开负离子、打开等离子风”。其中”打开负离子”属于多识别输出的，放入数组pNlpSpecialCmdParamTemp5中，而正确的识别是”打开等离子风”，放入数组pNlpSpecialCmdParamTemp5Real中，两个词在数组中的索引位置要一致，如图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3-CI13XX-7.png)

d. 数组 pNlpSpecialCmdParamTemp6 为固定的领域命令词条，且命令词顺序不可更改，如下图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3-CI13XX-8.png)

示例是空调领域，填写的是”空调打开”、”打开空调”、”关闭空调”，如果用于风扇领域，则改为”风扇打开”、”打开风扇”。”关闭风扇”。

### 4. **参数修改**

|  |  |
| --- | --- |
| ``` 1 ``` | ``` 找到配置文件 system/sdk_default_config.h ``` |

a. 找到宏 ADAPTIVE\_CNT\_ENABLE 设置为 1

b. 找到宏 MAX\_STOP\_VAD\_FRM 设置为60

c. 找到宏 ASR\_FE\_REDUCE\_MEM 设置为1

找到文件components/nlp/ci\_nlp.h

d. 找到宏 NLP\_BUFFER\_MAX 表示用于存储单个命令词最大长度，如 设置成42 表示最大14个中文字符

e. 找到宏 NLP\_SAVE\_STR\_MAX\_NUM 表示用于存储中间结果的最大字符串数量，双意图通常设置为6，三意图通常设置为10

### **注意：**

a，针对多意图输出到协议有排序的情况，如对”开机”、”关机”类命令词做排序的情况，需要在文件 projects/offline\_asr\_alg\_pro\_sample/app/app\_main/ci\_nlp\_user.c 中找到函数nlp\_intents\_sort（）， 注明”开机”、”关机”类命令词的语义ID，如图：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%A4%9A%E6%84%8F%E5%9B%BE%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E5%BC%80%E5%8F%91%E6%96%87%E6%A1%A3-CI13XX-9.png)

b，关于设置多意图词条等待时长级别的问题说明，找到文件components/nlp/ci\_nlp\_control.h，第一个宏 WAIT\_NLP\_NEXT\_TIME：下一个意图识别等待最长时间设置，设置范围0~7档，默认设置4档1680ms；

1、最长等待时间，与长词条识别关联高，长词条在第二和第三个意图中出现时，当上一个意图识别到后，需要考虑说出下一个长词条的间隔时间，加上说出来长词条所需的时间；

2、两个时间相加会比较长。对于七八个字以上的长词，可能至少需要等待1800毫秒以上，才能较好的识别长词条的意图。这个等待时间，起始值为960ms，步进180ms，每个等级时间如下:0:960ms， 1:1140ms， 2:1320ms，3:1500ms，4:1680ms, 5:1860ms， 6:2040ms，7:2220ms

第二个宏NLP\_NEXT\_DELAY\_TIME：下一个意图识别，有效解码输出的超时等待时间，设置范围0~7档，默认设置3档900ms；

此超时时间说明分以下三种情况：

1、等待下一个意图识别的过程中，如果持续产生有效的解码输出。不是第三个意图时，

会重置这个超时时间。会有以下两种情况发生：

A、在WAIT\_NLP\_NEXT\_TIME超时时间内，出现有效意图识别，那么当前意图识别流程结束，

继续下一个意图识别流程；

B、在WAIT\_NLP\_NEXT\_TIME超时时间内，没能正确解码有效意图，整个多意图识别流程结束，输出最终多意图识别结果；

2、等待下一个意图识别的过程中，如果持续产生有效的解码输出。并且，此时是第三个意

图识别过程中，不会重置这个超时时间。会在这个超时时间内，解码第三个意图，最终可

能输出，也可能不输出第三个意图，并且结束多意图解码流程。可以看出，有三意图的情况

下，最后第三个意图超时时间是NLP\_NEXT\_DELAY\_TIME，而不是WAIT\_NLP\_NEXT\_TIME；

3、等待下一个意图识别的过程中，如果持续无有效的解码输出。那么，当这个超时等待时间结束后，会快速结束多意图识别流程。通常，在安静环境中，结束多意图识别流程，就是这种超时结束情况生效。

这个等待时间，起始值为720ms，步进60ms，每个等级时间如下:

0:720ms，1:780ms，2:840ms，3:900ms，4:960ms，5:1020ms，6:1080ms，7:1140ms

c，关于多意图是否输出同类型互斥词条，如同时说”一档风”、”二档风”、”三档风”，正常情况只会输出”三档风”，如果需要都输出，只需要将cmd info 表格中这三个词的类型ID设置不同值即可。