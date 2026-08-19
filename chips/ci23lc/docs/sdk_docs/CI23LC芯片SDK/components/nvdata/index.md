<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI23LC%E8%8A%AF%E7%89%87SDK/components/nvdata/ -->

# 用户数据管理(NV DATA)

---

## 1. 概述

NV(non-volatile)data管理模块是用于数据保存到flash器件,其特性包括：

* 能够支持数据块独立保存，并且每个数据块大小可自定义，使用ID（32bit数据）的方式区分数据块；
* 方便移植，将RTOS和flash相关操作的函数独立在单独文件内；
* 使用hotid（频繁操作的ID）功能，在节约RAM空间使用的同时，减少对flash的操作；
* 对数据的写入做充分校验，返回错误值，需应用层做重试；
* 对意外掉电进行数据恢复，可以使用上次的数据；
* 接口包含初始化、读数据、写数据、删除数据、注册hotid；
* NVdata的flash擦除块头标记使用4个字节，最低字节可以作为版本号使用，如果版本不同，则会执行对NVdata区域擦除操作；
* 对数据内容进行校验保存，尽量保证数据的正确性。

---

## 2. 使用说明

该组件主要包括以下用户接口：

| 函数接口 | 说明 |
| --- | --- |
| cinv\_init | 模块初始化 |
| cinv\_item\_init | 初始化item |
| cinv\_item\_read | 读出item数据 |
| cinv\_item\_write | 写入item数据 |
| cinv\_item\_delete | 删除item数据 |
| cinv\_register\_hotid | 注册为hotid |

使用示例：

```
/*定义需要保存data的ID*/
#define NVDATA_ID_VOLUME                0x50000001 /*'VOLU'*/

void nvdata_test(void *p_arg)
{   
    uint16_t real_len;   

    uint8_t volume = 0x5;

    /* 将NVDATA_ID_VOLUME注册到hotid，使用该方式会加快读取，
    但会增加内存使用。对速度要求不高时，可不写该语句 */
    cinv_register_hotid(NVDATA_ID_VOLUME);

    /* 创建数值型的item，并赋初值0x5，返回值可以判断是否为第一次写入 */
    if(CINV_ITEM_UNINIT == cinv_item_init(NVDATA_ID_VOLUME, sizeof(volume), &volume))
    {
        mprintf("first write\n");
    }

    /* 读取item值 */
    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_VOLUME, sizeof(volume), &volume, &real_len))
    {
        mprintf("read error\n");
        while(1);
    }
    else
    {
        mprintf("volume = 0x%x, real_len is %d\n", volume, real_len);
        volume ++;
    }

    /* 更新（写入）该item值 */
    if(CINV_OPER_SUCCESS != cinv_item_write(NVDATA_ID_VOLUME,real_len, &volume))
    {
        mprintf("write error\n");
        while(1);
    }
}
```

提示

NVDATA\_ID\_VOLUME宏需要自己定义，这是item的唯一ID。如果使用注册ID到hotid的方式，会额外需要一定内存，但从flash内读取写入该ID数据时会速度更快，可根据实际应用需求来添加。