#ifndef  __UPDATE_H__
#define  __UPDATE_H__

// #include "flash_partition_table.h"

#define VERTION                             "1.0.0"                 // 本软件版本号
#define OTA_BOOTLOADER_BAUDRATE             115200                  // 最大921600 (目前只能为115200)
#define OTA_UPDATER_BAUDRATE                2000000                 // 最大3000000

#define OTA_CLOUD_SIZE                      4096*10                    //>=MAX_DATA_LENGTH ,且能整除4096
#define MAX_DATA_LENGTH                     1024*4                    //必须大于等于32，小于等于4096，且能整除4096
#define MAX_PACKAGE_LENGTH                  (MAX_DATA_LENGTH + 10)
#define MIN_PARTITION_SIZE                  4096 
#define ERASE_BLOCK_SIZE                    4096

//MESSAGE TYPE
#define MSG_TYPE_CMD	                    0xA0
#define MSG_TYPE_REQ	                    0xA1
#define MSG_TYPE_ACK	                    0xA2
#define MSG_TYPE_NOTIFY	                    0xA3

#define MSG_CMD_UPDATE_REQ                  0x03
#define MSG_CMD_GET_INFO                    0x04
#define MSG_CMD_UPDATE_CHECK_READY          0x05
#define MSG_CMD_UPDATE_BLOCK_INFO           0x06
#define MSG_CMD_UPDATE_ERA                  0x07
#define MSG_CMD_UPDATE_WRITE                0x08
#define MSG_CMD_UPDATE_BLOCK_WRITE_DONE     0x09
#define MSG_CMD_UPDATE_VERIFY               0x0A
#define MSG_CMD_CHANGE_BAUDRATE             0x0B
#define MSG_CMD_TEST_BAUDRATE               0x0C
#define MSG_CMD_PROGRESS                    0x11
#define MSG_CMD_SET_FW_FMT_VER              0x12        //设置固件格式版本号，1个字节数据即版本号

#define MSG_CMD_SYS_RST                     0xA1        //复位命令

#define PARTITION_USER1_FLAG_MASK           0x0001      //需要升级user code 1
#define PARTITION_USER2_FLAG_MASK           0x0002      //需要升级user code 2
#define PARTITION_ASR_FLAG_MASK             0x0004      //需要升级ASR
#define PARTITION_DNN_FLAG_MASK             0x0008      //需要升级DNN
#define PARTITION_VOICE_FLAG_MASK           0x0010      //需要升级voice
#define PARTITION_USERFILE_FLAG_MASK        0x0020      //需要升级user file
#define PARTITION_UPDATE_ALL				(0x0001|0x0002|0x0004|0x0008|0x0010|0x0020)
#define PARTITION_INVALID_FILE_MASK         0x8000      //无效的固件文件

#define PARTITION_TABLE1_START_ADDR          0x6000      	//分区表起始地址
#define PARTITION_TABLE2_START_ADDR          0x8000      	//分区表起始地址
#define PROGRAM_AGENT_ADDR                   0x1ff58000		//3代updater运行地址
// #define UPDATER_FLASH_START_ADDR             (USER_SPACE_OFFSET + USER_SPACE_SIZE - 20*1024)      //updater存储起始地址为user分区的后20K
#define GET_LWORD(p)    ((*(unsigned char*)(p)) + ((*(unsigned char*)((p)+1))<<8) + ((*(unsigned char*)((p)+2))<<16) + ((*(unsigned char*)((p)+3))<<24))
#define GET_SWORD(p)    ((*(unsigned char*)(p)) + ((*(unsigned char*)((p)+1))<<8))

#pragma pack(1)
typedef struct
{
    unsigned int version;
    unsigned int address;
    unsigned int size;
    unsigned int crc;
    unsigned char status;
}partition_info_t;
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

    partition_info_t user_code1;
    partition_info_t user_code2;
    partition_info_t asr_cmd_model;
    partition_info_t dnn_model;
    partition_info_t voice;
    partition_info_t user_file;

    unsigned int ConsumerDataStartAddr;
    unsigned int ConsumerDataSize;
    
    unsigned short PartitionTableChecksum;
    //unsigned char  ota_statue; //ota状态记录
}partition_table_t; 
#pragma pack()
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
}updating_partitions_t;

typedef struct
{
    // cias_olcp_t*            ota_olcp;
    partition_table_t       file_info;
    partition_table_t       device_info;
    partition_table_t*      update_info;
    updating_partitions_t   updating;
    unsigned char           is_get_device_info;
    int                     file_size;
    char*                   url;
    void*                   session;
}cias_ota_data_t;

typedef enum
{
    OTA_FINISHED = 0,
    ENABLE_CLOUD_GET = 2,
    ENABLE_PACKAGE_SEND = 3,
}ota_status_t;

typedef struct
{
    ota_status_t    data_control;
    int             offset_in_file;
    int             partions_size;
}ota_update_flag_t;

int update_ota_main();
int cmd_update_reset_device();

#endif
