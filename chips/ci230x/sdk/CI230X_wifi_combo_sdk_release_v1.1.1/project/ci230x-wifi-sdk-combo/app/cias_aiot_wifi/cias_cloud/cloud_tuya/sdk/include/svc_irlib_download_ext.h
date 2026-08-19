/**
 * @file svc_irlib_download_ext.h
 * @author zhake@tuya.com
 * @brief Tuya Infrared code library download extend
 * @version 1.0.0
 * @date 2021-03-27
 * @copyright Copyright (c) tuya.inc 2021
 */


#ifndef __SVC_IRLIB_DOWNLOAD_EXT_H__
#define __SVC_IRLIB_DOWNLOAD_EXT_H__
 
#include "tuya_cloud_types.h"

 
#ifdef __cplusplus
 extern "C" {
#endif

#define     KT_STAT_REPORT_URL      "tuya.device.infrared.emb.data.report"
#define     KT_STAT_REPORT_VER      "2.0"


typedef struct
{
    BOOL_T power;
    CHAR_T temp;
    CHAR_T mode;
    CHAR_T spd;
    BOOL_T swing;
    
}TY_IRLIB_DL_KT_STAT_S;

/** 
 * @brief: Tuya_svc_irlib_dl_kt_status_rep
 * @desc: Tuya Infrared code library download kt status report
 * @param[in] progress:kt status progress
 * @param[in] p_sub_devid: Id of an infrared sub-device
 * @return OPERATE_RET
 * @note none
 */
OPERATE_RET Tuya_svc_irlib_dl_kt_status_rep(IN CHAR_T *p_sub_devid,TY_IRLIB_DL_KT_STAT_S *p_kt_stat);

/** 
 * @brief: Tuya_svc_irlib_dl_prog_rep
 * @desc: Tuya Infrared code library download progress report
 * @param[in] progress:Download progress
 * @param[in] p_sub_devid: Id of an infrared sub-device
 * @return OPERATE_RET
 * @note none
 */
OPERATE_RET tuya_svc_irlib_dl_prog_rep(IN UCHAR_T progress,IN CHAR_T *p_sub_devid);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SVC_IRLIB_DOWNLOAD_EXT_H__


