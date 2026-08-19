<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/%E8%87%AA%E5%8A%A8%E5%8C%96%E8%AF%86%E5%88%AB%E6%B5%8B%E8%AF%95/ -->

# 自动化识别测试

正常情况下，语音识别测试时间较长，且产品开发时经常会有多次测试的情况发生，如按照传统人工测试的方式，耗时间长且效率低，会拖慢产品开发到量产的周期。因此，我司开发了一套自动化识别测试的方法，用电脑代替人工进行测试，可以有效降低人员使用，提升测试效率，加速产品测试过程。详细描述如下。

## 测试所需的设备及仪器

要进行自动化识别测试，需要先准备以下的设备（其中一些设备型号非强制，用户如果有同样规格功能的设备也可以更换使用）：

| 序号 | 类别 | 设备 | 设备型号 | 设备品牌 | 作用 | 图片 |
| --- | --- | --- | --- | --- | --- | --- |
| 01 | 电脑 | 台式机/笔记本 | 不限 | 不限 | 监测语音模块反馈是否准确输出测试结果 | 电脑 |
| 02 | 声音源 | 人工嘴 | 4227-A | Brüel & Kjær | 播放音频信号 | 声音源 |
| 03 | 噪声监测 | 精密噪音计（声级计） | 1357 | TES | 测试到达麦克风的声压 | 噪音监测 |
| 04 | 噪声源 | 音箱/电视 | 监听音箱建议型号：FX8 | Fluid Audio | 播放噪声、模拟外部干扰音 | 噪声源 |
| 05 | 音频收集 | 高保真录音设备 | R44 | 逻兰/Roland | 音频的录制 | 音频收集 |

表1 语音识别测试设备

---

## 测试设备及仪器的布置

**人工嘴**

型号：4227-A  
性能指标：

* 额定输出声压SPL：
  + 200Hz - 2kHz ----- 110dB
  + 100Hz - 8kHz ----- 100dB
* 失真（@94dB）：
  + 200Hz - 250Hz ----- <2%
  + ’>‘250Hz ----- <1%
* 阻抗 ----- 4Ω
* 最大承受 ----- 10W
* 瞬间承受功率 ----- 50W
* 嘴径 ----- 20mm

人工嘴用于播放声音源，位置距离语音模块麦克风正前方，水平直线距离4-5米。声音源距离地面1-1.5m（模拟人声正常高度），如图1所示。

![音频收集](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/%E6%B5%8B%E8%AF%95%E8%AE%BE%E5%A4%87%E5%8F%8A%E5%B8%83%E7%BD%AE-6.png)

图1 人工嘴摆放位置

**噪声源（监听音箱）**

型号：Fluid Audio FX8  
性能指标：

* 频率响应：35Hz - 22kHz(±3dB)；
* 交叉频率：2.4kHz；
* 低频放大器功率：80 watts；
* 高频放大器功率：50 watts；
* 信号噪声：> 100dB（typical A-weighted）；
* 极性：正信号+输入时产生一个向外的低频位移；
* 输入阻抗：20千欧（平衡式），10千欧（不平衡式）；
* 输入灵敏度：当音量控制设置为最大值（102dB的最大声压）时，输入85 毫伏的粉红噪声会产生95dBA的输出声压；
* 电源：115V ~50/60 Hz 或230V~50/60 Hz（用户可进行切换）；
* 保护装置：射频干扰，输出电流限制，过温保护，瞬态开启/关；
* 保护，超低音滤波器，外部电源保险丝；
* 箱体：乙烯基层压中密度纤维板；
* 尺寸（单个监听音箱）：340毫米（高）x254毫米（宽）x270毫米（长）；
* 重量（单个监听音箱））：9.8千克。

如果没有监听音箱，也可以用电视代替。

语音模块和精密噪音计位于同一平面处（距离地面1米）。与语音模块麦克风距离大于或等于1.5米，不要超过2米，如图2所示。

**精密噪音计**

型号：TES 1357  
性能指标：

* 0.1dB 分辨率；
* 测量范围30到130dB；
* 1/1,⅓,⅙,1/12,1/24倍频程频谱分析软件(可选)；
* 准确度 ± 1.5dB (ref 94dB @1KHz)；
* 加权测量范围 30dB to 130dB；
* C加权测量范围 35dB ~ 130dB；
* 量测档位 30-80dB, 50-100dB, 60-110dB, 80-130dB；
* 频率响应 31.5 Hz to 8KHz；
* 数字显示 4 位数 LCD , 0.1dB resolution, updated every 0.5s；
* AC / DC 信号输出 2Vrms/每档满刻度，10mV/dB。

精密噪音计与语音模块麦克风尽量靠近（两者之间距离≤5cm），但不能与语音模块麦克风接触，如图2所示。

**电脑和待测模块**

电脑和待测模块用USB转串口工具连接，电脑上可以接收到模块发来的串口信息。电脑、模块、精密噪音计（分贝仪）和噪音源的位置关系如图2所示。

![音频收集](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/%E6%B5%8B%E8%AF%95%E8%AE%BE%E5%A4%87%E5%8F%8A%E5%B8%83%E7%BD%AE-7.png)

图2 噪声源摆放位置

布置好各类仪器设备和待测模块后，就可以使用我司提供的自动化测试工具进行测试。

---

## 自动化测试分析工具使用说明

## 一、工具主界面及目录

### 1.1、工具目录及说明

![image-1.1](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-1.1.png)

### 1.2、工具界面

![image-1.2](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-1.2.png)

## 二、自动化识别测试

### 2.1 工具结构说明

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-3.png)

#### 2.1.1 工作目录路径

.\ci\_test\_analysis\_tool\workspace\auto\_rectest\_tool

#### 2.1.2 工具目录

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-4.png)

工作目录下包含如下5个文件夹：

>>config：默认参数配置目录

>>result：测试结果存放目录

>>ResultBackUp：结果备份目录

>>SetTestInfo：测试信息配置目录

>>TestAudioFiles：测试音频目录

#### 2.1.3 自动化测试原理

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-5.png)

#### 2.1.4 测试工具界面

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-6.png)

### 2.2测试音频

#### 2.2.1 音频预处理

##### 2.2.1.1 音频标准化

大部分情况，录制的各测试音频音量大小不一致，故进行测试之前须对音频进行标准化处理，标准化前后的音频如下所示。

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-7.png)

##### 2.2.1.2 去音频前后静音

为了加快测试，减少不必要的时间浪费，测试播报前，也要对单个音频前后的静音进行处理，处理前后的音频如下所示。

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-8.png)

#### 2.2.2 测试播报音频存放要求

##### 2.2.2.1 音频存放方式

测试播报音频存放到此目录下有两种方式：

1、同一音频内容不同说话人的音频放一个目录下；

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-9.png)

2、同一说话人不同音频内容放一个目录下。

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-12.png)

##### 2.2.2.2 音频存放格式要求

音频文件格式为\*.wav格式。音频文件命名方式如下：

[1]小薇小薇.wav

[2]加热两分钟.wav

[197]加热六分钟.wav

[187]ALL-OPEN.wav

[188]HELLO-JENNY.wav

注：播报音频的字必须与预期输出结果的一样，因为内部是通过字符串匹配来判定识别正确与否。

### 2.3测试操作步骤

#### 2.3.1 自动播报噪音环境搭建

如果你不需要自动播报噪音，就略过此小节。

##### 2.3.1.1 组建噪音测试局域网

组件局域网的目的是为了测试时，自动播报噪音。需要将测试记录电脑与噪音播报电脑组建成局域网，且噪音播报电脑作为server，测试记录电脑作为client。

为了保证测试的稳定性，请使用网线连接电脑设备，不要使用无线连接进行组网，我们在测试中发现，无线组网不稳，会导致测试中断。如果你使用的电脑带有无线功能，请关闭后用网线连接进行组网。

###### ①硬件联网

将两台电脑用网线直接进行连接上。

###### ②配置server IP地址

右键点击信号图标

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-15.png)

点击“打开网络和共享中心”

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-16.png)

右键点击本地连接“启用”

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-17.png)

右键点击本地连接“属性”

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-18.png)

双击，在弹出的对话框中填入如下IP

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-19.png)

点击确定即可

###### ③ 配置client IP地址

安装3.3.2的配置方法，将测试记录电脑的IP配置成192.168.10.109

###### ④关闭防护墙

点击“开始->控制面板”

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-20.png)

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-21.png)

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-22.png)

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-23.png)

##### 2.3.1.2 准备噪音自动播放软件

将工具拷贝到噪音播报电脑上。

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-24.png)

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-25.png)

###### ①准备要播报的噪音

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-26.png)

噪音放在目录NoiseVoice下面，一种噪音一个文件目录，如：烟机、新闻噪声等。每个目录下面可以有多个\*.wav音频文件，如[1].wav、[2].wav等。

###### ②开启噪音自动播报软件

点击“开始”按钮开启噪音自动播报软件，如下：

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-27.png)

#### 2.3.2 测试固件制作及烧录

##### 2.3.2.1 固件制作要求

固件制作时，关闭所有其他与结果输出无关的打印。

##### 2.3.2.2 串口打印要求

串口打印须是如下形式：send result:小薇小薇 44。

其中：

“send result:”为结果标志符，其具体形式可在.\config config.txt中配置

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-28.png)

​ “小薇小薇”为命令词识别结果

“44”为命令词识别置信度

识别结果与置信度间用空格隔开

注：如果置信度后面还有打印其他信息，请用空格隔开即可

##### 2.3.2.3 固件测试模组数量要求

**为了避免硬件差异导致的测试不一致问题，建议测试时，每个固件使用两个模组板**。

#### 2.3.3 硬件连接及供电

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-29.png)

##### 2.3.3.1 连接收识别结果串口

测试电脑端接收串口RX连接测试模组的TX。

##### 2.3.3.2 供电

测试模组板5V供电，如果使用HUB对多块测试版进行供电，则HUB需要用稳定直流电供电，否则会因供电不足或不稳导致识别结果较差。

#### 2.3.4 填写默认配置文件

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-30.png)

打开.\config\config.txt，可以对默认邮箱、噪音播报IP、结果备份路径、识别结果标识符进行配置，实际参数填写在“[”以后。多个邮箱用英文分隔符隔开。噪音播报IP为噪音播报电脑（server）的IP地址。识别结果标识符默认是“send result:”，可配置，但必须与固件打印结果输出保存一致。

#### 2.3.5 测试音频准备

将按2.3准备好的音频存放到TestAudioFiles目录下。

#### 2.3.6 填写测试信息

打开目录SetTestInfo下的scheme.xls，按要求填写配置文件

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-31.png)

音频文件目录名为.\TestAudioFiles下的“热水器”

测试对象名称为固件的测试名称，相同的两个固件可以用：固件\_1、固件\_2表示

噪音音频名为噪音播报电脑. \playnoise\NoiseVoice目录下对应的噪声目录，如果名称为“0”，则表示不播报噪音

邮件发送人栏填写接收测试结果的邮箱，多个邮箱间用英文分号隔开。

#### 2.3.7 开启测试软件

如下所示：

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-32.png)

#### 2.3.8 播报间隔时间

音频播报间隔时间默认2S可调。

#### 2.3.9 选择是否发送邮件

根据需要，选择测试完成后是否将结果发送指定邮箱。

#### 2.3.10 开始测试

点击开始测试，程序运行正常，可在命令行窗口查看测试中间结果。

### 2.4 扩展功能

#### 2.4.1 追加测试任务

测试过程中，根据实际情况可以追加测试任务，打开excel，按格式填写追加任务保存即可。注意，无论任何时候，在excel的末尾都不要有空行出现。

#### 2.4.2 一唤醒一测试功能

离在线测试时为唤醒词和命令词交替进行播报，默认未使能。

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-33.png)

勾选后即可使能，并且可以设置唤醒词播报间隔时间。

唤醒的播报音频存放在路径：“.\TestAudioFiles\wakewords\热水器”目录下面，格式为以.wav结尾的wav文件。

#### 2.4.3 同一语义统计

为了统计结果的准确性，将同一语义的命令词存放在文件中：. \TestAudioFiles\热水器 SameSemanticCmd.txt

格式如下：

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-34.png)

同一语义的命令词在一行，命令词间用中文分号或英文分号分隔。

#### 2.4.4 仅播报功能

有时仅需要播报命令词，可直接点击界面上的“仅播报命令词”

#### 2.4.5 关闭噪音功能

如果需要关闭噪声播报，可直接点击界面上的“关闭噪声”

#### 2.4.6 重新生成结果功能

如果需要重新统计结果，可直接点击主界面上的“识别测试统计”按钮，并根据弹出的界面提示进行填写后重新统计。

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-35.png)

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-36.png)

#### 2.4.7 串口超时时间可配置

.\config\config.txt中可对串口超时时间进行配置

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-38.png)

#### 2.4.8 多输出结果格式对比测试

.\config\config.txt中可对结果标识符进行配置，多个标识符之间用英文分号（;）进行分隔，如下：

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-39.png)

如果需要第二分隔功能的固件，在测试参数表格中用“@1”进行标记，如下所示：

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-40.png)

#### 2.4.9 播报音筛选功能

如测试过程中存在命令词更改时，测试音频也需要对应修改，修改后可用“测试音频筛选”功能进行筛选和确认

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-41.png)

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-42.png)

##### 2.4.9.1 单个项目目录中筛选

###### ①源目录

源目录为完整的测试集音频，其目录格式为《测试集名称》—— 《人名文件夹》（可放多个人名文件夹）——测试音频wav文件，远，源目录路径选择《测试集名称》即可，对测试音频进行筛选

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-43.png)

###### ②输出目录

输出目录清空，用于保存已有的音频和相关日志文件

###### ③ 命令词文件

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-44.png)

###### ④结果

音频筛选出的目录结构

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-45.png)

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-46.png)

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-47.png)

##### 2.4.9.2 多个项目目录中筛选

###### ①源目录

源目录为所有的测试集音频，其目录格式为《测试音频集合目录》——《测试集名称》（可多个）——《人名文件夹》——测试音频wav文件，源目录路径选择《测试音频集合目录》即可，对所有的测试音频进行筛选，目录结构如下所示

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-48.png)

###### ②输出目录

需要在输出目录中新建所需要筛选人的空文件夹，用于存放每个人名所需要的测试音频。

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-49.png)

###### ③命令词文件

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-50.png)

###### ④选择功能

勾选“多个源目录”功能后点击“筛选”按钮。

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-51.png)

###### ⑤结果

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-52.png)

### 2.5 测试结果

#### 2.5.1 识别结果

识别结果在工作目录的result目录下

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-53.png)

更改前、更改后目录下存放的是详细日志。

\*\_time\_label.txt 是时间标签

\*\_summary.xls 是测试汇总结果

#### 2.5.2 结果汇总

\*\_summary.xls汇总

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-54.png)

#### 2.5.3 固件详情

\*\_summary.xls中各固件的详细汇总

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-55.png)

需要优化时，可参照个固件的详细汇总数据

## 三、误识别测试

### 3.1连接采音板

①采音板也叫录音板，是专用来配套开发板通过IIS传输对音频进行实时采集的重要外设，可在我司官网进行[样品购买](http://mall.chipintelli.com/0?product_id=74&brd=1)。

![image-20230505154211906](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-58.png)

②将采音板连接开发板，CI-D06GT01D三代离线语音开发板可直接将采音板对应排针插入排母中。最后将开发板与采音板分别使用type-c数据线与micro-usb数据线连接到电脑。

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-59.png)

注意：其他型号开发版或模块按照下图所示连接：

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-60.png)

| 开发板 | 录音板 |
| --- | --- |
| SDO | D1 |
| SCLK | CLK |
| LRCLK | LRCLK |
| GND | GND |

### 3.2串口连接板子

① 开发板使用Type-C数据线与电脑连接：

![image-20240830112824880](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-61.png)

②若是模块板则可使用USB转串口（USB-TTL）工具与电脑连接：

![模块和USB转串口工具连接及模块背面丝印图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-62.png)

### 3.3误识别测试

开启工具，点击“误识别测试”

![image-20240830104908302](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-56.png)

#### 3.3.1 勾选择需要测试的串口

![img](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%AD%E9%9F%B3%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95%E5%8F%8A%E8%AE%A4%E8%AF%81/img/image-57.png)

#### 3.3.2 标记测试固件的名称

#### 3.3.3 选择录音声道

#### 3.3.4 点击“开始”按钮进行测试

### 3.4仅采音

为解决第三方录音工具Cool Edit录音长时间后电脑卡死的问题而增加此功能，如果需要使用此功能，直接勾选“仅录音”即可。

#### 3.4.1 选择声道

#### 3.4.2 选择单个音频时长

#### 3.4.3 勾选“仅录音”

#### 3.4.4音频保存说明：

保存路径：. workspace\auto\_mistest\_tool\wav[1-1000]中

某个目录下音频个数与音频时长的关系如下：

| 音频时长 | 音频个数 |
| --- | --- |
| 2 | 500 |
| 3 | 500 |
| 4 | 500 |
| 5 | 500 |
| 10 | 500 |
| 20 | 500 |
| 50 | 200 |
| 100 | 100 |
| 600 | 30 |
| 1800 | 20 |
| 3600 | 10 |

wav 中的子目录从1 开始，录音满足时长对应的最大条数时，自动增加1

### 3.5停止测试

直接按“停止”按钮即可结束测试

### 3.6查看结果

点击“查看结果”查看测试结果