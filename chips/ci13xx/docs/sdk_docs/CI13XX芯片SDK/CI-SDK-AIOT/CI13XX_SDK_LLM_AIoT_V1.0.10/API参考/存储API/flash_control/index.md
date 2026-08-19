<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/flash_control/ -->

# 存储控制(FLASH CTL)

---

## 1. 概述

在CI13XX系列芯片中，flash用于多种功能，包括固件存储、用户数据存储，通常情况下，用户数据存储应使用NVdata组件相关API完成，如需在开发调试过程中快速读写flash数据以及开发bootload程序时，则可能需要直接读写flash，但由于flash器件被内核和dnn硬件单元同时访问，故不能直接读写flash，因此这里提供了FLASH控制读写接口，通过这组接口可以安全可靠的获取总线控制，进而读写flash。

---

## 2. 使用说明

| 函数接口 | 说明 |
| --- | --- |
| post\_write\_flash | 请求写flash |
| post\_read\_flash | 请求读flash |
| post\_erase\_flash | 请求擦除flash |

警告

一般情况下使用post\_write\_flash、post\_read\_flash、post\_erase\_flash接口进行操作即可完成flash数据相关需求。切记如需保存用户nv数据请使用NVdata组件相关API而非本组件。

## 3. 使用示例（1）

```
#include "flash_manage_outside_port.h"

#define WAVE_ADDR  0x1000 //flash偏移地址
char wav_format_head[44]; //读数据缓存

void read_flash_test(void)
{
    /* 读flash0x1000地址44个字节到wav_format_head内 */
    post_read_flash(wav_format_head, WAVE_ADDR, sizeof(wav_format_head));
}
```

## 4. 使用示例（2）

下面详细介绍，如何添加、打包、并读取FLASH中文件内容。

* （1）、在工程目录（SDK\projects\sample\_xxx\firmware\user\_file\）下新建文件，例如（[0]文本文件.txt），该文件内容为“123456789”。

!! note “注意”
文件名称不做限制，但文件名前需标注[ID]，只要不和[60000]的ID重名，该ID供程序读取使用。

![图-1](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash_control-1.png)

* （2）、双击运行工程目录（SDK\projects\sample\_xxx\firmware）下的（合成分区bin文件.bat），将文件打包到user\_file.bin中
* （3）、程序读文件内容程序示例如下：

```
#include "ci_flash_data_info.h"
#include "flash_manage_outside_port.h"

char value[512] = {0}; //读取内容缓存

void read_txt_file(void)
{
    uint32_t tabaddr1 = 0; //文件在FLASH中的偏移地址
    uint32_t tabsize1 = 0; //文件的大小

    //用get_userfile_addr读取ID为0的文件，在FLASH中的偏移地址和文件大小。
    if(get_userfile_addr(0, &tabaddr1, &tabsize1) != 0)
    {
        ci_loginfo(LOG_USER,"failed!\n");
    }

    ci_loginfo(LOG_USER,"file addr[%x],file size[%x]!\n",tabaddr1,tabsize1);

    //再传入ID为0的文件在FLASH中偏移地址和文件大小，读取文件内容。
    post_read_flash(value, tabaddr1, tabsize1);
    ci_loginfo(LOG_USER,"file value[%s]",value);
}
```

* (4)、运行结果如下：

```
file addr[376020],file size[9]!
file value[123456789]
```

## API 参考