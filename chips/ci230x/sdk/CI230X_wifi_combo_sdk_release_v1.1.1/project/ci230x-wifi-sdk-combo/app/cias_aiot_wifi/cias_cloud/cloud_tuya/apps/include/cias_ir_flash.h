#ifndef __CIAS_IR_FLASH_H__
#define __CIAS_IR_FLASH_H__

#include "cias_common.h"

#define IR_STORE_DATA_MAX_SIZE          20*1024    //空调存储最大空间20K          
#define IR_STORE_FLASH_START_ADDR       0x0011A000//0x0011F000//0x13F000  
#define IR_SET_TOP_BOX_START_ADDR       IR_STORE_FLASH_START_ADDR
#define IR_IV_DATA_START_ADDR           IR_SET_TOP_BOX_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_TV_BOX_START_ADDR            IR_IV_DATA_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_DVD_START_ADDR               IR_TV_BOX_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_AIR_CONDITIONER_START_ADDR   IR_DVD_START_ADDR + IR_STORE_DATA_MAX_SIZE      // 存储空调数据
#define IR_PROJECTOR_START_ADDR         IR_AIR_CONDITIONER_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_AUDIO_START_ADDR             IR_PROJECTOR_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_FAN_START_ADDR               IR_AUDIO_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_CAMERA_START_ADDR            IR_FAN_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_LIGHT_START_ADDR             IR_CAMERA_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_FILTER_START_ADDR            IR_LIGHT_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_GEYSER_START_ADDR            IR_FILTER_START_ADDR + IR_STORE_DATA_MAX_SIZE
#define IR_DIY_START_ADDR               IR_GEYSER_START_ADDR + IR_STORE_DATA_MAX_SIZE


typedef enum
{
    IR_SET_TOP_BOX_ID = 1,    //机顶盒
    IR_IV_DATA_ID,            //电视
    IR_TV_BOX_ID,             //电视盒子
    IR_DVD_ID,                //DVD
    IR_AIR_CONDITIONER_ID,    //空调红外数据
    IR_PROJECTOR_ID,          //投影仪
    IR_AUDIO_ID,              //音响
    IR_FAN_ID,                //风扇
    IR_CAMERA_ID,             //单反相机
    IR_LIGHT_ID,              //灯光红外数据
    IR_FILTER_ID,             //净化器
    IR_GEYSER_ID,             //热水器
    IR_DIY_ID,                //DIY设备
    IR_DEV_MAX
}IR_DEVICE_TYPE_t;
typedef struct 
{
   uint8_t ir_available_flag; //码库是否可用
   uint32_t ir_remote_id;     //remote id
   uint8_t  tuya_ir_dev_type; //红外设备类型
   short ir_data_offset;      //红外数据总偏移
   short ir_data_num;         //红外码库数据个数
   short ir_dev_id_len;       //红外码库ID长度
   short ir_head_id_len;      //红外码库head长度
   uint8_t *ir_dev_id;        //红外码库ID 
   uint8_t *ir_head_id;       //红外码库head
}tuya_ir_code_head_t;   //红外空调

typedef struct 
{
   short ir_key_len;        //红外key长度
   short ir_data_len;       //红外数据长度
   uint8_t  *ir_key;        //红外key
   uint8_t  *ir_data;       //红外key数据 
}tuya_conditioner_ir_code_data_t;   //红外空调码库

typedef struct   //四字节对齐
{
    uint32_t keyid;
    uint16_t ir_data_len;         //红外数据长度
    uint8_t  *ir_data;            //红外key数据
}tuya_universal_ir_code_data_t;   //红外通用设备码库


typedef struct 
{
    tuya_ir_code_head_t ir_code_head;
    tuya_conditioner_ir_code_data_t ir_conditioner_code_data;  //空调设备码库
    tuya_universal_ir_code_data_t ir_universal_code_data;
}tuya_ir_code_store_t;  //红外空调



void cias_ir_store_data_write(int offset, int wlen, uint8_t *wdata);
void cias_ir_store_data_read(int offset,  int rlen, uint8_t *rdata);
void tuya_ir_data_read_from_flash(uint8_t tuya_ir_dev_type);
int check_ir_data_available(uint8_t tuya_ir_dev_type, tuya_ir_code_head_t *ptuya_ir_code_head);
int tuya_ir_data_find_from_flash(uint8_t tuya_ir_dev_type, uint8_t *ir_key, int ir_key_value);
int tuya_ir_store_head_info_read(uint8_t tuya_ir_dev_type, tuya_ir_code_head_t *ptuya_ir_code_head);
void tuya_ir_store_head_info_write(uint8_t tuya_ir_dev_type, const tuya_ir_code_head_t tuya_ir_code_head);
void tuya_ir_data_write_flash(uint8_t tuya_ir_dev_type, uint32_t offset, uint8_t *ir_data_buf, int ir_data_len);

#endif   //__CIAS_IR_FLASH_H__