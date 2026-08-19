#include "system_msg_deal.h"
#include "ci130x_it.h"

#define CUSTOMR_BLUETOOTH_VER    0x00
#define CUSTOMR_APP_VER    0x01


#define BLUETOOTH_STATE_HEADER0      0x0
#define BLUETOOTH_STATE_HEADER1      0x1
#define BLUETOOTH_STATE_VER      0x2
#define BLUETOOTH_STATE_CMD     0x03
#define BLUETOOTH_STATE_DATA1    0x04
#define BLUETOOTH_STATE_DATA2     0x05
#define BLUETOOTH_STATE_CHECK_SUM      0x06
#define BLUETOOTH_STATE_END      0x07


#define APP_STATE_CMD     0x08
#define APP_STATE_PAYLOAD     0x09
#define APP_STATE_CHECK_SUM      0x0A
#define APP_STATE_CHECK_END      0x0B

#define BLUETOOTH_CHECK_SUM      0x01
#define BLUETOOTH_CHECK_END      0xFB


#define APP_UIDH     0x00
#define APP_UIDL     0x04

#define APP_DEVICEH     0xB0
#define APP_DEVICEL     0x01

#define APP_DEVICE_TYPEH    0x00
#define APP_DEVICE_TYPEL     0x01


#define BLE_COMMON_DATA_HEADER0    0xA5
#define BLE_COMMON_DATA_HEADER1    0xFA

#define BLE_TRANSMISSION_HEAD0			  	0xAA
#define BLE_TRANSMISSION_HEAD1			  	0xAE
#define BLE_TRANSMISSION_END        0xFB

#define CUSTOMR_BLUETOOTH_END        0xFB
#define CUSTOMR_BLUETOOTH_CHECK_SUM       1 

#define MODE_QUANTITY		6

#define WRITE_BT_NAME				0
#define WRITE_BLE_NAME				1 

#define BT_NAME_ANSWER				0x10
#define BLE_NAME_ANSWER				0x11 


#define BLUETOOTH_CONNECT_OK    0x1E01
#define BLUETOOTH_CONNECT_FAIL   0x1E00

#define BLUETOOTH_CURRENT_PLAY    1
#define BLUETOOTH_CURRENT_MOBILE_STOP    2
#define BLUETOOTH_CURRENT_VOICE_STOP    3
#define BLUETOOTH_CURRENT_IN_LINE   5

#define CI_TO_BT_BTMAME_TYPE			0x81

#define BT_TO_CI_CURRENCY_DATA_TYPE	   	0xA0
#define BT_TO_CI_BTNAME_DATA_TYPE		0xA1
#define MNP_TO_CI_BTNAME_DATA_TYPE		0x91
#define BLUETTOTH_VOL_ADJUST 0x820100

#define 	SUPPORT_READ_BT_NAME_CMD	0x801000
#define 	SUPPORT_READ_BLE_NAME_CMD    0x801100


#define SET_BT_NAME        "启英茶吧机蓝牙"
#define SET_BLE_NAME       "启英茶吧机小程序"

typedef enum
{
	AIR_CONDITION_START		= 0,//开关空调
	AIR_CONDITION_SWITCH_AIRCONDITION		=1,//开关空调
	AIR_CONDITION_MODE_CHANGE				,//模式切换
	AIR_CONDITION_SWITCH_WINDOW				,//开关扫风
	AIR_CONDITION_TEMPTURE_ADD				,//温度增大
	AIR_CONDITION_TEMPTURE_SUB				,//温度减小
	AIR_CONDITION_TEMPTURE_SET				,//温度设置
	AIR_CONDITION_SPEED_ADD					,//风速增大
	AIR_CONDITION_SPEED_SUB					,//风速减小
	AIR_CONDITION_SPEED_AUTO				,//自动风速
	AIR_CONDITION_EVE_LIGHT	= 10				,//台灯夜灯
	AIR_CONDITION_LIGHT_ADD					,//亮一点
	AIR_CONDITION_LIGHT_SUB					,//暗一点
	AIR_CONDITION_READ_MODE					,//阅读模式
	AIR_CONDITION_LIGHE_MODE				,//照明模式
	AIR_CONDITION_NIGHT_LIGHE				,//夜灯模式
	AIR_CONDITION_RED_MODE					,//红色模式
	AIR_CONDITION_GREEN_MODE				,//绿色模式
	AIR_CONDITION_BLUE_MODE					,//蓝色模式
	AIR_CONDITION_BALCONY_LIGHT				,//阳台灯
	AIR_CONDITION_BEDROOM_LIGHT		=20		,//卧室灯
	AIR_CONDITION_GARDEN_LIGHT				,//花园灯
	AIR_CONDITION_RESTAURANT_LIGHT			,//餐厅灯
	AIR_CONDITION_TOILET_LIGHT				,//厕所灯
	AIR_CONDITION_LIVING_LIGHT				,//花园灯
	TEA_MACHINE_END       ,      //--end
}TEA_MACHINE_TO_VOICE_ID;

typedef enum
{
	VOICE_TO_BLUETTOTH_CMD = 0x80,
	VOICE_TO_BLUETTOTH_VOL = 0x82,
	BLE_TRANSMISSION_CMD1  =   0x91,
	BLE_TRANSMISSION_CMD2  =   0x92,
	BLUETOOTH_TO_VOICE_CMD = 0xA0,
	BLUETOOTH_VOICE_CMD_END       ,      //--end
}BLUETOOTH_VOICE_CMD;


typedef enum
{
	BLUETTOTH_PLAY_MUSIC_SEMANTIC_ID = 0x800100,
	BLUETTOTH_PAUSE_MUSIC_SEMANTIC_ID = 0x800200,
	BLUETTOTH_PRE_MUSIC_SEMANTIC_ID = 0x800300,
	BLUETTOTH_NEXT_MUSIC_SEMANTIC_ID = 0x800400,
	BLUETTOTH_OPEN_SEMANTIC_ID1  = 0x800B00,
	BLUETTOTH_OPEN_SEMANTIC_ID2 = 0x800C00,
	BLUETTOTH_CLOSE_SEMANTIC_ID = 0x800D00,
	BLUETTOTH_ANSWER_PHONE_SEMANTIC_ID = 0x800E00,
	BLUETTOTH_ANSWER_BL_SEMANTIC_ID = 0x800F00,
	BLUETTOTH_READ_BT_NAME_SEMANTIC_ID = 0x801000,
	BLUETTOTH_READ_BLE_NAME_SEMANTIC_ID = 0x801100,
	BLUETTOTH_HUNG_UP_PHONE_SEMANTIC_ID = 0x801200,
	BLUETTOTH_SAVE_FAMILY_NUMBER_SEMANTIC_ID = 0x830101,
	BLUETTOTH_SAVE_FATHER_NUMBER_SEMANTIC_ID = 0x830102,
	BLUETTOTH_SAVE_FRENDS_NUMBER_SEMANTIC_ID = 0x830103,
	BLUETTOTH_SAVE_EMERGENCY_NUMBER_SEMANTIC_ID = 0x830109,
	BLUETTOTH_CALL_FAMILY_NUMBER_SEMANTIC_ID = 0x830201,
	BLUETTOTH_CALL_FATHER_NUMBER_SEMANTIC_ID = 0x830202,
	BLUETTOTH_CALL_FRENDS_NUMBER_SEMANTIC_ID = 0x830203,
	BLUETTOTH_CALL_EMERGENCY_NUMBER_SEMANTIC_ID = 0x830300,
	BLUETOOTH_SEMANTIC_ID_END       ,      //--end
}BLUETOOTH_SEMANTIC_ID;

typedef enum
{
	BL_ANSWER_PLAY_MUSIC_ID = 1,
	BL_ANSWER_PHONE_PAUSE_MUSIC_ID,
	BL_ANSWER_VOICE_PAUSE_MUSIC_ID,
	BL_ANSWER_CONNECT_OK_ID,
	BL_ANSWER_CONNECT_FAIL_ID,
	BL_ANSWER_VOLUME_MAX_ID,
	BL_ANSWER_VOLUME_MIN_ID,
	BL_ANSWER_CALL_IN_STATE,
	BL_ANSWER_CALL_OUT_STATE					,//呼出电话
	BL_ANSWER_CALL_SUCCESSEFUL_STATE	 =0xa				,//电话接通
	BL_ANSWER_HANG_UP_PHONE_STATE	= 0xb			,//挂断电话
	BL_ANSWER_READ_BT_NAME_CMD	=0x10				,//串口读取蓝牙名称	
	BL_ANSWER_READ_BLE_NAME_CMD= 0x11				,//串口读取蓝牙ble名称
	BL_ANSWER_END       ,      //--end
}BLUETOOTH_ANSWER_STATE;

typedef struct light_control
{
	uint8_t	night_light_state  :1;
	uint8_t 	balcony_light_state:1;
	uint8_t	bedroom_light_state:1;
	uint8_t	garden_light_state :1;
	uint8_t	dining_light_state :1;
	uint8_t	toilet_light_state :1;
	uint8_t	living_light_state :1;
	uint8_t					   :1;

}light_control_stru;

typedef struct user_data
{
	uint16_t air_mode;
	uint16_t tempture;
	uint16_t wind_mode;
	uint16_t air_switch;
	light_control_stru light_state;
	uint16_t wind_speed;

}user_data_stru;


typedef void (*bluetoothSend)(unsigned char *buffer,int lenth,int bchecksumflag,unsigned char dEnd_cmd); 

void bluetooth_deal_msg(sys_bluetooth_data_t *iot_rev_data);
void app_deal_msg(sys_app_data_t *iot_rev_data);
int get_bluetooth_phone_state(void);

void Init_bluetooth_Paramter_If(void);
void bluetooth_check_playstate(void);
int get_bluetooth_play_state(void);
int get_bluetooth_connect_state(void);
void Audio_Play_IO_init(void);
void bluetooth_send_cmd(uint32_t semantic);

int get_bluetooth_volume_max_min_flag(void);
void clean_bluetooth_volume_max_min_flag(void);
void bluetooth_wakeup_send_cmd(uint32_t semantic);
void set_aec_state_play_music(void);
void stop_play_music_timer(void);
void set_cmd_play_music_state(void);

