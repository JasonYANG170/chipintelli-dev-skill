/*
 * @Description: 
 * @Author: liutq
 * @Date: 2021-12-09 16:36:28
 * @LastEditTime: 2021-12-17 14:08:42
 * @LastEditors: hongchuan.wu
 * @Reference: 
 */
#include <stdbool.h>
#include "cias_freertos_task.h"
#include "cias_freertos_queue.h"
#include "cias_freertos_debug.h"  
#include "cias_slave_message_handle.h"
#include "cias_user_config.h"
#include "cias_common.h"
#include "utils/system_parameter.h"
#include "cias_ble_config.h"
#include "Config.h"   //CmakeList.txt中生成
#if CIAS_IOT_CLOUD_HISENSE_ENABLE
#include "cias_log.h"
#include "hisense_main_task.h"
#include "md5.h"
#include "mongoose.h"
#include "ln_aes.h"
#include "aes.h"
#include "mbedtls/cipher.h"


#define DEVICES_AUTH    "{\"id\":101,\"wid\":\"%s\",\"ver\":\"HMC300T.4.02.11.00\",\"ProtVer\":\"215\"}"
#define DEVICES_KEY     "{\"id\":102,\"token\":\"%s\"}"
#define DEVICES_SYS     "{\"id\":103,\"token\":\"%s\"}"
#define DEVICES_HB      "{\"id\":902}"

#define DEVICES_CLOUD   "{\"id\":602,\"token\":\"%s\",\"seq\":0,\"devid\":\"\",\"dataType\":\"ReportTicket\",\"data\":\"%s\"}"
#define DEVICES_DATA    "{\"SSID\":\"\",\"BSSID\":\"%s\",\"wid\":\"%s\",\"Ticket\":\"%d\"}"

#define DEVICES_STATUS_UP      "{\"id\":901,\"token\":\"%s\",\"err\":0,\"status\":[%s]}"
#define DEVICES_STATUS         "{\"msgtype\":30,\"msgsubtype\":0,\"result\":\"sAEAAhUAAAA=\"}, {\"msgtype\":102,\"msgsubtype\":0,\"result\":\"9PUBQEcBAP4BAgEBAGYAAS0WIUAAAAAAAEcAAAAAIAAAABUAAAAAAAAAAAcAKAAdKjEULg8ZCgABAAAAQAAAAAAAAAAAAAAAAAAAAARv9Ps=\"}"

#define HISENSE_SSID_STR       "HIS-W2-01-%02X%02X"

// adb logcat -v time D:\RIVEN\log\2022_10\log_

typedef enum
{
    HISENSE_AUTH             = 101,
    HISENSE_KEY_EXCHANGE     = 102,
    HISENSE_SYS_PARAM        = 103,
    HISENSE_CONTROL_CMD      = 201,
    HISENSE_CHECK_STATUS     = 202,
    HISENSE_OTA_CHECK        = 301,
    HISENSE_OTA_UPLOAD       = 302,
    HISENSE_DEVICES_BIND     = 401,
    HISENSE_DEVICES_UNBIND   = 402,
    HISENSE_BIND_LIST        = 403,
    HISENSE_CLOUD            = 602,
    HISENSE_SYS_HB           = 902,
    HISENSE_STATUS_UP        = 901,
    HISENSE_STEP_IDE         = 999,

};

int8_t hisense_recv_buff[HISENSE_RECV_BUFF_LEN] = {0};
int8_t hisense_deal_temp[HISENSE_RECV_BUFF_LEN] = {0};
int8_t hisense_send_buff[HISENSE_SEND_BUFF_LEN] = {0};

static int8_t g_hisense_key[16]    ={0};//aes key value
static int8_t g_hisense_token[40]  ={0};//new token

static uint32_t g_hisense_ticket = 0;//system ticket
static uint32_t g_hisense_ticket_hb = 0;//
static uint32_t g_hisense_ticket_rp = 0;//

static uint16_t g_heartbeat = 0;  // 心跳间隔时间，单位秒
static uint16_t g_report    = 0;  // 上报状态间隔时间，单位秒
static uint16_t g_reconnect = 0;  // 连接重试间隔时间，单位秒
// static uint32_t g_time_num  = 0;  // 当前的服务器时间，使用 unix 时间戳表示，即自 1970-1-1 之后的秒数，精度为秒，零时区时间


static uint16_t hisense_step = 0;

hisense_data_t hisense_send_queue_data;

cias_task_t ci_hisense_tcp_recv_handler; 

cias_task_t ci_hisense_network_test_handler; 

cias_queue_t ci_hisense_network_test_queue;

static TimerHandle_t hisense_xTimerUser; // 定义句柄


static void hisense_vTimerCallback( TimerHandle_t xTimer )
{
    g_hisense_ticket ++;
}

static void hisense_timer_start(void)
{
    // 申请定时器， 配置
    int hisense_time_id = 0;

    hisense_xTimerUser = xTimerCreate("hisense ticket",                   
                                pdMS_TO_TICKS(1000),   /*定时溢出周期， 单位是任务节拍数*/                   
                                pdTRUE, /*是否自动重载， 此处设置周期性执行*/                   
                                &hisense_time_id,/*记录定时器溢出次数， 初始化零, 用户自己设置*/                  
                                hisense_vTimerCallback); /*回调函数*/

    if( hisense_xTimerUser != NULL)// 启动定时器， 0 表示不阻塞
    {
        
        xTimerStart( hisense_xTimerUser, pdMS_TO_TICKS(1000));
    }     
}

static uint8_t get_index_from_char(char c)
{
    if ((c >= 'A') && (c <= 'Z'))           return (c - 'A');
    else if ((c >= 'a') && (c <= 'z'))      return (c - 'a' + 26);
    else if ((c >= '0') && (c <= '9'))      return (c - '0' + 52);
    else if (c == '+')                      return 62;
    else if (c == '/')                      return 63;
    else if (c == '=')                      return 64;
    else if ((c == '\r') || (c == '\n'))    return 254;
    else                                    return 255;
}
 
static char get_char_from_index(uint8_t i)
{
    if ((i >= 0) && (i <= 25))              return (i + 'A');
    else if ((i >= 26) && (i <= 51))        return (i - 26 + 'a');
    else if ((i >= 52) && (i <= 61))        return (i - 52 + '0');
    else if (i == 62)                       return '+';
    else if (i == 63)                       return '/';
    else                                    return '=';
}

static int other_base64_decode(const char *in, uint8_t *out, uint16_t *out_len)
{
    uint16_t i = 0, cnt = 0;
    uint8_t c, in_data_cnt;
    bool error_msg = false;
    uint32_t tmp = 0;
 
    if ((!in) || (!out) || (!out_len)) {
        //invalid parameter
        return -1;
    }
 
    in_data_cnt = 0;
    while (in[i] != '\0') {
        c = get_index_from_char(in[i++]);
        if (c == 255) {
            //MSG_PRINTF(LOG_ERR, "INVALID MESSAGE CODE\n");
            return -1;
        } else if (c == 254) {
            continue;           // Carriage return or newline feed, skip
        } else if (c == 64) {
            break;              // Meet '=', break
        }
 
        tmp = (tmp << 6) | c;
        if (++in_data_cnt == 4) {
            out[cnt++] = (uint8_t)((tmp >> 16) & 0xFF);
            out[cnt++] = (uint8_t)((tmp >> 8) & 0xFF);
            out[cnt++] = (uint8_t)(tmp & 0xFF);
            in_data_cnt = 0;
            tmp = 0;
        }
    }
 
    // Meet '=' or '\0'
    if (in_data_cnt == 3) {          // 3 chars before '=', encoded msg like xxx= OR
        tmp = (tmp << 6);           // 3 chars before '\0', encoded msg like xxx, considered '=' omitted
        out[cnt++] = (uint8_t)((tmp >> 16) & 0xFF);
        out[cnt++] = (uint8_t)((tmp >> 8) & 0xFF);
    } else if (in_data_cnt == 2) {   // 2 chars before '=', encoded msg like xx== OR
        tmp = (tmp << 6);           // 2 chars before '\0', encoded msg like xx, considered '=' omitted
        tmp = (tmp << 6);
        out[cnt++] = (uint8_t)((tmp >> 16) & 0xFF);
    } else if (in_data_cnt != 0) {
        error_msg = -1;           // Warn that the message format is wrong, but we tried our best to decode
    }
 
    *out_len = cnt;
 
    return 0;
}

static int other_base64_encode(const uint8_t *in, uint16_t in_len, char *out)
{
    int i;
    uint32_t tmp = 0;
    uint16_t out_len = 0;
    uint16_t left = in_len;
 
    if ((!in) || (!out)) {
        //invalid parameter
        return -1;
    }
 
    for (i = 0; i < in_len;) {
        if (left >= 3) {
            tmp = in[i];
            tmp = (tmp << 8) + in[i+1];
            tmp = (tmp << 8) + in[i+2];
            out[out_len++] = get_char_from_index((tmp & 0x00FC0000) >> 18);
            out[out_len++] = get_char_from_index((tmp & 0x0003F000) >> 12);
            out[out_len++] = get_char_from_index((tmp & 0x00000FC0) >> 6);
            out[out_len++] = get_char_from_index(tmp & 0x0000003F);
            left -= 3;
            i += 3;
        } else {
            break;
        }
    }
 
    if (left == 2) {
        tmp = in[i];
        tmp = (tmp << 8) + in[i+1];
        out[out_len++] = get_char_from_index((tmp & 0x0000FC00) >> 10);
        out[out_len++] = get_char_from_index((tmp & 0x000003F0) >> 4);
        out[out_len++] = get_char_from_index((tmp & 0x0000000F) << 2);
        out[out_len++] = get_char_from_index(64);
    } else if (left == 1) {
        tmp = in[i];
        out[out_len++] = get_char_from_index((tmp & 0x000000FC) >> 2);
        out[out_len++] = get_char_from_index((tmp & 0x00000003) << 4);
        out[out_len++] = get_char_from_index(64);
        out[out_len++] = get_char_from_index(64);
    }
 
    out[out_len] = '\0';
 
    return out_len;
}

void* hisense_aes_encrypt(int8_t* aes_src,int8_t* aes_key,int8_t* aes_dst)
{
    unsigned char pt_buf[512] = {0};
    char T_hainxin_dst[512] = {0};

    int16_t aes_src_len = strlen(aes_src);

    memcpy(pt_buf,aes_src,aes_src_len);

    int16_t pading = 16 - (aes_src_len % 16);

    for(int16_t i = 0; i < pading; i++)
    {
        pt_buf[aes_src_len + i] = pading;
    }

    int cnt = (aes_src_len + pading) / 16;

    mbedtls_aes_context aes_enc;

    mbedtls_aes_init(&aes_enc);

    mbedtls_aes_setkey_enc( &aes_enc, (const unsigned char*) aes_key, strlen(aes_key) * 8 );

    CIAS_PRINT_DEBUG("hisense_aes_src is:%s \r\n",pt_buf);//原始数据

    for (int i=0; i<cnt; i++)
    {
        mbedtls_aes_crypt_ecb(&aes_enc,MBEDTLS_AES_ENCRYPT,pt_buf+(i << 4), T_hainxin_dst+(i << 4));        
    }

    mbedtls_aes_free( &aes_enc );

    // CIAS_PRINT_INFO("AES recv beas64(len %d): ",176);
    // for(int16_t i = 0;i<(aes_src_len + pading);i++)
    // {
    //     if(i%16 == 0)CIAS_PRINT_INFO("\r\n");
    //     CIAS_PRINT_INFO("%02x ",T_hainxin_dst[i]);
    // }
    // CIAS_PRINT_INFO("\r\n\n");

    other_base64_encode(T_hainxin_dst,(aes_src_len + pading),aes_dst);
    // hisense_base64_encode(T_hainxin_dst,aes_dst,strlen(T_hainxin_dst));

    CIAS_PRINT_INFO("hisense_aes_dst is:%s \r\n",aes_dst);//输出数据

    return aes_dst;
}

void* hisense_aes_decrypt(uint8_t *hisense_src,uint8_t *hisense_key,uint8_t *hisense_dst)
{
    int8_t *T_hisense_key_string = HISENSE_MD5_KEY;
    char T_hisense_src2[512] = {0};
    char T_hainxin_dst[512] = {0};
    int base64_len = 0;
    mbedtls_aes_context aes;

    if(hisense_src != NULL)
    {
        // hisense_base64_decode(hisense_src,T_hisense_src2);
        other_base64_decode(hisense_src,T_hisense_src2,&base64_len);
        // CIAS_PRINT_INFO("AES recv src:%s\r\n",hisense_src);
    }
    else
    {
        CIAS_PRINT_INFO("hisense_src is NULL \r\n");
        return NULL;
    }
    
    // CIAS_PRINT_INFO("AES recv beas64(len %d): ",base64_len);
    // for(int16_t i = 0;i<base64_len;i++)
    // {
    //     if(i%16 == 0)CIAS_PRINT_INFO("\r\n");
    //     CIAS_PRINT_INFO("%02x ",T_hisense_src2[i]);
    // }
    // CIAS_PRINT_INFO("\r\n\n");
    
    if(strlen(hisense_key) != 0)  
    {
        T_hisense_key_string = hisense_key;
    }
    // CIAS_PRINT_INFO("AES used key:%s\r\n",T_hisense_key_string);
    

    int cnt = base64_len / 16;
  
    
    mbedtls_aes_init(&aes);

    mbedtls_aes_setkey_dec(&aes,T_hisense_key_string,strlen(T_hisense_key_string)*8);
    for (int i=0; i<cnt; i++)
    {
        mbedtls_aes_crypt_ecb(&aes,MBEDTLS_AES_DECRYPT,T_hisense_src2+(i << 4), T_hainxin_dst+(i << 4));    
    }

    

    if(strlen(hisense_key) != 0)
    {
        memcpy(hisense_dst,T_hainxin_dst,strlen(T_hainxin_dst));

        // CIAS_PRINT_INFO("AES DECRYPT data(%d):%s\r\n\n",base64_len,hisense_dst);
        return hisense_dst;
    }

    else
    {
        CIAS_PRINT_INFO("\r\nAES0 new key:%s ,old key:%s\r\n",T_hainxin_dst,T_hisense_key_string);
        
        memcpy(hisense_key,T_hainxin_dst,16);

        return hisense_key;
    }
    
    return NULL;
}


void hisense_md5_encrypt(unsigned char *h_encrypt,char* h_hisense_md5)
{
    mbedtls_md5_context hisense_md5_t;
    unsigned char decrypt[16] = {0};

    // CIAS_PRINT_INFO("加密前: %s \r\n",h_encrypt);

    mbedtls_md5_init(&hisense_md5_t);

    mbedtls_md5_starts(&hisense_md5_t);
    mbedtls_md5_update(&hisense_md5_t, h_encrypt, strlen((char *)h_encrypt));
    mbedtls_md5_finish(&hisense_md5_t, decrypt);

    for(int8_t i=4; i<12; i++) 
    {       
        sprintf(h_hisense_md5+((i-4)*2),"%02x",decrypt[i]);                    
    }
    
    // CIAS_PRINT_INFO("加密后: %s \r\n",h_hisense_md5);

    mbedtls_md5_free(&hisense_md5_t);
}

int hisense_control_step(uint16_t arg)
{
    int32_t ret = 0;

    hisense_send_queue_data.num = arg;
    ret = cias_queue_send(&ci_hisense_network_test_queue, &hisense_send_queue_data, sizeof(cias_raw_speech_t));
    if (ret != CIAS_OK)
    {
        CIAS_LOG_HL("send_net_info fail\n");
        return -1;
    }
    return 0;
}

static int  hisense_cloud_json_parse(char *str)
{
    cJSON *json  = NULL;  
    cJSON *json_value  = NULL;  
    int string_len = 0, json_ret = -1;
    

    json = cJSON_Parse(str);

    json_value = cJSON_GetObjectItem(json,"id");
    if(json_value)
    {
        
        if(json_value->valueint == HISENSE_CONTROL_CMD ||json_value->valueint == HISENSE_CHECK_STATUS)
        {
            hisense_control_step(json_value->valueint);
            CIAS_LOG_HL("tcp recv id:%d\r\n",json_value->valueint);
        }
    }
 
    json_value = cJSON_GetObjectItem(json,"token");    
    if(json_value)
    {   
        string_len = strlen(json_value->valuestring);
        memcpy(g_hisense_token,json_value->valuestring,string_len);
        CIAS_LOG_HL("tcp recv token:%d,\"%s\" \r\n",string_len,g_hisense_token);

        hisense_control_step(HISENSE_KEY_EXCHANGE);
    }

    json_value = cJSON_GetObjectItem(json,"key");
    if(json_value)
    {    
        string_len = strlen(json_value->valuestring);    
        CIAS_LOG_HL("tcp recv key:%d,\"%s\"\r\n",string_len,json_value->valuestring); 
        hisense_aes_decrypt(json_value->valuestring,g_hisense_key,NULL);
        hisense_control_step(HISENSE_SYS_PARAM);
    }

    json_value = cJSON_GetObjectItem(json,"args");    
    if(json_value)
    {        
        string_len = strlen(json_value->valuestring);
        CIAS_LOG_HL("tcp recv args:%d,\"%s\"\r\n",string_len,json_value->string);         
        json = json_value->child;
        json_value = cJSON_GetObjectItem(json,"hb");
        if(json_value->valueint)
        {
            CIAS_LOG_HL("tcp real hb:\"%d\" \r\n",json_value->valueint);
            g_heartbeat = json_value->valueint;
        }

        json_value = cJSON_GetObjectItem(json,"rp");
        if(json_value->valueint)
        {
            CIAS_LOG_HL("tcp real rp:\"%d\" \r\n",json_value->valueint);
            g_report = json_value->valueint;
        }

        json_value = cJSON_GetObjectItem(json,"retry");
        if(json_value->valueint)
        {
            CIAS_LOG_HL("tcp real retry:\"%d\" \r\n",json_value->valueint);
            g_reconnect = json_value->valueint;
        }

        json_value = cJSON_GetObjectItem(json,"time");
        if(json_value->valueint)
        {      
            g_hisense_ticket = json_value->valueint;
            g_hisense_ticket_hb = json_value->valueint;
            g_hisense_ticket_rp = json_value->valueint;
            hisense_timer_start();
            CIAS_LOG_HL("tcp real time:\"%d\" \r\n",json_value->valueint);
        }

        
        json_value = cJSON_GetObjectItem(json,"timezone");
        if(json_value->valueint)
        {
            CIAS_LOG_HL("tcp real timezone:\"%d\" \r\n",json_value->valueint);
        }

        hisense_control_step(HISENSE_SYS_HB);
        hisense_control_step(HISENSE_CLOUD);
        hisense_control_step(HISENSE_STATUS_UP);
        
    }
    
    cJSON_Delete(json);

    return json_ret;
}

static void hisense_cloud_recv(int32_t hisense_cloud_fd)
{
    int32_t hisense_cloud_mret = 0;   
    while(1)
    {        
        cias_task_delay_ms(1000);

        hisense_cloud_mret = recv(hisense_cloud_fd, hisense_recv_buff, HISENSE_RECV_BUFF_LEN,0);
        CIAS_PRINT_DEBUG("recv(%d):%s \r",hisense_cloud_mret,hisense_recv_buff);
       

        if(hisense_cloud_mret > 0)
        {           
            hisense_cloud_json_parse(hisense_recv_buff+2);
        }

        bzero(hisense_recv_buff, HISENSE_RECV_BUFF_LEN);
    }

    return NULL;
}

static void ci_hisense_TCP_recv_handler(int32_t hisense_cloud_fd)
{

    cias_task_create(&ci_hisense_tcp_recv_handler
                    , "hisense_cloud_recv"
                    ,hisense_cloud_recv
                    ,hisense_cloud_fd
                    ,CIAS_APP_PRIORITY+1
                    ,(unsigned short)1024*12);

}
static void ci_hisense_softap_start(void)
{
    extern wifi_softap_cfg_t ap_cfg;

    uint8_t hisense_ssid_str[16];
    uint8_t hisense_bssid_hex[BSSID_LEN];

    sysparam_sta_mac_get(hisense_bssid_hex);

    CIAS_PRINT_INFO("hisense MAC:\"%02X:%02X:%02X:%02X:%02X:%02X\" \r\n",hisense_bssid_hex[0]
    , hisense_bssid_hex[1], hisense_bssid_hex[2], hisense_bssid_hex[3], hisense_bssid_hex[4], hisense_bssid_hex[5]);

    sprintf(hisense_ssid_str,HISENSE_SSID_STR,hisense_bssid_hex[4], hisense_bssid_hex[5]);

    ap_cfg.ssid = hisense_ssid_str;
    ap_cfg.bssid = hisense_bssid_hex;

    CIAS_PRINT_DEBUG("hisense ap start ssid:%s, pwd:%s, bssid:%02X:%02X:%02X:%02X:%02X:%02X\n",ap_cfg.ssid,ap_cfg.pwd,
    ap_cfg.bssid[0],ap_cfg.bssid[1],ap_cfg.bssid[2],ap_cfg.bssid[3],ap_cfg.bssid[4],ap_cfg.bssid[5]);
    
    cias_start_ap_net();

}





extern int ip_addr_get_status;//add by hw(used to ap net)

void ci_hisense_main_task(void)
{
    err_t err;
    int32_t mret = 0;
    int32_t queue_mret = 0;
    int16_t net_len = 0;
    int32_t hisense_fd = 0;
    
    ip_addr_t *name_addr = NULL;
    char     mdst_addr_s[16] = {0};
    uint8_t  mdst_addr_i[4]    = {0};
    
    char* hisense_head = HISENSE_HEAD;
    char* hisense_data = DEVICES_AUTH;
    char* hisense_tail = HISENSE_TAIL;

    struct sockaddr_in hisense_addr;
	socklen_t len = sizeof(hisense_addr);
    
    
    unsigned char encrypt[HISENSE_SEND_BUFF_LEN] = {0};    
    unsigned char hisense_md5[16] = {0};

    hisense_data_t hisense_deal = {0};

CONNECT_lable:
	bzero(&hisense_addr, len);
    if(name_addr == NULL)
    {
        name_addr = (ip_addr_t *)pvPortMalloc(sizeof(ip_addr_t));
    }
    
    
    //等待网络连接
    while(!ip_addr_get_status)
    {
        cias_task_delay_ms(1000);
    }
    cias_task_delay_ms(500);
    CIAS_PRINT_INFO("hisense sta connected \r\n");

 
    

    // extern int8_t g_HISENSE_BLE_CONFIG_EXIT;//BLE 配网标志（后期通过命令词或flash取代）
    // while (!g_HISENSE_BLE_CONFIG_EXIT)
    // {
    //     cias_task_delay_ms(2000);
    //     //关闭蓝牙配网任务
    // }
        


    // soft ap
    // extern int8_t g_HISENSE_TCP_SERVER_EXIT; //ap 配网标志（后期通过命令词或flash取代）
    // if(!g_HISENSE_TCP_SERVER_EXIT)
    // {
    //     ci_hisense_softap_start();
    //     goto  CONNECT_lable;
    // }


    err = netconn_gethostbyname(DEST_HOST,name_addr);

    if(err == ERR_OK)
    {
        mdst_addr_i[0] = name_addr->addr;
        mdst_addr_i[1] = name_addr->addr>>8; 
        mdst_addr_i[2] = name_addr->addr>>16;
        mdst_addr_i[3] = name_addr->addr>>24; 
        sprintf(mdst_addr_s,"%d.%d.%d.%d",mdst_addr_i[0],mdst_addr_i[1],mdst_addr_i[2],mdst_addr_i[3]);
        CIAS_PRINT_DEBUG("netconn_gethostbyname success\n");
        vPortFree(name_addr);
        name_addr = NULL;
    }
    else
    {
        CIAS_PRINT_DEBUG("netconn_gethostbyname failed(%d)\n",err);
        goto  CONNECT_lable;
    }

    CIAS_PRINT_DEBUG("server ip:%s\r\n",mdst_addr_s);

    
	hisense_addr.sin_family = AF_INET;
	hisense_addr.sin_addr.s_addr = inet_addr(mdst_addr_s);
    hisense_addr.sin_port   = htons(atoi(DEST_PORT));
    // hisense_addr.sin_addr.s_addr = inet_addr("192.168.3.145");    
	// hisense_addr.sin_port   = htons(atoi("777"));

    hisense_fd = socket(AF_INET, SOCK_STREAM, 0);

    //发起tcp连接请求... ...
    while(connect(hisense_fd, (struct sockaddr *)&hisense_addr, len))
    {   
        close(hisense_fd);        
        CIAS_PRINT_DEBUG("hisense connect failed\n");
        cias_task_delay_ms(1000);
        goto  CONNECT_lable;
    }
    CIAS_PRINT_DEBUG("hisense connected:\n");

    ci_hisense_TCP_recv_handler(hisense_fd);


    hisense_control_step(HISENSE_AUTH);
    
	while(1)
	{
        bzero(hisense_send_buff, HISENSE_SEND_BUFF_LEN);
        bzero(hisense_deal_temp, HISENSE_RECV_BUFF_LEN);
        // CIAS_PRINT_DEBUG("Heap left: %d; min:%d; Task High Water:%d.\r\n", 
        // xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize(),uxTaskGetStackHighWaterMark(NULL));
        queue_mret = cias_queue_receive(&ci_hisense_network_test_queue, &hisense_deal, 1000);        
        if(queue_mret == CIAS_OK)
        {    
            if(hisense_deal.num > 0)
            {
                hisense_step = hisense_deal.num;
            }
        }

        switch(hisense_step)
        {
            //设备认证 101
            case HISENSE_AUTH:
            {
                //字符串打包               
                hisense_data = hisense_deal_temp;

                sprintf(hisense_data,DEVICES_AUTH,WIFIID);
                sprintf(encrypt,"%s%s",HISENSE_MD5_KEY,hisense_data);

                hisense_md5_encrypt(encrypt,hisense_md5);//字符串加密
                cias_task_delay_ms(100);

                sprintf(hisense_send_buff,"%s%s%s%s",hisense_head,hisense_data,hisense_md5,hisense_tail);
                
                mret = send(hisense_fd,hisense_send_buff,strlen(hisense_send_buff),0);

                CIAS_PRINT_DEBUG("send(%d,%d):%s\r\n",mret,strlen(hisense_send_buff),hisense_send_buff);

                break;

            }
            // 密钥的交换 102
            case HISENSE_KEY_EXCHANGE:
            {                
                //字符串打包 
                sprintf(hisense_deal_temp,DEVICES_KEY,g_hisense_token);
                hisense_data = hisense_deal_temp;

                sprintf(encrypt,"%s%s",HISENSE_MD5_KEY,hisense_data);
                hisense_md5_encrypt(encrypt,hisense_md5);
                cias_task_delay_ms(100);

                sprintf(hisense_send_buff,"%s%s%s%s",hisense_head,hisense_data,hisense_md5,hisense_tail);

                mret = send(hisense_fd,hisense_send_buff,strlen(hisense_send_buff),0);

                CIAS_PRINT_DEBUG("send(%d,%d,%d):%s\r\n",mret,strlen(hisense_data),strlen(hisense_send_buff),hisense_send_buff);

                break;
            }
            // 系统参数的获取 103
            case HISENSE_SYS_PARAM:
            {
                sprintf(hisense_deal_temp,DEVICES_SYS,g_hisense_token);
                hisense_data = hisense_deal_temp;  

                sprintf(encrypt,"%s%s",HISENSE_MD5_KEY,hisense_data);                
                hisense_md5_encrypt(encrypt,hisense_md5);
                cias_task_delay_ms(100);

                sprintf(hisense_send_buff,"%s%s%s%s",hisense_head,hisense_data,hisense_md5,hisense_tail);
                mret = send(hisense_fd,hisense_send_buff,strlen(hisense_send_buff),0);

                CIAS_PRINT_DEBUG("send(%d,%d,%d):%s\r\n",mret,strlen(hisense_data),strlen(hisense_send_buff),hisense_send_buff);
                break;
            }
            // 602
            case HISENSE_CLOUD:
            {             
                extern uint32_t g_hisense_ap_ticket;
                extern wifi_sta_connect_t hisense_ap_connect;

                if(g_hisense_ap_ticket == 0||hisense_ap_connect.bssid == NULL)
                {
                    break;
                }

                bzero(hisense_send_buff, HISENSE_SEND_BUFF_LEN);
                bzero(hisense_deal_temp, HISENSE_RECV_BUFF_LEN);

                sprintf(hisense_deal_temp,DEVICES_DATA,hisense_ap_connect.bssid,WIFIID,g_hisense_ap_ticket);
                CIAS_PRINT_DEBUG("DEVICES_DATA_src %0.3d:%s\r\n",strlen(hisense_deal_temp),hisense_deal_temp);

                other_base64_encode(hisense_deal_temp,strlen(hisense_deal_temp),hisense_send_buff);
                // hisense_base64_encode(T_hainxin_dst,aes_dst,strlen(T_hainxin_dst));

                CIAS_PRINT_DEBUG("DEVICES_DATA_enc %0.3d:%s\r\n",strlen(hisense_send_buff),hisense_send_buff);

                bzero(hisense_deal_temp, HISENSE_RECV_BUFF_LEN);
                sprintf(hisense_deal_temp,DEVICES_CLOUD,g_hisense_token,hisense_send_buff);

                hisense_data = hisense_deal_temp;
                sprintf(encrypt,"%s%s",HISENSE_MD5_KEY,hisense_data);
                hisense_md5_encrypt(encrypt,hisense_md5);
                cias_task_delay_ms(100);

                bzero(hisense_send_buff, HISENSE_RECV_BUFF_LEN);
                sprintf(hisense_send_buff,"%s%s%s%s",hisense_head,hisense_data,hisense_md5,hisense_tail);

                mret = send(hisense_fd,hisense_send_buff,strlen(hisense_send_buff),0);
                CIAS_PRINT_DEBUG("send(%d,%d,%d):%s\r",mret,strlen(hisense_data),strlen(hisense_send_buff),hisense_send_buff);

                break;
            }
            // 902
            case HISENSE_SYS_HB://心跳
            {       
                bzero(hisense_send_buff, HISENSE_RECV_BUFF_LEN);
                sprintf(hisense_send_buff,"%s%s%s",hisense_head,DEVICES_HB,hisense_tail);
                cias_task_delay_ms(100);

                mret = send(hisense_fd,hisense_send_buff,strlen(hisense_send_buff),0);

                CIAS_PRINT_DEBUG("send(%d,%d):%s\r",mret,strlen(hisense_send_buff),hisense_send_buff);

                break;
            }
            // 901
            case HISENSE_STATUS_UP: //状态上报
            {                
                sprintf(hisense_deal_temp,DEVICES_STATUS_UP,g_hisense_token,DEVICES_STATUS);
                hisense_data = hisense_deal_temp;  

                sprintf(encrypt,"%s%s",HISENSE_MD5_KEY,hisense_data);                
                hisense_md5_encrypt(encrypt,hisense_md5);
                cias_task_delay_ms(100);

                sprintf(hisense_send_buff,"%s%s%s%s",hisense_head,hisense_data,hisense_md5,hisense_tail);
                mret = send(hisense_fd,hisense_send_buff,strlen(hisense_send_buff),0);

            
                CIAS_PRINT_DEBUG("send(%d,%d,%d):%s\r\n",mret,strlen(hisense_data),strlen(hisense_send_buff),hisense_send_buff);

                break;
            }
            default:
            {
                // CIAS_PRINT_DEBUG("hisense_step:%d \r\n",hisense_step);
                break;
            }
        }
        
        //服务器关闭
        if(mret<= 0 && (mret != EINTR || mret != EWOULDBLOCK || mret != EAGAIN))
        {   
            close(hisense_fd);
            CIAS_PRINT_DEBUG("hisense server disconnect %d...\r",mret);
            goto  CONNECT_lable;
        }
        

        if((g_heartbeat > 0)&&(g_hisense_ticket - g_hisense_ticket_hb) >= g_heartbeat/3)//心跳
        {
            g_hisense_ticket_hb = g_hisense_ticket;

            hisense_control_step(HISENSE_SYS_HB);

            CIAS_PRINT_DEBUG("net real time:%d \r",g_hisense_ticket);
        }

        if((g_report > 0)&&(g_hisense_ticket - g_hisense_ticket_rp) >= g_report/2)//状态上报
        {
            g_hisense_ticket_rp = g_hisense_ticket; 

            hisense_control_step(HISENSE_STATUS_UP);
        }


        hisense_step = HISENSE_STEP_IDE;
        cias_task_delay_ms(100);        
	}
	close(hisense_fd);	
}


/**
 * @brief 用于内部调试
 * name:int ci_hisense_main(void) 
 * 返回值 ：无
 * 入口参数： 0
 */
int ci_hisense_main(void)
{
    
    cias_queue_create(&ci_hisense_network_test_queue, sizeof(hisense_data_t), 15);

    cias_task_create(&ci_hisense_network_test_handler
                    , "ci_hisense_main"
                    ,ci_hisense_main_task
                    ,0
                    ,CIAS_APP_PRIORITY+1
                    ,(unsigned short)1024*20);
    return 0;

}
#endif  // CIAS_IOT_CLOUD_HISENSE_ENABLE