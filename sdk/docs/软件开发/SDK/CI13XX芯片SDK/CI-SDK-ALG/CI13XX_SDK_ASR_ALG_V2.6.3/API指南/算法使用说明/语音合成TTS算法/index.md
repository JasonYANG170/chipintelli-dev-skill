<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%AF%AD%E9%9F%B3%E5%90%88%E6%88%90TTS%E7%AE%97%E6%B3%95/ -->

# 语音合成TTS算法

语音合成算法是一种将文本转换为声音信号的技术，又称为文本转换语音技术(TextToSpeech,TTS)，多应用于ETC、公交智能报站器、叫号系统等领域。

**1. 文本合成TTS算法涉及前端算法模型和相关库文件，firmware文件请替换external\firmware参考\tts\firmware。**

**2.算法功能配置步骤如下：**

打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，对CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_TTS)

**CI\_ALG\_TYPE变量和算法功能对应说明请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**3. 该算法参数宏说明在projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\app\app\_main\user\_config.h文件中**

```
//TTS文本合成通信串口号
#define UART_TTS_NUMBER         HAL_UART1_BASE  
//TTS文本合成通信串口中断号        
#define UART_TTS_IRQ            UART1_IRQn     
//TTS文本合成通信串口波特率         
#define UART_TTS_BAUDRATE       UART_BaudRate115200
```

**4. 语音芯片和上位机串口通信协议说明☞[TTS上位机和语音芯片通信串口协议\_V1.xlsx](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E9%80%9A%E4%BF%A1%E5%8D%8F%E8%AE%AE/TTS%E4%B8%8A%E4%BD%8D%E6%9C%BA%E5%92%8C%E8%AF%AD%E9%9F%B3%E8%8A%AF%E7%89%87%E9%80%9A%E4%BF%A1%E4%B8%B2%E5%8F%A3%E5%8D%8F%E8%AE%AE_V1.xlsx)**

说明：上位机按照协议发送文本合成指令，合成后的音频会直接通过语音芯片播放出来(用户无法获取到播放的音频数据)

**5. 该算法Demo使用涉及”启英泰伦-语音开发工具”，请前往启英泰伦语音AI平台[开发资料](https://aiplatform.chipintelli.com/attachment)中下载获取chipintelli-audio-tools\_vx.x.x.exe**
![语音开发工具](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E8%AF%AD%E9%9F%B3%E5%BC%80%E5%8F%91%E5%B7%A5%E5%85%B7%E4%B8%8B%E8%BD%BD.png)

5.1 打开启英泰伦-语音开发工具，选择 TTS串口工具功能，通信串口配置与代码中TTS串口配置一样后打开串口。

![图3-4 通信串口配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E7%AE%97%E6%B3%95SDK2.4.18%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-4.png)

5.2 输入待合成文本或加载默认文本后，点击开始合成 ，工具将把**光标起始后的文本内容**使用通信串口发送协议数据给语音芯片，芯片通过日志串口打印出文本信息并进行播报。

![图3-5 加载文本开始合成](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E7%AE%97%E6%B3%95SDK2.4.18%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-5.png)

注意

1. TTS串口接收数据支持GB2312编码格式，GB2312将转换utf-8格式进行后续处理。
2. 从端接收协议buffer大小限制，协议数据长度请不要超过3000个字节，超出协议数据可能会解析出错。