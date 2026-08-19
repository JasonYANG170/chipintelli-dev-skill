#ifndef __TFM_ELE_ENERGY_FAC_TEST_H__
#define __TFM_ELE_ENERGY_FAC_TEST_H__

#include "tuya_cloud_types.h"
#include "tdl_ele_energy_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief tfm_ele_energy_fac_test
 *        电量统计出厂测试处理
 * @return OPERATE_RET {OPRT_OK}:产测处理成功,{other err code}:产测处理失败
 */
OPERATE_RET tfm_ele_energy_fac_test(VOID_T);

/**
 * @brief tfm_ele_energy_read_fac_test_result
 *        获取产测结果
 * @return BOOL_T {TRUE}:产测成功 {FALSE}:产测失败
 */
BOOL_T tfm_ele_energy_read_fac_test_result(VOID_T);

/**
 * @brief tfm_ele_energy_read_fac_test_data
 *        电量统计出厂校准参数初始化
 * @brief {fac_test_data}: 产测数据
 * @return OPERATE_RET {OPRT_OK}:初始化成功,{other err code}:初始化失败
 */
OPERATE_RET tfm_ele_energy_read_fac_test_data(tdl_ele_energy_drv_fac_test_t *fac_test_data);

/**
 * @brief tfm_ele_energy_clear_fac_test_data
 *        清校准参数
 * @return
 */
VOID_T tfm_ele_energy_clear_fac_test_data(VOID_T);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // !1