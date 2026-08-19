#include "Config.h"
#if CIAS_BLE_CONFIG_ENABLE && !CIAS_IOT_TUYA_ENABLE
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
#include "usr_ble_app.h"
#include "usr_send_data.h"
#include "usr_app.h"
#include "cias_ble_config.h"
#include "cias_slave_message_handle.h"


OS_Thread_t ble_g_usr_app_thread;
#define BLE_USR_APP_TASK_STACK_SIZE  (1024)
#define DEVICE_NAME                  ("CI_BLE")
// #define DEVICE_NAME_LEN              (sizeof(DEVICE_NAME))
// #define USR_DATA                     ("111111")
// #define USR_DATA_LEN                 (sizeof(USR_DATA))
#define ADV_DATA_MAX_LENGTH          (28) //(USR_DATA_LEN + USR_DATA_LEN) <= ADV_DATA_MAX_LENGTH !!!

// #define USR_RES_DATA                 ("chipintelli_ln882h*&!@aaaa")
// #define USR_RES_DATA_LEN             (sizeof(USR_RES_DATA))
// #define RES_DATA_MAX_LENGTH          (31) //USR_RES_DATA_LEN <= ADV_DATA_MAX_LENGTH !!!
#define BLE_CONF_OK   "ble_success\r\n"
#define BLE_CONF_WAIT "ble_waiting\r\n"
#define BLE_CONF_FAIL "ble_failed!\r\n"

#define BLE_NET_CONFIG_INFO_LEN 256
#define BLE_NET_CONFIG_SSID_LEN 33
#define BLE_NET_CONFIG_PSWD_LEN 64

#define SLAVE 1
#define SERVICE 1
#define BLE_NET_CONFIG_TIMEOUT 20 //80s配网超时时间

uint8_t temp_net_info[BLE_NET_CONFIG_INFO_LEN];
uint8_t temp_net_info_ssid[BLE_NET_CONFIG_SSID_LEN];
uint8_t temp_net_info_pswd[BLE_NET_CONFIG_PSWD_LEN];
extern  wifi_sta_connect_t ble_connect;

struct ln_gattc_send_evt_cmd send_data;
uint8_t ln_conidx = 0;

extern uint8_t svc_uuid[16];
extern uint8_t con_num;
static uint8_t m_ble_con = 0;
static int m_ble_state = -1;
uint8_t adv_actv_idx  = 0;
uint8_t init_actv_idx = 0;

static OS_Queue_t ble_usr_queue;
static OS_Semaphore_t usr_semaphore;


static int m_ln_net_config = CIAS_NET_CONFIG_END;

void ln_set_ble_connect_flag(int flag)
{
    m_ble_state = flag;
}

int ln_get_ble_connect_flag(void)
{
    return m_ble_state;
}

int ln_get_net_config_state(void)
{
    return m_ln_net_config;
}

void ln_set_ble_con(uint8_t conidx)
{
    m_ble_con =conidx;
    LOG(LOG_LVL_INFO, "[%s:%d]ble_con=%d\r\n", __func__, __LINE__,m_ble_con);
}


uint8_t ln_get_ble_con(void)
{
    LOG(LOG_LVL_INFO, "[%s:%d]ble_con=%d\r\n", __func__, __LINE__, m_ble_con);
    return m_ble_con;
}

uint16_t ln_get_handle_ind(void)
{
    uint16_t handle = gattm_get_start_hdl() + DATA_TRANS_CHAR_VAL_TX + 1;
    LOG(LOG_LVL_INFO, "[%s:%d]handle=%d\r\n", __func__, __LINE__, handle);
    return handle;
}

int32_t ln_send_net_config_state(E_CIAS_NET_CONF_ST st, uint8_t* data, int32_t data_len)
{
    if(LN_NET_CONFIG_MODE == CIAS_NET_CONFIG_BLE && ln_get_ble_connect_flag() == 0)
    {
   //     jl_send_net_config_state(st, data, data_len);  //remove by yjd
    }
}

static void ln_ble_create_advertising(void)
{
#define APP_ADV_CHMAP                (0x07)  // Advertising channel map - 37, 38, 39
#define APP_ADV_INT_MIN              (640)   // Advertising minimum interval - 40ms (64*0.625ms)
#define APP_ADV_INT_MAX              (640)   // Advertising maximum interval - 40ms (64*0.625ms)

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
    LOG(LOG_LVL_DEBUG, "[%s] %d\r\n", __func__, __LINE__);
}

typedef struct cias_ble_product
{
    int8_t    product_name[6];
    int32_t   product_id;
    int8_t    device_ID[6];

}cias_ble_product_t;


static void ln_ble_set_adv_data(void)
{   
    uint8_t adv_data[ADV_DATA_MAX_LENGTH] = {0};
    cias_ble_product_t device_info;
    memset(&device_info,0,sizeof(cias_ble_product_t));

    memcpy(&device_info.product_name,DEVICE_NAME,sizeof(device_info.product_name));//设备名字
    device_info.product_id = 0x02;      //注意大小端 1红外遥控器 2红外空调伴侣 3台灯
    device_info.device_ID[0] = 0xFF;    //设备id 6bytes
    device_info.device_ID[1] = 0xFF;
    device_info.device_ID[2] = 0xFF;
    device_info.device_ID[3] = 0xFF;
    device_info.device_ID[4] = 0xFF;
    device_info.device_ID[5] = 0xFF;

    adv_data[0] = sizeof(device_info.product_name)+1;
    adv_data[1] = 0x09;  //adv type :local name
    memcpy(&adv_data[2],&device_info.product_name,sizeof(device_info.product_name));



    adv_data[sizeof(device_info.product_name)+2] = sizeof(device_info.product_id)+sizeof(device_info.device_ID)+1;
    adv_data[sizeof(device_info.product_name)+3] = 0xFF; //adv type :Manufacturer's data

    memcpy(&adv_data[sizeof(device_info.product_name)+4],&device_info.product_id,sizeof(device_info.product_id));
    memcpy(&adv_data[sizeof(device_info.product_name)+sizeof(device_info.product_id)+4]
    ,&device_info.device_ID,sizeof(device_info.device_ID));


    struct ln_gapm_set_adv_data_cmd adv_data_param;
    adv_data_param.actv_idx = adv_actv_idx;
    adv_data_param.length = sizeof(adv_data);
    adv_data_param.data = adv_data;
    ln_app_set_adv_data(&adv_data_param);
    
    // uint8_t rsp_data[RES_DATA_MAX_LENGTH] = {0};
    // rsp_data[0] = USR_RES_DATA_LEN + 1;
    // rsp_data[1] = 0xFF;
    // memcpy(&rsp_data[2],USR_RES_DATA,USR_RES_DATA_LEN);


    // struct ln_gapm_set_adv_data_cmd scan_rsp_data;
    // scan_rsp_data.actv_idx = adv_actv_idx;
    // scan_rsp_data.length   = sizeof(rsp_data);;
    // scan_rsp_data.data     = rsp_data;
    // ln_app_set_scan_rsp_data(&scan_rsp_data);
    LOG(LOG_LVL_DEBUG, "[%s] %d\r\n", __func__, __LINE__);
}

static void ln_ble_start_advertising(void)
{
    struct ln_gapm_activity_start_cmd  adv_start_param;
    adv_start_param.actv_idx = adv_actv_idx;
    adv_start_param.u_param.adv_add_param.duration = 0;
    adv_start_param.u_param.adv_add_param.max_adv_evt = 0;
    ln_app_advertise_start(&adv_start_param);
    LOG(LOG_LVL_DEBUG, "[%s] %d\r\n", __func__, __LINE__);
}

void ln_ble_stop_advertising(void)
{
    ln_app_activity_stop(adv_actv_idx);
}

void ln_ble_restart_adv(void)
{
    ln_ble_start_advertising();
}

static void ln_ble_start_adv(void)
{
    ln_ble_create_advertising();
    ln_ble_set_adv_data();
    ln_ble_start_advertising();
    LOG(LOG_LVL_DEBUG, "[%s] %d\r\n", __func__, __LINE__);
}


void usr_creat_queue(void)
{
    if(OS_OK != OS_QueueCreate(&ble_usr_queue, BLE_USR_MSG_QUEUE_SIZE, sizeof(ble_usr_msg_t)))
    {
        BLIB_LOG(BLIB_LOG_LVL_E, "usr QueueCreate rw_queue failed!!!\r\n");
    }
}

void usr_queue_msg_send(uint16_t id, uint16_t length, void *msg)
{
    ble_usr_msg_t usr_msg;
    usr_msg.id = id;
    usr_msg.len = length;
    usr_msg.msg = msg;
    OS_QueueSend(&ble_usr_queue, &usr_msg, OS_WAIT_FOREVER);
}

int usr_queue_msg_recv(void *msg, uint32_t timeout)
{
    return OS_QueueReceive(&ble_usr_queue, msg, timeout);
}

int ln_stop_ble_netconfig(void)
{
    // if (LN_NET_CONFIG_MODE != CIAS_NET_CONFIG_BLE) {
    //     return 0;
    // }
    
    // if (ln_get_ble_connect_flag() == 0) {
    //     ln_app_disconnect(ln_get_ble_con(), 0x16);
    // }
    
    // if (m_ln_net_config == CIAS_NET_CONFIG_ING) {
    //     ln_ble_stop_advertising();
    // }
    
    // m_ln_net_config = CIAS_NET_CONFIG_END;

    ln_app_disconnect(ln_get_ble_con(), 0x16);
    // ln_ble_stop_advertising();
    return 0;
}

int ln_cancel_ble_netconfig(void)  //认证测试case用
{
    if (ln_get_ble_connect_flag() == 0) {
        ln_send_net_config_state(E_CIAS_NET_CONF_ST_EXIT, NULL, 0); 
    }
    
    ln_stop_ble_netconfig();
}
extern uint8_t start_CI_BLE_config(bool BLE_switch);

void ble_netconfig_cias(uint8_t * ptr_ch,uint32_t ptr_len)
{
    static uint8_t temp_net_info_len = 0;
    static uint8_t temp_net_info_f = 0;
    static uint8_t ssid_len = 0;
    static uint8_t pswd_len = 0;
    uint8_t satrt_recv = 10;
    uint8_t stop_recv  = 11;
    uint8_t crc_recv  = 0;
    uint8_t crc_calculate  = 0;
    
    if(ptr_ch[ptr_len -1] == 0xFB && temp_net_info_len == (ssid_len+pswd_len))
    {
        LOG(LOG_LVL_DEBUG, "[3Rive recv data]:<%02X,%02X>,(%d)\r\n",ptr_ch[ptr_len -2],ptr_ch[ptr_len -1],ptr_len);
        crc_recv = ptr_ch[ptr_len -2];
        crc_calculate = (crc16_func(0,temp_net_info,temp_net_info_len)&0x00FF);//取后8位
        LOG(LOG_LVL_DEBUG, "[crc data]:(%d)(%d)\r\n",crc_recv,crc_calculate);
        if(crc_recv == crc_calculate)//crc校验
        {
            temp_net_info_f = stop_recv;//结束接收wifi数据
        }
        else
        {
            temp_net_info_f = 0;//异常结束接收wifi数据（数据出错）
            temp_net_info_len = 0;
            LOG(LOG_LVL_ERROR, "crc_recv error!!!\r\n");

            ln_stop_ble_netconfig();         
            cias_message_send_interface(SEND_WIFI_DISCONNECTED_CMD, DEF_FILL, 0, NULL);   //联网失败
            OS_MsDelay(100);
            start_CI_BLE_config(false);
            return;
        }
        
    }

    if(temp_net_info_f == stop_recv)//接收完wifi数据
    {
        memcpy(temp_net_info_ssid,temp_net_info,ssid_len);
        memcpy(temp_net_info_pswd,temp_net_info+ssid_len,pswd_len);
        
        LOG(LOG_LVL_INFO, "[4Rive recv data] wifi:%s,%d;pwd:%s,%d(%d)\r\n",temp_net_info_ssid,ssid_len,temp_net_info_pswd,pswd_len,temp_net_info_len);
        temp_net_info_f = 0;
        ble_connect.ssid = temp_net_info_ssid;
        ble_connect.pwd  = temp_net_info_pswd;
        wifi_sta_connect_t ble_connect_save;
        ble_connect_save.ssid = temp_net_info_ssid;
        ble_connect_save.pwd = temp_net_info_pswd;
        
        OS_MsDelay(1000);
        uint8_t timeout_t = BLE_NET_CONFIG_TIMEOUT;
        while(cias_get_wifi_sta_connect_state() != NETDEV_LINK_UP)
        {
            timeout_t--;
            if(timeout_t)
            {
                send_data.length = sizeof(BLE_CONF_WAIT);
                send_data.value = BLE_CONF_WAIT;
                ln_app_gatt_send_ntf(ln_conidx,&send_data);
            }
            else
            {
                break;
            }
            LOG(LOG_LVL_DEBUG, "wifi connecting\r\n");
            OS_MsDelay(1000);
        }
        if(timeout_t>0)
        {
            set_sta_to_flash(&ble_connect_save);            

            
            send_data.length = sizeof(BLE_CONF_OK);
            send_data.value = BLE_CONF_OK;
            ln_app_gatt_send_ntf(ln_conidx,&send_data);    
            LOG(LOG_LVL_INFO, "wifi connect success\r\n");  
            
            OS_MsDelay(2000);                                                         //需要小程序显示配网成功后在播报，不加延时会提前播报	
            cias_message_send_interface(ENTER_SMATRCONFIG_CMD, DEF_FILL, 0, NULL);    //进入配网模式
            ln_stop_ble_netconfig();
            // OS_MsDelay(1000);
            // start_CI_BLE_config(false);

            extern void ln_chip_reboot(void);//设备重启
            ln_chip_reboot();
            
        }
        else
        {
            LOG(LOG_LVL_CTRL, "wifi connect failed\r\n");
            send_data.length = sizeof(BLE_CONF_FAIL);
            send_data.value = BLE_CONF_FAIL;
            ln_app_gatt_send_ntf(ln_conidx,&send_data);

            ln_stop_ble_netconfig();         
            cias_message_send_interface(SEND_WIFI_DISCONNECTED_CMD, DEF_FILL, 0, NULL);   //联网失败
            OS_MsDelay(100);
            start_CI_BLE_config(false);
        }
        return;
    }

    if(temp_net_info_f == satrt_recv)//接收wifi数据
    {   
        if(temp_net_info_len < (ssid_len+pswd_len))
        {
            memcpy(temp_net_info+temp_net_info_len,ptr_ch,ptr_len) ;
            temp_net_info_len += ptr_len;
            LOG(LOG_LVL_DEBUG, "[2Rive recv data]:<%s>, T:%d\r\n",ptr_ch,ptr_len,temp_net_info_len);
        }
        else
        {
            LOG(LOG_LVL_ERROR, "wifi info length error !!!\r\n");
            ln_stop_ble_netconfig();         
            cias_message_send_interface(SEND_WIFI_DISCONNECTED_CMD, DEF_FILL, 0, NULL);   //联网失败
            OS_MsDelay(100);
            start_CI_BLE_config(false);
        }
        
        return;
    }

    if(ptr_ch[0] == 0xFA && temp_net_info_f == 0)
    {        
        ssid_len = ptr_ch[1];
        pswd_len = ptr_ch[2];
        if((ssid_len > 0 && ssid_len < BLE_NET_CONFIG_SSID_LEN) && (pswd_len > 0 && pswd_len < BLE_NET_CONFIG_PSWD_LEN))
        {
            temp_net_info_f = satrt_recv;//开始接收wifi数据
            temp_net_info_len = 0;
            memset(temp_net_info,0,BLE_NET_CONFIG_INFO_LEN);
            memset(temp_net_info_ssid,0,BLE_NET_CONFIG_SSID_LEN);
            memset(temp_net_info_pswd,0,BLE_NET_CONFIG_PSWD_LEN);
            LOG(LOG_LVL_DEBUG, "[1Rive recv data]:<%02X,%02X,%02X>,(%d),(%d),(%d)\r\n",ptr_ch[0],ptr_ch[1],ptr_ch[2],ptr_len,ssid_len,pswd_len);
        }
        else
        {
            LOG(LOG_LVL_DEBUG, "[error ssid_len||pswd_len]:<%02X,%02X,%02X>\r\n",ptr_ch[0],ptr_ch[1],ptr_ch[2]);

            ln_stop_ble_netconfig();         
            cias_message_send_interface(SEND_WIFI_DISCONNECTED_CMD, DEF_FILL, 0, NULL);   //联网失败
            OS_MsDelay(100);
            start_CI_BLE_config(false);
        }
        return;       
    }

}


void ln_ble_app_task_entry(void *params)
{
    ble_usr_msg_t usr_msg;

    usr_creat_queue();

    extern void ble_app_init(void);
    ble_app_init();
#if (SLAVE)
    ln_ble_start_adv();
#endif
#if (MASTER)
	ln_ble_start_init();
#endif
#if SERVICE
	data_trans_svc_add();
#endif

    LOG(LOG_LVL_CTRL, "\r\n\r\n\n\n !!!CI_BLE_CONFIG_START!!! \r\n\n\n\n",con_num);

	while(1)
	{
        if(OS_OK == usr_queue_msg_recv((void *)&usr_msg, OS_WAIT_FOREVER))
		{
            LOG(LOG_LVL_TRACE, "connect device number :%d \r\n",con_num);
			switch(usr_msg.id)
			{
                case BLE_MSG_WRITE_DATA:
                {
                    struct ln_attc_write_req_ind *p_param = (struct ln_attc_write_req_ind *)usr_msg.msg;
                    
                    hexdump(LOG_LVL_INFO, "[recv data]", (void *)p_param->value, p_param->length);
                    // LOG(LOG_LVL_TRACE, "[recv data]\r\n");
                    ble_netconfig_cias(p_param->value,p_param->length);
                    send_data.handle = p_param->handle + 2;
                    ln_conidx = p_param->conidx;

                }
                break;

                case BLE_MSG_CONN_IND:
                {
                    struct ln_gapc_connection_req_info *p_param=(struct ln_gapc_connection_req_info *)usr_msg.msg;
#if (CLIENT)
                    struct ln_gattc_disc_cmd param_ds;
                    param_ds.operation = GATTC_DISC_BY_UUID_SVC;
                    param_ds.start_hdl = 1;
                    param_ds.end_hdl   = 0xFFFF;
                    param_ds.uuid_len  =sizeof(svc_uuid);
                    param_ds.uuid = svc_uuid;
                    ln_app_gatt_discovery(p_param->conidx, &param_ds);
#endif
                    ln_app_gatt_exc_mtu(p_param->conidx);
                    struct ln_gapc_set_le_pkt_size_cmd pkt_size;
                    pkt_size.tx_octets = 251;
                    pkt_size.tx_time   = 2120;
                    OS_MsDelay(1000);
                    ln_app_param_set_pkt_size(p_param->conidx,  &pkt_size);

                    struct ln_gapc_conn_param conn_param;
                    conn_param.intv_min = 80;  // 10x1.25ms  (7.5ms--4s)
                    conn_param.intv_max = 90;  // 10x1.25ms  (7.5ms--4s)
                    conn_param.latency  = 10;
                    conn_param.time_out = 3000;  //ms*n
                    ln_app_update_param(p_param->conidx, &conn_param);
                }
                break;
                case BLE_MSG_DISCONN_IND:
                {
                    OS_ThreadDelete(NULL);
                }
                break;

                case BLE_MSG_SVR_DIS:
                {
#if (CLIENT)
                    struct ln_gattc_disc_svc *p_param = (struct ln_gattc_disc_svc *)usr_msg.msg;
                    uint8_t data[] = {0x12,0x78,0x85};
                    struct ln_gattc_write_cmd param_wr;
                    param_wr.operation    = GATTC_WRITE;
                    param_wr.auto_execute = true;
                    param_wr.handle       = p_param->start_hdl + 2;
                    param_wr.length       = sizeof(data);
                    param_wr.offset = 0;
                    param_wr.value = data;
                    ln_app_gatt_write(p_param->conidx,&param_wr);
#endif
                }
                break;

                default:
                    break;
			}
            blib_free(usr_msg.msg);
		}
	}
}

static void ln_ap_startup_cb(void * arg)
{
    netdev_set_state(NETIF_IDX_AP, NETDEV_UP);
}

#endif
