/**
 * @file svc_irlib_download.h
 * @author zhake@tuya.com
 * @brief Tuya Infrared code library download
 * @version 1.0.0
 * @date 2021-03-27
 * @copyright Copyright (c) tuya.inc 2021
 */

#ifndef __SVC_IRLIB_DOWNLOAD_H__
#define __SVC_IRLIB_DOWNLOAD_H__

#include "tuya_cloud_types.h"
#include "uni_msg_queue.h"
#include "uni_thread.h"
#include "tuya_hal_semaphore.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    TY_IRLIB_DL_HEADINFO = 0,
    TY_IRLIB_DL_KEYINFO = 1,
}TY_IRLIB_DL_DATA_TYPE_E;

typedef enum
{
    TY_IRLIB_DL_RUN = 0,
    TY_IRLIB_DL_BLOCK = 1,
}TY_IRLIB_DL_BLOCK_STAT_E;

typedef enum
{
    TY_IRLIB_DL_INVALID_TYPE = 0,
    TY_IRLIB_DL_KEY_TYPE = 1,
    TY_IRLIB_DL_KEYID_TYPE = 2,
}TY_IRLIB_DL_TYPE_E;

typedef struct
{
    CHAR_T *p_irdata;
    TY_IRLIB_DL_TYPE_E keyorkeyid;
    union
    {
        CHAR_T *key;
        UINT_T key_id;
    }key_info_u;
}TY_IRLIB_DL_IRDATA_INFO_S;

/** 
 * @brief: Infrared code library download infrared data transmission callback function
 * @desc: Call back the infrared code base data to the application layer
 * @param[in] data_type:The infrared data type headinf or keyinf
 * @param[in] p_head:Infrared HEAD data
 * @param[in] p_keyinfo:Infrared key data
 * @param[in] keyinfo_num:num of p_keyinfo
 * @param[inout] p_isblock:Whether blocking
 * @param[in] total_num:Total number of remote control keys
 * @param[in] cur_num:Current key offset
 * @return OPERATE_RET
 * @note If it is blocked, call tuya_svc_irlib_dl_post_wait_result() to decide whether to continue execution
 */
typedef OPERATE_RET (*IRLIB_DL_DATA_TRANS_CB)(IN TY_IRLIB_DL_DATA_TYPE_E data_type,IN CHAR_T *p_head,IN TY_IRLIB_DL_IRDATA_INFO_S *p_keyinfo,\
                                                     IN UCHAR_T keyinfo_num,INOUT TY_IRLIB_DL_BLOCK_STAT_E *p_isblock,IN USHORT_T total_num,IN USHORT_T cur_num);

typedef struct
{
    IRLIB_DL_DATA_TRANS_CB irlib_dl_data_cb;
}TY_IRLIB_DL_USER_REG_CB_S;

typedef enum
{
    TY_IRLIB_DL_BLOCK_POST_SUCC = 0,    //Unblock and continue
    TY_IRLIB_DL_BLOCK_POST_FAILED = 1,  //Unblock and exit
}TY_IRLIB_DL_BLOCK_POST_RESULT_E;


/** 
 * @brief: Tuya infrared code library download initialization
 * @desc: Tuya infrared code library download initialization
 * @param[in] p_user_cb: The infrared code library downloads the user callback registration structure
 * @return OPERATE_RET
 * @note none
 */
OPERATE_RET tuya_svc_irlib_dl_init(IN TY_IRLIB_DL_USER_REG_CB_S *p_user_cb);

/** 
 * @brief: Tuya infrared code library download start
 * @desc: Tuya infrared code library download start
 * @param[in] p_sub_devid: Id of an infrared sub-device
 * @return OPERATE_RET
 * @note none
 */
OPERATE_RET tuya_svc_irlib_dl_start(IN CONST CHAR_T *p_sub_devid);

/** 
 * @brief: tuya_svc_irlib_dl_post_wait_result
 * @desc: Notifies the infrared code library download service to continue or exit
 * @param[in] TY_IRLIB_DL_BLOCK_POST_RESULT_E: TY_IRLIB_DL_BLOCK_POST_SUCC :Continue to download
                                               TY_IRLIB_DL_BLOCK_POST_FAILED :Exit the download
 * @return OPERATE_RET
 * @note none
 */
OPERATE_RET tuya_svc_irlib_dl_post_wait_result(IN TY_IRLIB_DL_BLOCK_POST_RESULT_E result);

/** 
 * @brief: Tuya infrared code library download exit
 * @desc: Tuya infrared code library download exit
 * @return OPERATE_RET
 * @note none
 */
OPERATE_RET tuya_svc_irlib_dl_exit(VOID_T);

/** 
 * @brief: Tuya infrared code library download get ir head
 * @desc: Tuya infrared code library download get ir head
 * @param[in] p_sub_devid: Id of an infrared sub-device
 * @param[inout] p_ir_head: Head value of infrared code
 * @return OPERATE_RET
 * @note p_ir_head need to call Free() to free memory
 */
OPERATE_RET tuya_svc_irlib_dl_gethead(IN CHAR_T* p_sub_devid,INOUT CHAR_T **p_ir_head);

/** 
 * @brief: tuya_svc_irlib_dl_get_irdata
 * @desc: Obtain one or more infrared code values based on key or keyid
 * @param[in] p_sub_devid: Id of an infrared sub-device
 * @param[in] p_key_buf: Remote control key value table Json Array Format such as ["M0_T26_S0","power_off"]
 * @param[in] p_keyid_buf: Remote control keyid value table Json Array Format such as [1,42]
 * @param[inout] p_irdata: Infrared code data information structure
 * @param[inout] irdata_num: num of p_irdata
 * @return OPERATE_RET
 * @note p_irdata need to call tuya_svc_irlib_dl_keyinf_free() to free memory
 */
OPERATE_RET tuya_svc_irlib_dl_get_irdata(IN CHAR_T *p_sub_devid,IN CHAR_T *p_key_buf,IN CHAR_T *p_keyid_buf,INOUT TY_IRLIB_DL_IRDATA_INFO_S **p_irdata,INOUT UCHAR_T *irdata_num);

/** 
 * @brief: Tuya_svc_irlib_dl_keyinf_free
 * @desc: Free irdata_inf memory
 * @return OPERATE_RET
 * @note none
 */
OPERATE_RET tuya_svc_irlib_dl_keyinf_free(IN TY_IRLIB_DL_IRDATA_INFO_S *p_irdata,IN UCHAR_T irdata_num);

/* set/get ir lib download thread stack depth */
void set_ir_lib_thread_stack(unsigned int stack_depth);
unsigned int get_ir_lib_thread_stack(void);

/* set/get ir lib download number */
void set_ir_lib_dl_num_max(unsigned char number);
unsigned char get_ir_lib_dl_num_max(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SVC_IRLIB_DOWNLOAD_H__

