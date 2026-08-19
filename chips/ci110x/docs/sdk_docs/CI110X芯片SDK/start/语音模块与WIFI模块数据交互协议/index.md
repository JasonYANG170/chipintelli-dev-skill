<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/%E8%AF%AD%E9%9F%B3%E6%A8%A1%E5%9D%97%E4%B8%8EWIFI%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE/ -->

# 语音模块与WIFI模块数据交互协议

## 1. 概述

本文为CI110X系列芯片与其他芯片通过UART传输大规模数据时，为保证传输数据的稳定性和准确顶，规定的串口传输协议。

---

## 2. UART串口传输协议

### 2.1. UART配置参数

请按照下述参数配置用于传输的串口UART。

* 波特率：921600；
* 有效数据位宽：8bit；
* 停止位：1bit；
* 校验位：无。

### 2.2. 应用层协议

#### 2.2.1. 数据格式

![应用层数据格式](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/start/img/%E8%AF%AD%E9%9F%B3%E6%A8%A1%E5%9D%97%E4%B8%8EWIFI%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE/%E8%AF%AD%E9%9F%B3%E6%A8%A1%E5%9D%97%E4%B8%8EWIFI%E6%A8%A1%E5%9D%97%E6%95%B0%E6%8D%AE%E4%BA%A4%E4%BA%92%E5%8D%8F%E8%AE%AE-1.jpg)

上图数据格式的UART数据头部分可封装成以下数据结构，和上图对应:

```
typedef struct dna_standard_head
{
    unsigned int magic; /* 数据包头，固定设为0x5a5aa5a5; */
    unsigned short checksum; /* 校验位，暂无*/
    unsigned short type; /* 命令类型*/
    unsigned short len; /* 有效负载数据长度*/
    unsigned short version; /*版本信息*/
    unsigned int fill_data; /*填充数据*/
}dna_standard_head_t;
```

结构体中各个变量说明如下：

* magic（数据包头）：固定为0x5a5aa5a5；
* checksum（校验位）：为加快传输速度，暂时没有使用；
* type（命令类型）：UART数据包的指令类型；
* len（有效数据长度）：有效负载的长度（不包含数据包头）；
* version（版本信息）：版本信息，暂时没有使用；
* fill\_data（补充数据）：保留数据，扩展使用。

#### 2.2.2. 数据传输方式

数据按照低位字节先发送，高位字节后发送的方式进行发送和接收解析。发送数据时整个数据包的长度(包括数据包头)必须为16字节的整数倍。如果数据包长度不满足要求，在包尾填0补充。

### 2.3. 传输层协议

1. LinkMsgProc.c和LinkMsgProc.h文件中封装了编码接口LinkMsgCode和解码数据包接口LinkMsgDeCode，该代码如用户有需要，可以联系我司技术支持人员获取。打包时，将应用层打包过的数据(数据格式如图1所示)传入LinkMsgCode编码接口，输出的数据补齐为16字节的整数倍，然后发送。解包时，将接收到的数据传入LinkMsgDeCode解码接口，会自行解出完整数据包。
2. 封包（编码）协议，主要对数据（按应用层数据格式封装的未编码数据）做以下三部分编码（具体代码可以参考LinkMsgProc.c）:

* 在包头添加一字节：0xa5；在包尾添加2字节：0xa4、0x00；
* 包尾在添加两个字节之前（即0xa4之前）会再添加1字节数据，该数据值为数据包各个字节的异或最终值；
* 数据包中间的数据，如果等于0xa5,则在该数据后再添加0xa5，如果等于0xa4，则在该数据后再添加0xa4。

  解包（解码）与封包过程相反，即去掉封包协议时添加的各字节，恢复原始应用层格式数据。

3. 发送数据过程：应用层的数据打包完成后，通过传输层封包编码（LinkMsgCode），将数据发送出去。

   接收数据过程：接收到数据后，通过传输层协议，解析出应用层数据包，再按照应用层格式解析数据包（LinkMsgDeCode）。

我司有完整的语音离在线及AIOT的SDK开发包，如需开发请联系我司技术支持人员获取。