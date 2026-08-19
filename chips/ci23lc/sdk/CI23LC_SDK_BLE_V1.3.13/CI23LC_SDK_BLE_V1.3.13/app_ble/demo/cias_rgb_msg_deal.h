#ifndef __CIAS_RGB_MSG_H__
#define __CIAS_RGB_MSG_H__

//命令词条对应的ID，填写命令词cmd_id或语义semantic_id。修改BLE_DEV_STATE_REPORT_JUDGEMENT_MODE宏选择状态上报判断方式
#define TURN_OFF               1001//关灯控
#define TURN_ON                1002//打灯控
#define RGB_BRIGHTNESS_UP      1003//亮一点
#define RGB_BRIGHTNESS_DOWN    1004//暗一点
#define RGB_BRIGHTNESS_MIX     1005//最高亮度
#define RGB_BRIGHTNESS_MIN     1006//最低亮度
#define RGB_BRIGHTNESS_MID     1007//中等亮度
#define RGB_WHITE_COLOR        1008//白色灯光
#define RGB_ORANGE_COLOR       1009//橙色灯光
#define RGB_RED_COLOR          1010//红色灯光
#define RGB_PURPLE_COLOR       1011//紫色灯光
#define RGB_BLUE_COLOR         1012//蓝色灯光
#define RGB_GREEN_COLOR        1013//绿色灯光
#define RGB_YELLOW_COLOR       1014//黄色灯光
#define RGB_MUSIC_MODE         1015//音乐律动
#define TURN_OFF_TIMING        1016//关闭定时
#define TIMMING_1H             1017//定时一小时
#define TIMMING_2H             1018//定时二小时
#define TIMMING_3H             1019//定时三小时
#define TIMMING_4H             1020//定时四小时
#define TIMMING_5H             1021//定时五小时
#define TIMMING_6H             1022//定时六小时
#define TIMMING_7H             1023//定时七小时
#define TIMMING_8H             1024//定时八小时
#define TIMMING_9H             1025//定时九小时
#define TIMMING_10H            1026//定时十小时
#define TIMMING_11H            1027//定时十一小时
#define TIMMING_12H            1028//定时十二小时
#define TIMMING_HARF           1029//定时半小时

#define MAX_VOLUME             2000//最大音量	
#define MIN_VOLUME             2001//最小音量
#define VOLUME_UP              2002//音量增大
#define VOLUME_DOWN            2003//音量减小
#define TURN_ON_VOICE          2004//开启语音
#define TURN_OFF_VOICE         2005//关闭语音
#define TIME_ON                10011//定时时间到


//function_id
#define RGB_POWER                 0x01    //开关
#define RGB_BRIGHTNESS            0x02    //亮度
#define RGB_PROPERTY_MODE         0x03    //属性模式
#define RGB_TIMING                0x04    //定时
#define RGB_VOICE                 0x05    //音量

#define DEV_BRIGHTNESS_MAX        100
#define DEV_BRIGHTNESS_MID        50
#define DEV_BRIGHTNESS_MIN        10
#define DEV_VOICE_MAX             1
#define DEV_VOICE_MIN             7


typedef struct
{
    uint8_t power;      //uint8_t rgb_switch_status;
    uint8_t brightness;//uint8_t duty_lasted;
    uint8_t red_value;
    uint8_t green_value;
    uint8_t blue_value;
    uint8_t e_value;
    uint8_t s_value;
    uint8_t property_mode;
    uint8_t timing;
}rgb_dev_t;




#endif