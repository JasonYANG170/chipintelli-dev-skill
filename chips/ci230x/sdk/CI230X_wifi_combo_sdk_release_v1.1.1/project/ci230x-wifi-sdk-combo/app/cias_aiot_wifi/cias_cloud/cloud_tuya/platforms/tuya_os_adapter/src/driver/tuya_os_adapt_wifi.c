/**
 * @file tuya_os_adapt_wifi.c
 * @brief wifi操作接口
 *
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 *
 */

#include <string.h>

#include "tuya_os_adapt_wifi.h"
#include "tuya_os_adapter.h"
#include "tuya_os_adapter_error_code.h"
#include "tuya_iot_config.h"

#include "hal/hal_adc.h"
#include "drv_adc_measure.h"

#include "netif/ethernetif.h"
#include "wifi.h"
#include "wifi_port.h"
#include "wifi_manager.h"
#include "ln_kv_api.h"
#include "ln_psk_calc.h"
#include "ln_nvds.h"
#include "ln_utils.h"

#include "utils/debug/log.h"
#include "utils/system_parameter.h"
#include "utils/power_mgmt/ln_pm.h"

#include "lwip/inet.h"
#include "lwip/netif.h"

#undef  UNUSED
#define UNUSED(x) (void)x

#define TUYA_PSK_INFO_STORE_NUM           (1)  // now, only support 1
#define TUYA_PSK_VAL_LEN                  (40)
#define WIFI_PARAMS_GET()                 (&s_tuya_wifi_params)

#undef  TUYA_PSK_KV_INFO
#define TUYA_PSK_KV_INFO                  ((const char *)"ty_ada_psk")

#define TY_ADA_WIFI_PS_MODE           (WIFI_MAX_POWERSAVE) // used mode when power-save enabled
#define TY_ADA_WIFI_DEFAULT_PS_MODE   (WIFI_NO_POWERSAVE)  // default mode

typedef struct {
    uint8_t ssid[SSID_MAX_LEN];
    uint8_t pwd[PASSWORD_MAX_LEN];
    uint8_t pwd_len;
    uint8_t psk[TUYA_PSK_VAL_LEN];
} tuya_hal_wifi_psk_info_t;

typedef struct {
    int                 ty_wifi_started;
    WF_WK_MD_E          mode;
    COUNTRY_CODE_E      ccode;
    sta_ps_mode_t       ps_mode;
    OS_Semaphore_t      scan_comp;
    int                 wait_scan;
    int                 ty_wifi_status;
    WIFI_REV_MGNT_CB    mgnt_recv_cb;
    SNIFFER_CALLBACK    sniffer_cb;
} tuya_hal_wifi_params_t;


static void ap_startup_cb(void *arg);
static void wifi_scan_complete_cb(void *arg);
static void wifi_disconnect_cb(void *arg);
static void tuya_wifi_get_ip_cb(struct netif *nif);
static void tuya_hal_wifi_sniffer_handler(void *buf, uint16_t len, wifi_pkt_type_t pkt_type, int8_t rssi_db);
static int tuya_hal_wifi_psk_update(const tuya_hal_wifi_psk_info_t * psk_info);
static int tuya_hal_wifi_psk_get(tuya_hal_wifi_psk_info_t * psk_info);
static int tuya_hal_wifi_get_country_code(COUNTRY_CODE_E *ccode);
static int tuya_hal_wifi_start(WF_WK_MD_E mode);


static tuya_hal_wifi_params_t s_tuya_wifi_params = {
    .ty_wifi_started = 0,
    .mode = WWM_STATION,
    .ccode = COUNTRY_CODE_CN,
    .ps_mode = TY_ADA_WIFI_DEFAULT_PS_MODE,
};

static void *s_sem_scan = NULL;
static OS_Timer_t s_wifi_temp_cal_timer = {0};

static const TUYA_OS_WIFI_INTF m_tuya_os_wifi_intfs = {
    .all_ap_scan                  = tuya_os_adapt_wifi_all_ap_scan,
    .assign_ap_scan               = tuya_os_adapt_wifi_assign_ap_scan,
    .release_ap                   = tuya_os_adapt_wifi_release_ap,
    .set_cur_channel              = tuya_os_adapt_wifi_set_cur_channel,
    .get_cur_channel              = tuya_os_adapt_wifi_get_cur_channel,
    .sniffer_set                  = tuya_os_adapt_wifi_sniffer_set,
    .get_ip                       = tuya_os_adapt_wifi_get_ip,
    .set_mac                      = tuya_os_adapt_wifi_set_mac,
    .get_mac                      = tuya_os_adapt_wifi_get_mac,
    .set_work_mode                = tuya_os_adapt_wifi_set_work_mode,
    .get_work_mode                = tuya_os_adapt_wifi_get_work_mode,
    .ap_start                     = tuya_os_adapt_wifi_ap_start,
    .ap_stop                      = tuya_os_adapt_wifi_ap_stop,
    .get_connected_ap_info_v2     = tuya_os_adapt_wifi_get_connected_ap_info_v2,
    .fast_station_connect_v2      = tuya_os_adapt_wifi_fast_station_connect_v2,
    .station_connect              = tuya_os_adapt_wifi_station_connect,
    .station_disconnect           = tuya_os_adapt_wifi_station_disconnect,
    .station_get_conn_ap_rssi     = tuya_os_adapt_wifi_station_get_conn_ap_rssi,
    .get_bssid                    = tuya_os_adapt_wifi_get_bssid,
    .station_get_status           = tuya_os_adapt_wifi_station_get_status,
    .set_country_code             = tuya_os_adapt_wifi_set_country_code,
    .send_mgnt                    = tuya_os_adapt_wifi_send_mgnt,
    .register_recv_mgnt_callback  = tuya_os_adapt_wifi_register_recv_mgnt_callback,
    .set_lp_mode                  = tuya_os_adapt_set_wifi_lp_mode,
    .rf_calibrated                = tuya_os_adapt_wifi_rf_calibrated,
};


static void wifi_scan_complete_cb(void *arg)
{
    UNUSED(arg);
    tuya_hal_semaphore_post(s_sem_scan);
}

static void wifi_disconnect_cb(void *arg)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();
    UNUSED(arg);
    wifi_params->ty_wifi_status = WSS_IDLE;
    wifi_set_allow_cpu_sleep_flag(0);
}

static void tuya_wifi_get_ip_cb(struct netif *nif)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();
    UNUSED(nif);
    wifi_params->ty_wifi_status = WSS_GOT_IP;
}

/**
 * @brief scan current environment and obtain all the ap
 *        infos in current environment
 *
 * @param[out]      ap_ary      current ap info array
 * @param[out]      num         the num of found ap
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_all_ap_scan(AP_IF_S **ap_ary, unsigned int *num)
{
    wifi_mode_t mode;
    int ret = OPRT_OS_ADAPTER_OK;

    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(NULL == ap_ary || NULL == num) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (WIFI_MODE_STATION != (mode = wifi_current_mode_get()))
    {
        return OPRT_OS_ADAPTER_ERRCODE_WIFI;
    }

    if (WWM_STATION != wifi_params->mode)
    {
        LOG(LOG_LVL_ERROR, "Pattern does not match! mode:%d; param->mode:%d\r\n",
                mode, wifi_params->mode);
        return OPRT_OS_ADAPTER_ERRCODE_WIFI;
    }

    if (0 == wifi_params->ty_wifi_started) {
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start(WWM_STATION);
    }

    {
        #define CONNECTED_SCAN_TIMES     (6)
        #define DEFAULT_SCAN_TIMES       (1)
        #define SCAN_TIMEOUT             (1500)

        int8_t            scan_cnt   = DEFAULT_SCAN_TIMES;
        wifi_sta_status_t sta_status = WIFI_STA_STATUS_STARTUP;
        wifi_scan_cfg_t   scan_cfg   = {0,};
        sysparam_sta_scan_cfg_get(&scan_cfg);

        //1. creat sem, reg scan complete callback.
        if (0 != tuya_hal_semaphore_create_init(&s_sem_scan, 0, 1))
        {
            return OPRT_OS_ADAPTER_ERRCODE_WIFI;
        }

        wifi_manager_reg_event_callback(WIFI_MGR_EVENT_STA_SCAN_COMPLETE, &wifi_scan_complete_cb);

        //2. start scan, wait scan complete
        wifi_get_sta_status(&sta_status);
        if (sta_status == WIFI_STA_STATUS_CONNECTED || \
            sta_status == WIFI_STA_STATUS_DISCONNECTING)
        {
            scan_cnt = CONNECTED_SCAN_TIMES;
        }
        LOG(LOG_LVL_INFO, "Scan cnt:%d; scan timeout:%d\r\n", scan_cnt, SCAN_TIMEOUT);

        for (; scan_cnt > 0; scan_cnt--)
        {
            wifi_sta_scan(&scan_cfg);
            tuya_hal_semaphore_wait(s_sem_scan);
        }

        //3. scan complete,output ap list.
        {
            ln_list_t *list;
            uint8_t node_count = 0;
            ap_info_node_t *pnode;
            AP_IF_S *ap_info = NULL, *ap_itr = NULL;

            wifi_manager_ap_list_update_enable(LN_FALSE);
            wifi_manager_get_ap_list(&list, &node_count);

            LOG(LOG_LVL_INFO, "AP cnt:%d\r\n", node_count);
            if (node_count <= 0) {
                ret = OPRT_OS_ADAPTER_AP_NOT_FOUND;
                goto __finish;
            }

            // malloc memory to store ap list,and free in tuya_hal_wifi_release_ap()
            ap_info = tuya_os_adapt_system_malloc(sizeof(AP_IF_S) * node_count);
            if(NULL == ap_info) {
                ret = OPRT_OS_ADAPTER_MALLOC_FAILED;
                goto __finish;
            }

            ap_itr = ap_info;
            memset(ap_info, 0, (sizeof(AP_IF_S) * node_count));

            LN_LIST_FOR_EACH_ENTRY(pnode, ap_info_node_t, list, list)
            {
                ap_info_t *info = &pnode->info;

                ap_itr->channel = info->channel;
                ap_itr->rssi    = info->rssi;
                memcpy(ap_itr->bssid, info->bssid, BSSID_LEN);
                memcpy(ap_itr->ssid,  info->ssid,  WIFI_SSID_LEN + 1);
                ap_itr->ssid[WIFI_SSID_LEN] = '\0';

                ap_itr->s_len = strlen((const char *)ap_itr->ssid);
                ap_itr++;
            }

__finish:
            *num = node_count;
            *ap_ary = ap_info;
            wifi_manager_ap_list_update_enable(LN_TRUE);
        }

        //4. delete sem, callback
        wifi_manager_reg_event_callback(WIFI_MGR_EVENT_STA_SCAN_COMPLETE, NULL);
        tuya_hal_semaphore_release(s_sem_scan);
        wifi_manager_cleanup_scan_results();
        s_sem_scan = NULL;
    }

    return ret;
}


/**
 * @brief scan current environment and obtain the specific
 *        ap info.
 *
 * @param[in]       ssid        the specific ssid
 * @param[out]      ap          the ap info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_assign_ap_scan(const signed char *ssid, AP_IF_S **ap)
{
    int ret = OPRT_OS_ADAPTER_OK;
    wifi_mode_t mode;

    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(NULL == ssid || NULL == ap) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    if (WIFI_MODE_STATION != (mode = wifi_current_mode_get()))
    {
        return OPRT_OS_ADAPTER_ERRCODE_WIFI;
    }

    if (WWM_STATION != wifi_params->mode)
    {
        LOG(LOG_LVL_ERROR, "Pattern does not match! mode:%d; param->mode:%d\r\n",
                mode, wifi_params->mode);
        return OPRT_OS_ADAPTER_ERRCODE_WIFI;
    }

    if (0 == wifi_params->ty_wifi_started) {
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start(WWM_STATION);
    }

    {
        #define CONNECTED_SCAN_TIMES     (6)
        #define DEFAULT_SCAN_TIMES       (1)
        #define SCAN_TIMEOUT             (1500)

        int8_t            scan_cnt   = DEFAULT_SCAN_TIMES;
        wifi_sta_status_t sta_status = WIFI_STA_STATUS_STARTUP;
        wifi_scan_cfg_t   scan_cfg   = {0,};
        sysparam_sta_scan_cfg_get(&scan_cfg);

        //1. creat sem, reg scan complete callback.
        if (0 != tuya_hal_semaphore_create_init(&s_sem_scan, 0, 1))
        {
            return OPRT_OS_ADAPTER_ERRCODE_WIFI;
        }

        wifi_manager_reg_event_callback(WIFI_MGR_EVENT_STA_SCAN_COMPLETE, &wifi_scan_complete_cb);

        //2. start scan, wait scan complete
        wifi_get_sta_status(&sta_status);
        if (sta_status == WIFI_STA_STATUS_CONNECTED || \
            sta_status == WIFI_STA_STATUS_DISCONNECTING)
        {
            scan_cnt = CONNECTED_SCAN_TIMES;
        }
        LOG(LOG_LVL_INFO, "Scan cnt:%d; scan timeout:%d\r\n", scan_cnt, SCAN_TIMEOUT);

        for (; scan_cnt > 0; scan_cnt--)
        {
            wifi_sta_scan(&scan_cfg);
            tuya_hal_semaphore_wait(s_sem_scan);
        }

        //3. scan complete,output ap list.
        {
            ln_list_t *list;
            uint8_t node_count = 0;
            ap_info_node_t *pnode;
            AP_IF_S *ap_info = NULL;

            wifi_manager_ap_list_update_enable(LN_FALSE);
            wifi_manager_get_ap_list(&list, &node_count);

            LOG(LOG_LVL_INFO, "AP cnt:%d\r\n", node_count);

            if (node_count <= 0) {
                ret = OPRT_OS_ADAPTER_AP_NOT_FOUND;
                goto __finish;
            }

            LN_LIST_FOR_EACH_ENTRY(pnode, ap_info_node_t, list, list)
            {
                ap_info_t *info = &pnode->info;

                if (memcmp(ssid, info->ssid, strlen((const char *)ssid)) == 0)
                {
                    // malloc memory to store ap list,and free in tuya_hal_wifi_release_ap()
                    ap_info = tuya_os_adapt_system_malloc(sizeof(AP_IF_S) * node_count);
                    if(NULL == ap_info) {
                        ret = OPRT_OS_ADAPTER_MALLOC_FAILED;
                        goto __finish;
                    }

                    memset(ap_info, 0, (sizeof(AP_IF_S) * node_count));

                    ap_info->channel = info->channel;
                    ap_info->rssi    = info->rssi;
                    ap_info->s_len   = strlen((const char *)ssid);
                    memcpy(ap_info->bssid, info->bssid, BSSID_LEN);
                    memcpy(ap_info->ssid,  info->ssid,  ap_info->s_len);
                    ap_info->ssid[ap_info->s_len] = '\0';
                    goto __finish;
                }
            }

__finish:
            if (NULL == ap_info)
            {
                ret = OPRT_OS_ADAPTER_AP_NOT_FOUND;
            }
            *ap = ap_info;
            wifi_manager_ap_list_update_enable(LN_TRUE);
        }

        //4. delete sem, callback
        wifi_manager_reg_event_callback(WIFI_MGR_EVENT_STA_SCAN_COMPLETE, NULL);
        tuya_hal_semaphore_release(s_sem_scan);
        wifi_manager_cleanup_scan_results();
        s_sem_scan = NULL;
    }

    return ret;
}

/**
 * @brief release the memory malloced in <tuya_os_adapt_wifi_all_ap_scan>
 *        and <tuya_os_adapt_wifi_assign_ap_scan> if needed. tuya-sdk
 *        will call this function when the ap info is no use.
 *
 * @param[in]       ap          the ap info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_release_ap(AP_IF_S *ap)
{
    if(NULL == ap) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    tuya_os_adapt_system_free(ap);

    return OPRT_OS_ADAPTER_OK;
}


/**
 * @brief set wifi interface work channel
 *
 * @param[in]       chan        the channel to set
 * @return  OPRT_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_set_cur_channel(const unsigned char chan)
{
    COUNTRY_CODE_E ccode;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(0 == wifi_params->ty_wifi_started){
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start(WWM_STATION);
    }

    if(OPRT_OS_ADAPTER_OK != tuya_hal_wifi_get_country_code(&ccode)){
        return OPRT_OS_ADAPTER_CHAN_SET_FAILED;
    }

    if(((ccode == COUNTRY_CODE_CN || ccode == COUNTRY_CODE_EU) && (chan > 13 || chan == 0))
        ||((ccode == COUNTRY_CODE_US) && (chan > 11 || chan == 0))
        ||((ccode == COUNTRY_CODE_JP) && (chan > 14 || chan == 0))) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    wifi_set_channel((uint8_t)chan);
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief get wifi interface work channel
 *
 * @param[out]      chan        the channel wifi works
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_cur_channel(unsigned char *chan)
{
    uint8_t channel = 0;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(chan == NULL) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if(0 == wifi_params->ty_wifi_started){
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start(WWM_STATION);
    }

    wifi_get_channel(&channel);
    *chan = channel;
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief get wifi ip info.when wifi works in
 *        ap+station mode, wifi has two ips.
 *
 * @param[in]       wf          wifi function type
 * @param[out]      ip          the ip addr info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_ip(const WF_IF_E wf, NW_IP_S *ip)
{
    tcpip_ip_info_t ip_info = {0};
    wifi_mode_t mode = WIFI_MODE_STATION;
    netif_idx_t net_if_idx;

    if(ip == NULL) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    mode = wifi_current_mode_get();
    if ((WF_STATION == wf) && mode != WIFI_MODE_STATION) {
        return OPRT_OS_ADAPTER_COM_ERROR;
    } else if ((WF_AP == wf) && mode != WIFI_MODE_AP) {
        return OPRT_OS_ADAPTER_COM_ERROR;
    }

    net_if_idx = (wf == WF_STATION) ? NETIF_IDX_STA: NETIF_IDX_AP;

    if (NETDEV_LINK_UP != netdev_get_link_state(netdev_get_active())) {
        return OPRT_OS_ADAPTER_COM_ERROR;
    }

    if(!netif_is_link_up(netdev_get_netif(net_if_idx))){
        LOG(LOG_LVL_ERROR, "network is not up yet!\r\n");
        return OPRT_OS_ADAPTER_NOT_SUPPORTED;
    }

    netdev_get_ip_info(net_if_idx, &ip_info);

    inet_ntoa_r(ip_info.ip, ip->ip, sizeof(ip->ip));
    inet_ntoa_r(ip_info.gw, ip->gw, sizeof(ip->gw));
    inet_ntoa_r(ip_info.netmask, ip->mask, sizeof(ip->mask));
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief set wifi mac info.when wifi works in
 *        ap+station mode, wifi has two macs.
 *
 * @param[in]       wf          wifi function type
 * @param[in]       mac         the mac info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_set_mac(const WF_IF_E wf, const NW_MAC_S *mac)
{
    wifi_interface_t if_index;
    wifi_mode_t mode = WIFI_MODE_STATION;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(mac == NULL) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if(0 == wifi_params->ty_wifi_started){
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start((wf == WF_STATION) ? WWM_STATION : WWM_SOFTAP);
    }

    mode = wifi_current_mode_get();
    if ((WF_STATION == wf) && mode != WIFI_MODE_STATION) {
        LOG(LOG_LVL_ERROR, "Error at [%s:%d]\r\n", __func__, __LINE__);
        return OPRT_OS_ADAPTER_COM_ERROR;
    } else if ((WF_AP == wf) && mode != WIFI_MODE_AP) {
        LOG(LOG_LVL_ERROR, "Error at [%s:%d]\r\n", __func__, __LINE__);
        return OPRT_OS_ADAPTER_COM_ERROR;
    }

    if_index = (wf == WF_STATION) ? STATION_IF : SOFT_AP_IF;
    wifi_set_macaddr(if_index, (const uint8_t *)mac->mac);
    sysparam_sta_mac_update((const uint8_t *)mac->mac);
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief get wifi mac info.when wifi works in
 *        ap+station mode, wifi has two macs.
 *
 * @param[in]       wf          wifi function type
 * @param[out]      mac         the mac info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_mac(const WF_IF_E wf, NW_MAC_S *mac)
{
    wifi_interface_t if_index;
    wifi_mode_t mode = WIFI_MODE_STATION;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(mac == NULL) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if(0 == wifi_params->ty_wifi_started){
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start((wf == WF_STATION) ? WWM_STATION : WWM_SOFTAP);
    }

    mode = wifi_current_mode_get();
    if ((WF_STATION == wf) && mode != WIFI_MODE_STATION) {
        LOG(LOG_LVL_ERROR, "Error at [%s:%d]\r\n", __func__, __LINE__);
        return OPRT_OS_ADAPTER_COM_ERROR;
    } else if ((WF_AP == wf) && mode != WIFI_MODE_AP) {
        LOG(LOG_LVL_ERROR, "Error at [%s:%d]\r\n", __func__, __LINE__);
        return OPRT_OS_ADAPTER_COM_ERROR;
    }

    if_index = (wf == WF_STATION) ? STATION_IF : SOFT_AP_IF;

    LOG(LOG_LVL_INFO, "[%s:%d]\r\n", __func__, __LINE__);
    wifi_get_macaddr(if_index, mac->mac);
    LOG(LOG_LVL_INFO, "[%s:%d] if: %d, mac: %02x:%02x:%02x:%02x:%02x:%02x:\r\n",
        __func__, __LINE__, if_index,
        mac->mac[0], mac->mac[1], mac->mac[2], mac->mac[3], mac->mac[4], mac->mac[5]);

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief set wifi work mode
 *
 * @param[in]       mode        wifi work mode
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_set_work_mode(const WF_WK_MD_E mode)
{
    wifi_mode_t  ln_wifi_mode, curr_mode;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();
    wifi_params->mode = mode;

    switch(mode)
    {
        case WWM_STATION:
            ln_wifi_mode=WIFI_MODE_STATION;
            break;
        case WWM_SOFTAP:
            ln_wifi_mode=WIFI_MODE_AP;
            break;
        case WWM_LOWPOWER:
            // if(0 != wifi_station_set_powersave(WIFI_MAX_POWERSAVE)){
            //     return OPRT_OS_ADAPTER_WORKMODE_SET_FAILED;
            // }else{
                return OPRT_OS_ADAPTER_OK;
            // }
            #warning "TODO: WWM_LOWPOWER"
        case WWM_STATIONAP:
            // ln_wifi_mode = WIFI_MODE_AP_STATION;
            // break;
        case WWM_SNIFFER:
        default:
            ln_wifi_mode = WIFI_MODE_STATION;
            break;
    }

    if (wifi_params->ty_wifi_started == 0)
    {
        tuya_hal_wifi_start(mode);
    }

    curr_mode = wifi_current_mode_get();
    if (curr_mode == ln_wifi_mode)
    {
        LOG(LOG_LVL_INFO, "No need to switch mode, mode:%d\r\n", curr_mode);
        goto __ok;
    }

    wifi_stop();
    if (curr_mode == WIFI_MODE_STATION)
    {
        netdev_set_state(NETIF_IDX_STA, NETDEV_DOWN);
    }
    else
    {
        netdev_set_state(NETIF_IDX_AP, NETDEV_DOWN);
    }

    if (ln_wifi_mode == WIFI_MODE_STATION)
    {
        uint8_t mac_addr[MAC_ADDRESS_LEN];
        sysparam_sta_mac_get(mac_addr);
        netdev_set_mac_addr(NETIF_IDX_STA, mac_addr);
        netdev_set_active(NETIF_IDX_STA);
        wifi_sta_start(mac_addr, wifi_params->ps_mode);
    }
    else
    {
        uint8_t mac_addr[MAC_ADDRESS_LEN];
        sysparam_softap_mac_get(mac_addr);
        wifi_softap_cfg_t ap_cfg = {
            .ssid            = "temp",
            .pwd             = "",
            .bssid           = mac_addr,
            .ext_cfg = {
                .channel         = 6,
                .authmode        = WIFI_AUTH_WPA_WPA2_PSK,
                .ssid_hidden     = 1,
                .beacon_interval = 100,
                .psk_value = NULL,
            }
        };

        ap_cfg.ext_cfg.psk_value = NULL;
        netdev_set_active(NETIF_IDX_AP);
        wifi_softap_start(&ap_cfg);
    }

__ok:
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief get wifi work mode
 *
 * @param[out]      mode        wifi work mode
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_work_mode(WF_WK_MD_E *mode)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    *mode = wifi_params->mode;
    return OPRT_OS_ADAPTER_OK;
}


/**
 * @brief monitor packet callback function.When receive monitor packets,
 *  wifi mac will invoke this function
 * @param[in]     buff  point to receive packet memory
 *                len   received packet buffer length
 *                type  the type of received packet
 * @param[out]    pass the buffer pointer to the function defined outside
 * @return        none
 */
static void tuya_hal_wifi_sniffer_handler(void *buf, uint16_t len, wifi_pkt_type_t pkt_type, int8_t rssi_db)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();
    const uint8_t * ppkt = (uint8_t *)buf;

    UNUSED(pkt_type);

    // if (pkt_type == WIFI_PKT_TYPE_MGMT && wifi_params->mgnt_recv_cb) {
    //     // LOG(LOG_LVL_DEBUG, "[%s:%d] mgnt recv cb\r\n", __func__, __LINE__);

    //     // typedef void (*WIFI_REV_MGNT_CB)(UCHAR_T *buf, UINT_T len);
    //     wifi_params->mgnt_recv_cb(&ppkt[0], len);
    // }

    if (wifi_params->sniffer_cb) {
        // LOG(LOG_LVL_DEBUG, "[%s:%d] sniffer recv cb\r\n", __func__, __LINE__);
        wifi_params->sniffer_cb((const uint8_t *)&ppkt[0], (const uint16_t)len, (const int8_t)rssi_db);
    }
}

/**
 * @brief enable / disable wifi sniffer mode.
 *        if wifi sniffer mode is enabled, wifi recv from
 *        packages from the air, and user shoud send these
 *        packages to tuya-sdk with callback <cb>.
 *
 * @param[in]       en          enable or disable
 * @param[in]       cb          notify callback
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_sniffer_set(const bool en, const SNIFFER_CALLBACK cb)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if (en && (NULL == cb)) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (0 == wifi_params->ty_wifi_started) {
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start(WWM_STATION);
    }

    // LOG(LOG_LVL_ERROR, "[%s] mode:%d, en:%d\r\n", __func__, wifi_current_mode_get(), en);

    if (en) {
        sniffer_cfg_t sniffer_cfg = {
            .enable = 1,
            .type = 0,
            .filter_mask = WIFI_SNIFFER_FILTER_MASK_DATA
        };

        if (wifi_current_mode_get() != WIFI_MODE_STATION)
        {
            LOG(LOG_LVL_ERROR, "[%s:%d] work mode error\r\n", __func__, __LINE__);
            return OPRT_OS_ADAPTER_SNIFFER_SET_FAILED;
        }

        wifi_params->sniffer_cb = (SNIFFER_CALLBACK)cb;

        wifi_sta_set_powersave(WIFI_NO_POWERSAVE);
        // wifi_params->ps_mode = WIFI_NO_POWERSAVE;

        //Register promiscuous rx callback
        wifi_sta_set_sniffer_cfg(&sniffer_cfg, (wifi_sniffer_cb_t)tuya_hal_wifi_sniffer_handler);
    } else {
        sniffer_cfg_t sniffer_cfg = { 0 };

        if (0 != wifi_sta_set_sniffer_cfg(&sniffer_cfg, (wifi_sniffer_cb_t)tuya_hal_wifi_sniffer_handler))
        {
            LOG(LOG_LVL_INFO, "Stop sniffer mode failed!\r\n");
        }
        wifi_params->sniffer_cb = NULL;
        wifi_sta_set_powersave(TY_ADA_WIFI_PS_MODE);
        // wifi_params->ps_mode = TY_ADA_WIFI_PS_MODE;

        tuya_hal_wifi_start(WWM_STATION);
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief start a soft ap
 *
 * @param[in]       cfg         the soft ap config
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_ap_start(const WF_AP_CFG_IF_S *cfg)
{
    wifi_softap_cfg_t ap_cfg = {0};
    uint8_t psk_value[40]    = {0};
    NW_MAC_S mac;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if (NULL == cfg) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (0 == wifi_params->ty_wifi_started)
    {
        tuya_hal_wifi_start(WWM_SOFTAP);
    }

    tuya_hal_wifi_get_mac(WF_AP, &mac);

    ap_cfg.ext_cfg.channel = cfg->chan;
    ap_cfg.ext_cfg.authmode = (wifi_auth_mode_t)cfg->md;
    ap_cfg.ext_cfg.ssid_hidden = cfg->ssid_hidden;
    ap_cfg.ext_cfg.beacon_interval = (cfg->ms_interval == 0) ? 100 : cfg->ms_interval;

    ap_cfg.ssid  = (char *)cfg->ssid;
    ap_cfg.pwd   = (char *)cfg->passwd;
    ap_cfg.bssid = (uint8_t *)mac.mac;

    LOG(LOG_LVL_INFO, "ssid:%s; pwd:%s; channel:%d, ecn:%d, max conn:%d, hidden:%d" \
        "beacon:%d, mac:%02x:%02x:%02x:%02x:%02x:%02x; start:%d\r\n", 
        ap_cfg.ssid, ap_cfg.pwd, ap_cfg.ext_cfg.channel, 
        ap_cfg.ext_cfg.authmode, cfg->max_conn, ap_cfg.ext_cfg.ssid_hidden,
        ap_cfg.ext_cfg.beacon_interval, 
        mac.mac[0], mac.mac[1], mac.mac[2], mac.mac[3], mac.mac[4], mac.mac[5], wifi_params->ty_wifi_started);

    wifi_stop();
    netdev_set_state(NETIF_IDX_AP, NETDEV_DOWN);

    {
        tcpip_ip_info_t  ip_info = {0};
        server_config_t  server_config = {0};
        uint32_t dhcps_start = 0;
        uint32_t dhcps_end = 0;

        // sysparam_softap_ip_info_get(&ip_info);
        // sysparam_dhcpd_cfg_get(&server_config);

        ip_info.ip.addr      = ipaddr_addr((const char *)cfg->ip.ip);
        ip_info.gw.addr      = ipaddr_addr((const char *)cfg->ip.gw);
        ip_info.netmask.addr = ipaddr_addr((const char *)cfg->ip.mask);

        server_config.server.addr   = ip_info.ip.addr;
        server_config.port          = 67;
        server_config.lease         = 2880;
        server_config.renew         = 2880;
        server_config.client_max    = cfg->max_conn;

        dhcps_start = (uint32_t)server_config.server.addr;
        dhcps_end   = (((dhcps_start >> 24) + cfg->max_conn) << 24) | (dhcps_start & 0x00FFFFFF);
        dhcps_start = (((dhcps_start >> 24) + 1) << 24) | (dhcps_start & 0x00FFFFFF);

        server_config.ip_start.addr = dhcps_start;
        server_config.ip_end.addr   = dhcps_end;

        if (LN_FALSE != dhcpd_is_running())
        {
            /* dhcpd is running, stop it first */
            dhcpd_stop();
        }

        if (DHCPD_ERR_NONE != dhcpd_curr_config_set(&server_config))
        {
            LOG(LOG_LVL_INFO, "dhcpd has been running!\r\n");
        }

        LOG(LOG_LVL_INFO, "ip info: \r\n");
        LOG(LOG_LVL_INFO, "\tip:      %s\r\n", ip4addr_ntoa(&ip_info.ip));
        LOG(LOG_LVL_INFO, "\tgw:      %s\r\n", ip4addr_ntoa(&ip_info.gw));
        LOG(LOG_LVL_INFO, "\tnetmask: %s\r\n", ip4addr_ntoa(&ip_info.netmask));

        LOG(LOG_LVL_INFO, "server config:\r\n");
        LOG(LOG_LVL_INFO, "\tip:       %s\r\n", ip4addr_ntoa(&server_config.server));
        LOG(LOG_LVL_INFO, "\tip start: %s\r\n", ip4addr_ntoa(&server_config.ip_start));
        LOG(LOG_LVL_INFO, "\tip end:   %s\r\n", ip4addr_ntoa(&server_config.ip_end));

        LOG(LOG_LVL_INFO,
                "port:%d\r\n" \
                "lease:%d\r\n" \
                "renew:%d\r\n" \
                "max client:%d\r\n",
                server_config.port,
                server_config.lease, server_config.renew, server_config.client_max);

        netdev_set_mac_addr(NETIF_IDX_AP, ap_cfg.bssid);
        netdev_set_ip_info(NETIF_IDX_AP, &ip_info);
        netdev_set_active(NETIF_IDX_AP);
    }

    ap_cfg.ext_cfg.psk_value = NULL;
    if ((ap_cfg.pwd != NULL && strlen((const char *)ap_cfg.pwd) > 0) &&
        (ap_cfg.ext_cfg.authmode != WIFI_AUTH_OPEN) &&
        (ap_cfg.ext_cfg.authmode != WIFI_AUTH_WEP)) {
        memset(psk_value, 0, sizeof(psk_value));
        if (0 == ln_psk_calc((const char *)ap_cfg.ssid, (const char *)ap_cfg.pwd, (uint8_t *)psk_value, sizeof(psk_value))) {
            ap_cfg.ext_cfg.psk_value = psk_value;
            hexdump(LOG_LVL_DEBUG, "psk value ", psk_value, sizeof(psk_value));
        }
    }
    wifi_manager_reg_event_callback(WIFI_MGR_EVENT_SOFTAP_STARTUP, &ap_startup_cb);

    if (0 != wifi_softap_start(&ap_cfg))
    {
        LOG(LOG_LVL_ERROR, "[%s:%d] wifi_softap_start() failed!", __func__, __LINE__);
        goto __exit;
    }

    wifi_params->ty_wifi_started = 1;
    return OPRT_OS_ADAPTER_OK;

__exit:
    return OPRT_OS_ADAPTER_AP_START_FAILED;
}

/**
 * @brief stop a soft ap
 *
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_ap_stop(void)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(wifi_params->mode != WWM_SOFTAP || wifi_params->ty_wifi_started == 0) {
        LOG(LOG_LVL_ERROR, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        LOG(LOG_LVL_ERROR, "Error at [%s:%d]\r\n", __func__, __LINE__);
        return OPRT_OS_ADAPTER_AP_STOP_FAILED;
    }

    wifi_stop();
    netdev_set_state(NETIF_IDX_AP, NETDEV_DOWN);
    wifi_params->ty_wifi_started = 0;
    return OPRT_OS_ADAPTER_OK;
}


static int tuya_hal_wifi_psk_update(const tuya_hal_wifi_psk_info_t * psk_info)
{
    int ret = 0;
    ret = ln_kv_set(TUYA_PSK_KV_INFO, psk_info, sizeof(tuya_hal_wifi_psk_info_t));
    if (ret != KV_ERR_NONE)
    {
        LOG(LOG_LVL_ERROR, "kv set failed: %d\r\n", ret);
        return -1;
    }
    return 0;
}

static int tuya_hal_wifi_psk_get(tuya_hal_wifi_psk_info_t * psk_info)
{
    int ret = 0;
    size_t v_len = 0;
    if (psk_info == NULL)
    {
        return -1;
    }

    memset(psk_info, 0x0, sizeof(tuya_hal_wifi_psk_info_t));
    ret = ln_kv_get(TUYA_PSK_KV_INFO, psk_info, sizeof(tuya_hal_wifi_psk_info_t), &v_len);
    if (ret != KV_ERR_NONE)
    {
        LOG(LOG_LVL_ERROR, "kv get failed: %d\r\n", ret);
        return -1;
    }
    return 0;
}

/**
 * @brief : get ap info for fast connect
 * @param[out]      fast_ap_info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_connected_ap_info_v2(FAST_WF_CONNECTED_AP_INFO_V2_S **fast_ap_info)
{
    FAST_WF_CONNECTED_AP_INFO_V2_S *ap_infor_v2_buf = NULL;
    unsigned int len = sizeof(tuya_hal_wifi_psk_info_t);

    int ret = 0;

    if(NULL == fast_ap_info) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    ap_infor_v2_buf = (FAST_WF_CONNECTED_AP_INFO_V2_S *)tuya_os_adapt_system_malloc(sizeof(FAST_WF_CONNECTED_AP_INFO_V2_S) - 1 + len);
    if(NULL == ap_infor_v2_buf) {
        return OPRT_OS_ADAPTER_MALLOC_FAILED;
    }
    memset(ap_infor_v2_buf, 0, sizeof(FAST_WF_CONNECTED_AP_INFO_V2_S) - 1 + len);
    ap_infor_v2_buf->len = len;

    ret = tuya_hal_wifi_psk_get((tuya_hal_wifi_psk_info_t *)(ap_infor_v2_buf->data));
    if (ret != 0) {
        tuya_os_adapt_system_free(ap_infor_v2_buf);
        *fast_ap_info = NULL;
        return OPRT_OS_ADAPTER_APINFO_GET_FAILED;
    }

    *fast_ap_info = (void *)ap_infor_v2_buf;

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief : fast connect
 * @param[in]      fast_ap_info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_fast_station_connect_v2(const FAST_WF_CONNECTED_AP_INFO_V2_S *fast_ap_info)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();
    tuya_hal_wifi_psk_info_t *ap_psk_info = NULL;

    if(NULL == fast_ap_info) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    ap_psk_info = (tuya_hal_wifi_psk_info_t *)(fast_ap_info->data);

    if(NULL == ap_psk_info) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    LOG(LOG_LVL_INFO, "psk info ssid   :%s\r\n", ap_psk_info->ssid);
    LOG(LOG_LVL_INFO, "psk info pwd    :%s\r\n", ap_psk_info->pwd);
    LOG(LOG_LVL_INFO, "psk info pwd len:%d\r\n", ap_psk_info->pwd_len);

    if(0 == wifi_params->ty_wifi_started){
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start(WWM_STATION);
    }

    {
        uint8_t mac_addr[MAC_ADDRESS_LEN] = {0};
        wifi_sta_connect_t conn = {0};
        wifi_scan_cfg_t scan_cfg = {0};

        // wifi_sta_disconnect();
        wifi_stop();
        netdev_set_state(NETIF_IDX_STA, NETDEV_DOWN);

        wifi_get_macaddr((wifi_interface_t)NETIF_IDX_STA, mac_addr);
        netdev_set_mac_addr(NETIF_IDX_STA, mac_addr);
        netdev_set_active(NETIF_IDX_STA);
        wifi_sta_start(mac_addr, wifi_params->ps_mode);

        conn.ssid  = (char *)ap_psk_info->ssid;
        conn.pwd   = (char *)ap_psk_info->pwd;
        conn.bssid = NULL;
        conn.psk_value = NULL;
        if (ap_psk_info->pwd_len != 0)
        {
            conn.psk_value = ap_psk_info->psk;
        }

        if (SYSPARAM_ERR_NONE != sysparam_sta_scan_cfg_get(&scan_cfg))
        {
            goto __exit;
        }

        if (wifi_sta_connect(&conn, &scan_cfg) != 0)
        {
            LOG(LOG_LVL_ERROR, "Join failed! ssid:%s, pwd:%s\r\n",
                    conn.ssid, conn.pwd);
            goto __exit;
        }

        wifi_params->ty_wifi_started = 1;
    }

    return OPRT_OS_ADAPTER_OK;
__exit:
    LOG(LOG_LVL_ERROR, "[%s:%d] conn v2 failed!\r\n", __func__, __LINE__);
    return OPRT_OS_ADAPTER_NOT_SUPPORTED;
}

/**
 * @brief connect wifi with ssid and passwd
 *
 * @param[in]       ssid
 * @param[in]       passwd
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_station_connect(const signed char *ssid, const signed char *passwd)
{
    uint8_t mac_addr[MAC_ADDRESS_LEN] = {0};
    wifi_sta_connect_t conn           = {0};
    wifi_scan_cfg_t scan_cfg          = {0};
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();
    tuya_hal_wifi_psk_info_t psk_info = {0};
    uint8_t need_cal_psk = 0;

    LOG(LOG_LVL_DEBUG, "[%s:%d] ssid:%s pwd:%s\r\n", __func__, __LINE__, ssid, passwd);

    if(NULL == ssid) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (wifi_params->mode != WWM_STATION) {
        return OPRT_OS_ADAPTER_ERRCODE_WIFI;
    }

    if (wifi_params->ty_wifi_started == 0)
    {
        LOG(LOG_LVL_ERROR, "[%s:%d] wifi is not started!\r\n", __func__, __LINE__);
        tuya_hal_wifi_start(WWM_STATION);
    }
    else
    {
        // wifi_sta_disconnect();
        wifi_stop();
        netdev_set_state(NETIF_IDX_STA, NETDEV_DOWN);

        sysparam_sta_mac_get(mac_addr);
        netdev_set_mac_addr(NETIF_IDX_STA, mac_addr);
        netdev_set_active(NETIF_IDX_STA);
        wifi_sta_start(mac_addr, wifi_params->ps_mode);
    }

    conn.ssid  = (char *)ssid;
    conn.pwd   = (char *)((passwd != NULL) ? passwd : "");
    conn.bssid = NULL;
    conn.psk_value = NULL;

    if (SYSPARAM_ERR_NONE != sysparam_sta_scan_cfg_get(&scan_cfg))
    {
        goto __exit;
    }

    /**
     * Need to update psk regardless of whether there is a password.
     * Because the passwordless hotspot natively supports fast connection!
    */
    do {
        if (passwd == NULL) {
            break;
        }

        if (tuya_hal_wifi_psk_get(&psk_info) != 0) { 
            // Failed get psk saved in kv. Recalculate psk
            LOG(LOG_LVL_TRACE, "[%d] Need to calculate psk!\r\n", __LINE__);
            need_cal_psk = 1;
        } else {
            LOG(LOG_LVL_TRACE, "flash info: ssid:%s-%s, pwd:%s-%s,plen:%d \r\n",
                    psk_info.ssid, ssid, psk_info.pwd, passwd, psk_info.pwd_len);
            if ((strncmp(psk_info.ssid, (const char *)ssid,
                    strlen((const char *)ssid)) == 0) &&
                (psk_info.pwd_len != 0) &&
                (memcmp(psk_info.pwd, passwd, psk_info.pwd_len) == 0)) {
                // No need to calculate psk if the information is consistent
                conn.psk_value = psk_info.psk;
                LOG(LOG_LVL_TRACE, "[%d] Find psk!\r\n", __LINE__);
                break;
            } else {
                // Recalculate psk
                need_cal_psk = 1;
                LOG(LOG_LVL_TRACE, "[%d] Need to calculate psk!\r\n", __LINE__);
            }
        }

        if (need_cal_psk) {
            psk_info.pwd_len = strlen((const char *)passwd);
            strncpy(psk_info.pwd, passwd, sizeof(psk_info.pwd));
            strncpy((char *)psk_info.ssid, (const char *)ssid, sizeof(psk_info.ssid));

            LOG(LOG_LVL_TRACE, "[%d] calculate psk!\r\n", __LINE__);
            if (0 != ln_psk_calc(conn.ssid, conn.pwd, psk_info.psk, sizeof(psk_info.psk))) {
                break;
            }

            conn.psk_value = psk_info.psk;

            if (0 != tuya_hal_wifi_psk_update((const tuya_hal_wifi_psk_info_t *)&psk_info))
            {
                LOG(LOG_LVL_ERROR, "update psk info failed!\r\n");
                break;
            }
        }
    } while(0);

    if (wifi_sta_connect(&conn, &scan_cfg) != 0)
    {
        LOG(LOG_LVL_ERROR, "Join failed! ssid:%s, pwd:%s\r\n",
                conn.ssid, conn.pwd);
        goto __exit;
    }

    wifi_params->ty_wifi_started = 1;

    /**
     * Need to wait connect success, then exit this function! TODO
    */

    return OPRT_OS_ADAPTER_OK;

__exit:
    return OPRT_OS_ADAPTER_CONN_FAILED;
}

/**
 * @brief disconnect wifi from connect ap
 *
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_station_disconnect(void)
{
    if (wifi_sta_disconnect() != 0)
    {
        return OPRT_OS_ADAPTER_DISCONN_FAILED;
    }
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief get wifi connect rssi
 *
 * @param[out]      rssi        the return rssi
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_station_get_conn_ap_rssi(signed char *rssi)
{
    int8_t sta_rssi = 0;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if (NULL == rssi || wifi_params->mode != WWM_STATION) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (wifi_params->ty_wifi_started == 0) {
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    wifi_sta_get_rssi(&sta_rssi);
    *rssi = sta_rssi;
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief get wifi bssid
 * 
 * @param[out]      mac         uplink mac
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_bssid(unsigned char *mac)
{
    const char *ssid = NULL;
    const uint8_t *bssid = NULL;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if (NULL == mac) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (wifi_params->mode != WWM_STATION || wifi_params->ty_wifi_started == 0) {
        LOG(LOG_LVL_ERROR, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        return OPRT_COM_ERROR;
    }

    wifi_get_sta_conn_info(&ssid, &bssid);
    if (bssid != NULL) {
        LOG(LOG_LVL_DEBUG, "conn ap bssid: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
        memcpy(mac, bssid, MAC_ADDRESS_LEN);
    } else {
        return OPRT_COM_ERROR;
    }
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief get wifi station work status
 * 
 * @param[out]      stat        the wifi station work status
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_station_get_status(WF_STATION_STAT_E *stat)
{
    wifi_sta_status_t link_status = WIFI_STA_STATUS_STARTUP;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(NULL == stat || wifi_params->mode != WWM_STATION) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if(wifi_params->ty_wifi_started == 0)
    {
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start(WWM_STATION);
    }

    wifi_get_sta_status(&link_status);
    // LOG(LOG_LVL_DEBUG, "[%s:%d] link_status:%d; wifi status:%d\r\n", 
    //         __func__, __LINE__, link_status, wifi_params->ty_wifi_status);

    switch (link_status)
    {
        case WIFI_STA_STATUS_CONNECTING:
            *stat = WSS_CONNECTING;
            break;
        case WIFI_STA_STATUS_CONNECTED:
            *stat = WSS_CONN_SUCCESS;

            if (wifi_params->ty_wifi_status == WSS_GOT_IP) {
                *stat = WSS_GOT_IP;
            }
            break;
        case WIFI_STA_STATUS_DISCONNECTING:
        case WIFI_STA_STATUS_DISCONNECTED:
        case WIFI_STA_STATUS_STARTUP:
        case WIFI_STA_STATUS_SCANING:
        default:
            *stat = WSS_IDLE;
            break;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief set wifi country code
 *
 * @param[in]       ccode  country code
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_set_country_code(const COUNTRY_CODE_E ccode)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();
    wifi_country_code_t ctry_code;

    if (0 == wifi_params->ty_wifi_started) {
        LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
        tuya_hal_wifi_start(WWM_STATION);
    }

    wifi_params->ccode = ccode;

    if (ccode == COUNTRY_CODE_EU) {
        ctry_code = CTRY_CODE_CN;
    } else {
        ctry_code = (wifi_country_code_t)ccode;
    }

    if (0 != wifi_set_country_code(ctry_code)) {
        return OPRT_OS_ADAPTER_CCODE_SET_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

static int tuya_hal_wifi_get_country_code(COUNTRY_CODE_E *ccode)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(NULL == ccode) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    *ccode = wifi_params->ccode;

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief set wifi lowpower mode
 *
 * @param[in]       en    enable or disable
 * @param[in]      dtim
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_set_wifi_lp_mode(const bool en, const unsigned char dtim)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    // low power mode only in WWM_STATION
    if(wifi_params->mode != WWM_STATION || wifi_params->ty_wifi_started == 0){
        return OPRT_OS_ADAPTER_ERRCODE_WIFI;
    }

    if(en && ((dtim == 0) || (dtim > 10))) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (en == 1) {
#if defined(TY_BT_MOD) && (TY_BT_MOD == 1)
        // stop ble adv
        // suspend ble main task
        {
            extern int tuya_os_adapt_bt_stop_adv(void);
            tuya_os_adapt_bt_stop_adv();
        }

        // disable ble clock
        {
            soc_module_clk_gate_disable(CLK_G_BLE);
        }
#endif

        // set cpu_wifi_mode_dr as 1
        {
            #undef     RF_P0_BASE
            #define    RF_P0_BASE                 0x40012000
            #undef     CPU_WIFI_MODE_DR_BIT
            #define    CPU_WIFI_MODE_DR_BIT       (1 << 31)
            uint32_t old = 0;
            volatile uint32_t *rf_addr = (uint32_t *)(RF_P0_BASE + 4 * 0x7b);
            old = *rf_addr;
            *rf_addr = old | (CPU_WIFI_MODE_DR_BIT);
        }
        wifi_sta_set_powersave(TY_ADA_WIFI_PS_MODE);
        wifi_params->ps_mode = TY_ADA_WIFI_PS_MODE;
        ln_pm_sleep_mode_set(LIGHT_SLEEP);
    } else {
        ln_pm_sleep_mode_set(ACTIVE);

        // set cpu_wifi_mode_dr as 0
        {
            #define    RF_P0_BASE                 0x40012000
            #define    CPU_WIFI_MODE_DR_BIT       (1 << 31)
            uint32_t old = 0;
            volatile uint32_t *rf_addr = (uint32_t *)(RF_P0_BASE + 4 * 0x7b);
            old = *rf_addr;
            *rf_addr = old & (~(CPU_WIFI_MODE_DR_BIT));
        }

#if defined(TY_BT_MOD) && (TY_BT_MOD == 1)
        {
            soc_module_clk_gate_enable(CLK_G_BLE);
        }
#endif

        wifi_sta_set_powersave(WIFI_NO_POWERSAVE);
        wifi_params->ps_mode = WIFI_NO_POWERSAVE;

#if defined(TY_BT_MOD) && (TY_BT_MOD == 1)
        extern int tuya_os_adapt_bt_start_adv(void);
        tuya_os_adapt_bt_start_adv();
#endif
    }

    LOG(LOG_LVL_TRACE, "[%s:%d] en:%d\r\n", __func__, __LINE__, en);
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief send wifi management
 *
 * @param[in]       buf         pointer to buffer
 * @param[in]       len         length of buffer
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_send_mgnt(const unsigned char *buf, const unsigned int len)
{
    int ret = 0;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if ((NULL == buf) || (len == 0) || wifi_params->ty_wifi_started == 0) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    ret = wifi_send_80211_mgmt_raw_frame((unsigned char *)buf, len + 4); // FCS_LEN: 4
    // LOG(LOG_LVL_ERROR, "[%s] wifi mode:%d, len:%d, ret:%d\r\n", __func__, wifi_params->mode, len, ret);
    // hexdump(LOG_LVL_ERROR, "wifi send mgnt", buf, len);

    return OPRT_OS_ADAPTER_OK;
}

static void ty_wifi_recv_mgnt_cb(UCHAR_T *buf, UINT_T len)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    // LOG(LOG_LVL_ERROR, "[%s] len:%d\r\n", __func__, len);
    // hexdump(LOG_LVL_ERROR, "wifi recv mgnt", buf, len);
    if (wifi_params->mgnt_recv_cb) {
        wifi_params->mgnt_recv_cb(buf, len);
    }
}

/**
 * @brief register receive wifi management callback
 *
 * @param[in]       enable
 * @param[in]       recv_cb     receive callback
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_register_recv_mgnt_callback(const bool enable, const WIFI_REV_MGNT_CB recv_cb)
{
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();

    if(enable && (NULL == recv_cb)) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    // LOG(LOG_LVL_ERROR, "[%s] mode:%d, en:%d\r\n", __func__, wifi_current_mode_get(), enable);

    if (enable) {
        sniffer_cfg_t sniffer_cfg = {
            .enable = 1,
            .type = 1,
            .filter_mask = WIFI_SNIFFER_FILTER_MASK_MGMT
        };

        if(wifi_params->ty_wifi_started == 0) {
            LOG(LOG_LVL_DEBUG, "[%s:%d] wifi is not started\r\n", __func__, __LINE__);
            tuya_hal_wifi_start(WWM_STATION);
        } else if (wifi_current_mode_get() != WIFI_MODE_STATION) {
            LOG(LOG_LVL_ERROR, "[%s:%d] work mode error\r\n", __func__, __LINE__);
            return OPRT_OS_ADAPTER_SNIFFER_SET_FAILED;
        }

        if((wifi_params->mode == WWM_LOWPOWER) || (wifi_params->mode == WWM_SNIFFER)) {
            return OPRT_OS_ADAPTER_COM_ERROR;
        }

        wifi_sta_set_powersave(WIFI_NO_POWERSAVE);

        //Register promiscuous rx callback
        wifi_sta_set_sniffer_cfg(&sniffer_cfg, (wifi_sniffer_cb_t)tuya_hal_wifi_sniffer_handler);
        wifi_params->mgnt_recv_cb = recv_cb;
        ln_wifi_mgnt_cb_set((wifi_mgnt_cb_t)ty_wifi_recv_mgnt_cb);
    } else {
        sniffer_cfg_t sniffer_cfg = { 0 };

        if (0 != wifi_sta_set_sniffer_cfg(&sniffer_cfg, (wifi_sniffer_cb_t)tuya_hal_wifi_sniffer_handler))
        {
            LOG(LOG_LVL_INFO, "Stop sniffer mode failed!\r\n");
        }

        wifi_params->mgnt_recv_cb = NULL;
        ln_wifi_mgnt_cb_set(NULL);

        wifi_sta_set_powersave(TY_ADA_WIFI_PS_MODE);
    }

    return OPRT_OS_ADAPTER_OK;
}

static void ap_startup_cb(void *arg)
{
    UNUSED(arg);
    netdev_set_state(NETIF_IDX_AP, NETDEV_UP);
}

bool tuya_os_adapt_wifi_rf_calibrated(void)
{
    uint8_t val = 0;

    if (NVDS_ERR_OK == ln_nvds_get_ate_result(&val)) {
        if (val == NV9_ATE_RESULT_OK) {
            return true;
        } else if (val == NV9_ATE_RESULT_FAILED){
            return false;
        } else {
            LOG(LOG_LVL_ERROR, "[%s:%d] ATE test not done!\r\n", __func__, __LINE__);
            return false;
        }
    } else {
        LOG(LOG_LVL_ERROR, "[%s:%d] get ATE result failed!\r\n", __func__, __LINE__);
        return false;    
    }
}

static void wifi_temp_calibrate_timer_cb(void *arg)
{
    uint16_t adc_val = 0;
    int16_t curr_adc = 0;
    LN_UNUSED(arg);

    adc_val = drv_adc_read(ADC_CH0);
    wifi_do_temp_cal_period(adc_val);

    curr_adc = (adc_val & 0xFFF);
    static volatile uint8_t cnt = 0;
    cnt++;
    if ((cnt % 60) == 0) {
        LOG(LOG_LVL_INFO, "adc raw: %4d, temp_IC: %4d\r\n",
                curr_adc, (int16_t)(25 + (curr_adc - 770) / 2.54f));
    }
}

/**
 * @brief tuya_hal_wifi_start
 *
 * @param[in]       mode        wifi work mode
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
static int tuya_hal_wifi_start(WF_WK_MD_E mode)
{
    wifi_mode_t  ln_wifi_mode;
    tuya_hal_wifi_params_t *wifi_params = WIFI_PARAMS_GET();
    wifi_params->mode = mode;
    static uint8_t is_wifi_hw_ready = 0;

    if (is_wifi_hw_ready == 0) {
        // rf preprocess,img cal
        wifi_rf_preprocess();    // time consuming 80ms
        wifi_rf_image_cal();     // time consuming 156ms

        // init wifi
        wifi_init();
        wifi_manager_init();     // Creat wifi manager task

        // Start wifi temperature calibrate
        {
            int8_t cap_comp = 0;

            if (NVDS_ERR_OK == ln_nvds_get_xtal_comp_val((uint8_t *)&cap_comp)) {
                if ((uint8_t)cap_comp == 0xFF) {
                    cap_comp = 0;
                }
            }

            drv_adc_init();
            wifi_temp_cal_init(drv_adc_read(ADC_CH0), cap_comp);

            if (OS_OK != OS_TimerCreate(&s_wifi_temp_cal_timer, OS_TIMER_PERIODIC,
                    wifi_temp_calibrate_timer_cb, NULL, 1000)) {
                LOG(LOG_LVL_ERROR, "create wifi temp calibrate timer failed.\r\n");
            } else {
                OS_TimerStart(&s_wifi_temp_cal_timer);
            }
        }
        is_wifi_hw_ready = 1;
    }

    switch(mode)
    {
        case WWM_STATION:
            ln_wifi_mode=WIFI_MODE_STATION;
            break;
        case WWM_SOFTAP:
            ln_wifi_mode=WIFI_MODE_AP;
            break;
        case WWM_LOWPOWER:
            // if(0 != wifi_station_set_powersave(WIFI_MAX_POWERSAVE)){
            //     return OPRT_OS_ADAPTER_WORKMODE_SET_FAILED;
            // }else{
                return OPRT_OS_ADAPTER_OK;
            // }
            #warning "TODO: WWM_LOWPOWER"
        case WWM_STATIONAP:
            // ln_wifi_mode = WIFI_MODE_AP_STATION;
            // break;
        case WWM_SNIFFER:
        default:
            ln_wifi_mode = WIFI_MODE_STATION;
            break;
    }

    if (0 != wifi_params->ty_wifi_started) {
        goto __ok;
    }

    wifi_stop();

    if (ln_wifi_mode == WIFI_MODE_STATION)
    {
        uint8_t mac_addr[MAC_ADDRESS_LEN];
        sysparam_sta_mac_get(mac_addr);
        netdev_set_state(NETIF_IDX_STA, NETDEV_DOWN);
        netdev_set_mac_addr(NETIF_IDX_STA, mac_addr);
        netdev_set_active(NETIF_IDX_STA);

        wifi_manager_reg_event_callback(WIFI_MGR_EVENT_STA_DISCONNECTED, wifi_disconnect_cb);
        netdev_get_ip_cb_set(tuya_wifi_get_ip_cb);
        wifi_sta_start(mac_addr, wifi_params->ps_mode);
    }
    else
    {
        uint8_t mac_addr[MAC_ADDRESS_LEN];
        sysparam_softap_mac_get(mac_addr);
        wifi_softap_cfg_t ap_cfg = {
            .ssid            = "temp",
            .pwd             = "",
            .bssid           = mac_addr,
            .ext_cfg = {
                .channel         = 6,
                .authmode        = WIFI_AUTH_WPA_WPA2_PSK,
                .ssid_hidden     = 1,
                .beacon_interval = 100,
                .psk_value = NULL,
            }
        };

        ap_cfg.ext_cfg.psk_value = NULL;

        netdev_set_state(NETIF_IDX_AP, NETDEV_DOWN);
        netdev_set_active(NETIF_IDX_AP);
        wifi_manager_reg_event_callback(WIFI_MGR_EVENT_SOFTAP_STARTUP, &ap_startup_cb);
        wifi_softap_start(&ap_cfg);
    }

    wifi_params->ty_wifi_started = 1;

__ok:
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_reg_wifi_intf 接口注册
 * @return int 
 */
int tuya_os_adapt_reg_wifi_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_WIFI, (void *)&m_tuya_os_wifi_intfs);
}
