<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/iic/ -->

# 集成电路总线(IIC)

---

## 1、IIC概述

IIC总线是飞利浦半导体公司开发的，用于芯片之间连接的一种双线双向的串行总线。IIC总线由一根串行数据线（SDA）和一根串行时钟线（SCL）组成。允许多个Master 和多个Slave设备共享总线。任何时刻总线只能由一个主设备控制，当从设备空闲时总线启动数据传输。它包括总线冲突检查和仲裁机制，以保证当多个主机试图控制总线时，数据不会丢失。该总线特别适合于多个器件之间的短距离通信。I²C总线的标准速率为100kbit/s，快速模式为400kbit/s。CI23LC仅有一组IIC0。

---

## 2、IIC时序

### 2.1、基础简介

* SDA（串行数据线，双向 I/O 线）
* SCL （串行时钟线，由master提供）
* 可编程器件既可作为 master 也可作为 slave
* master（主机，start信号传送，时钟产生者，stop信号传送）
* slave（从机，被寻址的器件均被认为是从器件）
* ACK （接收到数据）
* NACK（未接收到数据、或告诉发送者停止发送）

### 2.2、I2C总线物理拓扑图

* 总线可连接多个器件，每个连到总线的器件都有唯一的地址，同一时刻只允许有一个master发起请求。

![IIC波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/img/iic-1.png)

### 2.3、时序图分析

* （1）、master往slave写数据（传输方向不变）

![IIC波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/img/iic-2.png)

* （2）、master往slave写数据，再从slave读回数据（传输方向改变）

![IIC波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/img/iic-3.png)

* （3）、master从slave读数据（传输方向不变）

![IIC波形图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/driver/ci23lc_driver/img/iic-4.png)

---

## 3、API列表

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

## 4、使用示例

```
#include "ci13lc_iic.h"          /*包含I2C相关接口定义*/
#include "ci13lc_system.h"       /*包含I2C寄存器、基地址相关定义*/

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