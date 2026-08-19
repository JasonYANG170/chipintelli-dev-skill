/**
 * @file exe_hal_ble.c
 * @brief HS6220's RF HAL layer in EXE stack.
 * @date Fri, Feb 28, 2020  6:03:10 PM
 * @author luwei
 *
 * @addtogroup HAL
 * @ingroup EXE
 * @details
 *
 * @{
 */

/*********************************************************************
 * INCLUDES
 */
#if NIMBLE_CFG_CONTROLLER
#include <stdint.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "exe_hal_ble.h"

#if defined(__xtensa__)
#include "hal_mcu_esp8266.h"
#define HW_SPI_TRANSFER
#endif
#if defined(STM32F10X_MD)
#include "hal_stm32f103.h"
#elif defined(PANGU_M0)
#include "hal_pangu.h"
#endif
#include <stdio.h>

#define RAM_CODE_SECTION
#define ostaskENTER_CRITICAL()
#define ostaskEXIT_CRITICAL()

#else
#include "ble_ll.h"
#include "exe_api.h"
#include "exe_hal.h"
#include "exe_pkt.h"
#include "exe_hal_ble.h"
#include "ci_ble_rf.h"
#include "ci130x_timer.h"
#include "ci130x_core_eclic.h"
#if !defined(NDEBUG)
#include <stdio.h>
#endif
#endif

#include "ci_log.h"
/*********************************************************************
 * MACROS
 */
#define CE_USE_COMMAND 1
uint8_t rf_setup = 0x58;
uint8_t ble_pwr = 7;
uint8_t ble_xtal = 7;
/*********************************************************************
 * CONSTANTS
 */

#if !NIMBLE_CFG_CONTROLLER
/*********************************************************************
 * LOCAL VARIABLES
 */
static ble_adv_header_t *g_rx_header;
static volatile uint16_t g_rx_status = 0; // hal_rf_is_rx_hdr() 锟叫讹拷取一锟斤拷锟斤拷频状态锟斤拷hal_rf_disable_rx()锟斤拷锟斤拷0
#if !defined(NDEBUG)
void hal_dump_rf_register(void);
#endif

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern __ALIGNED(4) uint8_t ll_gbuf_xpkt_rx[];

#endif // #if !NIMBLE_CFG_CONTROLLER

/**
 * @brief write one byte command to hs6220.
 *
 * @param [in] none
 * @return none
 */
RAM_CODE_SECTION void rf_operation(unsigned char opt)
{
#if defined(HW_SPI_TRANSFER)
    hal_spim_transfer(opt, NULL, 0, NULL, 0);
#else
    hal_csn_low();
    hal_rf_spi_wrd(opt);
    hal_csn_high();
#endif
}

/**
 * @brief write one byte data to hs6220 register.
 *
 * @param [in] addr - register address;
 *        [in] buf - the data will be write ;
 * @return none
 */
RAM_CODE_SECTION void rf_write_byte(unsigned char addr, unsigned char D)
{
#if defined(HW_SPI_TRANSFER)
    hal_spim_transfer(RF_W_REGISTER | addr, &D, 1, NULL, 0);
#else
    hal_csn_low();
    hal_rf_spi_wrd(RF_W_REGISTER | addr);
    hal_rf_spi_wrd(D);
    hal_csn_high();
#endif
}

/*write Commad  function, cmd = code; D = data*/
static void rf_wr_cmd(unsigned char cmd, unsigned char D)
{
#if defined(HW_SPI_TRANSFER)
    hal_spim_transfer(cmd, &D, 1, NULL, 0);
#else
    hal_csn_low();
    hal_rf_spi_wrd(cmd);
    hal_rf_spi_wrd(D);
    hal_csn_high();
#endif
}

/**
 * @brief read hs6220 one byte register.
 *
 * @param [in] addr - register address;
 *
 * @return the register vale
 */
unsigned char rf_read_byte(unsigned char addr)
{
    unsigned char rxdata;

#if defined(HW_SPI_TRANSFER)
    hal_spim_transfer(RF_R_REGISTER | addr, NULL, 0, &rxdata, 1);
#else
    hal_csn_low();
    hal_rf_spi_wrd(RF_R_REGISTER | addr);
    rxdata = hal_rf_spi_wrd(0);
    hal_csn_high();
#endif

    return (rxdata);
}

/**
 * @brief write muti byte data to hs6220.
 *
 * @param [in] addr - register address or command(eg. RF_W_TX_PAYLOAD);
 *        [in] buf - the data point which will be write;
 *        [in] len - the data length;
 * @return none
 */
RAM_CODE_SECTION void rf_wr_buffer(unsigned char addr, const unsigned char *buf, unsigned char len)
{
    if ((len <= 0) || (len > 32))
        return;
#if defined(HW_SPI_TRANSFER)
    hal_spim_transfer(RF_W_REGISTER | addr, buf, len, NULL, 0);
#else
    hal_csn_low();
    hal_rf_spi_wrd(RF_W_REGISTER | addr);
    while (len--)
    {
        hal_rf_spi_wrd(*buf++);
    }
    hal_csn_high();
#endif
}

/**
 * @brief read muti byte data from hs6220.
 *
 * @param [in] addr - register address or command(eg. RF_R_RX_PAYLOAD);
 *        [in] buf - the data point ;
 *        [in] len - the data length will be read;
 * @return none
 */
RAM_CODE_SECTION void rf_read_buffer(unsigned char addr, unsigned char *buf, unsigned char len)
{
    if (0 == len)
        return;
#if defined(HW_SPI_TRANSFER)
    hal_spim_transfer(RF_R_REGISTER | addr, NULL, 0, buf, len);
#else
    hal_csn_low();
    hal_rf_spi_wrd(RF_R_REGISTER | addr);

    while (len--)
    {
        *buf++ = hal_rf_spi_wrd(0);
    }
    hal_csn_high();
#endif
}

/**
 * @brief pull down the ce signal by command.
 */
void rf_ce_low(void)
{
#if CE_USE_COMMAND
    rf_operation(RF_CE_LOW);
#else
    hal_ce_low();
#endif
}

/**
 * @brief pull up the ce signal by command.
 */
void rf_ce_high(void)
{
#if CE_USE_COMMAND
    rf_operation(RF_CE_HIGH);
#else
    hal_ce_high();
#endif
}

/**
 * @brief Emit CE pulse by command.
 * @note OM6220 only.
 */
static void rf_ce_pulse(void)
{
    rf_operation(RF_CE_PULSE);
}

/**
 * @brief reuse the tx fifo.
 */
void rf_tx_reuse(void)
{
    rf_operation(RF_TX_REUSE);
}

/**
 * @brief switch hs6220 register bank.
 * @param [in] bank - the bank which want to switch
 */
void rf_bank_switch(RF_Bank_TypeDef bank)
{
    unsigned char sta;

    sta = rf_read_byte(RF_BANK0_STATUS);

    if (bank != RF_Bank0)
    {
        if (!(sta & RF_BANK1))
        {
            rf_wr_cmd(RF_ACTIVATE, RF_ACTIVATE_DATA);
        }
    }
    else
    {
        if (sta & RF_BANK1)
        {
            rf_wr_cmd(RF_ACTIVATE, RF_ACTIVATE_DATA);
        }
    }
}

/**
 * @brief clear hs6220 all interrupt [excpet rtc, clear rtc interrupt by write STATUS_EXT[bit2]].
 *
 */
void rf_clear_all_irq(void)
{
#if HS6220_IRQ_STATUS_EXT_WRITE_CLEAR
    rf_write_byte(RF_BANK0_STATUS_EXT, 0x04);
    rf_write_byte(RF_BANK0_STATUS, 0x70);
#else
    rf_read_byte(RF_BANK0_STATUS_EXT);
#endif
}

/**
 * @brief flush hs6220 rx fifo.
 */
void rf_flush_rx(void)
{
    rf_operation(RF_FLUSH_RX);
}

/**
 * @brief flush hs6220 tx fifo.
 */
void rf_flush_tx(void)
{
    rf_operation(RF_FLUSH_TX);
}

/**
 * @brief soft reset the hs6220 & switch to 4-wire spi.
 */
static void rf_soft_resset(void)
{
    rf_write_byte(RF_BANK0_FEATURE, SOFT_RST); // soft_reset
    rf_write_byte(RF_BANK0_DYNPD, 0x08);       // switch to 4 wire spi mode, By pass IO = 0
}

/* 锟斤拷频校准值 锟斤拷要锟斤拷锟斤拷锟斤拷校准值PLL_CTL0, DAC_RANGE
 1Mbps CAL_CTL  DAC_RANGE
 2Mbps CAL_CTL[  DAC_RANGE
*/
static uint8_t cal_value[2][2];

/**
 * @brief hs6220 radio calibration.
 * @param [in] daterate - 0 for 1Mbps
 *                      - 1 for 2Mbps
 */


bool hs6220_radio_calibration(uint8_t datarate)
{
    uint16_t cal_counter;
    unsigned char temp[4];

    rf_soft_resset(); // hs6200 module soft reset & swich to 4 wire spi mode

    rf_ce_low();
    rf_write_byte(RF_BANK0_CONFIG, 0x8B);  // power up
    hal_delay_ms(3);                       // wait 3 ms
    rf_write_byte(RF_BANK0_PMU_CTL, 0x8C); // RF_PWRDWN = 00 32K_en = 1
    hal_delay_ms(2);                       // wait 2 ms

    rf_write_byte(RF_BANK0_RF_CH, 0x28);

    if (datarate)
        rf_write_byte(RF_BANK0_RF_SETUP, 0x58); // RF_DR_HIGH = 1'b    //only 2Mbps config

    rf_bank_switch(RF_Bank1);

    // rf_write_byte(RF_BANK1_TEST_PKDET, 0x24); //for A1
    /* initial frequency drift pass with CMW500 */
    rf_write_byte(RF_BANK1_TEST_PKDET, 0x20); // for A2

#if 0
	temp[1] = 0x11; //vco_amp_mn=1, manually
    temp[0] = 0xc3; //vco_amp_ctl= 0xc at max value @1000mV
	temp[1] = 0x10; //vco_amp_mn=0, automatically
    temp[0] = 0x01; //vco_amp        @400mV
#endif
    rf_write_byte(RF_BANK1_FAGC_CTRL_1, 0x01); // 300mV
    // rf_write_byte(RF_BANK1_FAGC_CTRL_1, 0x05); //500mV

    if (datarate)
    {
        rf_read_buffer(RF_BANK1_PLL_CTL0, temp, 4);
        temp[3] &= ~0x03;
        temp[3] |= BIT1;
        rf_wr_buffer(RF_BANK1_PLL_CTL0, temp, 4); // PLL_OFFSET_SEL = 10'b   //only 2Mbps config
        rf_write_byte(RF_BANK1_FDEV, 0x29);       // only 2Mbps config
        rf_write_byte(RF_BANK1_RX_CTRL, 0x31);    // h_idx = 01'b            //only 2Mbps config
    }
    else
    {
        /* Adjust threshold of sync algorithm to reduce wrong sync. */
        temp[1] = 0x1c; // sbe_max_th=0 xcorr_th=0x70
        temp[0] = 0x32;
        rf_wr_buffer(RF_BANK1_RX_CTRL, temp, 2);
    }

    rf_bank_switch(RF_Bank0);
    // vco_amp_tx_mux=0 before calibration for small vco_amp: A1's default is 1.
    rf_write_byte(RF_BANK0_FEATURE, 0x10);

    hal_ce_high_pulse(200);

    while ((rf_read_byte(RF_BANK0_RF_SETUP) & 0x20) == 0x00)
    {
        hal_delay_ms(2);
        cal_counter ++;
        if (cal_counter > 500)
        {
            mprintf("cal erro!!!\r\n");
            return false;
        }
    } // wait cal done
    char reg_value;

    rf_write_byte(RF_BANK0_RF_SETUP, rf_read_byte(RF_BANK0_RF_SETUP) & (~BIT4)); // clear cal_en                                                                            // rf_write_byte(RF_BANK0_RF_SETUP,0x60);
    mprintf("cali done\r\n");

    rf_bank_switch(RF_Bank1);
#if 0
    /* save calibration value */
    if(datarate) { //2Mbps
        cal_value[1][0] = rf_read_byte(RF_BANK1_CAL_CTL)|BIT6 ; //save Rc_cal_ctl_reg & Rc_cal_ctr_mn = 1
        cal_value[1][1] = rf_read_byte(RF_BANK1_DAC_RANGE);     //save dac_range   
        /* calibration from register, not from fsm */
        rf_write_byte(RF_BANK1_CAL_CTL, cal_value[1][0]);   //Rc_cal_ctr_mn = 1 & write back Rc_cal_ctl_reg to register
        temp[0] = rf_read_byte(RF_BANK1_PLL_CTL0)|BIT5;     //dac_range_mn = 1
        rf_write_byte(RF_BANK1_PLL_CTL0, temp[0]);        
        rf_write_byte(RF_BANK1_DAC_RANGE, cal_value[1][1]); //write back Rc_cal_ctl_reg to register        
    }
    else { // 1Mbps 
        cal_value[0][0] = rf_read_byte(RF_BANK1_CAL_CTL)|BIT6 ; //save Rc_cal_ctl_reg & Rc_cal_ctr_mn = 1
        cal_value[0][1] = rf_read_byte(RF_BANK1_DAC_RANGE);     //save dac_range 
        /* calibration from register, not from fsm */
        rf_write_byte(RF_BANK1_CAL_CTL, cal_value[0][0]);   //Rc_cal_ctr_mn = 1 & write back Rc_cal_ctl_reg to register
        temp[0] = rf_read_byte(RF_BANK1_PLL_CTL0)|BIT5;     //dac_range_mn = 1
        rf_write_byte(RF_BANK1_PLL_CTL0, temp[0]);        
        rf_write_byte(RF_BANK1_DAC_RANGE, cal_value[0][1]); //write back Rc_cal_ctl_reg to register
    }
#endif

    temp[2] = 0x75; // bp_dac =1 bp_rc = 1
    temp[1] = 0x98; // bp_vco_amp = 1 bp_vco_ldo=1
    temp[0] = 0x20;
    rf_wr_buffer(RF_BANK1_CAL_CTL, temp, 3);

#if 0    
    temp[2] = 0x01; //LNA fsm
    temp[1] = 0x04;  
    temp[0] = 0x17; //0x1D ; [00,1F] Carrier frequency is lowest when 0x1F.
    rf_wr_buffer(RF_BANK1_RF_IVGEN, temp, 3); //xtal_cc = 0x1d
#endif

    rf_write_byte(RF_BANK1_RF_IVGEN, 0x17);
    if ((ble_pwr == Pwr_4db) || (ble_pwr == Pwr_n12db) || (ble_pwr == Pwr_n43db))
    {
        rf_write_byte(RF_BANK1_RF_IVGEN, ble_xtal + 0x80);
    }
    else
        rf_write_byte(RF_BANK1_RF_IVGEN, ble_xtal);
    HS6220_Change_Pwr(Pwr_8db);
    rf_bank_switch(RF_Bank0);
    return true;
}

/**
 * @brief hs6220 radio restore the calibration value.
 *        Note: use for switch 1Mbps & 2Mbps
 * @param [in] daterate - 0 for 1Mbps
 *                      - 1 for 2Mbps
 */
void rf_restore_cali_value(uint8_t datarate)
{
#if 0
    uint8_t temp[4];
	rf_bank_switch(RF_Bank1);
    if(datarate) { //2Mbps
        rf_write_byte(RF_BANK1_CAL_CTL, cal_value[1][0]);   //write back Rc_cal_ctl_reg to register      
        rf_write_byte(RF_BANK1_DAC_RANGE, cal_value[1][1]); //write back Rc_cal_ctl_reg to register 
        rf_read_buffer(RF_BANK1_PLL_CTL0, temp, 4);
        temp[3] &= ~0x03;
        temp[3] |= BIT1;
        rf_wr_buffer(RF_BANK1_PLL_CTL0,temp, 4); //PLL_OFFSET_SEL = 10'b   
        rf_write_byte(RF_BANK1_FDEV, 0x29);                                
        rf_write_byte(RF_BANK1_RX_CTRL, 0x31);   //h_idx = 01'b  modualtion index=0.32          
    }
    else { // 1Mbps 
        rf_write_byte(RF_BANK1_CAL_CTL, cal_value[0][0]);   // write back Rc_cal_ctl_reg to register       
        rf_write_byte(RF_BANK1_DAC_RANGE, cal_value[0][1]); //write back Rc_cal_ctl_reg to register
        rf_read_buffer(RF_BANK1_PLL_CTL0, temp, 4);
        temp[3] &= ~0x03;
        temp[3] |= BIT0;
        rf_wr_buffer(RF_BANK1_PLL_CTL0, temp, 4); //PLL_OFFSET_SEL = 01'b   
        rf_write_byte(RF_BANK1_FDEV, 0x20);                                
        rf_write_byte(RF_BANK1_RX_CTRL, 0x32);   //h_idx = 10'b, modualtion index=0.5           
    }
	rf_bank_switch(RF_Bank0);
#endif
}

volatile int32_t timer0_overtime_rtc_count = 0;
/**
 * @brief 系统的timer3定时器到期后，蓝牙协议栈运行的tick加1
 */
int32_t ble_timer3_irq_deal(void)
{
	timer_clear_irq(TIMER3);
	timer0_overtime_rtc_count++;
}

/**
 * @brief 初始化系统的timer3作为蓝牙协议栈运行的tick，定时周期设置为30us
 */
static void timer3_init(void)
{
	timer_init_t init_t;

	scu_set_device_gate(HAL_TIMER3_BASE, ENABLE);
	scu_set_device_reset(HAL_TIMER3_BASE);
	scu_set_device_reset_release(HAL_TIMER3_BASE);

	init_t.count = get_apb_clk() / 62500; // 32768; //30us
	init_t.div = timer_clk_div_0;
	init_t.width = timer_iqr_width_f;
	init_t.mode = timer_count_mode_auto;

	eclic_irq_enable(TIMER3_IRQn);
    __eclic_irq_set_vector(TIMER3_IRQn, (int32_t)ble_timer3_irq_deal);
	timer_init(TIMER3, init_t);
	timer_start(TIMER3);
	//mprintf("timer3 init finshed,30us\n");
}

/**
 * @brief 初始化射频芯片进入蓝牙ble模式
 */
void hs6220_init(void)
{
    hal_spi_init(); // init spi interface
    timer3_init();
    if(hs6220_radio_calibration(0) == false); // power up calibration
    {
        hs6220_reset_init();
        return; 
    }

    rf_write_byte(RF_BANK0_FEATURE, 0x40); // BLE_EN=1, bp_gau=0(GFSK), vco_amp_tx_mux=0(small vco_amp)

    rf_write_byte(RF_BANK0_PRE_GURD, 0x76); // 15-bit preamble

    // rf_write_byte(RF_BANK0_EN_AA, 0x00);

    rf_write_byte(RF_BANK0_SETUP_VALUE, 0xff);     // rx timeout: 255*16us = 4080us
    rf_write_byte(RF_BANK0_CONFIG_EXT, 0x3d);      // rf tx during writing fifo, enable timer counter
    rf_write_byte(RF_BANK0_BLE_SETUP_DELAY, 0xf0); // sub extra 7us delay
}

void hs6220_reset_init(void)
{
    hs6220_radio_calibration(0); // power up calibration

    rf_write_byte(RF_BANK0_FEATURE, 0x40); // BLE_EN=1, bp_gau=0(GFSK), vco_amp_tx_mux=0(small vco_amp)

    rf_write_byte(RF_BANK0_PRE_GURD, 0x76); // 15-bit preamble

    // rf_write_byte(RF_BANK0_EN_AA, 0x00);

    rf_write_byte(RF_BANK0_SETUP_VALUE, 0xff);     // rx timeout: 255*16us = 4080us
    rf_write_byte(RF_BANK0_CONFIG_EXT, 0x3d);      // rf tx during writing fifo, enable timer counter
    rf_write_byte(RF_BANK0_BLE_SETUP_DELAY, 0xf0); // sub extra 7us delay
}
/**
 * @brief Uninitilize RF module.
 */
void hal_rf_cleanup(void)
{
#if !NIMBLE_CFG_CONTROLLER
    rf_ce_low();
    rf_write_byte(RF_BANK0_STATUS, 0x70);
#endif
    rf_flush_rx();
    rf_flush_tx();
    rf_operation(RF_RSTN);
}

/**
 * @brief 初始化蓝牙功能.
 */
void hal_rf_init(void)
{
    hs6220_init();
#if (NDEBUG) && !defined(NIMBLE_CFG_CONTROLLER)
    hal_dump_rf_register();
#endif
}

/**
 * @note Borrow this routine to setup AdvA for adv tx.
 */
void hal_rf_setup_buf(void)
{
#if !NIMBLE_CFG_CONTROLLER
    rf_wr_buffer(RF_BANK0_TX_ADVA_ADDR, exe_gbuf_bda, 6);
#endif
}

/**
 * @brief Set CRC Init for CRC generator in hw, and reverse it for CRC generator in sw.
 *
 * @param [in] crc_init - 0x00555555 for adv;
 *                        CONNECT_IND.CRCInit for conn.
 * @return the crc init value in reverse order.
 */
uint32_t hal_rf_set_crc_init(uint32_t crc_init)
{
#if !NIMBLE_CFG_CONTROLLER
    if (crc_init == 0x555555)
        rf_write_byte(RF_BANK0_BLE_CTL, 0x03); // adv mode
    else
        rf_write_byte(RF_BANK0_BLE_CTL, 0xed); // connect mode, disable hardware Flow Control
#endif

#if defined(CPU_BIG_ENDIAN)
    little_endian_store_32((uint8_t *)&crc_init, 0, crc_init);
#endif
    rf_wr_buffer(RF_BANK0_CRC_INIT, (uint8_t *)&crc_init, 3);
    return 0;
}

/**
 * @brief Set Access Address to sync the received bit stream.
 *
 * @param [in] access_address - 0x8E89BED6 for adv;
 *                              CONNECT_IND.AA for conn.
 */
void hal_rf_set_aa(uint32_t access_address)
{
#if defined(CPU_BIG_ENDIAN)
    little_endian_store_32((uint8_t *)&access_address, 0, access_address);
#endif
    rf_wr_buffer(RF_BANK0_ACCESS_ADDR, (uint8_t *)&access_address, 4);
}

/**
 * @brief Set RF frequency point for transmit or receive.
 * @note 锟斤拷锟斤拷锟斤拷锟斤拷锟脚碉拷锟斤拷RF Channel Number锟斤拷锟斤拷锟斤拷锟竭硷拷锟脚碉拷锟斤拷锟斤拷RF Channel Index.
 *
 * @param [in] off_2mhz - The offset to 2400MHz, step in 2MHz,
 *                        ie. RF Channel Number.
 */
void hal_rf_set_freq_point(int off_2mhz)
{
    rf_clear_all_irq();
    rf_flush_rx();
    rf_flush_tx();
    rf_write_byte(RF_BANK0_RF_CH, off_2mhz * 2);
}

/**
 * @brief Set transmit power level.
 *
 * @param [in] dBm - The tx power level in dBm.
 */

void hal_rf_set_tx_pwr_lvl(int dBm)
{
}

/**
 * @brief Adjust frequency offset for accurate RF PLL.
 *
 * @param [in] freq_off_cw - The control word of frequency offset.
 */
void hal_rf_set_freq_off(uint8_t freq_off_ctrl)
{
}

/**
 * @brief Set whiten index before transmit or receive.
 *
 * @param [in] chn_inx - The RF Channel Index.
 */
void hal_rf_set_whiten_idx(uint8_t chn_idx)
{
}

/**
 * @brief Emit CE pulse for the inital tx or the long rx.
 * @note The width of pulse cannot be greater than the time of payload,
 *       otherwise digital fsm will go wrong.
 *
 * @param [in] us - the width of the pulse.
 */
void hal_ce_high_pulse(uint8_t us)
{
    ostaskENTER_CRITICAL();
    rf_ce_high();
    hal_delay_us(us);
    rf_ce_low();
    ostaskEXIT_CRITICAL();
}

/**
 * @note Obsoloted.
 */
void hal_rf_debug(void)
{
    rf_write_byte(RF_BANK0_SETUP_RETR, 0xaa);
}

/**
 * @brief Fill a BLE packet, including header and payload, to RF module.
 *
 * @param [in] p_pkt - The pointer to BLE packet.
 */
#if !NIMBLE_CFG_CONTROLLER
void hal_rf_fill_pdu(uint8_t *p_pkt)
{
    rf_wr_buffer(RF_BANK0_TX_HEADER, p_pkt, 2);
    if (exe_stk_state == EXE_LINK_STATE_ADV /*0x0*/)
    {
        rf_wr_buffer(RF_W_TX_PAYLOAD, p_pkt + 8, p_pkt[1] - 6);
    }
    else
    {
        rf_wr_buffer(RF_W_TX_PAYLOAD, p_pkt + 2, p_pkt[1]);
        /* This workaround can avoid that tx fifo's empty fsm goes wrong w/o spi clock, thus one byte won't be sent twice. */
        rf_operation(RF_NOP);
    }
}

/**
 * @brief Prepare the initial tx for adv tx in classic timing.
 */
void hal_rf_init_tx(void)
{
    /* switch to tx */
    rf_write_byte(RF_BANK0_CONFIG, 0x8a);
}

/**
 * @brief Clear the interrupt status of tx.
 */
void hal_rf_clear_tx(void)
{
    rf_ce_low();
    rf_flush_tx();
}
/**
 * @brief Enable the initial tx for adv tx in classic timing.
 */
void hal_rf_enable_tx(void)
{
    hal_ce_high_pulse(40);
}

/**
 * @brief Enable tx for rx2tx in LE timing.
 */
void hal_rf_enable_tx_tifs(void)
{
}

/**
 * @brief Tx long adv packet: rf transmit long adv packet while filling tx fifo.
 * @param [in] p_pkt - The pointer to long adv packet buffer,
 *                     including 2-byte header, and upto 255-byte payload data.
 */
RAM_CODE_SECTION void hal_rf_tx_adv_pkt_long(uint8_t *p_pkt)
{
    uint16_t stat;
    uint8_t pos = 8, len = p_pkt[1] - 6;

    /* Enable rf tx while filling tx fifo. */
    rf_write_byte(RF_BANK0_CONFIG_EXT, 0x39); // fifo_len_ctl=1, msk_tx_aempty=0

    /* Fill tx header of the adv packet. */
    rf_wr_buffer(RF_BANK0_TX_HEADER, p_pkt, 2);
    /* Fill tx fifo with 1st part pdu of the adv packet. */
    rf_wr_buffer(RF_W_TX_PAYLOAD, p_pkt + pos, len >= 32 ? 32 : len);
    if (len >= 32)
    {
        len -= 32;
        pos += 32;
    }
    else
    {
        len = 0;
        pos += len;
    }

    ostaskENTER_CRITICAL();
    /* Emit ce pulse to start rf tx. */
    hal_rf_enable_tx();
    /* Doing rf tx while filling tx fifo. */
    while (len > 0)
    {
        rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&stat, 2);
        /* AEMPTY is half-empty, i.e. spare space is 16-byte. */
        if (stat & TX_AEMPTY)
        {
            uint8_t tlen = len >= 16 ? 16 : len;
            rf_wr_buffer(RF_W_TX_PAYLOAD, p_pkt + pos, tlen);
            len -= tlen;
            pos += tlen;
        }
    }
    ostaskEXIT_CRITICAL();

    /* Wait rf tx done */
    do
    {
        rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&stat, 2);
        if (stat & TX_DS)
        {
            rf_write_byte(RF_BANK0_STATUS, RF_STATUS_TX_DS); // clear tx irq
            break;
        }
    } while (1);

    /* restore CONFIG_EXT. */
    rf_write_byte(RF_BANK0_CONFIG_EXT, 0x3d); // fifo_len_ctl=1, msk_tx_aempty=1
}

/**
 * @brief Scan to recevie short adv packet at lower CPU & SPI frequency.
 * @param [in] p_pkt - The pointer to adv packet buffer,
 *                     including 2-byte header, and upto 37-byte payload data.
 */
RAM_CODE_SECTION void hal_rf_scan_adv_pkt_lowhz(uint8_t *p_pkt)
{
    uint16_t stat;

    rf_write_byte(RF_BANK0_BLE_CTL, 0x06); // scan mode
    /* Enable rf rx while reading rx fifo. */
    // rf_write_byte(RF_BANK0_CONFIG_EXT, 0x3d); //fifo_len_ctl=1

    /* switch to rx & drive CE to high. */
    rf_write_byte(RF_BANK0_CONFIG, 0xab);
    // hal_ce_high_pulse(40);
    rf_ce_high();

    ostaskENTER_CRITICAL();

    /* Poll wait for sync done . */
    do
    {
        rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&stat, 2);
        if (stat & SYNC_DS)
            break;
        /* In case of rx timeout if normal scan mode. */
        if (stat & RX_TIME_OUT)
        {
            rf_write_byte(RF_BANK0_STATUS, RX_DR | SYNC_DS);
            rf_ce_low();
            hal_delay_us(200);
            rf_ce_high();
        }
    } while (1);

    /* Adjust me to gurantee that
       2-byte header are received into register and
       3~6-byte of AdvA are received into rx fifo. */
    hal_delay_us(10); //(2+3)*8
    /* Immediately read AdvA from rx fifo while receving the remains. */
    rf_read_buffer(RF_R_RX_PAYLOAD, &p_pkt[2], 6);

    ostaskEXIT_CRITICAL();

    /* Wait rf rx done */
    hal_rf_wait_rx_done();
    rf_ce_low();

    /* Lazy read rx header from register. */
    rf_read_buffer(RF_BANK0_RX_HEADER, p_pkt, 2);
    /* Omit the received packet if any error. */
    if (stat & (RX_CRC_ERR | RX_LEN_ERR | RX_TYPE_ERR))
    {
        p_pkt[1] = 0;
    }
    if (p_pkt[1] > 37)
    {
        p_pkt[1] = 0;
    }
    /* Lazy read the remain byte from rx fifo. */
    if (p_pkt[1] > 6)
    {
        rf_read_buffer(RF_R_RX_PAYLOAD, &p_pkt[2 + 6], p_pkt[1] - 6);
        bsp_led_set(2, EXE_LED_PULSE_TOGGLE_1US);
    }
}

/**
 * @brief Scan to recevie adv packet and send SCAN_REQ or CONNECT_IND.
 * @note HS6220A2 or OM6220 can run this API.
 *
 * @param [out] p_pkt_rx - The pointer to rx adv packet buffer,
 *                         including 2-byte header, and upto 31-byte payload data.
 * @param [in]  p_pkt_tx - The pointer to tx adv packet buffer,
 *                         including 2-byte header, always SCAN_REQ in 12-byte or CONNECT_IND in 34-byte.
 */
void hal_rf_scan_adv_pkt_tifs(uint8_t *p_pkt_rx, uint8_t *p_pkt_tx)
{
    uint16_t stat;

    /* Borrow ble connect in slave mode to implement scan's rx2tx. */
    rf_write_byte(RF_BANK0_BLE_CTL, 0xED); // connect mode w/o hw fc
    // rf_write_byte(RF_BANK0_BLE_CTL, 0x06); //scan mode
    /* Enable rf rx while reading rx fifo. */
    rf_write_byte(RF_BANK0_CONFIG_EXT, 0x39); // fifo_len_ctl=1, msk_tx_aempty=0

    /* switch to rx & drive CE to high. */
    rf_write_byte(RF_BANK0_CONFIG, 0xab);
    /* Use CE pulse rather than high level, otherwise hw fsm doesn't transmit packet. */
    hal_ce_high_pulse(40);
    // rf_ce_pulse(); //OM6220 only

    ostaskENTER_CRITICAL();

    /* Don't wait for sync done, but rx done. */

    /* Wait rf rx done */
    while (1)
    {
        rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&stat, 2);
        if (stat & (RX_CRC_ERR | RX_LEN_ERR | RX_TYPE_ERR | RX_TIME_OUT | RX_DR))
        {
#if HS6220_IRQ_STATUS_EXT_WRITE_CLEAR
            rf_write_byte(RF_BANK0_STATUS, RX_DR | SYNC_DS);
#endif
            break;
        }
    }
    /* Omit the received packet if any error, including timeout. */
    if (stat & (RX_CRC_ERR | RX_LEN_ERR | RX_TYPE_ERR | RX_TIME_OUT))
    {
        p_pkt_rx[1] = 0;
        hal_rf_cleanup();
        ostaskEXIT_CRITICAL();
        hal_delay_us(200);
        return;
    }

    /* Fill the tx fifo to transmit. */
    if (p_pkt_tx && p_pkt_tx[1] > 0)
    {
        uint8_t len = p_pkt_tx[1] > 32 ? 32 : p_pkt_tx[1];
        rf_wr_buffer(RF_BANK0_TX_HEADER, p_pkt_tx, 2);
        rf_wr_buffer(RF_W_TX_PAYLOAD, p_pkt_tx + 2, len);
        len = p_pkt_tx[1] - len;
        if (len > 0)
        {
            hal_delay_us(4 * 8);
            while (1)
            {
                rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&stat, 2);
                /* AEMPTY is half-empty, i.e. spare space is 16-byte. */
                if (stat & TX_AEMPTY)
                    break;
            }
            rf_wr_buffer(RF_W_TX_PAYLOAD, p_pkt_tx + 2 + 32, len);
        }
        rf_operation(RF_NOP);
    }

    ostaskEXIT_CRITICAL();

    /* Lazy read rx header from register. */
    rf_read_buffer(RF_BANK0_RX_HEADER, p_pkt_rx, 2);
    /* Omit the received packet if rx fifo overflow. */
    if (p_pkt_rx[1] > 32)
    {
        p_pkt_rx[1] = 0;
        hal_rf_cleanup();
        return;
    }

    /* Lazy read AdvA+AdvData from rx fifo. */
    if (p_pkt_rx[1] > 0)
    {
        rf_read_buffer(RF_R_RX_PAYLOAD, &p_pkt_rx[2], p_pkt_rx[1]);
        bsp_led_set(2, EXE_LED_PULSE_TOGGLE_1US);
        /* Check whether the received ADV_IND.AdvA is matched with SCAN_REQ.AdvA or CONNECT_IND.AdvA. */
        if (0 != memcmp(&p_pkt_rx[2], &p_pkt_tx[2 + 6], 6))
        {
            hal_rf_cleanup();
            p_pkt_rx[1] = 0;
            return;
        }
    }

    if (p_pkt_tx && p_pkt_tx[1] > 0)
    {
        hal_delay_us((p_pkt_tx[1] + 25) * 8 /*bit*/ - 73 /*read spi*/);
        /* Poll wait for tx done . */
        do
        {
            rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&stat, 2);
            if (stat & TX_DS)
                break;
        } while (1);
    }
    hal_rf_cleanup();

    /* restore CONFIG_EXT. */
    rf_write_byte(RF_BANK0_CONFIG_EXT, 0x3d); // fifo_len_ctl=1, msk_tx_aempty=1
}

/**
 * @brief Scan to recevie adv packet in whitelist and send SCAN_REQ or CONNECT_IND.
 * @note HS6220 cannot run this API, OM6220 can.
 * @note The bluetooth device address in whitelist doesn't contain address type.
 *
 * @param [out] p_pkt_rx - The pointer to rx adv packet buffer,
 *                         including 2-byte header, and upto 37-byte payload data.
 * @param [in]  p_pkt_tx - The pointer to tx adv packet buffer,
 *                         including 2-byte header, always SCAN_REQ in 12-byte or CONNECT_IND in 34-byte.
 * @param [in] p_whitelist - The pointer to whitelist, 6-byte AdvA per entry, 2-entry.
 *                           The scanner receives the adv packets which AdvA is matched within whitelist.
 *                           Disable whitelist if NULL.
 */
void hal_rf_scan_adv_pkt_tifs_wl(uint8_t *p_pkt_rx, uint8_t *p_pkt_tx, uint8_t *p_whitelist)
{
    uint16_t stat;
    uint8_t rx_len, tx_len = p_pkt_tx ? p_pkt_tx[1] : 0;

    if (p_whitelist)
    {
        rf_write_byte(RF_BANK0_BLE_CTL, 0x07); // filter scan mode.
        rf_wr_buffer(RF_BANK0_WHITE_LIST_RXADDR_1, p_whitelist, 6);
        rf_wr_buffer(RF_BANK0_WHITE_LIST_RXADDR_2, p_whitelist + 6, 6);
    }
    else
    {
        rf_write_byte(RF_BANK0_BLE_CTL, 0x06); // normal scan mode.
    }

    /* switch to rx & drive CE to high. */
    rf_write_byte(RF_BANK0_CONFIG, 0xab);
    rf_ce_high();

    ostaskENTER_CRITICAL();

    /* Don't wait for sync done, since hw fsm might continue rx if not matched in whitelist. */

    /* Wait rf rx done */
    while (1)
    {
        rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&stat, 2);
        if (stat & (RX_CRC_ERR | RX_LEN_ERR | RX_TYPE_ERR | RX_TIME_OUT | RX_DR))
        {
#if HS6220_IRQ_STATUS_EXT_WRITE_CLEAR
            rf_write_byte(RF_BANK0_STATUS, RX_DR | SYNC_DS);
#endif
            break;
        }
    }
    rf_ce_low();
    /* Omit the received packet if any error, including timeout. */
    if (stat & (RX_CRC_ERR | RX_LEN_ERR | RX_TYPE_ERR | RX_TIME_OUT))
    {
        p_pkt_rx[1] = 0;
        hal_rf_cleanup();
        ostaskEXIT_CRITICAL();
        return;
    }

    /* Fill the register and tx fifo to transmit. */
    if (p_pkt_tx && tx_len > 0)
    {
        rf_wr_buffer(RF_BANK0_TX_HEADER, p_pkt_tx, 2);
        rf_wr_buffer(RF_BANK0_TX_ADVA_ADDR, p_pkt_tx + 2, 6);
        if (tx_len > 6)
        {
            rf_wr_buffer(RF_W_TX_PAYLOAD, p_pkt_tx + 2 + 6, tx_len - 6);
        }
        rf_operation(RF_NOP);
    }

    ostaskEXIT_CRITICAL();

    /* Lazy read rx header from register. */
    rf_read_buffer(RF_BANK0_RX_HEADER, p_pkt_rx, 2);
    /* Omit the received packet if rx fifo overflow. */
    if (p_pkt_rx[1] > 37)
    {
        p_pkt_rx[1] = 0;
        hal_rf_cleanup();
        return;
    }
    rx_len = p_pkt_rx[1];

    /* Lazy read AdvA+AdvData from register & rx fifo. */
    if (rx_len > 0)
    {
        rf_read_buffer(RF_BANK0_RX_ADVA_ADDR, &p_pkt_rx[2], 6);
        if (rx_len > 6)
        {
            rf_read_buffer(RF_R_RX_PAYLOAD, &p_pkt_rx[2 + 6], rx_len - 6);
        }
        bsp_led_set(2, EXE_LED_PULSE_TOGGLE_1US);
    }
    p_pkt_rx[1] = rx_len;

    if (p_pkt_tx && tx_len > 0)
    {
        hal_delay_us((tx_len + 25) * 8 /*bit*/ - 73 /*read spi*/);
        /* Poll wait for tx done . */
        do
        {
            rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&stat, 2);
            if (stat & TX_DS)
                break;
        } while (1);
    }
    hal_rf_cleanup();
}

/**
 * @brief Enable rx in long rx always.
 */
void hal_rf_enable_rx(void)
{
    if (exe_stk_state != EXE_LINK_STATE_ADV /*0x0*/)
    {
        /* switch to rx & drive a ce pulse */
        rf_write_byte(RF_BANK0_CONFIG, 0xab);
        hal_ce_high_pulse(40);
    }
}

/**
 * @brief Enable the initial tx for adv tx in classic timing.
 */
void hal_rf_disable_rx(void)
{
}

/**
 * @brief Check whether adv tx is done, if tx done, clear the tx interrupt.
 *
 * @return true for transmitted, false for not.
 */
bool hal_rf_is_tx_done(void)
{
    if (hal_get_irq_status())
        return false;
    else
    {
        rf_write_byte(RF_BANK0_STATUS, RF_STATUS_TX_DS); // clear tx irq
        return true;
    }
}

/**
 * @brief Check whether receive the header of a ble packet.
 * @note It also implies synced.
 *
 * @return true for received, false for not.
 */
bool hal_rf_is_rx_hdr(void)
{
    if (hal_get_irq_status())
    {
        return false;
    }
    rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&g_rx_status, 2);
    if (g_rx_status & SYNC_DS)
    {
#if !HAL_BLE_HW_TIMER
        /* ygw: 8us algorithm sync cost on HS6220; 20+us on HS660x. */
        BLETICK_ON_SYNC = hal_sys_tick() - EXE_US_TO_SYSTICK(40 /*PRE1+AA4*/ + 12 /*sync cost*/ + 8 /*irq cost*/);
#endif
        /* read rx header to get rx length */
        rf_read_buffer(RF_BANK0_RX_HEADER, ll_gbuf_xpkt_rx + 4, 2);
        g_rx_header = (ble_adv_header_t *)(ll_gbuf_xpkt_rx + 4);
#if HS6220_IRQ_STATUS_EXT_WRITE_CLEAR /* clear by hal_rf_wait_rx_done() */
        // rf_write_byte(RF_BANK0_STATUS, SYNC_DS);
#endif
        return true;
    }

    return false;
}

/**
 * @note Obsoloted.
 */
void hal_rf_set_rx_size(int size) {}
void hal_rf_rx_data(int bytes) {}

/**
 * @brief clear the rtc timer interrupt.
 */
void hal_rf_clear_rtc_timer_interrupt(void)
{
#if HS6220_IRQ_STATUS_EXT_WRITE_CLEAR
    rf_write_byte(RF_BANK0_STATUS_EXT, 0x04);
#endif
}

/**
 * @brief lock the rtc current value.
 */
void hal_tim_tick_lock(void)
{
    rf_operation(RF_TIM_LOCK);
    while ((rf_read_byte(RF_BANK0_EN_AA) & BIT4) == 0)
        ;
}

/**
 * @brief Read the current timer counter, must call hal_tim_tick_lock() before read.
 *
 * @param [in] none
 *
 * @return the rtc value at 32kHz when call hal_tim_tick_lock() locked.
 */
uint32_t hal_rf_read_tim_tick(void)
{
    uint32_t rtc_tick;
    hal_tim_tick_lock();
    rf_read_buffer(RF_BANK0_BLE_TIM_CNT, (uint8_t *)&rtc_tick, 4);
#if defined(CPU_BIG_ENDIAN)
    return little_endian_read_32((const uint8_t *)&rtc_tick, 0);
#else
    return rtc_tick;
#endif
}

/**
 * @brief Read the latest time on sync.
 *
 * @return the time at 32kHz on sync.
 */
uint32_t hal_rf_read_sync_tick(void)
{
    uint32_t rtc_tick;
    rf_read_buffer(RF_BANK0_CLKN_SYNC, (uint8_t *)&rtc_tick, 4);
#if defined(CPU_BIG_ENDIAN)
    return little_endian_read_32((const uint8_t *)&rtc_tick, 0);
#else
    return rtc_tick;
#endif
}

/**
 * @brief set the tick value to rtc timer.
 *
 * @param [in] tick - the tick value will be set.
 * @return none
 */
void hal_rf_set_tim_tick(uint32_t tick)
{
#if defined(CPU_BIG_ENDIAN)
    little_endian_store_32((uint8_t *)&tick, 0, tick);
#endif
    rf_wr_buffer(RF_BANK0_BLE_TIM_CNT, (uint8_t *)&tick, 4);
}

/**
 * @brief set the delta tick value to rtc timer.
 * @note  OM6220 only.
 *
 * @param [in] delta - the delta tick value will be set.
 */
void hal_rf_set_rtc_timer_delta(uint32_t delta)
{
    // rf_write_byte(RF_BANK0_CONFIG_EXT, 0x3d); //rf tx during writing fifo, enable rtc timer irq and counter
    rf_write_byte(RF_BANK0_RTC_TIMER_MODE, 0x2);
    rf_write_byte(RF_BANK0_BLE_TIM_CNT, delta);
    /* Don't restore the absoluate mode at once. */
    // rf_write_byte(RF_BANK0_RTC_TIMER_MODE, 0x0);
}

/**
 * @brief Poll wait for received a ble packet or receive error.
 */
void hal_rf_wait_rx_done(void)
{
    /* Poll wait for interrupt */
    while (1)
    {
        rf_read_buffer(RF_BANK0_STATUS_EXT, (uint8_t *)&g_rx_status, 2);
        if (g_rx_status & (RX_CRC_ERR | RX_LEN_ERR | RX_TYPE_ERR | RX_DR))
        {
#if HS6220_IRQ_STATUS_EXT_WRITE_CLEAR
            rf_write_byte(RF_BANK0_STATUS, RX_DR | SYNC_DS);
#endif
            break;
        }
    }
}

/**
 * @brief Read a BLE packet's payload, from RF module.
 * @param [in] pkt_len - The payload length of BLE packet.
 */
void hal_rf_rx_all(int pkt_len)
{
    if (pkt_len > 37)
        return;
    if (pkt_len == 0)
        return;
    if (exe_stk_state == EXE_LINK_STATE_ADV /*0x0*/)
    {
        rf_read_buffer(RF_R_RX_PAYLOAD, ll_gbuf_xpkt_rx + 12, g_rx_header->Length - 6);
        if (g_rx_header->Type == CONNECT_IND)
            rf_read_buffer(RF_BANK0_RX_ADVA_ADDR, ll_gbuf_xpkt_rx + 6, 6);
    }
    else // if (exe_stk_state == EXE_LINK_STATE_ADV/*0x0*/)
    {
        rf_read_buffer(RF_R_RX_PAYLOAD, ll_gbuf_xpkt_rx + 6, g_rx_header->Length);
    }
}

/**
 * @brief Check whether receive CRC error occurs, including type or len errors.
 *
 * @return true for receive CRC error,
 *         false for receive a good packet.
 */
bool hal_rf_is_crc_err(void)
{
    return g_rx_status & 0x0e00; //(CRC_ERR+LEN_ERR+TYPE_ERR);
}

/**
 * @note Obsoloted.
 */
bool hal_rf_is_sync_err(void)
{
    return g_rx_status & RX_SYNC_ERR;
}

/**
 * @note Obsoloted.
 */
bool hal_rf_is_length_err(void)
{
    return g_rx_status & RX_LEN_ERR;
}

/**
 * @brief Stop all tx and rx state machine of RF module.
 */
void hal_rf_stop_tx_rx(void)
{
    /* Abort adv event: ADV_IND/SCAN_REQ where scan_req.AdvA is not me. */
    rf_write_byte(RF_BANK0_STATUS, 0x70);
}

/**
 * @brief wakeup th RF module from sleep mode.
 * @param [in] mode - RF_DEEP_SLEEP for deep sleep wake up, the xtal is stop.
 *                  - RF_LIGHT_SLEEP for standby sleep wake up, the xtal is active.
 */
void hal_rf_wakeup(uint8_t mode)
{
    if (mode == RF_LIGHT_SLEEP)
        rf_write_byte(RF_BANK0_PMU_CTL, 0xac); // RF_PWRDWN = b'00
    else
    {
        rf_write_byte(RF_BANK0_CONFIG, 0x8B);  // power up
        hal_delay_ms(3);                       // wait 3 ms
        rf_write_byte(RF_BANK0_PMU_CTL, 0x8C); // RF_PWRDWN = 00 32K_en = 1
        hal_delay_ms(2);                       // wait 2 ms
    }
}

/**
 * @brief th RF module enter sleep mode.
 * @param [in] mode - RF_DEEP_SLEEP for deep sleep, the xtal is stop.
 *                 - RF_LIGHT_SLEEP for standby sleep, the xtal is active.
 */
void hal_rf_sleep(uint8_t mode)
{
    if (mode == RF_LIGHT_SLEEP)
        rf_write_byte(RF_BANK0_PMU_CTL, 0xae); // RF_PWRDWN = b'10
    else
    {
        rf_write_byte(RF_BANK0_PMU_CTL, 0xad); // RF_PWRDWN = b'01
        hal_delay_ms(2);
        rf_write_byte(RF_BANK0_CONFIG, 0x00); // PWR_UP = 0;
    }
}

#endif // #if !NIMBLE_CFG_CONTROLLER end

#if !defined(NDEBUG)
/**
 * @brief dump all hs6220 register.
 */
void hal_dump_rf_register(void)
{
  unsigned char  reg_value;
  unsigned char  temp[6]; 
  signed char i;
  
  rf_bank_switch(RF_Bank0);
    
  rf_read_buffer(RF_BANK0_CONFIG, &reg_value, 1);
  mprintf("\r\n\r\n\r\nRF_BANK0_CONFIG: %02x\r\n", reg_value);  
  

  rf_read_buffer(RF_BANK0_EN_AA, &reg_value, 1);
  mprintf("RF_BANK0_EN_AA: %02x\r\n", reg_value); 

  rf_read_buffer(RF_BANK0_EN_RXADDR, &reg_value, 1);
  mprintf("RF_BANK0_EN_RXADDR: %02x\r\n", reg_value);  
  

  rf_read_buffer(RF_BANK0_PMU_CTL, &reg_value, 1);
  mprintf("RF_BANK0_PMU_CTL: %02x\r\n", reg_value); 
  
  rf_read_buffer(RF_BANK0_SETUP_RETR, &reg_value, 1);
  mprintf("RF_BANK0_SETUP_RETR: %02x\r\n", reg_value);	
	
  
  rf_read_buffer(RF_BANK0_RF_CH, &reg_value, 1);
  mprintf("RF_BANK0_HS6220_CH: %02x\r\n", reg_value); 
  
  rf_read_buffer(RF_BANK0_RF_SETUP, &reg_value, 1);
  mprintf("RF_BANK0_HS6220_SETUP: %02x\r\n", reg_value); 

  rf_read_buffer(RF_BANK0_STATUS, &reg_value, 1);
  mprintf("RF_BANK0_STATUS: %02x\r\n", reg_value); 
	
  rf_read_buffer(RF_BANK0_ACCESS_ADDR, temp, 4);
  mprintf("RF_BANK0_ACCESS_ADDR: "); 
  for(i=3;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");
  
  rf_read_buffer(RF_BANK0_RX_ADDR_P1, &reg_value, 1);
  mprintf("RF_BANK0_RX_ADDR_P1: %02x\r\n", reg_value); 
  
  rf_read_buffer(RF_BANK0_BLE_CTL, &reg_value, 1);
  mprintf("RF_BANK0_BLE_CTL: %02x\r\n", reg_value); 

  rf_read_buffer(RF_BANK0_TX_ADVA_ADDR, temp, 6);
  mprintf("RF_BANK0_TX_ADVA_ADDR: "); 
  for(i=5;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");
	
  rf_read_buffer(RF_BANK0_TX_HEADER, temp, 2);
  mprintf("RF_BANK0_TX_HEADER: "); 
  for(i=1;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");
  
  rf_read_buffer(RF_BANK0_CRC_INIT, temp, 3);
  mprintf("RF_BANK0_CRC_INIT: "); 
  for(i=2;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");
	
  
  rf_read_buffer(RF_BANK0_RX_HEADER, temp, 2);
  mprintf("RF_BANK0_RX_HEADER: "); 
  for(i=1;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");
  
  rf_read_buffer(RF_BANK0_RX_ADVA_ADDR, temp, 6);
  mprintf("RF_BANK0_RX_ADVA_ADDR: "); 
  for(i=5;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");

 rf_read_buffer(RF_BANK0_FIFO_STATUS, &reg_value, 1);
  mprintf("RF_BANK0_FIFO_STATUS: %02x\r\n", reg_value);  
  
  rf_read_buffer(RF_BANK0_DYNPD, &reg_value, 1);
  mprintf("RF_BANK0_DYNPD: %02x\r\n", reg_value); 
  
  rf_read_buffer(RF_BANK0_FEATURE, &reg_value, 1);
  mprintf("RF_BANK0_FEATURE: %02x\r\n", reg_value); 


  rf_read_buffer(RF_BANK0_SETUP_VALUE, temp, 3);
  mprintf("RF_BANK0_SETUP_VALUE: ");
  for(i=2;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");
  
  
  rf_read_buffer(RF_BANK0_PRE_GURD, temp, 3);
  mprintf("RF_BANK0_PRE_GURD: ");  
  for(i=2;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n\r\n");
  
  rf_bank_switch(RF_Bank1);

  rf_read_buffer(RF_BANK1_CHIP_ID, temp, 2);
  mprintf("RF_BANK1_CHIP_ID:");
  for(i=1;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n"); 
  
  rf_read_buffer(RF_BANK1_PLL_CTL0, temp, 4);
  mprintf("RF_BANK1_PLL_CTL0:");
  for(i=3;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");  

  rf_read_buffer(RF_BANK1_PLL_CTL1, temp, 3);
  mprintf("RF_BANK1_PLL_CTL1:");
  for(i=2;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");

  rf_read_buffer(RF_BANK1_CAL_CTL, temp, 5);
  mprintf("RF_BANK1_CAL_CTL:");
  for(i=4;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");

  rf_read_buffer(RF_BANK1_STATUS, &reg_value, 1);
  mprintf("RF_BANK1_STATUS:%02x\r\n", reg_value);   

  rf_read_buffer(RF_BANK1_STATE, &reg_value, 2);
  mprintf("RF_BANK1_STATE:");
  for(i=1;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n"); 

  rf_read_buffer(RF_BANK1_CHAN, &reg_value, 4);
  mprintf("RF_BANK1_CHAN:");
  for(i=3;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");    


  rf_read_buffer(RF_BANK1_FDEV, &reg_value, 1);
  mprintf("RF_BANK1_FDEV:%02x\r\n", reg_value);  
	
  rf_read_buffer(RF_BANK1_DAC_RANGE, &reg_value, 1);
  mprintf("RF_BANK1_DAC_RANGE:%02x\r\n", reg_value);  

  rf_read_buffer(RF_BANK1_CTUNING, temp, 2);
  mprintf("RF_BANK1_CTUNING:");
  for(i=3;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n"); 
	
  rf_read_buffer(RF_BANK1_RX_CTRL, temp, 2);
  mprintf("RF_BANK1_RX_CTRL:");
  for(i=3;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");    

  rf_read_buffer(RF_BANK1_FAGC_CTRL_1, temp, 4);
  mprintf("RF_BANK1_FAGC_CTRL_1:");
  for(i=3;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n"); 
	
  rf_read_buffer(RF_BANK1_DOC_DACI, &reg_value, 1);
  mprintf("RF_BANK1_DOC_DACI:%02x\r\n", reg_value);  
	
  rf_read_buffer(RF_BANK1_DOC_DACQ, &reg_value, 1);
  mprintf("RF_BANK1_DOC_DACQ:%02x\r\n", reg_value);  

  rf_read_buffer(RF_BANK1_AGC_CTRL, temp, 4);
  mprintf("RF_BANK1_AGC_CTRL:");
  for(i=3;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n"); 
	
  rf_read_buffer(RF_BANK1_AGC_GAIN, temp, 4);
  mprintf("RF_BANK1_AGC_GAIN:");
  for(i=3;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n"); 

  rf_read_buffer(RF_BANK1_RF_IVGEN, temp, 3);
  mprintf("RF_BANK1_RF_IVGEN:");
  for(i=2;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n"); 
	
  rf_read_buffer(RF_BANK1_TEST_PKDET, temp, 3);
  mprintf("RF_BANK1_TEST_PKDET:");
  for(i=2;i>=0;i--)
    mprintf("%02x " , temp[i]);
  mprintf("\r\n");

  rf_bank_switch(RF_Bank0);	
}
#endif //(NDEBUG)
/** @} */
