#ifndef __CIAS_IR_COMMON_H__
#define __CIAS_IR_COMMON_H__

#include "uni_log.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_cloud_com_defs.h"
#include "ty_cJSON.h"

#include "dp_process.h"
#include "tuya_ir_control.h"
#include "cias_ir_flash.h"
#include "cias_log.h"
#include "cias_system_manage.h"
#include "cias_slave_message_handle.h"
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

//记录空调状态
typedef struct 
{
    char conditioner_ir_devid[32];   //空调码库ID
    int ir_enable_flag;  //当前码库是否可用
    int power_status;  //开关状态  0:关闭 1:打开
    int mode_val;      //模式值    0:制冷 1:制热 2:自动 3:送风 4:除湿
    int temp_val;      //温度值    16度-30度
    int wind_val;      //风速值    0:自动风速 1:低速风 2:中 3:高
}air_conditioner_ir_status_t;

OPERATE_RET cias_local_send_ir(IN CHAR_T *key, IN CHAR_T *head);
#endif   //__CIAS_IR_COMMON_H__