/*
 * @Description: 
 * @Author: hongchuan.wu
 * @Date: 2021-12-09 16:36:48
 * @LastEditTime: 2021-12-10 19:08:37
 * @LastEditors: hongchuan.wu
 * @Reference: 
 */


#ifndef __CIAS_LAN_NETWORK_H__
#define __CIAS_LAN_NETWORK_H__

#include "cias_freertos_queue.h"


#define   DEST_HOST                 "ns.wg.hismarttv.com"
#define   DEST_PORT                 "8088"
#define   HISENSE_MD5_LEN            16
#define   HISENSE_RECV_BUFF_LEN      (1501)
#define   HISENSE_SEND_BUFF_LEN      (1501)
#define   WIFIID                    "86100c00900500100000004080cbbc6eb568"
#define   HISENSE_MD5_KEY            "6ecefdcd7aac8d64"
#define   HISENSE_AP_KEY             "HisenseOverSeasIOTSetUpProtocal;"


// 1kk0350sun0000128149137
// 空调featurecode :86100c009005001000000040
// mac:80CBBC6EB568
// key：6ecefdcd7aac8d64  //6ECEFDCD7AAC8D64
// wifidd:86100c00900500100000004080cbbc6eb568 //86100c00900500100000004080CBBC6EB568


// key：f0f63c43c48e206c 测试环境密钥 
// 18822141886  123456 app提供一个测试环境的账号
// ping 一下这俩域名的地址 把解析出的地址发一下 api-wg.hismarttv.com    ns.wg.hismarttv.com
// DNS是47.104.211.24

#define HISENSE_HEAD "HS"
#define HISENSE_TAIL "\r\n"


#define HISENSE_AP_LIST_COUNT_MAX        10

typedef struct 
{
    int8_t *data;
    uint16_t num;

}hisense_data_t;

extern cias_queue_t ci_hisense_network_test_queue;

extern int8_t hisense_recv_buff[HISENSE_RECV_BUFF_LEN];
extern int8_t hisense_deal_temp[HISENSE_RECV_BUFF_LEN];
extern int8_t hisense_send_buff[HISENSE_SEND_BUFF_LEN];


int ci_hisense_main(void);

void  hisense_md5_encrypt(unsigned char *h_encrypt,char* h_hisense_md5);
void* hisense_aes_decrypt(uint8_t *hisense_src,uint8_t *hisense_key,uint8_t *hisense_dst);
// void* hisense_aes_encrypt(void);
void* hisense_aes_encrypt(int8_t* aes_src,int8_t* aes_key,int8_t* aes_dst);



#endif //__CIAS_LAN_NETWORK_H__