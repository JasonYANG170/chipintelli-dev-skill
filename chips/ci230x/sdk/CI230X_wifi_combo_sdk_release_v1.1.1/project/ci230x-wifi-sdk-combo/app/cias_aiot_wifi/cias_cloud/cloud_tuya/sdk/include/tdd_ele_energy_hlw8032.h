#ifndef __TDD_ELE_ENERGY_HLW8032_H__
#define __TDD_ELE_ENERGY_HLW8032_H__

#include "tuya_cloud_types.h"
#include "tdl_ele_energy_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /*串口选择*/
    UCHAR_T uart_no;
} tdd_hlw8032_drv_resource_t;

/**
 * @name: tdd_ele_energy_hlw8032_init
 * @msg:  注册hlw8032驱动
 * @param {tdl_ele_energy_drv_cfg_t} *drv_cfg
 * @param {tdd_hlw8032_drv_resource_t} *dev_info
 * @return {*}
 */
OPERATE_RET tdd_ele_energy_hlw8032_init(tdl_ele_energy_drv_cfg_t *drv_cfg, tdd_hlw8032_drv_resource_t *dev_info);


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // !__TDD_ELE_ENERGY_HLW8032_H__