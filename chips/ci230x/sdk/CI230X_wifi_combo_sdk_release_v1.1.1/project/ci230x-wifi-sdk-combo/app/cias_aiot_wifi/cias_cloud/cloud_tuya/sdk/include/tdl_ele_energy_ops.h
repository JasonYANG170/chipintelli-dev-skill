#ifndef __TDL_ELE_ENERGY_OPS_H__
#define __TDL_ELE_ENERGY_OPS_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 220V 产测参考电压、电流、功率 */
#define __TDL_ELE_V_REF_220 2200
#define __TDL_ELE_I_REF_220 392
#define __TDL_ELE_P_REF_220 864

/* 120V 产测参考电压、电流、功率 */
#define __TDL_ELE_V_REF_120 1200
#define __TDL_ELE_I_REF_120 833
#define __TDL_ELE_P_REF_120 1000

typedef UCHAR_T tdl_ele_energy_test_result_t;
#define __TDL_ELE_TEST_PASS 1
#define __TDL_ELE_TEST_FAIL 2

typedef enum {
    __TDL_ELE_220V = 0,
    __TDL_ELE_120V = 1
} tdl_ele_energy_volt_type_t;

typedef enum {
    __TDL_ELE_NORMAL_MODE = 0,
    __TDL_ELE_TEST_MODE,
} tdl_ele_energy_mode_t;

typedef enum {
    __TDL_ELE_IO_DRV_LOW, // 低电平有效
    __TDL_ELE_IO_DRV_HIGH, // 高电平有效
    __TDL_ELE_IO_DRV_ERR // 该IO不存在
} tdl_ele_energy_drv_io_type_t;

typedef struct {
    tdl_ele_energy_drv_io_type_t type; // 有效电平类型
    UCHAR_T pin; // 引脚号
} tdl_ele_energy_drv_io_cfg_t;

typedef struct {
    UCHAR_T err_percent; // 产测通过范围，0：完全一致； 10: 采样和参考值允许10%误差; 最大100%
    UCHAR_T resval; // 电阻规格    1\2 毫欧
    tdl_ele_energy_volt_type_t volt_type;
} tdl_ele_energy_params_t;

/*校准参数v,i,p,e*/
typedef struct {
    UINT_T v_coe;
    UINT_T i_coe;
    UINT_T p_coe;
    UINT_T e_coe;
} tdl_ele_energy_coe_t;

typedef struct {
    UINT_T power_val;
    UINT_T volt_val;
    UINT_T curr_val;
    UINT_T energy_val;
} tdl_ele_energy_pvie_t;

typedef struct {
    tdl_ele_energy_test_result_t test_result;
    tdl_ele_energy_coe_t coe_data; // 默认的校准数据
} tdl_ele_energy_drv_fac_test_t;

typedef struct {
    tdl_ele_energy_drv_fac_test_t fac_test;
    tdl_ele_energy_params_t params;
    tdl_ele_energy_mode_t mode;
} tdl_ele_energy_drv_cfg_t;

typedef struct {
    OPERATE_RET (*drv_init)(IN CONST tdl_ele_energy_drv_cfg_t *drv_cfg, IN CONST VOID_T *platform_resource);
    VOID_T (*drv_ctrl)(BOOL_T state);
    VOID_T (*drv_read_pvie_value)(OUT tdl_ele_energy_pvie_t *pvie_val);
    VOID_T (*drv_read_fac_test_data)(OUT tdl_ele_energy_drv_fac_test_t *fac_test_data);
    BOOL_T (*drv_read_fac_test_state)(VOID_T);
} tdl_ele_energy_drv_ops_t;

typedef struct {
    CHAR_T *name; // 驱动名
    tdl_ele_energy_drv_ops_t *ops; // 驱动操作
    tdl_ele_energy_drv_cfg_t *cfg; // 驱动参数配置
    VOID_T *platform_resource; // 平台资源配置
} tdl_ele_energy_driver_t;

/**
 * @brief tdl_ele_energy_driver_register
 * @param {chip_info} 芯片驱动信息
 * @return OPERATE_RET 
 */
OPERATE_RET tdl_ele_energy_driver_register(tdl_ele_energy_driver_t *chip_info);
/*
 * @brief tdl_ele_energy_ctrl
 *        计量芯片控制
 * @param[in] {state} {TRUE}：开启计量采集 {FASLE}:关闭计量采集
 * @return VOID_T 
 */
OPERATE_RET tdl_ele_energy_ctrl(BOOL_T state);

/* @brief tdl_ele_energy_read_fac_test_data
 *        读产测校准数据
 * @param[in] {fac_test_data} 产测数据
 * @return VOID_T 
 */
OPERATE_RET tdl_ele_energy_read_fac_test_data(OUT tdl_ele_energy_drv_fac_test_t *fac_test_data);
/**
 * @brief 读产测状态
 * @return {BOOL_T} TRUE:产测结束 FASLE:产测未开始或未结束
 */
BOOL_T tdl_ele_energy_read_fac_test_state(VOID_T);
/**
 * @brief tuya_ele_energy_adapt_get_pvi_value
 *        获取电压，电流，功率采样数据;如果仅需要其中某一项参数，别的传参填NULL即可
 * @param[in]
 * @return VOID_T 
 */
VOID_T tdl_ele_energy_read_pvi_value(OUT UINT_T *pwr, OUT UINT_T *volt, OUT UINT_T *curr);

/**
 * @brief tuya_ele_energy_adapt_get_enery_value
 *        获取累计电量值
 * @param[in]
 * @return VOID_T 
 */
VOID_T tdl_ele_energy_read_enery_value(OUT UINT_T *enry);

/**
 * @brief tdl_ele_energy_rang_judge
 *        范围判断
 * @param[in] {src_value}：源值
 * @param[in] {tag_value}：目标值
 * @param[in] {range}：范围值
 * @return BOOL_T TRUE:范围内，FASLE:范围外
 */
BOOL_T tdl_ele_energy_rang_judge(IN UINT_T src_value, IN UINT_T tag_value, IN UINT_T range);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif