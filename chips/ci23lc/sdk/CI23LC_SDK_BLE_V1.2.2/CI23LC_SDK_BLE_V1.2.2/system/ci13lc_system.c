#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <malloc.h>
#include <string.h>
#include "sdk_default_config.h"
#include "ci_uart.h"
#include "ci_spiflash.h"
#include "ci13lc_core_eclic.h"
#include "ci13lc_dpmu.h"
#include "FreeRTOS.h"
#include "ci_log.h"
#include "port_api.h"
#include "status_share.h"



void _delay_10us(uint32_t cnt)
{
    volatile uint32_t i,j = 0;
    for(i = 0; i < cnt; i++)
    {
        for(j = 0; j < 160; j++)
        {
            __asm volatile("nop");
        }
    }
}

float load_freq_correct_factor()
{
    float ret = 1.0;
    uint8_t buffer[12];
    const spic_security_reg_t regs[3] = {SPIC_SECURITY_REG1, SPIC_SECURITY_REG2, SPIC_SECURITY_REG3};

    flash_init(QSPI0);
    for (int i = 0;i < 2;i++)
    {
        spic_read_security_reg(QSPI0, regs[i], (uint32_t)buffer, 0, 12);
        float t = *(float*)&buffer[0];
        uint32_t check_value_i = ~*(uint32_t*)&buffer[4];
        float check_value_f = *(float*)&check_value_i;
        if ((check_value_f == t) && (t > 0.90f) && (t < 1.10f))
        {
            ret = t;
            break;
        }
        
    }
    eclic_irq_disable(DMA_IRQn);
    ciss_set(CI_SS_FREQ_CLBT_FACTOR, *(uint32_t*)&ret);
    return ret;
}

/**
 * @brief 配置系统中断优先级
 * 
 */
void init_irq_pri(void)
{
    for (int i = SCU_IRQn;i <= PC_IRQn;i++ )
    {
        eclic_irq_set_priority(i, 6, 0);
    }
}


/**
 * @brief 配置总线时钟
 * 
 */
void init_clk_div(void)
{   
    #if (USE_V7 || USE_V5)
    uint32_t main_frequency = MAIN_FREQUENCY;
	#else
	uint32_t main_frequency = dpmu_get_pll_frequency()/get_freq_factor();
	// uint32_t main_frequency = MAIN_FREQUENCY;
	#endif

    /* PLL 480M ip_core 240M */
    set_ipcore_clk(main_frequency);
    
    /* AHB 240M */
    set_ahb_clk(main_frequency);

    /* APB 120M */
    void set_apb_clk(uint32_t clk);
    set_apb_clk(main_frequency/2);

    /* SRC 12.288M */
	#if (USE_V7 || USE_V5)
    void set_src_clk(uint32_t clk);
    set_src_clk(SRC_FREQUENCY);
	set_systick_clk(GET_SRC_CLK);
	
	#else

    /* 内核timer时钟 7.5M x 2(双边沿) */
    // set_systick_clk(main_frequency/12);
    set_systick_clk(get_src_clk()/128);
    #endif
}




/**
 * @brief 初始化系统
 * 
 */
void init_platform(void)
{    
    init_clk_div();
    init_irq_pri();
}

float get_freq_factor()
{
    uint32_t t = ciss_get(CI_SS_FREQ_CLBT_FACTOR);
    return *(float*)&t;
}


static uint32_t ipcore_clk;
static uint32_t ahb_clk;
static uint32_t apb_clk;
static uint32_t system_tick_clk;
static uint32_t osc_clk;
static uint32_t src_clk;


/**
 * @brief 获取ipcore时钟
 * 
 * @return uint32_t ipcore时钟
 */
uint32_t get_ipcore_clk(void)
{
    return ipcore_clk;
    //return (uint32_t)ciss_get(CI_SS_IPCORE_CLK);
}


/**
 * @brief 获取AHB时钟
 * 
 * @return uint32_t AHB时钟
 */
uint32_t get_ahb_clk(void)
{
    return ahb_clk;
    //return (uint32_t)ciss_get(CI_SS_AHB_CLK);
}


/**
 * @brief 获取APB时钟
 * 
 * @return uint32_t APB时钟
 */
uint32_t get_apb_clk(void)
{
    return apb_clk;
    //return (uint32_t)ciss_get(CI_SS_APB_CLK);
}


/**
 * @brief 获取systick时钟
 * 
 * @return uint32_t systick时钟
 */
uint32_t get_systick_clk(void)
{
    return system_tick_clk;
    //return (uint32_t)ciss_get(CI_SS_SYSTEMTICK_CLK);
}





/**
 * @brief 获取osc时钟
 * 
 * @return uint32_t osc时钟
 */
uint32_t get_osc_clk(void)
{
    return osc_clk;
    //return (uint32_t)ciss_get(CI_SS_EXT_OSC_CLK);
}


uint32_t get_src_clk(void)
{
    return src_clk;
    //return (uint32_t)ciss_get(CI_SS_SRC_CLK);
}

/**
 * @brief 设置IPCORE时钟
 * 
 * @param clk IPCORE时钟
 */
void set_ipcore_clk(uint32_t clk)
{
    ipcore_clk = clk;
    //ciss_set(CI_SS_IPCORE_CLK, (status_t)clk);
}


/**
 * @brief 设置AHB时钟
 * 
 * @param clk AHB时钟
 */
void set_ahb_clk(uint32_t clk)
{
    ahb_clk = clk;
    //ciss_set(CI_SS_AHB_CLK, (status_t)clk);
}


/**
 * @brief 
 * 
 * @param clk APB时钟
 */
void set_apb_clk(uint32_t clk)
{
    apb_clk = clk;
    //ciss_set(CI_SS_APB_CLK, (status_t)clk);
}

/**
 * @brief 设置SRC时钟
 * 
 * @param clk SRC时钟
 */
void set_src_clk(uint32_t clk)
{
    src_clk = clk;
    //ciss_set(CI_SS_SRC_CLK, (status_t)clk);
}


/**
 * @brief 设置晶振时钟
 * 
 * @param clk 晶振时钟
 */
void set_osc_clk(uint32_t clk)
{
    src_clk = clk;
    //ciss_set(CI_SS_EXT_OSC_CLK, (status_t)clk);
}

/**
 * @brief 设置systick时钟
 * 
 * @param clk systick时钟
 */
void set_systick_clk(uint32_t clk)
{
    system_tick_clk = clk;
    //ciss_set(CI_SS_SYSTEMTICK_CLK, (status_t)clk);
}

StaticTask_t timerTaskTCB;
StackType_t timerTaskStack[configMINIMAL_STACK_SIZE];
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &timerTaskTCB;
    *ppxTimerTaskStackBuffer = timerTaskStack;
    *pulTimerTaskStackSize = configMINIMAL_STACK_SIZE;
}

StaticTask_t idleTaskTCB;
StackType_t idleTaskStack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxTaskTCBBuffer, StackType_t **ppxTaskStackBuffer, uint32_t *pulTaskStackSize )
{
    *ppxTaskTCBBuffer = &idleTaskTCB;
    *ppxTaskStackBuffer = idleTaskStack;
    *pulTaskStackSize = configMINIMAL_STACK_SIZE; 
}

void init_romlib_freertos()
{
    // mprintf("init_romlib_freertos\n");
    /* 初始化操作系统库 */
    extern void vPortSetMSIPInt(void);
    extern void vPortClearMSIPInt(void);
    extern unsigned long taskswitch( unsigned long sp, unsigned long arg1);
    extern void vDoTaskSwitchContext( void );
    extern void vPortEnterCritical( void );
    extern void vPortExitCritical( void );
    extern void vPortClearInterruptMask(int int_mask);
    extern int xPortSetInterruptMask(void);
    extern StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );
    extern void prvTaskExitError( void );
    extern void vPortSetupTimer(void);
    extern void vPortSetupMSIP(void);
    extern void vPortSetup(void);
    extern BaseType_t xPortStartScheduler( void );
	extern void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );
	extern void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );
    extern void vPortEndScheduler( void );

    freertos_port_api_t freertos_port_api = {
        vPortSetMSIPInt,
        vPortClearMSIPInt,
        taskswitch,
        vDoTaskSwitchContext,
        vPortEnterCritical,
        vPortExitCritical,
        vPortClearInterruptMask,
        xPortSetInterruptMask,
        pxPortInitialiseStack,
        // prvTaskExitError,
        vPortSetupTimer,
        vPortSetupMSIP,
        // vPortSetup,
        xPortStartScheduler,
	    vApplicationGetIdleTaskMemory,
	    vApplicationGetTimerTaskMemory,
        vPortEndScheduler,
        _printf,
        pvPortMalloc,
        vPortFree,
    };

    /* 创建启动任务 */
    reg_port_func(&freertos_port_api);
}

/**
 * @brief 初始化 maskrom lib
 * 
 */
void maskrom_lib_init(void)
{
    //mprintf("maskrom lib mark %s!\n",MASK_ROM_LIB_FUNC->ci_lib_romruntime);
    //mprintf("maskrom lib ver %d!\n",MASK_ROM_LIB_FUNC->verison);
    extern int init_lib_romruntime(void);
    extern void MP3Lib_Set_Func(void* f1, void* f2,void* f3,void* f4,void* f5);
	extern void __malloc_lock(struct _reent *p);
	extern void __malloc_unlock(struct _reent *p);
    extern void NEWLib_Set_Func(void* f1,void* f2,void* f3);
	init_lib_romruntime();
	NEWLib_Set_Func((void *)sbrk,(void *)__malloc_lock,(void *)__malloc_unlock);
	MP3Lib_Set_Func((void *)(malloc), (void *)(free), (void *)(memcpy), (void *)(memmove), (void *)(memset));
    init_romlib_freertos();
}

