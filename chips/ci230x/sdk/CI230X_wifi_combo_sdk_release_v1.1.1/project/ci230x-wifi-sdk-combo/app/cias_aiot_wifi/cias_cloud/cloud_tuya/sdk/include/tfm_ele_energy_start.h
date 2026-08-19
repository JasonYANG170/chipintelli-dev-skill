#ifndef  __TFM_ELE_ENERGY_APP_H__
#define  __TFM_ELE_ENERGY_APP_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /*电压变化率：内部默认2%，内部数据源扩大10倍；范围：1%~100%*/
    UCHAR_T volt_threshold;
    /*功率变化率：内部默认20%，内部数据源扩大10倍；范围：1%~100%*/
    UCHAR_T pwr_threshold;
    /*长定时周期: 单位s，内部默认1h；可设范围：1min(60s)~24h(86400)*/
    UINT_T  long_period;
} tfm_ele_energy_pvi_rp_params_t;

typedef struct {
    UINT_T volt;
    UINT_T curr;
    UINT_T pwr;
} tfm_ele_energy_pvi_t;

typedef struct {
    OPERATE_RET (*pvi_data_inform)(CONST tfm_ele_energy_pvi_t *pvi_data);// pvi上报回调
    OPERATE_RET (*energy_data_inform)(IN UINT_T time, IN UINT_T ele_value);// 电能数据上报
} tfm_ele_energy_inform_cb_t;

/**
 * @brief 过冲事件触发回调
 */
typedef VOID_T (*tfm_ele_energy_ov_charge_cb_t)(VOID_T);


/**
 * @brief tfm_ele_energy_start
 * @param[in] {user_define_interval} 上报间隔设置，如果采用内部默认，直接填NULL
 * @param[in] {ele_energy_data_inform_cb} 应用上报接口注册
 * @return OPERATE_RET {OPRT_OK}:初始化成功
 */
OPERATE_RET tfm_ele_energy_start(CONST tfm_ele_energy_pvi_rp_params_t *user_define_interval, CONST tfm_ele_energy_inform_cb_t *ele_energy_data_inform_cb);
/**
 * @brief: tfm_ele_energy_clear_all_energy
 *         清除所有电量，当前临时电量和带时标的电量
 * @param {VOID_T}
 * @return {VOID_T}
 */
OPERATE_RET tfm_ele_energy_clear_all_energy(VOID_T);
/**
 * @brief 过充注册
 * @param {overcharge_event_cb} 过充事件回调
 * @return OPERATE_RET 
 */
OPERATE_RET tfm_ele_energy_overcharge_register(tfm_ele_energy_ov_charge_cb_t overcharge_event_cb);
/**
 * @brief 设置过冲状态
 * @param {state} 设置过冲状态
 * @return OPERATE_RET 
 */
OPERATE_RET tfm_ele_energy_overcharge_sw_set(BOOL_T state);
/**
 * @brief 删除过冲参数
 * @param {VOID_T}
 * @return OPERATE_RET
 */
OPERATE_RET tfm_ele_energy_delete_overcharge(VOID_T);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__TFM_ELE_ENERGY_APP_H__*/
