<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/driver/ci110x_driver/iic/ -->

# 集成电路总线(IIC)

---

## IIC概述

IIC总线是飞利浦半导体公司开发的，用于芯片之间连接的一种双线双向的串行总线。IIC总线由一根串行数据线（SDA）和一根串行时钟线（SCL）组成。允许多个Master 和多个Slave设备共享总线。任何时刻总线只能由一个主设备控制，当从设备空闲时总线启动数据传输。它包括总线冲突检查和仲裁机制，以保证当多个主机试图控制总线时，数据不会丢失。该总线特别适合于多个器件之间的短距离通信。I²C总线的标准速率为100kbit/s，快速模式为400kbit/s。

---

## API列表

| 函数名 | 描述 |
| --- | --- |
| i2c\_init | Master 模式初始化 |
| i2c\_slave\_init | Slave 模式初始化 |
| i2c\_io\_init | I/O 初始化 |
| i2c\_master\_transfer | 消息传输 |
| i2c\_master\_send | 发送数据 |
| i2c\_master\_recv | 接收数据 |

---

## 使用示例

```
 /**
  * @ 以slave设备es8388为例子列出接口，用户
  * @ 可根据需求定义函数名称、参数及内部操作。
  */
#include "ci110x_i2c.h"          /*包含I2C相关接口定义*/
#include "ci110x_system.h"       /*包含I2C寄存器、基地址相关定义*/
#include "platform_config.h"     /*包含引脚选择接口定义i2c_io_init*/
#include "string.h"              /*包含strcpy接口定义*/
#define ES8388_I2C_ADDR  0x10    /*SLAVE设备IIC总线地址*/

/**
  * @功能： es8388读单个字节
  * @参数： reg 寄存器地址
  * @返回值：读出来的值
  */
uint8_t es8388_read_data(uint8_t reg)      
{
    int8_t buf[2] = {0};                 
    struct i2c_client client = {0};      
    client.I2Cx = IIC1;                  
    client.addr = ES8388_I2C_ADDR;       
    strcpy(client.name,"es8388");      
    buf[0] = reg;                        
    i2c_master_recv(&client,buf,1,1);    /*调用i2c接口发送1字节寄存器，读出1字节数据*/
    return (uint8_t)buf[0];              
}

/**
  * @功能：es8388读单个字节
  * @参数：reg 寄存器地址
  * @     val 写进去的值
  */
int32_t es8388_write_data(uint8_t reg,uint8_t val)
{
    int8_t buf[2] = {0};
    struct i2c_client client = {0}; 
    client.I2Cx = IIC1;
    client.addr = ES8388_I2C_ADDR;
    strcpy(client.name,"es8388");
    buf[0] = reg;
    buf[1] = val;
    i2c_master_send(&client,buf,2);    /*发送1字节寄存器地址，发送1字节数据*/
    return 0;
}

void i2c_test()
{
    /*单字节值缓存*/
    uint8_t val;          

    /*I/O 引脚初始化，参数含义请查看platform_config.c中该接口注释*/
    i2c_io_init(I2C1_SCL_PAD,I2C1_SDA_PAD);
    i2c_init(IIC1,100,0X5FFFFF);

    /*寄存器地址：0x11*/
    /*写入的值：0x55*/
    val = es8388_read_data(0x11);
    es8388_write_data(0x11,0x55);
}
```

注意：更多示例可参考SDK下文件components\example\ci110x\_i2c\_test.c 。