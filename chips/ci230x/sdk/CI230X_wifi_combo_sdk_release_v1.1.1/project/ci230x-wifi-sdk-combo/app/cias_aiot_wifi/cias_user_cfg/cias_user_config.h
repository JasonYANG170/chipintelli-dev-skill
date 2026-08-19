#ifndef __CIAS_SDK_CONFIG__
#define __CIAS_SDK_CONFIG__
#include "cias_freertos_task.h"
#include "Config.h"   //CmakeList.txt中生成

#define PROJECT_NAME       "tuya_ir_remote"   //涂鸦红外遥控器
#define PROJECT_VERSION    "v1.0.1"           //涂鸦红外遥控器版本

#define CIAS_HARD_VERSION_MAJOR    3
#define CIAS_HARD_VERSION_MIDDLE   0
#define CIAS_HARD_VERSION_MINOR    0

#define  AUDIO_SPEEX_WORK       1 //audio 侧speex是否开启，1开启，0未开启 ，需check audio侧源码



//AIOT通讯串口编号
#define AIOT_AUDIO_UART_NUM          SER_PORT_UART2     //串口2
#define AIOT_AUDIO_UART_BAND_RATE    1000000 //        1MBps 

/****************************************云平台功能配置 *************************************************/


/***************************************调试功能配置****************************************************/
#define TEST_MODE_EN         1   // 是否进入调试模式
#define PCM_DATA_DEAL        1   // 1 采用缓冲区 0 采用队列

#if PCM_DATA_DEAL
#define PCM_BUFF_COUNT          (140) //pcm数据缓冲区大小 
#define PCM_BUFF_COUNT_MIN       (1) //pcm数据缓冲区大小 
#define PCM_BUFF_LEN            (43)//pcm数据大小 
#endif

#if CIAS_IOT_TVS_ENABLE
#define NET_AUDIO_DOWNLOAD_ENABLE       1  //网络音频是否开启
#endif
#if CIAS_IOT_TENCENT_ENABLE
#define TENCENT_AP_NET_WORK_EN         0   //是否开启 腾讯连连小程序ap配网功能   1是 0否   //add by hw
#endif

/****************************************用户任务资源配置 *************************************************/

//执行任务队列executor（包括授权、智能语音对话、媒体播控、数据上报.....） 
#ifndef EXECUTOR_TASK_NAME
#define EXECUTOR_TASK_NAME      "tvs_executor"   //任务名称 
#endif

#ifdef EXECUTOR_TASK_NAME
#define EXECUTOR_TASK_SIZE      (256*5)
#define EXECUTOR_TASK_PRIO      (5)
#endif
//建立下行通道
#ifndef DOWN_TASK_NAME
#define DOWN_TASK_NAME      "tvs_down" //任务名称 
#endif

#ifdef DOWN_TASK_NAME
#define DOWN_TASK_SIZE      (1024*2)
#define DOWN_TASK_PRIO      (4)
#endif

//mebedtls 库缓冲区大小
#define CIAS_MBEDTLS_SSL_OUT_CONTENT_LEN  2048   //数据上传缓冲区最大2KB，下载最大缓冲区系统定义为16K
//腾讯IOT相关（MQTT，数据上下行.....)
#ifndef QCLOUD_IOT_TASK_NAME
#define QCLOUD_IOT_TASK_NAME      "cias_iot_tencent_main"    //任务名称 
#endif
#ifndef QCLOUD_IOT_TASK_STACK_DEPTH  
#define QCLOUD_IOT_TASK_SIZE       1024*5         //任务空间20K
#endif
#ifndef QCLOUD_IOT_TASK_PRIORITY           
#define QCLOUD_IOT_TASK_PRIORITY         4         //任务优先级
#endif

//腾讯TVS相关
#ifndef TVS_TASK_NAME
#define TVS_TASK_NAME       "cias_iot_tvs_main" //任务名称 
#endif
#ifndef TVS_TASK_SIZE
#define TVS_TASK_SIZE      (1024*8)        //任务空间8K
#endif
#ifndef TVS_TASK_PRIORITY           
#define TVS_TASK_PRIORITY   4               //任务优先级
#endif
#define TVS_HTTP_DOWNLOAD_BUF_SIZE          1024*1 + 1//tvs http取数据buf大小10KB
//阿里IOT相关
#ifndef QCLOUD_Ali_TASK_NAME
#define QCLOUD_Ali_TASK_NAME       "cias_iot_ali_main" //任务名称 
#endif
#ifndef QCLOUD_Ali_TASK_SIZE
#define QCLOUD_Ali_TASK_SIZE      (1024*8)            //任务空间8K
#endif
#ifndef QCLOUD_Ali_TASK_PRIORITY           
#define QCLOUD_Ali_TASK_PRIORITY   4                  //任务优先级
#endif
//tuya IOT相关
#ifndef QCLOUD_TUYA_TASK_NAME
#define QCLOUD_TUYA_TASK_NAME      "cias_iot_tuay_main"    //任务名称 
#endif
#ifndef QCLOUD_TUYA_TASK_STACK_DEPTH  
#define QCLOUD_TUYA_TASK_SIZE       1024*8        //任务空间8K
#endif
#ifndef QCLOUD_TUYA_TASK_PRIORITY           
#define QCLOUD_TUYA_TASK_PRIORITY         4         //任务优先级
#endif
#define APP_BIN_NAME "cias_aiot_tuya_bin"
#define USER_SW_VER "1.0.0"

//TVS
//cias aiot
#define AIOT_CLOUD_TENCENT_AUTH_PROFILE                      "cloud_tencent_auth_profile"   //and by yjd
#define AIOT_CLOUD_TVS                                       "cloud_tvs_auth_profile"

//开机连接网络，AP配网
#ifndef AP_NET_TASK_NAME
#define AP_NET_TASK_NAME            "cias_ap_net"   //任务名称 
#endif

#ifdef AP_NET_TASK_NAME
#define AP_NET_TASK_SIZE            (1024*2)        //任务大小
#define AP_NET_TASK_PRIO            (4)             //任务优先级
#define AP_NET_DEBUG                1               //softAP配网debug打印使能（0：不使能，1：使能）
#define AUDIO_DISTRIBUTION_NET_CMD  2               //audio端“开始配网”命令词ID
#endif

//AIRKISS 动态任务
#ifndef AIRKISS_TASK_NAME 
#define AIRKISS_TASK_NAME     "airkiss" //任务名称 
#endif

#ifdef AIRKISS_TASK_NAME
#define AIRKISS_TASK_SIZE      (1024*4)
#define AIRKISS_TASK_PRIO      (20)
#endif

//播放远端音频（http连接，音频下载，传输）
#ifndef MEDIA_TASK_NAME 
#define MEDIA_TASK_NAME     "cias_media_task" //任务名称 
#endif

#ifdef MEDIA_TASK_NAME
#define MEDIA_TASK_SIZE      (1024*3)
#define MEDIA_TASK_PRIO      (4)
#endif

//同步网络连接状态 to TVS底层
#ifndef WLAN_STATUS_TASK_NAME 
#define WLAN_STATUS_TASK_NAME     "wlan_status_handler" //任务名称 
#endif

#ifdef WLAN_STATUS_TASK_NAME
#define WLAN_STATUS_TASK_SIZE      (1024)
#define WLAN_STATUS_TASK_PRIO      (4)
#endif

//向从端发送数据
#ifndef SEND_SLAVE_DATA_TASK_NAME 
#define SEND_SLAVE_DATA_TASK_NAME     "send_slave_msg" //任务名称 
#endif

#ifdef  SEND_SLAVE_DATA_TASK_NAME
// #define SEND_SLAVE_DATA_TASK_SIZE      (1024*1)
#define SEND_SLAVE_DATA_TASK_SIZE      (1024*2)
#define SEND_SLAVE_DATA_TASK_PRIO      (4)
#endif

//接收从端数据

#ifndef RECV_SLAVE_DATA_TASK_NAME
#define RECV_SLAVE_DATA_TASK_NAME     "recv_slave_msg" //任务名称  cias_slave_message_recv_task
#endif

#ifdef  RECV_SLAVE_DATA_TASK_NAME
#define RECV_SLAVE_DATA_TASK_SIZE      (1024*2)
#define RECV_SLAVE_DATA_TASK_PRIO      (5)
#endif



//从端数据处理
#ifndef DEAL_RECV_SLAVE_DATA_TASK_NAME 
#define DEAL_RECV_SLAVE_DATA_TASK_NAME     "slave_msg_recv_deal" //任务名称 
#endif


#ifdef  DEAL_RECV_SLAVE_DATA_TASK_NAME
#if CIAS_IOT_TUYA_ENABLE
#define DEAL_ADUIO_RECV_TASK_SIZE      (1024*5)   //涂鸦平台资源内存要求更高
#else
#define DEAL_ADUIO_RECV_TASK_SIZE      (1024*4)
#endif
#define DEAL_AUDIO_RECV_TASK_PRIO      (4)
#endif



//IOT 相关队列参数  
#define SLAVE_DATA_SEND_QUEUE_ITEAM_COUNT   2         //队列包含元素个数    
#define SLAVE_DATA_RECV_QUEUE_ITEAM_COUNT   5         //队列包含元素个数

#if CIAS_IOT_TUYA_ENABLE
#define SLAVE_DATA_SEND_QUEUE_ITEAM_SIZE    (1024+50)   //队列中每个元素的最大长度, 涂鸦红外码库长度大概230字节左右
#elif NET_AUDIO_DOWNLOAD_ENABLE
#define SLAVE_DATA_SEND_QUEUE_ITEAM_SIZE    (TVS_HTTP_DOWNLOAD_BUF_SIZE + 50)   //队列中每个元素的最大长度
#else 
#define SLAVE_DATA_SEND_QUEUE_ITEAM_SIZE    (128+50)    //队列中每个元素的最大长度
#endif

#define SLAVE_DATA_RECV_QUEUE_ITEAM_SIZE (64)      //队列中每个元素的最大长度，


//部分系统任务未做修改
#ifndef KMSGBK_TASK_NAME 
#define KMSGBK_TASK_NAME     "kmsgbk"
#endif
#ifdef KMSGBK_TASK_NAME
#define KMSGBK_TASK_SIZE      (1024*2)
#define KMSGBK_TASK_PRIO      (3)   
#endif


#ifndef WPAS_TASK_NAME 
#define WPAS_TASK_NAME     "wpas_thread"
#endif
#ifdef WPAS_TASK_NAME
#define WPAS_TASK_SIZE      (4000)
#define WPAS_TASK_PRIO      (5)   // use THD_WPAS_PRIORITY
#endif

#define test_ota_mode 0

#if 0
#ifndef CORE_TASK_NAME 
#define CORE_TASK_NAME     "core_thread"
#endif
#ifdef CORE_TASK_NAME
#define CORE_TASK_SIZE      (1024*2)
#define CORE_TASK_PRIO      (2)
#endif

#ifndef SHELL_TASK_NAME 
#define SHELL_TASK_NAME     "tshell"
#endif
#ifdef SHELL_TASK_NAME
#define SHELL_TASK_SIZE      (1024*8)
#define SHELL_TASK_PRIO      (20)
#endif

#ifndef SARADC_TASK_NAME 
#define SARADC_TASK_NAME     "saradc_intf"
#endif
#ifdef SARADC_TASK_NAME
#define SARADC_TASK_SIZE      (512)
#define SARADC_TASK_PRIO      (18)
#endif

#ifndef NTP_SYNC_TASK_NAME 
#define NTP_SYNC_TASK_NAME     "ntp_sync"
#endif
#ifdef NTP_SYNC_TASK_NAME
#define NTP_SYNC_TASK_SIZE      (1536)
#define NTP_SYNC_TASK_PRIO      (26)
#endif

#ifndef TCPIP_TASK_NAME 
#define TCPIP_TASK_NAME     "tcpip"
#endif
#ifdef TCPIP_TASK_NAME
#define TCPIP_TASK_SIZE      (1024*2)
#define TCPIP_TASK_PRIO      (4)
#endif

#ifndef TIMER_TASK_NAME 
#define TIMER_TASK_NAME     "timer"
#endif
#ifdef TIMER_TASK_NAME
#define TIMER_TASK_SIZE      (1024*4)
#define TIMER_TASK_PRIO      (6)
#endif

#ifndef TIDLE_TASK_NAME 
#define TIDLE_TASK_NAME     "tidle"
#endif
#ifdef TIDLE_TASK_NAME
#define TIDLE_TASK_SIZE      (512)
#define TIDLE_TASK_PRIO      (31)
#endif
#endif  
#endif   //__CIAS_SDK_CONFIG__