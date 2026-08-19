#ifndef __CIAS_FLASH_UPDATE_H__
#define __CIAS_FLASH_UPDATE_H__
#include <stdint.h>

typedef enum
{
  FLASH_PROTECT_NONE,
  FLASH_PROTECT_ALL,
  FLASH_PROTECT_HALF,
  FLASH_UNPROTECT_LAST_BLOCK
} PROTECT_TYPE;

/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum
{
  BK_PARTITION_BOOTLOADER = 0,
  BK_PARTITION_AUTHENTICATION,
  BK_PARTITION_APPLICATION,
  BK_PARTITION_FACTORY_PARAM,     //and by yjd
  BK_PARTITION_IOT_UPDATE_PARAM,  //and by yjd
  BK_PARTITION_RF_FIRMWARE,
  BK_PARTITION_NET_PARAM,
  BK_PARTITION_USR_CONFIG,
  BK_PARTITION_MAX,
} bk_partition_t;


typedef enum 
{
  WIFI_IMG_UPDATE,
  AUDIO_IMG_UPDATE,
  BT_IMG_UPDATE,
  ZIGBEE_IMG_UPDATE,
}IMG_UPDATE_TYPEDEF;
typedef enum
{
  FIRMWARE_UPDATE_ING = 0x0c,          //audio升级中
  FIRMWARE_UPDATE_SUCCESSFUL = 0x0e,   //audio升级成功
  FIRMWARE_UPDATE_FAIL = 0x0f,         //audio升级失败
}FIRWARE_UPDATE_STATUS_TYPEDEF;
typedef enum
{
  AUDIO_PARTITION_PRIMARY_CONFIG,
  AUDIO_PARTITION_BACKUP_CONFIG,
  AUDIO_PARTITION_PRIMARY_CODE,
  AUDIO_PARTITION_BACKUP_CODE,
  AUDIO_PARTITION_ASR,
  AUDIO_PARTITION_DNN,
  AUDIO_PARTITION_VOICE,
  AUDIO_PARTITION_USERFILE,
  WIFI_PARTITION_PRIMARY_CONFIG,
  WIFI_PARTITION_BACKUP_CONFIG,
  WIFI_PARTITION_PRIMARY_CODE,
  WIFI_PARTITION_BACKUP_CODE,
}UPDATE_PARTITION_UPDATE_INDEX;
#define IMG_NET_ADDRESS_MAX_LEN 512 
typedef struct iot_img_net_address
{
  uint32_t img_data_len;
  int8_t   img_data[IMG_NET_ADDRESS_MAX_LEN];
}iot_img_net_address_st;   //iot升级地址信息

typedef struct update_result_param_info
{
    uint8_t  update_type;                      //升级类型
    uint8_t  curent_update_flag;               //当前升级状态
    uint8_t  current_update_partition_index;   //当前升级索引
    uint8_t  current_update_partition_status;  //当前升级分区结果
    uint32_t current_update_partition_offset;  //当前升级分区升级偏移地址-升级失败时需要记录该偏移
    iot_img_net_address_st iot_img_net_address;
    uint8_t  wifi_version[3];                //wifi固件版本   
    uint8_t  audio_version[3];               //audio固件版本     
}update_result_param_info_st;    //升级结果记录信息

int update_result_param_info_read(void);
int update_result_param_info_write(void);

#endif   //__CIAS_FLASH_UPDATE_H__