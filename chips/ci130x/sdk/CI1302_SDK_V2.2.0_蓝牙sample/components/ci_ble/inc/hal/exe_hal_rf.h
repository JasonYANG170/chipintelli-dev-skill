/*============================================================================*/
/* @file exe_hal_rf.h
 * @brief EXE HAL Radio header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_RF_H__
#define __EXE_HAL_RF_H__

#include <stdint.h>
#include <stdbool.h>

#define CE_USE_COMMAND       1

/* Comment the lines after HS6220A1 ECO. */
#define HS6220_IRQ_STATUS_EXT_WRITE_CLEAR 1


/***************************  蓝牙收发器管理相关函数  *************************/

/**
 * @brief Initilize RF module.
 */
void hal_rf_init(void);

/**
 * @brief Uninitilize RF module.
 */
void hal_rf_cleanup(void);

/**
 * @note Obsoloted.
 */
void hal_rf_setup_buf(void);

/**
 * @brief Set CRC Init for CRC generator in hw, and reverse it for CRC generator in sw.
 *
 * @param [in] crc_init - 0x00555555 for adv;
 *                        CONNECT_IND.CRCInit for conn.
 * @return the crc init value in reverse order.
 */
uint32_t hal_rf_set_crc_init(uint32_t crc_init);

/**
 * @brief Set Access Address to sync the received bit stream.
 *
 * @param [in] access_address - 0x8E89BED6 for adv;
 *                              CONNECT_IND.AA for conn.
 */
void hal_rf_set_aa(uint32_t access_adddress);

/**
 * @brief Set RF frequency point for transmit or receive.
 * @note 传入物理信道号RF Channel Number，不是逻辑信道索引RF Channel Index.
 *
 * @param [in] off_2mhz - The offset to 2400MHz, step in 2MHz,
 *                        ie. RF Channel Number.
 */
void hal_rf_set_freq_point(int off_2mhz);

/**
 * @brief Set transmit power level.
 *
 * @param [in] dBm - The tx power level in dBm.
 */
void hal_rf_set_tx_pwr_lvl(int dBm);

/**
 * @brief Adjust frequency offset for accurate RF PLL.
 *
 * @param [in] freq_off_cw - The control word of frequency offset.
 */
void hal_rf_set_freq_off(uint8_t freq_off_cw);


/**
 * @brief Set whiten index before transmit or receive.
 *
 * @param [in] chn_inx - The RF Channel Index.
 */
void hal_rf_set_whiten_idx(uint8_t chn_idx);

/**
 * @brief Fill a BLE packet, including header and payload, to RF module.
 *
 * @param [in] p_pkt - The pointer to BLE packet.
 */
void hal_rf_fill_pdu(uint8_t *p_pkt);


/**
 * @brief Prepare the initial tx for adv tx in classic timing.
 */
void hal_rf_init_tx(void);

/**
 * @brief Clear the interrupt status of tx.
 */
void hal_rf_clear_tx(void);

/**
 * @brief Enable the initial tx for adv tx in classic timing.
 * @note 本函数用于adv事件中的初始发送包时序，其本质上是经典蓝牙时序。
 */
void hal_rf_enable_tx(void);

/**
 * @brief Enable tx for rx2tx in LE timing.
 * @note 本函数用于adv或conn事件中的收转发的发送包时序，要遵循LE的TIFS时序。
 */
void hal_rf_enable_tx_tifs(void);

/**
 * @brief Check whether adv tx is done.
 *
 * @return true for transmitted, false for not.
 */
bool hal_rf_is_tx_done(void);

/**
 * @brief Enable rx in long rx always, for adv rx or conn rx.
 */
void hal_rf_enable_rx(void);

/**
 * @brief Disable rx after received or receive error.
 */
void hal_rf_disable_rx(void);

/**
 * @brief Check whether receive the header of a ble packet.
 * @note It also implies synced.
 *
 * @return true for received, false for not.
 */
bool hal_rf_is_rx_hdr(void);

/**
 * @brief Poll wait for received a ble packet or receive error.
 */
void hal_rf_wait_rx_done(void);

/**
 * @note Obsoloted.
 */
void hal_rf_set_rx_size(int size);
void hal_rf_rx_data(int bytes);

/**
 * @brief Read a BLE packet's payload, from RF module.
 * @param [in] pkt_len - The payload length of BLE packet.
 */
void hal_rf_rx_all(int pkt_len);

/**
 * @brief Check whether receive CRC error occurs, including type or len errors.
 *
 * @return true for receive CRC error,
 *         false for receive a good packet.
 */
bool hal_rf_is_crc_err(void);

/**
 * @brief Stop all tx and rx state machine of RF module.
 * @note 本函数用于中止（abort）当前的adv或conn事件，比如发出ADV_NONCONN_IND后，或者收到CRC错的包后。
 *       当RF模块（HS6220A2）不支持该功能时，exBLE需要更多代码逻辑来特殊实现。
 */
void hal_rf_stop_tx_rx(void);

/**
 * @note HS6220 RF module specific.
 */
 
/**
 * @brief wakeup th RF module from sleep mode.
 * @param [in] mode - RF_DEEP_SLEEP for deep sleep wake up, the xtal is stop in sleep state.
 *                  - RF_LIGHT_SLEEP for standby sleep wake up, the xtal is active in sleep state.
 */
void hal_rf_wakeup(uint8_t mode);

/**
 * @brief th RF module enter sleep mode.
 * @param [in] mode - RF_DEEP_SLEEP for deep sleep, the xtal is stop.
 *                  - RF_LIGHT_SLEEP for standby sleep, the xtal is active.
 */
void hal_rf_sleep(uint8_t mode);

/**
 * @brief set the absolute tick value to rtc timer.
 *
 * @param [in] tick - the tick value will be set.
 * @return none
 */
void hal_rf_set_tim_tick(uint32_t tick);

/**
 * @brief set the delta tick value to rtc timer.
 * @note  OM6220 only.
 *
 * @param [in] delta - the delta tick value will be set.
 */
void hal_rf_set_rtc_timer_delta(uint32_t delta);

/**
 * @brief read hs6220 rtc timer value, must call hal_tim_tick_lock() before read.
 *
 * @param [in] none
 * 
 * @return the rtc value when call hal_tim_tick_lock() locked.
 */
uint32_t hal_rf_read_tim_tick(void);

/**
 * @brief Read the latest time on sync.
 * 
 * @return the time at 32kHz on sync.
 */
uint32_t hal_rf_read_sync_tick(void);


#endif /* #ifndef __EXE_HAL_RF_H__ */

