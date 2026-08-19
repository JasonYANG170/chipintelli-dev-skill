<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/iic/ -->

# 集成电路总线(IIC)

---

## 1、简介

* IIC是一种双向双线同步串行总线，包括SDA(串行数据线)和SCL(串行时钟线)，SDA和SCL管脚均为开漏输出。IIC总线通常用于单个或多个主设备和单个或多个从设备之间通信，每个连接到总线上的设备都有一个唯一的地址，同一时刻仅允许有一个master主设备发起请求访问slave从设备。
* CI13XX支持1个IIC，其数据帧格式通常由起始信号、地址信号、应答信号、数据信号和停止信号五部分组成，支持标准传输速率100kbit/s和快速传输速率400kbit/s两种模式。

---

## 2、特性

* SDA：串行数据线，双向I/O线；
* SCL：串行时钟线，由master提供；
* 支持master和slave模式通过API(iic\_polling\_init)配置；
* master：作为master主设备时启动总线传输数据，并产生时钟；
* slave：作为slave从设备时被寻址的任意器件，具有唯一地址；
* 起始信号：SCL为高电平时，SDA从高电平跳变至低电平，表示传输开始；
* 地址信号：支持7位寻址模式，包含7bit地址位和1bit读写位；
* 应答信号：ACK接收成功，NACK接受失败或传输结束；
* 数据信号：按Byte传输，先发送最高位MSB最后发送最低位LSB；
* 停止信号：SCL为高电平时，SDA从低电平跳变至高电平，表示传输结束；
* 总线传输速率通过API(iic\_polling\_init)可配置为标准-100kbit/s和快速-400kbit/s；

---

## 3、IIC时序

### 3.1、I2C总线物理拓扑图

* I2C总线可连接到多个设备，通常是单个或多个主设备和单个或多个从设备，每个连接到总线上的设备都有一个唯一的地址，同一时刻仅允许有一个master主设备发起请求访问slave从设备。

![IIC波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iic-1.png)

### 3.2、时序图

* （1）、连续写数据操作时序图：

![IIC波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iic-2.png)

* （2）、先写后读数据操作时序图：

![IIC波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iic-3.png)

* （3）、连续读数据操作时序图：

![IIC波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/img/iic-4.png)

---

* 主设备通过产生Start起始条件来启动通信：在SCL为高电平时将SDA拉低，并通过SCL发送8个时钟脉冲用于传输1个Byte，该Byte包含7Bit地址位和1Bit读/写位。若从设备的地址与传输的7Bit地址匹配，则产生应答信号。主设备和从设备可根据读/写位判断是发送还是接收数据，并根据应答位的逻辑电平判断是否结束数据传输。在数据传输过程中，SDA仅在SCL为低时发生变化。一旦完成通信，主设备发送STOP停止条件来结束通信：在SCL为高电平时将SDA拉高。

## 4、API列表

| 函数名 | 描述 |
| --- | --- |
| iic\_polling\_init | Master 或 Slave 模式查询方式初始化 |
| iic\_master\_polling\_send | Master 查询方式发送数据 |
| iic\_master\_polling\_recv | Master 查询方式接收数据 |
| iic\_master\_multi\_transmission | Master 进行多条消息 |
| iic\_slave\_polling\_send | Slave 查询方式发送数据 |
| iic\_slave\_polling\_recv | Slave 查询方式接收数据 |
| iic\_interrupt\_init | Master 或 Slave 模式中断方式初始化 |
| iic\_master\_interrupt\_send | Master 中断方式发送数据 |
| iic\_master\_interrupt\_recv | Master 中断方式接收数据 |
| iic\_slave\_interrupt\_send | Slave 中断方式发送数据 |
| iic\_slave\_interrupt\_recv | Slave 中断方式接收数据 |
| i2c\_master\_only\_send | 只发送数据的1条消息 |
| i2c\_master\_send\_recv | 先发送数据再读取数据的2条消息 |
| i2c\_master\_only\_recv | 只读取数据的1条消息 |

---

## 5、使用示例

以下代码为IIC的初始化、读写示例代码：

```
#include "ci130x_iic.h"          /*包含I2C相关接口定义*/
#include "ci130x_system.h"       /*包含I2C寄存器、基地址相关定义*/

/*不带读写位，IIC设备地址左移一位，或上读写位后为0x40，0x41*/
#define IIC_TEST_SLAVE_ADDR  0x20

void i2c_master_test()
{
    /*可自定义 I/O 引脚初始化*/
    pad_config_for_i2c();

    /*master模式初始化，IIC0总线，100K速率，自己的地址设置为0表示master模式，设置为有效值表示slave模式，超时等待时间*/
    iic_polling_init(IIC0,100,0,LONG_TIME_OUT);

    /*往IIC设备地址为0x20的设备，只写入5个字节*/
    char send_buf[5] = {0x01,0x02,0x03,0x04,0x05} ;
    uint8_t last_ack_flag = 0;
    iic_master_polling_send(IIC0,IIC_TEST_SLAVE_ADDR,send_buf,5,&last_ack_flag); 

    /*往IIC设备地址为0x20的设备，只读出5个字节*/
    char recv_buf [5] = {0} ;
    iic_master_polling_recv(IIC0,IIC_TEST_SLAVE_ADDR,recv_buf,5);
}
```

以下代码配置IIC口（PB7/PC0）为开漏输出模式：

```
#include "ci130x_dpmu.h"

void iic_pad_init()
{
    dpmu_set_io_reuse(PB7,THIRD_FUNCTION);              //设置引脚功能复用为IIC
    dpmu_set_io_reuse(PC0,THIRD_FUNCTION);              //设置引脚功能复用为IIC
    dpmu_set_io_open_drain(PB7,ENABLE);                 //配置引脚开漏功能，支持外部上拉5V
    dpmu_set_io_open_drain(PC0,ENABLE);                 //配置引脚开漏功能，支持外部上拉5V
    dpmu_set_io_pull(PB7,DPMU_IO_PULL_DISABLE);         //关闭上拉
    dpmu_set_io_pull(PC0,DPMU_IO_PULL_DISABLE);         //关闭上拉
    dpmu_set_io_direction(PC0,DPMU_IO_DIRECTION_OUTPUT);//SCL配置成输出
}
```

## 6、API 参考