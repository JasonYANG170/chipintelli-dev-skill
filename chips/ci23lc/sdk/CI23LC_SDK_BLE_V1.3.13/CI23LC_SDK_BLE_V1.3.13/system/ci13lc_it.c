/**
 ******************************************************************************
 * @文件    ci13lc_it.c
 * @版本    V1.0.0
 * @日期    2022-2-11
 * @概要    这个文件是chipintelli公司的CI13LC芯片程序的各个外设中断服务函数.
 ******************************************************************************
 * @注意
 *
 * 版权归chipintelli公司所有，未经允许不得使用或修改
 *
 ******************************************************************************
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "ci_dma.h"
#include "ci_it.h"
#include "ci_iic.h"
#include "ci_iwdg.h"
#include "ci_log.h"
#include "ci_iisdma.h"

__attribute__((weak)) void SCU_IRQHandler(void){}
__attribute__((weak)) void NPU_IRQHandler(void){}
__attribute__((weak)) void EPWM_IRQHandler(void){}
__attribute__((weak)) void DMA_IRQHandler(void){}
__attribute__((weak)) void TIMER0_IRQHandler(void){}
__attribute__((weak)) void TIMER1_IRQHandler(void){}

__attribute__((weak)) void IIC0_IRQHandler(void){}
__attribute__((weak)) void PA_IRQHandler(void){}
__attribute__((weak)) void PB_IRQHandler(void){}
__attribute__((weak)) void UART0_IRQHandler(void){}
__attribute__((weak)) void UART1_IRQHandler(void){}
__attribute__((weak)) void UART2_IRQHandler(void){}
__attribute__((weak)) void IIS0_IRQHandler(void){}
__attribute__((weak)) void IIS1_IRQHandler(void){}
__attribute__((weak)) void IIS_DMA_IRQHandler(void){}
__attribute__((weak)) void ALC_TIMEOUT_IRQHandler(void){}
__attribute__((weak)) void DTR_IRQHandler(void){}
__attribute__((weak)) void V11_OK_IRQHandler(void){}
__attribute__((weak)) void VDT_IRQHandler(void){}
__attribute__((weak)) void EXT0_IRQHandler(void){}
__attribute__((weak)) void EXT1_IRQHandler(void){}
__attribute__((weak)) void IWDG_IRQHandler(void){}
__attribute__((weak)) void PVDC_IRQHandler(void){}
__attribute__((weak)) void EFUSE_IRQHandler(void){}
__attribute__((weak)) void PC_IRQHandler(void){}

/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
