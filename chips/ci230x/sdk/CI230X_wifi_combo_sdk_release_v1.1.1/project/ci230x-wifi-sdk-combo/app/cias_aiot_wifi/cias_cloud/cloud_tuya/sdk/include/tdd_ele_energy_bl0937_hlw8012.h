#ifndef __TDD_ELE_ENERGY_BL0937_HLW8012_H__
#define __TDD_ELE_ENERGY_BL0937_HLW8012_H__

#include "tuya_cloud_types.h"
#include "tdl_ele_energy_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /*硬件定时器号*/
    UCHAR_T timer_no;
    /*功率检测引脚*/
    UCHAR_T epin;
    /*电压、电流检测管脚*/
    UCHAR_T ivpin;
    /*sel脚*/
    tdl_ele_energy_drv_io_cfg_t ivcpin;
} tdd_bl0937_hlw8012_drv_resource_t;

/**
 * @brief tdd_ele_energy_bl0937_init
 * @param[in] {drv_cfg} 芯片驱动配置参数
 * @param[in] {platform_resource} 驱动芯片所需的外设资源
 * @return OPRT_OK: 注册成功，其他注册失败
 */
OPERATE_RET tdd_ele_energy_bl0937_init(IN CONST tdl_ele_energy_drv_cfg_t *drv_cfg, IN CONST tdd_bl0937_hlw8012_drv_resource_t *platform_resource);

/**
 * @brief tdd_ele_energy_hlw8012_init
 * @param[in] {drv_cfg} 芯片驱动配置参数
 * @param[in] {platform_resource} 驱动芯片所需的外设资源
 * @return OPRT_OK: 注册成功，其他注册失败
 */
OPERATE_RET tdd_ele_energy_hlw8012_init(IN CONST tdl_ele_energy_drv_cfg_t *drv_cfg, IN CONST tdd_bl0937_hlw8012_drv_resource_t *platform_resource);


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // !__TDD_ELE_ENERGY_BL0937_HLW8012_H__
