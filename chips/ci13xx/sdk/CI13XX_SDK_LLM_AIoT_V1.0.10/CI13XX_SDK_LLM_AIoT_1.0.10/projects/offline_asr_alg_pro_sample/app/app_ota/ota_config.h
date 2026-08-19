/**
 * @file ci_flash_data_info.h
 * @brief flash data struct
 * @version 
 * @date 
 * 
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 * 
 */

#ifndef _CI_OTA_FUN_23LC_H_
#define _CI_OTA_FUN_23LC_H_

#include <stdbool.h>
#include "ci_flash_data_info.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OTA_DEVICE_13XX                    1
#define OTA_NO_DNN                         0                        //ota不升级dnn，被升级固件包没有dnn
#define FILECONFIG_SPIFLASH_BAK_ADDR       0x8000
#define USER_CODE1_ADDR                    0xA000
#define OTA_PACK_LENGTH                    1024                    //每包有效数据长度
#define OTA_CHECK_LENGTH                   1024                    
#define NPU_BASE_ADDR                      (0x40000000)

#define MSG_OTA_HEAD0                      0xA5
#define MSG_OTA_HEAD1                      0x0F
#define MSG_OTA_TAIL                       0xFF
#define MSG_TYPE_OTA_VERSION 	           0xA0
#define MSG_TYPE_OTA_START 	               0xA1
#define MSG_TYPE_OTA_FIRMWARE 	           0xA2
#define MSG_TYPE_OTA_FINISH 	           0xA3
#define MSG_TYPE_OTA_REQUEST 	           0xA4

#define OTA_ACK_PACK_LEN 	               20
#define OTA_ACK_PACK_HEAD_LEN 	           5
#define MSG_OTA_ACK_PACK_TAIL_LEN 	       3
#define OTA_PACK_PALOAD_HEAD_LEN           4
#define OTA_PACK_ACK_START_HEAD_LEN        4
#define OTA_PACK_ACK_VERSION_HEAD_LEN      10
#define OTA_PACK_ACK_FIRMWAER_HEAD_LEN     9
#define OTA_PACK_ACK_REQUEST_HEAD_LEN      10
#define OTA_PACK_PALOAD_HEAD_LEN           4
#define OTA_PACK_PALOAD_FIRMWAER_HEAD_LEN  2
#define ERASE_4K                          (4 * 1024)
#define ERASE_MAX_SIZE                    (64 * ERASE_4K)
// 传输包解析结构类型，用于构造和解包
#pragma pack(1)
typedef struct
{
	unsigned char head0;
	unsigned char head1;
	unsigned char len0;
	unsigned char len1;
	unsigned char msg_type;
	unsigned char *data;
	unsigned char crc0;
	unsigned char crc1;
	unsigned char tail;
}cias_ota_pack_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint32_t version;
    uint32_t addr;
    uint32_t size;
    uint32_t crc;
    uint8_t status;
}single_partition_t; 
#pragma pack()

typedef enum 
{
    UPDATA_FILECONFIG_BAK =  0x00, 
    UPDATA_USER_CODE1, 
    UPDATA_USER_CODE2,           
    UPDATA_ASR,           
    UPDATA_DNN,                
    UPDATA_VOICE,  
    UPDATA_USER_FILE,  
    UPDATA_OVER_FLOW,      
}partition_index_t;

typedef enum 
{
    PARTITION_OK =  0x00, 
    PARTITION_TABLE_ERRO, 
    PARTITION_ERRO,      
}partition_check_t;
typedef struct 
{
    partition_index_t partition_index;
    bool first_updata_user_code2;
    uint32_t version;
    uint32_t offset;
    uint32_t size;
    uint32_t crc;
    uint32_t recv_size;
    uint32_t partition_size;
}ota_status_t;

extern partition_check_t partition_check;
partition_check_t check_ota_finish();
bool check_ota_partitoin_table_finish();
void get_ota_version(uint8_t *data);
void set_ota_partition_new(partition_index_t partition_index, uint32_t version, uint32_t offset, uint32_t size, uint32_t crc);
void ota_start(void);
void ota_write_flash_data(const uint8_t *buffer, const uint32_t size);
void ota_main_task(void);
int8_t get_ota_partition_status(uint16_t *pkg_next, uint16_t pkg_count, uint16_t pkg_len);
void send_ota_recv_msg(cias_ota_pack_t *msg, BaseType_t *xHigherPriorityTaskWoken);
uint16_t get_ota_pack_payload_len(cias_ota_pack_t *ota_pack);
int vmup_send_ota_ack_packet(cias_ota_pack_t * msg);
int32_t post_ota_write_flash(char *buf, uint32_t addr, uint32_t size);
int32_t post_ota_erase_flash(uint32_t addr, uint32_t size);
int32_t post_ota_read_flash(char *buf, uint32_t addr, uint32_t size);
int vmup_port_hw_ota_init(void);
#ifdef __cplusplus
}
#endif

#endif

