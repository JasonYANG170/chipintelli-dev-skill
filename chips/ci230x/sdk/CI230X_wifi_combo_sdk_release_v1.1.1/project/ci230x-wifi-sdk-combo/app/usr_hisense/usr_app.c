#include "osal/osal.h"
#ifndef PROJECT_WIFI_SSID
#define PROJECT_WIFI_SSID ""
#endif
#ifndef PROJECT_WIFI_PASSWORD
#define PROJECT_WIFI_PASSWORD ""
#endif
#include "utils/debug/log.h"
#include "wifi.h"
#include "wifi_port.h"
#include "netif/ethernetif.h"
#include "wifi_manager.h"
#include "lwip/tcpip.h"
#include "drv_adc_measure.h"
#include "utils/debug/ln_assert.h"
#include "utils/system_parameter.h"
#include "utils/sysparam_factory_setting.h"
#include "utils/ln_psk_calc.h"
#include "utils/power_mgmt/ln_pm.h"
#include "hal/hal_adc.h"
#include "ln_nvds.h"
#include "ln_wifi_err.h"
#include "ln_misc.h"
#include "ln882h.h"
#include "usr_app.h"
#include "cias_user_config.h"

#include "cias_demo_config.h"
#include "cias_ble_config.h"
#include "Config.h"
#if CONFIG_APP_CIAS_CLOUD_FUNC_TEST
#include "cias_lan_network.h"
#endif

#if NET_AUDIO_DOWNLOAD_ENABLE
#include "cias_media.h"
#endif

#if CIAS_IOT_CLOUD_HISENSE_ENABLE
#include "hisense_main_task.h"
#include "hisense_wifi_message_deal.h"
#include "hisense_ble_message_deal.h"
#endif

#if TENCENT_AP_NET_WORK_EN
#include "cias_log.h"
#include "cias_freertos_common.h"
#include "cias_system_manage.h"
#include "Config.h"
#include "cias_ap_net_port.h"
extern ci_ap_net_t ci_ap_net;
#endif//TENCENT_AP_NET_WORK_EN
#if CIAS_AIOT_WIFI_OTA_ENABLE
#include "cias_wifi_ota_config.h"
#endif//CIAS_AIOT_WIFI_OTA_ENABLE

#define PM_DEFAULT_SLEEP_MODE             (ACTIVE)
#define PM_WIFI_DEFAULT_PS_MODE           (WIFI_NO_POWERSAVE)
#define WIFI_TEMP_CALIBRATE               (1)

#define USR_APP_TASK_STACK_SIZE           (6*256) //Byte

#if WIFI_TEMP_CALIBRATE
static OS_Thread_t g_temp_cal_thread;
#define TEMP_APP_TASK_STACK_SIZE          (4*256) //Byte
#endif

static OS_Thread_t g_usr_app_thread;

/* declaration */
#if TENCENT_AP_NET_WORK_EN
extern int wifi_softAP_connect_flag;
void wifi_init_ap(void);
void wifi_init_sta(void);
#else//TENCENT_AP_NET_WORK_EN
static void wifi_init_ap(void);
static void wifi_init_sta(void);
#endif//TENCENT_AP_NET_WORK_EN
static void usr_app_task_entry(void *params);
static void temp_cal_app_task_entry(void *params);

static uint8_t mac_addr[6]        = {0x00, 0x50, 0xC2, 0x5E, 0x88, 0x99};
static uint8_t psk_value[40]      = {0x0};
// static uint8_t target_ap_bssid[6] = {0xC0, 0xA5, 0xDD, 0x84, 0x6F, 0xA8};
wifi_sta_connect_t ble_connect = {
    .ssid    = NULL,
    .pwd     = NULL,
    .bssid   = NULL,
    .psk_value = NULL,
};
wifi_sta_connect_t hisense_ap_connect = {
    .ssid    = NULL,
    .pwd     = NULL,
    .bssid   = NULL,
    .psk_value = NULL,
};
wifi_sta_connect_t connect = {

    .ssid = PROJECT_WIFI_SSID,
    .pwd = PROJECT_WIFI_PASSWORD,
    // .bssid = PROJECT_WIFI_SSID,
    // .ssid = PROJECT_WIFI_SSID,
    // .pwd = PROJECT_WIFI_PASSWORD,    
    // .bssid = PROJECT_WIFI_SSID,
    // .bssid = PROJECT_WIFI_SSID,
    .psk_value = NULL,
};

wifi_scan_cfg_t scan_cfg = {
    .channel = 0,
    .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    .scan_time = 20,
};

wifi_softap_cfg_t ap_cfg = {
    // .ssid = PROJECT_WIFI_SSID,
    .ssid = PROJECT_WIFI_SSID,
    .pwd = PROJECT_WIFI_PASSWORD,
    .bssid = mac_addr,
    .ext_cfg = {
        .channel = 6,
        .authmode = WIFI_AUTH_WPA_WPA2_PSK, //WIFI_AUTH_OPEN,
        .ssid_hidden = 0,
        .beacon_interval = 100,
        .psk_value = NULL,
    }
};

static void wifi_scan_complete_cb(void * arg)
{
#if 1    //remove by yjd, 删除扫描打印
    LN_UNUSED(arg);

    ln_list_t *list;
    uint8_t node_count = 0;
    ap_info_node_t *pnode;

    wifi_manager_ap_list_update_enable(LN_FALSE);

    // 1.get ap info list.
    wifi_manager_get_ap_list(&list, &node_count);

    // 2.print all ap info in the list.
    LN_LIST_FOR_EACH_ENTRY(pnode, ap_info_node_t, list,list)
    {
        uint8_t * mac = (uint8_t*)pnode->info.bssid;
        ap_info_t *ap_info = &pnode->info;

        // LOG(LOG_LVL_INFO, "\tCH=%2d,RSSI= %3d,", ap_info->channel, ap_info->rssi);
        // LOG(LOG_LVL_INFO, "BSSID:[%02X:%02X:%02X:%02X:%02X:%02X],SSID:\"%s\"\r\n", 
        //                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ap_info->ssid);
        hisense_ap_data_write(ap_info);
        hisense_ble_scan_ap_list_write(ap_info);
    }

    wifi_manager_ap_list_update_enable(LN_TRUE);
#endif
}

#define SSID_FLASH_ADDR     10
#define PWD_FLASH_ADDR      60
char ssid_in_flash[33] = {0};
char pwd_in_flash[64] = {0};

int8_t get_sta_from_flash(wifi_sta_connect_t *get_connect)
{
    int flash_ret = 0;

    flash_ret = ln_nvds_read(SSID_FLASH_ADDR,ssid_in_flash,sizeof(ssid_in_flash));
    flash_ret += ln_nvds_read(PWD_FLASH_ADDR,pwd_in_flash,sizeof(pwd_in_flash));

    if(ble_connect.ssid != NULL && ble_connect.pwd!=NULL )
    {
        get_connect->ssid = ble_connect.ssid;
        get_connect->pwd =  ble_connect.pwd;
    }
    // else if(ap_cfg.ssid != NULL && ap_cfg.pwd!=NULL )
    // {
    //     get_connect->ssid = ap_cfg.ssid;
    //     get_connect->pwd =  ap_cfg.pwd;
    // }
    else if(flash_ret == 0&&(ssid_in_flash[0] == 'H'))
    {
        get_connect->ssid = &ssid_in_flash[1];
        get_connect->pwd =  pwd_in_flash;
    }

    LOG(LOG_LVL_INFO,"hisense ssid :%s\r\n",get_connect->ssid);
    LOG(LOG_LVL_INFO,"hisense pwd  :%s\r\n",get_connect->pwd);
    LOG(LOG_LVL_INFO,"hisense bssid:%s\r\n",get_connect->bssid);
    // LOG(LOG_LVL_INFO,"\r\nBLE ssid:%s,pwd:%s,bssid:%s\r\n",ble_connect.ssid,ble_connect.pwd,ble_connect.bssid);
    
    return 0;

}


int8_t set_sta_to_flash(wifi_sta_connect_t *set_connect)
{
    int flash_ret = 0;
    uint8_t ssid_to_flash[33] = {0};
    sprintf(ssid_to_flash,"%c%s",'H',set_connect->ssid);

    if(strlen(set_connect->pwd) != 0)
    {
        flash_ret = 0;
        flash_ret += ln_nvds_write(SSID_FLASH_ADDR,ssid_to_flash,sizeof(ssid_in_flash));
        flash_ret += ln_nvds_write(PWD_FLASH_ADDR,(uint8_t*)(set_connect->pwd),sizeof(pwd_in_flash));

        LOG(LOG_LVL_INFO,"2nvds write ssid:%s,pwd:%s----------------\r\n",ssid_to_flash,set_connect->pwd);
        return 0;
    }

    return -1;
}
void wifi_init_sta(void)
{
    sta_ps_mode_t ps_mode = PM_WIFI_DEFAULT_PS_MODE;

    //1. sta mac get
    if (SYSPARAM_ERR_NONE != sysparam_sta_mac_get(mac_addr))
    {
        LOG(LOG_LVL_ERROR, "[%s]sta mac get filed!!!\r\n", __func__);
        return;
    }

    if (mac_addr[0] == STA_MAC_ADDR0 &&
        mac_addr[1] == STA_MAC_ADDR1 &&
        mac_addr[2] == STA_MAC_ADDR2 &&
        mac_addr[3] == STA_MAC_ADDR3 &&
        mac_addr[4] == STA_MAC_ADDR4 &&
        mac_addr[5] == STA_MAC_ADDR5)
    {
        ln_generate_random_mac(mac_addr);
        sysparam_sta_mac_update((const uint8_t *)mac_addr);
    }

    //1. net device(lwip)
    netdev_set_mac_addr(NETIF_IDX_STA, mac_addr);
    netdev_set_active(NETIF_IDX_STA);

    //2. wifi start
    wifi_manager_reg_event_callback(WIFI_MGR_EVENT_STA_SCAN_COMPLETE, &wifi_scan_complete_cb);

    if(WIFI_ERR_NONE != wifi_sta_start(mac_addr, ps_mode)){
        LOG(LOG_LVL_ERROR, "[%s]wifi sta start filed!!!\r\n", __func__);
    }
    //3. 获取sta info
    get_sta_from_flash(&connect);

    connect.psk_value = NULL;
    if (strlen(connect.pwd) != 0)
    {
        if (0 == ln_psk_calc(connect.ssid, connect.pwd, psk_value, sizeof(psk_value)))
        {
            connect.psk_value = psk_value;
            hexdump(LOG_LVL_INFO, "psk value ", psk_value, sizeof(psk_value));
        }
        if(strlen(connect.ssid) != 0)
        wifi_sta_connect(&connect, &scan_cfg);
    }

    
}

static void ap_startup_cb(void *arg)
{
    netdev_set_state(NETIF_IDX_AP, NETDEV_UP);
}

void wifi_init_ap(void)
{
    tcpip_ip_info_t ip_info;
    server_config_t server_config;

    ip_info.ip.addr = ipaddr_addr((const char *)"192.168.4.1");
    ip_info.gw.addr = ipaddr_addr((const char *)"192.168.4.1");
    ip_info.netmask.addr = ipaddr_addr((const char *)"255.255.255.0");

    server_config.server.addr = ip_info.ip.addr;
    server_config.port = 67;
    server_config.lease = 2880;
    server_config.renew = 2880;
    server_config.ip_start.addr = ipaddr_addr((const char *)"192.168.4.100");
    server_config.ip_end.addr = ipaddr_addr((const char *)"192.168.4.150");
    server_config.client_max = 3;
    dhcpd_curr_config_set(&server_config);

    //1. net device(lwip).
    netdev_set_mac_addr(NETIF_IDX_AP, mac_addr);
    netdev_set_ip_info(NETIF_IDX_AP, &ip_info);
    netdev_set_active(NETIF_IDX_AP);
    wifi_manager_reg_event_callback(WIFI_MGR_EVENT_SOFTAP_STARTUP, &ap_startup_cb);

    sysparam_softap_mac_update((const uint8_t *)mac_addr);

    ap_cfg.ext_cfg.psk_value = NULL;
    if ((strlen(ap_cfg.pwd) != 0) &&
        (ap_cfg.ext_cfg.authmode != WIFI_AUTH_OPEN) &&
        (ap_cfg.ext_cfg.authmode != WIFI_AUTH_WEP))
    {
        memset(psk_value, 0, sizeof(psk_value));
        if (0 == ln_psk_calc(ap_cfg.ssid, ap_cfg.pwd, psk_value, sizeof(psk_value)))
        {
            ap_cfg.ext_cfg.psk_value = psk_value;
            hexdump(LOG_LVL_INFO, "psk value ", psk_value, sizeof(psk_value));
        }
    }

    //2. wifi
    if (WIFI_ERR_NONE != wifi_softap_start(&ap_cfg))
    {
        LOG(LOG_LVL_ERROR, "[%s, %d]wifi_start() fail.\r\n", __func__, __LINE__);
    }
}


void usr_app_task_entry(void *params)
{
    LN_UNUSED(params);
#if TENCENT_AP_NET_WORK_EN
    int link_fail_state_num = 0;
#endif//TENCENT_AP_NET_WORK_EN
    wifi_manager_init();

    wifi_init_sta();

    while (NETDEV_LINK_UP != netdev_get_link_state(netdev_get_active()))
    {
        // OS_MsDelay(1000);
#if CIAS_BLE_CONFIG_ENABLE
        if(ble_connect.ssid != NULL)
        {
            wifi_stop();
            wifi_init_sta();
            ble_connect.ssid = NULL;
            ble_connect.pwd = NULL;
        }
#endif
#if TENCENT_AP_NET_WORK_EN
        if(link_fail_state_num != -1)
            link_fail_state_num++;
        if(link_fail_state_num > 10)
        {
            link_fail_state_num = -1;
            wifi_softAP_connect_flag = 1;
            LOG(LOG_LVL_INFO,"net_link_fail!!!\r\n");
        }
#endif// TENCENT_AP_NET_WORK_EN
        OS_MsDelay(1000);
    }
    while (1)
    { 
        OS_MsDelay(1000);
#if (CIAS_BLE_CONFIG_ENABLE||CIAS_IOT_CLOUD_HISENSE_ENABLE)
        if(ble_connect.ssid != NULL)
        {
            wifi_stop();
            wifi_init_sta();
            ble_connect.ssid = NULL;
            ble_connect.pwd = NULL;
        }
#endif

// #if CIAS_IOT_CLOUD_HISENSE_ENABLE
//         if(hisense_ap_connect.ssid != NULL)
//         {
//             wifi_stop();
//             connect.ssid = hisense_ap_connect.ssid;
//             connect.pwd  = hisense_ap_connect.pwd;
//             wifi_init_sta();
//             hisense_ap_connect.ssid = NULL;
//         }
// #endif

    }
}

void temp_cal_app_task_entry(void *params)
{
    LN_UNUSED(params);
    uint8_t cnt = 0;
    int8_t cap_comp = 0;
    uint16_t adc_val = 0;
    int16_t curr_adc = 0;

    if (NVDS_ERR_OK == ln_nvds_get_xtal_comp_val((uint8_t *)&cap_comp)) {
        if ((uint8_t)cap_comp == 0xFF) {
            cap_comp = 0;
        }
    }

    drv_adc_init();

    wifi_temp_cal_init(drv_adc_read(ADC_CH0), cap_comp);

    while (1)
    {
        OS_MsDelay(1000);

        adc_val = drv_adc_read(ADC_CH0);
        wifi_do_temp_cal_period(adc_val);

        curr_adc = (adc_val & 0xFFF);

        cnt++;
        if ((cnt % 60) == 0) {
            LOG(LOG_LVL_INFO, "adc raw: %4d, temp_IC: %4d\r\n",
                    curr_adc, (int16_t)(25 + (curr_adc - 770) / 2.54f));
            LOG(LOG_LVL_INFO, "Total:%d; Free:%ld;\r\n", 
                    OS_HeapSizeGet(), OS_GetFreeHeapSize());
            
            cias_heap_info();
        }
    }
}

int creat_usr_app_task(void)
{
    {
        ln_pm_sleep_mode_set(PM_DEFAULT_SLEEP_MODE);

        /**
         * CLK_G_EFUSE: For wifi temp calibration
         * CLK_G_BLE  CLK_G_I2S  CLK_G_WS2811  CLK_G_DBGH  CLK_G_SDIO  CLK_G_EFUSE  CLK_G_AES
        */
        ln_pm_always_clk_disable_select(CLK_G_I2S | CLK_G_WS2811 | CLK_G_SDIO | CLK_G_AES);

        /**
         * ADC0: For wifi temp calibration
         * TIM3: For wifi pvtcmd evm test
         * CLK_G_ADC  CLK_G_GPIOA  CLK_G_GPIOB  CLK_G_SPI0  CLK_G_SPI1  CLK_G_I2C0  CLK_G_UART1  CLK_G_UART2
         * CLK_G_WDT  CLK_G_TIM_REG  CLK_G_TIM1  CLK_G_TIM2  CLK_G_TIM3  CLK_G_TIM4  CLK_G_MAC  CLK_G_DMA
         * CLK_G_RF  CLK_G_ADV_TIMER  CLK_G_TRNG
        */
        ln_pm_lightsleep_clk_disable_select(CLK_G_GPIOA | CLK_G_GPIOB | CLK_G_SPI0 | CLK_G_SPI1 | CLK_G_I2C0 |
                                            CLK_G_UART1 | CLK_G_UART2 | CLK_G_WDT | CLK_G_TIM_REG | CLK_G_TIM1 | CLK_G_TIM2 | CLK_G_TIM4 | CLK_G_MAC | CLK_G_DMA | CLK_G_RF | CLK_G_ADV_TIMER| CLK_G_TRNG);
    }

    if(OS_OK != OS_ThreadCreate(&g_usr_app_thread, "UsrAPP", usr_app_task_entry, NULL, OS_PRIORITY_BELOW_NORMAL, USR_APP_TASK_STACK_SIZE)) {
        LN_ASSERT(1);
    }

    /* print sdk version */
    {
        LOG(LOG_LVL_INFO, "LN882H SDK Ver: %s [build time:%s][0x%08x]\r\n",
        LN882H_SDK_VERSION_STRING, LN882H_SDK_BUILD_DATE_TIME, LN882H_SDK_VERSION);
    }


    LOG(LOG_LVL_INFO,"********>>>>>Heap left: %d <start> min:%d<<<<<********\r\n", \
    xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());

#if (CIAS_BLE_CONFIG_ENABLE ||CIAS_IOT_CLOUD_HISENSE_ENABLE)
        if(OS_OK != OS_ThreadCreate(&ble_g_usr_app_thread, "BleUsrAPP", ln_ble_app_task_entry, NULL, OS_PRIORITY_BELOW_NORMAL, BLE_USR_APP_TASK_STACK_SIZE)) 
        {
            LN_ASSERT(1);
        }
#endif

#if WIFI_TEMP_CALIBRATE
    if (OS_OK != OS_ThreadCreate(&g_temp_cal_thread, "TempAPP", temp_cal_app_task_entry, NULL, OS_PRIORITY_BELOW_NORMAL, TEMP_APP_TASK_STACK_SIZE))
    {
        LN_ASSERT(1);
    }
#endif

#if TENCENT_AP_NET_WORK_EN
    ci_ap_net_port_init();
#endif

#if NET_AUDIO_DOWNLOAD_ENABLE
    if (cias_media_init_interface() != CIAS_OK)
    {
        LOG(LOG_LVL_ERROR, "cias_media_init_interface call error\r\n");
        return CIAS_FAIL;
    }
#endif


#if CIAS_AIOT_AUDIO_OTA_ENABLE
    if (cias_ota_main() != CIAS_OK)
    {
        LOG(LOG_LVL_ERROR, "cias_ota_main init error");
        return CIAS_FAIL;
    }
#endif

#if CIAS_AIOT_WIFI_OTA_ENABLE
    if( cias_wifi_ota_main() != CIAS_OK )
    {
        return CIAS_FAIL;
    }
#endif


#if CONFIG_APP_CIAS_CLOUD_FUNC_TEST
	ci_lan_network_test_task();
#endif

#if CIAS_IOT_CLOUD_HISENSE_ENABLE
    ci_hisense_main();
#endif

#if CIAS_LN_HARDWARE_TEST_ENABLE
    cias_ln_hardware_init();
#endif

#if CIAS_SYSTEM_MANAGE_ENABLE
    // cias_system_manage();   //系统监控任务
#endif

    LOG(LOG_LVL_INFO,"********>>>>>Heap left: %d <end> min:%d<<<<<********************\r\n",\
    xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());

    return CIAS_OK;
}
