/**
 * @file tuya_os_adapt_bt.c
 * @brief bt操作接口
 * 
 * @copyright Copyright (c) {2018-2020} 涂鸦科技 www.tuya.com
 * 
 */
#include "tuya_os_adapt_bt.h"
#include "tuya_os_adapter.h"
#include "tuya_os_adapter_error_code.h"

#include "proj_config.h"

#include "utils/debug/log.h"

#include "rwip_config.h"
#include "co_error.h"
#include "llm_int.h"
#include "ln_app_gatt.h"
#include "gapm_task.h"
#include "ln_app_gatt.h"
#include "ln_app_gap.h"
#include "ble_arch/arch.h"

#include "osal/osal.h"

#include "ty_ada_export_api.h"


enum trans_data_svc{
    DATA_TRANS_DECL_CHAR_RX = 0,
    DATA_TRANS_CHAR_VAL_RX,
    DATA_TRANS_DECL_CHAR_TX,
    DATA_TRANS_CHAR_VAL_TX,
    DATA_TRANS_CLIENT_CHAR_CFG_TX,
};

#if defined(TY_BT_MOD) && (TY_BT_MOD == 1)

#define TY_BT_VER10_PROTOCOL_TYPE           0xff
#define TY_BT_VER11_PROTOCOL_TYPE_IDENTIFY  0x01
#define TY_BT_VER11_PROTOCOL_TYPE_SER_UUID  0x02
#define TY_BT_VER11_PROTOCOL_TYPE_SER_DATA  0x16


uint8_t g_adv_actv_idx  = 0;
uint8_t g_scan_actv_idx = 0;

volatile uint8_t g_adv_is_started = 0;
volatile uint8_t g_conidx = GAP_INVALID_CONIDX;
uint16_t svc_data_transfer_hdl = 0;

static volatile uint8_t g_ble_is_run = 0;
volatile uint8_t g_bt_rssi_is_update = 0;
volatile uint8_t g_bt_rssi = 0;

TY_BT_MSG_CB ty_bt_msg_cb = NULL;
TY_BT_SCAN_ADV_CB ty_bt_scan_adv_cb = NULL;
static uint8_t g_svc_uuid[2]={0x10, 0x19};

static OS_Mutex_t s_ble_sync_api_mutex = { 0 };
static OS_Semaphore_t s_ble_sync_api_sem = { 0 };
static OS_Semaphore_t g_ty_ada_bt_ready_sem = { 0 };
static OS_Semaphore_t sg_wait_rssi_sem = { 0 };

static volatile ty_bt_scan_info_t ty_target_scan_info = { 0 };
static volatile uint8_t g_bt_scan_found = 0;

static const TUYA_OS_BT_INTF m_tuya_os_bt_intfs = {
    .port_init      = tuya_os_adapt_bt_port_init,
    .port_deinit    = tuya_os_adapt_bt_port_deinit,
    .gap_disconnect = tuya_os_adapt_bt_gap_disconnect,
    .send           = tuya_os_adapt_bt_send,
    .reset_adv      = tuya_os_adapt_bt_reset_adv,
    .get_rssi       = tuya_os_adapt_bt_get_rssi,
    .start_adv      = tuya_os_adapt_bt_start_adv,
    .stop_adv       = tuya_os_adapt_bt_stop_adv,
    .assign_scan    = tuya_os_adapt_bt_assign_scan,
    .scan_init      = tuya_os_adapt_bt_scan_init,
    .start_scan     = tuya_os_adapt_bt_start_scan,
    .stop_scan      = tuya_os_adapt_bt_stop_scan,
};


static void ln_ble_advertising_create(void)
{
#define APP_ADV_CHMAP                (0x07)  // Advertising channel map - 37, 38, 39
#define APP_ADV_INT_MIN              (160)   // Advertising minimum interval - 100ms (N*0.625ms)
#define APP_ADV_INT_MAX              (160)   // Advertising maximum interval - 100ms (N*0.625ms)

    struct ln_gapm_activity_create_adv_cmd  adv_creat_param = {0};

    adv_creat_param.own_addr_type                     = GAPM_STATIC_ADDR;
    adv_creat_param.adv_param.type                    = GAPM_ADV_TYPE_LEGACY;//GAPM_ADV_TYPE_EXTENDED;//GAPM_ADV_TYPE_LEGACY;
    adv_creat_param.adv_param.filter_pol              = ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_creat_param.adv_param.prim_cfg.chnl_map       = APP_ADV_CHMAP;
    adv_creat_param.adv_param.prim_cfg.phy            = GAP_PHY_1MBPS;
    adv_creat_param.adv_param.prop                    = GAPM_ADV_PROP_UNDIR_CONN_MASK;//GAPM_ADV_PROP_NON_CONN_SCAN_MASK;//GAPM_ADV_PROP_UNDIR_CONN_MASK;//GAPM_ADV_PROP_UNDIR_CONN_MASK;//GAPM_EXT_ADV_PROP_UNDIR_CONN_MASK;//GAPM_ADV_PROP_UNDIR_CONN_MASK;
    adv_creat_param.adv_param.disc_mode               = GAPM_ADV_MODE_GEN_DISC;
    adv_creat_param.adv_param.prim_cfg.adv_intv_min   = APP_ADV_INT_MIN;
    adv_creat_param.adv_param.prim_cfg.adv_intv_max   = APP_ADV_INT_MAX;
    adv_creat_param.adv_param.max_tx_pwr              = 0;
    //adv_creat_param.adv_param.second_cfg.phy        = GAP_PHY_1MBPS;//GAP_PHY_1MBPS;//GAP_PHY_CODED;
    adv_creat_param.adv_param.second_cfg.max_skip     = 0x00;
    adv_creat_param.adv_param.second_cfg.phy          = 0x01;
    adv_creat_param.adv_param.second_cfg.adv_sid      = 0x00;
    adv_creat_param.adv_param.period_cfg.adv_intv_min = 0x0400;
    adv_creat_param.adv_param.period_cfg.adv_intv_max = 0x0400;
    ln_app_advertise_creat(&adv_creat_param);
}

static void ln_ble_adv_data_set(const char *name)
{
#define DEVICE_NAME                  ("TY")
#define DEVICE_NAME_LEN              (sizeof(DEVICE_NAME))
#define ADV_DATA_MAX_LENGTH          (28)

    uint8_t adv_data[ADV_DATA_MAX_LENGTH] = {0};
    struct ln_gapm_set_adv_data_cmd adv_data_param;

    adv_data[0] = DEVICE_NAME_LEN + 1;
    adv_data[1] = 0x09;  //adv type :local name
    memcpy(&adv_data[2], DEVICE_NAME, DEVICE_NAME_LEN);

    adv_data_param.actv_idx = g_adv_actv_idx;
    adv_data_param.length   = sizeof(adv_data);
    adv_data_param.data     = adv_data;
    ln_app_set_adv_data(&adv_data_param);
}

void ln_ble_adv_start(void)
{
    struct ln_gapm_activity_start_cmd  adv_start_param;
    adv_start_param.actv_idx = g_adv_actv_idx;
    adv_start_param.u_param.adv_add_param.duration = 0;
    adv_start_param.u_param.adv_add_param.max_adv_evt = 0;
    ln_app_advertise_start(&adv_start_param);
}

static void ln_ble_adv_init(const char *name)
{
    ln_ble_advertising_create();
    ln_ble_adv_data_set(name);
    ln_ble_adv_start();
}

static const struct gattm_att_desc g_gap_service_atts[] = {
    [DATA_TRANS_DECL_CHAR_RX] = {
        .uuid = { 0x03, 0x28 }, 
        .perm = PERM_MASK_RD,
        .max_len = 0,
        .ext_perm = (0 <<  PERM_POS_UUID_LEN),
    },
    [DATA_TRANS_CHAR_VAL_RX] = {
        .uuid = {0x11, 0x2b },  //0x2b11
        .perm = PERM_MASK_WRITE_COMMAND,
        .max_len = BLE_MAX_CHAR_LEN,
        .ext_perm = (0 <<PERM_POS_UUID_LEN),
    },

    [DATA_TRANS_DECL_CHAR_TX] = {
        .uuid = { 0x03, 0x28 }, 
        .perm = PERM_MASK_RD,
        .max_len = 0,
        .ext_perm =  (0 << PERM_POS_UUID_LEN),
    },
    [DATA_TRANS_CHAR_VAL_TX] = {
        .uuid = {0x10, 0x2b},   //0x2b10
        .perm = PERM_MASK_NTF,
        .max_len = BLE_MAX_CHAR_LEN,
        .ext_perm =  (0 << PERM_POS_UUID_LEN),
    },
    [DATA_TRANS_CLIENT_CHAR_CFG_TX] = {
        .uuid = { 0x02, 0x29 }, //INB_ATT_DESC_SERVER_CHAR_CFG,
        .perm = PERM_MASK_WRITE_REQ|PERM_MASK_RD,
        .max_len = 0,
        .ext_perm = 0,
    },
};

static void ln_ble_gatt_basic_service_add(void)
{
    int nb_att = sizeof(g_gap_service_atts)/sizeof(g_gap_service_atts[0]);
    struct ln_gattm_add_svc_req p_svc_desc;

    LOG(LOG_LVL_INFO, "[%s:%d]\r\n", __func__, __LINE__);

    /// Service Definition
    p_svc_desc.svc_desc.start_hdl = 0;
    p_svc_desc.svc_desc.perm = 0;
    memcpy(p_svc_desc.svc_desc.uuid, g_svc_uuid, 2);
    p_svc_desc.svc_desc.nb_att = nb_att;
    p_svc_desc.svc_desc.atts = g_gap_service_atts;

    ln_app_gatt_add_svc(&p_svc_desc);
}


static void create_scanning(void)
{
    struct ln_gapm_activity_create_cmd scan_creat_param = { 0 };
    scan_creat_param.own_addr_type = GAPM_STATIC_ADDR;
    ln_app_scan_creat(&scan_creat_param);
}

static void start_scanning(void)
{
    struct ln_gapm_activity_start_cmd  scan_start_param;

    memset(&scan_start_param, 0, sizeof(struct ln_gapm_activity_start_cmd));

    scan_start_param.actv_idx                                      = g_scan_actv_idx;
    scan_start_param.u_param.scan_param.type                       = GAPM_SCAN_TYPE_OBSERVER;
    scan_start_param.u_param.scan_param.prop                       = GAPM_SCAN_PROP_ACTIVE_1M_BIT|GAPM_SCAN_PROP_PHY_1M_BIT;
    scan_start_param.u_param.scan_param.dup_filt_pol               = 1;  //0;
    scan_start_param.u_param.scan_param.scan_param_1m.scan_intv    = 0xa0; //20*0.625 ms
    scan_start_param.u_param.scan_param.scan_param_1m.scan_wd      = 0x50; //15*0.625 ms
    scan_start_param.u_param.scan_param.duration                   = 0;  
    scan_start_param.u_param.scan_param.scan_param_coded.scan_intv = 0xa0;
    scan_start_param.u_param.scan_param.scan_param_coded.scan_wd   = 0x50;
    scan_start_param.u_param.scan_param.period                     = 2;
    ln_app_scan_start(&scan_start_param);
}

static void app_start_scan(void)
{
    create_scanning();
    start_scanning();
}

/**
 * Must be called before the scheduler starts
 * 
 * @return 0 on success, otherwise error
*/
int ty_ada_bt_init(void)
{
    if (OS_SemaphoreCreate(&g_ty_ada_bt_ready_sem, 0, 1) != OS_OK) {
        return -1;
    }
    return 0;
}

int ty_ada_bt_deinit(void)
{
    if (OS_SemaphoreIsValid(&g_ty_ada_bt_ready_sem)) {
        OS_SemaphoreDelete(&g_ty_ada_bt_ready_sem);
        g_ty_ada_bt_ready_sem.handle = NULL;
    }
    return 0;
}

/**
 * Wait until the bt hardware initialization is complete
 * 
 * @return 0 on success, otherwise error
*/
static int ty_ada_bt_wait_hw_ready(void)
{
    if (OS_SemaphoreWait(&g_ty_ada_bt_ready_sem, OS_WAIT_FOREVER) != OS_OK) {
        return -1;
    }
    return 0;
}

/**
 * Notify the waiter that the bt hardware is ready
 * 
 * @return 0 on success, otherwise error
*/
int ty_ada_bt_hw_ready_notify(void)
{
    if (OS_SemaphoreRelease(&g_ty_ada_bt_ready_sem) != OS_OK) {
        return -1;
    }
    return 0;
}

static int ln_ble_ada_start(void)
{
    /**
     * init ble hw, start ble main task
    */
    {
        uint8_t mac[6] ={0};
        ln_generate_random_mac(mac);
        mac[5] |= 0xC0; // This address is random generated, so assign 0x11 => Static Random Address

        rw_init(mac);
    }

    ty_ada_bt_init();
    ble_app_init();

    if (OS_OK != OS_SemaphoreCreate(&sg_wait_rssi_sem, 0, 1)) {
        return OPRT_OS_ADAPTER_COM_ERROR;
    }

    /* 1. init adv service */
    ln_ble_adv_init(NULL);

    /* 2. add basic service */
    ln_ble_gatt_basic_service_add();

    ty_ada_bt_wait_hw_ready();

    return OPRT_OS_ADAPTER_OK;
}

static int ln_ble_ada_stop(void)
{
    rw_deinit();
    if (OS_SemaphoreIsValid(&sg_wait_rssi_sem)) {
        OS_SemaphoreDelete(&sg_wait_rssi_sem);
        sg_wait_rssi_sem.handle = NULL;
    }

    ty_ada_bt_deinit();
}

/**
 * @brief tuya_os_adapt_bt 蓝牙初始化
 * @return int 
 */
 int tuya_os_adapt_bt_port_init(ty_bt_param_t *p)
{
    uint8_t i=0;
    LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);

    if((NULL == p)  || (NULL == p->cb)) 
    {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    LOG(LOG_LVL_TRACE, "[%s:%d] link num:%d, mode:%d, name:%s\r\n", 
        __func__, __LINE__,
        p->link_num, p->mode, p->name);

    if (g_ble_is_run == 0) {
        ty_bt_msg_cb = p->cb;
        ln_ble_ada_start();

        if(ty_bt_msg_cb != NULL) {
            ty_bt_msg_cb(0, TY_BT_EVENT_ADV_READY ,NULL);
        }
        g_ble_is_run = 1;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_bt 蓝牙断开关闭
 * @return int 
 */
int tuya_os_adapt_bt_port_deinit(void)
{
    LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);

    if (g_ble_is_run != 0) {
        ty_bt_msg_cb = NULL;

        if (g_conidx != GAP_INVALID_CONIDX) {
            // 1. disconnect with ble master when ble is connected
            ln_app_disconnect(g_conidx, CO_ERROR_REMOTE_USER_TERM_CON);
        }

        // 2. stop adv
        // 3. stop scan
        // 4. stop others activities
        // 5. deinit ble hw
        ln_ble_ada_stop();
    }

    return OPRT_OS_ADAPTER_OK;
}


/**
 * @brief tuya_os_adapt_bt 蓝牙断开
 * @return int 
 */
int tuya_os_adapt_bt_gap_disconnect(void)
{
    LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);

    if (g_conidx != GAP_INVALID_CONIDX) {
        ln_app_disconnect(g_conidx, CO_ERROR_REMOTE_USER_TERM_CON);
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_bt 蓝牙发送
 * @return int 
 */
int tuya_os_adapt_bt_send(const unsigned char *data, const unsigned char len)
{
    int ret = 0;
    uint16_t ntf_handle = 0;
    struct ln_gattc_send_evt_cmd send_data;

    LOG(LOG_LVL_TRACE, "[%s:%d] len:%d\r\n", __func__, __LINE__, len);

    if (0 == len || NULL == data) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    hexdump(LOG_LVL_TRACE, "send data:", data, len);

    send_data.handle = svc_data_transfer_hdl + DATA_TRANS_CHAR_VAL_TX + 1;
    send_data.length = len;
    send_data.value  = data;

    LOG(LOG_LVL_TRACE, "[%s:%d] hd:%d - %d\r\n", __func__, __LINE__, send_data.handle, svc_data_transfer_hdl);
    ln_app_gatt_send_ntf(g_conidx, &send_data);

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_bt 广播包重置
 * @return int 
 */
// ADV data::
// 00000000:  02 01 06 03 02 01 A2 14  16 01 A2 00 78 72 6C 6C  ............xrll
// 00000010:  77 36 36 7A 66 61 74 63  73 71 6D 72              w66zfatcsqmr

// Scan Rsp data::
// 00000000:  03 09 54 59 19 FF D0 07  01 03 00 00 0C 00 9E C0  ..TY............
// 00000010:  65 4C D3 74 01 D9 86 ED  8E 2E FF AC 71 AF        eL.t........q.

int tuya_os_adapt_bt_reset_adv(tuya_ble_data_buf_t *adv, tuya_ble_data_buf_t *scan_resp)
{
    if (g_adv_is_started != 0) {
        ln_app_activity_stop(g_adv_actv_idx);

        // while(g_adv_is_started != 0)
        // {
        //     OS_MsDelay(5);
        // }
    }

    LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);

    // set adv data
    {
        struct ln_gapm_set_adv_data_cmd adv_data_param;
        adv_data_param.actv_idx = g_adv_actv_idx;
        adv_data_param.length   = adv->len - 3;
        adv_data_param.data     = &adv->data[3];
        ln_app_set_adv_data(&adv_data_param);
    }

    // set scan resp data
    {
        struct ln_gapm_set_adv_data_cmd scan_rsp_data;
        scan_rsp_data.actv_idx = g_adv_actv_idx;
        scan_rsp_data.length   = scan_resp->len;
        scan_rsp_data.data     = &scan_resp->data[0];
        ln_app_set_scan_rsp_data(&scan_rsp_data);
    }

    // start adv
    ln_ble_adv_start();

    return OPRT_OS_ADAPTER_OK; 
}

void ty_ada_get_rssi_cb(int8_t rssi)
{
    if (g_ble_is_run == 0) {
        return;
    }

    g_bt_rssi = rssi;
    OS_SemaphoreRelease(&sg_wait_rssi_sem);
}

/**
 * @brief tuya_os_adapt_bt 获取rssi信号值
 * @return int 
 */
int tuya_os_adapt_bt_get_rssi(signed char *rssi)
{
    static uint8_t s_has_entry = 0;
    if ((g_ble_is_run == 0) || (g_conidx == GAP_INVALID_CONIDX)) {
        LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);
        return OPRT_OS_ADAPTER_COM_ERROR;
    }

    if (s_has_entry == 0) {
        s_has_entry = 1;
    } else {
        return OPRT_OS_ADAPTER_COM_ERROR;
    }

    LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);

    if (rssi) {
        struct ln_gapc_get_info_cmd peer_info = {
            .operation = GET_CON_RSSI
        };

        ln_app_get_peer_info(g_conidx, &peer_info);

        /**
         * wait rssi results
         * todo, add timeout
        */
       OS_SemaphoreWait(&sg_wait_rssi_sem, OS_WAIT_FOREVER);

        LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);
        *rssi = g_bt_rssi;

        s_has_entry = 0;
        return OPRT_OS_ADAPTER_OK;
    }
    s_has_entry = 0;

    return OPRT_OS_ADAPTER_COM_ERROR;
}

/**
 * @brief tuya_os_adapt_bt start adv
 * @return int 
 */
int tuya_os_adapt_bt_start_adv(void)
{
    LOG(LOG_LVL_TRACE, "%s:%d\r\n", __func__, __LINE__);

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_bt stop adv
 * @return int 
 */
int tuya_os_adapt_bt_stop_adv(void)
{
    LOG(LOG_LVL_TRACE, "%s:%d\r\n", __func__, __LINE__);

    if (g_adv_is_started != 0) {
        ln_app_activity_stop(g_adv_actv_idx);
        g_adv_is_started = 0;
    }
    return OPRT_OS_ADAPTER_OK;
}

static void tuya_bt_scan_pre_decode(struct gapm_ext_adv_report_ind *recv_adv)
{
    unsigned char buffer[32];
    unsigned char pos = 0;
    unsigned char mac[6];
    static unsigned char old_data[24];
    unsigned short uuid;

    while (pos < recv_adv->length)
    {
        /* Length of the AD structure. */
        unsigned char length = recv_adv->data[pos++];
        unsigned char type;

        if ((length > 0x01) && ((pos + length) <= 31))
        {
            /* Copy the AD Data to buffer. */
            memcpy(buffer, recv_adv->data + pos + 1, length - 1);
            
            /* AD Type, one octet. */
            type = recv_adv->data[pos];

            switch (type)
            {
                case TY_BT_VER10_PROTOCOL_TYPE:
                    //老版遥控器定长25字节
                    if(length != 25){
                        return;
                    }
                    //重复过滤
                    if(0 == memcmp(old_data, buffer, 24)){
                        return;
                    }
                    memcpy(old_data, buffer, 24);
                    
                    if(ty_target_scan_info.scan_adv_cb){
                        /* copy mac */
                        memcpy(mac, recv_adv->trans_addr.addr.addr, 6);
                        //LOG_DEBUG("mac:%x:%x:%x:%x:%x:%x", mac[0],mac[1], mac[2],mac[3], mac[4],mac[5]);
                        ty_target_scan_info.scan_adv_cb((char *)buffer, length - 1, mac, type);
                    }
                    return;

                case TY_BT_VER11_PROTOCOL_TYPE_IDENTIFY:
                    #if 0
                        if(length != 0x02 || buffer[0] != 0x06) {
                            //LOG_ERR("data err, should be 0x06\r\n");
                        }
                    #endif
                    break;

                case TY_BT_VER11_PROTOCOL_TYPE_SER_UUID:
                    #if 1
                        uuid = ((*(unsigned char *)(buffer)) << 8) + (*(unsigned char *)(buffer + 1));
                        if(length != 0x03 || uuid != 0x50FD) {
                            //LOG_ERR("\r\nuuid err:0x%x", uuid);
                            return;
                        }
                    #endif
                    break;

                case TY_BT_VER11_PROTOCOL_TYPE_SER_DATA:
                    {
                        if((buffer[0] != 0x50) && (buffer[1] != 0xFD)) {
                            return;
                        }

                        //协议：len+type+uuid+data，新协议的buff要偏移掉uuid内容(2个字节)
                        if(ty_target_scan_info.scan_adv_cb){
                            /* copy mac */
                            // memcpy(mac, recv_adv->trans_addr.addr.addr, 6);

                            mac[5] = recv_adv->trans_addr.addr.addr[0];
                            mac[4] = recv_adv->trans_addr.addr.addr[1];
                            mac[3] = recv_adv->trans_addr.addr.addr[2];
                            mac[2] = recv_adv->trans_addr.addr.addr[3];
                            mac[1] = recv_adv->trans_addr.addr.addr[4];
                            mac[0] = recv_adv->trans_addr.addr.addr[5];

                            //LOG_DEBUG("mac:%x:%x:%x:%x:%x:%x\r\n", mac[0],mac[1], mac[2],mac[3], mac[4],mac[5]);
                            //tuya_debug_hex_dump("ble_scan_adv", 20, buffer, 32);
                            ty_target_scan_info.scan_adv_cb((char *)(buffer + 2), length -3, mac, type);
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        pos += length;
    }
}

void ty_bt_scan_adv_data_parser(struct gapm_ext_adv_report_ind *adv)
{
    if (ty_target_scan_info.scan_type == TY_BT_SCAN_BY_MAC)
    {
        if (g_bt_scan_found != 0) {
            return;
        }
        if (0 == memcmp(ty_target_scan_info.bd_addr, adv->trans_addr.addr.addr, 6)) {
            g_bt_scan_found = 1;
        }
    } else if (ty_target_scan_info.scan_type == TY_BT_SCAN_BY_NAME) {
        uint8_t pos = 0;
        uint8_t type = 0;
        uint8_t len  = 0;

        if (g_bt_scan_found != 0) {
            return;
        }

        do {
            len = adv->data[pos++];

            if ((len > 1) && ((pos + len) <= 31)) {
                type = adv->data[pos];
                if (GAP_AD_TYPE_COMPLETE_NAME == type) {
                    LOG(LOG_LVL_TRACE, "ADV NAME:%.*s\r\n", len - 1, (char *)(&adv->data[pos+1]));
                    if (0 == strncmp(&adv->data[pos+1], ty_target_scan_info.name, len - 1)) {
                        LOG(LOG_LVL_TRACE, "find target name!\r\n");
                        g_bt_scan_found = 1;
                        ty_target_scan_info.channel = 0;
                        ty_target_scan_info.rssi = adv->rssi;
                        memcpy(ty_target_scan_info.bd_addr,  adv->trans_addr.addr.addr, 6);
                        memset(ty_target_scan_info.name, 0, sizeof(ty_target_scan_info.name));
                        strncpy(ty_target_scan_info.name, 
                                &adv->data[pos+1], 
                                (len - 1) > DEVICE_NAME_LEN ? DEVICE_NAME_LEN : (len - 1));
                        break;
                    } else {
                        break;
                    }
                } else {
                    pos += len;
                }
            } else {
                break;
            }
        } while(pos < adv->length);
    } else if (ty_target_scan_info.scan_type == TY_BT_SCAN_BY_ADV) {
        tuya_bt_scan_pre_decode(adv);
    }
}

/**
 * @brief tuya_os_adapt_bt 主动扫描
 * @return int 
 */
int tuya_os_adapt_bt_assign_scan(IN OUT ty_bt_scan_info_t *info)
{
    LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);

    if (NULL == info) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    LOG(LOG_LVL_TRACE, "type:%d, name:%s, rssi:%d, channel:%d, timeout:%d, cb:%p\r\n",
            info->scan_type, info->name, info->rssi,
            info->channel, info->timeout_s, info->scan_adv_cb);

    memcpy(&ty_target_scan_info, info, sizeof(ty_bt_scan_info_t));

    g_bt_scan_found = 0;

    create_scanning();
    OS_MsDelay(10);
    start_scanning();

    /**
     * wait scan results
     * todo, add timeout
    */
#define SCAN_ONCE_WAIT_TIME_MS (10)
    volatile uint32_t wait_time_cnt = (info->timeout_s * 1000 + 
            SCAN_ONCE_WAIT_TIME_MS - 1) / SCAN_ONCE_WAIT_TIME_MS;
    while((g_bt_scan_found == 0) && (wait_time_cnt > 0))
    {
        wait_time_cnt--;
        OS_MsDelay(SCAN_ONCE_WAIT_TIME_MS);
    }
    LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);

    if (g_bt_scan_found != 0) {
        memcpy(info, &ty_target_scan_info, sizeof(ty_bt_scan_info_t));
        g_bt_scan_found = 0;
    } else {
        info->channel = 0;
        info->rssi = 0;
    }

    ln_app_activity_stop(g_scan_actv_idx);
    memset(&ty_target_scan_info, 0, sizeof(ty_bt_scan_info_t));
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_bt 开始scan接收
 * @return int 
 */
int tuya_os_adapt_bt_start_scan(void)
{
    LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);
    if (TY_BT_SCAN_BY_ADV == ty_target_scan_info.scan_type) {
        start_scanning();
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_bt 停止scan接收
 * @return int 
 */
int tuya_os_adapt_bt_stop_scan(void)
{
    LOG(LOG_LVL_TRACE, "[%s:%d]\r\n", __func__, __LINE__);
    if (TY_BT_SCAN_BY_ADV == ty_target_scan_info.scan_type) {
        ln_app_activity_stop(g_scan_actv_idx);
    }
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_bt scan初始化
 * @return int 
 */
int tuya_os_adapt_bt_scan_init(IN TY_BT_SCAN_ADV_CB scan_adv_cb)
{
    LOG(LOG_LVL_TRACE, "[%s:%d] cb:%p\r\n", __func__, __LINE__, scan_adv_cb);

    ty_bt_scan_adv_cb = scan_adv_cb;
    ty_target_scan_info.scan_type = TY_BT_SCAN_BY_ADV;
    ty_target_scan_info.scan_adv_cb = scan_adv_cb;

    create_scanning();
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_reg_bt_intf 接口注册
 * @return int 
 */
int tuya_os_adapt_reg_bt_intf(void)
{
    LOG(LOG_LVL_TRACE, "tuya_os_adapt_reg_bt_intf\r\n");
    return tuya_os_adapt_reg_intf(INTF_BT, (void *)&m_tuya_os_bt_intfs);
}
#endif /* defined(TY_BT_MOD) && (TY_BT_MOD == 1) */

