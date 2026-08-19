/**
 * @file ci13lc_it.h
 * @brief 中断服务函数
 * @version 1.0
 * @date 2018-05-21
 * 
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 * 
 */
#ifndef __CI13LC_IT_H
#define __CI13LC_IT_H


#ifdef __cplusplus
 extern "C" {
#endif 

void SCU_IRQHandler(void);
void NPU_IRQHandler(void);
void EPWM_IRQHandler(void);
void DMA_IRQHandler(void);
void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);

void IIC0_IRQHandler(void);
void PA_IRQHandler(void);
void PB_IRQHandler(void);
void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);
void IIS0_IRQHandler(void);                    
void IIS1_IRQHandler(void);
void IIS_DMA_IRQHandler(void);
void ALC_TIMEOUT_IRQHandler(void);
void DTR_IRQHandler(void);
void V11_OK_IRQHandler(void);
void VDT_IRQHandler(void);
void EXT0_IRQHandler(void);
void EXT1_IRQHandler(void);
void IWDG_IRQHandler(void);
void PVDC_IRQHandler(void);
void EFUSE_IRQHandler(void);
void PC_IRQHandler(void);

#ifdef __cplusplus
}
#endif 


#endif /* __CI13LC_IT_H */
