<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/FatFS/ -->

# 文件系统(FATFS)

---

## 1. 概述

FatFS是用于小型嵌入式系统的通用FAT/exFAY文件系统模块。FatFS模块的编写符合ANSI（C89），并与磁盘I/O层完全分离。目前在CI110X系列芯片SDK中已集成了FatFS R0.09b版本，其中磁盘I/O层对接了SDC接口和SPI1接口，使用时无需关系硬件直接通过文件系统函数即可完成对sd卡的操作。

提示

更多FatFS信息请查看 ☞ [FatFS官网](http://elm-chan.org/fsw/ff/00index_e.html)  获取帮助。

---

## 2. 使用说明

在SDK中定义了SDC接口磁盘盘符为0:，SPI1接口的磁盘盘符为1:，使用时首先应挂载磁盘：

```
f_mount(FS_SD, &fs_sdc);  //挂载SDC接口上的SD卡
f_mount(FS_SPI, &fs_spi); //挂载SPI1接口上的SD卡
```

其他常用文件接口说明及示例：

| 函数接口 | 说明 |
| --- | --- |
| f\_open | 打开文件 |
| f\_read | 读文件 |
| f\_write | 写文件 |
| f\_lseek | 移动文件指针 |
| f\_close | 关闭文件 |

```
// 打开磁盘0根目录的config.txt文件
res = f_open(&fp,"0:/config.txt",FA_READ);
ci_logdebug(LOG_TEST_APP,"res is %d\n",res);

//读取256字节文件数据到buff
res = f_read(&fp, buff,256,&lr);
ci_logdebug(LOG_TEST_APP,"res is %d\n",res);

//关闭文件
res = f_close(&fp);
ci_logdebug(LOG_TEST_APP,"res is %d\n",res);

// 打开并创建磁盘0根目录的test.txt文件
res = f_open(&fp,"0:/test.txt",FA_CREATE_ALWAYS|FA_WRITE);
ci_logdebug(LOG_TEST_APP,"res is %d\n",res);

//写入buff中1024字节数据到文件
res = f_write(&fp, buff,1024,&br);
ci_logdebug(LOG_TEST_APP,"res is %d\n",res);

//关闭文件
res = f_close(&fp);
ci_logdebug(LOG_TEST_APP,"res is %d\n",res);
```