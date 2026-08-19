<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/spiflash/ -->

# 串行闪存控制器(SPIFLASH)

---

## 简介

SPIFLASH控制器为本公司自行开发的模块（qspi\_controller），所以与市面上常见的SPI控制器有所不同；有很多独有的功能：支持AMBA 2.0协议，APB配置和AHB数据传输，只支持32bit数据宽度，支持DMA方式数据传输，支持CPU boot传输，以SINGLE模式传输，支持SINGLE INCR INCR4 INCR8 INCR16传输方式，两条AHB数据总线，支持PARA参数预取操作及普通操作，支持M值的配置，深度为64，宽度为32bit的发送与接收FIFO，支持写、擦除地址保护，支持管脚复位FLASH，支持AHB和AHB的总线检测AHB0端，支持CPU boot传输，以SINGLE模式传输，支持M值的配置，深度为64，宽度为32bit的发送与接收FIFO，支持水线可配，只支持32bit数据宽度，支持DMA方式数据传输（读写传输都支持），DMA支持single和burst请求，支持SINGLE/INCR/INCR4/INCR8/INCR16传输方式，除boot模式外，每次读写操作前都需要通过APB端口配置传输模式、传输数据大小，传输数据起始地址等等。

---

## API列表

| 函数名 | 描述 |
| --- | --- |
| flash\_init | 初始化SPIFLASH设备 |
| spic\_read\_unique\_id | SPIFLASH设备读取UNIQUE ID |
| spic\_read\_jedec\_id | SPIFLASH设备读取JEDEC ID |
| spic\_erase\_security\_reg | SPIFLASH设备擦除安全寄存器 |
| spic\_write\_security\_reg | SPIFLASH设备写安全寄存器 |
| spic\_read\_security\_reg | SPIFLASH设备读安全寄存器 |
| spic\_security\_reg\_lock | SPIFLASH设备锁定安全寄存器 |
| flash\_erase | SPIFLASH擦除 |
| flash\_write | SPIFLASH写 |
| flash\_read | SPIFLASH读 |
| spic\_quad\_mode | SPIFLASH设置四线模式 |

---

## 示例

以下示例初始化FLASH并检查模式，最后进行了擦除读写操作：

```
#include "ci130x_spiflash.h"
#include "ci_log.h"

uint8_t write_buf[1024] = {0}; /* 写入FLASH的数据缓存 */
uint8_t read_buf[1024] = {0};  /* 读出FLASH数据的缓存 */

void flash_test()
{
    uint32_t flash_addr = 0x10000; /*FLASH偏移地址*/
    uint32_t size = 256; /* 读写数据大小 */

    for(int i=0; i<size; i++)
    {
        write_buf[i] = i;
        read_buf[i] = 0;
    }

    /* FLASH 初始化*/
    if(RETURN_OK != flash_init(QSPI0))
    {
        mprintf("flash init error !!!\n");
        while(1);
    }
    /* FLASH 擦除4K*/
    if(RETURN_OK != flash_erase(QSPI0,flash_addr,(4 * 1024)))
    {
        mprintf("flash erase error !!!\n");
        while(1);
    }
    /* FLASH 写入数据*/
    if(RETURN_OK != flash_write(QSPI0,flash_addr,write_buf,size))
    {
        mprintf("flash write error !!!\n");
        while(1);
    }
    /* FLASH 读出数据*/
    if(RETURN_OK != flash_read(QSPI0,read_buf,flash_addr,size))
    {
        mprintf("flash read error !!!\n");
        while(1);
    }
    /* 比较写入和读出的数据*/
    if(memcmp(write_buf, read_buf, size))
    {
        mprintf("cmp error !!!\n");
        while(1);
    }
    else
    {
        //比较正确
    }
}
```

## API 参考