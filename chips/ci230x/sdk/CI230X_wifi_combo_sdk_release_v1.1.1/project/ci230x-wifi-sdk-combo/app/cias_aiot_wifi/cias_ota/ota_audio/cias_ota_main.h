#ifndef _CIAS_HTTP_OTA_H_
#define _CIAS_HTTP_OTA_H_

#include "cias_olcp_export.h"
#include "cias_qcloud_main.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define OTA_PER_FRAME_LEN             4096              //OTA升级每一帧包的大小
#define OTA_MIN_PARTITION_SIZE        4096              //最小分区大小(由于flash一次擦除4K，所以4K对齐)
#define OTA_BUFF_LEN                  OTA_PER_FRAME_LEN+4 //包括4byte偏移
#define OTA_GET_HEADER_BUFSZ          1024

#define PARTITION_TABLE1_START_ADDR          0x6000      //分区表起始地址
#define PARTITION_TABLE2_START_ADDR          0x8000      //分区表起始地址

#define USER1_NEED_UPDATE_MASK           0x0001      //需要升级user code 1
#define USER2_NEED_UPDATE_MASK           0x0002      //需要升级user code 2
#define ASR_NEED_UPDATE_MASK             0x0004      //需要升级ASR
#define DNN_NEED_UPDATE_MASK             0x0008      //需要升级DNN
#define VOICE_NEED_UPDATE_MASK           0x0010      //需要升级voice
#define USERFILE_NEED_UPDATE_MASK        0x0020      //需要升级user file
#define INVALID_FILE_MASK                0x8000      //无效的固件文件

//代码分区状态
#define USERCODE_AREA_STA_UPDATE        0xFC    //需要更新(不能使用代码)
#define USERCODE_AREA_STA_OK          	0xF0    //可以使用(正常可以使用代码)
#define USERCODE_AREA_STA_OLD          	0xC0    //之前代码(正常可以使用代码)
//其他分区状态
#define OTHER_AREA_OK                   0x00    //可以使用
#define OTHER_AREA_UPDATE               0xff    //需要更新

//分区命名
#define OTA_ERASE_NAME              "Erase     "      
#define OTA_TABLE1_NAME             "Table1    "
#define OTA_TABLE2_NAME             "Table2    "
#define OTA_CODE1_NAME              "User code1"
#define OTA_CODE2_NAME              "User code2"
#define OTA_ASR_NAME                "ASR       "
#define OTA_DNN_NAME                "DNN       "
#define OTA_VOICE_NAME              "Voice     "
#define OTA_USER_FILE_NAME          "User file "

//ota 状态描述
typedef enum
{
    OTA_IDLE = 0,           //未进行OTA
    OTA_UPDATE_SUCCESS = 1, //更新成功
    OTA_UPDATING = 2,       //正在更新中 
    OTA_IS_LATEST_VERSION=3,//已是最新版本
}cias_ota_statue_t;

typedef struct
{
    char*           name;
    unsigned int    addr;
    unsigned int    size;
    unsigned char * buff;
    int             len;
    int             offset;
    int             need_offset;
    int             need_len;
    unsigned char   timeout;
    unsigned short  crc16;
    unsigned short  recv_crc16;
    unsigned int    file_size;
    unsigned short  file_crc16;
    unsigned char   is_update_done;
    unsigned char   is_recv_crc16;
    unsigned char   http_err;
}updating_partition_t;

#pragma pack(1)
typedef struct
{
    unsigned int version;
    unsigned int address;
    unsigned int size;
    unsigned int crc;
    unsigned char status;
}partition_t;
typedef struct
{
    unsigned int ManufacturerID;
    unsigned int ProductID[2];

    unsigned int HWName[16];
    unsigned int HWVersion;
    unsigned int SWName[16];
    unsigned int SWVersion;

    unsigned int BootLoaderVersion;
    unsigned char Reserve[14];

    partition_t user_code1;
    partition_t user_code2;
    partition_t asr_cmd_model;
    partition_t dnn_model;
    partition_t voice;
    partition_t user_file;

    unsigned int ConsumerDataStartAddr;
    unsigned int ConsumerDataSize;
    
    unsigned short PartitionTableChecksum;
    //unsigned char  ota_statue; //ota状态记录
}partitions_info_t; 
#pragma pack()
typedef struct
{
    cias_olcp_t*            ota_olcp;
    partitions_info_t       file_info;
    partitions_info_t       device_info;
    partitions_info_t*      update_info;
    updating_partition_t    updating;
    unsigned char           is_get_device_info;
    int                     file_size;
    char*                   url;
    void*                   session;
}cias_ota_t;
/**
 * @brief 获取ota的olcp句柄
 * 
 * @retval NULL: error
 * @retval !NULL: olcp句柄
 */
cias_olcp_t* cias_ota_get_olcp_handle(void);
/**
 * @brief 获取ota句柄
 * 
 * @retval ***: ota句柄
 */
cias_ota_t* cias_ota_get_ota_handle(void);
/**
 * @brief ota主函数，通过此函数进行OTA升级
 * 
 * @param hal_send: 硬件通信发送函数
 * @param uri: 固件url
 * @retval -1: error
 * @retval 0: successful
 */
#ifdef __cplusplus
}
#endif
#endif
