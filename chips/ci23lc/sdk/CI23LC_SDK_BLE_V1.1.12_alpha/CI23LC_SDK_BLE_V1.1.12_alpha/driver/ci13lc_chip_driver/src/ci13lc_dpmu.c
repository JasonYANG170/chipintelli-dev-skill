#include "ci_dpmu.h"
#include "sdk_default_config.h"
#include "ci_core_eclic.h"
#include "ci_log.h"

#define SOFTWARE_RESET_KEY      0xdeadbeef
#define SYS_CLK_SEL_PLL_CLK       (0x1 << 2)
#define SYS_CLK_SEL_SRC_CLK        (~(0x1 <<2))

#define DIV_MASK4   0xf
#define DIV_MASK6   0x3f
#define DIV_MASK7   0x7f
#define DIV_MASK20  0xfffff

typedef struct
{
    uint32_t od:2;
    uint32_t n:4;
    uint32_t m:8;
    uint32_t bp:1;
    uint32_t rev:17;
}pll_cfg0_t;

typedef struct
{
    union pll_reg0 
    {
        uint32_t reg0_int;
        pll_cfg0_t reg0_bits;
    }reg0;
}pll_set_t;

// /*must be const!!! used in low_level_init function--for180MHz*/
// const pll_set_t pll_12d288_upto_480 =
// {
//     .reg0=
//     {
//         .reg0_bits =
//         {
//             .od = 1,
//             .n = 3,
//             .m = 78*3,
//             .bp = 0,
//             .rev = 0,
//         },
//     },
// };


// const pll_set_t pll_12d288_upto_475d136 =
// {
//     .reg0=
//     {
//         .reg0_bits =
//         {
//             .od = 1,
//             .n = 3,
//             .m = 232,
//             .bp = 0,
//             .rev = 0,
//         },
//     },
// };

// /*must be const!!! used in low_level_init function--for220MHz*/
// const pll_set_t pll_12d288_upto_440 =
// {
//     .reg0=
//     {
//         .reg0_bits =
//         {
//             .od = 2,
//             .n = 1,
//             .m = 143,
//             .bp = 0,
//             .rev = 0,
//         },
//     },
// };

// /*must be const!!! used in low_level_init function--for180MHz*/
// const pll_set_t pll_12d288_upto_400 =
// {
//     .reg0=
//     {
//         .reg0_bits =
//         {
//             .od = 2,
//             .n = 1,
//             .m = 130,
//             .bp = 0,
//             .rev = 0,
//         },
//     },
// };

/*must be const!!! used in low_level_init function--for180MHz*/
const pll_set_t pll_12d288_upto_420 =
{
    .reg0=
    {
        .reg0_bits =
        {
            .od = 1,
            .n = 1,
            .m = 68,
            .bp = 0,
            .rev = 0,
        },
    },
};

const pll_set_t pll_12d288_upto_164 =
{
    .reg0=
    {
        .reg0_bits =
        {
            .od = 3,
            .n = 1,
            .m = 80,
            .bp = 0,
            .rev = 0,
        },
    },
};

/*
计算方式：
NO = 2的（Od0 + 2*Od1）次方
M / (N * NO) = 360M / 12.288M
*/
// const pll_set_t pll_12d288_upto_360 =
// {
//     .reg0=
//     {
//         .reg0_bits =
//         {
//             .od = 2,
//             .n = 1,
//             .m = 117,
//             .bp = 0,
//             .rev = 0,
//         },
//     },
// };

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

static uint32_t dpmu_enter_critical(void)
{
    uint32_t  base_pri = eclic_get_mth();
    eclic_set_mth((0x6<<5)|0x1f); //0XDF
    return base_pri;
}

static void dpmu_exit_critical(uint32_t base_pri)
{
    eclic_set_mth(base_pri);
}

/**
 * @brief 解锁dpmu配置寄存器
 * 
 */
void dpmu_unlock_cfg_config(void)
{
    DPMU->CFG_LOCK_CFG = 0x51AC0FFE;
    while(!(DPMU->CFG_LOCK_CFG));
}

/**
 * @brief 锁定dpmu配置寄存器
 * 
 */
void dpmu_lock_cfg_config(void)
{
    DPMU->CFG_LOCK_CFG = 0x0;
}

int32_t dpmu_pll_set_high_freq_verify(const pll_set_t *pll_set)
{
	uint32_t  base_pri = dpmu_enter_critical();
    dpmu_unlock_cfg_config();

	/*选择CPU时钟为PLL倍频前,使用晶振时钟*/
	DPMU->SYS_CLK_SEL_CFG &= SYS_CLK_SEL_SRC_CLK;
    /*选择DTR控制器时钟来源为，PLL倍频前的时钟*/
    scu_sel_dtrflash_clk(DTR_CLK_SEL_SRC_CLK);

	DPMU->AON_RESET_CFG &= ~(0x1 << 4);
	/*a few reference periods*/
	__asm volatile("nop");

	/*配置PLL*/
	DPMU->PLL_CFG = pll_set->reg0.reg0_int;
	/*设置PLL配置参数生效*/
	DPMU->AON_CLK_PARAM_EN_CFG |= (0x1 << 0);
	DPMU->AON_CLK_PARAM_CFG &= !(0x3F << 0);
	DPMU->AON_CLK_PARAM_CFG |= (0x1 << 0);
	DPMU->AON_CLK_PARAM_EN_CFG |= (0x1 << 1);

	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");

	DPMU->AON_RESET_CFG |= (0x1 << 4);

	for(volatile int32_t i=800;i>0;i--)
	{
	    __asm volatile("nop");
	}

    /*等待PLL稳定后切换到PLL时钟*/
    scu_wait_pll_lock_state();

	/*选择CPU时钟为PLL倍频后*/
	DPMU->SYS_CLK_SEL_CFG |= SYS_CLK_SEL_PLL_CLK;
    /*选择DTR控制器时钟来源为，PLL倍频后的时钟*/
    scu_sel_dtrflash_clk(DTR_CLK_SEL_PLL_CLK);

	dpmu_lock_cfg_config();
    dpmu_exit_critical(base_pri);
	return RETURN_OK;
}

uint32_t dpmu_get_pll_frequency()
{
    dpmu_unlock_cfg_config();
    pll_set_t pll = {0};
    pll.reg0.reg0_int = DPMU->PLL_CFG;
    uint32_t temp = (0x1 << pll.reg0.reg0_bits.od);  //2的od次方
    
    uint32_t frequency = (uint32_t)get_src_clk() * pll.reg0.reg0_bits.m / pll.reg0.reg0_bits.n / temp;  //主频是PLL_OUT的一半

    dpmu_lock_cfg_config();
    return frequency;
}

/**
 * @brief 选择SRC时钟的来源
 * 
 */
void dpmu_set_src_source(Dpmu_Src_Source_Sel_t sel)
{
    dpmu_unlock_cfg_config();
    DPMU->SYS_CLK_SEL_CFG &= ~(3<<0);
    DPMU->SYS_CLK_SEL_CFG |= (sel<<0);
    dpmu_lock_cfg_config();
}

/**
 * @brief 选择系统时钟的工作时钟（SRC或PLL）
 * 
 */
void dpmu_sys_clk_sel_cfg(Dpmu_Sys_Clk_Sel_t sel)
{
	dpmu_unlock_cfg_config();
    DPMU->SYS_CLK_SEL_CFG &= ~(0x1 << 2);
    DPMU->SYS_CLK_SEL_CFG |= (sel << 2);
    dpmu_lock_cfg_config();
}


void dpmu_use_rc(void)
{
    uint32_t  base_pri = dpmu_enter_critical();
    dpmu_unlock_cfg_config();

    /*选择CPU时钟为PLL倍频前,使用Rc时钟*/
    DPMU->SYS_CLK_SEL_CFG &= SYS_CLK_SEL_SRC_CLK;
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    dpmu_lock_cfg_config();
    dpmu_exit_critical(base_pri);
}

/**
 * @brief 配置dpmu_pll
 * @note 注意可选时钟有效性
 * 
 * @param in_clk pll输入时钟频率
 * @param out_clk pll输出时钟频率
 */
void dpmu_pll_config(uint32_t in_clk, uint32_t out_clk)
{
    {
        pll_set_t const *pll_12d288_upto_config;
        #if 0
        #else
        uint32_t d1 = out_clk/8192000;
        if ((out_clk % 8192000) > 4096000)
        {
            d1 += 1;
        }
        out_clk = 8192000*d1;
        uint32_t pll_freq = out_clk*4;
        uint32_t min_remainder = in_clk;
        uint32_t best_m = 192;
        uint32_t best_n = 3;
        for (uint32_t m = 255; m > 1;m -= 1)
        {
            uint32_t remainder = m*in_clk%pll_freq;
            if (remainder < min_remainder)
            {
                min_remainder = remainder;
                best_m = m;
                best_n = m*in_clk/pll_freq;
                if (remainder == 0)
                {
                    break;
                }
            }
        }
        pll_set_t pll_info = {
            .reg0 = {
                .reg0_bits = {
                    .od = 2,
                    .n = best_n,
                    .m = best_m,
                    .bp = 0,
                    .rev = 0,
                }
            }
        };
        pll_12d288_upto_config = &pll_info;
        #endif
        dpmu_pll_set_high_freq_verify(pll_12d288_upto_config);
    }
}

// /**
//  * @brief 配置scu_pll
//  * @note 注意可选时钟有效性
//  * 
//  * @param clk pll 时钟
//  */
// void dpmu_pll_12d_config(uint32_t clk)
// {
//     pll_set_t const *pll_12d288_upto_config = &pll_12d288_upto_420;
//     switch(clk)
//     {
//     	case 420000000:
//     	{
//     		pll_12d288_upto_config = &pll_12d288_upto_420;
//     		break;
//     	}
//         case 164000000:
//     	{
//     		pll_12d288_upto_config = &pll_12d288_upto_164;
//     		break;
//     	}
//         default:
//         {
//             while(1);
//         }
//     }

//     dpmu_pll_set_high_freq_verify(pll_12d288_upto_config);
// }

/**
 * @brief 设置时钟开关
 * 
 * @param device_base ，需要设置的外设基址
 * @param gate ，DISABLE ：关闭 ，ENABLE ：打开
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t dpmu_set_device_gate(uint32_t device_base, int32_t gate)
{
	uint32_t  base_pri = dpmu_enter_critical();
    uint32_t base = device_base;
    base -= 0x40000000;
    base /= 0x1000;

    uint8_t mod = base%16;

    dpmu_unlock_cfg_config();
    if(mod)
    {
        mod = mod - 1;
        DPMU->AON_CLKGATE_CFG &= ~(0x1 << mod);
    	DPMU->AON_CLKGATE_CFG |= ((0x1 & gate) << mod);
    }
    else
    {
    	dpmu_lock_cfg_config();
        dpmu_exit_critical(base_pri);
        return PARA_ERROR;
    }
    dpmu_lock_cfg_config();
    dpmu_exit_critical(base_pri);
    return RETURN_OK;
}

/**
 * @brief 设置iwatchdog计数和复位受到CPU HALTED信号控制
 * 
 */
void dpmu_set_iwdg_halt()
{
    uint32_t  base_pri = dpmu_enter_critical();
    dpmu_unlock_cfg_config();
    DPMU->AON_CLKGATE_CFG |= (0x1 << (5));
    dpmu_lock_cfg_config();
    dpmu_exit_critical(base_pri);
}

/**
 * @brief 设置watchdog计数和复位不受CPU HALTED信号控制
 * 
 */
void dpmu_clean_iwdg_halt()
{
    uint32_t  base_pri = dpmu_enter_critical();
    dpmu_unlock_cfg_config();
    DPMU->AON_CLKGATE_CFG &= ~(0x1 << (5));
    dpmu_lock_cfg_config();
    dpmu_exit_critical(base_pri);
}

/**
 * @brief IWDG模块检测到系统喂狗异常时的复位范围：无复位操作
 *        
 */
void dpmu_iwdg_reset_none_config(void)
{
    dpmu_unlock_cfg_config();
    volatile uint32_t tmp;
    tmp = DPMU->SYS_RESET_CFG;
    tmp &= ~(0x3 << 6);
    DPMU->SYS_RESET_CFG = tmp;
    dpmu_lock_cfg_config();
}

/**
 * @brief IWDG模块检测到系统喂狗异常时的复位范围：复位全系统
 *        
 */
void dpmu_iwdg_reset_system_config(void)
{
    dpmu_unlock_cfg_config();
    volatile uint32_t tmp;
    tmp = DPMU->SYS_RESET_CFG;
    tmp &= ~(0x3 << 6);
    tmp |= (0x2 << 6);
    DPMU->SYS_RESET_CFG = tmp;
    dpmu_lock_cfg_config();
}

/**
 * @brief IWDG模块检测到系统喂狗异常时的复位范围：复位系统总线
 *        
 */
void dpmu_iwdg_reset_bus_config(void)
{
    dpmu_unlock_cfg_config();
    volatile uint32_t tmp;
    tmp = DPMU->SYS_RESET_CFG;
    tmp &= ~(0x3 << 6);
    tmp |= (0x3 << 6);
    DPMU->SYS_RESET_CFG = tmp;
    dpmu_lock_cfg_config();
}

/**
 * @brief 软件无复位操作
 *        
 */
void dpmu_software_reset_none_config(void)
{
	dpmu_unlock_cfg_config();
    DPMU->SYS_RESET_CFG &= ~(0x3 << 4);
    DPMU->SYS_SOFTRST_CFG = SOFTWARE_RESET_KEY;
    dpmu_lock_cfg_config();
}

/**
 * @brief 软件复位全系统
 *        
 */
void dpmu_software_reset_system_config(void)
{
	dpmu_unlock_cfg_config();
    DPMU->SYS_RESET_CFG &= ~(0x3 << 4);
    DPMU->SYS_RESET_CFG |= (0x2 << 4);
    DPMU->SYS_SOFTRST_CFG = SOFTWARE_RESET_KEY;
    dpmu_lock_cfg_config();
}

/**
 * @brief 软件复位系统总线
 *        
 */
void dpmu_software_reset_bus_config(void)
{
	dpmu_unlock_cfg_config();
    DPMU->SYS_RESET_CFG &= ~(0x3 << 4);
    DPMU->SYS_RESET_CFG |= ( 0x3 << 4);
    DPMU->SYS_SOFTRST_CFG = SOFTWARE_RESET_KEY;
    dpmu_lock_cfg_config();
}

/**
 * @brief cpu内核发出复位请求时的复位范围：无复位操作
 *        
 */
void dpmu_core_reset_none_config()
{
	dpmu_unlock_cfg_config();
    DPMU->SYS_RESET_CFG &= ~(0x3 << (0));
    dpmu_lock_cfg_config();
}

/**
 * @brief cpu内核发出复位请求时的复位范围：复位CPU内核
 *        
 */
void dpmu_core_reset_system_config()
{
	dpmu_unlock_cfg_config();
    DPMU->SYS_RESET_CFG &= ~(0x3 << (0));
    DPMU->SYS_RESET_CFG |= ( 0x2 << (0));
    dpmu_lock_cfg_config();
}

/**
 * @brief cpu内核发出复位请求时的复位范围：复位系统总线
 *        
 */
void dpmu_core_reset_bus_config()
{
	dpmu_unlock_cfg_config();
    DPMU->SYS_RESET_CFG &= ~(0x3 << (0));
    DPMU->SYS_RESET_CFG |= ( 0x3 << (0));
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置管脚复用对应功能
 *        
 * @param pin， 管脚名
 * @param io_function， 第 X 功能选择
 */
void dpmu_set_io_reuse(PinPad_Name pin,IOResue_FUNCTION io_function)
{
    uint32_t new_iocfg,new_iocfg1;

	dpmu_unlock_cfg_config();

    new_iocfg = DPMU->IOREUSE_CFG0;
    new_iocfg1 =  DPMU->IOREUSE_CFG1;

    switch(pin)
    {
    	//case PA0:
        case 0:
			new_iocfg &= ~(0x3 << 0);
			new_iocfg |= (io_function << 0);
			break;
        //case PA2:
        case 6:
			new_iocfg &= ~(0x7 << 2);
			new_iocfg |= (io_function << 2);
			break;
        //case PA3:
        case 7:
			new_iocfg &= ~(0x7 << 5);
			new_iocfg |= (io_function << 5);
			break;
        //case PA4:
        case 8:
			new_iocfg &= ~(0x7 << 8);
			new_iocfg |= (io_function << 8);
			break;
        //case PA5:
        case 9:
			new_iocfg &= ~(0x7 << 11);
			new_iocfg |= (io_function << 11);
			break;
        //case PA6:
        case 10:
			new_iocfg &= ~(0x7 << 14);
			new_iocfg |= (io_function << 14);
			break;
        //case PA7:
        case 11:
			new_iocfg &= ~(0x3 << 17);
			new_iocfg |= (io_function << 17);
			break;
        //case PB0:
        case 12:
			new_iocfg &= ~(0x3 << 19);
			new_iocfg |= (io_function << 19);
			break;
        //case PB1:
        case 13:
			new_iocfg &= ~(0x3 << 21);
			new_iocfg |= (io_function << 21);
			break;
        //case PB2:
        case 14:
			new_iocfg &= ~(0x3 << 23);
			new_iocfg |= (io_function << 23);
			break;
        //case PB5:
        case 17:
			new_iocfg &= ~(0x7 << 29);
			new_iocfg |= (io_function << 29);
			break;
        //case PB6:
        case 18:
			new_iocfg1 &= ~(0x7 << 0);
			new_iocfg1 |= (io_function << 0);
			break;
        //case PB7:
        case 19:
			new_iocfg1 &= ~(0x7 << 3);
			new_iocfg1 |= (io_function << 3);
			break;
        //case PC0:
        case 20:
			new_iocfg1 &= ~(0x7 << 6);
			new_iocfg1 |= (io_function << 6);
			break;
        //case PC1:
        case 26:
			new_iocfg1 &= ~(0x7 << 9);
			new_iocfg1 |= (io_function << 9);
			break;
        //case PC2:
        case 27:
			new_iocfg1 &= ~(0x7 << 12);
			new_iocfg1 |= (io_function << 12);
			break;
        //case PC3:
        case 28:
			new_iocfg1 &= ~(0x7 << 15);
			new_iocfg1 |= (io_function << 15);
			break;
        //case PC4:
        case 29:
			new_iocfg1 &= ~(0x7 << 18);
			new_iocfg1 |= (io_function << 18);
			break;
		default:
			break;
    }

    if(DPMU->IOREUSE_CFG0 != new_iocfg)
    {
    	DPMU->IOREUSE_CFG0 = new_iocfg;
    }

    if(DPMU->IOREUSE_CFG1 != new_iocfg1)
    {
    	DPMU->IOREUSE_CFG1 = new_iocfg1;
    }
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置管脚数字模拟功能
 * 
 * @param pin 管脚名
 * @param adio_mode 数字/模拟，功能选择
 * @return 无
 */
void dpmu_set_adio_reuse(PinPad_Name pin,ADIOResue_MODE adio_mode)
{
    uint32_t new_adiocfg = DPMU->AD_CFG0;

	dpmu_unlock_cfg_config();

    if(DIGITAL_MODE == adio_mode)
    {
        switch(pin)
        {
            //case PA0:
            case 0:
                new_adiocfg &= ~(0x1 << 0);
                break;
            //case PA1:
            case 1:
                new_adiocfg &= ~(0x1 << 1);
                break;
            //case PC1:
            case 26:
                new_adiocfg &= ~(0x1 << 2);
                break;
            //case PC2:
            case 27:
                new_adiocfg &= ~(0x1 << 3);
                break;
            //case PC3:
            case 28:
                new_adiocfg &= ~(0x1 << 4);
                break;
            //case PC4:
            case 29:
                new_adiocfg &= ~(0x1 << 5);
                break;
        }
    }
    else if(ANALOG_MODE == adio_mode)
    {
        switch(pin)
        {
            //case PA0:
            case 0:
                new_adiocfg |= (0x1 << 0);
                break;
            //case PA1:
            case 1:
                new_adiocfg |= (0x1 << 1);
                break;
            //case PC1:
            case 26:
                new_adiocfg |= (0x1 << 2);
                break;
            //case PC2:
            case 27:
                new_adiocfg |= (0x1 << 3);
                break;
            //case PC3:
            case 28:
                new_adiocfg |= (0x1 << 4);
                break;
            //case PC4:
            case 29:
                new_adiocfg |= (0x1 << 5);
                break;
        }
    }

    if(DPMU->AD_CFG0 != new_adiocfg)
    {
        DPMU->AD_CFG0 = new_adiocfg;
    }
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置外部中断滤波参数和使能
 * 
 * @param num 外部中断选择
 * @param flag 使能，不使能
 * @param param 参数
 * @return 无
 */
void dpmu_set_ext_filter_config(Ext_Num num,FunctionalState flag,uint32_t param)
{
	dpmu_unlock_cfg_config();

    if(flag == ENABLE)
    {
        switch(num)
        {
            case EXT0:
            {
                DPMU->WAKEUP_EXT_FILTER_CFG &= ~(0xffff);
                DPMU->WAKEUP_EXT_FILTER_CFG |= (param << 1)|0x1;
                break;
            }
            case EXT1:
            {
                DPMU->WAKEUP_EXT_FILTER_CFG &= ~(0xffff << 16);
                DPMU->WAKEUP_EXT_FILTER_CFG |= ((param << 1)|0x1)<<16;
                break;
            }
        }
    }
    else
    {
        switch(num)
        {
            case EXT0:
            {
                DPMU->WAKEUP_EXT_FILTER_CFG &= ~(0x1);
                break;
            }
            case EXT1:
            {
                DPMU->WAKEUP_EXT_FILTER_CFG &= ~(0x1 << 16);
                break;
            }
        }
    }

    dpmu_lock_cfg_config();
}

/**
 * @brief 配置管脚开漏功能（例如IIC需要引脚配置成此功能）
 * 
 * @param pin， 管脚名
 * @param cmd，ENABLE 使能开漏功能，DISABLE，不使能开漏功能
 * @return 无
 */
void dpmu_set_io_open_drain(PinPad_Name pin,FunctionalState cmd)
{
    dpmu_unlock_cfg_config();
    //if((pin >= PA2) & (pin <= PC0))
    if((pin >= 6) & (pin <= 20))
    {
        //pin = pin - PA2;
        pin = pin - 6;
        DPMU->OD_CFG0 &= ~(0x1 << pin);
        DPMU->OD_CFG0 |= (cmd << pin);
    }
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置管脚上下拉功能
 * 
 * @param pin， 管脚名
 * @param pull， 关闭上下拉、开上拉、开下拉
 * @return 无
 */
void dpmu_set_io_pull(PinPad_Name pin,Dpmu_Io_Pull_t pull)
{
    volatile uint32_t* PDx[] = {(uint32_t*)&DPMU->PD_CFG0};
    volatile uint32_t* PUx[] = {(uint32_t*)&DPMU->PU_CFG0};
    uint32_t reg_index = 0;
    uint32_t pd_data = 0,pu_data = 0;

    dpmu_unlock_cfg_config();
    pd_data = *PDx[reg_index];
    pu_data = *PUx[reg_index];
    if(pull == DPMU_IO_PULL_DISABLE)
    {
        pd_data &= ~(0x1 << pin);
        pu_data &= ~(0x1 << pin);
    }
    else if(pull == DPMU_IO_PULL_UP)
    {
        pd_data &= ~(0x1 << pin);
        pu_data |= (0x1 << pin);
    }
    else if(pull == DPMU_IO_PULL_DOWN)
    {
        pd_data |= (0x1 << pin);
        pu_data &= ~(0x1 << pin);
    }
    *PDx[reg_index] = pd_data;
    *PUx[reg_index] = pu_data;
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置管脚方向
 * 
 * @param pin， 管脚名
 * @param dir，输入（IE为1）、输出（IE为0）
 * @return 无
 */
void dpmu_set_io_direction(PinPad_Name pin,Dpmu_Io_Direction_t dir)
{
    volatile uint32_t* IEx[] = {(uint32_t*)&DPMU->IE_CFG0};
    uint32_t reg_index = 0;
    uint32_t reg_data = 0;
    
    //if(pin > PA1)
    if(pin > 1)
    {
        //只配置PA0、PA1的IE，其他全部保持默认值
        return;
    }

    dpmu_unlock_cfg_config();
    reg_data = *IEx[reg_index];
	if(dir == DPMU_IO_DIRECTION_INPUT)
    {
        reg_data |= (0x1 << pin);
    }
    else
    {
        reg_data &= ~(0x1 << pin);
    }
    *IEx[reg_index] = reg_data;
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置管脚触发器模式
 * 
 * @param pin， 管脚名
 * @param schmitt_trigger，触发器模式normal、schmitt trigger
 * @return 无
 */
void dpmu_set_io_schmitt_trigger(PinPad_Name pin,Dpmu_Io_Schmitt_Trigger_t schmitt_trigger)
{
    dpmu_unlock_cfg_config();
    DPMU->ST_CFG0 &= ~(0x1 << pin);
    DPMU->ST_CFG0 |= (schmitt_trigger << pin);
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置管脚电压转换率模式
 * 
 * @param pin， 管脚名
 * @param slew_rate，电压转换率slow、fast
 * @return 无
 */
void dpmu_set_io_slew_rate(PinPad_Name pin,Dpmu_Io_Slew_Rate_t slew_rate)
{
    dpmu_unlock_cfg_config();
    DPMU->SL_CFG0 &= ~(0x1 << pin);
    DPMU->SL_CFG0 |= (slew_rate << pin);
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置管脚驱动强度
 * 
 * @param pin， 管脚名
 * @param driver_strength，驱动强度级别选择
 * @return 无
 */
void dpmu_set_io_driver_strength(PinPad_Name pin,Dpmu_Io_Driver_Strength_t driver_strength)
{
    dpmu_unlock_cfg_config();
    //if(pin <= PB2)
    if(pin <= 14)
    {
        DPMU->DS_CFG0 &= ~(0x3 << (pin * 2));
        DPMU->DS_CFG0 |= (driver_strength << (pin * 2));
    }
    else
    {
        //pin = pin - PB5 + 1;
        pin = pin - 17 + 1;
        DPMU->DS_CFG1 &= ~(0x3 << (pin * 2));
        DPMU->DS_CFG1 |= (driver_strength << (pin * 2));
    }
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置外设复位
 * @note  配合dpmu_set_device_reset_release  使用，先reset,然后release，外设复位完成
 * @param device_base， 设备基地址
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t dpmu_set_device_reset(uint32_t device_base)
{
	dpmu_unlock_cfg_config();

    if(device_base == HAL_PC_BASE)
    {
    	DPMU->AON_RESET_CFG &= ~(0x1 << 0);
    }
    else if(device_base == HAL_IWDG_BASE)
    {
    	DPMU->AON_RESET_CFG &= ~(0x1 << 1);
    }
    else if(device_base == HAL_EFUSE_BASE)
    {
    	DPMU->AON_RESET_CFG &= ~(0x1 << 2);
    }
    else if(device_base == HAL_PVDC_BASE)
    {
    	DPMU->AON_RESET_CFG &= ~(0x1 << 3);
    }
	else if(device_base == PLL_BASE)
	{
		DPMU->AON_RESET_CFG &= ~(0x1 << 4);
	}
	else
	{
		dpmu_lock_cfg_config();
		return PARA_ERROR;
	}

	_delay_10us(2);
	dpmu_lock_cfg_config();
	return RETURN_OK;
}

/**
 * @brief 配置外设复位释放
 * @note  配合dpmu_set_device_reset 使用，先reset,然后release，外设复位完成
 * @param device_base， 设备基地址
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t dpmu_set_device_reset_release(uint32_t device_base)
{
	dpmu_unlock_cfg_config();
    
    if(device_base == HAL_PC_BASE)
    {
    	DPMU->AON_RESET_CFG |= (0x1 << 0);
    }
    else if(device_base == HAL_IWDG_BASE)
    {
    	DPMU->AON_RESET_CFG |= (0x1 << 1);
    }
    else if(device_base == HAL_EFUSE_BASE)
    {
    	DPMU->AON_RESET_CFG |= (0x1 << 2);
    }
    else if(device_base == HAL_PVDC_BASE)
	{
		DPMU->AON_RESET_CFG |= (0x1 << 3);
	}
    else if(device_base == PLL_BASE)
	{
		DPMU->AON_RESET_CFG |= (0x1 << 4);
	}
	else
	{
		dpmu_lock_cfg_config();
		return PARA_ERROR;
	}

	_delay_10us(2);
	dpmu_lock_cfg_config();
	return RETURN_OK;
}

void dpmu_set_low_power_mode(Dpmu_Lowpower_Mode_t mode)
{
	dpmu_unlock_cfg_config();
    DPMU->LOW_POWER_CFG &= ~(0x3 << (0));
    DPMU->LOW_POWER_CFG |= (mode << (0));
	dpmu_lock_cfg_config();
}

/**
 * @brief 低功耗唤醒源使能
 *
 * @param wake_int_num ，唤醒源共8组，可配置 0 ~ 7
 * @param flag ，ENABLE，使能。DISABLE，不使能
 */
void dpmu_set_wakeup_int(Dpmu_Wakeup_SRC_t wake_int_num,FunctionalState flag)
{
	dpmu_unlock_cfg_config();

    if(flag == ENABLE)
    {
        DPMU->WAKEUP_MASK_CFG |= (0x1 << wake_int_num);
    }
    else
    {
    	DPMU->WAKEUP_MASK_CFG &= ~(0x1 << wake_int_num);
    }

    dpmu_lock_cfg_config();
}


/**
 * @brief 系统退出低功耗进入系统复位流程时是否复位模块
 *
 * @param model ，复位的模块
 * @param flag ，ENABLE，复位。DISABLE，不复位
 */
void dpmu_wakeup_reset_cfg(Dpmu_Wakeup_Reset_Cfg_t model, FunctionalState flag)
{
	dpmu_unlock_cfg_config();

    if(flag == ENABLE)
    {
        DPMU->WAKEUP_RESET_CFG |= (0x1 << model);
    }
    else
    {
        DPMU->WAKEUP_RESET_CFG &= ~(0x1 << model);
    }

    dpmu_lock_cfg_config();
}

/**
 * @brief 获取唤醒状态
 * 
 * @return state: 唤醒状态寄存器值
 */
uint32_t dpmu_get_wakeup_state(void)
{
	dpmu_unlock_cfg_config();
    uint32_t state = DPMU->PWR_WAKEUP_STATE_REG;
    DPMU->PWR_WAKEUP_STATE_REG = state;
    dpmu_lock_cfg_config();
    return state;
}

/**
 * @brief 获取复位状态
 * 
 * @return state: 复位状态寄存器值
 */
uint32_t dpmu_get_reset_state(void)
{
	dpmu_unlock_cfg_config();
    uint32_t state = DPMU->RST_STATE_REG;
    //DPMU->RST_STATE_REG = state;
    dpmu_lock_cfg_config();
    return state;
}

/**
 * @brief 清除复位状态
 * 
 * @return state: 清除状态寄存器值
 */
void dpmu_clean_reset_state(void)
{
	dpmu_unlock_cfg_config();
    uint32_t state = DPMU->RST_STATE_REG;
    DPMU->RST_STATE_REG = state;
    dpmu_lock_cfg_config();
}

void dpmu_para_en_enable(uint32_t device_base)
{
    if(device_base == PLL_BASE)
    {
	    DPMU->AON_CLK_PARAM_EN_CFG |= 0x1<<0;
    }
    else if(device_base == IPCORE_BASE)
    {
		DPMU->AON_CLK_PARAM_EN_CFG |= 0x1<<1;
    }
    else if(device_base == APB_BASE)
	{
		DPMU->AON_CLK_PARAM_EN_CFG |= 0x1<<2;
	}
    else if(device_base == HAL_IWDG_BASE)
	{
		DPMU->AON_CLK_PARAM_EN_CFG |= 0x1<<3;
	}
    else if(device_base == TEST_CLK_BASE)
	{
		DPMU->AON_CLK_PARAM_EN_CFG |= 0x1<<5;
	}
}

void dpmu_para_en_disable(uint32_t device_base)
{
    if(device_base == PLL_BASE)
    {
	    DPMU->AON_CLK_PARAM_EN_CFG &= ~(0x1<<0);
    }
    else if(device_base == IPCORE_BASE)
    {
		DPMU->AON_CLK_PARAM_EN_CFG &= ~(0x1<<1);
    }
    else if(device_base == APB_BASE)
	{
		DPMU->AON_CLK_PARAM_EN_CFG &= ~(0x1<<2);
	}
    else if(device_base == HAL_IWDG_BASE)
	{
		DPMU->AON_CLK_PARAM_EN_CFG &= ~(0x1<<3);
	}
    else if(device_base == TEST_CLK_BASE)
	{
		DPMU->AON_CLK_PARAM_EN_CFG &= ~(0x1<<5);
	}
}

/**
 * @brief 配置外设时钟分频
 * 
 * @param device_base， 设备基地址
 * @param div_num， 分频参数
 * @return PARA_ERROR: 参数错误 ，RETURN_OK：配置完成
 */
int32_t dpmu_set_div_parameter(uint32_t device_base,uint32_t div_num)
{
    uint32_t new_div0, new_div1;

    new_div0 = DPMU->AON_CLK_PARAM_CFG;
    new_div1 = DPMU->AON_CLK_PARAM_CFG1;

    if(device_base == IPCORE_BASE)
    {
        new_div0 &= ~(DIV_MASK6 << 0);
        new_div0 |= ((div_num  & DIV_MASK6 ) << 0);
    }
    else if(device_base == APB_BASE)
    {
        new_div0 &= ~(DIV_MASK4 << 6);
        new_div0 |= ((div_num & DIV_MASK4) << 6);
    }
    else if(device_base == HAL_IWDG_BASE)
	{
	    new_div0 &= ~(DIV_MASK7 << 10);
	    new_div0 |= ((div_num & DIV_MASK7) << 10);
	}
    else if(device_base == TEST_CLK_BASE)
	{
	    new_div1 &= ~(DIV_MASK20 << 0);
	    new_div1 |= ((div_num & DIV_MASK20) << 0);
	}
    else
    {
        return PARA_ERROR;
    }

    dpmu_unlock_cfg_config();

    //先置0
    dpmu_para_en_disable(device_base);
    _delay_10us(1);

    if(DPMU->AON_CLK_PARAM_CFG != new_div0)
    {
    	DPMU->AON_CLK_PARAM_CFG = new_div0;
    }

    if(DPMU->AON_CLK_PARAM_CFG1 != new_div1)
    {
    	DPMU->AON_CLK_PARAM_CFG1 = new_div1;
    }

    //再置1
    dpmu_para_en_enable(device_base);
    /*分频使能之后，delay*/
    _delay_10us(1);

    dpmu_lock_cfg_config();
    return RETURN_OK;
}

/**
 * @brief 设置晶振IO功能配置
 * 
 * @param mode，功能选择
 * @param en，ENABLE，使能；DISABLE，关闭
 */
void dpmu_osc_pad_cfg_en(Dpmu_Xtal_Mode_t mode,FunctionalState cmd)
{
    dpmu_unlock_cfg_config();
    if(cmd == ENABLE)
    {
        DPMU->OSC_PAD_CFG |= (0x1 << mode);
    }
    else
    {
        DPMU->OSC_PAD_CFG &= ~(0x1 << mode);
    }
    dpmu_lock_cfg_config();
}

/**
 * @brief 设置晶振IO频率和驱动能力选择
 * 
 * @param num，IO频率和驱动能力
 */
void dpmu_osc_pad_cfg_fma(uint8_t num)
{
    dpmu_unlock_cfg_config();
    DPMU->OSC_PAD_CFG &= ~(0x7 << 0);
    DPMU->OSC_PAD_CFG |= (num << 0);
    dpmu_lock_cfg_config();
}

/**
 * @brief 设置VDT mask
 * 
 * @param en
 */
void dpmu_set_vdt_mask(bool en)
{
    dpmu_unlock_cfg_config();
    DPMU->CHIP_INT_MASK_CFG_ADDR &= ~(0x1 << 1);
    DPMU->CHIP_INT_MASK_CFG_ADDR |= (en << 1);
    dpmu_lock_cfg_config();
}

/**
 * @brief 设置LDO mask
 * 
 * @param en
 */
void dpmu_set_ldo_mask(bool en)
{
    dpmu_unlock_cfg_config();
    DPMU->CHIP_INT_MASK_CFG_ADDR &= ~(0x1 << 0);
    DPMU->CHIP_INT_MASK_CFG_ADDR |= (en << 0);
    dpmu_lock_cfg_config();
}

/**
 * @brief LDO1输出电压调整
 * 
 * @param lv 
 */
void dpmu_ldo1_lv_set(uint8_t lv)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_CFG &= ~(0x1f<<0);
    DPMU->PMU_CFG |= lv<<0;
    dpmu_lock_cfg_config();
}

/**
 * @brief LDO3输出电压调整
 * 
 * @param lv 
 */
void dpmu_ldo3_lv_set(uint8_t lv)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_CFG &= ~(0x7<<11);
    DPMU->PMU_CFG |= lv<<11;
    dpmu_lock_cfg_config();
}

/**
 * @brief LDO3输出使能
 * 
 * @param en：ture 使能，false 不使能
 */
void dpmu_ldo3_en(bool en)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_CFG &= ~(1<<10);
    DPMU->PMU_CFG |= ((!en)<<10);
    dpmu_lock_cfg_config();
}

/**
 * @brief 进入低功耗LDO1输出电压调整
 * 
 * @param lv 
 */
void dpmu_enter_lowpower_ldo1_lv(uint8_t lv)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_PWROFF_CFG &= ~(0x1f<<0);
    DPMU->PMU_PWROFF_CFG |= lv<<0;
    dpmu_lock_cfg_config();
}

/**
 * @brief 进入低功耗LDO3输出电压调整
 * 
 * @param lv 
 */
void dpmu_enter_lowpower_ldo3_lv(uint8_t lv)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_PWROFF_CFG &= ~(0x7<<11);
    DPMU->PMU_PWROFF_CFG |= lv<<11;
    dpmu_lock_cfg_config();
}

/**
 * @brief 进入低功耗LDO3输出使能
 * 
 * @param en：ture 使能，false 不使能
 */
void dpmu_enter_lowpower_ldo3_en(bool en)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_PWROFF_CFG &= ~(1<<10);
    DPMU->PMU_PWROFF_CFG |= ((!en)<<10);
    dpmu_lock_cfg_config();
}

/**
 * @brief 退出低功耗LDO1输出电压调整
 * 
 * @param lv 
 */
void dpmu_exit_lowpower_ldo1_lv(uint8_t lv)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_PWRON_CFG &= ~(0x1f<<0);
    DPMU->PMU_PWRON_CFG |= lv<<0;
    dpmu_lock_cfg_config();
}

/**
 * @brief 退出低功耗LDO3输出电压调整
 * 
 * @param lv 
 */
void dpmu_exit_lowpower_ldo3_lv(uint8_t lv)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_PWRON_CFG &= ~(0x7<<11);
    DPMU->PMU_PWRON_CFG |= lv<<11;
    dpmu_lock_cfg_config();
}

/**
 * @brief 退出低功耗LDO3输出使能
 * 
 * @param en：ture 使能，false 不使能
 */
void dpmu_exit_lowpower_ldo3_en(bool en)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_PWRON_CFG &= ~(1<<10);
    DPMU->PMU_PWRON_CFG |= ((!en)<<10);
    dpmu_lock_cfg_config();
}

/**
 * @brief PMU update en配置
 * 
 * @param num 
 */
void dpmu_set_pmu_update_en(Dpmu_Update_En_t num)
{
    dpmu_unlock_cfg_config();
    DPMU->PMU_UPDATE_EN &= ~(0x1<<num);
    DPMU->PMU_UPDATE_EN |= 1<<num;
    _delay_10us(100);//TODO:配置之后等一段时间，模拟生效的时间，从配置之后到模拟器件生效具体时间待测试
    dpmu_lock_cfg_config();
}


/**
 * @brief RC trim时粗调值设置
 * 
 * @param val 
 */
void dpmu_set_rc_trim_c_value(uint8_t val)
{
    // dpmu_unlock_cfg_config();
    DPMU->RC_CFG &= ~(0xff<<1);
    DPMU->RC_CFG |= val<<1;
    // dpmu_lock_cfg_config();
}


/**
 * @brief RC trim时精调值设置
 * 
 * @param val 
 */
void dpmu_set_rc_trim_f_value(uint8_t val)
{
    // dpmu_unlock_cfg_config();
    DPMU->RC_CFG &= ~(0xf<<9);
    DPMU->RC_CFG |= val<<9;
    // dpmu_lock_cfg_config();
}

/**
 * @brief RC EN设置
 * 
 * @param val 
 */
void dpmu_set_rc_en(bool en)
{
    dpmu_unlock_cfg_config();
    DPMU->RC_CFG &= ~(1<<0);
    DPMU->RC_CFG |= (!en)<<0;
    dpmu_lock_cfg_config();
}

/**
 * @brief RC_CFG使能生效
 * 
 * @param val 
 */
void dpmu_set_rc_update_cfg(void)
{
    dpmu_unlock_cfg_config();
    DPMU->RC_UPDATE_CFG |= 1;
    _delay_10us(10);//TODO:配置之后等一段时间，模拟生效的时间，从配置之后到模拟器件生效具体时间待测试
    dpmu_lock_cfg_config();
}

/**
 * @brief 配置晶振脚PA0、PA1功能选择（晶振/GPIO）
 * 
 * @param en: ENABLE（GPIO功能），DISABLE（晶振功能）
 */
void dpmu_osc_pad_for_gpio(FunctionalState en)
{
    if(en == ENABLE)
    {
        //使能GPIO，关晶振功能
        dpmu_osc_pad_cfg_en(DPMU_XTAL_EN,DISABLE);
    }
    else
    {
        dpmu_osc_pad_cfg_en(DPMU_XTAL_EN,ENABLE);
    }
}

/**
 * @brief 测试时钟来源选择，可通过PAD查看
 * 
 * @param src 来源
 */
void dpmu_test_clk_sel(Dpmu_Test_Clk_Sel_t src)
{
    dpmu_unlock_cfg_config();
    DPMU->SYS_CLK_SEL_CFG &= ~(0x3 << 3);
    DPMU->SYS_CLK_SEL_CFG |= (src << 3);
    dpmu_lock_cfg_config();
}

/**
 * @brief IWDG时钟来源选择
 * 
 * @param src 来源
 */
void dpmu_iwdg_clk_sel(Dpmu_Iwdg_Clk_Sel_t src)
{
    dpmu_unlock_cfg_config();
    DPMU->SYS_CLK_SEL_CFG &= ~(0x1 << 5);
    DPMU->SYS_CLK_SEL_CFG |= (src << 5);
    dpmu_lock_cfg_config();
}

void dpmu_sel_inner_rc_test()
{
    dpmu_sys_clk_sel_cfg(DPMU_SYS_CLK_SRC);
    dpmu_set_device_gate(PLL_BASE,DISABLE);
    dpmu_set_src_source(DPMU_SRC_USE_OUTSIDE_OSC);

    dpmu_unlock_cfg_config();
    dpmu_set_rc_trim_f_value(0xf);
    dpmu_set_rc_trim_c_value(0x7f);
    dpmu_lock_cfg_config();
    dpmu_set_rc_en(ENABLE);
    dpmu_set_rc_update_cfg();
    _delay_10us(5);

    dpmu_unlock_cfg_config();
    dpmu_set_rc_trim_f_value(0x0);
    dpmu_set_rc_trim_c_value(0x80);
    dpmu_lock_cfg_config();
    dpmu_set_rc_en(ENABLE);
    dpmu_set_rc_update_cfg();
    _delay_10us(5);

    dpmu_set_src_source(DPMU_SRC_USE_INNER_RC);
    _delay_10us(5);
    dpmu_set_device_gate(PLL_BASE,ENABLE);
    dpmu_sys_clk_sel_cfg(DPMU_SYS_CLK_PLL);
}