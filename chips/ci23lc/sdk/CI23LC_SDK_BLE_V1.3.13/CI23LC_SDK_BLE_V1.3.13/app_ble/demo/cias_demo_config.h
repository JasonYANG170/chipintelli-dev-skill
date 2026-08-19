#ifndef __CIAS_DEMO_CONFIG_H__
#define __CIAS_DEMO_CONFIG_H__

#define DEV_DRIVER_EN_ID                    DEV_FAN_MAIN_ID

#define CIAS_PROTOCOL_VER                   1           //和小程序通信协议版本：1-V1.0
#define APP_GET_CMD_INFO_ENABEL             0           //通过小程序获取词条，次功能占用内存较多(和词条数量有关)，默认不开启
#define APP_PAGE_TITLE                      "语音智能设备"//设备控制页面标题. 中文最大长度9,字母数字最大长度15
#define APP_PAGE_BACKGROUND                 "1"         //设备控制页面背景. "1":默认背景图; "2":灰色; "3":白黑渐变; "4":三色渐变; "5":开/关机-蓝/白切换;
#define BLE_DEV_STATE_REPORT_JUDGEMENT_MODE 0           //设备识别词条状态上报判断方式，0：判断命令词ID进行上报  1：判断语义ID进行上报


//主设备ID定义
#define DEV_IR_CONTROL_MAIN_ID              0x01   //红外遥控器
#define DEV_AIRCONDITION_MAIN_ID            0X02   //空调
#define DEV_LIGHT_CONTROL_MAIN_ID           0x03   //灯控
#define DEV_SOUND_MAIN_ID                   0x04   //音响设备
#define DEV_TEA_BAR_MAIN_ID                 0x05   //茶吧机
#define DEV_FAN_MAIN_ID                     0x06   //风扇
#define DEV_HEATTABLE_MAIN_ID               0x07   //取暖桌
#define DEV_WARMER_MAIN_ID                  0x08   //取暖器
#define DEV_WATERHEATED_MAIN_ID             0x09   //水暖毯
//次设备ID定义
#define DEV_LIGHT_CONTROL_ONE_SUB_ID        0x01  //一路灯
#define DEV_LIGHT_CONTROL_THREE_SUB_ID      0x02  //三路灯
#define DEV_LIGHT_CONTROL_FOUR_SUB_ID       0x03  //四路灯
#define DEV_LIGHT_CONTROL_FIVE_SUB_ID       0x04  //五路灯
#define DEV_LIGHT_CONTROL_LAMP_SUB_ID       0x05  //灯带
#define DEV_LIGHT_CONTROL_SPOT_SUB_ID       0x06  //射灯
#define DEV_LIGHT_CONTROL_RGB_SUB_ID        0x07  //RGB灯



#if(DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)    
    #define DEV_TYPE_ID       DEV_AIRCONDITION_MAIN_ID
    #define DEV_NUMBER_ID     1   
    #define CONFIG_TYPE       0  
#elif(DEV_DRIVER_EN_ID == DEV_LIGHT_CONTROL_MAIN_ID)               
    #define DEV_TYPE_ID        DEV_LIGHT_CONTROL_MAIN_ID
    #define DEV_NUMBER_ID      DEV_LIGHT_CONTROL_RGB_SUB_ID
    #define CONFIG_TYPE       0
    #if CIAS_BLE_ADV_GROUP_MODE_ENABEL
    #define CONFIG_TYPE       4     //小程序区别广播设备
    #endif
#elif (DEV_DRIVER_EN_ID == DEV_TEA_BAR_MAIN_ID)  
    #define DEV_TYPE_ID       DEV_TEA_BAR_MAIN_ID
    #define DEV_NUMBER_ID     1   
    #define CONFIG_TYPE       0 
#elif(DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID)  
    #define DEV_TYPE_ID       DEV_FAN_MAIN_ID
    #define DEV_NUMBER_ID     9 
    #define CONFIG_TYPE       0   
#elif(DEV_DRIVER_EN_ID == DEV_HEATTABLE_MAIN_ID)
    #define DEV_TYPE_ID       DEV_HEATTABLE_MAIN_ID
    #define DEV_NUMBER_ID     1   
    #define CONFIG_TYPE       0 
#elif(DEV_DRIVER_EN_ID == DEV_WARMER_MAIN_ID)
    #define DEV_TYPE_ID       DEV_WARMER_MAIN_ID
    #define DEV_NUMBER_ID     1 
    #define CONFIG_TYPE       0 
#elif(DEV_DRIVER_EN_ID == DEV_WATERHEATED_MAIN_ID)
    #define DEV_TYPE_ID       DEV_WATERHEATED_MAIN_ID
    #define DEV_NUMBER_ID     1 
    #define CONFIG_TYPE       0 
#endif
#endif   //__CIAS_DEMO_CONFIG_H__