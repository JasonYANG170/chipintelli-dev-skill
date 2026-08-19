/**
 * @file ci13lc_init.c
 * @brief C环境启动预初始
 * @version 1.0.0
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ci_system.h"
#include "ci_core_eclic.h"
#include "ci_core_misc.h"
#include "ci_core_timer.h"
#include "ci_scu.h"
#include "ci_uart.h"
#include "ci_spiflash.h"
#include "sdk_default_config.h"
#include "ci_dpmu.h"
#include "crc.h"
#include "ci_log.h"
#include "flash_rw_process.h"
#include "board.h"
#include "status_share.h"


void _init()
{
#if (USE_V5 || USE_V7)
#else
    // #ifndef NO_RC_CLOCK_CONFIG
    extern char SHARE_SRAM_ADDR;
    extern char SHARE_SRAM_SIZE;
    memset(&SHARE_SRAM_ADDR,0x00,&SHARE_SRAM_SIZE);

	// //dpmu_rc_freq_sel(DPMU_RC_FREQ_12d288M);

    extern void board_clk_source_set(void);
    board_clk_source_set();

    // #if USE_LOWPOWER_DOWN_FREQUENCY
    // dpmu_pll_config(SRC_FREQUENCY_LOWPOWER, MAIN_FREQUENCY);
    // set_src_clk(SRC_FREQUENCY_LOWPOWER);
    // set_osc_clk(SRC_FREQUENCY_LOWPOWER);
    // #else
    // set_apb_clk(SRC_FREQUENCY_NORMAL/2);
    #if (USE_EXTERNAL_CRYSTAL_OSC == 0)
    load_freq_correct_factor();
    #else
    float ret = 1.0;
    ciss_set(CI_SS_FREQ_CLBT_FACTOR, *(uint32_t*)&ret);
    #endif
    dpmu_pll_config((uint32_t)(SRC_FREQUENCY_NORMAL/get_freq_factor()), MAIN_FREQUENCY);
    set_src_clk(SRC_FREQUENCY_NORMAL);
    // set_osc_clk(SRC_FREQUENCY_NORMAL);
	// #endif
    // #endif
#endif

    /* ECLIC init */
    eclic_init(ECLIC_NUM_INTERRUPTS);
    eclic_mode_enable();

    disable_mcycle_minstret();

    //SystemInit();

    // init_clk_div();
    init_irq_pri();

    /* 设置中断优先级分组 */
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO0);

    /* 开启全局中断 */
    eclic_global_interrupt_enable();

    enable_mcycle_minstret();

#ifdef STDOUT_INTERFACE
#if (STDOUT_INTERFACE == HAL_UART0_BASE) || (STDOUT_INTERFACE == HAL_UART1_BASE) || (STDOUT_INTERFACE == HAL_UART2_BASE)
    UARTPollingConfig((UART_TypeDef*)STDOUT_INTERFACE, UART_BaudRate921600);
#endif
#endif
}

void _fini()
{
}


void SystemInit(void)
{ 
    /*scu关闭时钟*/
    scu_unlock_clk_config();
    scu_unlock_reset_config();
    scu_unlock_system_config();
    SCU->AHB_CLKGATE_CFG &= ~(0x1F << 1);   //DSU不关
    SCU->APB0_CLKGATE_CFG &= ~(0x7FF << 0);   //CODEC不关
    SCU->APB1_CLKGATE_CFG &= ~(0x71B << 0);   //IIS0、IIS1、UART0不关
    scu_lock_clk_config();
    scu_lock_reset_config();
    scu_lock_system_config();

    dpmu_unlock_cfg_config();
    DPMU->AON_CLKGATE_CFG &= ~(0x09 << 0);    /*dpmu关闭时钟，iwdg和efuse不关*/
    DPMU->PAD_FILTER_CFG_ADDR |= (0x1 << 10);  /*设置复位脚防抖动功能*/
    dpmu_lock_cfg_config();
}

extern char _text_in_flash_start;
extern char _text_in_flash_end;
//#pragma GCC optimize("O0")
void debug_mode_init_code_in_flash() 
{
    uint32_t dst_addr = (uint32_t)&_text_in_flash_start - 0x50000000;
    uint32_t erase_addr = dst_addr & 0xFFFFF000;
    uint32_t size = (uint32_t)&_text_in_flash_end - (uint32_t)&_text_in_flash_start;
    if (size <= 0 || strncmp((char*)0x1ff50a00, "Debugging", 9))
    {
        return;
    }

    uint32_t erase_size = ((uint32_t)&_text_in_flash_end - 0x50000000 - erase_addr + 4095) & 0xFFFFF000;
    uint32_t src_addr = dst_addr - 0x4010 + 0x1ff51000; //SRAM_START_ADDR            = 0x1ff51000;
    {
        uint32_t asr_bin_addr = 0;
        memcpy(&asr_bin_addr, (void*)(0x50000000 + 0x2000 + 0xCC), 4);
        if (erase_addr + erase_size > asr_bin_addr)
        {
            while(1) asm volatile ("ebreak");
        }
        void flash_config_to_normal(void);
        flash_config_to_normal();
        spic_protect(QSPI0, DISABLE);
        flash_erase(QSPI0, erase_addr, erase_size);
        flash_write(QSPI0, dst_addr, src_addr, size);
        uint8_t zero = 0;
        flash_write(QSPI0, 0x2000+0xB6, (uint32_t)&zero, 1);
        extern void flash_init_to_xip(void);
        flash_config_to_xip();
    }
}