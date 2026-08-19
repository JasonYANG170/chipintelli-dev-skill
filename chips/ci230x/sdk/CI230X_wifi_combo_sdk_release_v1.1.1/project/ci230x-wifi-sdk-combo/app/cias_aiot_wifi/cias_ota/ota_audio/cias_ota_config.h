#ifndef __CIAS_OTA_CONFIG_H__
#define __CIAS_OTA_CONFIG_H__
#include "cias_log.h"
#include "update.h"
#include "reboot_trace.h"
#include "cias_ln_hardware.h"
#include "cias_communication.h"
#include "cias_olcp.h"
#include "cias_qcloud_ota.h"
#include "cias_olcp_export.h"
#include "task.h"
#include "flash_partition_table.h"
#include "cias_qcloud_main.h"
#include "cias_system_manage.h"

/****************************************************************************************
audio ota function right start step
{
    *1-cias_ota_callback_init();                            //init ota callback func
    *2-cias_get_cloud_data_main();                          //start firmware-get task
    *3-get_cias_ota_handle()->cias_ota_task_delay_ms(2)     //delay few milliseconds
    *4-get_cias_ota_handle()->cias_net_data_struct_init();  //Download the data package structure initialization
    *5-cias_ota_get_partitions_info();                      //Get partition table information
    *6-cias_ota_main();                                     //start firamware-update task
}

CI2305 SDK示例注意：g_cias_system_manage_param.ota_start_flag = 1;为ota升级任务启动标志，（网络、平台等）初始化后在升级开始前将ota_start_flag置1
*****************************************************************************************/
#define AUDIO_OTA_TASK_NAME                 "cias_ota_main"        //ota总任务名称 
#define AUDIO_OTA_PROGRESS_TASK_NAME        "cias_ota_main_task"   //ota升级任务名称 
#define AUDIO_OTA_GET_FIRMWARE_TASK_NAME    "cias_ota_get_task"    //ota固件获取任务名称 
#define AUDIO_OTA_TASK_SIZE                 1024*6                 //任务空间(6kb)
#define AUDIO_OTA_TASK_PRIORITY             4                      //任务优先级

#define UPDATER_FLASH_START_ADDR             (USER_SPACE_OFFSET + USER_SPACE_SIZE - 20*1024)//updater在falsh中的存储起始地址,这里设置为user分区的后20K

//debug
#define DEBUG_LOG       1
#define SUBMIT_TEST     1

typedef enum{
    AUDIO_NO_ANSWER = 1,
    UPDATER_INFO_SEND_ERROR,
    UPDATER_SEND_FAIL,
    UPDATER_VERIFY_FAIL,
    AUDIO_BAUD_SET_FAIL,
    AUDIO_NO_ANSWER_AFTER_CHANGE_BAUD,
    SET_AUDIO_FIRMWARE_VER_FAIL,
    MALLOC_FAIL,
    READ_AUDIO_PARTITION_FAIL,
    AUDIO_PARTITION_INVALID,
    PARTITION_1_UPDATE_FAIL,
    PARTITION_2_UPDATE_FAIL,
    USER_CODE_1_UPDATE_FAIL,
    USER_CODE_2_UPDATE_FAIL,
    ASR_UPDATE_FAIL,
    DNN_UPDATE_FAIL,
    VOICE_UPDATE_FAIL,
    USERFILE_UPDATE_FAIL = 18,
    NO_PARTITION_NEED_UPDATE = 19
} report_ota_status_t;

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
cias_ota_handle_t *get_cias_ota_handle(void);
int cias_ota_callback_init(void);
void cias_net_protocol_init(void);
int cias_get_cloud_data_main(void);
int cias_ota_main(void);

#endif