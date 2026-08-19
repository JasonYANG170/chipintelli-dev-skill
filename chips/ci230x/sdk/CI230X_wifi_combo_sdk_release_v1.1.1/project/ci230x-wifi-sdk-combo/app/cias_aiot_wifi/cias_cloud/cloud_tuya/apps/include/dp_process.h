/**
* @file dp_process.h
* @author www.tuya.com
* @brief 
* @version 0.1
* @date 2021-08-19
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __DP_PROCESS_H__
#define __DP_PROCESS_H__

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "cias_demo_config.h"
#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#if  AIR_CONDITION_COMPANION_ENABLE     //空调伴侣demo
#define DPID_LIB_SYNC   50 //raw
#define DPID_LIB_RATE   51 //value
#define DPID_LIB_INFO   52 //raw
#elif   INFRARED_REMOTE_CONTROL_ENABLE  //红外遥控器demo
#define DPID_LIB_SYNC   1 //raw
#define DPID_LIB_RATE   2 //value
#define DPID_LIB_INFO   3 //raw
#endif
#define DPID_IR_SEND    201 //staring
#define DPID_IR_STUDY   202 //raw

#define IR_CODE_DATA_MAX_LEN   512          //每条红外码库数据最大长度

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

struct remote_info {
    unsigned char type;
    unsigned int remote_id;
    unsigned char dev_id[32];
};

/***********************************************************
***********************function define**********************
***********************************************************/

/**
* @brief upload all dp data
*
* @return none
*/
VOID_T update_all_dp(VOID_T);

/**
* @brief handle dp commands from the cloud
*
* @param[in] root: pointer header for dp data
* @return none
*/
VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root);
void raw_dp_proc(IN CONST TY_RECV_RAW_DP_S *raw_dp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DP_PROCESS_H__ */
