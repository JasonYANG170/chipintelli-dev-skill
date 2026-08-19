/**
  ******************************************************************************
  * @文件    cias_network_msg_protocol.h
  * @版本    V1.0.0
  * @日期    2019-5-9
  * @概要    uart protocol
  ******************************************************************************
  * @注意
  *
  * 版权归chipintelli公司所有，未经允许不得使用或修改
  *
  ******************************************************************************
  */
#ifndef _cias_network_msg_protocol_H
#define _cias_network_msg_protocol_H

#include <stdbool.h>
#include <stdint.h>
#include "system_msg_deal.h"
#include "ci130x_gpio.h"
#include "audio_play_os_port.h"
#include "semphr.h"
#ifdef __cplusplus
    extern "C"{
#endif

#define USE_LOCAL_VAD_END (1)   //本地VAD END 结束语音上传
    



#define SDIO_REV_IO_PIN_NAME        1
#define SDIO_REV_IO_PIN_BASE        1
#define SDIO_REV_IO_PIN_NUMBER      1
#define SDIO_REV_IO_FUNCTION        1
#define SDIO_REV_IO_PIN_IRQ         1

#define SDIO_ACK_IO_PIN_NAME        1
#define SDIO_ACK_IO_PIN_BASE        1
#define SDIO_ACK_IO_PIN_NUMBER      1
#define SDIO_ACK_IO_FUNCTION        1

typedef struct
{
    PinPad_Name pin_name;
    gpio_base_t gpio_base;
    gpio_pin_t pin_num;
    IOResue_FUNCTION io_fun;
    IRQn_Type gpio_irq;
}sdio_revio_info;

typedef struct
{
    PinPad_Name pin_name;
    gpio_base_t gpio_base;
    gpio_pin_t pin_num;
    IOResue_FUNCTION io_fun;
}sdio_ackio_info;

typedef struct
{
    sdio_revio_info rev_pin;
    sdio_ackio_info out_pin;
}sdio_pin_info;

//工作模式
typedef enum
{
    WORKING_FACTORY_MODE_LEVEL1  = 0xff,
    WORKING_FACTORY_MODE_LEVEL2  = 0xfe,
    WORKING_APPLICATION_MODE   = 0x01,
    //WORKING_FAIL_MODE
}work_factory_mode_typedef;

typedef enum{
    //语音识别相关
    LOCAL_ASR_RESULT_NOTIFY             =  0x0101,  /*本地语音识别通知*/
    WAKE_UP                             =  0x0102,  /*唤醒*/
    VAD_END                             =  0x0103,  /* 云端VAD END */
    SKIP_INVAILD_SPEAK                  =  0x0104,  /* 跳过无效语音 */
    PCM_MIDDLE                          =  0x0105,  /*PCM数据中间包*/
    PCM_FINISH                          =  0x0106,  /*PCM数据结束包*/
    PCM_IDLE                            =  0x0107,  /*PCM数据空闲 */
    VAD_START                           =  0x0108,  /*vad start*/ 
    EXIT_WAKE_UP                        =  0x0109,  /*退出唤醒*/
    SET_VAD_SENSITIVITY                 =  0x010A,  /*设置VAD灵敏度*/
    VAD_START_BY_KEY                    =  0x010B,  /*手动按键触发vad start*/
    VAD_END_BY_KEY                      =  0x010C,  /*手动触发vad end*/
    SET_AUDIO_EXIT_WAKE_UP              =  0x010D,  /*设置语音退出唤醒，带参数1字节1-带退出提示音(默认) 0-不带退出唤醒提示音*/
	PCM_DENOISE_ENABLE                  =  0x010E,  /*上传音频是否带功能降噪，带参数1字节 1-带降噪(默认) 0-不带降噪*/
	SET_VAD_FILTER_FRAME                =  0x010F,  /*过滤vad start和end之间帧数(16ms一帧)，小于该设定值被判断为无效语音，语音芯片不上传, 带参数2字节*/
	SET_VAD_SENSITIVITY_ACTIVATE_LENTH  =  0x0110,  /*多长时间的静音产生vad end，带参数1字节*/
	SET_VAD_START_MAX_TIMEOUT           =  0x0111,  /*设置vad start 最大时间，超过设置时间则强制结束上传语音，单位秒，默认5S，带参数2字节*/  
    SET_PLAY_VOICE_ID                   =  0x0112,  /*设置离线播放音频，带参数4字节(id号)*/ 
    SET_WAKE_UP_CONTINUE_TIME           =  0x0113,  /*设置持续唤醒时间，超过设置时间，语音芯片强制退出唤醒，带参数2字节(默认15S)*/
    SET_ENTER_WAKE_UP                   =  0x0114,  /*通过指令让语音芯片进入唤醒状态，不用等待唤醒词唤醒*/
    SET_INTERACTION_NULTI_ROUND_ENABLE =  0x0115,  /*设置单轮还是多轮，带参数1字节，0-单轮 1-多轮(默认)*/
    UPLOAD_PLAY_FULL_DUPLEX_EANBLE      =  0x0116,  /*设置播放音频时是否上传录音，带参数1字节 0-不上传(默认) 1-上传*/
    SET_AUDIO_VOLUME                    =  0x0117,  /*设置语音芯片音量，带参数1字节，范围(1-7), 默认7*/
    SET_AUDIO_COMPRESS_TYPE             =  0x0118,  /*压缩类型，默认只支持speex压缩，预留，暂时不做支持*/
    SET_VOLUME_MUTE_STATE               =  0X0119,   /*设置mute状态1-mute 0-非mute*/
    SET_AUDIO_START_RECORD              =  0x011A,   /*通过指令开始录音*/
    SET_AUDIO_STOP_RECORD               =  0x011B,   /*通过指令结束录音(和SET_AUDIO_START_RECORD必须配对使用)*/
    SET_CLOUD_ANS_TIMEOUT_EXIT          =  0x011C,   /*设置退出云端响应超时*/
	
    //网络播放相关
    NET_PLAY_START                      =  0x0201,   /*开始播放 */
    NET_PLAY_PAUSE                      =  0x0202,   /*播放暂停 */
    NET_PLAY_RESUME                     =  0x0203,   /*恢复播放 */
    NET_PLAY_STOP                       =  0x0204,   /*停止播放 */
    NET_PLAY_RESTART                    =  0x0205,   /*重播*/
    NET_PLAY_NEXT                       =  0x0206,   /*播放下一首 */
    NET_PLAY_LOCAL_TTS                  =  0x0207,   /*播放本地TTS */
    NET_PLAY_END                        =  0x0208,   /*播放结束*/
    NET_PLAY_RECONECT_URL               =  0x0209,   /*重新获取连接 */
    PLAY_DATA_GET                       =  0x020a,   /*获取后续播放数据 */
    PLAY_DATA_RECV                      =  0x020b,   /*接收播放数据 */
    PLAY_DATA_END                       =  0x020c,   /*播放数据接收完*/
    PLAY_TTS_END                        =  0x020d,   /*播放tts结束*/
    PLAY_EMPTY                          =  0x020e,   /*播放空指令 */
    PLAY_NEXT                           =  0x020f,   /*播放完上一首，主动播放下一首*/
    PLAYING_TTS                         =  0x0210,   /*当前正在播放TTS音频*/
    PLAY_RESUME_ERRO                    =  0x0211,   /*重播失败*/
    PLAY_LAST                           =  0x0212,   /*播放上一首*/
    PLAY_AUDIO_SIZE                     =  0x0213,   /*播放音频数据长度*/  
    PLAY_AUDIO_TYPE                     =  0x0214,   /*播放数据类型*/
    SET_AUDIO_PLAY_MODE                 =  0x0215,    /*设置播放模式，带参数1字节，1-打断当前播报 0-不打断当前播报，顺序播放默认)*/
    VAD_START_STOP_PLAY                 =  0x0216,    /*设置全双工模式下，vad start是否停止当前播放，带参数1字节，1-停止播放 0-不停止播放)*/
    LOCAL_AUDIO_PLAY_START              =  0x0217,    /*本地播放音频开始*/
    LOCAL_AUDIO_PLAY_STOP               =  0x0218,    /*本地播放音频开始*/
    SET_NET_PLAY_STATE                  =  0x0219,    /*设置播放状态，带参数1字节，1-开始播报 0-播报停止)*/
    MASTER_PLAY_STATE                   =  0x0220,    /*主控发送IIS播放状态*/

    //IOT自定义协议
    QCLOUD_IOT_CMD                      =  0x0301,   /*云端IOT指令 */
    NET_VOLUME                          =  0x0302,   /*云端音量 */
    LOCAL_VOLUME                        =  0x0303,   /*本地音量 */
    VOLUME_INC                          =  0x0304,   /*增大音量 */
    VOLUME_DEC                          =  0x0305,   /*减小音量 */
    VOLUME_MAXI                         =  0x0306,   /*最大音量 */
    VOLUME_MINI                         =  0x0307,   /*最小音量 */
    CIAS_CJSON_DATA                     =  0x0308,   //云端iot的json格式数据
    IOT_VOLUME_MUTE                     =  0x0309,    // 云端音量静音
    IOT_VOLUME_UNMUTE                   =  0x030a,   // 云端音量取消静音
    IOT_QUITE_WAKE_UP_MODE              =  0x030b,   // 退出唤醒模式
    GET_SW_VERSION                      =  0x030d,   // 获取软件版本信息
    //网络相关
    ENTER_NET_CONFIG                    =  0x0401,   //进入配网模式
    NET_CONFIGING                       =  0x0402,   //配网中
    EXIT_NET_CONFIG                     =  0x0403,   //退出配网模式
    INIT_SMARTCONFIG                    =  0x0404,   //初始密码状态 出厂配置状态
    WIFI_DISCONNECTED                   =  0x0405,   //网络断开
    WIFI_CONNECTED                      =  0x0406,   //网络连接成功
    GET_PROFILE                         =  0x0407,   //已获取鉴权文件
    NEED_PROFILE                        =  0x0408,   //需要鉴权文件，add by roy
    CLOUD_CONNECTED                     =  0x0409,   //云端已连接
    CLOUD_DISCONNECTED                  =  0x040a,   //云端已断开
    NET_CONFIG_SUCCESS                  =  0x040b,   //配网成功
    NET_CONFIG_FAIL                     =  0x040c,   //配网失败
    
    //and by yjd 
    CIAS_OTA_START                      =  0x0501,      //开始ota
    CIAS_OTA_DATA                       =  0x0502,      //ota 数据
    CIAS_OTA_SUCESS                     =  0x0503,      // OTA升级成功
    CIAS_FACTORY_START                  =  0x0504,      //生产测试
    CIAS_FACTORY_OK                     =  0x0505,      //生产测试成功
    CIAS_FACTORY_FAIL                   =  0x0506,      //生产测试失败
    CIAS_FACTORY_SELF_TEST_START        =  0x0507,      //自测试
    CIAS_IR_DATA                        =  0x0508,      //红外数据发送
    CIAS_IR_LOADING_DATA                =  0x0509,      //红外码库下载中
    CIAS_IR_LOAD_DATA_OVER              =  0x050a,      //红外码库下载完成
    CIAS_IR_LOAD_DATA_START             =  0x050b,      //红外下载码库开始
    CIAS_FACTORY_TEST_ENG_THR_SET       =  0x050c,      //音频通路设置检测播报音能量阈值设置，范围(0-255db)
    CIAS_FACTORY_TEST_RSLT_MICL         =  0x050d,       //MICL生产测试结果返回
    CIAS_FACTORY_TEST_RSLT_MICR         =  0x050e,       //MILR生产测试结果返回
    CIAS_FACTORY_TEST_RSLT_REFL         =  0x050f,       //REFL生产测试结果返回
    CIAS_FACTORY_TEST_RSLT_REFR         =  0x0510,       //REFR生产测试结果返回
    CIAS_FACTORY_TEST_REAL_VAL_MICL      = 0x0511,       //MICL生产通路测试过程中实时值上传
    CIAS_FACTORY_TEST_REAL_VAL_MICR      = 0x0512,       //MICR生产通路测试过程中实时值上传
    CIAS_FACTORY_TEST_REAL_VAL_REFL      = 0x0513,       //REFL生产通路测试过程中实时值上传
    CIAS_FACTORY_TEST_REAL_VAL_REFR      = 0x0514,       //REFR生产通路测试过程中实时值上传

    //语音系统相关
    CIAS_AUDIO_SYS_READY                = 0x0601,       //语音系统ready
    CIAS_AUDIO_SYS_ERR                  = 0x0602,       //语音系统异常 
    CIAS_AUDIO_RST                      = 0x0603,        //复位语音芯片
	//自学习相关
    CWSL_UART_REGISTRATION_WAKE         = 0x0701,         /*串口进入学习唤醒词*/
    CWSL_UART_REGISTRATION_WAKE_ING     = 0x0702,         /*正在学习唤醒词状态中*/
    CWSL_UART_REGISTRATION_WAKE_END_SUCCESSFUL = 0x0703,  /*学习唤醒词成功*/
    CWSL_UART_REGISTRATION_WAKE_END_FAILED = 0x0704,      /*学习唤醒词失败*/
    CWSL_UART_EXIT_REGISTRATION            = 0x0705,      /*退出学习*/
    CWSL_UART_DELETE_WAKE                  = 0x0706,      /*删除唤醒词*/
    //指令执行状态相关
    CIAS_CMD_EXEC_STATE                    =  0x0801,     /*指令执行状态，返回数据部分3字节 前两字节表示语音芯片接收到指令的类型， 第3字节表示指令执行结果(0x01-执行成功 0x02-执行失败)*/

   //===注意用户使用地址请从0x4xxx开始，避免和系统指令冲突
}wifi_communicate_cmd_t;

typedef enum
{
    WIFI_CONNECTED_STATE                =  0x0101,  //wifi链接成功：获取成功IP，但未登录目标云
    WIFI_CONFIGING_STATE                =  0x0102,  //配网中
    WIFI_DISCONNECT_STATE               =  0x0103,  //wifi断开链接：失去IP
    CLOUD_CONNECTED_STATE               =  0x0104,  //与目标云端建立链接（腾讯）
    CLOUD_DISCONNECT_STATE              =  0x0105,  //断开目标云端的链接（腾讯）
    //RECV_NET_CONNECTED_CMD              =  0x0106,   // 网络连接成功
    //note！！！ 后面需要对不同的云添加状态，例如同时链接腾讯和阿里时，需要根据不同功能进行添加
}wifi_curr_state_t;

#define INVAILD_SPEAK                (0x12345666)
#define RECV_TTS_PLAY                (0x12345677)
#define RECV_MP3_PLAY                (0x12345688)
#define RECV_M4A_PLAY                (0x123456aa)
#define IDLE_STATUS_RECV_M4A_PLAY    (0x123456ab)
#define RECV_WAV_PLAY                (0x123456bb)
#define DEF_FILL                     (0x12345678)
#define WAKEUP_FILL_DATA             (0x12345678)
#define NO_WAKEUP_FILL_DATA          (0x0)
    
    
/*******OTA****************/
/*OTA defines*/
#define DL_OTA_REQ                      0x0001  
#define DL_OTA_RSP                      0x0002
#define DL_LOGIN_INFO_REQ               0x0003
#define DL_LOGIN_INFO_RSP               0x0004
#define DL_VER_REQ                      0x0005
#define DL_VER_RSP                      0x0006
#define DL_FILE_CONFIG_REQ              0x04
#define DL_FILE_CONFIG_RSP              0x04
#define DL_QUERY_READY_REQ              0x05
#define DL_QUERY_READY_RSP              0x05
#define DL_AREA_INFO_REQ                0x06
#define DL_AREA_INFO_RSP                0x06
#define DL_ERASE_AREA_REQ               0x07
#define DL_ERASE_AREA_RSP               0x07
#define DL_WRITE_DATA_REQ               0x08
#define DL_WRITE_DATA_RSP               0x08
#define DL_WRITE_DATA_COMPLETE_REQ      0x09
#define DL_WRITE_DATA_COMPLETE_RSP      0x09
#define DL_VERIFY_AREA_REQ              0x0a
#define DL_VERIFY_AREA_RSP              0x0a
#define DL_WRITE_CONFIG_REQ             0x10
#define DL_WRITE_CONFIG_RSP             0x10
#define DL_WRITE_VER_REQ                0x11
#define DL_WRITE_VER_RSP                0x11
#define DL_COMPLETE_OTA_REQ             0x0e
#define DL_COMPLETE_OTA_RSP             0x0e
#define DL_CMD_ACK                      0x001b
#define DL_RESET_SYS                    0xfe
#define DL_OTA_TEST                     0xff
    
    
    
/*数据包解析*/
    
#pragma pack(1)
typedef struct
{
    unsigned int    send_time;//重发的系统时间
    unsigned int    send_num;//k重发次数
    unsigned int    cmd;
    unsigned char   *cmd_buf; //2KB buf做重发使用
    unsigned int    flag;
    unsigned int    length;
} pro_wait_acktypedef;
#pragma pack()
    
    
#pragma pack(1)
typedef struct
{
    unsigned int    head;
    unsigned short  checksum;
    unsigned short  cmd;
    unsigned short  length;
    unsigned short  version;
}pro_headpart_typedef;
#pragma pack()
    
typedef enum network_data_type_t
{
    CMD_AND_DATA    = 1,
    ONLY_CMD        = 2,
    AUDIO_START     = 3,
    AUDIO_MIDDLE    = 4,
    AUDIO_END       = 5,
}network_data_type;
    
#pragma pack(1)
typedef struct wifi_module_sync_info
{
    unsigned char wifi_link_status; /*0 未连接网络; 1 已连接网络*/
}wifi_module_sync_info_t;
#pragma pack()

typedef enum
{
    PLAYER_PLAY_IDEL        = 0,
    PLAYER_PLAY_LOCAL_TTS   = 1,
    PLAYER_PLAY_NET_TTS     = 2,
    PLAYER_PLAY_NET_M4A,
    PLAYER_PAUSE_NET_M4A,
    PLAYER_STOP_NET_M4A,
    PLAYER_PLAY_NET_MP3,
    PLAYER_PAUSE_NET_MP3,
    PLAYER_STOP_NET_MP3,
    PLAYER_PLAY_NET_WAVE,
    PLAYER_PAUSE_NET_WAVE,
    PLAYER_STOP_NET_WAVE,
}local_play_status;

typedef enum
{
    WAKEUP_STATUS,
    WIFI_LINK_STATUS,
    LOCAL_PLAYER_STATUS,
    ELECTRONIC_STATUS,
    ELECTRONIC_DOOR_STATUS,
    WIFI_COMMUNICATION_STATUS,
    MCU_SEND_PLAYER_CONTROL_STATUS,
    CI_REPLY_MCU_FLAG_STATUS,
}device_sync_status_t;




typedef struct
{
    uint8_t wakeup_time;
    uint8_t volume;
    uint8_t direct_recognition_state;
    uint8_t recognize_state;
    uint8_t auto_play;
    uint8_t auto_play_flag;
    uint8_t inited;
}recive_uart_data;


typedef enum
{
    CI_PCM_START       = 0,
    CI_PCM_MIDDLE,
    CI_PCM_END,
    CI_PCM_TIMEOUT,
    CI_NET_TIMEOUT,
    CI_CANCLE_UPDATE,
    LOCAL_TIMEOUT,
    CLOUD_RETURN_VAILD,
    CLOUD_RETURN_INVAILD,
}ci_voice_msg_type;


typedef enum
{
    VOICE_PROCESS_NULL  = 0,
    VOICE_PROCESS_RECV,
    VOICE_PROCESS_SEND,
}vad_voice_deal_type;


typedef enum
{
    NET_PLAYER_IDLE     = 0,
    NET_PLAYER_READY    = 1,
    NET_PLAYER_START    = 2,
    NET_PLAYER_PAUSE    = 3,
    NET_PLAYER_STOP     = 4,
    NET_PLAYER_RESUME   = 5,
}net_player_status_t;


typedef enum
{
    PLAY_NULL   = 0,
    PLAY_MP3    = 1,
    PLAY_TTS    = 2,
    PLAY_M4A    = 3,
    PLAY_WAV    = 4,
}net_player_type_t;


typedef struct
{
    uint32_t data_addr;
    uint16_t data_length;
    char no_need_response_cloud;
    ci_voice_msg_type type;
}voice_msg_t;


typedef enum
{
    NO_VOICE_DATA   = 0,
    VAILD_VOICE     = 1,
    INVAILD_VOICE   = 2,
    WAIT_RESPONSE   = 3,
}voice_type_t;

typedef struct
{
    uint32_t wifi_msg_index;
}sdio_msg_t;

typedef enum
{
    MSG_FIND_HEAD   = 0,
    MSG_RECV_MSG    = 1,
    MSG_VERIFY      = 2,
}sdio_recv_state_t;

#pragma pack(1)
typedef struct
{
    unsigned short head;
    unsigned short length;
    unsigned char msg_type;
    unsigned char msg_cmd;
    unsigned char msg_seq;
    unsigned char *data;
    unsigned short crc;
    unsigned char tail;
}ota_header_typedef;
#pragma pack()

extern audio_play_os_stream_t tts_player ;
extern audio_play_os_sem_t tts_player_end ;
extern audio_play_os_stream_t mp3_player ;
extern audio_play_os_sem_t mp3_player_end ;
extern audio_play_os_stream_t m4a_player ;
extern audio_play_os_sem_t m4a_player_end ;
extern audio_play_os_stream_t wav_player ;
extern audio_play_os_sem_t wav_player_end ;
extern volatile bool start_recv_flag ;//向wifi请求数据
extern audio_play_os_stream_t cur_play_stream;

bool cmd_need_ack(uint16_t cmd);
void network_recv_data_task(void *parameter);
void send_exit_wakeup_msg(char debug);
void upload_voice_vad_timeout_cb(uint32_t voice_size);

int32_t wifi_current_state_get(void);
int32_t wifi_current_state_set(int32_t arg);
void request_play_data_task(void *parameter);
int32_t wifi_msg_deal(uint8_t *msg_buf, int32_t msg_len);
#ifdef __cplusplus
    }
#endif

#endif
