#include "cias_user_config.h"
#include "cias_common.h"
#if 1

#include "utils/debug/log.h"
#include "utils/debug/ln_assert.h"
#include "utils/system_parameter.h"
#include "wifi.h"
#include "ln_wifi_err.h"
#include "netif/ethernetif.h"
#include "dhcpd_api.h"
#include "wifi_manager.h"

#include "ln_app_gap.h"
#include "gapm_task.h"
#include "ln_app_gatt.h"
#include "ln_app_callback.h"
#include "ln_gatt_callback.h"
#include "usr_ble_app.h"
#include "usr_send_data.h"
#include "usr_app.h"
#include "cias_ble_config.h"
#include "hisense_ble_message_deal.h"


extern uint8_t ln_conidx;
extern uint8_t adv_actv_idx;
extern uint8_t init_actv_idx;
extern wifi_sta_connect_t ble_connect;

typedef struct{
    uint8_t bssid_octer[6];
    uint8_t rssi;
    uint8_t secuity;
    uint8_t ssid_len;
    uint8_t ssid[33];
} ble_scan_ap_info_node;

ble_scan_ap_info_node ble_scan_ap_list[HISENSE_AP_LIST_COUNT_MAX] = {0};
static int8_t ble_scan_ap_list_count = 0;
int8_t g_HISENSE_BLE_CONFIG_EXIT = 0;

int hisense_ble_scan_ap_list_write(ap_info_t *ap_info)
{

    if(ble_scan_ap_list_count == 0)
    {
        bzero(ble_scan_ap_list, sizeof(ble_scan_ap_list));
    }

    if( ble_scan_ap_list_count < HISENSE_AP_LIST_COUNT_MAX )
    {
        if(ble_scan_ap_list_count>1)
        {   int i = ble_scan_ap_list_count;
            while(i--)
            {
                if(strstr(ble_scan_ap_list[i].ssid,ap_info->ssid))
                    return 0;
            }
        }
        memcpy(ble_scan_ap_list[ble_scan_ap_list_count].bssid_octer,ap_info->bssid,sizeof(ap_info->bssid));
        memcpy(ble_scan_ap_list[ble_scan_ap_list_count].ssid,ap_info->ssid,sizeof(ap_info->ssid));

        ble_scan_ap_list[ble_scan_ap_list_count].rssi = ap_info->rssi * -1;
        ble_scan_ap_list[ble_scan_ap_list_count].secuity = ap_info->authmode;
        ble_scan_ap_list[ble_scan_ap_list_count].ssid_len = strlen(ap_info->ssid);

        uint8_t *mac = ble_scan_ap_list[ble_scan_ap_list_count].bssid_octer;
        LOG(LOG_LVL_INFO,"AP2 count     : %d\r\n",ble_scan_ap_list_count);
        LOG(LOG_LVL_INFO,"AP2 rssi      : %d\r\n",ble_scan_ap_list[ble_scan_ap_list_count].rssi);
        LOG(LOG_LVL_INFO,"AP2 secuity   : %d\r\n",ble_scan_ap_list[ble_scan_ap_list_count].secuity);
        LOG(LOG_LVL_INFO,"AP2 ssid_len  : %d\r\n",ble_scan_ap_list[ble_scan_ap_list_count].ssid_len);
        LOG(LOG_LVL_INFO,"AP2 ssid      : %s\r\n",ble_scan_ap_list[ble_scan_ap_list_count].ssid);
        LOG(LOG_LVL_INFO,"AP2 bssid     : %02X:%02X:%02X:%02X:%02X:%02X\r\n\n",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        

        ble_scan_ap_list_count++;
        return ble_scan_ap_list_count;
    }

    return -1;     
}

void hisense_ble_set_adv_data(void)
{   
    uint8_t adv_data[ADV_DATA_MAX_LENGTH] = {0};
    
    uint8_t name_data_hisense[10]    ={0};    
    uint8_t server_data_hisense[11]  ={0};

    // 蓝牙设备    
    server_data_hisense[0] = 0x0a;    
    server_data_hisense[1] = GAP_AD_TYPE_SERVICE_16_BIT_DATA;
    server_data_hisense[2] = 0xff;
    server_data_hisense[3] = 0xff;
    server_data_hisense[4] = 0x02;
    server_data_hisense[5] = 0x01;
    server_data_hisense[6] = 0x01;
    server_data_hisense[7] = 0x0f;
    server_data_hisense[8] = 0x00;
    server_data_hisense[9] = 0x00;
    server_data_hisense[10] = 0x00;



    //蓝牙设备
    name_data_hisense[0] = 0x09;
    name_data_hisense[1] = GAP_AD_TYPE_COMPLETE_NAME;  //adv type :local name
    memcpy(&name_data_hisense[2],DEVICE_NAME_HISENSE,4);
    // name_data_hisense[6] = 0x37;
    // name_data_hisense[7] = 0x45;
    // name_data_hisense[8] = 0x31;
    // name_data_hisense[9] = 0x36;

    name_data_hisense[6] = 0xBC;
    name_data_hisense[7] = 0x6E;
    name_data_hisense[8] = 0xB5;
    name_data_hisense[9] = 0x68;

    

    if(sizeof(server_data_hisense)+sizeof(name_data_hisense)>=ADV_DATA_MAX_LENGTH)
    {
        while (1)
        {
            LOG(LOG_LVL_ERROR, "[%s] %d adv lenth err!!!\r\n", __func__, __LINE__);
        }
    }
    memcpy(&adv_data[0],&server_data_hisense[0],sizeof(server_data_hisense));
    memcpy(&adv_data[sizeof(server_data_hisense)],&name_data_hisense[0],sizeof(name_data_hisense));
    

    struct ln_gapm_set_adv_data_cmd adv_data_param;
    adv_data_param.actv_idx = adv_actv_idx;
    adv_data_param.length = sizeof(adv_data);
    adv_data_param.data = adv_data;
    ln_app_set_adv_data(&adv_data_param);
    
    
    LOG(LOG_LVL_DEBUG, "[%s] %d\r\n", __func__, __LINE__);
}




int16_t hisense_test_data_data_len = 0;
uint8_t hisense_test_data_data_src[HISENSE_SEND_BUFF_LEN] = {0};

uint16_t hisense_ble_write_data(int8_t*src,uint16_t len)
{
    
    hisense_test_data_data_len = len;
    bzero(hisense_test_data_data_src,HISENSE_SEND_BUFF_LEN);
    memcpy(hisense_test_data_data_src,src,len);

    LOG(LOG_LVL_CTRL, "\r\n[HIS write to app]:");
    for(int j=0;j<hisense_test_data_data_len;j++)
    {
        if(j%16 == 0)LOG(LOG_LVL_CTRL, "\r\n");
        LOG(LOG_LVL_CTRL, "%.2x ",hisense_test_data_data_src[j]);
    }
    LOG(LOG_LVL_CTRL, "\r\n\n");
    OS_MsDelay(100);
        
    return 0;
}


uint8_t hisense_bssid_len = 0;
uint8_t hisense_bssid_src[33] ={0};
uint8_t hisense_pwd_len = 0;
uint8_t hisense_pwd_src[64] ={0};


uint16_t hisense_ble_netconfig_test(uint8_t * ptr_ch,uint32_t ptr_len,uint16_t handle)
{


    if(ptr_ch == NULL||ptr_len<1)
        return;

    uint16_t recv_tag = 0;
    uint16_t recv_len = 0;
    uint8_t* recv_value = NULL;  
    struct ln_gattc_send_evt_cmd hisense_BLE_send_data;
    struct ln_gattc_write_cmd param_wr;


    
    hisense_BLE_send_data.handle = handle;

    
    recv_tag |= ((ptr_ch[0]<<8)|ptr_ch[1]);
    recv_len |= ((ptr_ch[2]<<8)|ptr_ch[3]);
    


    switch(recv_tag)
    {

        case TAG_BT_CMD_SCAN :  
        {
            uint16_t scan_info_len = 0;
            uint8_t scan_info[256] = {0};
            uint8_t count = ble_scan_ap_list_count;
            if(count == 0)
            {              
                scan_info[0] = ((TAG_BT_CMD_REPLY_SCAN>>8)&0x00ff);
                scan_info[1] = TAG_BT_CMD_REPLY_SCAN&0x00ff;
                scan_info[2] = 0x00;
                scan_info[3] = 0x04;
                scan_info[4] = 0xff;
                scan_info[5] = 0xff;
                scan_info[6] = 0xff;
                scan_info[7] = 0xff;
                scan_info_len = 8;

                
                hisense_BLE_send_data.length = scan_info_len;
                hisense_BLE_send_data.value  = scan_info;
                hisense_ble_write_data(scan_info,scan_info_len);
                ln_app_gatt_send_ntf(ln_conidx,&hisense_BLE_send_data);

                
            }
            else if(count > 0)
            {
                int i = 0;
                while(1)
                {
                    int tmp = count - i;
                    tmp = tmp>4?4:tmp;

                    scan_info[0] = ((TAG_BT_CMD_REPLY_SCAN>>8)&0x00ff);
                    scan_info[1] = TAG_BT_CMD_REPLY_SCAN&0x00ff;
                    scan_info[2] = 0x00;                
                    scan_info[3] = 2+tmp*sizeof(ble_scan_ap_info_node);    
                    scan_info[4] = count;
                    scan_info[5] = tmp;

                    memcpy(&scan_info[6],&ble_scan_ap_list[i],tmp*sizeof(ble_scan_ap_info_node));
                    
                    i += tmp;

                    scan_info_len = 6+tmp*sizeof(ble_scan_ap_info_node);

                    hisense_BLE_send_data.length = scan_info_len;
                    hisense_BLE_send_data.value  = scan_info;
                    hisense_ble_write_data(scan_info,scan_info_len);
                    ln_app_gatt_send_ntf(ln_conidx,&hisense_BLE_send_data);

                    

                    if(i == count) 
                    {
                        scan_info[0] = ((TAG_BT_CMD_REPLY_SCAN>>8)&0x00ff);
                        scan_info[1] = TAG_BT_CMD_REPLY_SCAN&0x00ff;
                        scan_info[2] = 0x00;                
                        scan_info[3] = 0x00;    
                        scan_info[4] = 0x00;
                        scan_info[5] = 0x00;
                          
                        hisense_BLE_send_data.length = scan_info_len;
                        hisense_BLE_send_data.value  = scan_info;
                        hisense_ble_write_data(scan_info,scan_info_len);
                        ln_app_gatt_send_ntf(ln_conidx,&hisense_BLE_send_data);
                        
                        break;
                    }
                }
                break;

            }

            break;
        }            
        case TAG_BT_CMD_CONNECT:
        {
            uint8_t his_src_wifi_info[128] = {0};
            uint8_t his_wifi_info_len = ptr_ch[4];

            memcpy(his_src_wifi_info,ptr_ch+4,his_wifi_info_len);

            for(int j=0; j<his_wifi_info_len+4; j++)
            {
                if(j%16 == 0)LOG(LOG_LVL_INFO, "\r\n");
                LOG(LOG_LVL_INFO, "%.2x ",ptr_ch[j]);
            }
            LOG(LOG_LVL_INFO, "(app 原始数据）\r\n");cias_task_delay_ms(5);

           
            for(int j=0; j<his_wifi_info_len; j++)
            {
                if(j%16 == 0)LOG(LOG_LVL_INFO, "\r\n");
                LOG(LOG_LVL_INFO, "%.2x ",his_src_wifi_info[j]);
            }
            
            uint8_t ch = 0;
            for(int i = 0;i<his_wifi_info_len-1;i++) ch^=his_src_wifi_info[i];
            
            if(ch != his_src_wifi_info[his_wifi_info_len-1])
            {
                LOG(LOG_LVL_INFO, "数据校验出错\r\n");
                break;
            }
            else
            {
                LOG(LOG_LVL_INFO, "（协议 原始数据 校验值:%x)\r\n",ch);cias_task_delay_ms(5);
            }


            
            uint16_t test = his_src_wifi_info[his_wifi_info_len-2]<<8;//1，1.1解码 bssid 长度
            test |= his_src_wifi_info[3];
            test = test>>his_src_wifi_info[2];
            

            for(int i = his_wifi_info_len;i>3;i--)//1，1.2解码 其他数据
            {      
                uint16_t test2 = his_src_wifi_info[i-1]<<8;
                test2 |= his_src_wifi_info[i];          
                his_src_wifi_info[i] = test2>>his_src_wifi_info[2];
            }
            his_src_wifi_info[3] = test;


            for(int j=0; j<his_wifi_info_len; j++)
            {
                if(j%16 == 0)LOG(LOG_LVL_INFO, "\r\n");
                LOG(LOG_LVL_INFO, "%.2x ",his_src_wifi_info[j]);
            }
            LOG(LOG_LVL_INFO, "（解码后数据）\r\n");cias_task_delay_ms(5);

            

            //2，bssid 解析
            hisense_bssid_len = his_src_wifi_info[3];
            for(int i = 0;i<hisense_bssid_len;i++)
            {
                hisense_bssid_src[i] = his_src_wifi_info[4+i];
            }

            LOG(LOG_LVL_INFO, "wifi  id :%.2d\r\n",his_src_wifi_info[1]);cias_task_delay_ms(5);
            LOG(LOG_LVL_INFO, "wifi  mac:%.2d,%s\r\n",hisense_bssid_len,hisense_bssid_src);cias_task_delay_ms(10);
            
            //3,根据bssid在ap列表中找到目标ssid// ble_scan_ap_list[i].bssid_octer hisense_bssid_src;
            uint8_t temp_find_ssid[24] = {0};
            for(int i = 0;i<ble_scan_ap_list_count;i++)
            {
                
                uint8_t *mac = ble_scan_ap_list[i].bssid_octer;
                bzero(temp_find_ssid,sizeof(temp_find_ssid));
                sprintf(temp_find_ssid,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

                if(strstr(temp_find_ssid,hisense_bssid_src))
                {
                    ble_connect.ssid = ble_scan_ap_list[i].ssid;
                    g_HISENSE_BLE_CONFIG_EXIT = 1;

                    LOG(LOG_LVL_INFO, "find  mac:%.2d,%s\r\n",sizeof(temp_find_ssid),temp_find_ssid);cias_task_delay_ms(5);
                    LOG(LOG_LVL_INFO, "wifi ssid:%.2d,%s\r\n",ble_scan_ap_list[i].ssid_len,ble_connect.ssid);cias_task_delay_ms(5);
                    break;
                }

            }


            //4,pasword 解析
            hisense_pwd_len = his_src_wifi_info[4+hisense_bssid_len]; 
            for(int i = 0;i<hisense_pwd_len;i++)
            {
                hisense_pwd_src[i] = his_src_wifi_info[5+hisense_bssid_len+i];
            }  
            
            LOG(LOG_LVL_INFO, "wifi  pwd:%.2d,%s\r\n\n",hisense_pwd_len,hisense_pwd_src);

            //5, 5.1连接目标路由，5.2将数据写入flash
            ble_connect.bssid = hisense_bssid_src;
            ble_connect.pwd = hisense_pwd_src;
            cias_task_delay_ms(50);

            break;
        }
        case TAG_BT_CMD_GET_STATUS :
        {
            uint8_t his_wifi_status[6] = {0};
            his_wifi_status[0] = ((TAG_BT_CMD_REPLY_GET_STATUS>>8)&0x00ff);
            his_wifi_status[1] = TAG_BT_CMD_REPLY_GET_STATUS&0x00ff;
            his_wifi_status[2] = 0x00;
            his_wifi_status[3] = 0x02;
            extern int ip_addr_get_status;
            if(ip_addr_get_status) 
            {
                g_HISENSE_BLE_CONFIG_EXIT = 1;
                his_wifi_status[4] = 0x04;
            }                
            else 
            {
                // g_HISENSE_BLE_CONFIG_EXIT = 0;
                his_wifi_status[4] = 0x01;
            }
                
            
            his_wifi_status[5] = 0x01;
            hisense_BLE_send_data.length = 6;
            hisense_BLE_send_data.value  = his_wifi_status;
            ln_app_gatt_send_ntf(ln_conidx,&hisense_BLE_send_data);
            hisense_ble_write_data(his_wifi_status,6);

            break;

        }
        case TAG_BT_CMD_GET_WLAN_BAND:
        {
            break;
        }
        case TAG_BT_CMD_GET_WIFI_ID :
        {
            uint8_t his_wifi_id[40] = {0};
            uint8_t*wifi_id_p = WIFIID;
            uint16_t wifi_id_len  = sizeof(WIFIID)-1;
            his_wifi_id[0] = ((TAG_BT_CMD_REPLY_WIFI_ID>>8)&0x00ff);
            his_wifi_id[1] = TAG_BT_CMD_REPLY_WIFI_ID&0x00ff;
            his_wifi_id[2] = ((wifi_id_len>>8)&0x00ff);
            his_wifi_id[3] = wifi_id_len&0x00ff;

            for(int i=0;i<wifi_id_len;i++)
            {
                his_wifi_id[4+i] = wifi_id_p[i];
            }       

            hisense_BLE_send_data.length = wifi_id_len+4;
            hisense_BLE_send_data.value  = his_wifi_id;
            ln_app_gatt_send_ntf(ln_conidx,&hisense_BLE_send_data);
            hisense_ble_write_data(his_wifi_id,wifi_id_len+4);

            break;
            
        }
        case TAG_BT_CMD_GET_DEV_ID  :
        {
            uint8_t his_dev_id[4] = {0};


            his_dev_id[0] = ((TAG_BT_CMD_REPLY_DEV_ID>>8)&0x00ff);
            his_dev_id[1] = TAG_BT_CMD_REPLY_DEV_ID&0x00ff;
            his_dev_id[2] = 0;
            his_dev_id[3] = 0;


            hisense_BLE_send_data.length = 4;
            hisense_BLE_send_data.value  = his_dev_id;
            ln_app_gatt_send_ntf(ln_conidx,&hisense_BLE_send_data);
 
            hisense_ble_write_data(his_dev_id,4);

            break;

        }
        case TAG_BT_CMD_SSID_CONNECT :
        case TAG_BT_CMD_START_SCAN :
        case TAG_BT_CMD_REPLY_FAIL:
        default:
        {
            LOG(LOG_LVL_DEBUG, "tag:0x%.4x ,2len:%d\r\n",recv_tag,recv_len);
            break;
        }
        
    }

    memset(ptr_ch,0,ptr_len);
    return ;
}
#endif