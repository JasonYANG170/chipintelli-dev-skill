#include "ci_scu.h"
#include "ci_dpmu.h"
#include "ci_core_eclic.h"

#define SYSCFG_UNLOCK_MAGIC 0x51ac0ffe
#define CKCFG_UNLOCK_MAGIC  0x51ac0ffe
#define RSTCFG_UNLOCK_MAGIC 0x51ac0ffe

#define DIV_MASK3   0x7
#define DIV_MASK7   0x7f
#define DIV_MASK10  0x3ff
#define DIV_MASK12  0xfff

extern int32_t dpmu_set_device_reset(uint32_t device_base);
extern int32_t dpmu_set_device_reset_release(uint32_t device_base);
extern int32_t dpmu_set_device_gate(uint32_t device_base, int32_t gate);
extern uint32_t dpmu_get_reset_state(void);

/**
 * @brief 延迟函数
 * 
 * @param cnt 延迟时间 毫秒
 * @return 无
 */
// static void _delay_10us(uint32_t cnt)
// {
//     volatile uint32_t i,j = 0;
//     for(i = 0; i < cnt; i++)
//     {
//         for(j = 0; j < 132; j++)
//         {
//             __asm volatile("nop");
//         }
//     }
// }

static uint32_t scu_enter_critical(void)
{
    uint32_t  base_pri = eclic_get_mth();
    eclic_set_mth((0x6<<5)|0x1f); //0XDF
    return base_pri;
}

static void scu_exit_critical(uint32_t base_pri)
{
    eclic_set_mth(base_pri);
}

/**
 * @brief 解锁系统控制寄存器
 * 
 */
void scu_unlock_system_config(void)
{
    SCU->SYSCFG_LOCK_CFG = SYSCFG_UNLOCK_MAGIC;
    while(!(SCU->SYSCFG_LOCK_CFG));
}

/**
 * @brief 解锁时钟相关寄存器
 * 
 */
void scu_unlock_clk_config(void)
{
    SCU->CKCFG_LOCK_CFG = CKCFG_UNLOCK_MAGIC;
    while(!(SCU->CKCFG_LOCK_CFG));
}

/**
 * @brief 解锁复位相关寄存器
 * 
 */
void scu_unlock_reset_config(void)
{
    SCU->RSTCFG_LOCK_CFG = RSTCFG_UNLOCK_MAGIC;
    while(!(SCU->RSTCFG_LOCK_CFG));
}

/**
 * @brief 锁定系统控制寄存器
 * 
 */
void scu_lock_system_config(void)
{
    SCU->SYSCFG_LOCK_CFG = 0x0;
}

/**
 * @brief 锁定时钟相关寄存器
 * 
 */
void scu_lock_clk_config(void)
{
    SCU->CKCFG_LOCK_CFG = 0x0;
}

/**
 * @brief 锁定复位寄存器
 * 
 */
void scu_lock_reset_config(void)
{
    SCU->RSTCFG_LOCK_CFG = 0x0;
}

/**
 * @brief 设置系统时钟开关
 * 
 * @param base ，需要设置的系统设备基地址
 * @param gate ，DISABLE ：关闭 ，ENABLE ：打开
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_set_system_clk_gate(Sys_Clk_Gate_t base,FunctionalState gate)
{
    volatile uint32_t* Sysx[] = {(uint32_t*)&SCU->SYS_CLKGATE_CFG0,
                                 (uint32_t*)&SCU->SYS_CLKGATE_CFG1};
    uint32_t  base_pri = scu_enter_critical();
    scu_unlock_clk_config();

    uint32_t reg_data = *Sysx[0];
    reg_data &= ~(0x1 << base);
    reg_data |= ((0x1 & gate) << base);
    *Sysx[0] = reg_data;
    
    scu_lock_clk_config();
    scu_exit_critical(base_pri);
    return RETURN_OK;
}

/**
 * @brief 设置外设时钟开关
 * 
 * @param device_base ，需要设置的外设基址
 * @param gate ，DISABLE ：关闭 ，ENABLE ：打开
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_set_device_gate(uint32_t device_base, int32_t gate)
{
    uint32_t  base_pri = scu_enter_critical();
    uint32_t base = device_base;
    
    base -= 0x40000000;
    base /= 0x1000;
    
    uint8_t group = base/16;
    uint8_t mod = base%16;

    scu_unlock_clk_config();
    switch(group)
    {
    case 0:
    {
        SCU->AHB_CLKGATE_CFG &= ~(0x1 << mod);
        SCU->AHB_CLKGATE_CFG |= ((0x1 & gate) << mod);
    }
    break;
    case 1:
    {
        if(mod == 3)
        {
            SCU->APB0_CLKGATE_CFG &= ~(0x3 << (mod + 8));
            SCU->APB0_CLKGATE_CFG |= ((0x1 & gate) << (mod + 8)); //CODEC_AD
            SCU->APB0_CLKGATE_CFG |= ((0x1 & gate) << (mod + 9)); //CODEC_DA
            break;
        }
        else if(mod > 3)
        {
            mod = mod - 1;
        }
        SCU->APB0_CLKGATE_CFG &= ~(0x1 << mod);
        SCU->APB0_CLKGATE_CFG |= ((0x1 & gate) << mod);
    }
    break;
    case 2:
    {
        if(mod == 6)
        {
            SCU->APB1_CLKGATE_CFG &= ~(0x3 << mod);
            SCU->APB1_CLKGATE_CFG |= ((0x1 & gate) << (mod + 0));  //IIS1_RX
            SCU->APB1_CLKGATE_CFG |= ((0x1 & gate) << (mod + 1));  //IIS1_TX
            break;
        }
        else if(mod >= 9)
        {
            mod = mod - 3;
        }
        else if((mod >= 7) && (mod <= 8))
        {
            mod = mod + 1;
        }
        SCU->APB1_CLKGATE_CFG &= ~(0x1 << mod);
        SCU->APB1_CLKGATE_CFG |= ((0x1 & gate) << mod);
    }
    break;
    case 3:
    {
        dpmu_set_device_gate(device_base, gate);
    } 
    break;
    default:
        scu_lock_clk_config();
        scu_exit_critical(base_pri);
        return PARA_ERROR;
    }
    scu_lock_clk_config();
    scu_exit_critical(base_pri);
    return RETURN_OK;
}

/**
 * @brief 设置外部事件中断唤醒
 * 
 * @param num ，需要设置的外部事件编号
 * @param cmd ，DISABLE ：禁止 ，ENABLE ：使能
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_set_ext_wakeup_int(Wakeup_Mask_t num, FunctionalState cmd)
{
    volatile uint32_t* Wakex[] = {(uint32_t*)&SCU->WAKEUP_MASK_CFG0,
                                  (uint32_t*)&SCU->WAKEUP_MASK_CFG1};
    uint32_t  base_pri = scu_enter_critical();
    scu_unlock_system_config();

    if(cmd == ENABLE)
    {
        *Wakex[0] |= ((0x1 << num) | 0x1);
    }
    else
    {
        *Wakex[0] &= ~((0x1 << num) | 0x1);
    }

    scu_lock_system_config();
    scu_exit_critical(base_pri);
    return RETURN_OK;
}

/**
 * @brief 清除外部事件中断唤醒状态
 * 
 * @param num ，需要设置的外部事件编号
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_clear_ext_int_state(Wakeup_Mask_t num)
{
    volatile uint32_t* Intx[] = {(uint32_t*)&SCU->INT_STATE_REG0,
                                 (uint32_t*)&SCU->INT_STATE_REG1};
    uint32_t  base_pri = scu_enter_critical();
    scu_unlock_system_config();

    *Intx[0] |= (0x1 << num)|0x1;

    scu_lock_system_config();
    scu_exit_critical(base_pri);
    return RETURN_OK;
}

/**
 * @brief 设置pad输入信号（外部事件）滤波
 * 
 * @param num ，需要设置的外部事件编号
 * @param cmd ，DISABLE ：不使能 ，ENABLE ：使能
 * @param param ，pad输入信号（外部事件）滤波参数
 * @return 无
 */
void scu_set_ext_filter_config(Ext_Num num,FunctionalState cmd,uint32_t param)
{
    uint32_t  base_pri = scu_enter_critical();
    volatile uint32_t* Extx[] = {(uint32_t*)&SCU->EXT0_FILTER_CFG,
                                 (uint32_t*)&SCU->EXT1_FILTER_CFG};
    scu_unlock_system_config();

    if(cmd == ENABLE)
    {
        *Extx[num] &= ~(0xfffff<<1);
        *Extx[num] |= (param << 0)|(0x1<<20);
    }
    else
    {
        *Extx[num] &= ~(0x1<<20);
    }

    scu_lock_system_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 配置外设复位
 * @note  配合scu_set_device_reset_Release 使用，先reset,然后release，外设复位完成
 * @param device_base， 设备基地址
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_set_device_reset(uint32_t device_base)
{
    uint32_t  base_pri = scu_enter_critical();
    uint32_t base = device_base;
    
    base -= 0x40000000;
    base /= 0x1000;
    
    uint8_t group = base/16;
    uint8_t mod = base%16;

	scu_unlock_reset_config();
    switch(group)
    {
    case 0:
    {
        SCU->AHB_RESET_CFG &= ~(0x1 << mod);
    }
    break;
    case 1:
    {
        if(mod > 3)
        {
            mod |= 0x1;
        }
        SCU->APB0_RESET_CFG &= ~(0x1 << mod);
    }
    break;
    case 2:
    {
        if(mod > 6)
        {
            mod = mod + 1;
        }
        SCU->APB1_RESET_CFG &= ~(0x1 << mod);
    }
    break;
    case 3:
    {
        dpmu_set_device_reset(device_base);
    }
    break;
    default:
        scu_lock_reset_config();
        scu_exit_critical(base_pri);
		return PARA_ERROR;
    }

	_delay_10us(2);
	scu_lock_reset_config();
    scu_exit_critical(base_pri);
	return RETURN_OK;
}

/**
 * @brief 配置外设复位释放
 * @note  配合scu_set_device_reset  使用，先reset,然后release，外设复位完成
 * @param device_base， 设备基地址
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_set_device_reset_release(uint32_t device_base)
{
    uint32_t  base_pri = scu_enter_critical();
    uint32_t base = device_base;
    
    base -= 0x40000000;
    base /= 0x1000;
    
    uint8_t group = base/16;
    uint8_t mod = base%16;

	scu_unlock_reset_config();
    switch(group)
    {
    case 0:
    {
        SCU->AHB_RESET_CFG |= (0x1 << mod);
    }
    break;
    case 1:
    {
        if(mod > 3)
        {
            mod |= 0x1;
        }
        SCU->APB0_RESET_CFG |= (0x1 << mod);
    }
    break;
    case 2:
    {
        if(mod > 6)
        {
            mod = mod + 1;
        }
        SCU->APB1_RESET_CFG |= (0x1 << mod);
    }
    break;
    case 3:
    {
        dpmu_set_device_reset_release(device_base);
    }
    break;
    default:
        scu_lock_reset_config();
        scu_exit_critical(base_pri);
		return PARA_ERROR;
    }

	_delay_10us(2);
	scu_lock_reset_config();
    scu_exit_critical(base_pri);
	return RETURN_OK;
}

/**
 * @brief 配置外设分频参数之前需将CLK_DIV_PARAM_EN_CFG寄存器相应位置0
 * 
 * @param device_base， 设备基地址
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int8_t scu_para_en_disable(uint32_t device_base)
{
    uint32_t para_en;
    uint32_t  base_pri = scu_enter_critical();
    
    para_en = SCU->CLK_DIV_PARAM_EN_CFG;
    
    if(HAL_DTRFLASH_BASE == device_base)
    {
        para_en &= ~(0x1 << 1);
    }
    else if(HAL_DTRFLASH_RAM_BASE == device_base)
    {
        para_en &= ~(0x1 << 2);
    }
    else if(SYSTICK_BASE == device_base)
    {
        para_en &= ~(0x1 << 3);
    }
    else if((device_base >= HAL_PWM0_BASE) && (device_base <= HAL_TIMER1_BASE))  
    {
        para_en &= ~(0x1 << 4);
    }
    else if(HAL_UART0_BASE == device_base)
    {
        para_en &= ~(0x1 << 5);
    }
    else if(HAL_UART1_BASE == device_base)
    {
        para_en &= ~(0x1 << 6);
    }
    else if(HAL_UART2_BASE == device_base)
    {
        para_en &= ~(0x1 << 7);
    }
    else
    {
        scu_exit_critical(base_pri);
        return PARA_ERROR;
    }
    
    if(SCU->CLK_DIV_PARAM_EN_CFG != para_en)
    {
        SCU->CLK_DIV_PARAM_EN_CFG = para_en;
    }
    scu_exit_critical(base_pri);
    return RETURN_OK;
}

/**
 * @brief 配置外设分频参数之后需将CLK_DIV_PARAM_EN_CFG寄存器相应位置1
 * 
 * @param device_base， 设备基地址
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int8_t scu_para_en_enable(uint32_t device_base)
{
    uint32_t para_en;
    uint32_t  base_pri = scu_enter_critical();
    
    para_en = SCU->CLK_DIV_PARAM_EN_CFG;

    if(HAL_DTRFLASH_BASE == device_base)
    {
        para_en |= (0x1 << 1);
    }
    else if(HAL_DTRFLASH_RAM_BASE == device_base)
    {
        para_en |= (0x1 << 2);
    }
    else if(SYSTICK_BASE == device_base)
    {
        para_en |= (0x1 << 3);
    }
    else if((device_base >= HAL_PWM0_BASE) && (device_base <= HAL_TIMER1_BASE))  
    {
        para_en |= (0x1 << 4);
    }
    else if(HAL_UART0_BASE == device_base)
    {
        para_en |= (0x1 << 5);
    }
    else if(HAL_UART1_BASE == device_base)
    {
        para_en |= (0x1 << 6);
    }
    else if(HAL_UART2_BASE == device_base)
    {
        para_en |= (0x1 << 7);
    }
    else
    {
        scu_exit_critical(base_pri);
        return PARA_ERROR;
    }
    
    if(SCU->CLK_DIV_PARAM_EN_CFG != para_en)
    {
        SCU->CLK_DIV_PARAM_EN_CFG = para_en;
    }
    scu_exit_critical(base_pri);
    return RETURN_OK;
}

/**
 * @brief 设置外设时钟分频
 * @note 更改分频系数的时候详细步骤如下：
 *      （1）、关闭外设时钟Gate：scu_set_device_gate
 *      （2）、复位外设Reset：scu_set_device_reset
 *      （3）、设置外设时钟分频参数div：scu_set_div_parameter
 *      （4）、复位释放外设ResetRelease：scu_set_device_reset_release
 *      （5）、打开外设时钟Gate：scu_set_device_gate
 * @param device_base， 设备基地址
 * @param div_num， 分频参数
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t scu_set_div_parameter(uint32_t device_base,uint32_t div_num)
{
    uint32_t new_div0,new_div1;
    uint32_t  base_pri = scu_enter_critical();
    
    new_div0 = SCU->CLK_DIV_PARAM0_CFG;   
    new_div1 = SCU->CLK_DIV_PARAM1_CFG;   

    if(device_base == HAL_DTRFLASH_BASE)
    {
        new_div0 &= ~(DIV_MASK3 << 6);
        new_div0 |= ((div_num & DIV_MASK3) << 6);
    }
    else if(device_base == HAL_DTRFLASH_RAM_BASE)
    {
        new_div0 &= ~(DIV_MASK3 << 9);
        new_div0 |= ((div_num & DIV_MASK3) << 9);
    }
    else if(device_base == SYSTICK_BASE)
    {
        new_div0 &= ~(DIV_MASK12 << 12);
        new_div0 |= ((div_num & DIV_MASK12) << 12);
    }
    else if((device_base >= HAL_PWM0_BASE) && (device_base <= HAL_TIMER1_BASE))         
    {
        new_div0 &= ~(DIV_MASK7 << 24);
        new_div0 |= ((div_num & DIV_MASK7) << 24);
    }
    else if(device_base == HAL_UART0_BASE)
    {
        new_div1 &= ~(DIV_MASK7 << 0);
        new_div1 |= ((div_num & DIV_MASK7) << 0);
    }
    else if(device_base == HAL_UART1_BASE)
    {
        new_div1 &= ~(DIV_MASK7 << 7);
        new_div1 |= ((div_num & DIV_MASK7) << 7);
    }
    else if(device_base == HAL_UART2_BASE)
    {
        new_div1 &= ~(DIV_MASK7 <<14);
        new_div1 |= ((div_num & DIV_MASK7) << 14);
    }
    else
    {
        scu_exit_critical(base_pri);
        return PARA_ERROR;
    }
    
    scu_unlock_clk_config();
    scu_unlock_reset_config();
    
    //先置0
    scu_para_en_disable(device_base);

    _delay_10us(1);
    
    if(SCU->CLK_DIV_PARAM0_CFG != new_div0)
    {
        SCU->CLK_DIV_PARAM0_CFG = new_div0;
    }
    
    if(SCU->CLK_DIV_PARAM1_CFG != new_div1)
    {
        SCU->CLK_DIV_PARAM1_CFG = new_div1;
    }
    
    //再置1
    scu_para_en_enable(device_base);
    /*分频使能之后，delay*/
    _delay_10us(1);

    scu_lock_clk_config();
    scu_lock_reset_config();
    scu_exit_critical(base_pri);
    return RETURN_OK;
}


/**
 * @brief 配置QSPI 0 非boot 模式
 * 
 */
void scu_spiflash_no_boot_set(void)
{
    uint32_t  base_pri = scu_enter_critical();
    scu_unlock_system_config();
    uint32_t reg_data = SCU->SYS_CTRL_CFG;
    reg_data &= ~(0x1 << 0);
    SCU->SYS_CTRL_CFG = reg_data;
    scu_lock_system_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 设置程序在FLASH中运行
 * 
 */
void scu_run_in_flash(void)
{
	uint32_t  base_pri = scu_enter_critical();
    scu_unlock_system_config();
    uint32_t reg_data = SCU->SYS_CTRL_CFG;
    reg_data |= (0x1 << 9);
    SCU->SYS_CTRL_CFG = reg_data;
	scu_lock_system_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 设置程序不在FLASH中运行
 * 
 */
void scu_run_not_in_flash(void)
{
	uint32_t  base_pri = scu_enter_critical();
    scu_unlock_system_config();
    SCU->SYS_CTRL_CFG &= ~(0x1 << 9);
	scu_lock_system_config();
    scu_exit_critical(base_pri);
}



/**
 * @brief 设置DTR控制器时钟来源
 * 
 * @param clk，DTR控制器时钟来源（PLL倍频前、PLL倍频后）
 * 
 */
void scu_sel_dtrflash_clk(Dtr_Clk_Sel_t clk)
{
    uint32_t base_pri = scu_enter_critical();
    scu_unlock_system_config();
    uint32_t reg_data = SCU->SYS_CTRL_CFG;
    if(clk == DTR_CLK_SEL_SRC_CLK)
    {
        reg_data &= ~(0x1 << 10);
    }
    else if(clk == DTR_CLK_SEL_PLL_CLK)
    {
        reg_data |= (0x1 << 10);
    }
    SCU->SYS_CTRL_CFG = reg_data;
	scu_lock_system_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 等待 PLL 处于 LOCK 状态
 * 
 */
void scu_wait_pll_lock_state()
{
    uint32_t base_pri = scu_enter_critical();
	scu_unlock_system_config();
    while((SCU->SCU_STATE_REG & (0x1 << 2)) == 0);
	scu_lock_system_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 设置IIS MCLK 时钟来源 SRC 时钟（0、1）的分频
 * 
 * @param config 配置结构体
 * @return 无
 */
void scu_iis_src_config(IIS_Src_Config_t *config,IIS_Mclk_Source_t src)
{
    uint32_t base_pri = scu_enter_critical();
    scu_unlock_clk_config();
    volatile uint32_t* srcx[] = {(uint32_t*)&SCU->SRC0_MCLK_CFG, 
                                 (uint32_t*)&SCU->SRC1_MCLK_CFG};
    uint32_t para_en[] = {8, 9};
    uint32_t div_cfg[] = {0, 10};
    uint8_t id = src;
    uint32_t reg_data = *srcx[id];
    uint32_t param_en = SCU->CLK_DIV_PARAM_EN_CFG;
    uint32_t param2 = SCU->CLK_DIV_PARAM2_CFG;

    //关闭时钟
    reg_data &= ~(0x1 << 0);
    *srcx[id] = reg_data;
    _delay_10us(1);
    //来源配置
    reg_data &= ~(0x3 << 1);
    reg_data |= (config->source << 1);
    *srcx[id] = reg_data;
    //分频更新不使能
    param_en &= ~(0x1 << para_en[id]);
    SCU->CLK_DIV_PARAM_EN_CFG = param_en;
    _delay_10us(1);
    //设置分频参数
    param2 &= ~(DIV_MASK10 << div_cfg[id]);
    param2 |= (config->source_div << div_cfg[id]);
    SCU->CLK_DIV_PARAM2_CFG = param2;
    _delay_10us(1);
    //分频更新使能
    param_en |= (0x1 << para_en[id]);
    SCU->CLK_DIV_PARAM_EN_CFG = param_en;
    //延时再打开时钟
    _delay_10us(1);
    *srcx[id] |= (0x1 << 0);

    scu_lock_clk_config();

    scu_exit_critical(base_pri);
}

/**
 * @brief 设置IIS MCLK（0、1） 时钟参数
 *
 * @param config 配置结构体
 * @return 无
 */
void scu_iis_mclk_config(IIS_Mclk_Config_t *config)
{
    uint32_t base_pri = scu_enter_critical();
    scu_unlock_clk_config();
    volatile uint32_t* mclkx[] = {(uint32_t*)&SCU->MCLK0_CFG, 
                                  (uint32_t*)&SCU->MCLK1_CFG};
    uint8_t id = config->mclk;
    uint32_t reg_data = *mclkx[id];

    //关闭时钟
    reg_data &= ~(0x1 << 0);
    *mclkx[id] = reg_data;
    _delay_10us(1);
    //配置MCLK来源、SCK/LRCK、过采样率
    reg_data &= ~(0x1F << 1);
    reg_data |= (config->src << 1);
    reg_data |= (config->sck_lrck << 3);
    reg_data |= (config->fs << 4);
    *mclkx[id] = reg_data;
    //延时再打开时钟
    _delay_10us(1);
    reg_data |= (0x1 << 0);
    *mclkx[id] = reg_data;

    scu_lock_clk_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 设置IIS（0、1、2）时钟参数
 *
 * @param device，IIS 设备选择
 * @param clk_source，SCK/LRCK 来源选择
 * @return 无
 */
void scu_iis_clk_config(IISNumx device, IIS_Clk_Source_t clk_source)
{
    uint32_t base_pri = scu_enter_critical();
	scu_unlock_clk_config();
    volatile uint32_t* iisx[] = {(uint32_t*)&SCU->IIS0_CLK_SEL_CFG, 
                                 (uint32_t*)&SCU->IIS1_CLK_SEL_CFG, 
                                 (uint32_t*)&SCU->IIS1_CLK_SEL_CFG};
    uint32_t clk_en[] = {0, 0, 4};
    uint32_t clk_src[] = {1, 1, 5};
    uint32_t reg_data = *iisx[device];

    reg_data &= ~(0x1 << clk_en[device]);
    *iisx[device] = reg_data;
    _delay_10us(1);
    reg_data &= ~(0x7 << clk_src[device]);
    reg_data |= (clk_source << clk_src[device]);
    *iisx[device] = reg_data;
    _delay_10us(1);
    reg_data |= (0x1 << clk_en[device]);
    *iisx[device] = reg_data;

	scu_lock_clk_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 设置 IIS PAD CLK 时钟参数
 *
 * @param clk_source ，SCK/LRCK 来源选择
 * @param mode ，SCK/LRCK 时钟输入输出选择
 * @return 无
 */
void scu_iis_pad_clk_config(IIS_Clk_Source_t clk_source, IIS_Clk_Mode_t mode)
{
    uint32_t base_pri = scu_enter_critical();
	scu_unlock_clk_config();
    uint32_t reg_data = SCU->PAD_CLK_SEL_CFG;
    //关SCK时钟
    reg_data &= ~(0x1 << 3);
    SCU->PAD_CLK_SEL_CFG = reg_data;
    _delay_10us(1);
    if(mode == IIS_CLK_MODE_OUTPUT)
    {
        //PAD SCK/LRCK输出来源
        reg_data &= ~(0x7 << 4);
        reg_data |= (clk_source << 4);
    }
    //SCK和LRCK的pad的方向选择
    reg_data &= ~(0x1 << 8);
    reg_data |= (mode << 8);
    SCU->PAD_CLK_SEL_CFG = reg_data;
    //开SCK时钟
    _delay_10us(1);
    reg_data |= (0x1 << 3);
    SCU->PAD_CLK_SEL_CFG = reg_data;
	scu_lock_clk_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 设置IIS PAD MCLK时钟参数
 *
 * @param src ，MCLK 来源选择
 * @param mode ，MCLK 时钟输入输出选择
 * @return 无
 */
void scu_iis_pad_mclk_config(IIS_Mclk_Source_t src, IIS_Clk_Mode_t mode)
{
    uint32_t base_pri = scu_enter_critical();
	scu_unlock_clk_config();
    uint32_t reg_data = SCU->PAD_CLK_SEL_CFG;
    //关SCK时钟
    reg_data &= ~(0x1 << 0);
    SCU->PAD_CLK_SEL_CFG = reg_data;
    _delay_10us(1);
    if(mode == IIS_CLK_MODE_OUTPUT)
    {
        //PAD MCLK输出来源
        reg_data &= ~(0x3 << 1);
        reg_data |= (src << 1);
    }
    //MCLK的pad的方向选择
    reg_data &= ~(0x1 << 7);
    reg_data |= (mode << 7);
    SCU->PAD_CLK_SEL_CFG = reg_data;
    //开SCK时钟
    _delay_10us(1);
    reg_data |= (0x1 << 0);
    SCU->PAD_CLK_SEL_CFG = reg_data;

	scu_lock_clk_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 设置CODEC MCLK 时钟参数
 *
 * @param channel ，CODEC AD/DA 选择
 * @param src ，MCLK 来源选择
 * @return 无
 */
void scu_iis_codec_mclk_config(Codec_Channel_t channel, IIS_Mclk_Source_t src)
{
    uint32_t base_pri = scu_enter_critical();
	scu_unlock_clk_config();
    uint32_t reg_data = SCU->CODEC_CLK_SEL_CFG;

    uint32_t clk_en[] = {0, 3};
    uint32_t src_t[] = {1, 4};
    //关SCK时钟
    reg_data &= ~(0x1 << clk_en[channel]);
    SCU->CODEC_CLK_SEL_CFG = reg_data;
    _delay_10us(1);
    //MCLK 来源
    reg_data &= ~(0x3 << src_t[channel]);
    reg_data |= (src << src_t[channel]);
    SCU->CODEC_CLK_SEL_CFG = reg_data;
    //开SCK时钟
    _delay_10us(1);
    reg_data |= (0x1 << clk_en[channel]);
    SCU->CODEC_CLK_SEL_CFG = reg_data;

	scu_lock_clk_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 设置 codec dac IIS 数据来源
 *
 * @param src ，codec dac IIS 数据来源
 * @return 无
 */
void scu_iis_codec_dac_data_config(Codec_Dac_Data_Sel_t src)
{
    uint32_t base_pri = scu_enter_critical();
	scu_unlock_clk_config();
    scu_unlock_system_config();
    uint32_t reg_data = SCU->IIS_DATA_SEL_CFG;
    reg_data &= ~(0x3 << 2);
    reg_data |= (src << 2);
    SCU->IIS_DATA_SEL_CFG = reg_data;
	scu_lock_clk_config();
    scu_lock_system_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief 设置 pad IIS输出数据来源选择
 *
 * @param src ，pad IIS输出数据来源选择
 * @return 无
 */
void scu_iis_pad_data_config(Pad_IIS_Data_Sel_t src)
{
    uint32_t base_pri = scu_enter_critical();
	scu_unlock_clk_config();
    scu_unlock_system_config();
    uint32_t reg_data = SCU->IIS_DATA_SEL_CFG;
    reg_data &= ~(0x3 << 0);
    reg_data |= (src << 0);
    SCU->IIS_DATA_SEL_CFG = reg_data;
	scu_lock_clk_config();
    scu_lock_system_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief IIS时钟配置
 *
 * @param config ，配置参数结构体
 * @return 无
 */
void iis_clk_config(IIS_Clk_ConfigTypedef* config)
{
    //SLAVE模式
    if(config->model_sel == IIS_SLAVE)
    {
        scu_iis_clk_config(config->device_select, IIS_CLK_SOURCE_PAD_IN);
        scu_iis_pad_clk_config(config->clk_cfg,IIS_CLK_MODE_INPUT);
        //仅输入MCLK，不分SCK和LRCK
        if(config->mclk_mode = IIS_MCLK_IN)
        {
            scu_iis_pad_mclk_config(IIS_MCLK_SOURCE_PAD_IN, IIS_CLK_MODE_INPUT);
        }
    }
    //MASTER模式
    else
    {
        //MCLK从PAD输入
        if(IIS_MCLK_IN == config->mclk_mode)
        {
            scu_iis_mclk_config(&config->mclk_cfg);
            scu_iis_clk_config(config->device_select, config->clk_cfg);
            scu_iis_pad_mclk_config(config->mclk_cfg.src, IIS_CLK_MODE_INPUT);
        }
        //MCLK从SRC来
        else
        {
            scu_iis_src_config(&config->src_cfg, config->mclk_cfg.src);
            scu_iis_mclk_config(&config->mclk_cfg);
            scu_iis_clk_config(config->device_select, config->clk_cfg);
            //MCLK从PAD输出
            if(IIS_MCLK_OUT == config->mclk_mode)
            {
                scu_iis_pad_mclk_config(config->mclk_cfg.src, IIS_CLK_MODE_OUTPUT);
            }
        }
        //SCK/LRCK从PAD输出
        if(IIS_SCKLRCK_OUT == config->clk_mode)
        {
            scu_iis_pad_clk_config(config->clk_cfg,IIS_CLK_MODE_OUTPUT);
        }
    }

    //IIS1_TX对应CODEC_DA
    if(IISNum1_TX == config->device_select)
    {
        scu_iis_codec_mclk_config(CODEC_CHANNEL_DA, config->mclk_cfg.src);
    }
    //IIS1_RX对应CODEC_AD
    else if(IISNum1_RX == config->device_select)
    {
        scu_iis_codec_mclk_config(CODEC_CHANNEL_AD, config->mclk_cfg.src);
    }
}

/**
 * @brief 获取系统复位状态
 * 
 * @return PARA_ERROR，异常复位；RETURN_OK，正常上电复位
 */
int32_t scu_get_system_reset_state(void)
{
    static int32_t scu_state = RETURN_OK;
    if(scu_state == RETURN_OK)
    {
        if(dpmu_get_reset_state() & (0x1F << 1))
        {
            dpmu_clean_reset_state();
            scu_state = PARA_ERROR;
        }
    }
    
    return scu_state;
}

/**
 * @brief nmi中断选择
 * 
 * @param irq ，外部中断选择
 */
void scu_nmi_irq_cfg(Nmi_Irq_t irq)
{
    uint32_t base_pri = scu_enter_critical();
    scu_unlock_system_config();
    uint32_t reg_data = SCU->SYS_CTRL_CFG;
    reg_data &= ~(0xF << (1));
    reg_data |= (irq << (1));
    SCU->SYS_CTRL_CFG = reg_data;
    scu_lock_system_config();
    scu_exit_critical(base_pri);
}

/**
 * @brief efuse测试使能
 *
 * @param en ，ENABLE，打开；DISABLE，关闭
 */
void scu_efuse_test_mode(FunctionalState cmd)
{
	scu_unlock_system_config();
	uint32_t reg_data = SCU->EFUSE_TEST_MD;
	reg_data &= ~1;
	reg_data |= (cmd);
	SCU->EFUSE_TEST_MD = reg_data;
	scu_lock_system_config();
}

/**
 * @brief 外部中断使能
 *
 * @param num ，外部中断选择
 * @param en ，ENABLE，打开；DISABLE，关闭
 */
void scu_set_ext0_ext1_int(Ext_Num num,FunctionalState en)
{
	scu_unlock_system_config();
    uint32_t bit[] = {2,3};
    SCU->EXT_INT_CFG &= ~(0x1 << bit[num]);
	SCU->EXT_INT_CFG |= (en << bit[num]);
	scu_lock_system_config();
}

/**
 * @brief 清除外部中断状态
 *
 * @param num ，外部中断选择
 */
void scu_clear_ext0_ext1_int(Ext_Num num)
{
    scu_unlock_system_config();
    uint32_t bit[] = {0,1};
    SCU->EXT_INT_CFG |= (0x1 << bit[num]);
    scu_lock_system_config();
}
