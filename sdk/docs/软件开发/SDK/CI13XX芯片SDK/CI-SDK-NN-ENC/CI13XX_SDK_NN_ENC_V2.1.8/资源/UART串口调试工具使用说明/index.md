<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E8%B5%84%E6%BA%90/UART%E4%B8%B2%E5%8F%A3%E8%B0%83%E8%AF%95%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# 启英泰伦AI语音uart串口调试工具使用说明

## 串口调试工具作用

串口调试工具主要用于计算机与外部设备（例如语音开发板）之间的串行通信。它可以将计算机的数据转换成串行信号发送给外部设备，或者将外部设备的串行信号转换成计算机可识别的数据。串口调试工具常用于嵌入式系统开发、硬件调试、设备控制等场景。启英商城购买链接☞[串口调试工具](http://mall.chipintelli.com/chip2?product_id=60&brd=1)。

![串口调试工具使用说明-1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E8%B5%84%E6%BA%90/img/%E4%B8%B2%E5%8F%A3%E8%B0%83%E8%AF%95%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-1.png)

---

## 串口调试工具如何使用

1.安装串口驱动：首先，需要在计算机上安装与串口调试工具相匹配的驱动程序，以确保计算机能够识别并与串口设备正常通信。以广泛使用的USB转串口芯片CH340为例，安装驱动程序可以浏览器搜索“CH340官方驱动”，点击进入官网之后即可选择自己电脑对应系统的驱动进行下载安装，下载链接参考☞[CH340官方驱动](https://www.wch.cn/products/CH340.html)。![串口调试工具使用说明-2](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E8%B5%84%E6%BA%90/img/%E4%B8%B2%E5%8F%A3%E8%B0%83%E8%AF%95%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-2.png)

2.打开串口调试助手：安装完成后，打开串口调试助手软件。在软件界面中，选择正确的串口号、波特率、数据位 、停止位和校验位等参数，以确保与串口设备的通信设置一致。串口调试助手软件下载参考☞[SSCOM 5.13.1下载](https://soft.3dmgame.com/down/247967.html)，软件使用参考☞[SSCOM5.13.1使用](https://blog.csdn.net/weixin_36078669/article/details/142141649?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0-142141649-blog-145615357.235^v43^pc_blog_bottom_relevance_base3&spm=1001.2101.3001.4242.1&utm_relevant_index=2)。

![串口调试工具使用说明-3](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E8%B5%84%E6%BA%90/img/%E4%B8%B2%E5%8F%A3%E8%B0%83%E8%AF%95%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-3.png)

3.发送和接收数据：设置好参数后，可以通过串口调试助手发送数据给外部设备，或者接收外部设备发送过来的数据。在发送数据时，可以在串口调试助手的发送框中输入要发送的内容，然后点击发送按钮；在接收数据时，串口调试助手会自动将接收到的数据显示在接收框中。

---

## TX和RX的连接要求

在串口通信中，TX代表发送（Transmit），RX代表接收（Receive）。一般情况下，计算机的串口TX端会连接到外部设备的RX端，计算机的串口RX端会连接到外部设备的TX端。这样，计算机就可以通过串口发送数据给外部设备，并接收外部设备发送过来的数据。

---

## 针对启英泰伦AI语音芯片（以下统称CI芯片）的功能

1.固件烧录：CI芯片通常需要通过串口调试工具进行固件烧录。在烧录过程中，需要将CI芯片连接到计算机的串口上，并使用串口调试工具将固件文件写入CI芯片中。固件烧录完成后，CI芯片就可以按照预定的程序进行工作。具体图文流程可以查看☞[CI芯片烧录流程](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/SDK%E5%BF%AB%E9%80%9F%E5%BC%80%E5%8F%91/)。

2.查看日志：CI芯片程序默认可以通过烧录用的串口0进行日志打印，方便开发者调试程序，串口升级工具也在升级界面添加了日志查看的功能，选好对应的串口号，波特率默认为921600，在日志输出前的框中勾选即可看到运行的日志。

![串口调试工具使用说明-4](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E8%B5%84%E6%BA%90/img/%E4%B8%B2%E5%8F%A3%E8%B0%83%E8%AF%95%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-4.png)

3.串口采音级分析：CI芯片还可以通过串口调试工具进行采音级分析。在采音过程中，可以使用串口调试工具将CI芯片采集到的音频信号发送到计算机上进行处理和分析。这有助于了解CI芯片的音频采集性能和音质表现。具体图文流程可以查看☞[启英泰伦采音板操作说明及语音模块板底噪分析\_V1.2](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/%E8%B5%84%E6%BA%90/pdf/%E5%90%AF%E8%8B%B1%E6%B3%B0%E4%BC%A6-%E9%87%87%E9%9F%B3%E6%9D%BF%E6%93%8D%E4%BD%9C%E8%AF%B4%E6%98%8E%E5%8F%8A%E8%AF%AD%E9%9F%B3%E6%A8%A1%E5%9D%97%E6%9D%BF%E5%BA%95%E5%99%AA%E5%88%86%E6%9E%90_V1.3_20250928.pdf)。

---

## USB隔离器的作用

USB隔离器是一种用于保护计算机和外部设备免受电气干扰和损坏的设备。它可以将计算机与外部设备之间的电气连接进行隔离，从而避免电气噪声和干扰对通信和数据的影响。在购买和使用隔离器时，需要选择与计算机和外部设备相匹配的型号和规格，并按照说明书进行正确连接和配置。购买链接参考☞[USB隔离器购买链接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.38122e8dfGNLzZ&id=589748066375&_u=3alm7e6c669)

备注

电路板供电为非隔离电源才会用到此工具，接隔离电源不用此工具

---

## 其他注意事项

1.在使用串口调试工具时，需要注意保持计算机与外部设备之间的电气连接稳定可靠，避免接触不良或松动导致通信失败或数据丢失。

2.在配置串口参数时，需要根据实际使用的串口设备和通信协议进行正确设置，以确保通信的正常进行。

3.在进行固件烧录或采音级分析时，需要遵循相关教程或文档中的操作步骤和注意事项，以避免对CI芯片或计算机造成损坏或数据丢失。

4.在使用隔离器时，需要注意选择合适的型号和规格，并按照说明书进行正确连接和配置，以确保隔离效果和保护作用得到充分发挥。