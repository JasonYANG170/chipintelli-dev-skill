/******************************************************************************
* @file    customer asr.c 
* @company  Chipintelli Technology Co., Ltd.
* @author  chenronglin.
* @version V1.0.0
* @date    2018.03.03
* @function: use customer command
******************************************************************************/

#include "system_msg_deal.h"

#define CUSTOMR_SOFT_VERSION0    0xCD
#define CUSTOMR_SOFT_VERSION1    0xCB

#define CUSTOMER_CMD_ACK_RIGHT 0xFE
#define CUSTOMER_CMD_ACK_ERROR 0xFA
#define CUSTOMER_CMD_REPEAT_TIMES 3
#define CUSTOMR_UART_HEADER0    0xA5
#define CUSTOMR_UART_HEADER1    0xFA
#define CUSTOMR_UART_HEADER2    0x00
#define HEADER_STATE1      0x00
#define HEADER_STATE2      0x01
#define DATA_STATE1     0x02
#define DATA_STATE2     0x03
#define DATA_STATE3     0x04
#define DATA_STATE4     0x05
#define CHECKSUM_STATE1     0x06
#define END_STATE2     0x07
#define ZHUOMIAN_SLEEP_TIME   (30)      //TIME **************
#define CUSTOMR_UART_END        0xFB
#define CUSTOMR_CHECK_SUM       1 
#define ZHUOMIAN_UP           70       //INDEX **************
#define ZHUOMIAN_DOWN         71      //INDEX **************
#define ZHUOMIAN_STOP        72      //INDEX **************

#define SUPPORT_WAKEUP_ID1      1
#define SUPPORT_WAKEUP_ID2      -2

#define  SUPPORT_GOODBAY_ID 0x10000002
#define  SUPPORT_WECOME_ID  0x10000001
#define SUPPORT_CHECK_VERSION_ID  0x10000004


#define  SUPPORT_CLOSE_WARM1 (78)
#define  SUPPORT_CLOSE_WARM2 (78)

#define VOLUME_UP_COMMAND_INDEX_1  0x800700

#define VOLUME_DOWN_COMMAND_INDEX_1  0x800800


#define VOLUME_MAX_COMMAND_INDEX_1  0x800900


#define VOLUME_MIN_COMMAND_INDEX_1  0x800A00

#define PLAY_VOLMAX_VOICE_INDEX 0x800900
#define PLAY_VOLMIN_VOICE_INDEX 0x800A00

#define VOICE_OFF_COMMAND_INDEX_1  (-1)
#define VOICE_OFF_COMMAND_INDEX_2  (-1)

#define VOICE_ON_COMMAND_INDEX_1  (-1)
#define VOICE_ON_COMMAND_INDEX_2  (-1)
#define  ENTER_AUTO_TEST_WAKE_ID   0x90000001
#define  ENTER_AUTO_TEST_CMD_ID   0x90000002

/* 语音端命令词列表 */
/* --semantic_id--  */
typedef enum command_id
{
	OPEN_AIRCONDITION					=2,//打开空调
 	CLOSE_AIRCONDITION  			 	,//关闭空调
 	MODE_CHANGE		   				 	,//模式切换
	AIR_AUTO_MODE					  	,//全自动
	AIR_HAT_MODE		   				,//制热模式
	AIR_CLODE_MODE				 		,//制冷模式
	AIR_WIND_MODE						,//送风模式
	SAVA_ENERGY_MODE					,//节能模式
	REMOVE_WET_MODE						,//除湿模式
	AIR_SLEEP_MODE						,//睡眠模式
	SWEEP_WIND_MODE						,//打开扫风
	CLODE_SWEEP_WIND					,//关闭扫风
	TEMPTURE_ADD						,//增大温度
	TEMPTURE_SUB						,//减小温度
	AIR_TEMPTURE_MIN					=16,//十六度
	AIR_TEMPTURE_MAX					=30,//三十二度
	ONE_GEAR_SPEED  			=33,//一档风
	TWO_GER_SPEED						,//二挡风
	THREE_GEAR_SPEED					,//三挡风
	WIND_SPEED_ADD						,//增大风速
	WIND_SPEED_SUB						,//减小风速
	MAX_SPEED							,//最大风
	MIN_SPEED							,//最小风
	AUTO_SPEED_MODE						,//自动风速
	BRIGHTNESS_ADD						,//调亮一点
	BRIGHTNESS_SUB		 =42				,//调暗一点
	LIGHT_READ_MODE						,//阅读模式
	LIGHT_MODE							,//照明模式
	NIGHT_LAMP_MODE	  					,//夜灯模式
	LIGHT_RED_MODE						,//红色模式
	LIGHT_GREEN_MODE					,//绿色模式
	LIGHT_BLUE_MODE						,//蓝色模式
	OPEN_NIGHT_LIGHT					,//打开台灯
	OPEN_BALCONY_LIGHT					,//打开阳台灯
	OPEN_BEDROOM_LIGHT					,//打开卧室灯
	OPEN_GARDEN_LIGHT		=52			,//打开花园灯
	OPEN_DINING_LIGHT					,//打开餐厅灯
	OPEN_TOILET_LIGHT					,//打开厕所灯
	OPEN_LIVING_ROOM					,//打开客厅灯
	CLOSE_NIGHT_LIGHT					,//关闭台灯
	CLOSE_BALCONY_LIGHT					,//关闭阳台灯
	CLOSE_BEDROOM_LIGHT					,//关闭卧室灯
	CLOSE_GARDEN_LIGHT					,//关闭花园灯
	CLOSE_DINING_LIGHT					,//关闭餐厅灯
	CLOSE_TOILET_LIGHT					,//关闭厕所灯
	CLOSE_LIVING_ROOM			= 62		,//关闭客厅灯
       COMMAND_ID_MAX,
}command_id_enum;


typedef enum
{
	CI_TX_START =   0x80, //start   
	CI_TX_ASR	    ,  
	CI_TX_SLEEP	    , 
	CI_TX_VER,
	CI_TX_REV,
	CI_TX_PLAY_INDEX,
	CI_TX_OVER_PLAY,
	CI_TX_VOLSET_OK,
	CI_TX_VOICE_ONOFF_OK,
	CI_TX_SET_WAKE_TIME_OK,
	CI_TX_ASR_START_OK,
	CI_TX_VOICE_AUTO_TEST_OK,
	CI_TX_END       ,      //--end
}CMD_CI_SEND;

typedef enum
{
	CI_RX_START	    =   0,//--start
	CI_RX_ECHO_EN	    ,
	CI_RX_CHK_EN        ,
	CI_RX_PLAY_INDEX =0x10    ,
	CI_RX_RESET    ,
	CI_RX_UNWAKE_CMD    ,
	CI_RX_MUTE,
	CI_RX_VOLSET ,
	CI_RX_GETVER,
	CI_RX_SET_WAKE_TIME ,
	CI_RX_ASR_SWITCH,
	CI_RX_AUTO_TEST,
	CI_RX_END   ,//--end
}CMD_CI_RECIVED;
typedef void (*pUartSend)(unsigned char *buffer,int lenth,int bchecksumflag,unsigned char dEnd_cmd); 

static int crx_state=HEADER_STATE1;
/******************************************************************************
  * @bool uart_send_customer_cmd(unsigned int index)
  * @parameter:  comdata 串口数据
  * @return : none
  * @function : 用于发送串口数据
*******************************************************************************/
void uart_send_customer_cmd(unsigned int index);

/******************************************************************************
  * @void Deal_Serial_msg_If(sys_com1_msg_data_t *com_rev_data)
  * @parameter:  sys_com1_msg_data_t *com_rev_data 
  * @return : none
  * @function :处理控制器发的串口数据
*******************************************************************************/
void custormer_com_deal_msg(sys_msg_com_data_t *com_rev_data);
/******************************************************************************
  * @bool Play_Voice_Over_If(void)
  * @parameter:  none 
  * @return : none
  * @function : 播放完后处理协议
*******************************************************************************/
void Play_Voice_Over_If(void);
/***************************************************************************************
* @void Init_Customer_Paramter_If(void)
* @parameter: none
* @return :    none
* @function : use init customer special parameter 
*******************************************************************************************/
void Init_Customer_Paramter_If(void);
void system_check_playid_status(cmd_handle_t cmd_handle);
void custormer_deal_asr_msg(sys_msg_asr_data_t *asr_msg);

#if  UART_BAUDRATE_CALIBRATE
void Init_baudrate_sync(void);
void send_baudrate_sync_req(void);
#endif
