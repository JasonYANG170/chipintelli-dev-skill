/**
 ******************************************************************************
 * @文件    ci130x_it.c
 * @版本    V1.0.0
 * @日期    2022-2-11
 * @概要    这个文件是chipintelli公司的CI130X芯片程序的各个外设中断服务函数.
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
#include "ci130x_uart.h"
#include "ci130x_dma.h"
#include "ci130x_it.h"
#include "ci130x_iic.h"
#include "ci130x_TimerWdt.h"
#include "ci130x_iwdg.h"
#include "ci_log.h"
#include "ci130x_iisdma.h"


pUartCallBack g_Uart0Idle = NULL;
pUartCallBack g_Uart1Idle = NULL;
pUartCallBack g_Uart2Idle =NULL;
void	Chip_Regist_Uart0CallBack(pUartCallBack dUartCB)
{
	g_Uart0Idle = (pUartCallBack)dUartCB;
}

void	Chip_Regist_Uart1CallBack(pUartCallBack dUartCB)
{
	g_Uart1Idle = (pUartCallBack)dUartCB;
}
void	Chip_Regist_Uart2CallBack(pUartCallBack dUartCB)
{
	g_Uart2Idle = (pUartCallBack)dUartCB;
}

__attribute__((weak)) void MAIL_BOX_IRQHandler(void){}
__attribute__((weak)) void IWDG_IRQHandler(void){}
__attribute__((weak)) void TWDG_IRQHandler(void){}
__attribute__((weak)) void SCU_IRQHandler(void){}
__attribute__((weak)) void NPU_IRQHandler(void){}
__attribute__((weak)) void ADC_IRQHandler(void){}
__attribute__((weak)) void DMA_IRQHandler(void){
	dma_irq_handler();
}
__attribute__((weak)) void TIMER0_IRQHandler(void){}
__attribute__((weak)) void TIMER1_IRQHandler(void){}
__attribute__((weak)) void TIMER2_IRQHandler(void){}
__attribute__((weak)) void TIMER3_IRQHandler(void){}
__attribute__((weak)) void IIC0_IRQHandler(void){}//有点大
__attribute__((weak)) void PA_IRQHandler(void){}//有点大
__attribute__((weak)) void PB_IRQHandler(void){}//有点大
__attribute__((weak)) void UART0_IRQHandler(void)
{
	/*发送数据*/
	if(g_Uart0Idle)
	{
		if (UART_MaskIntState(UART0,UART_TXInt))
		{
			UART_IntClear(UART0,UART_TXInt);
		}
		/*接受数据*/
		if (UART_MaskIntState(UART0,UART_RXInt))
		{
			//here FIFO DATA must be read out
			while(0 == (UART0->UARTFlag & (0x1 << 6)))
			{
				g_Uart0Idle(UART_RXDATA(UART0));
			}
			UART_IntClear(UART0,UART_RXInt);
		}
		UART_IntClear(UART0,UART_AllInt);
	}
}//有点大
__attribute__((weak)) void UART1_IRQHandler(void)
{
	if(g_Uart1Idle)
	{
		if (UART_MaskIntState(UART1,UART_TXInt))
		{
			mprintf("uart rev overrun!\n");
			UART_IntClear(UART1,UART_TXInt);
		}

		/*接受数据*/
		if (UART_MaskIntState(UART1,UART_RXInt))
		{
			//here FIFO DATA must be read out
			while(0 == (UART1->UARTFlag & (0x1 << 6)))
			{
				g_Uart1Idle(UART_RXDATA(UART1));
			}
			UART_IntClear(UART1,UART_RXInt);
		}

		UART_IntClear(UART1,UART_AllInt);
	}
}
__attribute__((weak)) void UART2_IRQHandler(void)
{
	if(g_Uart2Idle)
	{
		if (UART_MaskIntState(UART2,UART_TXInt))
		{
			mprintf("uart rev overrun!\n");
			UART_IntClear(UART2,UART_TXInt);
		}

		/*接受数据*/
		if (UART_MaskIntState(UART2,UART_RXInt))
		{
			//here FIFO DATA must be read out
			while(0 == (UART2->UARTFlag & (0x1 << 6)))
			{			
				g_Uart2Idle(UART_RXDATA(UART2));
			}
			UART_IntClear(UART2,UART_RXInt);
		}
		UART_IntClear(UART2,UART_AllInt);
	}
}
__attribute__((weak)) void IIS0_IRQHandler(void){}
__attribute__((weak)) void IIS1_IRQHandler(void){}
__attribute__((weak)) void IIS2_IRQHandler(void){}
__attribute__((weak)) void IIS_DMA_IRQHandler(void){}//有点大
__attribute__((weak)) void ALC_IRQHandler(void){}
__attribute__((weak)) void PDM_IRQHandler(void){}
__attribute__((weak)) void DTR_IRQHandler(void){}
__attribute__((weak)) void V11_OK_IRQHandler(void){}
__attribute__((weak)) void VDT_IRQHandler(void){}
__attribute__((weak)) void EXT0_IRQHandler(void){}
__attribute__((weak)) void EXT1_IRQHandler(void){}
__attribute__((weak)) void AON_TIM_INT0_IRQHandler(void){}
__attribute__((weak)) void AON_TIM_INT1_IRQHandler(void){}
__attribute__((weak)) void AON_EFUSE_IRQHandler(void){}
__attribute__((weak)) void AON_PC_IRQHandler(void){}

/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
