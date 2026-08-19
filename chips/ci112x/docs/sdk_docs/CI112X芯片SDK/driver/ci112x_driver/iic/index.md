<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/driver/ci112x_driver/iic/ -->

# 集成电路总线(IIC)

---

## IIC概述

IIC总线是飞利浦半导体公司开发的，用于芯片之间连接的一种双线双向的串行总线。IIC总线由一根串行数据线（SDA）和一根串行时钟线（SCL）组成。允许多个Master 和多个Slave设备共享总线。任何时刻总线只能由一个主设备控制，当从设备空闲时总线启动数据传输。它包括总线冲突检查和仲裁机制，以保证当多个主机试图控制总线时，数据不会丢失。该总线特别适合于多个器件之间的短距离通信。I²C总线的标准速率为100kbit/s，快速模式为400kbit/s。CI1122仅有一组IIC0。

---

## API列表

| 函数名 | 描述 |
| --- | --- |
| i2c\_master\_init | Master 模式初始化 |
| i2c\_slave\_init | Slave 模式初始化 |
| i2c\_io\_init | I/O 初始化 |
| i2c\_master\_transfer | 消息传输 |
| i2c\_master\_only\_send | 只发送数据 |
| i2c\_master\_send\_recv | 先发送数据再读取数据 |
| i2c\_master\_only\_recv | 只读取数据 |

---

## 使用示例

```
#include "ci112x_i2c.h"          /*包含I2C相关接口定义*/
#include "ci112x_system.h"       /*包含I2C寄存器、基地址相关定义*/
#include "platform_config.h"     /*包含引脚选择接口定义i2c_io_init*/

/*不带读写位，IIC设备地址左移一位，或上读写位后为0x40，0x41*/
#define IIC_TEST_SLAVE_ADDR  0x20

void i2c_master_test()
{
    /*I/O 引脚初始化*/
    i2c_io_init();

    /*master模式初始化，IIC0总线，100K速率，超时等待时间*/
    i2c_master_init(IIC0,100,LONG_TIME_OUT);

    /*往IIC设备地址为0x20的设备，只写入5个字节*/
    char only_send_buf[5] = {0x01,0x02,0x03,0x04,0x05} ;
    i2c_master_only_send(IIC_TEST_SLAVE_ADDR,only_send_buf,5); 

    /*往IIC设备地址为0x20的设备，先写入1个字节，再读出5个字节*/
    char send_recv_buf [10] = {0} ;
    send_recv_buf [0] = 0x01;
    i2c_master_send_recv(IIC_TEST_SLAVE_ADDR,send_recv_buf,1,10);

    /*往IIC设备地址为0x20的设备，只读出5个字节*/
    char only_recv_buf [10] = {0} ;
    i2c_master_only_recv(IIC_TEST_SLAVE_ADDR,only_recv_buf,10);
}
```