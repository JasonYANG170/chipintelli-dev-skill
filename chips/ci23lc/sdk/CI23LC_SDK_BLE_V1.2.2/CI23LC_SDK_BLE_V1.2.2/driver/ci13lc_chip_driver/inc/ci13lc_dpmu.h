#ifndef CI13LC_DPMU_H
#define CI13LC_DPMU_H

#include "ci_system.h"
#include "ci_scu.h"

typedef enum
{
    DPMU_IO_PULL_DISABLE = 0,
    DPMU_IO_PULL_UP     = 1,
    DPMU_IO_PULL_DOWN   = 2,
}Dpmu_Io_Pull_t;

typedef enum
{
    DPMU_IO_DIRECTION_INPUT = 0,
    DPMU_IO_DIRECTION_OUTPUT = 1,
}Dpmu_Io_Direction_t;

typedef enum
{
    DPMU_LOWPOWER_NO_MODE = 0,
    DPMU_LOWPOWER_SLEEP_MODE = 1,
    DPMU_LOWPOWER_DEEP_SLEEP_MODE = 2,
	DPMU_LOWPOWER_BOTH_MODE = 3,
}Dpmu_Lowpower_Mode_t;

/**
 * @brief 系统退出低功耗进入系统复位流程时是否复位
 *
 */
typedef enum
{
    DPMU_WAKEUP_RESET_GPIO2            = 0, //aon gpio模块
    DPMU_WAKEUP_RESET_IWDG             = 1, //iwdg模块
    DPMU_WAKEUP_RESET_EFUSE            = 2, //efuse ctrl模块
    DPMU_WAKEUP_RESET_TIMER_PWM        = 3, //timer0/1_gpwm0/1模块
    DPMU_WAKEUP_RESET_IO_REG           = 4, //IO相关控制寄存器
    DPMU_WAKEUP_RESET_PMU_RC           = 5, //PMU及RC相关控制寄存器
}Dpmu_Wakeup_Reset_Cfg_t;

/**
 * @brief 唤醒源
 *
 */
typedef enum
{
    DPMU_WAKEUP_SRC_V11_OK         = 2, //V11_OK
	DPMU_WAKEUP_SRC_VDT            = 3, //VDT
    DPMU_WAKEUP_SRC_IWDG           = 6, //IWDG
    DPMU_WAKEUP_SRC_GPIO2          = 7, //aon gpio
}Dpmu_Wakeup_SRC_t;

/**
 * @brief 晶振功能
 *
 */
typedef enum
{
    DPMU_XTAL_EN      = 10,    //晶振功能开始使能
    DPMU_XTAL_BYP     = 9,     //晶振BYPASS
    DPMU_XTAL_RF_EN   = 8,     //晶振反馈使能
}Dpmu_Xtal_Mode_t;

/**
 * @brief IO触发器选择
 *
 */
typedef enum
{
    DPMU_IO_SCHMITT_TRIGGER_DISABLE = 0,  //普通触发器
    DPMU_IO_SCHMITT_TRIGGER_ENABLE = 1,   //施密特触发器
}Dpmu_Io_Schmitt_Trigger_t;

/**
 * @brief IO电压转换率模式
 *
 */
typedef enum
{
    DPMU_IO_SLEW_RATE_SLOW = 0,    //转换速率慢
    DPMU_IO_SLEW_RATE_FAST = 1,    //转换速率快
}Dpmu_Io_Slew_Rate_t;

/**
 * @brief IO驱动强度（数字越大，驱动能力越强）
 *
 */
typedef enum
{
    DPMU_IO_DRIVER_STRENGTH_0 = 0,
    DPMU_IO_DRIVER_STRENGTH_1 = 1,
    DPMU_IO_DRIVER_STRENGTH_2 = 2,
    DPMU_IO_DRIVER_STRENGTH_3 = 3,
}Dpmu_Io_Driver_Strength_t;

/**
 * @brief SRC时钟源选择
 * 
 */
typedef enum
{
    /*系统默认选择，PAD，以及efuse选择*/
    DPMU_SRC_USE_SYSTEM_DEFAULT = 0,
    /*SRC时钟来源于内部RC*/
    DPMU_SRC_USE_INNER_RC = 1,
    /*系统默认选择*/
    DPMU_SRC_USE_SYSTEM_DEFAULT_ELSE = 2,
    /*SRC时钟来源于外部OSC*/
    DPMU_SRC_USE_OUTSIDE_OSC = 3,
}Dpmu_Src_Source_Sel_t;

/**
 * @brief 系统工作时钟选择
 * 
 */
typedef enum
{
    DPMU_SYS_CLK_SRC = 0,
    DPMU_SYS_CLK_PLL = 1,
}Dpmu_Sys_Clk_Sel_t;


/**
 * @brief PMU配置update的更新项
 * 
 */
typedef enum
{
    DPMU_UPDATE_EN_LDO1 = 0,
    DPMU_UPDATE_EN_LDO3 = 2,
    DPMU_UPDATE_EN_VDT  = 3,
    DPMU_UPDATE_EN_TRIM = 4,
}Dpmu_Update_En_t;

/**
 * @brief 测试时钟选择
 * 
 */
typedef enum
{
    /*测试时钟来源于外部晶振*/
    DPMU_TEST_CLK_EXT_OSC  = 0,
    /*测试时钟来源于内部晶振*/
    DPMU_TEST_CLK_INTER_RC = 1,
    /*测试时钟来源于PLL*/
    DPMU_TEST_CLK_PLL      = 2,
}Dpmu_Test_Clk_Sel_t;

/**
 * @brief IWDG时钟选择
 * 
 */
typedef enum
{
    /*测试时钟来源于外部晶振*/
    DPMU_IWDG_CLK_EXT_OSC  = 1,
    /*测试时钟来源于内部晶振*/
    DPMU_IWDG_CLK_INTER_RC = 0,
}Dpmu_Iwdg_Clk_Sel_t;

/**
 * @brief 解锁dpmu配置寄存器
 */
void dpmu_unlock_cfg_config(void);

/**
 * @brief 锁定dpmu配置寄存器
 */
void dpmu_lock_cfg_config(void);

/**
 * @brief 配置管脚复用对应功能
 * @param pin， 管脚名称
 * @param io_function， 第 X 功能选择
 */
void dpmu_set_io_reuse(PinPad_Name pin,IOResue_FUNCTION io_function);

/**
 * @brief 配置管脚数字模拟功能
 * @param pin 管脚名
 * @param adio_mode 数字/模拟，功能选择
 */
void dpmu_set_adio_reuse(PinPad_Name pin,ADIOResue_MODE adio_mode);

/**
 * @brief 配置管脚开漏功能（例如IIC需要引脚配置成此功能）
 * @param pin， 管脚名
 * @param cmd，ENABLE 使能开漏功能，DISABLE，不使能开漏功能
 */
void dpmu_set_io_open_drain(PinPad_Name pin,FunctionalState cmd);

/**
 * @brief 配置管脚上下拉功能
 * @param pin，管脚名
 * @param pull，关闭上下拉、开上拉，开下拉
 */
void dpmu_set_io_pull(PinPad_Name pin,Dpmu_Io_Pull_t pull);

/**
 * @brief 配置管脚方向
 * @param pin，管脚名
 * @param dir，输入（IE为1）、输出（IE为0）
 */
void dpmu_set_io_direction(PinPad_Name pin,Dpmu_Io_Direction_t dir);

/**
 * @brief 配置管脚电压转换率模式
 * @param pin， 管脚名
 * @param slew_rate，电压转换率slow、fast
 */
void dpmu_set_io_slew_rate(PinPad_Name pin,Dpmu_Io_Slew_Rate_t slew_rate);

/**
 * @brief 配置管脚触发器模式
 * @param pin， 管脚名
 * @param schmitt_trigger，触发器模式normal、schmitt trigger
 */
void dpmu_set_io_schmitt_trigger(PinPad_Name pin,Dpmu_Io_Schmitt_Trigger_t schmitt_trigger);

/**
 * @brief 配置管脚驱动强度
 * @param pin， 管脚名
 * @param driver_strength，驱动强度级别选择
 */
void dpmu_set_io_driver_strength(PinPad_Name pin,Dpmu_Io_Driver_Strength_t driver_strength);

/**
 * @brief 配置晶振脚PA0、PA1功能选择（晶振/GPIO）
 * 
 * @param en: ENABLE（GPIO功能），DISABLE（晶振功能）
 */
void dpmu_osc_pad_for_gpio(FunctionalState en);

/**
 * @brief 测试时钟来源选择，可通过PAD查看
 * 
 * @param src 来源
 */
void dpmu_test_clk_sel(Dpmu_Test_Clk_Sel_t src);

/**
 * @brief IWDG时钟来源选择
 * 
 * @param src 来源
 */
void dpmu_iwdg_clk_sel(Dpmu_Iwdg_Clk_Sel_t src);


/**
 * @brief 配置dpmu_pll
 * @note 注意可选时钟有效性
 * 
 * @param in_clk pll输入时钟频率
 * @param out_clk pll输出时钟频率
 */
void dpmu_pll_config(uint32_t in_clk, uint32_t out_clk);

//复位系统、复位总线、不复位配置
void dpmu_iwdg_reset_none_config(void);
void dpmu_iwdg_reset_system_config(void);
void dpmu_iwdg_reset_bus_config(void);
void dpmu_software_reset_none_config(void);
void dpmu_software_reset_system_config(void);
void dpmu_software_reset_bus_config(void);
void dpmu_core_reset_none_config();
void dpmu_core_reset_system_config();
void dpmu_core_reset_bus_config();

//LDO配置
void dpmu_ldo1_lv_set(uint8_t lv);
void dpmu_ldo3_lv_set(uint8_t lv);
void dpmu_ldo3_en(bool en);
void dpmu_enter_lowpower_ldo1_lv(uint8_t lv);
void dpmu_enter_lowpower_ldo3_lv(uint8_t lv);
void dpmu_enter_lowpower_ldo3_en(bool en);
void dpmu_exit_lowpower_ldo1_lv(uint8_t lv);
void dpmu_exit_lowpower_ldo3_lv(uint8_t lv);
void dpmu_exit_lowpower_ldo3_en(bool en);
void dpmu_set_pmu_update_en(Dpmu_Update_En_t num);
void dpmu_set_ldo_mask(bool en);

//RC配置
void dpmu_set_rc_trim_c_value(uint8_t val);
void dpmu_set_rc_trim_f_value(uint8_t val);
void dpmu_set_rc_en(bool en);
void dpmu_set_rc_update_cfg(void);

//晶振IO频率和驱动能力选择：0~7
void dpmu_osc_pad_cfg_fma(uint8_t num);
void dpmu_osc_pad_cfg_en(Dpmu_Xtal_Mode_t mode,FunctionalState cmd);

//PLL配置
void dpmu_pll_12d_config(uint32_t clk);
uint32_t dpmu_get_pll_frequency();

//SRC时钟源配置
void dpmu_set_src_source(Dpmu_Src_Source_Sel_t sel);
//选择系统工作时钟
void dpmu_sys_clk_sel_cfg(Dpmu_Sys_Clk_Sel_t sel);

//清除复位状态
void dpmu_clean_reset_state(void);

void dpmu_set_low_power_mode(Dpmu_Lowpower_Mode_t mode);
void dpmu_set_wakeup_int(Dpmu_Wakeup_SRC_t wake_int_num,FunctionalState flag);
void dpmu_set_vdt_mask(bool en);
uint32_t dpmu_get_wakeup_state(void);
void dpmu_wakeup_reset_cfg(Dpmu_Wakeup_Reset_Cfg_t model, FunctionalState flag);
void dpmu_use_rc(void);
int32_t dpmu_set_div_parameter(uint32_t device_base,uint32_t div_num);
void dpmu_set_ext_filter_config(Ext_Num num,FunctionalState flag,uint32_t param);
void dpmu_set_iwdg_halt();
void dpmu_clean_iwdg_halt();

#endif /*CI13LC_DPMU_H*/
