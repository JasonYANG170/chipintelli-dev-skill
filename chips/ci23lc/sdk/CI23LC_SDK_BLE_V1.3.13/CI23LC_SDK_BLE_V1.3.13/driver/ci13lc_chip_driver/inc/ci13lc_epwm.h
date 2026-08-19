/**
 * @file ci13lc_epwm.h
 * @brief  EPWM驱动文件
 * @version 0.1
 * @date 2023-07-20
 *
 * @copyright Copyright (c) 2023  Chipintelli Technology Co., Ltd.
 *
 */

#ifndef _CI13LC_EPWM_H
#define _CI13LC_EPWM_H

#ifdef __cplusplus
 extern "C" {
#endif 
#include "ci_system.h"

/*中断使能选择*/
typedef enum
{
    EPWM_TZINT_CBC = 1, /*CBC中断，连续刹车，刹车信号不会自动清除*/
    EPWM_TZINT_OST = 2, /*one-shot中断，单点刹车，刹车信号保持一个周期，自动清除*/
}EPWM_TZINTx;

/*中断清除选择*/
typedef enum
{
    EPWM_TZCLR_INT = 0, /*EPWM中断*/ 
    EPWM_TZCLR_CBC = 1, /*CBC中断*/
    EPWM_TZCLR_OST = 2, /*one-shot中断*/
}EPWM_TZCLRx;

/*当计数器完成一个周期时计数器的行为*/
typedef enum
{
    /*停止计数*/
    EPWM_FREESOFT_STOP               = 0,
    /*递增模式，当counter = period(TBPRD)时停止。递减和增减模式，counter = 0时停止*/ 
    EPWM_FREESOFT_STOP_TBPRD_ZERO    = 1,
    /*继续运行*/  
    EPWM_FREESOFT_CONTINUE           = 2, 
}EPWM_FREESOFTx;

/*选择同步信号有效，或软件强制同步，或TBPHS刷新后，计数器的计数方向*/
/*（计数器运行在增减模式才有用）*/
typedef enum
{
    EPWM_PHSDIR_DEC = 0, /*递减模式*/
    EPWM_PHSDIR_INC = 1, /*递增模式*/
}EPWM_PHSDIRx;

/* 计数器计数基准时钟TBCLK分频参数，TBCLK = HCLK / CLKDIV */
typedef enum
{
    EPWM_CLKDIV1   = 0,
    EPWM_CLKDIV2   = 1,
    EPWM_CLKDIV4   = 2,
    EPWM_CLKDIV8   = 3,
    EPWM_CLKDIV16  = 4,
    EPWM_CLKDIV32  = 5,
    EPWM_CLKDIV64  = 6,
    EPWM_CLKDIV128 = 7,
}EPWM_CLKDIVx;

/* 计数器计数使能 */
typedef enum
{
    EPWM_CNTEN_STOP=0,
    EPWM_CNTEN_ENABLE=1,
}EPWM_CNTENx;

/*软件强制同步控制*/
typedef enum
{
    EPWM_SWFSYNC_NOSYNC = 0,
    EPWM_SWFSYNC_SYNC   = 1,
}EPWM_SWFSYNCx;

/*同步信号输出控制*/
typedef enum
{
    EPWM_SYNCOSEL_EPWMxSYNCI = 0, /*等于EPWMxSYNCI*/
    EPWM_SYNCOSEL_ZERO       = 1, /*counter = ZERO*/
    EPWM_SYNCOSEL_CMPB       = 2, /*counter = CMPB*/
    EPWM_SYNCOSEL_NOSYN      = 3, /*不输出同步信号*/
}EPWM_SYNCOSELx;

/*周期控制直接寄存器是否获取TBPRD寄存器值*/
typedef enum
{
    EPWM_PRDLD_ZERO = 0, /*counter为ZERO时控制直接寄存器才可以获取TBPRD的值*/
    EPWM_PRDLD_FREE = 1, /*任何时候周期控制直接寄存器都可以获取TBPRD的值*/
}EPWM_PRDLDx;

/*TBPHS赋值给counter使能*/
typedef enum
{
    EPWM_PHSEN_INVALID   = 0, /*禁止TBPHS赋值给counter*/
    EPWM_PHSEN_SYNCVALID = 1, /*当EPWMxSYNCI有效或软件强制同步时，将TBPHS赋值给counter*/
}EPWM_PHSENx;

/*计数器counter计数方式*/
typedef enum
{
    EPWM_CTRMODE_INC     = 0, /*递增模式*/
    EPWM_CTRMODE_DEC     = 1, /*递减模式*/
    EPWM_CTRMODE_INC_DEC = 2, /*增减模式*/
    EPWM_CTRMODE_STOP    = 3, /*停止计数*/
}EPWM_CTRMODEx;

typedef struct
{
    EPWM_FREESOFTx FREESOFT;
    EPWM_PHSDIRx PHSDIR;
    EPWM_CLKDIVx CLKDIV;
    EPWM_CNTENx CNTEN;
    EPWM_SWFSYNCx SWFSNC;
    EPWM_SYNCOSELx SYNCOSEL; 
    EPWM_PRDLDx PRDLD;
    EPWM_PHSENx PHSEN;
    EPWM_CTRMODEx CTRMODE;    
}epwm_tbctl_init_t;

/*CMPA、CMPB寄存器模式选择*/
typedef enum
{
    EPWM_SHDWABMODE_DIRECT   = 0, /*间接模式，写CMPA、CMPB的值不会立刻给直接寄存器*/
    EPWM_SHDWABMODE_INDIRECT = 1, /*直接模式，写CMPA、CMPB的值立刻生效于直接寄存器*/
}EPWM_SHDWABMODEx;

/*在何时获取CMPA或CMPB的值（当SHDWBMODE = 0时有效，即间接模式）*/
typedef enum
{
    EPWM_LOADABMODE_ZERO     = 0, /*当counter = ZERO时*/
    EPWM_LOADABMODE_PRD      = 1, /*当counter = PRD时*/
    EPWM_LOADABMODE_ZERO_PRD = 2, /*当counter = ZERO或counter = PRD时*/
    EPWM_LOADABMODE_NONE     = 3, /*永不*/
}EPWM_LOADABMODEx;

typedef struct
{
    EPWM_SHDWABMODEx SHDWBMODE;
    EPWM_SHDWABMODEx SHDWAMODE;
    EPWM_LOADABMODEx LOADBMODE;
    EPWM_LOADABMODEx LOADAMODE;
}epwm_cmpctrl_init_t;

/*EPWMx输出的操作*/
typedef enum
{
    EPWM_OUT_NONE = 0, /*无动作*/
    EPWM_OUT_LOW  = 1, /*置低*/
    EPWM_OUT_HIGH = 2, /*置高*/
    EPWM_OUT_EDG  = 3, /*翻转*/
}EPWM_OUTx;

typedef struct
{
    EPWM_OUTx CBD; /*当计数器counter=CMPB，递减阶段时EPWMx输出的操作*/
    EPWM_OUTx CBU; /*当计数器counter=CMPB，递增阶段时EPWMx输出的操作*/
    EPWM_OUTx CAD; /*当计数器counter=CMPA，递减阶段时EPWMx输出的操作*/
    EPWM_OUTx CAU; /*当计数器counter=CMPA，递增阶段时EPWMx输出的操作*/
    EPWM_OUTx PRD; /*当计数器counter=TBPRD，EPWMx输出的操作*/
    EPWM_OUTx ZRO; /*当计数器counter=ZERO，EPWMx输出的操作*/
}epwm_aqctlx_init_t;

/*AQSFRC何时赋值给AQSFRC直接寄存器并生效*/
typedef enum
{
    EPWM_RLDCSF_ZERO        = 0, /*counter = ZERO时*/
    EPWM_RLDCSF_PRD         = 1, /*counter = PRD时*/
    EPWM_RLDCSF_ZERO_PDR    = 2, /*当counter = ZERO或counter = PRD时*/
    EPWM_RLDCSF_IMMEDIATELY = 3, /*直接赋值，立即生效*/
}EPWM_RLDCSFx;

/*软件强制控制EPWMA或EPWMB*/
typedef enum
{
    EPWM_OTSFAB_NONE  = 0, /*无操作*/
    EPWM_OTSFAB_FORCE = 1, /*产生软件强制信号*/
}EPWM_OTSFABx;

/*选择当软件强制信号产生后的操作*/
typedef enum
{
    EPWM_ACTSFAB_NONE = 0,  /*无操作*/
    EPWM_ACTSFAB_LOW  = 1,  /*置低*/
    EPWM_ACTSFAB_HIGH = 2,  /*置高*/
    EPWM_ACTSFAB_EDG  = 3,  /*翻转*/  
}EPWM_ACTSFABx;

typedef struct
{
    EPWM_RLDCSFx RLDCSF;
    EPWM_OTSFABx OTSFB;
    EPWM_ACTSFABx ACTSFB;    
    EPWM_OTSFABx OTSFA;
    EPWM_ACTSFABx ACTSFA;
}epwm_aqsfrc_init_t;

/*软件强制控制EPWMxA或EPWMxA*/
typedef enum
{
    EPWM_CSFAB_NONE     =0, /*无操作*/
    EPWM_CSFAB_LOW      =1, /*置低*/
    EPWM_CSFAB_HIGH     =2, /*置高*/
    EPWM_CSFAB_INVALID  =3, /*软件强制无效*/
}EPWM_CSFABx;

typedef struct
{
    EPWM_CSFABx CSFB;
    EPWM_CSFABx CSFA;
}epwm_aqcsfrc_init_t;

/*dead-band输入模式选择*/
typedef enum
{
    EPWM_IN_MODE_EPWMxA_FALLING_A_RISING = 0, /*EPWMxA上升沿与下降沿都延迟*/
    EPWM_IN_MODE_EPWMxA_FALLING_B_RISING = 1, /*EPWMxB上升沿延迟，EPWMxB下降沿延迟*/
    EPWM_IN_MODE_EPWMxB_FALLING_A_RISING = 2, /*EPWMxA上升沿延迟，EPWMxB下降沿延迟*/
    EPWM_IN_MODE_EPWMxB_FALLING_B_RISING = 0, /*EPWMxB上升沿与下降沿都延迟*/
}EPWM_IN_MODEx;

/*极性选择*/
typedef enum
{
    EPWM_POLSEL_NOOPPOSITEEPWMxAB = 0, /*EPWMxA、EPWMxB均不反向*/
    EPWM_POLSEL_OPPOSITEEPWMxA    = 1, /*EPWMxA反向*/
    EPWM_POLSEL_OPPOSITEEPWMxB    = 2, /*EPWMxB反向*/
    EPWM_POLSEL_OPPOSITEEPWMxAB   = 3, /*EPWMxA、EPWMxB均反向*/
}EPWM_POLSELx;

/*dead-band输出模式选择，EPWMxA、EPWMxB*/
typedef enum
{
    EPWM_OUT_MODE_CLOSE         = 0, /*关闭所有延时功能*/
    EPWM_OUT_MODE_FALLING       = 1, /*关闭上升沿延时功能*/
    EPWM_OUT_MODE_RISING        = 2, /*关闭下降沿延时功能*/
    EPWM_OUT_MODE_FALLINGRISING = 3, /*同时开下降沿、下降沿延时功能*/
}EPWM_OUT_MODEx;

typedef struct
{
    EPWM_IN_MODEx IN_MODE;
    EPWM_POLSELx POLSEL;
    EPWM_OUT_MODEx OUT_MODE;
}epwm_dbctl_init_t;

/*刹车的模式*/
typedef enum
{
    EPWM_MODE_IMMEDIATELY = 0, /*立即刹车*/
    EPWM_MODE_CTRZERO     = 1, /*等待 CTR = 0 时刹车*/
}EPWM_MODEx;

/*控制每个刹车信号的开关，触发后，会持续直到用户清除TZCLR寄存器*/
typedef enum
{
    EPWM_TZx_n_CLOSE = 0, /*关闭TZx_n作为One-Shot信号*/
    EPWM_TZx_n_OPEN  = 1, /*打开TZx_n作为One-Shot信号*/
}EPWM_TZx_n;

typedef struct
{
    EPWM_TZx_n TZx_CBC[7];
    EPWM_MODEx CBCMOD;   /*Cycle-by-Cycle(CBC)刹车的模式*/
    EPWM_TZx_n TZx_OSHT[7];
    EPWM_MODEx OSHTMOD;  /*One-Shot(OSHT)刹车的模式*/
}epwm_tzsel_init_t;

// typedef enum
// {
//     EPWM_TZn_EN_DISABLE =0,
//     EPWM_TZn_EN_ENABLE  =1,
// }EPWM_TZn_ENx;

// typedef enum
// {
//     EPWM_TZn_SEL_HIGH   =0,
//     EPWM_TZn_SEL_LOW    =1,
// }EPWM_TZn_SELx;

/*当刹车命令到来时，对EPWMxA、EPWMxB输出的动作*/
typedef enum
{
    EPWM_TZx_HIGHZ  =0,  /*高阻态*/
    EPWM_TZx_HIGH   =1,  /*置高*/
    EPWM_TZx_LOW    =2,  /*置低*/
    EPWM_TZx_NONE   =3,  /*无动作*/
}EPWM_TZABx;

typedef struct
{
    // EPWM_TZn_ENx TZn_EN[7];
    // EPWM_TZn_SELx TZn_SEL[7]; 
    EPWM_TZABx TZB;
    EPWM_TZABx TZA;
}epwm_tzctl_init_t;

/*EPWMxSOCA、EPWMxSOCB控制ADC使能*/
typedef enum
{
    EPWM_SOCABEN_DISABLE    =0,
    EPWM_SOCABEN_ENABLE     =1,
}EPWM_SOCABENx;

typedef enum
{
    EPWM_SOCA=0,
    EPWM_SOCB=1,
}EPWM_SOCx;

/*EPWMxSOCA、EPWMxSOCB在何时产生脉冲*/
typedef enum
{
    EPWM_SOCABSEL_ZERO      = 0x1, /*time-base计数值等于0*/
    EPWM_SOCABSEL_PRD       = 0x2, /*time-base计数值等于PRD*/
    EPWM_SOCABSEL_ZERO_PRD  = 0x3, /*time-base计数值等于0或PRD*/
    EPWM_SOCABSEL_CMPAINC   = 0x4, /*time-base计数值等于CMPA且递增*/
    EPWM_SOCABSEL_CMPADEC   = 0x5, /*time-base计数值等于CMPA且递减*/
    EPWM_SOCABSEL_CMPBINC   = 0x6, /*time-base计数值等于CMPB且递增*/
    EPWM_SOCABSEL_CMPBDEC   = 0x7, /*time-base计数值等于CMPB且递减*/
    EPWM_SOCABSEL_CPR1INC   = 0x8, /*time-base计数值等于CPR1且递增*/
    EPWM_SOCABSEL_CPR1DEC   = 0x9, /*time-base计数值等于CPR1且递减*/
    EPWM_SOCABSEL_CPR2INC   = 0xA, /*time-base计数值等于CPR2且递增*/
    EPWM_SOCABSEL_CPR2DEC   = 0xB, /*time-base计数值等于CPR2且递减*/
}EPWM_SOCABSELx;

/*ePWM中断使能*/
typedef enum
{
    EPWM_INTEN_DISABLE  =0,
    EPWM_INTEN_ENABLE   =1,
}EPWM_INTENx;

/*ePWM中断在何时产生*/
typedef enum
{
    EPWM_INTSEL_ZERO     = 1, /*time-base计数值等于0*/
    EPWM_INTSEL_PRD      = 2, /*time-base计数值等于PRD*/
    EPWM_INTSEL_ZERO_PRD = 3, /*time-base计数值等于0或PRD*/
    EPWM_INTSEL_CMPAINC  = 4, /*time-base计数值等于CMPA且递增*/
    EPWM_INTSEL_CMPADEC  = 5, /*time-base计数值等于CMPA且递减*/
    EPWM_INTSEL_CMPBINC  = 6, /*time-base计数值等于CMPB且递增*/
    EPWM_INTSEL_CMPBDEC  = 7, /*time-base计数值等于CMPB且递减*/
}EPWM_INTSELx;

typedef struct
{
    EPWM_SOCABENx SOCBEN;
    EPWM_SOCABSELx SOCBSEL;  
    EPWM_SOCABENx SOCAEN;
    EPWM_SOCABSELx SOCASEL;
    EPWM_INTENx INTEN;
    EPWM_INTSELx INTSEL;
}epwm_etsel_init_t;

/*事件计数器*/
typedef enum
{
    EPWM_INT_SOCABCNT_0 = 0,
    EPWM_INT_SOCABCNT_1 = 1,
    EPWM_INT_SOCABCNT_2 = 2,
    EPWM_INT_SOCABCNT_3 = 3,
}EPWM_INT_SOCABCNTx;

/*设定多少个ETSEL[SOCASEL]事件触发一次ADC*/
typedef enum
{
    EPWM_INT_SOCABPRD_NONE       = 0, /*从不产生EPWMxSOCA或EPWMxSOCB脉冲*/
    EPWM_INT_SOCABPRD_SOCABCNT01 = 1, /*当ETPS[SOCACNT]=01时，产生EPWMxSOCA或EPWMxSOCB脉冲*/
    EPWM_INT_SOCABPRD_SOCABCNT10 = 2, /*当ETPS[SOCACNT]=10时，产生EPWMxSOCA或EPWMxSOCB脉冲*/
    EPWM_INT_SOCABPRD_SOCABCNT11 = 3, /*当ETPS[SOCACNT]=11时，产生EPWMxSOCA或EPWMxSOCB脉冲*/
}EPWM_INT_SOCABPRDx;

typedef struct
{
    EPWM_INT_SOCABCNTx SOCBCNT;
    EPWM_INT_SOCABPRDx SOCBPRD;
    EPWM_INT_SOCABCNTx SOCACNT;
    EPWM_INT_SOCABPRDx SOCAPRD;
    EPWM_INT_SOCABCNTx INTCNT;
    EPWM_INT_SOCABPRDx INTPRD;
}epwm_etps_init_t;

/*状态标记*/
typedef enum
{
    EPWM_ETSTATUS_SOCB = 3,
    EPWM_ETSTATUS_SOCA = 2,
    EPWM_ETSTATUS_INT  = 0
}EPWM_ETSTATUSx;

typedef enum
{
    EPWM_SOCAB_NONE=0,
    EPWM_SOCAB_FORCE=1,
}EPWM_SOCABx;

typedef struct
{
    EPWM_SOCABx SOCB;
    EPWM_SOCABx SOCA;
}epwm_etfrc_init_t;

typedef struct
{
    unsigned short TBPRD;  
    unsigned short CMPA;
    unsigned short CMPB;
    unsigned short CPR1;
    unsigned short CPR2;
    unsigned short DBRED;
    unsigned short DBFED;
    unsigned short TBPHS; 
    /*设定在EPWMxSYNCI同步是，time_gen计数器的初始值，控制PWM的相位。当TBCTL[PHSEN]=0，忽略同步信号。
    当TBCTL[PHSEN]=1，当EPWMxSYNCI有效或软件强制同步时，计数器值等于TBPHS*/
    epwm_tbctl_init_t TBCTL;
    
    epwm_cmpctrl_init_t CMPCTL;
    
    epwm_aqctlx_init_t AQCTLA;
    epwm_aqctlx_init_t AQCTLB;
    
    epwm_aqsfrc_init_t AQSFRC;
    epwm_aqcsfrc_init_t AQCSFRC;
    epwm_dbctl_init_t DBCTL;
    epwm_tzsel_init_t TZSEL;
    epwm_tzctl_init_t TZCTL;
    epwm_etsel_init_t ETSEL;
    epwm_etps_init_t ETPS;
    epwm_etfrc_init_t ETFRC;
}epwm_init_t;


void epwm_soc_config(EPWM_TypeDef* epwmx,EPWM_SOCx socx,EPWM_SOCABSELx socsel);
void epwm_tzeint_enable(EPWM_TypeDef* epwmx,EPWM_TZINTx tzint,FunctionalState cmd);
void epwm_tzclr_clear(EPWM_TypeDef* epwmx,EPWM_TZCLRx tzclr);
void epwm_cpr1_config(EPWM_TypeDef* epwmx,unsigned short cpr1val);
void epwm_cpr2_config(EPWM_TypeDef* EPWMx,unsigned short cpr2val);
void epwm_aqsfrc_config(EPWM_TypeDef* EPWMx,epwm_aqsfrc_init_t* aqsfrc_init);
void epwm_aqcsfrc_config(EPWM_TypeDef* EPWMx,epwm_aqcsfrc_init_t* aqcsfrc_init);
void epwm_dbctl_config(EPWM_TypeDef* EPWMx,epwm_dbctl_init_t* dbctl_init);
void epwm_dbred_config(EPWM_TypeDef* EPWMx,unsigned short dbred);
void epwm_dbfed_config(EPWM_TypeDef* EPWMx,unsigned short dbfed);
void epwm_tzsel_config(EPWM_TypeDef* EPWMx,epwm_tzsel_init_t* tzsel_init);
void epwm_tzctl_config(EPWM_TypeDef* EPWMx,epwm_tzctl_init_t* tzctl_init);
void epwm_tzfrc_enable(EPWM_TypeDef* EPWMx,EPWM_TZCLRx tz);
int epwm_get_tzflag(EPWM_TypeDef* EPWMx,EPWM_TZCLRx tz);
void epwm_etsel_config(EPWM_TypeDef* EPWMx,epwm_etsel_init_t* etsel_init);
void epwm_etsel_interrupt_enable(EPWM_TypeDef* EPWMx,FunctionalState cmd);
void epwm_etps_config(EPWM_TypeDef* EPWMx,epwm_etps_init_t* etps_init);
void epwm_etclr_clear(EPWM_TypeDef* EPWMx,EPWM_ETSTATUSx etstatus);
void epwm_etclr_clear_all(EPWM_TypeDef* EPWMx);
void epwm_etfrc_config(EPWM_TypeDef* EPWMx,epwm_etfrc_init_t* etfrc_init);
void epwm_init(EPWM_TypeDef* epwmx,epwm_init_t* EPWMInit_Struct);


void epwm_tbctl_config(EPWM_TypeDef* EPWMx,epwm_tbctl_init_t* tbctl_init);
void epwm_tbprd_config(EPWM_TypeDef* EPWMx,unsigned short tbprd);
void epwm_cmpa_config(EPWM_TypeDef* epwmx,unsigned short cmpaval);
void epwm_cmpb_config(EPWM_TypeDef* EPWMx,unsigned short cmpbval);
void epwm_aqctla_config(EPWM_TypeDef* EPWMx,epwm_aqctlx_init_t* aqctla_init);
void epwm_aqctlb_config(EPWM_TypeDef* EPWMx,epwm_aqctlx_init_t* aqctlb_init);
void epwm_cmpctrl_config(EPWM_TypeDef* EPWMx,epwm_cmpctrl_init_t* cmpctrl_init);
void epwm_start(EPWM_TypeDef* EPWMx);
void epwm_stop(EPWM_TypeDef* EPWMx);


#ifdef __cplusplus
}
#endif


#endif /*_CI13LC_EPWM_H*/
