/**
* @file dp_process.c
* @author www.tuya.com
* @brief 
* @version 0.1
* @date 2021-08-19
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
#include <stdio.h>
#include "uni_log.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_cloud_com_defs.h"
#include "ty_cJSON.h"

#include "dp_process.h"
#include "tuya_ir_control.h"
#include "cias_ir_flash.h"
#include "cias_log.h"
#include "cias_system_manage.h"
#include "cias_slave_message_handle.h"
#include "cias_common.h"
#include "svc_irlib_download_ext.h"
#include "cias_ir_common.h"
TY_IRLIB_DL_KT_STAT_S g_ty_irlib_dl_kt_info; //记录空调当前设备状态
extern tuya_ir_code_store_t g_tuya_ir_code_store;
extern cias_system_manage_param_t g_cias_system_manage_param;
extern air_conditioner_ir_status_t g_air_conditioner_ir_status;
/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

struct remote_info g_remote_info = {0};

/***********************************************************
***********************function define**********************
***********************************************************/

/**
* @brief upload all dp data
*
* @return none
*/
VOID_T update_all_dp(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    update_local_ir_code_info();
    // INT_T dp_cnt = 1; /* update DP number */

    // /* no connect router, return */
    // GW_WIFI_NW_STAT_E wifi_state = STAT_LOW_POWER;
    // get_wf_gw_nw_status(&wifi_state);
    // if (wifi_state <= STAT_AP_STA_DISC || wifi_state == STAT_STA_DISC) {
    //     return;
    // }

    // TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    // if(NULL == dp_arr) {
    //     PR_ERR("malloc failed");
    //     return;
    // }

    // /* initialize requested memory space */
    // memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    // dp_arr[0].dpid = DPID_LIGHT_SWITCH; /* DP ID */
    // dp_arr[0].type = PROP_BOOL; /* DP type */
    // dp_arr[0].time_stamp = 0;
    // dp_arr[0].value.dp_bool = get_light_status(); /* DP data */

    // /* report DP */
    // op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    // /* free requested memory space */
    // Free(dp_arr);
    // dp_arr = NULL;
    // if(OPRT_OK != op_ret) {
    //     PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    // }
    return op_ret;
}

void update_irlib_dl_rate(unsigned char rate)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 1; /* update DP number */

    /* no connect router, return */
    GW_WIFI_NW_STAT_E wifi_state = STAT_LOW_POWER;
    get_wf_gw_nw_status(&wifi_state);
    if (wifi_state <= STAT_AP_STA_DISC || wifi_state == STAT_STA_DISC)
    {
        PR_ERR("update ir lib dl rate error, no connect wifi");
        return;
    }

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt * SIZEOF(TY_OBJ_DP_S));
    if (NULL == dp_arr)
    {
        PR_ERR("malloc failed");
        return;
    }

    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt * SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_LIB_RATE; /* DP ID */
    dp_arr[0].type = PROP_VALUE;    /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_value = rate; /* DP data */
    PR_ERR("rate:%d", rate);

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL, dp_arr, dp_cnt);

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;
    if (OPRT_OK != op_ret)
    {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num", op_ret);
    }
}

/**
* @brief handle dp commands from the cloud
*
* @param[in] root: pointer header for dp data
* @return none
*/
VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root)
{
    OPERATE_RET op_ret = OPRT_OK;

    UCHAR_T dpid;
    if (NULL == root)
    {
        return;
    }
    dpid = root->dpid;
    PR_NOTICE("dpid:%d", dpid);

    switch (dpid)
    {
    case DPID_LIB_RATE:
        PR_NOTICE("lib dl rate: %d", root->value);
        break;
    case DPID_IR_SEND:
        PR_NOTICE("DPID_IR_SEND:%s", root->value);
        if (root->value.dp_str != NULL)
        {
#if 0
            PR_NOTICE("start parse root->value....\r\n");
            ty_cJSON *json_root = NULL;
            json_root = ty_cJSON_Parse(root->value.dp_str);
            ty_cJSON *json_head = ty_cJSON_GetObjectItem(json_root, "head");
            ty_cJSON *json_key1 = ty_cJSON_GetObjectItem(json_root, "key1");
            //  PR_NOTICE("---head = %s\r\n", json_head->valuestring);
            //  PR_NOTICE("---key1 = %s\r\n", json_key1->valuestring);
            cias_local_send_ir(json_key1->valuestring, json_head->valuestring);
            ty_cJSON_Delete(json_root);
#endif
            PR_NOTICE("start parse root->value....\r\n");
            ty_cJSON *json_root = NULL;
            json_root = ty_cJSON_Parse(root->value.dp_str);
            ty_cJSON *json_control = ty_cJSON_GetObjectItem(json_root, "control");
            if (0 != strcmp(json_control->valuestring, "send_ir"))
            {
                PR_NOTICE("is no  send ir");
                ty_cJSON_Delete(json_root);
                return;
            }
            ty_cJSON *json_head = ty_cJSON_GetObjectItem(json_root, "head");
            ty_cJSON *json_key1 = ty_cJSON_GetObjectItem(json_root, "key1");
            ty_cJSON *json_data = ty_cJSON_GetObjectItem(json_key1, "data");
            ty_cJSON *json_key = ty_cJSON_GetObjectItem(json_key1, "key");
            PR_NOTICE("head:%s", json_head->valuestring);
            PR_NOTICE("data:%s", json_data->valuestring);
            PR_NOTICE("key:%s", json_key->valuestring);
            sscanf(json_key->valuestring, "M%d_T%d_S%d", &g_air_conditioner_ir_status.mode_val, &g_air_conditioner_ir_status.temp_val, &g_air_conditioner_ir_status.wind_val);
            PR_NOTICE("g_air_conditioner_ir_status.mode_val = %d", g_air_conditioner_ir_status.mode_val);
            PR_NOTICE("g_air_conditioner_ir_status.temp_val = %d", g_air_conditioner_ir_status.temp_val);
            PR_NOTICE("g_air_conditioner_ir_status.wind_val = %d", g_air_conditioner_ir_status.wind_val);
            //  PR_NOTICE("---head = %s\r\n", json_head->valuestring);
            //  PR_NOTICE("---key1 = %s\r\n", json_key1->valuestring);
            cias_local_send_ir(json_data->valuestring, json_head->valuestring);
            ty_cJSON_Delete(json_root);
        }
        break;

    default:
        break;
    }
}

void sync_cmd_proc(struct remote_info *info, char *raw_dp_data)
{
    if (NULL == info || NULL == raw_dp_data)
    {
        return;
    }

    ty_cJSON *root = NULL;
    ty_cJSON *temp_value = NULL;

    root = ty_cJSON_Parse(raw_dp_data);
    // g_cias_system_manage_param.tuya_ir_down_flag = 1;      //开始下载码库，节约内存删除其他任务
    // cias_task_delay_ms(2000);                              //延迟2S等待释放资源
    temp_value = ty_cJSON_GetObjectItem(root, "remoteid"); // 品类内唯一
    info->remote_id = temp_value->valueint;
    g_tuya_ir_code_store.ir_code_head.ir_remote_id = temp_value->valueint;
    PR_DEBUG("remote_id:%d", info->remote_id);

    temp_value = ty_cJSON_GetObjectItem(root, "type");
    info->type = temp_value->valueint;
    PR_DEBUG("type:%d", info->type);
    g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type = info->type;
    CIAS_LOG_HL("g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type = %d\r\n", g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type);

    temp_value = ty_cJSON_GetObjectItem(root, "devid"); //码库中唯一
    memset(info->dev_id, 0, sizeof(info->dev_id));
    memcpy(info->dev_id, temp_value->valuestring, sizeof(char) * strlen(temp_value->valuestring));
    CIAS_LOG_HL("dev_id:%s, strlen:%d", info->dev_id, strlen(info->dev_id));

    g_tuya_ir_code_store.ir_code_head.ir_data_offset = 0;
    g_tuya_ir_code_store.ir_code_head.ir_dev_id_len = strlen(info->dev_id);
    CIAS_LOG_HL("g_tuya_ir_code_store.ir_code_head.ir_dev_id_len = %d\r\n", g_tuya_ir_code_store.ir_code_head.ir_dev_id_len);
    cias_ir_partition_erase(g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type); //擦除分区
    g_tuya_ir_code_store.ir_code_head.ir_dev_id = cias_malloc(g_tuya_ir_code_store.ir_code_head.ir_dev_id_len);
    if (g_tuya_ir_code_store.ir_code_head.ir_dev_id != NULL)
    {
        memcpy(g_tuya_ir_code_store.ir_code_head.ir_dev_id, info->dev_id, g_tuya_ir_code_store.ir_code_head.ir_dev_id_len);
        CIAS_LOG_HL("g_tuya_ir_code_store.ir_code_head.ir_dev_id = %s\r\n", g_tuya_ir_code_store.ir_code_head.ir_dev_id);
    }
    else
    {
        CIAS_LOG_HL("g_tuya_ir_code_store.ir_code_head.ir_dev_id malloc error\r\n");
    }

    ty_cJSON_Delete(root);
}

void raw_dp_proc(IN CONST TY_RECV_RAW_DP_S *raw_dp)
{

    char lib_devid[32];

    UCHAR_T dpid;
    if (NULL == raw_dp)
    {
        PR_ERR("raw_dp_proc input error ");
        return;
    }

    dpid = raw_dp->dpid;

    switch (dpid)
    {
    case DPID_LIB_SYNC:
        PR_NOTICE("lib sync: %s", raw_dp->data);
        sync_cmd_proc(&g_remote_info, raw_dp->data);
        start_dl_ir_lib(g_remote_info.dev_id); // start download ir lib
        break;
    case DPID_LIB_INFO:
        PR_NOTICE("DPID_LIB_INFO: %s", raw_dp->data);
        update_local_ir_code_info();
        break;
    case DPID_IR_STUDY:
        /* nothing to do */
        break;

    default:
        break;
    }
}
//上传本地码库状态
void update_local_ir_code_info(void)
{
    ty_cJSON *root = ty_cJSON_CreateObject();
    tuya_ir_code_head_t tuya_ir_code_head;
    ty_cJSON *c_type = ty_cJSON_CreateArray();
    if (NULL == root)
    {
        PR_ERR("creat json obj err!");
        return;
    }
    CIAS_LOG_HL("------tuya_ir_store_head_info_read stat........\r\n");
    for (int i = IR_SET_TOP_BOX_ID; i < IR_DEV_MAX; i++)
    {
        CIAS_LOG_HL("------tuya_ir_store_head_info_read stat........\r\n");
        memset(&tuya_ir_code_head, 0, sizeof(tuya_ir_code_head_t) - 8);
        if (check_ir_data_available(i, &tuya_ir_code_head) == 0)
        {
            CIAS_LOG_HL("------tuya_ir_store_head_info_read ok, dev num = %d\r\n", i);
            CIAS_LOG_HL("tuya_ir_code_head.ir_remote_id = %d\r\n", tuya_ir_code_head.ir_remote_id);
            CIAS_LOG_HL("tuya_ir_code_head.tuya_ir_dev_type = %d\r\n", tuya_ir_code_head.tuya_ir_dev_type);
            CHAR_T buf[8] = {0};
            ty_cJSON_AddItemToArray(c_type, ty_cJSON_CreateNumber(tuya_ir_code_head.tuya_ir_dev_type));
            sprintf(buf, "r%d", tuya_ir_code_head.tuya_ir_dev_type);
            ty_cJSON_AddNumberToObject(root, buf, tuya_ir_code_head.ir_remote_id);
        }
    }
    ty_cJSON_AddItemToObject(root, "type", c_type);
    CHAR_T *out = ty_cJSON_PrintUnformatted(root);
    CIAS_LOG_HL("-----out:%s", out);
    dev_report_dp_raw_sync_extend(NULL, DPID_LIB_INFO, out, strlen(out), 10000);
    if(root)
    {
        ty_cJSON_Delete(root);
        root = NULL;
    }
    if (out)
    {
        Free(out);
        out = NULL;
    }
}
