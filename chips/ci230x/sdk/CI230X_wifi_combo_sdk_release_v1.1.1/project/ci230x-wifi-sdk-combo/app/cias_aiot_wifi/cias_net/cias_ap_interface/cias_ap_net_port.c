#include <stdio.h>
#ifndef PROJECT_WIFI_SSID
#define PROJECT_WIFI_SSID ""
#endif
#ifndef PROJECT_WIFI_PASSWORD
#define PROJECT_WIFI_PASSWORD ""
#endif
#include <string.h>
#include <stdlib.h>
#include "cias_log.h"
#include "cias_common.h"
#include "cJSON.h"
#include "cias_ap_net.h"
#include "cias_ap_net_port.h"
#include "lwip/udp.h"
#include "cias_user_config.h"
#if  TENCENT_AP_NET_WORK_EN//add by hw			
#include "log.h"
#include "wifi.h"
#include "ln_nvds.h"
#endif// TENCENT_AP_NET_WORK_EN

#include "cias_slave_message_handle.h"
#if ! TENCENT_AP_NET_WORK_EN//add by hw			
extern uint8_t gairkiss_entry_flag ;
#endif// TENCENT_AP_NET_WORK_EN
ci_ap_net_t ci_ap_net = {
    .is_recv = 0,
    .network_token = NULL,
    .upcb = NULL,
};

static unsigned int cias_smatrconfig_mode = 0;

unsigned int cias_smatrconfig_mode_get(void)
{
    return cias_smatrconfig_mode;
}

void cias_smatrconfig_mode_set(unsigned int mode)
{
    cias_smatrconfig_mode = mode;
}

void cias_save_ssid_and_passwd(const char *ssid,const char *passwd)
{
    if(ssid && passwd)
    {
        memset(ci_ap_net.ssid,0,sizeof(ci_ap_net.ssid));
        memset(ci_ap_net.password,0,sizeof(ci_ap_net.password));
        strcpy(ci_ap_net.ssid,ssid);
        strcpy(ci_ap_net.password,passwd);
#if ! TENCENT_AP_NET_WORK_EN//add by hw			
        __set_ssid_passwd_to_flash();
#endif// TENCENT_AP_NET_WORK_EN
        CIAS_LOG_HL("airkiss:save:ssid:%s,pwd:%s", ssid,passwd);
    }
}

int __send_wifi_connect_statue(unsigned char statue)
{
    // CIAS_LOG_HL("[1103]wifi:%d", statue);
    // cias_olcp_send_cmd_data_check_crc8_ack(ci_get_olcp_handle(), IOT_WIFI_CONNECT_OLCP, &statue, 1);
    return 0;
}

char* cias_get_qcloud_iot_network_token(void)
{
    return ci_ap_net.network_token;
}

void cias_free_qcloud_iot_network_token(void)
{
    cias_free(ci_ap_net.network_token);
}

#if !TENCENT_AP_NET_WORK_EN//add by hw			
#define SSID_FLASH_KEY       "cias_ssid"
#define PASSWD_FLASH_KEY     "cias_passwd"
int __set_ssid_passwd_to_flash(void)
{
    #if 0
    EfErrCode status;
    /* 保存SSID */
    status = ef_set_and_save_env(SSID_FLASH_KEY,ci_ap_net.ssid);
    CIAS_ASSERT_RETURN(status==EF_NO_ERR);
    /* 保存PASSWORD */
    status = ef_set_and_save_env(PASSWD_FLASH_KEY,ci_ap_net.password);
    CIAS_ASSERT_RETURN(status==EF_NO_ERR);
    #endif
    return 0;
}

int __get_ssid_passwd_from_flash(void)
{
    char *ssid = NULL;
    char *password = NULL;

    ssid = ef_get_env(SSID_FLASH_KEY);
    CIAS_ASSERT_RETURN(ssid!=NULL);
    strcpy(ci_ap_net.ssid, ssid);

    password = ef_get_env(PASSWD_FLASH_KEY);
    CIAS_ASSERT_RETURN(password!=NULL);
    strcpy(ci_ap_net.password, password);

    CIAS_LOG_HL("read flash ssid:[%s]",ci_ap_net.ssid);
    CIAS_LOG_HL("read flash passwd:[%s]\n",ci_ap_net.password);

    return 0;
}
#endif// TENCENT_AP_NET_WORK_EN

int cias_set_defult_ssid_passwd(void)
{
    // char *ssid = PROJECT_WIFI_SSID;
    // char *password = PROJECT_WIFI_PASSWORD;
    char *ssid = PROJECT_WIFI_SSID;
    char *password = PROJECT_WIFI_PASSWORD;


    strcpy(ci_ap_net.ssid, ssid);
    strcpy(ci_ap_net.password, password);
#if ! TENCENT_AP_NET_WORK_EN//add by hw			
	__set_ssid_passwd_to_flash();
#endif// TENCENT_AP_NET_WORK_EN
    CIAS_LOG_HL("cias_set_defult_ssid_passwd\n");
    return 0;
}


static int __udp_json_parse(char *str)
{
    cJSON *json  = NULL;  
    cJSON *json_value  = NULL;  
    int string_len, ret = -1;

    json = cJSON_Parse(str);

    json_value = cJSON_GetObjectItem(json, "token");
    CIAS_ASSERT_GOFAIL(json_value!=NULL);
    CIAS_ASSERT_GOFAIL(json_value->type==cJSON_String);
    cias_free_qcloud_iot_network_token();
    string_len = strlen(json_value->valuestring);
    ci_ap_net.network_token = cias_malloc(string_len+1);
    CIAS_ASSERT_GOFAIL(ci_ap_net.network_token!=NULL);
    strcpy(ci_ap_net.network_token, json_value->valuestring);

    json_value = cJSON_GetObjectItem(json, "ssid");
    CIAS_ASSERT_GOFAIL(json_value!=NULL);
    CIAS_ASSERT_GOFAIL(json_value->type==cJSON_String);
    string_len = strlen(json_value->valuestring);
    CIAS_ASSERT_GOFAIL(string_len<=SSID_LEN_LMT);
    strcpy(ci_ap_net.ssid, json_value->valuestring);

    json_value = cJSON_GetObjectItem(json, "password");
    CIAS_ASSERT_GOFAIL(json_value!=NULL);
    CIAS_ASSERT_GOFAIL(json_value->type==cJSON_String);
    string_len = strlen(json_value->valuestring);
    CIAS_ASSERT_GOFAIL(string_len<=PASSWD_LEN_LMT&&string_len>0);
    strcpy(ci_ap_net.password, json_value->valuestring);

    CIAS_LOG_HL("udp recv ssid:%s, passwd:%s, token:%s", ci_ap_net.ssid, ci_ap_net.password, ci_ap_net.network_token);
#if ! TENCENT_AP_NET_WORK_EN//add by hw			
    __set_ssid_passwd_to_flash();
#endif// TENCENT_AP_NET_WORK_EN
    ci_ap_net.is_recv = 1;
    ret = 0;
__fail:
    cJSON_Delete(json);

    return ret;
}

int udp_json_parse(char *str)
{
    return __udp_json_parse(str);
}

//UDP接收回调函数
static void __udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip4_addr_t *addr, uint16_t port)
{
    int rc;
    // char *reply_msg = "{\"cmdType\":2,\"productId\":\""CI_PRODUCT_ID"\",\"deviceName\":\""CI_DEVICE_NAME"\",\"protoVersion\":\"2.0\"}";
    struct pbuf *send_pbuf = NULL;
    char reply_msg[220];
    memset(reply_msg,'\0',sizeof(reply_msg));
    // strcat(reply_msg,"{\"cmdType\":2,\"productId\":\"");
    // strcat(reply_msg,ci_mqtt_get_product_id());
    // strcat(reply_msg,"\",\"deviceName\":\"");
    // strcat(reply_msg,ci_mqtt_get_device_name());
    // strcat(reply_msg,"\",\"protoVersion\":\"2.0\"}");
    #if CIAS_IOT_TENCENT_ENABLE
    sprintf(reply_msg,"{\"cmdType\":2,\"productId\":\"%s\",\"deviceName\":\"%s\",\"protoVersion\":\"2.0\"}",ci_mqtt_get_product_id(),ci_mqtt_get_device_name());
    #endif
    //打印接收到的数据
    CIAS_LOG_DEBUG("udp get msg from %d.%d.%d.%d:%d", *((uint8_t *)&addr->addr), *((uint8_t *)&addr->addr + 1),
            *((uint8_t *)&addr->addr + 2), *((uint8_t *)&addr->addr + 3), port);
    
    if (p == NULL)
    {
        CIAS_LOG_ERR("["__FILE__":%d] p==NULL", __LINE__);
    }

    CIAS_LOG_DEBUG("udp recv data:%s", (char *)p->payload);
    __udp_json_parse((char *)p->payload);

    CIAS_LOG_DEBUG("[udp]reply: %s",reply_msg);
    send_pbuf = pbuf_alloc(PBUF_TRANSPORT, strlen(reply_msg), PBUF_POOL);
    pbuf_take(send_pbuf, reply_msg, strlen(reply_msg));
    udp_sendto(upcb, send_pbuf, addr, port);
    cias_task_delay_ms(200);

    //释放缓冲区数据
    pbuf_free(p);  p = NULL;
    if(send_pbuf != NULL)
    {
        pbuf_free(send_pbuf); send_pbuf = NULL;
    }
    //回调设置配网为STA模式
    cias_udp_recv_data_after_sta_mode();
}
#if  TENCENT_AP_NET_WORK_EN//add by hw			
extern void wifi_init_ap(void);
#endif// TENCENT_AP_NET_WORK_EN
static int __wifi_ap_mode(void)
{
#if  TENCENT_AP_NET_WORK_EN//add by hw			
    wifi_softap_cfg_t * net_config;

    wifi_stop();
    wifi_init_ap();
#endif// TENCENT_AP_NET_WORK_EN
    return 0;
}

static int __udp_server_enter(void)
{
    //创建UDP控制块
    ip4_addr_t server_ip;
    int rc;

    IP4_ADDR(&server_ip, 192, 168, 4, 1);

    if (ci_ap_net.upcb == NULL)
    {
        ci_ap_net.upcb = udp_new();
    }
    CIAS_ASSERT_RETURN(ci_ap_net.upcb!=NULL);
    //绑定端口号
    rc = udp_bind(ci_ap_net.upcb, &server_ip, 8266);
    CIAS_ASSERT_RETURN(rc==0);
    //注册接收回调函数
    udp_recv(ci_ap_net.upcb, __udp_receive_callback, NULL);

    return 0;
}

static int __udp_server_exit(void)
{
    if (ci_ap_net.upcb != NULL)
    {
        udp_remove(ci_ap_net.upcb);
        ci_ap_net.upcb = NULL;
    }
    return 0;
}

static int __sta_mode_connect_wifi(void)
{
#if  TENCENT_AP_NET_WORK_EN//add by hw			
    wifi_stop();
    wifi_init_sta();
#endif// TENCENT_AP_NET_WORK_EN
    return 0;
}
#if  TENCENT_AP_NET_WORK_EN//add by hw			
extern void wifi_init_sta(void);
#endif// TENCENT_AP_NET_WORK_EN
static int __wifi_exit_ap_mode(void)
{
#if  TENCENT_AP_NET_WORK_EN//add by hw			
    // wifi_sta_start();
    wifi_stop();
    wifi_init_sta();
#endif// TENCENT_AP_NET_WORK_EN
    return 0;
}

static int __wifi_exit_sta_mode(void)
{
#if  TENCENT_AP_NET_WORK_EN//add by hw			
    wifi_stop();
#endif// TENCENT_AP_NET_WORK_EN
    return 0;
}

static void __ap_network_delay_ms(unsigned int ms)
{
    cias_task_delay_ms(ms);
}

static int __network_successful(void)
{
    unsigned short play_index = 1200;

    LOG(LOG_LVL_INFO,"network successful\r\n");
    cias_message_send_interface(SEND_WIFI_CONNECTED_CMD, DEF_FILL, 0, NULL);
    
    return 0;
}

static int __network_timeout(void)
{
    unsigned short play_index = 1040;

    CIAS_LOG_ERR("network timeout");
    cias_message_send_interface(QCLOUD_IOT_CMD, DEF_FILL, 2, (unsigned char*)(&play_index));
    
    return 0;
}

static int __network_sta_timeout(void)
{
    unsigned short play_index = 1041;

    CIAS_LOG_ERR("sta timeout");
    cias_message_send_interface(QCLOUD_IOT_CMD, DEF_FILL, 2, (unsigned char*)(&play_index));

    return 0;
}

extern bool ci_wifi_connect;

static int __sta_connect_event(void)
{
    ci_wifi_connect = true;
    //没有配网的时候才播报网络状态
    if (!cias_get_net_is_busy())
    {
        LOG(LOG_LVL_INFO,"网络空闲\n");
        //cias_message_send_interface(SEND_WIFI_CONNECTED_CMD, DEF_FILL, 0, NULL);   //remove by yjd
    }
}

static int __sta_disconnect_event(void)
{
    ci_wifi_connect = false;
    //没有配网的时候才播报网络状态
    if (!cias_get_net_is_busy())
    {
        LOG(LOG_LVL_INFO,"网络空闲\n");
        cias_message_send_interface(SEND_WIFI_DISCONNECTED_CMD, DEF_FILL, 0, NULL);   
    }
}
#if  TENCENT_AP_NET_WORK_EN//add by hw			
int wifi_softAP_connect_flag = 1;
int audio_softAP_cmd = 0;
#endif// TENCENT_AP_NET_WORK_EN
void ci_distribution_network_task(void *params)
{
    cias_ap_net_hal_t     ap_net_hal;
    cias_ap_net_hook_t    ap_net_hook;

    cias_task_delay_ms(3000);
    ap_net_hal.ap = __wifi_ap_mode;
    ap_net_hal.exit_ap = __wifi_exit_ap_mode;
    ap_net_hal.sta = __sta_mode_connect_wifi;
    ap_net_hal.exit_sta = __wifi_exit_sta_mode;
    ap_net_hal.udp_enter = __udp_server_enter;
    ap_net_hal.udp_exit = __udp_server_exit;
    ap_net_hal.delay_ms = __ap_network_delay_ms;

    ap_net_hook.sta_connect_event = __sta_connect_event;
    ap_net_hook.sta_disconnect_event = __sta_disconnect_event;
    ap_net_hook.sta_timeout = __network_sta_timeout;
    ap_net_hook.timeout = __network_timeout;
    ap_net_hook.successful = __network_successful;

    cias_ap_network_hooks_init(&ap_net_hal, &ap_net_hook);
    // cias_start_ap_net();
    while (1)
    {
#if  TENCENT_AP_NET_WORK_EN//add by hw			
        if(wifi_softAP_connect_flag || audio_softAP_cmd)
        {
            cias_start_ap_net();
            cias_ap_networking_loop();
        }
#endif// TENCENT_AP_NET_WORK_EN
        cias_task_delay_ms(1000);
    }
}

void * ci_ap_net_task_handle;
int ci_ap_net_port_init(void)
{
    int rc;
    cias_task_create(&ci_ap_net_task_handle,
					 AP_NET_TASK_NAME,
					 (cias_task_function_t)ci_distribution_network_task,
					 (void *)0,
					 AP_NET_TASK_PRIO,
					 (unsigned short)(AP_NET_TASK_SIZE));
    return 0;
}
