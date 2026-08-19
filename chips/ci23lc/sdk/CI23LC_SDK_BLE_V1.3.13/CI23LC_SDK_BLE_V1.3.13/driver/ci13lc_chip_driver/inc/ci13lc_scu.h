/**
 * @file ci13lc_scu.h
 * @brief 三代芯片scu底层驱动接口头文件
 * @version 0.1
 * @date 2021-07-05
 * 
 * @copyright Copyright (c) 2021  Chipintelli Technology Co., Ltd.
 * 
 */

#ifndef CI13LC_SCU_H
#define CI13LC_SCU_H

#include <stdbool.h>
#include "ci_system.h"
#include "sdk_default_config.h"

#ifdef __cplusplus
extern "C" {
#endif 


typedef enum
{
    NMI_IRQ_IWDG    = 0x1,
    NMI_IRQ_EXT0    = 0x3,
    NMI_IRQ_EXT1    = 0x4,
    NMI_IRQ_TIMER0  = 0x5,
    NMI_IRQ_TIMER1  = 0x6,
    NMI_IRQ_UART0   = 0x7,
    NMI_IRQ_UART1   = 0x8,
    NMI_IRQ_UART2   = 0x9,
    NMI_IRQ_PA      = 0xA,
    NMI_IRQ_PB      = 0xB,
    NMI_IRQ_PC      = 0xC,
    NMI_IRQ_VDT     = 0xD,
    NMI_IRQ_V11_OK  = 0xE,
}Nmi_Irq_t;

typedef enum
{
    SLEEPING_GATE    = 0,
    SLEEPDEEP_GATE   = 1,
    CPU_CORECLK_GATE = 2,
    STCLK_GATE       = 3,
    SRAM0_GATE       = 4,
    SRAM1_GATE       = 5,
    SRAM2_GATE       = 6,
    SRAM3_GATE       = 7,
    SRAM4_GATE       = 8,
    SRAM5_GATE       = 9,
    ROM_GATE         = 11,
}Sys_Clk_Gate_t;

typedef enum
{
    WAKRUP_MASK_EXT0    = 1,
    WAKRUP_MASK_EXT1    = 2,
    WAKRUP_MASK_IWDG    = 3,
    WAKRUP_MASK_TIMER0  = 5,
    WAKRUP_MASK_TIMER1  = 6,
    WAKRUP_MASK_UART0   = 7,
    WAKRUP_MASK_UART1   = 8,
    WAKRUP_MASK_UART2   = 9,
    WAKRUP_MASK_GPIO0   = 10,
    WAKRUP_MASK_GPIO1   = 11,
    WAKRUP_MASK_GPIO2   = 12,
    WAKRUP_MASK_IIS0    = 13,
    WAKRUP_MASK_VDT     = 14,
    WAKRUP_MASK_V11_OK  = 15,
    WAKRUP_MASK_IISDMA  = 18,
}Wakeup_Mask_t;

typedef enum
{
    FIRST_FUNCTION = 0,
    SECOND_FUNCTION = 1,
    THIRD_FUNCTION = 2,
    FORTH_FUNCTION = 3,
    FIFTH_FUNCTION = 4,
    SIXTH_FUNCTION = 5,
    SEVENTH_FUNCTION = 6,
}IOResue_FUNCTION;

typedef enum
{
    DIGITAL_MODE = 0,
    ANALOG_MODE = 1,
}ADIOResue_MODE;

/**
 * @brief IIS号选择
 *
 */
typedef enum
{
    //!IIS0
    IISNum0    = 0,
    //!IIS1_RX
    IISNum1_RX = 1,
    //!IIS1_TX
    IISNum1_TX = 2,
}IISNumx;

/**
 * @brief IIS主从模式选择
 *
 */
typedef enum
{
    //!IIS做SLAVE
    IIS_SLAVE = 1,
    //!IIS做MASTER
    IIS_MASTER = 0,
}IIS_Mode_Sel_t;

/**
 * @brief IIS MCLK 输入/输出
 *
 */
typedef enum
{
    IIS_MCLK_MODENULL = 0,
    IIS_MCLK_IN = 1,
    IIS_MCLK_OUT = 2,
}IIS_Mclk_Mode_t;

/**
 * @brief IIS SCK和LRCK 输入/输出
 *
 */
typedef enum
{
    IIS_SCKLRCK_MODENULL = 0,
    IIS_SCKLRCK_IN = 1,
    IIS_SCKLRCK_OUT = 2,
}IIS_SckLrck_Mode_t;

/**
 * @brief QSPI控制器模式
 *
 */
typedef enum
{
    //!BOOT模式
    QSPI_MODE_BOOT = 1,
    //!正常模式
    QSPI_MODE_NOMAL = 0,
}Qspi_Mode_t;

/**
 * @brief DTRFLASH时钟来源选择
 *
 */
typedef enum
{
    //!PLL倍频前的时钟
    DTR_CLK_SEL_SRC_CLK = 0,
    //!PLL倍频后的时钟
    DTR_CLK_SEL_PLL_CLK  = 1,
}Dtr_Clk_Sel_t;

/**
 * @brief MCLK 时钟源SRC的 来源选择
 *
 */
typedef enum
{
    //!来源于 IPCORE
    IIS_SRC_SOURCE_IPCORE   = 0,
    //!来源于 EXT_OSC
    IIS_SRC_SOURCE_EXT_OSC  = 1,
    //!来源于 INTERNAL RC
    IIS_SRC_SOURCE_INTER_RC = 2,
    //!来源于 PAD IN
    IIS_SRC_SOURCE_PAD_IN   = 3,
}IIS_Src_Source_t;

/**
 * @brief IIS MCLK 过采样率选择
 *
 */
typedef enum
{
    //!过采样率128
    IIS_MCLK_FS_128   = 0,
    //!过采样率192
    IIS_MCLK_FS_192   = 1,
    //!过采样率256
    IIS_MCLK_FS_256   = 2,
    //!过采样率384
    IIS_MCLK_FS_384   = 3,
}IIS_Mclk_Fs_t;

/**
 * @brief IIS SCK和LRCK的频率关系比值
 *
 */
typedef enum
{
    //!SCK/LRCK=32
    IIS_SCK_LRCK_WID_32  = 0,
    //!SCK/LRCK=64
    IIS_SCK_LRCK_WID_64  = 1,
}IIS_Sck_Lrck_Wid_t;

/**
 * @brief MCLK 时钟来源选择
 *
 */
typedef enum
{
    //!来源于 SRC0
    IIS_MCLK_SOURCE_SRC0    = 0,
    //!来源于 SRC1
    IIS_MCLK_SOURCE_SRC1    = 1,
    //!来源于 PAD IN
    IIS_MCLK_SOURCE_PAD_IN  = 3,
    //!做输入时，无输出来源
    IIS_MCLK_SOURCE_NONE    = 4,
}IIS_Mclk_Source_t;

/**
 * @brief MCLK 选择
 *
 */
typedef enum
{
    //!MCLK0
    IIS_MCLK_MCLK0    = 0,
    //!MCLK1
    IIS_MCLK_MCLK1    = 1,
}IIS_Mclk_t;

/**
 * @brief IIS SCK/LRCK输出来源
 *
 */
typedef enum
{
    //!MCLK0产生的SCK/LRCK
    IIS_CLK_SOURCE_MCLK0     = 0,
    //!MCLK1产生的SCK/LRCK
    IIS_CLK_SOURCE_MCLK1     = 1,
    //!PAD输入的SCK/LRCK
    IIS_CLK_SOURCE_PAD_IN    = 3,
    //!codec_ad_sck_in
    IIS_CLK_SOURCE_CODEC_AD  = 4,
    //!codec_da_sck_in
    IIS_CLK_SOURCE_CODEC_DA  = 5,
    //!做输入时，无输出选择
    IIS_CLK_SOURCE_NONE      = 6,
}IIS_Clk_Source_t;

/**
 * @brief codec dac IIS数据来源选择
 *
 */
typedef enum
{
    //!IIS1_TX
    CODEC_DAC_DATA_FROM_IIS1_TX   = 0,
    //!PAD_IN
    CODEC_DAC_DATA_FROM_PAD_IN    = 1,
    //!CODEC_ADC
    CODEC_DAC_DATA_FROM_CODEC_ADC = 2,
}Codec_Dac_Data_Sel_t;

/**
 * @brief pad IIS输出数据来源选择
 *
 */
typedef enum
{
    //!IIS0_TX
    PAD_IIS_DATA_FROM_IIS0_TX   = 0,
    //!IIS1_TX
    PAD_IIS_DATA_FROM_IIS1_TX   = 1,
    //!CODEC_ADC
    PAD_IIS_DATA_FROM_CODEC_ADC = 2,
}Pad_IIS_Data_Sel_t;

/**
 * @brief IIS 输入输出选择
 *
 */
typedef enum
{
    //!输出
    IIS_CLK_MODE_OUTPUT    = 0,
    //!输入
    IIS_CLK_MODE_INPUT     = 1,
}IIS_Clk_Mode_t;

/**
 * @brief CODEC AD/DA 选择
 *
 */
typedef enum
{
    //!AD
    CODEC_CHANNEL_AD    = 0,
    //!DA
    CODEC_CHANNEL_DA    = 1,
}Codec_Channel_t;

/**
 * @brief IIS 时钟源 SRC配置结构体
 * 
 */
typedef struct
{
    IIS_Src_Source_t source;     //!SRC SOURCE 来源选择
    uint32_t source_div;         //!分频参数
}IIS_Src_Config_t;

/**
 * @brief IIS MCLK配置结构体
 * 
 */
typedef struct
{
    IIS_Mclk_t mclk;             //!MCLK 编号选择
    IIS_Mclk_Source_t src;       //!MCLK 来源 SRC 编号选择
    IIS_Mclk_Fs_t fs;            //!MCLK 过采样率选择
    IIS_Sck_Lrck_Wid_t sck_lrck; //!SCK/LRCK 比值
}IIS_Mclk_Config_t;

typedef struct
{
    IISNumx device_select;       //0：iis0 1：iis1_tx 2：iis_rx
    IIS_Mode_Sel_t model_sel;     //主从模式
    IIS_Src_Config_t src_cfg;    //时钟来源SRC配置
    IIS_Mclk_Config_t mclk_cfg;  //MCLK配置
    IIS_Clk_Source_t clk_cfg;    //sck/lrck来源
    IIS_Mclk_Mode_t mclk_mode;   //mclk从PAD输出/输入
    IIS_SckLrck_Mode_t clk_mode; //sck和lrck从PAD输出/输入
}IIS_Clk_ConfigTypedef;

/**
 * @brief 解锁系统控制寄存器
 */
void scu_unlock_system_config(void);

/**
 * @brief 解锁复位相关寄存器
 */
void scu_unlock_reset_config(void);

/**
 * @brief 解锁时钟相关寄存器
 */
void scu_unlock_clk_config(void);

/**
 * @brief 锁定系统控制寄存器
 */
void scu_lock_system_config(void);

/**
 * @brief 锁定时钟相关寄存器
 */
void scu_lock_clk_config(void);

/**
 * @brief 锁定复位寄存器
 */
void scu_lock_reset_config(void);

/**
 * @brief 设置外设时钟开关
 * @param device_base ，需要设置的外设基址
 * @param gate ，DISABLE ：关闭 ，ENABLE ：打开
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_set_device_gate(uint32_t device_base, int32_t gate);

/**
 * @brief 配置外设复位
 * @note  配合scu_set_device_reset_Release 使用，先reset,然后release，外设复位完成
 * @param device_base， 设备基地址
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_set_device_reset(uint32_t device_base);

/**
 * @brief 配置外设复位释放
 * @note  配合scu_set_device_reset  使用，先reset,然后release，外设复位完成
 * @param device_base， 设备基地址
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_set_device_reset_release(uint32_t device_base);

/**
 * @brief 获取系统复位状态
 * @return PARA_ERROR: 异常复位 ，RETURN_OK：正常上电
 */
int32_t scu_get_system_reset_state(void);

//IIS相关系统接口
void scu_iis_src_config(IIS_Src_Config_t *config,IIS_Mclk_Source_t src);
void scu_iis_mclk_config(IIS_Mclk_Config_t *config);
void scu_iis_pad_mclk_config(IIS_Mclk_Source_t src, IIS_Clk_Mode_t mode);
void scu_iis_codec_mclk_config(Codec_Channel_t channel, IIS_Mclk_Source_t src);
void scu_iis_clk_config(IISNumx device, IIS_Clk_Source_t clk_source);
void scu_iis_pad_clk_config(IIS_Clk_Source_t clk_source, IIS_Clk_Mode_t mode);
void scu_iis_codec_dac_data_config(Codec_Dac_Data_Sel_t src);
void scu_iis_pad_data_config(Pad_IIS_Data_Sel_t src);
void iis_clk_config(IIS_Clk_ConfigTypedef* config);

int32_t scu_set_ext_wakeup_int(Wakeup_Mask_t num, FunctionalState cmd);
int32_t scu_clear_ext_int_state(Wakeup_Mask_t num);
void scu_set_ext_filter_config(Ext_Num num,FunctionalState cmd,uint32_t param);
int32_t scu_set_div_parameter(uint32_t device_base,uint32_t div_num);
void scu_spiflash_no_boot_set(void);
void scu_run_in_flash(void);
void scu_run_not_in_flash(void);
int32_t scu_set_system_clk_gate(Sys_Clk_Gate_t base,FunctionalState gate);
void scu_sel_dtrflash_clk(Dtr_Clk_Sel_t clk);
void scu_wait_pll_lock_state();
void scu_nmi_irq_cfg(Nmi_Irq_t irq);

void dsu_init(void);

#ifdef __cplusplus
}
#endif 

#endif /*CI13LC_SCU_H*/

/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/