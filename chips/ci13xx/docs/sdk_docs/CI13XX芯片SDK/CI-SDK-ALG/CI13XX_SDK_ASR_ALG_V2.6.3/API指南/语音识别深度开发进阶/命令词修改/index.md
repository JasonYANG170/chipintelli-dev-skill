<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9/ -->

# 命令词修改

## 1. 什么是asr模型

1.1. asr 是语言模型，是根据产品的命令词，使用启英平台，“语言模型开发”功能，生成的.dat数据，配合声学模型（dnn）一起实现语音识别的效果。

![命令词修改-20250610143903867](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610143903867.png)

---

## 2. asr模型有什么作用

2.1. asr模型里包含所做产品的命令词和唤醒词，如果需要增加、删除、修改某个词条，都需要修改asr模型。

---

## 3. 如何制作asr 模型

3.1. 命令词表格制作asr 模型

3.1.1. 找到启英平台”功能开发”图标

![命令词修改-20250610151343575](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610151343575.png)

3.1.2. 下滑，找到“语言模型开发”

![命令词修改-20250610143903867](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610143903867-1749555529392.png)

3.1.3 点击“新建项目”

![命令词修改-20250610151944934](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610151944934.png)

3.1.4. 编辑项目信息

a. 编辑项目名称：可以根据项目实际名称修改。

b. 应用场景：根据所做产品选择。

c. 芯片选择：选择相应芯片型号。

d. 语言类型：选择中文、英文或者其他语言。

e. 模型类型：根据所做产品选择相应的模型，模型类型和使用的SDK版本有关系，pro声学模型V2可以做更多的词条。

f. 选择操作类型：选择命令词合成语言模型。

![命令词修改-20250610152544984](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610152544984.png)

3.1.5. 点击“继续”按钮

a. 我们举例使用声学模型 “中文普通话通用\_pro\_V2\_1.3M\_V00874”。

b. 语言模型类型：选择固定词条，点击问号按钮，有相应说明。

![命令词修改-20250610154639995](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610154639995.png)

c. 点击附件样例，下载填写命令词的标准表格。

![命令词修改-20250610154336565](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610154336565.png)

3.1.6. 编辑下载的附件样例 “命令词列表\_中文模板.xlsx”。

a. 语义标签：按从小到大的序号排列，相同语义可以使用相同编号，例如：打开空调和空调打开。

b. 命令词：就是需要识别的产品词条，按顺序往下排，具体的规则可以参考此表格文件的第二张表“填写说明”。

c. 命令词类型：命令词有三种类型：唤醒词、命令词、负性词。

名词解释

* **唤醒词** ：让设备从非唤醒状态切换到命令词识别状态的词语。
* **命令词** ：让设备执行相应工作状态的词语。
* **负性词** ：理解为不需要识别的词，有些词语容易和需要识别的命令词发生误识别，加入这些词可以减少误识别的概率。

例如：命令词：开灯，可以加入负性词：开机，开关，开门等。

![命令词修改-20250610160233047](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610160233047.png)

d. 表格编辑好后，可以点击上传也可以直接拖拽到“上传附件”处。

e. 模型网络：点击问号，会显示相应说明。

![命令词修改-20250610162954534](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610162954534.png)

f. 点击提交，等待平台完成制作，下载asr模型

![命令词修改-20250610163322757](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610163322757.png)

3.​2. 使用分词文件制作asr 模型

3.2.​1. 什么是分词

分词是记录唤醒词、命令词、负性词的一种文本格式。

3.​2.1. 怎么看懂分词

用不同的结束标号来区别，数字中1是每个完整命令词或者负性词的统一起始点，2、3、4是不同的识别节点，包含相同部分的（晾杆上升、晾杆下降可以共用晾杆）命令词的分割，1000是最终命令词的终点，1到2，2到1000，组成一个命令词，1到2000是负性词，只有最终为1000的词才能有识别输出，最终节点为2000的词用于降低误唤醒或者误识别，可以加入日常常用的词。

![命令词修改-20250611141203797](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250611141203797.png)

3.3. 怎么用分词制作asr

3.3.​1. 在上述说明中的使用命令词表制作asr模型，第四步，选择操作类型选项重新选择”分词合成语言模型“

![命令词修改-20250610171205691](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610171205691.png)

3.3.​2. 将分词文件上传或者拖拽到上传附件处，点击提交按钮。

![命令词修改-20250610172253926](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610172253926.png)

3、实际开发情况

a. 有现成的相应命令词分词：就使用分词来制作asr模型。

b. 没有现成的命令词分词： 可以先使用命令词表格来制作asr模型，在下载文件里包含分词文件，在此分词文件的基础上进行，增加、删 除、修改、优化。

---

## 4. asr模型放在SDK的对应文件夹及注意事项

4.1. asr模型 需要放在SDK的指定目录：SDK开发包文件夹\工程文件夹\示例文件夹\固件文件夹\自动语音识别文件夹\。

例如：CI13XX\_SDK\_ASR\_ALG\_V2.6.3\projects\offline\_asr\_alg\_pro\_sample\firmware\asr\

![命令词修改-20250610135746243](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610135746243.png)

asr模型为.dat文件，如下：

![命令词修改-20250610181636199](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610181636199.png)

4.2. 制作唤醒词asr模型时，生成出来的.dat模型文件前的标号默认是[0]，可以手动修改为[1]。

4.3. asr模型前面的标号，需要和user\_file中[60000]开头的表格里的模型ID 保持对应。

![命令词修改-20250610182708680](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610182708680.png)

---

## 5. 如何测试

5.1. 完成制作asr模型后，放在相应文件夹，第一步，点击合成分区bin文件.bat，主要的作用是，把多个asr模型，dnn模型，[60000]开头的表格、voice文件夹中的播报音，都合成为可以打包成固件的二进制.bin文件。

5.2. 第二步，点击打包升级.bat，主要的作用是，把多个asr模型，dnn模型，[60000]开头的表格、voice文件夹中的播报音，都合成为一个完整的固件。

![命令词修改-20250610190525024](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610190525024.png)

5.3. 生成固件后，点击固件升级，勾选相应COM口，重启芯片，进度条开始移动，直到100%变成绿色，代表烧录完成。

![命令词修改-20250610191639972](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610191639972.png)

![命令词修改-20250610191833357](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610191833357.png)

5.4. 烧录完成后，可以勾选日志输出，喊出制作的唤醒词和命令词，日志会打印出文本。

![命令词修改-20250610192713268](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB%E6%B7%B1%E5%BA%A6%E5%BC%80%E5%8F%91%E8%BF%9B%E9%98%B6/img/%E5%91%BD%E4%BB%A4%E8%AF%8D%E4%BF%AE%E6%94%B9-20250610192713268.png)