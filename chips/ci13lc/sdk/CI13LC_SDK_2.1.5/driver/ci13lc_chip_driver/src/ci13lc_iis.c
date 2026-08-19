/**
 * @file ci13lc_iis.c
 * @brief 二代芯片IIS底层驱动接口
 * @version 0.1
 * @date 2021-07-05
 * 
 * @copyright Copyright (c) 2021  Chipintelli Technology Co., Ltd.
 * 
 */

#include "ci_iis.h"
#include "ci_assert.h"
#include "FreeRTOS.h"

#define IIS_CHK_TX_NUM (5)
#define IIS_CHK_RX_NUM (5)
#define IIS_CHK_DMA_NUM (5)

#define IIS_EN          (0x1 << 0)
#define IIS_RX_LOAD_EN  (0x1 << 0)
#define IIS_TX_LOAD_EN  (0x1 << 10)
#define IIS_RX_CH_MERGE (0x1 << 30)

typedef struct iis_register
{
    volatile uint32_t iis_int0;
    volatile uint32_t reserved_0[3];   
    volatile uint32_t iis_tx_ctrl[2];      /*!< 属性:RW 偏移:0x10、0x14 位宽:32 功能:发送通道控制寄存器 */
    volatile uint32_t reserved_2[2];   
    volatile uint32_t iis_rx_ctrl[3];      /*!< 属性:RW 偏移:0x20、0x24、0x28 位宽:32 功能:接收通道0、1、2控制寄存器 */
    volatile uint32_t reserved_3;      
    volatile uint32_t iis_load_ctrl;       /*!< 属性:RW 偏移:0x30 位宽:32 功能:装载使能控制寄存器 */
    volatile uint32_t iis_chken_dma_rx[3]; /*!< 属性:RW 偏移:0x34、0x38、0x3C 位宽:32 功能:RX0、RX1、RX2 DMA请求检测寄存器 */
    volatile uint32_t iis_chken_rx[3];     /*!< 属性:RW 偏移:0x40、0x44、0x48 位宽:32 功能:RX0、RX1、RX2 IIS时钟检测寄存器 */
    volatile uint32_t reserved_4; 
    volatile uint32_t iis_chken_tx[2];     /*!< 属性:RW 偏移:0x50、0x54 位宽:32 功能:TX0、TX1 IIS时钟检测寄存器 */
}iis_register_t,*iis_register_p;


static void iis_wait_load_ctrl(uint32_t iis_base)
{
    iis_register_p iis = (iis_register_p)iis_base;
    volatile uint32_t iis_timeout = 1000000;
    //_delay_10us(100);
    while(iis->iis_load_ctrl)
    {
        iis_timeout--;
        if(iis_timeout == 0)
        {
            // CI_ASSERT(0,"iis_load_ctrl timeout!\n");
            iis_timeout = 1000000;
            mprintf("iis_load_ctrl timeout!\n");
        }
    }
}

/**
 * @brief IIS发送使能
 *
 * @param iis_base IIS控制器基地址
 * @param cha IIS发送通道
 * @param cmd 是否使能
 */
void iis_tx_enable(uint32_t iis_base,iis_tx_channal_t cha,FunctionalState cmd)
{
    iis_register_p iis = (iis_register_p)iis_base;
    iis_wait_load_ctrl(iis_base);
    if(cmd == ENABLE)
    {
        iis->iis_tx_ctrl[cha] |= IIS_EN;
    }
    else
    {
        iis->iis_tx_ctrl[cha] &= ~IIS_EN;
    }
    iis->iis_load_ctrl |= IIS_TX_LOAD_EN;
    iis_wait_load_ctrl(iis_base);
}

/**
 * @brief IIS接收使能
 *
 * @param iis_base IIS控制器基地址
 * @param cha IIS接收通道
 * @param cmd 是否使能
 */
void iis_rx_enable(uint32_t iis_base,iis_rx_channal_t cha,FunctionalState cmd)
{
    iis_register_p iis = (iis_register_p)iis_base;
    iis_wait_load_ctrl(iis_base);
    if(cmd == ENABLE)
    {
        iis->iis_rx_ctrl[cha] |= IIS_EN;
    }
    else
    {
        iis->iis_rx_ctrl[cha] &= ~IIS_EN;
    }
    iis->iis_load_ctrl |= IIS_RX_LOAD_EN;
    iis_wait_load_ctrl(iis_base);
}

/**
 * @brief IIS发送模式，左声道静音
 *
 * @param iis_base IIS控制器基地址
 * @param cha IIS发送通道
 * @param cmd 是否静音
 */
void iis_tx_l_mute(uint32_t iis_base,iis_tx_channal_t cha,FunctionalState cmd)
{
    iis_register_p iis = (iis_register_p)iis_base;
    if(cmd == ENABLE)
    {
        iis->iis_tx_ctrl[cha] |= (0x1 << 7);
    }
    else
    {
        iis->iis_tx_ctrl[cha] &= ~(0x1 << 7);
    }
}

/**
 * @brief IIS发送模式，右声道静音
 *
 * @param iis_base IIS控制器基地址
 * @param cha IIS发送通道
 * @param cmd 是否静音
 */
void iis_tx_r_mute(uint32_t iis_base,iis_tx_channal_t cha,FunctionalState cmd)
{
    iis_register_p iis = (iis_register_p)iis_base;
    if(cmd == ENABLE)
    {
        iis->iis_tx_ctrl[cha] |= (0x1 << 8);
    }
    else
    {
        iis->iis_tx_ctrl[cha] &= ~(0x1 << 8);
    }
}

/**
 * @brief IIS发送模式，时钟检测
 *
 * @param iis_base IIS控制器基地址
 * @param cha IIS发送通道
 * @param cmd 是否使能时钟检测
 */
void iis_tx_chk(uint32_t iis_base,iis_tx_channal_t cha,FunctionalState cmd)
{
    iis_register_p iis = (iis_register_p)iis_base;
    iis->iis_chken_tx[cha] &= ~(0xFFFFF << 1);
    iis->iis_chken_tx[cha] |= (IIS_CHK_TX_NUM << 1);
    if(cmd == ENABLE)
    {
        iis->iis_chken_tx[cha] |= IIS_EN;
    }
    else
    {
        iis->iis_chken_tx[cha] &= ~IIS_EN;
    }
}

/**
 * @brief IIS发送模式，配置
 *
 * @param iis_base IIS控制器基地址
 * @param tx_cfg IIS发送配置信息
 */
void iis_tx_config(uint32_t iis_base,iis_tx_config_p tx_cfg)
{
    iis_register_p iis = (iis_register_p)iis_base;
    volatile uint32_t value = 0;
    uint8_t mono_flag = 0;
    uint8_t merge_flag = 0;
    if(IIS_SC_MONO == tx_cfg->tx_sc)
    {
        mono_flag = 1;
    }
    if(IIS_DW_16BIT == tx_cfg->txch_dw)
    {
        merge_flag = 1;
    }
    iis_wait_load_ctrl(iis_base);

    iis->iis_tx_ctrl[tx_cfg->cha] &= ~IIS_EN;
    iis->iis_load_ctrl |= IIS_TX_LOAD_EN;
    iis_wait_load_ctrl(iis_base);
    //////////////////////////////////////
    value = (tx_cfg->txfifo_trig << 3) | (tx_cfg->txch_dw << 4);
    if(mono_flag)
    {
        value |= (tx_cfg->txch_copy << 9);
    }
    value |= (tx_cfg->sck_lrck << 10) | (tx_cfg->tx_df << 11) | (tx_cfg->tx_sc << 13);
    if(merge_flag)
    {
        if(mono_flag)
        {
            //单声道两个 16bit merge
            value |= (tx_cfg->tx_merge << 15);
        }
        else
        {
            //双声道两个 16bit merge
            value |= (tx_cfg->tx_merge << 14);
        }
    }
    value |= (tx_cfg->tx_swap << 16);
    iis->iis_tx_ctrl[tx_cfg->cha] = value;
    //////////////////////////////////////
    //iis->iis_tx_ctrl[tx_cfg->cha] |= IIS_EN;
    iis->iis_load_ctrl |= IIS_TX_LOAD_EN;
 
    iis_wait_load_ctrl(iis_base);
}

/**
 * @brief IIS发送模式，立体环绕音使能（tx channel1不能工作，所以该接口不支持）
 *
 * @param iis_base IIS控制器基地址
 * @param cmd 是否使能立体环绕音
 */
void iis_tx_same(uint32_t iis_base,FunctionalState cmd)
{
    iis_register_p iis = (iis_register_p)iis_base;
    iis_wait_load_ctrl(iis_base);
    iis->iis_tx_ctrl[IIS_TX_CHANNAL_TX0] &= ~IIS_EN;
    iis->iis_tx_ctrl[IIS_TX_CHANNAL_TX1] &= ~IIS_EN;
    iis->iis_load_ctrl |= IIS_TX_LOAD_EN;
    iis_wait_load_ctrl(iis_base);
    if(cmd == ENABLE)
    {
        if(iis->iis_tx_ctrl[IIS_TX_CHANNAL_TX0] != iis->iis_tx_ctrl[IIS_TX_CHANNAL_TX1])
        {
            mprintf("IIS TX0与TX1配置不相同，无法配置为环绕立体音效\n");
            return;
        }
        iis->iis_tx_ctrl[IIS_TX_CHANNAL_TX1] |= (0x1 << 17);
    }
    else
    {
        iis->iis_tx_ctrl[IIS_TX_CHANNAL_TX1] &= ~(0x1 << 17);
    }
    iis->iis_tx_ctrl[IIS_TX_CHANNAL_TX0] |= IIS_EN;
    iis->iis_tx_ctrl[IIS_TX_CHANNAL_TX1] |= IIS_EN;
    iis->iis_load_ctrl |= IIS_TX_LOAD_EN;
    iis_wait_load_ctrl(iis_base);
}

/**
 * @brief IIS接收模式，静音
 *
 * @param iis_base IIS控制器基地址
 * @param cha IIS接收通道
 * @param cmd 是否使能静音
 */
void iis_rx_mute(uint32_t iis_base,iis_rx_channal_t cha,FunctionalState cmd)
{
    iis_register_p iis = (iis_register_p)iis_base;
    if(cmd == ENABLE)
    {
        iis->iis_rx_ctrl[cha] |= (0x1 << 26);
    }
    else
    {
        iis->iis_rx_ctrl[cha] &= ~(0x1 << 26);
    }
}

/**
 * @brief IIS接收模式，时钟检测
 *
 * @param iis_base IIS控制器基地址
 * @param cha IIS接收通道
 * @param cmd 是否使能时钟检测
 */
void iis_rx_chk(uint32_t iis_base,iis_rx_channal_t cha,FunctionalState cmd)
{
    iis_register_p iis = (iis_register_p)iis_base;
    iis->iis_chken_rx[cha] &= ~(0xFFFFF << 1);
    iis->iis_chken_rx[cha] |= (IIS_CHK_RX_NUM << 1);
    if(cmd == ENABLE)
    {
        iis->iis_chken_rx[cha] |= IIS_EN;
    }
    else
    {
        iis->iis_chken_rx[cha] &= ~IIS_EN;
    }
}

/**
 * @brief IIS接收模式，发送DMA请求检测
 *
 * @param iis_base IIS控制器基地址
 * @param cha IIS发送通道
 * @param cmd 是否检测DAM请求
 */
void iis_rx_dma_chk(uint32_t iis_base,iis_rx_channal_t cha,FunctionalState cmd)
{
    iis_register_p iis = (iis_register_p)iis_base;
    iis->iis_chken_dma_rx[cha] &= ~(0xFFFF << 0);
    iis->iis_chken_dma_rx[cha] |= (IIS_CHK_DMA_NUM << 0);
    if(cmd == ENABLE)
    {
        iis->iis_chken_dma_rx[cha] |= (0x1 << 16);
    }
    else
    {
        iis->iis_chken_dma_rx[cha] &= ~(0x1 << 16);
    }
}

/**
 * @brief IIS接收配置
 *
 * @param iis_base IIS控制器基地址
 * @param tx_cfg IIS接收配置信息
 */
void iis_rx_config(uint32_t iis_base,iis_rx_config_p rx_cfg)
{
    iis_register_p iis = (iis_register_p)iis_base;
    volatile uint32_t value = 0;
    uint8_t mono_flag = 0;
    uint8_t merge_flag = 0;
    if(IIS_SC_MONO == rx_cfg->rx_sc)
    {
        mono_flag = 1;
    }
    if(IIS_DW_16BIT == rx_cfg->rxch_dw)
    {
        merge_flag = 1;
    }
    iis_wait_load_ctrl(iis_base);

    iis->iis_rx_ctrl[rx_cfg->cha] &= ~IIS_EN;
    iis->iis_load_ctrl |= IIS_RX_LOAD_EN;
    iis_wait_load_ctrl(iis_base);
    //////////////////////////////////////
    value = (rx_cfg->rxfifo_trig << 4) | (rx_cfg->rxch_dw << 6) | (rx_cfg->rx_sc << 16) |
            (rx_cfg->sck_lrck << 27) | (rx_cfg->rx_df << 28);
    if(merge_flag)
    {
        if(mono_flag)
        {
            //单声道两个 16bit merge
            value |= (rx_cfg->rx_merge << 25);
        }
        else
        {
            //双声道两个 16bit merge
            value |= (rx_cfg->rx_merge << 24);
        }
    }
    value |= (rx_cfg->rx_swap << 17);
    iis->iis_rx_ctrl[rx_cfg->cha] = value;
    //////////////////////////////////////
    //iis->iis_rx_ctrl[rx_cfg->cha] |= IIS_EN;
    iis->iis_load_ctrl |= IIS_RX_LOAD_EN;
    
    iis_wait_load_ctrl(iis_base);
}

/**
 * @brief IIS接收模式，接收通道merge（rx channel1、channel2不能工作，所以该接口不支持）
 *
 * @param iis_base IIS控制器基地址
 * @param enable_rx0 发送通道0是否merge
 * @param enable_rx1 发送通道1是否merge
 * @param enable_rx2 发送通道2是否merge
 */
void iis_rx_cha_merge(uint32_t iis_base,uint8_t enable_rx0,uint8_t enable_rx1,uint8_t enable_rx2)
{
    iis_register_p iis = (iis_register_p)iis_base;
    uint32_t config_temp = 0;
    if(enable_rx0)
    {
        config_temp = iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX0];
    }
    if(enable_rx1)
    {
        if(config_temp)
        {
            if(config_temp != iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX1])
            {
                return;
            }
        }
        else
        {
            config_temp = iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX1];
        }
    }
    if(enable_rx2)
    {
        if(config_temp)
        {
            if(config_temp != iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX2])
            {
                return;
            }
        }
        else
        {
            return;
        }
    }
    iis_wait_load_ctrl(iis_base);
    iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX0] &= ~IIS_EN;
    iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX1] &= ~IIS_EN;
    iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX2] &= ~IIS_EN;
    iis->iis_load_ctrl |= IIS_TX_LOAD_EN;
    iis_wait_load_ctrl(iis_base);
    if(enable_rx0)
    {
        iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX0] |= IIS_RX_CH_MERGE;
    }
    else
    {
        iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX0] &= ~IIS_RX_CH_MERGE;
    }
    if(enable_rx1)
    {
        iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX1] |= IIS_RX_CH_MERGE;
    }
    else
    {
        iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX1] &= ~IIS_RX_CH_MERGE;
    }
    if(enable_rx2)
    {
        iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX2] |= IIS_RX_CH_MERGE;
    }
    else
    {
        iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX2] &= ~IIS_RX_CH_MERGE;
    }
    iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX0] |= IIS_EN;
    iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX1] |= IIS_EN;
    iis->iis_rx_ctrl[IIS_RX_CHANNAL_RX2] |= IIS_EN;
    iis->iis_load_ctrl |= IIS_TX_LOAD_EN;
    iis_wait_load_ctrl(iis_base);
}

/**
 * @brief IIS中断处理
 *
 * @param iis_base IIS控制器基地址
 */
void iis_int_handler(uint32_t iis_base)
{
    iis_register_p iis = (iis_register_p)iis_base;

#if 0
    for(int i = 0; i < 32 ; i++)
    {
        if(iis->iis_int0 & (0x1 << i))
        {
            iis->iis_int0 |= (0x1 << i);
        }
    }    
#else
    volatile uint32_t int_val = iis->iis_int0;
    iis->iis_int0 = int_val;
#endif
}

/**
 * @brief IISDMA0中断处理函数
 * 
 */
void IIS_DMA_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    volatile unsigned int tmp;
    tmp = IISDMA0->IISDMASTATE;

    extern void cm_input_interrupt_handler(IISDMAChax dma_channel, BaseType_t * xHigherPriorityTaskWoken);
    extern void cm_output_interrupt_handler(IISDMAChax dma_channel, BaseType_t * xHigherPriorityTaskWoken);
    //mprintf("tmp:0x%08x", tmp);

    if (tmp & (1 << (1 + IISDMACha0*2 + IISxDMA_RX_EN)))//iis0 rx end
    {
        IISDMA_INT_Clear(IISDMA0,(1 << (1 + IISDMACha0*2 + IISxDMA_RX_EN)));
        cm_input_interrupt_handler(IISDMACha0, &xHigherPriorityTaskWoken);
    }

    if (tmp & (1 << (1 + IISDMACha0*2 + IISxDMA_TX_EN)))//iis0 tx end
    {
        IISDMA_INT_Clear(IISDMA0,(1 << (1 + IISDMACha0*2 + IISxDMA_TX_EN)));
    }

    if (tmp & (1 << (1 + IISDMACha1*2 + IISxDMA_RX_EN)))//iis1 rx end
    {
        IISDMA_INT_Clear(IISDMA0,(1 << (1 + IISDMACha1*2 + IISxDMA_RX_EN)));
        cm_input_interrupt_handler(IISDMACha1, &xHigherPriorityTaskWoken);
    }

    if (tmp & (1 << (1 + IISDMACha1*2 + IISxDMA_TX_EN)))//iis1 tx end
    {
        IISDMA_INT_Clear(IISDMA0,(1 << (1 + IISDMACha1*2 + IISxDMA_TX_EN)));
    }


	if (tmp & (0x1 << 7)) //iis0 rx BusMatrix完成一次地址轮循
	{
		IISDMA_INT_Clear(IISDMA0,(0x1 << 7));
	}
	if (tmp & (0x1 << 8)) //iis1 rx BusMatrix完成一次地址轮循
	{
		IISDMA_INT_Clear(IISDMA0,(0x1 << 8));
	}
	if (tmp & (0x1 << 10)) //iis0 tx BusMatrix完成一次地址轮循
	{
        cm_output_interrupt_handler(IISDMACha0, &xHigherPriorityTaskWoken);
		IISDMA_INT_Clear(IISDMA0,(0x1 << 10));
	}
	if (tmp & (0x1 << 11)) //iis1 tx BusMatrix完成一次地址轮循
	{
        cm_output_interrupt_handler(IISDMACha1, &xHigherPriorityTaskWoken);
		IISDMA_INT_Clear(IISDMA0,(1 << 11));
	}

    {
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}

void IISx_RXDMA_Init(IIS_DMA_RXInit_Typedef* IISDMA_Str)
{
    IISDMAChax iisdmacha = IISDMA_Str->iisdmacha;  
    
    IISDMA_ChannelENConfig(IISDMA0,iisdmacha,IISxDMA_RX_EN,DISABLE);
    IISDMA_ADDRRollBackINT(IISDMA0,iisdmacha,IISxDMA_RX_EN,DISABLE);
    IISDMA_ChannelIntENConfig(IISDMA0,iisdmacha,IISxDMA_RX_EN,DISABLE);

    /*接受地址位rxaddr*/
    IISxDMA_RADDR(IISDMA0,iisdmacha,IISDMA_Str->rxaddr);
    IISxDMA_RNUM(IISDMA0,iisdmacha,IISDMA_Str->rxinterruptsize,IISDMA_Str->rollbackaddrsize,IISDMA_Str->rxsinglesize);

    /*打开IIS的DMA通道*/
    IISDMA_EN(IISDMA0,ENABLE);
}

void IISx_TXDMA_Init(IIS_DMA_TXInit_Typedef* IISDMA_Str)
{
    IISxDMA_TNUM0(IISDMA0,IISDMA_Str->iisdmacha,IISDMA_Str->rollbackaddr0size,IISDMA_Str->tx0singlesize);
    IISxDMA_TNUM1(IISDMA0,IISDMA_Str->iisdmacha,IISDMA_Str->rollbackaddr1size,IISDMA_Str->tx1singlesize);
}

/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/ 

