#ifndef  __TFM_ELE_ENERGY_FAULT_H__
#define  __TFM_ELE_ENERGY_FAULT_H__


#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    __TFM_ELE_FAULT_OVER_CURR = (1 << 0), // 过流
    __TFM_ELE_FAULT_OVER_VOLT = (1 << 1), // 过压
    __TFM_ELE_FAULT_OVER_PWR  = (1 << 2), // 过功率
    __TFM_ELE_FAULT_LESS_CURR = (1 << 3), // 欠流
    __TFM_ELE_FAULT_LESS_VOLT = (1 << 4), // 欠压
    __TFM_ELE_FAULT_LESS_PWR  = (1 << 5) // 欠功率
} tfm_ele_energy_fault_code_t;/*故障类型最多15种*/

typedef enum {
    __TFM_ELE_FAULT_EVT_START = 0,
    __TFM_ELE_FAULT_EVT_END
} tfm_ele_energy_fault_status_t;

//事件运行参数
typedef struct
{
    BOOL_T sw; //检测使能控制
    UCHAR_T fault_judge_time; // 单位s
    UINT_T  fault_judge_threshold;
    tfm_ele_energy_fault_code_t fault_event;
    VOID_T (*fault_event_cb)(tfm_ele_energy_fault_status_t evt_status); // TODO 考虑所有事件回调都用一个，增加对应事件码
} tfm_ele_energy_fault_params_t;

/**
 * @brief 故障事件注册
 *        支持重复注册，后面注册的会覆盖前面的注册信息
 * @param {event_list} 被注册的事件列表
 * @param {event_num} 被注册的事件数量
 * @return OPERATE_RET {OPRT_OK}:注册成功 {other code}: 注册失败
 */
OPERATE_RET tfm_ele_energy_fault_event_register(tfm_ele_energy_fault_params_t event_list[], UCHAR_T event_num);

/**
 * @brief 读故障所有故障码
 * @param [in] {VOID_T} 
 * @return 故障码，对应tfm_ele_energy_fault_code_t里事件位;
 * @return 例如要判断：__TFM_ELE_FAULT_OVER_VOLT事件是否发生，只需 fault_code & (1 << __TFM_ELE_FAULT_OVER_VOLT)后检查是否为1
 */
UINT_T tfm_ele_energy_read_fault_code(VOID_T);

/**
 * @brief tfm_ele_energy_set_fault_judge_sw
 * @param {fault_type} 要设置的故障码集合，支持多路故障同时开启关闭
 * @param {state} 开关设置，TRUE:开启，FALSE:关闭
 * @return VOID_T 
 */
VOID_T tfm_ele_energy_set_fault_judge_sw(tfm_ele_energy_fault_code_t fault_type, BOOL_T state);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__TFM_ELE_ENERGY_FAULT_H__*/