#ifndef __TDD_ELE_ENERGY_BL0942_H__
#define __TDD_ELE_ENERGY_BL0942_H__

#include "tuya_cloud_types.h"
#include "tdl_ele_energy_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    UCHAR_T uart_no;    /*串口选择*/
} tdd_bl0942_drv_resource_t;



/**
 * @brief tdd_ele_energy_bl0942_init
 * @param[in] {drv_cfg} 芯片驱动配置参数
 * @param[in] {platform_resource} 驱动芯片所需的外设资源
 * @return OPRT_OK: 注册成功，其他注册失败
 */
OPERATE_RET tdd_ele_energy_bl0942_init(tdl_ele_energy_drv_cfg_t *drv_cfg, tdd_bl0942_drv_resource_t *platform_resource);


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // !__TDD_ELE_ENERGY_BL0942_H__