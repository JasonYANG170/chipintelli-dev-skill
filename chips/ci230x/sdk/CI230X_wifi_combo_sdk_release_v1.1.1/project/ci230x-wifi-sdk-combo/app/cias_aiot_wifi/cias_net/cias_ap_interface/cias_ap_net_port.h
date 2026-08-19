
#ifndef _CIAS_WIFI_NET_PORT_H__
#define _CIAS_WIFI_NET_PORT_H__
#include "cias_user_config.h"
#define SSID_LEN_LMT        50		//限制wifi ssid字符串长度
#define PASSWD_LEN_LMT      50	//限制wifi password字符串长度
#if TENCENT_AP_NET_WORK_EN//add by hw			

#define WIFI_LOCK_ADDR      100
#define LN_RESET_FLAG_ADDR  150
#endif//TENCENT_AP_NET_WORK_EN
typedef struct
{
	unsigned char 	is_recv;  //接收状态  0:无, 1:接收到ssid/password
	char 			ssid[SSID_LEN_LMT];
	char 			password[PASSWD_LEN_LMT];
	char 			*network_token; //腾讯iot token
	void 			*upcb;			//udp pcb
}ci_ap_net_t;

#if TENCENT_AP_NET_WORK_EN// add by hw
typedef struct _network_init_typedef_st
{
    char wifi_mode;               /**< DHCP mode: @ref wlanInterfaceTypedef.*/
    char wifi_ssid[33];           /**< SSID of the wlan needs to be connected.*/
    char wifi_key[64];            /**< Security key of the wlan needs to be connected, ignored in an open system.*/
    char local_ip_addr[16];       /**< Static IP configuration, Local IP address. */
    char net_mask[16];            /**< Static IP configuration, Netmask. */
    char gateway_ip_addr[16];     /**< Static IP configuration, Router IP address. */
    char dns_server_ip_addr[16];   /**< Static IP configuration, DNS server IP address. */
    char dhcp_mode;                /**< DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
    char wifi_bssid[6];
    char reserved[26];
    int  wifi_retry_interval;     /**< Retry interval if an error is occured when connecting an access point,
                                     time unit is millisecond. */
} network_init_typedef_st;
#endif


void cias_save_ssid_and_passwd(const char *ssid,const char *passwd);

unsigned int cias_smatrconfig_mode_get(void);
void cias_smatrconfig_mode_set(unsigned int mode);
void set_token_status(unsigned char val);
unsigned char get_token_status(void);

int udp_json_parse(char *str);
char* cias_get_qcloud_iot_network_token(void);
void cias_free_qcloud_iot_network_token(void);
int ci_ap_net_port_init(void);

int cias_set_defult_ssid_passwd(void);

#endif   //_CIAS_WIFI_NET_PORT_H__

