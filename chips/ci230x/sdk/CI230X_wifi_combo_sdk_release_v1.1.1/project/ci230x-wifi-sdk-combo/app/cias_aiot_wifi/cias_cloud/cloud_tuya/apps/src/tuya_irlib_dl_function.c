/**
 * @file tuya_irlib_dl_function.c
 * @author zhake@tuya.com
 * @brief Tuya Infrared Code library download Demo
 * @version 1.0.0
 * @date 2021-05-25
 * @copyright Copyright (c) tuya.inc 2021
 */

#include "uni_log.h"
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "ty_cJSON.h"
#include "tuya_device.h"
#include "tuya_iot_wifi_api.h"
// #include "tuya_ir_control.h"
#include "svc_irlib_download.h"
#include "svc_irlib_download_ext.h"
#include <string.h>
#include "com_mmod.h"
#include "gw_intf.h"
#include "tuya_iot_com_api.h"

#include "cias_ir_flash.h"
#include "cias_system_manage.h"
#include "reboot_trace.h"
#include "cias_common.h"
#include "cias_slave_message_handle.h"
#include "cias_tuya_ir_protocol_handle.h"
#define IR_CODE_LIB_NUM 3 //存储码库个数
extern cias_system_manage_param_t g_cias_system_manage_param;
tuya_ir_code_store_t g_tuya_ir_code_store = {0};
int g_start_read_ir_flag = 0;
#define TY_DEVID_MAX_LEN (32)

STATIC TM_MSG_S *sg_ir_attr_tm_msg = NULL;
STATIC CHAR_T sub_devid[TY_DEVID_MAX_LEN];

char *my_ir_date = NULL;
extern void update_irlib_dl_rate(unsigned char rate);

STATIC CHAR_T cur_percent = 0;

OPERATE_RET demo_irlib_dl_data_trans_cb(IN TY_IRLIB_DL_DATA_TYPE_E data_type, IN CHAR_T *p_head, IN TY_IRLIB_DL_IRDATA_INFO_S *p_keyinfo,
                                        IN UCHAR_T keyinfo_num, INOUT TY_IRLIB_DL_BLOCK_STAT_E *p_isblock, IN USHORT_T total_num, IN USHORT_T cur_num)
{
    UCHAR_T i = 0;
    STATIC UCHAR_T cnt = 0;
    PR_NOTICE("app rev data_type:%d num:%d t_num:%d c_num:%d----remain heapsize:%d", data_type, keyinfo_num, total_num, cur_num, tuya_hal_system_getheapsize());
    switch (data_type)
    {
    case TY_IRLIB_DL_HEADINFO:
    {
        PR_NOTICE("App rev headinf:%s", p_head);
        g_tuya_ir_code_store.ir_code_head.ir_head_id_len = strlen(p_head);
        PR_NOTICE("g_tuya_ir_code_store.ir_code_head.ir_head_id_len = %d\r\n", g_tuya_ir_code_store.ir_code_head.ir_head_id_len);
        g_tuya_ir_code_store.ir_code_head.ir_head_id = cias_malloc(g_tuya_ir_code_store.ir_code_head.ir_head_id_len);
        if (g_tuya_ir_code_store.ir_code_head.ir_head_id != NULL)
        {
            g_tuya_ir_code_store.ir_code_head.ir_data_offset = g_tuya_ir_code_store.ir_code_head.ir_head_id_len + g_tuya_ir_code_store.ir_code_head.ir_dev_id_len + sizeof(tuya_ir_code_head_t) - 8;
            memcpy(g_tuya_ir_code_store.ir_code_head.ir_head_id, p_head, g_tuya_ir_code_store.ir_code_head.ir_head_id_len);
            g_start_read_ir_flag = 1;
            cias_message_send_interface(CIAS_IR_LOAD_DATA_START, DEF_FILL, 0, NULL);
        }
    }
    break;

    case TY_IRLIB_DL_KEYINFO:
    {
        uint8_t *ir_temp_buf = cias_malloc(1024); //申请1024做缓冲buf
        memset(ir_temp_buf, 0, 1024);
        int read_offset = 0;
        if (g_start_read_ir_flag)
        {
            g_start_read_ir_flag = 0;
            g_tuya_ir_code_store.ir_code_head.ir_data_num = total_num;
            PR_NOTICE("g_tuya_ir_code_store.ir_code_head.ir_head_id = %s\r\n", g_tuya_ir_code_store.ir_code_head.ir_head_id);
        }
        for (i = 0; i < keyinfo_num; i++)
        {
            if (TY_IRLIB_DL_KEY_TYPE == p_keyinfo[i].keyorkeyid) //空调类
            {
                PR_NOTICE("App rev keyinf key:%s,data:%s, len:%d", p_keyinfo[i].key_info_u.key, p_keyinfo[i].p_irdata, strlen(p_keyinfo[i].p_irdata));
                g_tuya_ir_code_store.ir_conditioner_code_data.ir_key_len = strlen(p_keyinfo[i].key_info_u.key);
                g_tuya_ir_code_store.ir_conditioner_code_data.ir_data_len = strlen(p_keyinfo[i].p_irdata);
                PR_NOTICE("g_tuya_ir_code_store.ir_conditioner_code_data.ir_key_len = %d\r\n", g_tuya_ir_code_store.ir_conditioner_code_data.ir_key_len);
                PR_NOTICE("g_tuya_ir_code_store.ir_conditioner_code_data.ir_data_len = %d\r\n", g_tuya_ir_code_store.ir_conditioner_code_data.ir_data_len);
                memcpy(ir_temp_buf + read_offset, (uint8_t *)&g_tuya_ir_code_store.ir_conditioner_code_data, sizeof(tuya_conditioner_ir_code_data_t) - 8);
                read_offset += (sizeof(tuya_conditioner_ir_code_data_t) - 8);
                memcpy(ir_temp_buf + read_offset, p_keyinfo[i].key_info_u.key, g_tuya_ir_code_store.ir_conditioner_code_data.ir_key_len);
                read_offset += g_tuya_ir_code_store.ir_conditioner_code_data.ir_key_len;
                memcpy(ir_temp_buf + read_offset, p_keyinfo[i].p_irdata, g_tuya_ir_code_store.ir_conditioner_code_data.ir_data_len);
                read_offset += g_tuya_ir_code_store.ir_conditioner_code_data.ir_data_len;
                CIAS_LOG_HL("g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type = %d, read_offset = %d\r\n", g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type, read_offset);
            }
            else //非空调类
            {
                PR_NOTICE("--App rev keyinf keyid:%d,data:%s", p_keyinfo[i].key_info_u.key_id,
                          p_keyinfo[i].p_irdata);
                g_tuya_ir_code_store.ir_universal_code_data.keyid = p_keyinfo[i].key_info_u.key_id;
                g_tuya_ir_code_store.ir_universal_code_data.ir_data_len = strlen(p_keyinfo[i].p_irdata);
                PR_NOTICE("g_tuya_ir_code_store.ir_universal_code_data.keyid = %d\r\n", g_tuya_ir_code_store.ir_universal_code_data.keyid);
                PR_NOTICE("g_tuya_ir_code_store.ir_universal_code_data.ir_data_len = %d\r\n", g_tuya_ir_code_store.ir_universal_code_data.ir_data_len);
                memcpy(ir_temp_buf + read_offset, (uint8_t *)&g_tuya_ir_code_store.ir_universal_code_data, sizeof(tuya_universal_ir_code_data_t) - 4);
                read_offset += (sizeof(tuya_universal_ir_code_data_t) - 4);
                memcpy(ir_temp_buf + read_offset, p_keyinfo[i].p_irdata, g_tuya_ir_code_store.ir_universal_code_data.ir_data_len);
                read_offset += g_tuya_ir_code_store.ir_universal_code_data.ir_data_len;
                CIAS_LOG_HL("g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type = %d, read_offset = %d\r\n", g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type, read_offset);
                CIAS_LOG_HL("g_tuya_ir_code_store.ir_code_head.ir_data_offset = 0x%x", g_tuya_ir_code_store.ir_code_head.ir_data_offset);
            }
        }
        tuya_ir_data_write_flash(g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type, g_tuya_ir_code_store.ir_code_head.ir_data_offset, ir_temp_buf, read_offset);
        g_tuya_ir_code_store.ir_code_head.ir_data_offset += read_offset;
        if (ir_temp_buf)
            free(ir_temp_buf);
        cur_percent = cur_num * 100 / total_num;
        update_irlib_dl_rate(cur_percent);
        cias_message_send_interface(CIAS_IR_LOADING_DATA, DEF_FILL, 0, NULL);
        if (cur_percent == 100)
        {
            CIAS_LOG_HL("start to read ir data ......\r\n");
            g_tuya_ir_code_store.ir_code_head.ir_available_flag = 0x5A; //码库可用标志
            tuya_ir_store_head_info_write(g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type, g_tuya_ir_code_store.ir_code_head);
            if (g_tuya_ir_code_store.ir_code_head.ir_dev_id)
                cias_free(g_tuya_ir_code_store.ir_code_head.ir_dev_id);
            if (g_tuya_ir_code_store.ir_code_head.ir_head_id)
                cias_free(g_tuya_ir_code_store.ir_code_head.ir_head_id);
             cias_message_send_interface(CIAS_IR_LOAD_DATA_OVER, DEF_FILL, 0, NULL);
             tuya_ir_protocol_init();
             update_local_ir_code_info();  //上报本地码库状态
            //cias_task_delay_ms(3000);
            //tuya_ir_data_read_from_flash(g_tuya_ir_code_store.ir_code_head.tuya_ir_dev_type);
            CIAS_LOG_HL("end to read ir data ......\r\n");
            //下载完码库，比较耗费资源，重启wifi
            //ln_chip_reboot();
        }
    }
    break;
    }

    *p_isblock = TY_IRLIB_DL_RUN;
    return OPRT_OK;
}

/** 
 * @brief: tuya_irlib_dl_funtion_init
 * @desc: Tuya infrared code library download function init
 * @return OPERATE_RET
 * @note none
 */
OPERATE_RET tuya_irlib_dl_funtion_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    TY_IRLIB_DL_USER_REG_CB_S irlib_dl_user_cb;
    irlib_dl_user_cb.irlib_dl_data_cb = demo_irlib_dl_data_trans_cb;
    op_ret = tuya_svc_irlib_dl_init(&irlib_dl_user_cb);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("tuya_svc_irlib_dl_init err:%d", op_ret);
        return op_ret;
    }
}

void start_dl_ir_lib(char *ir_lib_id)
{
    update_irlib_dl_rate(0);
    PR_NOTICE("ir_lib_id:%s", ir_lib_id);
    tuya_svc_irlib_dl_start(ir_lib_id);
}
