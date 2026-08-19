#ifndef __CIAS_WIFI_NET__
#define __CIAS_WIFI_NET__

#include "cias_common.h"



typedef struct{
  uint8_t wifi_connect_state;    //wifi连接状态状态
}cias_wifi_param_t;

#define AP_NETWORK_T			5*2  	//响应下一条指令的最短时间(5s)
#define AP_NETWORK_TIMEOUT		300*2  	//配网超时(5分钟)
#define AP_NETWORK_STA_TIMEOUT	30		//sta连接超时(30s)

/**
 * ap配网状态枚举
*/
typedef enum
{
    eCI_AP_UDP_INIT_WIFI_S = 0,  	//wifi模块开启AP和UDP服务
    eCI_STA_INIT_WIFI_S,     		//wifi模块开启sta模式，连接wifi。
    eCI_IDEL_WIFI_S,         		//空闲状态
}ap_net_status_e;

typedef int(*ap_net_func_t)(void);

/**
 * ap配网硬件相关函数
*/
typedef struct 
{ 
	ap_net_func_t 			ap;
	ap_net_func_t 			exit_ap;
	ap_net_func_t 			sta;
	ap_net_func_t 			exit_sta;
	ap_net_func_t 			udp_enter;
	ap_net_func_t 			udp_exit;
	void(*delay_ms)			(unsigned int ms);
}cias_ap_net_hal_t;
/**
 * ap配网事件钩子函数
*/
typedef struct 
{ 
	ap_net_func_t 			successful;
	ap_net_func_t 			timeout;
	ap_net_func_t			sta_timeout;
	ap_net_func_t 			sta_connect_event;
	ap_net_func_t			sta_disconnect_event;
}cias_ap_net_hook_t;
/**
 * ap配网标志和钩子函数结构体
*/
typedef struct 
{   
	unsigned char 			sta_is_connected;  		//sta模式是否连接上wifi
	unsigned char 			start_net;				//开始配网	
	unsigned char 			exit_net;				//退出配网
	unsigned char 			is_busy;				//是否正在配网
	ap_net_status_e 		networking_s;
	cias_ap_net_hal_t		hal;
	cias_ap_net_hook_t		hook;
}ap_net_t;

unsigned char cias_get_sta_connect_status(void);
void cias_set_sta_connect_status(unsigned char flag);

void cias_start_ap_net(void);
void cias_exit_ap_net(void);

void cias_udp_recv_data_after_sta_mode(void);

int cias_ap_network_hooks_init(cias_ap_net_hal_t *hal, cias_ap_net_hook_t *hook);

unsigned char cias_get_net_is_busy(void);

void cias_ap_networking_loop(void);
#endif   //__CIAS_WIFI_NET__