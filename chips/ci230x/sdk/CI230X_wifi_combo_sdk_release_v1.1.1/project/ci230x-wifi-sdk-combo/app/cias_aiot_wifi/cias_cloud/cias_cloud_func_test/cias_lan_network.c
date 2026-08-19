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
#include "lwip/sockets.h"
#include "cias_ap_net.h"
#include "cias_user_config.h"
#include "cias_ap_net.h"
#include "cias_common.h"
#include "cias_tvs_upload.h"
#include "utils/system_parameter.h"
#include "cias_media.h"
#if CIAS_LAN_NETWORK_ENABLE
#include "cias_log.h"
#include "cias_lan_network.h"
cias_task_t ci_lan_network_test_handler; 
cias_queue_t ci_lan_network_test_queue;


int8_t cias_lan_ip[16] = {0}; //获取本机ip 地址
/**
 * @brief 用于内部调试（数据是否丢失）
 * name:void ci_lan_network_test_fun(void) 
 * auther:wuhongchuan 2021，11，15
 * 返回值 ：无
 * 入口参数： 无
 */

uint8_t deal_cias_lan_ip(int8_t src[])
{
    uint8_t ret = 0;

    for(uint8_t i = 0;i<16;i++)
    {
        if(src[i] == '.')
        {
            ret = i;
        }
    }


    return ret+1;
}

#define TCP_CLIENT 1
#define TCP_SERVER 0


// void test_ip(void)
// {
//     tcpip_ip_info_t ip_info;
//     wifi_sta_status_t sta_status = WIFI_STA_STATUS_STARTUP;
//     char* name = "lan";
//     char ip[16] = {0};
//     char netmask[16] = {0};
//     char gw[16] = {0};

//     wifi_get_sta_status(&sta_status);
//     if(sta_status != WIFI_STA_STATUS_CONNECTED)
//     {
//         goto __exit;
//     }


 
//     if (0 != sysparam_sta_ip_info_get(&ip_info))
//     {
//         goto __exit;
//     }



//     if (inet_ntop(AF_INET, &ip_info.ip, ip, sizeof(ip)) == NULL)
//     {
//         goto __exit;
//     }

//     if (inet_ntop(AF_INET, &ip_info.netmask, netmask, sizeof(netmask)) == NULL)
//     {
//         goto __exit;
//     }

//     if (inet_ntop(AF_INET, &ip_info.gw, gw, sizeof(gw)) == NULL)
//     {
//         goto __exit;
//     }

//     CIAS_PRINT_DEBUG("%s:\"%s\"\r\n", name, ip);
//     CIAS_PRINT_DEBUG("%s:\"%s\"\r\n", name, gw);
//     CIAS_PRINT_DEBUG("%s:\"%s\"\r\n", name, netmask);

//     __exit:
    
//     return 0;

// }


// int8_t recv_buff[1024] = {0};
void ci_lan_network_test_fun(void)
{
    int mret = 0;
    cias_status ret;
    lan_data_t recv_msg;

    struct  sockaddr_in serveraddr;
    struct  sockaddr_in clientaddr;
	socklen_t len = sizeof(serveraddr);

    char mdst_addr[16] = {0};
    char* mdst_addr_2 = "80";
    char* mdst_htons = "8888";
    
    uint32_t toal_lan_size = 0;    
    static int listenfd,connfd; 


CONNECT_lable:

	bzero(&serveraddr, len);
    // bzero(&recv_buff, 1024);

    //等待网络连接
    // while(!cias_get_wifi_connect_state())
    while(cias_get_wifi_sta_connect_state() != NETDEV_LINK_UP)
    // while(!get_tvs_network_state())
    {
        cias_task_delay_ms(1000);
    }
    cias_task_delay_ms(3000);

    

    #if TCP_SERVER
    
    uint8_t dst_len =  deal_cias_lan_ip(cias_lan_ip);

    if(dst_len > 1)
    {
        memcpy(mdst_addr,cias_lan_ip,dst_len);
        memcpy(mdst_addr+dst_len,mdst_addr_2,3);
    }

    CIAS_PRINT_DEBUG("lan dst ip:%s post:%s\n",mdst_addr,mdst_htons);

    serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(mdst_addr);
	serveraddr.sin_port   = htons(atoi(mdst_htons));

	
    connfd = socket(AF_INET, SOCK_STREAM, 0);
  
    //发起tcp连接请求... ...
    while(connect(connfd, (struct sockaddr *)&serveraddr, len))
    {   
        close(connfd);        
        CIAS_PRINT_DEBUG("lan connect failed\n");
        LOG(LOG_LVL_DEBUG,"********>>>>>Heap left: %d <1> min:%d<<<<<********\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
        cias_task_delay_ms(500);
        goto  CONNECT_lable;
    }
    CIAS_PRINT_DEBUG("lan connected:\n");
    #endif

    #if TCP_CLIENT
    int16_t tcp_tmp,tcp_ret = 0;
    
    
    
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serveraddr,0,sizeof(serveraddr));
    memset(&clientaddr,0,sizeof(clientaddr));
    
    // bzero(recv_buff, 1024);    
    

    serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port   = htons(atoi(mdst_htons));


    tcp_ret = bind(listenfd,&serveraddr,sizeof(serveraddr));
    if(tcp_ret == -1)
    {
        CIAS_PRINT_DEBUG("lan TCP bind failed \r\n");
        goto  CONNECT_lable;  
    }

    tcp_ret = listen(listenfd,10);
    if(tcp_ret == -1)
    {
        CIAS_PRINT_DEBUG("lan TCP listen failed \r\n");
        goto  CONNECT_lable;  
    }
    CIAS_PRINT_DEBUG("lan TCP listen ip:%s post:%s\n",cias_lan_ip,mdst_htons);
    #endif

    while(1)
	{
        connfd = accept(listenfd,&clientaddr,tcp_tmp);
        if(connfd == -1)
        {
            CIAS_PRINT_DEBUG("TCP accept failed \r\n");
            goto  CONNECT_lable; 
        }
        else
        {
            CIAS_PRINT_DEBUG("TCP accept ok \r\n");
            // test_ip();
            break;
        }
    } 

	while(1)
	{
        // mret = read(connfd, recv_buff, 1024);
        // if(mret )
        // {
        //     CIAS_PRINT_DEBUG("recv:%s \r\n",recv_buff);
        //     recv_msg.data = (char *)recv_buff;
        //     recv_msg.len = mret;

        //     // CIAS_LOG_HL("recv iot url:%s(%d)\r\n",recv_msg.data,recv_msg.len);
        //     // extern void wakeup_deal(void);
        //     // cias_finish_play_music();
        //     // wakeup_deal();
        //     // cias_media_set_skip_speak(false);
        //     // cias_media_clear_suspension();
        //     // cias_media_set_block_size(0);
        //     // cias_media_quit_connect();
        //     // cias_media_recv_cmd(CIAS_RECV_PLAY_END_CMD);

        //     // cias_media_add_audio_url(recv_msg.data);
        //     // cias_media_set_url_type(ONEMUSIC);            
        //     // cias_media_set_audio_state(MEDIA_AUDIO_PLAY);
        //     // cias_media_set_speak_state(MEDIA_SPEAK_STOP);
        //     // cias_media_recv_cmd(CIAS_CONNECT_URL_CMD);

        //     //将原始数据传输到服务器
        //     mret = write(connfd, recv_msg.data, recv_msg.len);

        //     bzero(&recv_buff, 1024);
        // }
        // cias_task_delay_ms(500);

        ret = cias_queue_receive(&ci_lan_network_test_queue, &recv_msg, CIAS_WAIT_FOREVER);
        if(ret == CIAS_OK)
        {            
            //将原始数据传输到服务器
            // mret = write(connfd, recv_msg.data, recv_msg.len);
            mret = write(connfd, recv_msg.data, recv_msg.len);
            toal_lan_size+=recv_msg.len;
        }
        
            
        //服务器关闭
        if(mret<= 0 && mret != EINTR)
        {   
            close(connfd);
            goto  CONNECT_lable;
                        
        }
       
	}

	close(connfd);
	close(listenfd);
}




/**
 * @brief 用于内部调试（数据是否丢失）
 * name:int ci_lan_network_test_task(void) 
 * auther:wuhongchuan 2021，11，15
 * 返回值 ：无
 * 入口参数： 0
 */
int ci_lan_network_test_task(void)
{
    
    cias_queue_create(&ci_lan_network_test_queue,5,sizeof(lan_data_t));

    #if 0
        //add by whc 调试，在此处加入局域网通讯
        ret = cias_queue_send(&ci_lan_network_test_queue, &recv_msg, pdMS_TO_TICKS(10));
        if (ret != CIAS_OK)
        {
            CIAS_PRINT_WARN("SEND_PCM_info fail\n");
        }
    #endif

    cias_task_create(&ci_lan_network_test_handler
                    , "ci_lan_net_test"
                    ,ci_lan_network_test_fun
                    ,0
                    ,CIAS_APP_PRIORITY
                    ,(unsigned short)1024*2);
    return 0;

}
#endif  // CIAS_LAN_NETWORK_ENABLE