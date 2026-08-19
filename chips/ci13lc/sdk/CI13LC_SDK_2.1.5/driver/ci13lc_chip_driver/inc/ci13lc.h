/**
 * @file ci13lc.h
 * @brief  芯片系列公用头文件
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024  Chipintelli Technology Co., Ltd.
 *
 */

#ifndef _CI_13LC_H_
#define _CI_13LC_H_

#include <stdint.h>
#include "user_config.h"

// IRQ number 
typedef enum IRQn
{
/******  RISC-V N307 Processor Exceptions Numbers *******************************/
    MSIP_IRQn                 = 3,      /*!<            */
    MTIP_IRQ                  = 7,      /*!<            */
/****** smt specific Interrupt Numbers ****************************************/
    SCU_IRQn                  = 19 + 1,      /*!<            */
    NPU_IRQn                  = 19 + 2,      /*!<            */
    EPWM_IRQn                 = 19 + 3,      /*!<            */
    DMA_IRQn                  = 19 + 4,      /*!<            */
    TIMER0_IRQn               = 19 + 5,      /*!<            */
    TIMER1_IRQn               = 19 + 6,      /*!<            */
    IIC0_IRQn                 = 19 + 9,      /*!<            */
    PA_IRQn                   = 19 + 10,     /*!<            */
    PB_IRQn                   = 19 + 11,     /*!<            */
    UART0_IRQn                = 19 + 12,     /*!<            */
    UART1_IRQn                = 19 + 13,     /*!<            */
    UART2_IRQn                = 19 + 14,     /*!<            */
    IIS0_IRQn                 = 19 + 15,     /*!<            */
    IIS1_IRQn                 = 19 + 16,     /*!<            */
    IIS_DMA_IRQn              = 19 + 18,     /*!<            */
    ALC_TIMEOUT_IRQn          = 19 + 19,     /*!<            */
    DTR_IRQn                  = 19 + 21,     /*!<            */
    V11_OK_IRQn               = 19 + 22,     /*!<            */
    VDT_IRQn                  = 19 + 23,     /*!<            */
    EXT0_IRQn                 = 19 + 24,     /*!<            */
    EXT1_IRQn                 = 19 + 25,     /*!<            */
    IWDG_IRQn                 = 19 + 26,     /*!<            */
    PVDC_IRQn                 = 19 + 28,     /*!<            */
    EFUSE_IRQn                = 19 + 29,     /*!<            */
    PC_IRQn                   = 19 + 30,     /*!<            */
/********************************* END ****************************************/
} IRQn_Type;

#define IRQn_MAX_NUMBER (51)


/* APB peripherals*/
#define IPCORE_BASE             (0xa6a6a6a6)
#define APB_BASE                (0x5a5a5a5a)
#define SYSTICK_BASE            (0x77777777)
#define HAL_DTRFLASH_RAM_BASE   (0x45454545) 


#define HAL_SCU_BASE        (0x40000000)
#define HAL_GDMA_BASE       (0x40001000)
#define HAL_IISDMA0_BASE    (0x40003000)
#define HAL_DTRFLASH_BASE   (0x40004000)
#define HAL_NPU_BASE        (0x40006000) //非真实的地址
#define HAL_EPWM_BASE       (0x40007000)


#define HAL_IIC0_BASE       (0x40011000)
#define HAL_CODEC_BASE      (0x40013000)
#define HAL_PWM0_BASE       (0x40014000)
#define HAL_PWM1_BASE       (0x40015000)
#define HAL_PWM2_BASE       (0x40016000)
#define HAL_PWM3_BASE       (0x40017000)
#define HAL_TIMER0_BASE     (0x40018000)
#define HAL_TIMER1_BASE     (0x40019000)

//以下2个为非真实的地址
#define CODEC_AD_GATE       (0x4001c000)
#define CODEC_DA_GATE       (0x4001d000)


#define HAL_PA_BASE         (0x40020000)
#define HAL_PB_BASE         (0x40021000)
#define HAL_UART0_BASE      (0x40022000)
#define HAL_UART1_BASE      (0x40023000)
#define HAL_UART2_BASE      (0x40024000)
#define HAL_IIS0_BASE       (0x40025000)//(IO)
#define HAL_IIS1_BASE       (0x40026000)//(CODEC)
//以下2个为非真实的地址
#define IIS1_RX_GATE        (0x40029000)
#define IIS1_TX_GATE        (0x4002A000)


#define HAL_DPMU_BASE       (0x40030000)
#define HAL_PC_BASE         (0x40031000)
#define HAL_IWDG_BASE       (0x40032000)
#define HAL_EFUSE_BASE      (0x40033000)
#define HAL_PVDC_BASE       (0x40034000)
//以下4个为非真实的地址
#define IWDG_CPU0_HALT_GATE (0x40036000)
#define IWDG_CPU1_HALT_GATE (0x40037000)
#define PLL_BASE            (0x40038000)
#define TEST_CLK_BASE       (0x40039000)
#define TEST_CLK_BASE1      (0x4003a000)
#define IWDG_RCCLK_BASE     (0x4003b000)
#define IWDG_OSCCLK_BASE    (0x4003c000)


#define SPI0FIFO_BASE       (0x60000000)
#define UART0FIFO_BASE      (0x61000000)
#define UART1FIFO_BASE      (0x62000000)
#define UART2FIFO_BASE      (0x63000000)


#define SCU                 ((SCU_TypeDef*)HAL_SCU_BASE)
#define DPMU                ((DPMU_TypeDef*)HAL_DPMU_BASE)
#define UART0               ((UART_TypeDef*)HAL_UART0_BASE)
#define UART1               ((UART_TypeDef*)HAL_UART1_BASE)
#define UART2               ((UART_TypeDef*)HAL_UART2_BASE)
#define CODEC               ((CODEC_TypeDef*)HAL_CODEC_BASE)
#define DMAC                ((DMA_TypeDef*)HAL_GDMA_BASE)
#define IISDMA0             ((IISDMA_TypeDef*)HAL_IISDMA0_BASE)
#define EPWM                ((EPWM_TypeDef*)HAL_EPWM_BASE)
#define PVDC                ((PVDC_TypeDef*)HAL_PVDC_BASE)


/*SCU寄存器定义*/
typedef struct
{
    volatile unsigned int SYS_CTRL_CFG;         /*!< offest:0x00 功能: */
    volatile uint32_t REV_SYR_CFG_0[(0x0C-0x00)/4-0x1];       
    volatile unsigned int EXT_INT_CFG;          /*!< offest:0x0C 功能: */
    volatile uint32_t REV_SYR_CFG_1[(0x50-0x0C)/4-0x1];       
    volatile unsigned int SYSCFG_LOCK_CFG;      /*!< offest:0x50 功能: */
    volatile unsigned int RSTCFG_LOCK_CFG;      /*!< offest:0x54 功能: */
    volatile unsigned int CKCFG_LOCK_CFG;       /*!< offest:0x58 功能: */
    volatile uint32_t REV_SYR_CFG_2[(0x80-0x58)/4-0x1];       
    volatile unsigned int CLK_DIV_PARAM0_CFG;   /*!< offest:0x80 功能: */
    volatile unsigned int CLK_DIV_PARAM1_CFG;   /*!< offest:0x84 功能: */
    volatile unsigned int CLK_DIV_PARAM2_CFG;   /*!< offest:0x88 功能: */
    volatile uint32_t REV_SYR_CFG_3[(0xB0-0x88)/4-0x1];       
    volatile unsigned int CLK_DIV_PARAM_EN_CFG; /*!< offest:0xB0 功能: */
    volatile uint32_t REV_SYR_CFG_4[(0xC0-0xB0)/4-0x1];       
    volatile unsigned int SRC0_MCLK_CFG;        /*!< offest:0xC0 功能: */
    volatile unsigned int SRC1_MCLK_CFG;        /*!< offest:0xC4 功能: */
    volatile uint32_t REV_SYR_CFG_5[(0xD0-0xC4)/4-0x1];       
    volatile unsigned int MCLK0_CFG;            /*!< offest:0xD0 功能: */
    volatile unsigned int MCLK1_CFG;            /*!< offest:0xD4 功能: */
    volatile uint32_t REV_SYR_CFG_6[(0xE0-0xD4)/4-0x1];       
    volatile unsigned int IIS0_CLK_SEL_CFG;     /*!< offest:0xE0 功能: */
    volatile unsigned int IIS1_CLK_SEL_CFG;     /*!< offest:0xE4 功能: */
    volatile uint32_t REV_SYR_CFG_7[(0xF0-0xE4)/4-0x1];       
    volatile unsigned int PAD_CLK_SEL_CFG;      /*!< offest:0xF0 功能: */
    volatile unsigned int CODEC_CLK_SEL_CFG;    /*!< offest:0xF4 功能: */
    volatile uint32_t REV_SYR_CFG_8[(0x11C-0xF4)/4-0x1];       
    volatile unsigned int SYS_CLKGATE_CFG0;     /*!< offest:0x11C 功能: */
    volatile unsigned int SYS_CLKGATE_CFG1;     /*!< offest:0x120 功能: */
    volatile unsigned int AHB_CLKGATE_CFG;      /*!< offest:0x124 功能: */
    volatile unsigned int APB0_CLKGATE_CFG;     /*!< offest:0x128 功能: */
    volatile unsigned int APB1_CLKGATE_CFG;     /*!< offest:0x12C 功能: */
    volatile uint32_t REV_SYR_CFG_9[(0x178-0x12C)/4-0x1];       
    volatile unsigned int SCU_STATE_REG;        /*!< offest:0x178 功能: */
    volatile uint32_t REV_SYR_CFG_10[(0x190-0x178)/4-0x1];       
    volatile unsigned int AHB_RESET_CFG;        /*!< offest:0x190 功能: */
    volatile unsigned int APB0_RESET_CFG;       /*!< offest:0x194 功能: */
    volatile unsigned int APB1_RESET_CFG;       /*!< offest:0x198 功能: */
    volatile uint32_t REV_SYR_CFG_11[(0x1DC-0x198)/4-0x1];
    volatile unsigned int WAKEUP_MASK_CFG0;     /*!< offest:0x1DC 功能: */         
    volatile unsigned int WAKEUP_MASK_CFG1;     /*!< offest:0x1E0 功能: */
    volatile unsigned int EXT0_FILTER_CFG;      /*!< offest:0x1E4 功能: */
    volatile unsigned int EXT1_FILTER_CFG;      /*!< offest:0x1E8 功能: */
    volatile uint32_t REV_SYR_CFG_12[(0x1F4-0x1E8)/4-0x1];       
    volatile unsigned int INT_STATE_REG0;       /*!< offest:0x1F4 功能: */
    volatile unsigned int INT_STATE_REG1;       /*!< offest:0x1F8 功能: */
    volatile uint32_t REV_SYR_CFG_14[(0x240-0x1F8)/4-0x1];       
    volatile unsigned int MEM0_EMA_CFG;         /*!< offest:0x240 功能: */
    volatile unsigned int MEM1_EMA_CFG;         /*!< offest:0x244 功能: */
    volatile unsigned int MEM2_EMA_CFG;         /*!< offest:0x248 功能: */
    volatile uint32_t REV_SYR_CFG_15[(0x264-0x248)/4-0x1];       
    volatile unsigned int IIS_DATA_SEL_CFG;     /*!< offest:0x264 功能: */
    volatile uint32_t REV_SYR_CFG_16[(0x290-0x264)/4-0x1];       
    volatile unsigned int PAD_STATE;            /*!< offest:0x290 功能: */
    volatile uint32_t REV_SYR_CFG_17[(0x2C0-0x290)/4-0x1];
    volatile unsigned int BOOT_ADDR_CFG;        /*!< offest:0x2C0 功能: */
    volatile uint32_t REV_SYR_CFG_18[(0x2D4-0x2C0)/4-0x1];
    volatile unsigned int DUALCORE_JTAG_MD;     /*!< offest:0x2D4 功能: */
    volatile unsigned int EFUSE_TEST_MD;        /*!< offest:0x2D8 功能: */
}SCU_TypeDef;


/**
 * @brief DPMU寄存器结构体
 */
typedef struct
{
    volatile unsigned int CFG_LOCK_CFG;         /*!< offest:0x00 功能: */
    volatile uint32_t REV_SYR_CFG_0[(0x10-0x00)/4-0x1];       
    volatile unsigned int SYS_RESET_CFG;        /*!< offest:0x10 功能: */
    volatile unsigned int SYS_SOFTRST_CFG;      /*!< offest:0x14 功能: */
    volatile uint32_t REV_SYR_CFG_1[(0x20-0x14)/4-0x1];       
    volatile unsigned int SYS_CLK_SEL_CFG;      /*!< offest:0x20 功能: */
    volatile uint32_t REV_SYR_CFG_2[(0x30-0x20)/4-0x1];       
    volatile unsigned int PLL_CFG;              /*!< offest:0x30 功能: */
    volatile unsigned int AON_CLK_PARAM_CFG;    /*!< offest:0x34 功能: */
    volatile unsigned int AON_CLK_PARAM_CFG1;   /*!< offest:0x38 功能: */
    volatile uint32_t REV_SYR_CFG_3[(0x40-0x38)/4-0x1];       
    volatile unsigned int AON_CLK_PARAM_EN_CFG; /*!< offest:0x40 功能: */
    volatile uint32_t REV_SYR_CFG_4[(0x50-0x40)/4-0x1];       
    volatile unsigned int AON_CLKGATE_CFG;      /*!< offest:0x50 功能: */
    volatile uint32_t REV_SYR_CFG_5[(0x70-0x50)/4-0x1];       
    volatile unsigned int AON_RESET_CFG;        /*!< offest:0x70 功能: */
    volatile uint32_t REV_SYR_CFG_6[(0xC0-0x70)/4-0x1];       
    volatile unsigned int PMU_CFG;              /*!< offest:0xC0 功能: */
    volatile uint32_t REV_SYR_CFG_7[(0xC8-0xC0)/4-0x1];       
    volatile unsigned int PMU_UPDATE_EN;        /*!< offest:0xC8 功能: */
    volatile uint32_t REV_SYR_CFG_8[(0xD0-0xC8)/4-0x1];       
    volatile unsigned int LOW_POWER_CFG;        /*!< offest:0xD0 功能: */
    volatile unsigned int PMU_PWROFF_CFG;       /*!< offest:0xD4 功能: */
    volatile unsigned int PMU_PWRON_CFG;        /*!< offest:0xD8 功能: */
    volatile uint32_t REV_SYR_CFG_9[(0xE0-0xD8)/4-0x1];       
    volatile unsigned int WAKEUP_RESET_CFG;     /*!< offest:0xE0 功能: */
    volatile unsigned int WAKEUP_MASK_CFG;      /*!< offest:0xE4 功能: */
    volatile unsigned int WAKEUP_EXT_FILTER_CFG;/*!< offest:0xE8 功能: */
    volatile unsigned int WAKEUP_CFG;           /*!< offest:0xEC 功能: */
    volatile uint32_t REV_SYR_CFG_10[(0x100-0xEC)/4-0x1];       
    volatile unsigned int RC_CFG;               /*!< offest:0x100 功能: */
    volatile unsigned int RC_UPDATE_CFG;        /*!< offest:0x104 功能: */
    volatile unsigned int OSC_PAD_CFG;          /*!< offest:0x108 功能: */
    volatile uint32_t REV_SYR_CFG_11[(0x140-0x108)/4-0x1];
    volatile unsigned int IOREUSE_CFG0;         /*!< offest:0x140 功能: */
    volatile unsigned int IOREUSE_CFG1;         /*!< offest:0x144 功能: */
    volatile uint32_t REV_SYR_CFG_12[(0x14c-0x144)/4-0x1];
    volatile unsigned int OD_CFG0;              /*!< offest:0x14c 功能: */
    volatile unsigned int PD_CFG0;              /*!< offest:0x150 功能: */
    volatile uint32_t REV_SYR_CFG_13[(0x158-0x150)/4-0x1];       
    volatile unsigned int PU_CFG0;              /*!< offest:0x158 功能: */
    volatile uint32_t REV_SYR_CFG_14[(0x160-0x158)/4-0x1];       
    volatile unsigned int DS_CFG0;              /*!< offest:0x160 功能: */
    volatile unsigned int DS_CFG1;              /*!< offest:0x164 功能: */
    volatile uint32_t REV_SYR_CFG_15[(0x170-0x164)/4-0x1];       
    volatile unsigned int SL_CFG0;              /*!< offest:0x170 功能: */
    volatile uint32_t REV_SYR_CFG_16[(0x178-0x170)/4-0x1];       
    volatile unsigned int ST_CFG0;              /*!< offest:0x178 功能: */
    volatile uint32_t REV_SYR_CFG_17[(0x180-0x178)/4-0x1];       
    volatile unsigned int IE_CFG0;              /*!< offest:0x180 功能: */
    volatile uint32_t REV_SYR_CFG_18[(0x190-0x180)/4-0x1];       
    volatile unsigned int AD_CFG0;              /*!< offest:0x190 功能: */
    volatile uint32_t REV_SYR_CFG_19[(0x1C0-0x190)/4-0x1];       
    volatile unsigned int RST_STATE_REG;        /*!< offest:0x1C0 功能: */
    volatile uint32_t REV_SYR_CFG_20[(0x1D0-0x1C0)/4-0x1];       
    volatile unsigned int PWR_WAKEUP_STATE_REG; /*!< offest:0x1D0 功能: */
    volatile uint32_t REV_SYR_CFG_21[(0x1E0-0x1D0)/4-0x1];
    volatile unsigned int CHIP_STATE_REG_ADDR;  /*!< offest:0x1E0 功能: */
    volatile unsigned int CHIP_INT_MASK_CFG_ADDR; /*!< offest:0x1E4 功能: */
    volatile unsigned int PAD_FILTER_CFG_ADDR;  /*!< offest:0x1E8 功能: */
    volatile unsigned int V2I_CFG;              /*!< offest:0x1EC 功能: */
}DPMU_TypeDef;


/*UART寄存器定义*/
typedef struct
{
    volatile unsigned int UARTRdDR;         //0x0
    volatile unsigned int UARTWrDR;         //0x04
    volatile unsigned int UARTRxErrStat;    //0x08
    volatile unsigned int UARTFlag;         //0x0c
    volatile unsigned int UARTIBrd;         //0x10
    volatile unsigned int UARTFBrd;         //0x14
    volatile unsigned int UARTLCR;          //0x18
    volatile unsigned int UARTCR;           //0x1c
    volatile unsigned int UARTFIFOlevel;    //0x20
    volatile unsigned int UARTMaskInt;      //0x24
    volatile unsigned int UARTRIS;          //0x28
    volatile unsigned int UARTMIS;          //0x2c
    volatile unsigned int UARTICR;          //0x30
    volatile unsigned int UARTDMACR;        //0x34
    volatile unsigned int UARTTimeOut;      //0x38
    volatile unsigned int REMCR;            //0x3c
    volatile unsigned int REMTXDATA;        //0x40
    volatile unsigned int REMRXDATA;        //0x44
    volatile unsigned int REMINTCLR;        //0x48
    volatile unsigned int REMINTSTAE;       //0x4c
    volatile unsigned int BYTE_HW_MODE;     //0x50
    volatile unsigned int BAUD_MASK_INT;    //0x54
    volatile unsigned int BAUD_RIS;         //0x58
    volatile unsigned int BAUD_MIS;         //0x5C
    volatile unsigned int BAUD_ICR;         //0x60
    volatile unsigned int BAUD_CR;          //0x64
    volatile unsigned int BAUD_SMPL_RATE;   //0x68
    volatile unsigned int BAUD_STATUS;      //0x6C
    volatile unsigned int BAUD_SMPL_0;      //0x70
}UART_TypeDef;


typedef struct
{
	volatile unsigned int DMACCxSrcAddr;
	volatile unsigned int DMACCxDestAddr;
	volatile unsigned int DMACCxLLI;
	volatile unsigned int DMACCxControl;
	volatile unsigned int DMACCxConfiguration;
	unsigned int reserved[3];
}DMACChanx_TypeDef;


typedef struct
{
	volatile unsigned int DMACIntStatus;
	volatile unsigned int DMACIntTCStatus;
	volatile unsigned int DMACIntTCClear;
	volatile unsigned int DMACIntErrorStatus;
	volatile unsigned int DMACIntErrClr;
	volatile unsigned int DMACRawIntTCStatus;
	volatile unsigned int DMACRawIntErrorStatus;
	volatile unsigned int DMACEnbldChns;
	volatile unsigned int DMACSoftBReq;
	volatile unsigned int DMACSoftSReq;
	volatile unsigned int DMACSoftLBReq;
	volatile unsigned int DMACSoftLSReq;
	volatile unsigned int DMACConfiguration;
	volatile unsigned int DMACSync;
	unsigned int reserved1[50];
	DMACChanx_TypeDef DMACChannel[8];
	unsigned int reserved2[195];
	volatile unsigned int DMACITCR;
	volatile unsigned int DMACITOP[3];
	unsigned int reserved3[693];
	volatile unsigned int DMACPeriphID[4];
	volatile unsigned int DMACPCellID[4];
}DMA_TypeDef;


typedef struct
{
	unsigned int SrcAddr;
	unsigned int DestAddr;
	unsigned int NextLLI;
	unsigned int Control;
}DMAC_LLI;


typedef struct
{
    volatile unsigned int IISxDMARADDR;         //4     //1C    //34
    volatile unsigned int IISxDMARNUM;          //8     //20    //38
    volatile unsigned int IISxDMATADDR0;        //C     //24    //3c
    volatile unsigned int IISxDMATNUM0;         //10    //28    //40
    volatile unsigned int IISxDMATADDR1;        //14    //2C    //44
    volatile unsigned int IISxDMATNUM1;         //18    //30    //48
}IISDMAChanx_TypeDef;


typedef struct
{
    volatile unsigned int IISDMACTRL;           //0
    IISDMAChanx_TypeDef   IISxDMA[3];
    volatile unsigned int IISDMAPTR;            //4c
    volatile unsigned int IISDMASTATE;          //50
    volatile unsigned int IISDMACLR;            //54
    volatile unsigned int IISDMAIISCLR;         //58
    volatile unsigned int IISDMARADDR[3];       //5C,60,64
    volatile unsigned int RX_VAD_CTRL;          //68
	volatile unsigned int RX_LAST_ADDR;         //6C
	volatile unsigned int IIS_END_NUM_EN;       //70
	volatile unsigned int IIS_END_NUM;          //74
	volatile unsigned int DMA_REQ_CLR_STATE;    //78
	volatile unsigned int DMATADDR[3];          //7C,80,84
}IISDMA_TypeDef;


/*CODEC寄存器*/
//以2代的CODEC为基础，修改一些

typedef struct 
{
    volatile unsigned int reg40;
    volatile unsigned int reg41;
    volatile unsigned int reg42;
    volatile unsigned int reg43;
    volatile unsigned int reg44;
    volatile unsigned int reg45;
    volatile unsigned int reg46;
    volatile unsigned int reg47;
    volatile unsigned int reg48;
    volatile unsigned int reg49;
    unsigned int resver7[2];
    volatile unsigned int reg4c;
    unsigned int resver8[3];
}CODEC_ALC_TypeDef;

typedef struct 
{
    volatile unsigned int reg0;
    unsigned int resver1;
    volatile unsigned int reg2;
    volatile unsigned int reg3;
    volatile unsigned int reg4;
    volatile unsigned int reg5;
    volatile unsigned int reg6;
    volatile unsigned int reg7;
    volatile unsigned int adc_dig_gain_reg[2];
    volatile unsigned int rega;
    unsigned int resver4[22];
    volatile unsigned int reg21;
    volatile unsigned int reg22;
    volatile unsigned int reg23;
    volatile unsigned int reg24;
    volatile unsigned int reg25;
    volatile unsigned int reg26;
    volatile unsigned int pga_gain_reg[2];
    volatile unsigned int reg29;
    volatile unsigned int reg2a;
    volatile unsigned int reg2b;
    volatile unsigned int reg2c;
    volatile unsigned int reg2d;
    volatile unsigned int reg2e;
    volatile unsigned int reg2f;
    volatile unsigned int reg30;
    volatile unsigned int reg31;
    volatile unsigned int reg32;
    volatile unsigned int reg33;
    unsigned int resver6[12];

    CODEC_ALC_TypeDef alc_reg[2];
}CODEC_TypeDef;
/*CODEC寄存器end*/

/*EPWM寄存器定义*/
typedef struct{
	volatile unsigned int TBCTL;//0x00
  	volatile unsigned int TBSTS;
  	volatile unsigned int TBPHS;
  	volatile unsigned int TBCTR;
  	volatile unsigned int TBPRD;//0x10
  	volatile unsigned int CMPCTL;
  	volatile unsigned int CMPA;
  	volatile unsigned int CMPB;
  	volatile unsigned int CPR1;//0x20
  	volatile unsigned int CPR2;
  	unsigned int resver1[2];
  	volatile unsigned int AQCTLA;//0x30
  	volatile unsigned int AQCTLB;
  	volatile unsigned int AQSFRC;
  	volatile unsigned int AQCSFRC;
  	volatile unsigned int DBCTL;//0x40
  	volatile unsigned int DBRED;//up dead time
  	volatile unsigned int DBFED;//down dead time
  	unsigned int resver2;
  	volatile unsigned int PCCTL;//0x50
  	volatile unsigned int PCDUTY;
  	unsigned int resver3[2];
  	volatile unsigned int TZSEL;//0x60
  	volatile unsigned int TZCTL;
  	volatile unsigned int TZEINT;
  	volatile unsigned int TZFLG;
  	volatile unsigned int TZCLR;//0x70
  	volatile unsigned int TZFRC;
  	unsigned int resver4[2];
  	volatile unsigned int ETSEL;//0x80
  	volatile unsigned int ETPS;
  	volatile unsigned int ETFLG;
  	volatile unsigned int ETCLR;
  	volatile unsigned int ETFRC;//0x90
}EPWM_TypeDef;


/*PVDC寄存器定义*/
typedef struct{
	volatile unsigned int PVDC_LOCK;//0x00
    volatile unsigned int PVDC_INTR_RAW;
    volatile unsigned int PVDC_INTR_MASK;
    volatile unsigned int PVDC_INTR;
    volatile unsigned int PVDC_VOL; //0x10
    volatile unsigned int PVDC_EN;
    volatile unsigned int PVDC_TH;
    volatile unsigned int PVDC_CFG_VOL;
    volatile unsigned int PVDC_TIME;//0x20
    volatile unsigned int PVDC_MAX_VAL;
}PVDC_TypeDef;


typedef enum
{
    EXT0 = 0,
    EXT1 = 1,
}Ext_Num;



#endif


