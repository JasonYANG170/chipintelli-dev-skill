/**
* @file tuya_device.c
* @author www.tuya.com
* @brief template demo for SDK WiFi & BLE for BK7231N, hardware: BK7231N NANO SoC board
* @version 0.2
* @date 2022-03-14
*
* @copyright Copyright (c) tuya.inc 2022
*
*/
#define _TUYA_DEVICE_GLOBAL

#include "uni_log.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_hal_system.h"
#include "tuya_iot_com_api.h"
#include "tuya_cloud_error_code.h"
#include "gw_intf.h"
#include "tuya_ir_control.h"
#include "tuya_device.h"
#include "dp_process.h"
#include "tuya_device_auth.h"
#include "cias_slave_message_handle.h"
/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/

/**
* @brief pre_gpio_test gpio test pre-interface, used to prepare for the gpio test
*
* @return none
*/
VOID mf_user_pre_gpio_test_cb(VOID)
{
    return;
}

/**
* @brief erase application data from flash
*
* @return none
*/
STATIC VOID hw_reset_flash_data(VOID)
{
    /* erase application data from flash */
    return;
}

/**
* @brief configuring the write callback interface
*
* @return none
*/
VOID mf_user_callback(VOID)
{
    hw_reset_flash_data();

    return;
}

/**
* @brief configure to enter the production test callback interface
*
* @return none
*/
VOID mf_user_enter_callback(VOID)
{
    return;
}

/**
* @brief Finished Product test callbacks
*
* @return OPRT_OK
*/
OPERATE_RET mf_user_product_test_cb(USHORT_T cmd, UCHAR_T *data, UINT_T len, OUT UCHAR_T **ret_data, OUT USHORT_T *ret_len)
{
    return OPRT_OK;
}

/**
* @brief application initialization prep work
*
* @param[in] none
* @return none
*/
VOID pre_app_init(VOID)
{
    return;
}

/**
* @brief device initialization prep work
*
* @param[in] none
* @return none
*/
VOID pre_device_init(VOID)
{
    PR_NOTICE("%s", tuya_iot_get_sdk_info());                              /* print SDK information */
    PR_NOTICE("%s:%s", APP_BIN_NAME, DEV_SW_VERSION);                      /* print the firmware name and version */
    PR_NOTICE("firmware compiled at %s %s", __DATE__, __TIME__);           /* print firmware compilation time */
    PR_NOTICE("system reset reason:[%s]", tuya_hal_system_get_rst_info()); /* print system reboot causes */

    SetLogManageAttr(TY_LOG_LEVEL_DEBUG); /* set log level */

    return;
}

/**
* @brief application initialization interface
*
* @param[in] none
* @return none
*/
VOID app_init(VOID)
{
    return;
}

/**
* @brief report all dp status
*
* @param[in] none
* @return none
*/
VOID hw_report_all_dp_status(VOID)
{
    //report all dp status
    update_all_dp();
    return;
}

/**
* @brief device activation status change callback
*
* @param[in] status: current status
* @return none
*/
VOID status_changed_cb(IN CONST GW_STATUS_E status)
{
    PR_NOTICE("status_changed_cb is status:%d", status);

    if (GW_NORMAL == status)
    {
        hw_report_all_dp_status();
    }
    else if (GW_RESET == status)
    {
        PR_NOTICE("status is GW_RESET");
    }

    return;
}

/**
* @brief firmware download finish result callback
*
* @param[in] fw: firmware info
* @param[in] download_result: 0 means download succes. other means fail
* @param[in] pri_data: private data
* @return none
*/
VOID upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    PR_DEBUG("download  Finish");
    PR_DEBUG("download_result:%d", download_result);

    return;
}

/**
* @brief firmware download content process callback
*
* @param[in] fw: firmware info
* @param[in] total_len: firmware total size
* @param[in] offset: offset of this download package
* @param[in] data && len: this download package
* @param[out] remain_len: the size left to process in next cb
* @param[in] pri_data: private data
* @return OPRT_OK: success  Other: fail
*/
OPERATE_RET get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
                             IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
    PR_DEBUG("Rev File Data");
    PR_DEBUG("Total_len:%d ", total_len);
    PR_DEBUG("Offset:%d Len:%d", offset, len);

    return OPRT_OK;
}

/**
* @brief ota inform callback
*
* @param[in] fw: firmware info
* @return 
*/
INT_T gw_ug_inform_cb(IN CONST FW_UG_S *fw)
{
    PR_DEBUG("Rev GW Upgrade Info");
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%d", fw->file_size);

    return tuya_iot_upgrade_gw(fw, get_file_data_cb, upgrade_notify_cb, NULL);
}

/**
* @brief called after reset device or app remove device 
*
* @param[in] type: gateway reset type
* @return none
* @others reset factory clear flash data
*/
VOID gw_reset_cb(IN CONST GW_RESET_TYPE_E type)
{
    PR_DEBUG("gw_reset_cb type:%d", type);

    if (GW_REMOTE_RESET_FACTORY != type)
    {
        PR_DEBUG("type is GW_REMOTE_RESET_FACTORY");
        return;
    }

    hw_reset_flash_data();

    return;
}

/**
* @brief called after the cloud sends data of type bool, value, enum, string or fault
*
* @param[in] dp: obj dp info
* @return none
*/
VOID dev_obj_dp_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    UCHAR_T i = 0;

    PR_DEBUG("dp->cid:%s dp->dps_cnt:%d", dp->cid, dp->dps_cnt);

    for (i = 0; i < dp->dps_cnt; i++)
    {
        deal_dp_proc(&(dp->dps[i]));
        // dev_report_dp_json_async(get_gw_cntl()->gw_if.id, dp->dps, dp->dps_cnt);
    }

    return;
}

/**
* @brief called after the cloud sends data of type raw
*
* @param[in] dp: raw dp info
* @return none
*/
VOID dev_raw_dp_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{
    raw_dp_proc(dp);
    return;
}

/**
* @brief query device dp status
*
* @param[in] dp_qry: query info
* @return none
*/
STATIC VOID dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    PR_NOTICE("Recv DP Query Cmd");

    hw_report_all_dp_status();

    return;
}

/**
* @brief This function is called when the state of the device connection has changed
*
* @param[in] stat: curr network status
* @return none
*/
VOID wf_nw_status_cb(IN CONST GW_WIFI_NW_STAT_E stat)
{
    PR_NOTICE("wf_nw_status_cb, wifi_status:%d", stat);
    CHAR_T str[] ="\"ir_ac_status_reporter\":\"ir_device\",\"ir_business_type\":\"3\"";
    if((stat == STAT_UNPROVISION) || (stat == STAT_AP_STA_UNCFG) || (stat == STAT_STA_CONN) || (stat == STAT_UNPROVISION_AP_STA_UNCFG) )
    {
        PR_NOTICE("wifi connecting ....");
        cias_message_send_interface(RCV_NET_CONFIGING_CMD, DEF_FILL, 0, NULL);   //联网中
    }
    if (STAT_CLOUD_CONN == stat || STAT_AP_CLOUD_CONN == stat)
    {
        hw_report_all_dp_status();
        PR_NOTICE(" wifi connected ok....");
        tuya_ir_attribute_report(TY_IR_INIT_CODE_FASTLZ, TY_CODE_FASTLZ_VER, tuya_iot_get_gw_id(), str);
        cias_message_send_interface(SEND_WIFI_CONNECTED_CMD, DEF_FILL, 0, NULL);   //联网成功
        
    }

    return;
}

void get_heap_size(void)
{
    unsigned char close_bt = 0;
    GW_WIFI_NW_STAT_E conn_state = 0;

    for (;;)
    {
#if 0
        get_wf_gw_nw_status(&conn_state);
        if (STAT_CLOUD_CONN == conn_state && close_bt == 0) {
            PR_NOTICE("close bt");
            tuya_ble_set_serv_switch(false);
            close_bt=1;
        }
#endif
        PR_NOTICE("bt close:%d, free heap size:%d, mini size:%d", close_bt, tuya_os_adapt_system_getheapsize(), tuya_os_adapt_system_getMiniheapsize());
        tuya_hal_system_sleep(1000);
    }
}

/**
* @brief device initialization interface
*
* @param[in] none
* @return OPRT_OK: success, others: please refer to tuya error code description document
*/
OPERATE_RET device_init(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;

    TY_IOT_CBS_S wf_cbs = {
        status_changed_cb,\ 
        gw_ug_inform_cb,\   
        gw_reset_cb,
        dev_obj_dp_cb,
        dev_raw_dp_cb,
        dev_dp_query_cb,
        NULL,
    };
#if 1 // 直接通过代码初始化key
    ws_db_init_mf();  //调用该函数初始kv flash
    //mf_kv_init();
    unsigned char prod_uuid[64] = {0};
    unsigned char prod_auth_key[128] = {0};
    memcpy(prod_uuid, PROD_UUID, strlen(PROD_UUID));
    memcpy(prod_auth_key, PROD_AUTH_KEY, strlen(PROD_AUTH_KEY));
    GW_PROD_INFO_S product_info = {
        .uuid = prod_uuid,
        .auth_key = prod_auth_key,
    };
    tuya_iot_set_gw_prod_info(&product_info);
#endif
    /* tuya IoT framework initialization */
    op_ret = tuya_iot_wf_soc_dev_init_param(GWCM_OLD_PROD, WF_START_SMART_FIRST, &wf_cbs, NULL, PRODUCT_ID, DEV_SW_VERSION);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("tuya_iot_wf_soc_dev_init_param error,err_num:%d", op_ret);
        return op_ret;
    }

    /* register Wi-Fi connection status change callback function */
    op_ret = tuya_iot_reg_get_wf_nw_stat_cb(wf_nw_status_cb);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("tuya_iot_reg_get_wf_nw_stat_cb is error,err_num:%d", op_ret);
        return op_ret;
    }
    set_ir_lib_dl_num_max(4);
    tuya_irlib_dl_funtion_init();

    tuya_hal_thread_create(NULL, "get_heap_size", 1024 * 1, TRD_PRIO_4, get_heap_size, NULL);

    return op_ret;
}
