#ifndef __CIAS_FAN_MSG_H__
#define __CIAS_FAN_MSG_H__

//cmd_id
#define TURN_ON                2
#define TURN_OFF               3
#define SPEED_ONE              4//一档风
#define SPEED_TWO              5//二档风
#define SPEED_THREE            6//三档风
#define SPEED_FOUR             7//四档风
#define SPEED_FIVE             8//五档风
#define SPEED_SIX              9//六档风
#define SPEED_REDUCE           10//风速减小
#define SPEED_RAISE            11//风速增大
#define SPEED_MIN              12//风速最小
#define SPEED_MAX              13//风速最大	
#define SHAKE_ON               14//打开摇头	
#define SHAKE_OFF              15//关闭摇头	
#define SHAKE_LR_ON            16//打开左右摇头	
#define SHAKE_LR_OFF           17//关闭左右摇头	
#define SHAKE_HD_ON            18//打开上下摇头	
#define SHAKE_HD_OFF           19//关闭上下摇头	
#define NATURAL_ON             20//打开自然风	
#define SLEEP_ON               21//打开睡眠风	
#define NORMAL_ON              22//打开正风
#define ANION_ON               23//开负离子
#define ANION_OFF              24//关闭负离子
#define TIMMING_1H             25//定时一小时	
#define TIMMING_2H             26//定时二小时
#define TIMMING_3H             27//定时三小时	
#define TIMMING_4H             28//定时四小时	
#define TIMMING_5H             29//定时五小时	
#define TIMMING_6H             30//定时六小时
#define TIMMING_7H             31//定时七小时	
#define TIMMING_8H             32//定时八小时	
#define TIMMING_9H             33//定时九小时	
#define TIMMING_10H            34//定时十小时	
#define TIMMING_11H            35//定时十一小时	
#define TIMMING_12H            36//定时十二小时	
#define TIMING_OFF             37//关闭定时
#define VOICE_UP               38//音量增大	
#define VOICE_DOWN             39//音量减小
#define VOICE_MAX              40//最大音量	
#define VOICE_MIN              41//最小音量

/* 微信启英小程序蓝牙协议数据 */
//function_id
#define FAN_POWER              0x01    //开关
#define FAN_SPEED              0x02    //风速
#define FAN_SHAKE              0x03    //风向
#define FAN_MODE               0x04    //模式
#define FAN_SHAKE_LR_ANGLE     0x05    //左右摇头角度
#define FAN_SHAKE_UD_ANGLE     0x06    //上下摇头角度
#define FAN_ANION              0x07    //负离子
#define FAN_TIMING             0x08    //定时关机
#define FAN_SPEAKER            0x09    //播报音量
#define FAN_ASR                0x0A    //语音识别
#define FAN_MOSQUITO           0x0E    //驱蚊
#define FAN_LAMP               0x0F    //氛围灯/灯光
#define FAN_WARM_LAMP          0x10    //暖灯
#define FAN_HUMIDIFICATION     0x11    //加湿/雾化
#define FAN_COLD               0x12    //制冷
#define FAN_TMP_SHOW           0x13    //温度显示
#define FAN_SCREEN             0x14    //屏显
#define FAN_SHACK3D            0x15    //3D摇头
#define FAN_ECONOMICS          0x16    //节能
#define FAN_DEHUMIDIFICATION   0x17    //除湿
#define FAN_TIMING_ON          0x18    //预约定时开机


#define DEV_SPEED_MAX          6       //最大风档位
#define DEV_SPEED_MIN          1       //最小风档位
#define DEV_TIMING_MAX         0xAC    //定时关机最大定时(定时十二小时)
#define DEV_TIMING_MIN         0xA1    //定时关机最小定时(定时一小时)
#define DEV_TIMING_ON_MAX      0xB8    //预约开机最大定时(定时二十四小时)
#define DEV_TIMING_ON_MIN      0xA1    //预约开机最小定时(定时一小时)

#define SHAKE_LR_ON_DATA       0x12    //打开左右摇头
#define SHAKE_LR_OFF_DATA      0x11    //关闭左右摇头
#define SHAKE_HD_ON_DATA       0x22    //打开上下摇头
#define SHAKE_HD_OFF_DATA      0x21    //关闭上下摇头

#define VOICE_UP_DATA          0xF1    //音量增大	
#define VOICE_DOWN_DATA        0xF2    //音量减小
#define VOICE_MAX_DATA         0xF3    //最大音量	
#define VOICE_MIN_DATA         0xF4    //最小音量


#define FAN_MODE_NORMAL        0x03    //正常模式
#define FAN_MODE_SLEEP         0x04    //睡眠模式
#define FAN_MODE_NATURAL       0x05    //自然模式

typedef struct
{
    uint8_t power;          //电源
    uint8_t speed;          //风速
    uint8_t shake;          //摇头
    uint8_t mode;           //模式
    uint8_t anion;          //负离子
    uint8_t timing;         //定时关机
    uint8_t timing_on;      //预约定时开机
    uint8_t asr_status;     //语音识别状态
    uint8_t mosquito;       //驱蚊
    uint8_t lamp;           //氛围灯
    uint8_t warm_lamp;      //暖灯
    uint8_t humidification; //加湿\雾化
    uint8_t cold;           //制冷
    uint8_t tmp_show;       //温度显示
    uint8_t screen;         //屏显
    uint8_t shack3D;        //3D摇头
    uint8_t economics;      //节能
    uint8_t dehumidification;//除湿
}fan_dev_t;

#endif