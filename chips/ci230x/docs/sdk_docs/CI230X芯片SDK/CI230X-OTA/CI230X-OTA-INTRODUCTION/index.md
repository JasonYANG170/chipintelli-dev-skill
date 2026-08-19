<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/CI230X-OTA-INTRODUCTION/ -->

# 启英泰伦-三代芯片OTA

## 1. OTA功能概述

CI230X系列芯片OTA升级功能分为语音部分升级和WIFI部分升级，升级固件可以从云端获取，也可以从本地加载通过串口进行OTA升级，语音部分升级功能同样适用于CI13XX系列芯片，OTA升级代码可参考启英泰伦发布的CI230X系列芯片SDK:ci230x\_wifi\_combo\_sdk\_release，(如果使用第三方WIFI芯片对CI13XX系列芯片进行OTA，下面流程可供参考)：

## 2. CI230X系列芯片语音端OTA

### 2.1. 升级交互流程图

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/img/CI230X-OTA-INTRODUCTION-4.png)

图 2-1

### 2.2 打包工具使用说明

* 升级代理程序updater：ci130x\_updater.bin。升级代理程序已默认使用芯片的UART1作为升级串口，若有特殊情况需要更改使用其他串口（如UART0或UART2），请联系启英泰伦技术支持！
* OTA语音固件打包升级工具：使用\tools目录下PACK\_UPDATE\_TOOL355\_FOR\_OTA.exe工具打包，打包时请注意选中对应芯片型号后，右侧固件格式选项选择“FW\_V1”再打包固件，如下图所示：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/img/CI230X-OTA-INTRODUCTION-3.png)

图 2-2

### 2.3 WIFI端核心代码说明

1. CMakeList.txt（SDK/project/ci230x-wifi-sdk-combo/）文件中使能语音芯片OTA宏CIAS\_AIOT\_AUDIO\_OTA\_ENABLE以及需使用云平台的对应的宏
2. 按照如下配置顺序进行OTA初始化：

```
    cias_ota_callback_init();                            //init ota callback func
    cias_get_cloud_data_main();                          //start firmware-get task
    get_cias_ota_handle()->cias_ota_task_delay_ms(2);     //delay few milliseconds
    get_cias_ota_handle()->cias_net_data_struct_init();  //Download the data package structure initialization
    cias_ota_get_partitions_info();                      //Get partition table information
    cias_ota_progress_task();                            //start firamware-update task
```

**注意：g\_cias\_system\_manage\_param.ota\_start\_flag = 1 为ota升级任务启动标志，升级固件准备就绪开始升级时需要将ota\_start\_flag置1**

3. 如上所述要完成升级流程，WIFI端需要实现如下三个步骤：

* 需在cias\_ota\_callback\_init()中注册钩子函数，需要实现的接口如下所示：

```
typedef struct 
{
    /*audio升级固件获取，参数说明：1-存升级buf，2-距固件头偏移地址，3-需获取的大小*/
    int (*cias_get_audio_image) (unsigned char *, unsigned int, unsigned int);
    /*wifi芯片复位*/
    void (*cias_wifi_device_reboot) (void);
    /*audio芯片复位*/
    void (*cias_audio_device_reboot) (void);
    /*升级串口波特率更改*/
    void (*cias_communication_rate_change) (unsigned long);
    /*升级串口发送*/
    int (*cias_ota_communication_send) (char *, int);
    /*升级串口接收*/
    int (*cias_ota_communication_recv) (char *, int);
    /*延时(单位：ms)*/
    void (*cias_ota_task_delay_ms) (unsigned int);
    /*flash读取
    @param offset Starting offset from the beginning of the flash device.
    @param length Size of the portion of flash to read.
    @param buffer Pointer on data to read.
    @return uint8_t 0 if operation can start successfully.
    */
    unsigned char (*cias_ota_flash_read) (unsigned int offset, unsigned int length, unsigned char *buffer);
    /*网络通信协议等 初始化*/
    void (*cias_net_portocol_init) (void);
    /*ota网络端下载数据结构初始化*/
    int (*cias_net_data_struct_init) (void);
    /*上报升级状态到云端回调*/
    uint8_t (*cias_report_ota_status_to_iot)(uint8_t);
}cias_ota_handle_t;
```

* cias\_get\_cloud\_data\_main创建云端固件包获取任务，等待指令开始获取数据包。
* 延时2ms后ci\_ota\_progress\_task()创建ota串口传输任务，在任务中初始化云端数据包结构cias\_net\_data\_struct\_init并获取云端固件分区表信息cias\_ota\_get\_partitions\_info，获取成功后进入update\_ota\_main()开始和audio端bootloader通信传输updater并校验，再和updater通信传输待升级固件包。

---

## 3. CI230X系列芯片WIFI端OTA说明

### 3.1 使用配置

CMakeList.txt（SDK/project/ci230x-wifi-sdk-combo/）文件中使能wifi芯片OTA宏CIAS\_AIOT\_WIFI\_OTA\_ENABLE以及需使用云平台的对应的宏

### 3.2 核心代码接口说明

1. ota\_port\_init()初始化wifi端flash操作接口
2. cias\_wifi\_ota\_callback\_init()初始化函数中注册所需的回调函数：

```
typedef struct 
{
    /*从云端获取wifi升级数据*/
    int (*cias_get_wifi_image_from_cloud)(unsigned char *, unsigned int, unsigned int len);
    /*上报升级状态到云端回调*/
    uint8_t (*cias_report_ota_status_to_iot)(uint8_t);
}cias_wifi_ota_handle_t;
```

3. cias\_wifi\_ota\_main\_task()创建wifi端ota主任务，等待升级任务使能（收到平台下发wifi端ota指令后，将g\_cias\_system\_manage\_param.wifi\_ota\_start\_flag置1即使能）
4. 升级任务使能后，初始化看门狗wifi\_ota\_watch\_dog()，删除不需要运行的任务并创建cias\_wifi\_ota\_get\_cloud\_data()云端固件包获取任务，延时2ms后创建cias\_wifi\_ota\_progress\_task()任务

---

## 4. CI13XX系列芯片离线SDK配置OTA功能指南

#### 1.system/sdk\_default\_config.h

定义宏CIAS\_FLASH\_IMAGE\_UPGRADE\_ENABLE为1，使能sdk的ota功能

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/img/CI230X-OTA-INTRODUCTION-5.png)

图 2-3

#### 2.components\flash\_control\flash\_control\_inc\ci\_flash\_data\_info.h

包含sdk\_default\_config.h头文件，通过宏CIAS\_FLASH\_IMAGE\_UPGRADE\_ENABLE选择分区表起始地址

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/img/CI230X-OTA-INTRODUCTION-6.png)

图 2-4

#### 3.components\flash\_control\flash\_control\_src\ci\_flash\_data\_info.c

获取分区表校验和函数中，使能宏CIAS\_FLASH\_IMAGE\_UPGRADE\_ENABLE时，修改user\_code2\_status状态为0XF0

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/img/CI230X-OTA-INTRODUCTION-7.png)

图 2-5

#### 4.components文件夹下新建ota文件夹，如下图所示：

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/img/CI230X-OTA-INTRODUCTION-8.png)

图 2-6

#### 并在该文件夹下创建ci\_ota\_port.c和ci\_ota\_port.h

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/img/CI230X-OTA-INTRODUCTION-9.png)

图 2-7

#### 5.ci\_ota\_port.c

该文件中配置串口中断并创建接收队列和解包任务来接收复位指令

```
#include "ci_ota_port.h"
#include "FreeRTOS.h" 
#include "task.h"
#include "queue.h"
#include "ci130x_system.h"
#include "ci_log.h"

#if CIAS_FLASH_IMAGE_UPGRADE_ENABLE
volatile int irq_disable = 0;
static QueueHandle_t msg_recv_queue = NULL;

void ota_port_recv_queue_init(void)
{
    msg_recv_queue = xQueueCreate(1024*2, sizeof(int8_t));
    if (msg_recv_queue == NULL)
    {
        mprintf("msg_recv_queue create fail\r\n");
    }
    mprintf("msg_recv_queue create success\r\n");
}

int8_t uart_temp = 0;
void UART1_IRQHandler(void)
{
    if ((OTA_CMD_PORT->UARTMIS & (1UL << UART_RXInt)) || (OTA_CMD_PORT->UARTMIS & (1UL << UART_RXTimeoutInt)))
    {
        while (!(OTA_CMD_PORT->UARTFlag & (0x1 << 6))) // read fifo have data
        {
            uart_temp= (int8_t)UartPollingReceiveData(OTA_CMD_PORT);
            if(!irq_disable)
                xQueueSendFromISR(msg_recv_queue, &uart_temp, pdFALSE);
        }
        UART_IntClear(OTA_CMD_PORT, UART_RXInt);
    }
    OTA_CMD_PORT->UARTICR = 0xFFF;
}

void ota_recv_cmd_task(void *parameter)
{
    static int32_t recv_package_length = 0;
    static int32_t read_start_flag = 0;
    static int32_t curr_net_player_cache = 0;
    int32_t package_length = 0;
    int8_t  msg_state = NET_MSG_IDE;
    BaseType_t err;
    cias_data_standard_head_t *data_header;
    uint32_t cache_empty_num = 0;
    int8_t package_data[1050] = {0};
    int msg_end = 0;

    while (1)
    {
        package_length = 0;
        while(pdPASS == xQueueReceive(msg_recv_queue, &package_data[package_length], pdMS_TO_TICKS(50)))
        {
            package_length++;
            if(msg_state == NET_MSG_IDE) 
                msg_state = NET_MSG_HEAD;
            else if ((msg_state == NET_MSG_HEAD) && (package_length > (sizeof(cias_data_standard_head_t)-2)))
            {
                data_header = (cias_data_standard_head_t *)package_data;
                if(data_header->magic == 0x5a5aa5a5)
                    msg_state = NET_MSG_DATE; 
                else
                    msg_state = NET_MSG_ERR;
            }
            else if(msg_state == NET_MSG_DATE && package_length > (data_header->len + sizeof(cias_data_standard_head_t) - 1))
            {
                data_header = (cias_data_standard_head_t *)package_data;
                recv_package_length = data_header->len + sizeof(cias_data_standard_head_t);                   
                if(data_header->type == 0x0501)
                {
                    irq_disable = 1;
                    dpmu_software_reset_system_config();
                }
                package_length = 0;
                memset(package_data,0,1050);
                msg_state = NET_MSG_IDE; 
            }
            if(msg_state == NET_MSG_ERR)
            {
                ci_logdebug(LOG_USER, "ci_standard_head_t error\r\n");
                package_length = 0;
                memset(package_data,0,1050);
                msg_state = NET_MSG_IDE; 
            }
        }
        if(msg_state != NET_MSG_IDE)
        {
            ci_logdebug(LOG_USER, "recv data timeout error(%d)\r\n",package_length);
            package_length = 0;
            memset(package_data, 0, 1050);
            msg_state = NET_MSG_IDE; 
        }
    }
}

void ci_ota_init(void)
{
    UARTInterruptConfig(OTA_CMD_PORT, UART_BaudRate115200);
    ota_port_recv_queue_init();
    xTaskCreate(ota_recv_cmd_task, "IOT_recv", 256, NULL, 4, NULL);
}
#endif//CIAS_FLASH_IMAGE_UPGRADE_ENABLE
```

#### 6.ci\_ota\_port.h

```
#ifndef __CI_OTA_PORT_H__
#define __CI_OTA_PORT_H__

#include "sdk_default_config.h"
#include "ci130x_uart.h"

#define OTA_CMD_PORT    UART1
// #define OTA_CMD_PORT    1
#if CIAS_FLASH_IMAGE_UPGRADE_ENABLE
typedef enum
{
    NET_MSG_HEAD = 1,
    NET_MSG_DATE = 2,
    NET_MSG_ERR  = 0,
    NET_MSG_IDE  = 3,
}wifi_communicate_state_t;

typedef struct ci_uart_standard_head
{
    uint32_t magic;     /*帧头 定义为0x5a5aa5a5*/
    uint16_t checksum;  /*校验和*/
    uint16_t type;      /*命令类型*/
    uint16_t len;       /*数据有效长度*/
    uint16_t version;   /*版本信息*/
    uint32_t fill_data; /*填充数据，可以添加私有信息*/
}cias_data_standard_head_t;

//ota recv uart cmd init
void ci_ota_init(void);

#endif

#endif//__CI_OTA_PORT_H__
```

#### 7.projects\offline\_asr\_sample\project\_file\source\_file.prj

添加ci\_ota\_port.c文件编译路径

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/img/CI230X-OTA-INTRODUCTION-10.png)

图 2-8

#### 8.projects\offline\_asr\_sample\src\main.c

通过宏CIAS\_FLASH\_IMAGE\_UPGRADE\_ENABLE选择是否包含ci\_ota\_port.h头文件和调用ci\_ota\_init()初始化

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/CI230X-OTA/img/CI230X-OTA-INTRODUCTION-11.png)

图 2-9