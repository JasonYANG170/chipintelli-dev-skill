/*
 * @Description: 
 * @Author: liutq
 * @Date: 2021-12-09 16:36:28
 * @LastEditTime: 2021-12-17 14:08:42
 * @LastEditors: hongchuan.wu
 * @Reference: 
 */
#include "cias_user_config.h"
#include "Config.h"
#if CIAS_IOT_CLOUD_HISENSE_ENABLE
#include <stdlib.h>
#include <stdbool.h>
#include "cias_log.h"
#include "cias_common.h"
#include <lwip/sockets.h>
#include "cias_freertos_task.h"
#include "cias_freertos_debug.h" 
#include "utils/system_parameter.h"

#include "hisense_main_task.h"
#include "hisense_wifi_message_deal.h"






#define  SEND_INFO17     "{\"Hcmd\":3,\"RSP\":%d,\"FLAG\":1,\"DevInfo\":{\"WifiId\":\"%s\","\
                        "\"DevId\":\"\",\"DevType\":\"1\",\"DevProductType\":\"105\"}}"

#define  SEND_INFO03     "{\"Hcmd\":3,\"RSP\":%d,\"DevInfo\":{\"WifiId\":\"%s\","\
                        "\"DevId\":\"\",\"DevType\":\"1\",\"DevProductType\":\"105\"}}"

#define  SEND_INFO3     "{\"Hcmd\":3,\"RSP\":%d,\"ENDFLAG\":0,\"WiFiArr\":[%s]}"
#define  SEND_INFO4     "{\"SSID\":\"%s\",\"BSSID\":\"%02x%02x%02x%02x%02x%02x\",\"RSSI\":\"%3d\"}"


typedef enum
{
    HISENSE_AP_CONTROL       = 4,
    HISENSE_AP_SEARCH_WIIF   = 15,
    HISENSE_AP_ENDSOCKET     = 16,
    HISENSE_AP_GET_INFO      = 17,
};

extern int8_t hisense_recv_buff[HISENSE_RECV_BUFF_LEN];
extern int8_t hisense_send_buff[HISENSE_SEND_BUFF_LEN];

extern wifi_sta_connect_t hisense_ap_connect;

int8_t hisense_ap_ssid[64] = {0};
int8_t hisense_ap_pwd[64] = {0};
int8_t hisense_ap_bssid[32] = {0};


int8_t hisense_ap_list_buff[HISENSE_RECV_BUFF_LEN];


int8_t g_HISENSE_TCP_SERVER_EXIT = 0; //ap 配网标志

static int hisense_ap_hcmd   = 0;
static int hisense_ap_seq    = 0;
static int hisense_ap_status = 10;//默认不为0

uint32_t g_hisense_ap_ticket = 0;

cias_task_t ci_hisense_ap_tcp_server; 

int hisense_ap_data_write(ap_info_t *ap_info)
{
    static int16_t data_len = 0;
    static int16_t count = 0;
    uint8_t * mac = (uint8_t*)ap_info->bssid;
    
    if(data_len == 0)
    {
        bzero(hisense_ap_list_buff, HISENSE_RECV_BUFF_LEN);
    }

    if((data_len+sizeof(ap_info_t)< HISENSE_RECV_BUFF_LEN))
    {

        if(count <= HISENSE_AP_LIST_COUNT_MAX)
        {
            if(data_len != 0)
            {
                hisense_ap_list_buff[data_len] = ',';
                data_len ++;
            }
            
            data_len += sprintf(&hisense_ap_list_buff[data_len],SEND_INFO4,ap_info->ssid,mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],ap_info->rssi);

            count ++;
        }
       
        return data_len;
    }

    return -1;     
}

static int   hisense_ap_json_parse(char *str)
{
    cJSON *json  = NULL;  
    cJSON *json_value  = NULL;  
    int string_len = 0, json_ret = -1;

    json = cJSON_Parse(str);

    json_value = cJSON_GetObjectItem(json,"Ticket");
    string_len = strlen(json_value->valuestring);
    if(json_value)
    {      
        g_hisense_ap_ticket = atoi(json_value->valuestring);
    }


    json_value = cJSON_GetObjectItem(json,"SSID");
    string_len = strlen(json_value->valuestring);
    if(json_value)
    {
        memcpy(hisense_ap_ssid,json_value->valuestring,string_len);
        hisense_ap_connect.ssid = hisense_ap_ssid;
    }

    json_value = cJSON_GetObjectItem(json,"BSSID");
    string_len = strlen(json_value->valuestring);
    if(json_value)
    {
        memcpy(hisense_ap_bssid,json_value->valuestring,string_len);
        hisense_ap_connect.bssid = hisense_ap_bssid;
    }
        
    json_value = cJSON_GetObjectItem(json,"PWD");
    string_len = strlen(json_value->valuestring);
    if(json_value)
    {
        memcpy(hisense_ap_pwd,json_value->valuestring,string_len);
        hisense_ap_connect.pwd = hisense_ap_pwd;
    }    

    json_value = cJSON_GetObjectItem(json,"STATUS");
    if(json_value->valueint == 0)
    {
        hisense_ap_status = json_value->valueint;
    }
    else
    {
        hisense_ap_status = 10;//默认不为0
    }  

    json_value = cJSON_GetObjectItem(json,"Hcmd");
    if(json_value->valueint)
    {
        CIAS_LOG_HL("tcp recv Hcmd:%d \r\n",json_value->valueint);
        hisense_ap_hcmd = json_value->valueint;        
    }
        
    json_value = cJSON_GetObjectItem(json,"SEQ");
    if(json_value->valueint)
    {
        CIAS_LOG_HL("tcp recv SEQ:%d \r\n",json_value->valueint);
        hisense_ap_seq = json_value->valueint;
    }

    cJSON_Delete(json);

    return json_ret;
}

static int16_t hisense_ap_recv_deal(int8_t * recv_ptr,int recv_len, int connfd)
{
    int16_t del_ret = 0;
    int8_t *deal_ptr = hisense_deal_temp;
    int8_t *send_ptr = hisense_send_buff;
    char *t_k = HISENSE_AP_KEY;

    bzero(deal_ptr, HISENSE_SEND_BUFF_LEN);
    bzero(send_ptr, HISENSE_SEND_BUFF_LEN);

    if(recv_ptr != NULL && recv_len > 0)
    {       
        hisense_aes_decrypt(recv_ptr,HISENSE_AP_KEY,deal_ptr);//原始数据进行aes解码

        CIAS_PRINT_DEBUG("read aes app: %s\r\n",deal_ptr);

        hisense_ap_json_parse(deal_ptr);

        switch (hisense_ap_hcmd)
        {
            case HISENSE_AP_GET_INFO:
            {
                if(hisense_ap_seq >0)
                {
                    bzero(deal_ptr, HISENSE_SEND_BUFF_LEN);

                    sprintf(deal_ptr,SEND_INFO17,hisense_ap_seq,WIFIID);

                    hisense_aes_encrypt(deal_ptr,t_k,send_ptr);

                    del_ret = strlen(send_ptr);   
                }                
                break;
            }
            case HISENSE_AP_ENDSOCKET:
            {
                if(hisense_ap_status == 0)//收到ENDSOCKET指令
                {

                    CIAS_LOG_HL("ap recv STATUS:%d \r\n",hisense_ap_status);

                    del_ret = 0;
                }
                break;
            }
            case HISENSE_AP_SEARCH_WIIF:
            {
                // sprintf(hisense_ap_list_buff,SEND_INFO4,"AIOT_TEST","0C4B54CDB19A","-47");
                bzero(deal_ptr, HISENSE_SEND_BUFF_LEN);

                sprintf(deal_ptr,SEND_INFO3,0,hisense_ap_list_buff);

                CIAS_PRINT_DEBUG(" %s\r\n",deal_ptr);

                hisense_aes_encrypt(deal_ptr,t_k,send_ptr);

                del_ret = strlen(send_ptr); 

                break;
            }
            case HISENSE_AP_CONTROL:
            {
                g_HISENSE_TCP_SERVER_EXIT = 1;

                CIAS_LOG_HL("ap recv SSID:%s PWD:%s BSSID:%s Ticket:%d\r\n",hisense_ap_connect.ssid,hisense_ap_connect.pwd,hisense_ap_connect.bssid,g_hisense_ap_ticket);

                bzero(deal_ptr, HISENSE_SEND_BUFF_LEN);

                sprintf(deal_ptr,SEND_INFO03,hisense_ap_seq,WIFIID);

                hisense_aes_encrypt(deal_ptr,t_k,send_ptr);

                del_ret = strlen(send_ptr);   

                break;
            }
            
            default:
                break;
        }

        hisense_ap_hcmd      = 0;
        hisense_ap_seq       = 0;     
        bzero(deal_ptr, HISENSE_SEND_BUFF_LEN);
    }   
    
    return del_ret;
}

static int listenfd,connfd; 
void ci_hisense_network_tcp_server_fun(void)
{
    int16_t tcp_tmp,tcp_ret = 0;
    struct  sockaddr_in serveraddr;
    struct  sockaddr_in clientaddr;
    
    
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serveraddr,0,sizeof(serveraddr));
    memset(&clientaddr,0,sizeof(clientaddr));
    
    bzero(hisense_send_buff, HISENSE_SEND_BUFF_LEN);
    bzero(hisense_recv_buff, HISENSE_RECV_BUFF_LEN);
    

    serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port   = htons(5020);


    tcp_ret = bind(listenfd,&serveraddr,sizeof(serveraddr));
    if(tcp_ret == -1)
    {
        CIAS_PRINT_DEBUG("hisense TCP bind failed \r\n");
        while (1);  
    }

    tcp_ret = listen(listenfd,10);
    if(tcp_ret == -1)
    {
        CIAS_PRINT_DEBUG("hisense TCP listen failed \r\n");
        while (1);  
    }
    CIAS_PRINT_DEBUG("hisense TCP listen : \r\n");

    while (1)
    {
        connfd = accept(listenfd,&clientaddr,tcp_tmp);
        if(connfd == -1)
        {
            CIAS_PRINT_DEBUG("hisense TCP accept failed \r\n");
            while (1); 
        }
        else
        {
            CIAS_PRINT_DEBUG("hisense TCP accept ok \r\n");
        }
       
        while(1)
        {
            tcp_tmp = read(connfd,hisense_recv_buff,HISENSE_RECV_BUFF_LEN); //2，接收app 数据 by tcp

            CIAS_PRINT_DEBUG("read from app(%d): %s\r\n",tcp_tmp,hisense_recv_buff);

            if(tcp_tmp>0)
            {
                hisense_recv_buff[tcp_tmp] = '\n';
                
                tcp_ret = hisense_ap_recv_deal(hisense_recv_buff,tcp_tmp,connfd);//3，处理app 发来的数据
                if( tcp_ret > 0)
                {
                    cias_task_delay_ms(100);

                    hisense_send_buff[tcp_ret] = '\0';
                    hisense_send_buff[tcp_ret+1] = '\n';                    
                    tcp_ret += 2;

                    tcp_ret = write(connfd,hisense_send_buff,tcp_ret);

                    CIAS_PRINT_DEBUG("write to app(%d,%d): %s \r\n\n",tcp_ret,strlen(hisense_send_buff),hisense_send_buff);
                    cias_task_delay_ms(20);
                    
                }

                bzero(hisense_recv_buff, HISENSE_RECV_BUFF_LEN);
                bzero(hisense_send_buff, HISENSE_RECV_BUFF_LEN);
                // CIAS_PRINT_DEBUG("Heap left: %d; min:%d; Task High Water:%d.\r\n", 
                // xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize(),uxTaskGetStackHighWaterMark(NULL));
                
                if(tcp_ret == 0)
                {
                    CIAS_PRINT_DEBUG("ap tcp disconnect1 !!!\r\n");//4.1，收到app 断开指令

                    cias_udp_recv_data_after_sta_mode();
                }
                
            }
            else if(tcp_tmp == -1)
            {
                CIAS_PRINT_DEBUG("ap tcp disconnect2 !!!\r\n");//4.2，监测到app 断开tcp 连接

                cias_udp_recv_data_after_sta_mode();
            }

            if(g_HISENSE_TCP_SERVER_EXIT) break;  

            cias_task_delay_ms(100);
        } 

        if(g_HISENSE_TCP_SERVER_EXIT) break;

        cias_task_delay_ms(1000);
    }

hisense_ap_err:

    cias_task_delete(NULL);

    return 0;
}

void ci_hisense_TCP_server_enter(void)//进入ap配网，1，开启tcp server
{
    cias_task_create(&ci_hisense_ap_tcp_server
                    , "ci_hisense_ap_tcp_server"
                    ,ci_hisense_network_tcp_server_fun
                    ,0
                    ,CIAS_APP_PRIORITY+1
                    ,(unsigned short)1024*25);

}

void ci_hisense_TCP_server_exit(void)//退出ap配网，关闭tcp server
{
    
    
    close(connfd);

    close(listenfd);
    
    CIAS_PRINT_DEBUG("ci_hisense_TCP_server_exit \r\n");

    // cias_task_delete(&ci_hisense_ap_tcp_server);

}

#endif  // CIAS_IOT_CLOUD_HISENSE_ENABLE