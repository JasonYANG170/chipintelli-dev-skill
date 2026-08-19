<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/driver/ci110x_driver/spiflash/ -->

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
| dnn\_mode\_config | SPIFLASH配置DNN模式 |
| flash\_dnn\_mode | SPIFLASH打开（关闭）DNN模式 |
| flash\_check\_mode | SPIFLASH检测模式 |
| spic\_quad\_mode | SPIFLASH设置四线模式 |

---

## 示例

以下示例初始化FLASH并检查模式，最后进行了擦除读写操作：

```
/* QFLASH0 初始化*/
if(RETURN_OK !=  flash_init(FLASH_SPI_PORT))
{
    while(1);
}
if(1 == flash_check_mode(FLASH_SPI_PORT))
{
    mprintf("flash current mode is quad!!!\n");
}
else
{
    mprintf("flash current mode is stand!!!\n");
}
if(RETURN_OK != flash_erase(FLASH_SPI_PORT,flash_addr,flash_size))
{
    mprintf("flash erase error !!!\n");
    while(1);
}
if(RETURN_OK != flash_write(FLASH_SPI_PORT,flash_addr+offset,(unsigned int)&rev_msg->data[4],rev_msg->length-4))
{
    mprintf("flash write error !!!\n");
    while(1);
}
if(RETURN_OK != flash_read(FLASH_SPI_PORT,(unsigned int)read_buf_for_crc,flash_addr+offset,rev_msg->length-4))
{
    mprintf("flash read error !!!\n");
    while(1);
}
```