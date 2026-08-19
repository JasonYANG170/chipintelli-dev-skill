/**
 * @file ci13lc_epwm.c
 * @brief  PWM驱动文件
 * @version 0.1
 * @date 2023-07-11
 *
 * @copyright Copyright (c) 2023  Chipintelli Technology Co., Ltd.
 *
 */

#include "ci13lc_epwm.h"

/**
  * @功能：EPWMx 的触发信号的触发时刻选择
  * @注意:无  
  * @参数：1.EPWMx：EPWM组
  *           2.socx：EPWM触发的PWM波选择，EPWM_SOCA , EPWM_SOCB
  *        3.socsel：触发时刻选择
  * @返回值：无
  */
void epwm_soc_config(EPWM_TypeDef* EPWMx,EPWM_SOCx socx,EPWM_SOCABSELx socsel)
{
    EPWMx->ETSEL &= ~(0xF<<(6+5*socx));
    EPWMx->ETSEL |= (socsel<<(6+5*socx));
}

/**
  * @注意:无  
  * @功能：EPWM刹车中断使能控制
  * @参数：1.EPWMx:EPWM组
  *        2. EPWMtzint，刹车中断类型选择 
  *        3. cmd ，ENABLE 中断使能；DISABLE 禁止中断
  * @返回：无
  */
void epwm_tzeint_enable(EPWM_TypeDef* EPWMx,EPWM_TZINTx tzint,FunctionalState cmd)
{
    if(cmd!=ENABLE)
    {
        EPWMx->TZEINT &=~(1<<tzint);
    }
    else
    {
        EPWMx->TZEINT |=(1<<tzint);
    }
}

/**
  * @功能：EPWM刹车中断状态清除
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.tzclr 中断状态选择
  * @返回值：无
  */
void epwm_tzclr_clear(EPWM_TypeDef* EPWMx,EPWM_TZCLRx tzclr)
{
    EPWMx->TZCLR |=(1<<tzclr);
}

/**
  * @功能：EPWM 控制寄存器配置
  * @注意:无  
  *@参数：1.EPWMx:EPWM组
  *       2.tbctl_init 初始化结构体指针
  * @返回值:无
  */
void epwm_tbctl_config(EPWM_TypeDef* EPWMx,epwm_tbctl_init_t* tbctl_init)
{
      EPWMx->TBCTL = (tbctl_init->FREESOFT<<14)|\
      (tbctl_init->PHSDIR<<13)|\
      (tbctl_init->CLKDIV <<10)|\
      (tbctl_init->CNTEN<<9)|\
      (tbctl_init->SWFSNC<<6)|\
      (tbctl_init->SYNCOSEL <<4)|\
      (tbctl_init->PRDLD<<3)|\
      (tbctl_init->PHSEN<<2)|\
      (tbctl_init->CTRMODE<<0);        
}

/**
  * @功能：EPWM计数周期值配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组，EPWM1,EPWM2,EPWM3,EPWM(
  *         EPWM同时映射到EPWM1,EPWM2,EPWM3)
  *        2.tbprd  计数周期值 16位
  * @返回值：无
  */
void epwm_tbprd_config(EPWM_TypeDef* EPWMx,unsigned short tbprd)
{
    EPWMx->TBPRD = tbprd & 0xffff;
}

/**
  * @功能：EPWM比较控制寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.cmpctrl_init 初始化结构体指针
  * @返回值：无
  */
void epwm_cmpctrl_config(EPWM_TypeDef* EPWMx,epwm_cmpctrl_init_t* cmpctrl_init)
{
    EPWMx->CMPCTL = (cmpctrl_init->SHDWBMODE>>6)|\
      (cmpctrl_init->SHDWAMODE>>4)|\
      (cmpctrl_init->LOADBMODE>>2)|\
      (cmpctrl_init->LOADAMODE>>0);
}

/**
  * @功能：EPWM比较寄存器CMPA的值配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.cmpaval 比较值，16位
  *@返回值：无
  */
void epwm_cmpa_config(EPWM_TypeDef* EPWMx,unsigned short cmpaval)
{
    EPWMx->CMPA = cmpaval & 0xffff;
}

/**
  * @功能：EPWM比较寄存器CMPB的值配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.cmpbval 比较值，16位
  * @返回值：无
  */
void epwm_cmpb_config(EPWM_TypeDef* EPWMx,unsigned short cmpbval)
{
    EPWMx->CMPB = cmpbval & 0xffff ;
}

/**
  * @功能：EPWM的cpr1寄存器的值配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.cpr1val 比较值，16位
  * @返回值：无
  */
void epwm_cpr1_config(EPWM_TypeDef* EPWMx,unsigned short cpr1val)
{
    EPWMx->CPR1 = cpr1val & 0xffff;
}

/**
  * @功能：EPWM的cpr2寄存器的值配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组，EPWM1,EPWM2,EPWM3,EPWM(
  *         EPWM同时映射到EPWM1,EPWM2,EPWM3)
  *        2.cpr2val 比较值，16位
  * @返回值：无
  */
void epwm_cpr2_config(EPWM_TypeDef* EPWMx,unsigned short cpr2val)
{
    EPWMx->CPR2 = cpr2val & 0xffff;
}

/**
  * @功能：EPWM的AQCTLA寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.aqctla_init 初始化结构体指针
  * @返回值：无
  */
void epwm_aqctla_config(EPWM_TypeDef* EPWMx,epwm_aqctlx_init_t* aqctla_init)
{
    EPWMx->AQCTLA = (aqctla_init->CBD<<10)|\
      (aqctla_init->CBU <<8)|\
      (aqctla_init->CAD <<6)|\
      (aqctla_init->CAU <<4)|\
      (aqctla_init->PRD <<2)|\
      (aqctla_init->ZRO <<0);
}

/**
  * @功能：EPWM的AQCTLB寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.aqctlb_init 初始化结构体指针
  * @返回值：无
  */
void epwm_aqctlb_config(EPWM_TypeDef* EPWMx,epwm_aqctlx_init_t* aqctlb_init)
{
    EPWMx->AQCTLB = (aqctlb_init->CBD<<10)|\
      (aqctlb_init->CBU <<8)|\
      (aqctlb_init->CAD <<6)|\
      (aqctlb_init->CAU <<4)|\
      (aqctlb_init->PRD <<2)|\
      (aqctlb_init->ZRO <<0);
}

/**
  * @功能：EPWM的AQSFRC寄存器配置（软件强制控制单次生效）
  * @注意:无  
  *@参数：1.EPWMx:EPWM组
  *       2.aqsfrc_init 初始化结构体指针
  *@返回值：无
  */
void epwm_aqsfrc_config(EPWM_TypeDef* EPWMx,epwm_aqsfrc_init_t* aqsfrc_init)
{
    EPWMx->AQSFRC =(aqsfrc_init->RLDCSF<<6)|\
      (aqsfrc_init->OTSFB <<5)|\
      (aqsfrc_init->ACTSFB<<3)|\
      (aqsfrc_init->OTSFA <<2)|\
      (aqsfrc_init->ACTSFA<<0);
}

/**
  * @功能：EPWM的AQCSFRC寄存器配置（软件强制控制持续生效）
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.aqcsfrc_init 初始化结构体指针
  * @返回值：无
  */
void epwm_aqcsfrc_config(EPWM_TypeDef* EPWMx,epwm_aqcsfrc_init_t* aqcsfrc_init)
{
    EPWMx->AQCSFRC =(aqcsfrc_init->CSFB<<2)|\
      (aqcsfrc_init->CSFA <<0);
}

/**
  * @功能：EPWM的DBCTL寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.dbctl_init 初始化结构体指针
  * @返回值：无
  */
void epwm_dbctl_config(EPWM_TypeDef* EPWMx,epwm_dbctl_init_t* dbctl_init)
{
    EPWMx->DBCTL =(dbctl_init->IN_MODE <<4)|\
      (dbctl_init->POLSEL << 2)|\
      (dbctl_init->OUT_MODE <<0);
}

/**
  * @功能：EPWM的上沿死区计数值配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.dbred 死区计数值 16位
  * @返回值：无
  */
void epwm_dbred_config(EPWM_TypeDef* EPWMx,unsigned short dbred)
{
    EPWMx->DBRED = dbred & 0XFFFF;
}

/**
  * @功能：EPWM的下沿死区计数值配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.dbred 死区计数值 16位
  * @返回值：无
  */
void epwm_dbfed_config(EPWM_TypeDef* EPWMx,unsigned short dbfed)
{
    EPWMx->DBFED = dbfed & 0XFFFF;
}

/**
  * @功能：EPWM的刹车选择寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.tzsel_init 初始化结构体指针
  * @返回值：无
  */
void epwm_tzsel_config(EPWM_TypeDef* EPWMx,epwm_tzsel_init_t* tzsel_init)
{
    char i=0;
    EPWMx->TZSEL  &= ~0xFFFF;
    for(i=1;i<7;i++)
    {
        EPWMx->TZSEL |= (tzsel_init->TZx_CBC[i] << (i-1));
        EPWMx->TZSEL |= (tzsel_init->TZx_OSHT[i] << (i+7));
    }
    EPWMx->TZSEL |= (tzsel_init->CBCMOD << 6)|\
      (tzsel_init->OSHTMOD << 14);
}

/**
  * @功能：EPWM的刹车控制寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.tzctl_init 初始化结构体指针
  * @返回值：无
  */
void epwm_tzctl_config(EPWM_TypeDef* EPWMx,epwm_tzctl_init_t* tzctl_init)
{
    // char i=0;
    EPWMx->TZCTL =0x0;
    // for(i=1;i<7;i++)
    // {
    //     if(i!=4) 
    //     {
    //         EPWMx->TZCTL |=(tzctl_init->TZn_SEL[i]<<(i+3));
    //     }
    //     EPWMx->TZCTL |=(tzctl_init->TZn_EN[i]<<(i+9));
    // }
    EPWMx->TZCTL |= (tzctl_init->TZB << 2)|\
      (tzctl_init->TZA << 0);
}

/**
  * @功能：EPWM的软件触发寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.tz 刹车中断类型
  * @返回值：无
  */
void epwm_tzfrc_enable(EPWM_TypeDef* EPWMx,EPWM_TZCLRx tz)
{
    EPWMx->TZFRC |=(1<<tz);
}

/**
  * @功能：EPWM的事件选择寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.tz 刹车中断类型
  * @返回值：无
  */
int epwm_get_tzflag(EPWM_TypeDef* EPWMx,EPWM_TZCLRx tz)
{
    return   EPWMx->TZFLG & (1<<tz);
}

/**
  * @功能：EPWM的事件选择寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.etsel_init 初始化结构体指针
  * @返回值：无
  */
void epwm_etsel_config(EPWM_TypeDef* EPWMx,epwm_etsel_init_t* etsel_init)
{
    EPWMx->ETSEL =(etsel_init->SOCBEN << 15)|\
      (etsel_init->SOCBSEL <<11)|\
      (etsel_init->SOCAEN << 10)|\
      (etsel_init->SOCASEL << 6)|\
      (etsel_init->INTEN << 3  )|\
      (etsel_init->INTSEL << 0);
}

/**
  * @功能：EPWM的中断寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.cmd ENABLE 使能，DISABLE 关闭
  * @返回值：无
  */
void epwm_etsel_interrupt_enable(EPWM_TypeDef* EPWMx,FunctionalState cmd)
{
    if(cmd==ENABLE)
    {
        EPWMx->ETSEL |=(1<<3);
    }
    else
    {
        EPWMx->ETSEL &=~(1<<3);
    }
}

/**
  * @功能：EPWM的事件预分频寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.etps_init 初始化结构体指针
  * @返回值：无
  */
void epwm_etps_config(EPWM_TypeDef* EPWMx,epwm_etps_init_t* etps_init)
{
    EPWMx->ETPS = (etps_init->SOCBCNT<<14)|\
      (etps_init->SOCBPRD << 12)|\
      (etps_init->SOCACNT << 10)|\
      (etps_init->SOCAPRD <<  8)|\
      (etps_init->INTCNT  <<  2)|\
      (etps_init->INTPRD  <<  0);
}

/**
  * @功能：EPWM事件中断清除寄存器
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.etstatus 事件选择
  * @返回值：无
  */
void epwm_etclr_clear(EPWM_TypeDef* EPWMx,EPWM_ETSTATUSx etstatus)
{
    EPWMx->ETCLR |= (1<<(etstatus &0x3));
}

/**
  * @功能：EPWM所有中断清除
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  * @返回值：无   
  */
void epwm_etclr_clear_all(EPWM_TypeDef* EPWMx)
{
    EPWMx->ETCLR =0xF;
}

/**
  * @功能：EPWM事件触发寄存器配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *        2.etfrc_init 初始化结构体
  * @返回值：无
  */
void epwm_etfrc_config(EPWM_TypeDef* EPWMx,epwm_etfrc_init_t* etfrc_init)
{
    EPWMx->ETFRC = (etfrc_init->SOCA << 2)|\
      (etfrc_init->SOCB << 3);
}

/**
  * @功能：EPWM计数启动
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  * 
  * @返回值：无
  */
void epwm_start(EPWM_TypeDef* EPWMx)
{
    EPWMx->TBCTL |= (1<<9);
}

/**
  * @功能：EPWM计数停止
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  * 
  * @返回值：无
  */
void epwm_stop(EPWM_TypeDef* EPWMx)
{
    EPWMx->TBCTL &= ~(1<<9);
}

/**
  * @功能：EPWM初始化配置
  * @注意:无  
  * @参数：1.EPWMx:EPWM组
  *       2.init 初始化结构体指针
  * @返回值：无
  */
void epwm_init(EPWM_TypeDef* EPWMx,epwm_init_t* init)
{
    epwm_tbctl_config(EPWMx,&(init->TBCTL));
    epwm_tbprd_config(EPWMx,init->TBPRD);
    epwm_cmpctrl_config(EPWMx,&(init->CMPCTL));
    epwm_cmpa_config(EPWMx,init->CMPA);
    epwm_cmpb_config(EPWMx,init->CMPB);
    epwm_cpr1_config(EPWMx,init->CPR1);
    epwm_cpr2_config(EPWMx,init->CPR2);
    epwm_aqctla_config(EPWMx,&(init->AQCTLA));
    epwm_aqctlb_config(EPWMx,&(init->AQCTLB));
    epwm_aqsfrc_config(EPWMx,&(init->AQSFRC));
    epwm_aqcsfrc_config(EPWMx,&(init->AQCSFRC));
    epwm_dbctl_config(EPWMx,&(init->DBCTL));
    epwm_dbred_config(EPWMx,init->DBRED);
    epwm_dbfed_config(EPWMx,init->DBFED);
    epwm_tzctl_config(EPWMx,&(init->TZCTL));
    epwm_tzsel_config(EPWMx,&(init->TZSEL));
    epwm_etsel_config(EPWMx,&(init->ETSEL));
    epwm_etps_config(EPWMx,&(init->ETPS));
    epwm_etclr_clear(EPWMx,EPWM_ETSTATUS_INT);
    epwm_etclr_clear(EPWMx,EPWM_ETSTATUS_SOCA);
    epwm_etclr_clear(EPWMx,EPWM_ETSTATUS_SOCB);
    epwm_etfrc_config(EPWMx,&(init->ETFRC));
}

/**
  * @功能：占空比同比减小或增大多少倍
  * @参数：1.EPWMx:EPWM组
  *       2.bright 其中一路输出基础占空比
  *       3.rate 另一路相对占空比的比例
  * @返回值：无
  */
void epwm_set_duty_decrease(EPWM_TypeDef* EPWMx, uint32_t bright, float rate)
{
    uint32_t tbprd = EPWMx->TBPRD;

    uint32_t cmpa = EPWMx->CMPA; 
    uint32_t cmpb = EPWMx->CMPB;
    if(rate < 1.0f)
    {
        cmpa = tbprd - bright;
        cmpb = (uint32_t)(bright * rate);
    }
    else
    {
        cmpa = tbprd - (uint32_t)(bright / rate);
        cmpb = bright;
    }

    //设置相位（相位/周期=占空比）
    //cmpa配置
    epwm_cmpa_config(EPWMx,cmpa);
    //cmpb配置
    epwm_cmpb_config(EPWMx,cmpb);
}

/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/