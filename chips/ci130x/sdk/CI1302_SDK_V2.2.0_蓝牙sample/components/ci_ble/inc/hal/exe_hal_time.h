/*============================================================================*/
/* @file exe_hal_time.h
 * @brief EXE HAL time header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_TIME_H__
#define __EXE_HAL_TIME_H__

#include <stdint.h>
#include <stdbool.h>
#define CI13XX                          1

#if defined(LZ17H26)
#if EXE_NDA_HAL_LENZE
#define HAL_TIMER_HIGH_RESOLUTION       1
#define HAL_BLE_HW_WHITEN               0
#define HAL_BLE_HW_CRC                  0
#define SYS_TICK_HZ                     32000000
#define SYS_TICK_HALF                   0x3FFFFFFF
#define hal_sys_tick()                  reg_system_tick
#define hal_sys_tick_set(tick)          reg_system_tick = (tick)
#endif /* EXE_NDA */

#elif defined(HS6601) || defined(HS6601c)
#include <nds32_intrinsic.h>
///high resolution timer: 1us.
#define HAL_TIMER_HIGH_RESOLUTION       1
#define SYS_TICK_HZ                     48000000
#define SYS_TICK_HALF                   0x7FFFFFFF
#define hal_sys_tick()                  __nds32__mfsr(NDS32_SR_PFMC1)
#define hal_sys_tick_set(tick)          __nds32__mtsr((tick), NDS32_SR_PFMC1)

#elif defined(__MM32_MINIBOARD )
///high resolution timer: 1us.
#define HAL_TIMER_HIGH_RESOLUTION       1
#define SYS_TICK_HZ                     48000000
#define SYS_TICK_HALF                   0x7FFFFFFF
#define hal_sys_tick()                  (*(volatile uint32_t *)0x40000024)  //TIM2->CNT
#define hal_sys_tick_set(tick)         *(volatile uint32_t *)0x40000024 = (tick) //TIM2->CNT = tick

#elif defined(STM32F10X_MD)
///lower resolution timer.
uint32_t tim_us_ticker_read(void);
#define HAL_TIMER_HIGH_RESOLUTION       0
#define SYS_TICK_HZ                     (8000000/128)
#define SYS_TICK_HALF                   0x7FFFFFFF
#define hal_sys_tick()               tim_us_ticker_read() // (((uint32_t)RTC->CNTH << 16 ) | RTC->CNTL) 
void hal_sys_tick_set(uint32_t tick); 
///BLE tick use HS6220's timer.
#define HAL_BLE_HW_TIMER                1

#elif defined(THK88_BLE)
///lower resolution timer.
uint32_t hal_get_sys_tick(void);
void hal_sys_tick_set(uint32_t tick);
#define HAL_TIMER_HIGH_RESOLUTION       1
#define SYS_TICK_HZ                     (12000000)
#define SYS_TICK                        0xFFFFFFFF
#define SYS_TICK_HALF                   0x7FFFFFFF
#define hal_sys_tick()                  hal_get_sys_tick()         
//#define hal_sys_tick_set(tick)          *(volatile uint32_t *)0

#elif defined(C8051F380)
uint32_t tim_us_ticker_read(void);
///high resolution timer.
#define HAL_TIMER_HIGH_RESOLUTION       1
#define SYS_TICK_HZ                     (1000000)
#define SYS_TICK_HALF                   0x7FFFFFFF
#define hal_sys_tick()               tim_us_ticker_read()
void hal_sys_tick_set(uint32_t tick);

#elif defined(UM800_8051)
uint32_t hal_rf_read_tim_tick(void);
///high resolution timer.
#define HAL_TIMER_HIGH_RESOLUTION       1
#define SYS_TICK_HZ                     (32000)
#define SYS_TICK_HALF                   0x7FFFFFFF
#define hal_sys_tick()               hal_rf_read_tim_tick()
void hal_sys_tick_set(uint32_t tick);

#elif defined(HM1001_M0)
///lower resolution timer.
uint32_t tim_us_ticker_read(void);
#define HAL_TIMER_HIGH_RESOLUTION       0
#define SYS_TICK_HZ                     (32000)
#define SYS_TICK_HALF                   0x7FFFFFFF
#define hal_sys_tick()                (*(volatile uint32_t *)0x4004200C)  //RTC->TIME
#define hal_sys_tick_set(tick)        //*(volatile uint32_t *)0x40041884 = (tick) //TIMER->CNT = tick

///BLE tick use HS6220's timer.
#define HAL_BLE_HW_TIMER                1
///The advance wakeup time for anchor point: HS6220 wakeup; SPI traffic; RF setup
#define HAL_BLE_TICK_ADVANCE_WAKEUP    EXE_US_TO_BLETICK(400 +365 +8+130+150)

#elif defined(HS6621D)
///high resolution timer: 1us.
#define HAL_TIMER_HIGH_RESOLUTION       1
#define RTC_IS_32768HZ
#define SYS_TICK_HZ                     32000000
#define SYS_TICK_HALF                   0x7FFFFFFF
#ifdef CONFIG_FPGA
#define RTC_32K_HZ                      32786
#else
#define RTC_32K_HZ                      32768
#endif
#define hal_sys_tick()                  (*(volatile uint32_t *)(0xE0001004)) // DWT->CYCCNT
#define hal_sys_tick_set(tick)          *(volatile uint32_t *)(0xE0001004) = (tick)

#elif defined(__EC616)
///high resolution timer: 1us.
#define HAL_TIMER_HIGH_RESOLUTION       1
#define SYS_TICK_HZ                     204800000
#define SYS_TICK_HALF                   0x7FFFFFFF
///systick based on Host MCU's clock.
#define hal_sys_tick()                  (*(volatile uint32_t *)(0xE0001004)) // DWT->CYCCNT
#define hal_sys_tick_set(tick)          *(volatile uint32_t *)(0xE0001004) = (tick)
///RTC tick as scheduler based on low power timer.
#define RTC_32K_HZ                      32768

///BLE tick based on HS6220's timer.
#define HAL_BLE_HW_TIMER                1
///The advance wakeup time for anchor point: MCU wakeup; SPI traffic; RF setup
#define HAL_BLE_TICK_ADVANCE_WAKEUP    EXE_US_TO_BLETICK(47 +652 +8+130)

///hal_spim_transfer() API
#define HW_SPI_TRANSFER
///RTOS hooks
#define HAL_RTOS_SUPPORT                1
#define CONFIG_DEBUG_DIAG

#elif defined(ESP8266)
///high resolution timer: 1us.
#define HAL_TIMER_HIGH_RESOLUTION       1
#define SYS_TICK_HZ                     80000000
#define SYS_TICK_HALF                   0x7FFFFFFF
///systick based on Host MCU's clock.
__attribute__( ( always_inline ) ) static inline uint32_t __get_ccount(void)
{
    uint32_t result;
    asm volatile ("rsr.ccount %0" : "=r" (result) );
    return (result);
}
#define hal_sys_tick()                  __get_ccount()
#define hal_sys_tick_set(tick)
///RTC tick as scheduler based on low power timer.
#define RTC_32K_HZ                      32768

///BLE tick based on HS6220's timer.
#define HAL_BLE_HW_TIMER                1
///The advance wakeup time for anchor point: MCU wakeup; SPI traffic; RF setup
#define HAL_BLE_TICK_ADVANCE_WAKEUP    EXE_US_TO_BLETICK(47 +652 +8+130)

///hal_spim_transfer() API
#define HW_SPI_TRANSFER
///RTOS hooks
//#define HAL_RTOS_SUPPORT                1
#define CONFIG_DEBUG_DIAG

#elif defined(CI13XX)
uint32_t tim_us_ticker_read(void);
///high resolution timer: 1us.
extern volatile int32_t timer0_overtime_rtc_count;
#define HAL_TIMER_HIGH_RESOLUTION       0
#define SYS_TICK_HZ                     62500
#define SYS_TICK_HALF                   0x7FFFFFFF
#define hal_sys_tick()                  tim_us_ticker_read()
#define hal_sys_tick_set(tick)         (timer0_overtime_rtc_count = tick)
#define HAL_BLE_HW_TIMER                1

#else
///high resolution timer: 1us.
#define HAL_TIMER_HIGH_RESOLUTION       1
#define SYS_TICK_HZ                     32000000
#define SYS_TICK_HALF                   0x7FFFFFFF
#define hal_sys_tick()                  *(volatile uint32_t *)0
#define hal_sys_tick_set(tick)          *(volatile uint32_t *)0
#endif

#if !defined(HAL_BLE_HW_TIMER)
///BLE tick is based on host MCU's clock which is fine enough.
#define HAL_BLE_HW_TIMER                0
#define BLE_TICK_HZ                     SYS_TICK_HZ
#define BLE_TICK_HALF                   SYS_TICK_HALF
#define HAL_BLE_TICK_SYNC()             hal_sys_tick()
#define HAL_BLE_TICK_CURR()             hal_sys_tick()
#define EXE_TIMESTAMP_MS()              hal_sys_tick()/(SYS_TICK_HZ/1000)
#endif
#if HAL_BLE_HW_TIMER
///BLE tick is based on HS6220's timer, while host MCU's clock is coarse.
#define BLE_TICK_HZ                     32000
#define BLE_TICK_HALF                   0x7FFFFFFF
#define HAL_BLE_TICK_SYNC()             hal_rf_read_sync_tick()
#define HAL_BLE_TICK_CURR()             hal_rf_read_tim_tick()
#define EXE_TIMESTAMP_MS()              HAL_BLE_TICK_CURR() / 1000 * 3125 / 100
#endif

#if !defined(HAL_BLE_TICK_ADVANCE_WAKEUP)
///The advance wakeup time for anchor point, including MCU & HS6220 wakeup time, SPI traffic time, HS6220 RF setup time etc.
#define HAL_BLE_TICK_ADVANCE_WAKEUP    EXE_US_TO_BLETICK(800)
#endif

#if !defined(HAL_BLE_HW_WHITEN)
///RF supports hw ble whiten in default.
#define HAL_BLE_HW_WHITEN               1
#endif

#if !defined(HAL_BLE_HW_CRC)
///RF supports hw ble crc in default.
#define HAL_BLE_HW_CRC                  1
#endif

#if !defined(RTC_32K_HZ)
#define RTC_32K_HZ                      32000
#endif

#if !defined(HAL_RTOS_SUPPORT)
///nonOS version in default.
#define HAL_RTOS_SUPPORT                0
#endif

/**
 * @brief The global variable for anchor point.
 */
#define BLETICK_ON_SYNC                 *(uint32_t *)ll_gbuf_xpkt_rx
#define BLETICK_END_OF_RX               BLETICK_ON_SYNC + EXE_US_TO_BLETICK((1+4+2+ll_gbuf_xpkt_rx[5]+3)*8)


/*****************************  系统TICK相关函数  *****************************/

/**
 * @brief The translation from time to system tick at high but possible corse frequency.
 * @param [in] us,ms,sec - The time in us,ms,sec.
 * @return the system tick.
 */
#if HAL_TIMER_HIGH_RESOLUTION
/* high resolution timer: 1us. */
#define EXE_US_TO_SYSTICK(us)           ((us) * (hal_sys_freq()/1000000))
#else
/* lower resolution timer: <1us. */
#define EXE_US_TO_SYSTICK(us)           ((us)*0.0625)
#endif

#define EXE_MS_TO_SYSTICK(ms)           ((ms) * (hal_sys_freq()/1000))
#define EXE_1D25MS_TO_SYSTICK(v)        ((v) *  (hal_sys_freq()/1000) * 125 / 100)
#define EXE_SEC_TO_SYSTICK(sec)         ((sec) * hal_sys_freq())

#define EXE_SYSTICK_MS(tick)            ((tick) / (SYS_TICK_HZ/1000))

/**
 * @brief The translation from time to BLE tick at fine frequency.
 * @param [in] us,ms,sec - The time in us,ms,sec.
 * @return the BLE tick.
 */
#if defined(__C51__)
#define EXE_US_TO_BLETICK(us)           ((BLE_TICK_HZ / 1000 * (us) / 1000) + 1)
#else
#define EXE_US_TO_BLETICK(us)           ((((1ULL<<32) * BLE_TICK_HZ / 1000000 * (us)) >> 32) + 1)
#endif
#define EXE_MS_TO_BLETICK(ms)           ((ms) * BLE_TICK_HZ/1000)
#define EXE_1D25MS_TO_BLETICK(v)        ((v) *  BLE_TICK_HZ/1000 * 125 / 100)
#define EXE_SEC_TO_BLETICK(sec)         ((sec) * BLE_TICK_HZ)
#define BLETICK_TO_SYSTICK(ble_tick)    ((ble_tick) * (hal_sys_freq() / BLE_TICK_HZ))

/**
 * @brief The translation from 32K tick in low frequency to system tick at high frequency.
 * @param [in] tick_32k - The tick at 32KHz.
 * @return the system tick.
 */
#define RTC_TO_SYSTICK(tick_32k)        ((tick_32k) * (hal_sys_freq() / RTC_32K_HZ))

/**
 * @brief Get (timestamp - reference), including rollover.
 */
#define EXE_TIME_DIFF(timestamp, reference) exe_time_diff(timestamp, reference)

/**
 * @brief Get the frequency of the high frequency timer.
 *
 * @param[in] none
 * @returns the frequency of scheduler timer.
 */
__STATIC_INLINE uint32_t hal_sys_freq(void)
{
    return SYS_TICK_HZ;
}

#if !defined(hal_sys_tick_set)
/**
 * @brief Restore system tick after exit low power mode.
 *
 * @param[in] tick - The absolute system tick in high frequency.
 */
void hal_sys_tick_set(uint32_t tick);
#endif

/**
 * Checks whether the given time is older than the reference (occurs before in time).
 *
 * @verbatim
 -----------|---------------------|-----------> t
            t1                    t2
            time                  reference

 EXE_TIME_OLDER_THAN(t1, t2) => true
 EXE_TIME_OLDER_THAN(t2, t1) => false
 @endverbatim
 */
#define EXE_TIME_OLDER_THAN(time, ref) (bool)(((uint32_t) (time)) - ((uint32_t) (ref)) > SYS_TICK_HALF)
#define BLE_TIME_OLDER_THAN(time, ref) (bool)(((uint32_t) (time)) - ((uint32_t) (ref)) > BLE_TICK_HALF)

/**
 * @brief Get the absolute difference between two timestamps, regardless of order.
 *
 * @param[in] time1 First timestamp to compare
 * @param[in] time2 Second timestamp to compare
 *
 * @returns The difference between the two time parameters.
 */
#if defined(__LCC__)
extern uint32_t exe_time_diff(uint32_t time1, uint32_t time2);
#else
__STATIC_INLINE uint32_t exe_time_diff(uint32_t time1, uint32_t time2)
{
  if (time1 - time2 > SYS_TICK_HALF)
  {
    return time2 - time1;
  }
  else
  {
    return time1 - time2;
  }
}
#endif

/* Don't export this API if systick is not high resolution. */
#if HAL_TIMER_HIGH_RESOLUTION
/**
 * @brief 检查传入的参考时间点(systick)+时间跨度(us)是否已发生。
 *
 * @param[in] ref_tick  传入的参考时间点(单位systick)
 * @param[in] span_us   上述时间点再加一个时间跨度(单位us)
 *
 * @returns true已发生 false未发生
 */
#if defined(__LCC__)
extern bool exe_time_elapsed(uint32_t ref_tick, int32_t span_us);
#else
__STATIC_INLINE bool exe_time_elapsed(uint32_t ref_tick, int32_t span_us)
{
  return EXE_TIME_OLDER_THAN(ref_tick + span_us * EXE_US_TO_SYSTICK(1), hal_sys_tick());
}
#endif
#endif

/**
 * @brief 检查传入的参考时间点(systick)+时间跨度(systick)是否已发生。
 *
 * @param[in] ref_tick  传入的参考时间点(单位systick)
 * @param[in] span_tick 上述时间点再加一个时间跨度(单位systick)
 *
 * @returns true已发生 false未发生
 */
#if defined(__LCC__)
extern bool exe_time_elapsed_tick(uint32_t ref_tick, int32_t span_tick);
#else
__STATIC_INLINE bool exe_time_elapsed_tick(uint32_t ref_tick, int32_t span_tick)
{
  return EXE_TIME_OLDER_THAN(ref_tick + span_tick, hal_sys_tick());
}
#endif

/**
 * @brief 检查传入的参考时间点(bletick)+时间跨度(bletick)是否已发生。
 * @note 获得BLE tick的开销比较大，超过100us，注意使用场合和频次。
 *
 * @param[in] ref_tick  传入的参考时间点(单位bletick)
 * @param[in] span_tick 上述时间点再加一个时间跨度(单位bletick)
 *
 * @returns true已发生 false未发生
 */
#define EXE_TIME_ELAPSED_BLETICK(ref_tick, span_tick) \
  BLE_TIME_OLDER_THAN((ref_tick) + (span_tick), HAL_BLE_TICK_CURR())


/*******************************  时间相关函数  *******************************/

/**
 * @brief Clock module initialization.
 */
void hal_systime_init(void);

/**
 * @brief Poll wait at milliseconds level. 毫秒级延时。
 *
 * @param [in] ms - the delay time in milliseconds.
 *                  value range in [1, implementation limited].
 * @note 精度不用太高，可以偏大，但不能偏小
 */
void hal_delay_ms(int ms);

/**
 * @brief Poll wait at microseconds level. 微秒级延时。
 *
 * @param [in] us - the delay time in microseconds.
 *                  value range in [1, implementation limited].
 * @note 精度需要尽可能精确。
 */
void hal_delay_us(int us);

#endif /* #ifndef __EXE_HAL_TIME_H__ */

