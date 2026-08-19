/**
 * @file exe_hal_dummy.c
 * @brief template HAL layer in EXE stack.
 * @date
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
#include "ble_ll.h"
#include "exe_int_ll.h"
#include "exe_hal.h"
#include "exe_pkt.h"
#include "exe_api.h"
#include "exe_hal_time.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*

*/
#include "FreeRTOS.h"
#include "stdint.h"
#include "stdlib.h"
#include "ci_ble_rf.h"
#include "ci130x_gpio.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "ci130x_dpmu.h"
#include "ci130x_core_misc.h"

#define SPI_OUT              0
#define RF_SPI_MISO_PIN HS6220_MISO_PIN
#define RF_SPI_MOSI_PIN HS6220_MOSI_PIN
#define RF_SPI_SCK_PIN HS6220_SCK_PIN
#define RF_SPI_CS_PIN HS6220_CSN_PIN
#define RF_CE_PIN pin_5


void hal_gpio_init(void)
{
	scu_set_device_gate((uint32_t)PA, ENABLE);
	dpmu_set_io_reuse(PA2, FIRST_FUNCTION);	// 设置引脚功能复用为GPIO
	dpmu_set_io_reuse(PA3, FIRST_FUNCTION);	// 设置引脚功能复用为GPIO
	dpmu_set_io_reuse(PA4, FIRST_FUNCTION); // 设置引脚功能复用为GPIO
    dpmu_set_io_reuse(PC3, SECOND_FUNCTION); // 设置引脚功能复用为GPIO

	dpmu_set_io_direction(PA2, DPMU_IO_DIRECTION_OUTPUT);  // 设置引脚功能为输出模式
	dpmu_set_io_direction(PA3, DPMU_IO_DIRECTION_OUTPUT);  // 设置引脚功能为输出模式
	dpmu_set_io_direction(PA4, DPMU_IO_DIRECTION_OUTPUT); // 设置引脚功能为输出模式
    dpmu_set_io_direction(PC3, DPMU_IO_DIRECTION_OUTPUT); // 设置引脚功能为输出模式

	dpmu_set_io_pull(PA2, DPMU_IO_PULL_DISABLE);	 // 设置关闭上下拉
	dpmu_set_io_pull(PA3, DPMU_IO_PULL_DISABLE);	 // 设置关闭上下拉
	dpmu_set_io_pull(PA4, DPMU_IO_PULL_DISABLE); // 设置关闭上下拉
    dpmu_set_io_pull(PC3, DPMU_IO_PULL_DISABLE); // 设置关闭上下拉

	gpio_set_output_mode(PA, pin_2); // GPIO的pin脚配置成输出模式
	gpio_set_output_mode(PA, pin_3); // GPIO的pin脚配置成输出模式
	gpio_set_output_mode(PA, pin_4);
    gpio_set_output_mode(PC, pin_3);
}

/* GPIO相关操作 */

/* GPIO相关操作 */
bool hal_gpio_read(void *port, int pin)
{
    return gpio_get_input_level_single(PB, (0x1 << pin));
}
void hal_gpio_set_bit(void *port, int pin)
{
    gpio_set_output_level_single(PB, (0x1 << pin), 1);
}
void hal_gpio_clr_bit(void *port, int pin)
{
    gpio_set_output_level_single(PB, (0x1 << pin), 0);
}
void hal_gpio_toggle_bit(void *port, int pin)
{
    // 翻转gpio
}
void hal_gpio_set_direction(void *port, int pin, uint8_t dir)
{
    if (dir) // input
    {
        gpio_set_input_mode(PB, (0x1 << pin));
    }
    else
    {
        gpio_set_output_mode(PB, (0x1 << pin));
    }
}
void hal_gpio_write(void *port, int pin, uint8_t val)
{
    gpio_set_output_level_single(PB, (0x1 << pin), val);
}
/* @return the bitmap of the pressed buttons
           which is consistent with the HID Usages of Consume Control. */
uint16_t bsp_button_get_status(void)
{
    /*获取自拍杆按键状态 */
    return *(volatile uint16_t *)0;
}

void hal_adc_init(void)
{
    /*初始化系统ADC， 用于采集电池电量*/
}

void hal_flash_op(uint32_t addr, uint32_t len, uint8_t *buf, uint8_t op)
{
    /*操作MCU的存储器*/
}

/***********************************************************************************
3.3	HAL PM
   MCU和HS6220休眠和低功耗相关管理
***********************************************************************************/

SemaphoreHandle_t pmSemaphore = NULL;
bool pm_sem_wait = false;
/**
 * @brief 初始化蓝牙协议栈运行任务信号量，控制蓝牙任务的切换
 */
void hal_pm_init(void)
{
    if (pmSemaphore == NULL)
    {
        pmSemaphore = xSemaphoreCreateBinary();
    }
}

void hal_pm_reset(void)
{
    /*MCU软复位， 用于OTA */
}

/**
 * @brief 蓝牙协议栈获取到信号量，进入运行态
 */
void hal_pm_wakeup()
{
    if (pm_sem_wait)
    {
        xSemaphoreGiveFromISR(pmSemaphore, pdFALSE);
        //mprintf("wakeup hal_pm_sleep\r\n"); 
    }   
}

extern bool status_connected;
/**
 * @brief 蓝牙协议释放信号量，进入休眠，系统开始运行其他的语音，IOT等任务
 */
void hal_pm_sleep(uint8_t sleep_mode)
{
    uint8_t ret = 0;
    bool recv_irq = false;
    exe_wakeup_src = 0;
    if ((exe_stk_state == EXE_LINK_STATE_ADV) || status_connected)
    {
        pm_sem_wait = true;
        ret = xSemaphoreTake(pmSemaphore, portMAX_DELAY);
        pm_sem_wait = false;
    } 
    
    /*中断里面唤醒hs6220， 从唤醒6220到可以操作SPI需要延时400us */
    hal_rf_clear_rtc_timer_interrupt();
    hal_delay_us(400 - 3);
}


void hal_clk_save_reg(void)
{
    /*休眠前保存一些唤醒后丢失的数据， 唤醒后手动恢复 */
}
void hal_clk_restore_reg(void)
{
    /*唤醒后恢复休眠前保存的相关数据 */
}

uint32_t hal_clk_get_tick_32k(void)
{
    /*返回当前系统32K计数值 */
    return hal_rf_read_tim_tick();
}

extern volatile int32_t timer0_overtime_rtc_count;
uint32_t tim_us_ticker_read()
{
     uint32_t counter, counter2;
    // A situation might appear when Master (OC) overflows right after Slave (Update) is read and before the
    // new (overflowed) value of Master is read. Which would make the code below consider the
    // previous (incorrect) value of Slave and the new value of Master, which would return a
    // value in the past. Avoid this by computing consecutive values of the timer until they
    // are properly ordered.
    counter = timer0_overtime_rtc_count;
    while (1)
    {
        counter2 = timer0_overtime_rtc_count;
        if (counter2 > counter)
        {
            break;
        }
        counter = counter2;
    }
    return counter2;
}
uint32_t hal_clk_set_alarm_32k(uint32_t tick, uint32_t cal_cnt)
{
#if HAL_BLE_HW_TIMER // bleTimer
    // NVIC_EnableIRQ(GPIOB_IRQn);
    rf_clear_all_irq();
    hal_rf_set_tim_tick(tick - HAL_BLE_TICK_ADVANCE_WAKEUP);

#else
    uint32_t hs6220_tick = hal_rf_read_tim_tick();

    hal_rf_set_tim_tick(systick_delta * 16 /*一个tick 16us*/ * 100 / 3125 /*HS6220一个tick 31.25us*/ + hs6220_tick - 30 /*醒来到MCU时钟稳定190us固定延时*/ - (400 * 100 / 3125 + 1) /*HS6220醒来到可以操作spi需延时400us*/);
    if (!hal_get_irq_status())
        rf_clear_all_irq();
#endif

    return hal_sys_tick();
}
void hal_pm_save_context(uint8_t sleep_mode)
{
    /*有些MCU休眠前保护上下文现场 */
}
void hal_pm_restore_context(void)
{
    /* 唤醒后恢复上下文现场 */
}
uint8_t hal_pm_get_wakeup_status(void)
{
    /*获取唤醒状态 */
    return PM_WAKEUP_TIMER;
}
void hal_pm_clear_wakeup_status(void)
{
    /*清除唤醒状态 */
}
void hal_pm_set_wakeup_source(uint8_t wakeup_src)
{
    /*设置唤醒源 */
}

/***********************************************************************************
3.2	HAL HS6220
   对于HS6220相关操作的函数已经封装好可以直接调用，用户需要做的工作是实现和MCU相关四个函数。
void hal_spi_init(void)用于初始化控制HS6220的SPI接口。SPI配置为模式0/MSB, 要求有效速率高于2Mbps。
void hal_csn_high(void) 用于拉高CSN。
void hal_csn_low(void)  用于拉低CSN。
uint8_t hal_rf_spi_wrd(uint8_t txDat)用于完成1Byte数据读写。
***********************************************************************************/
bool hal_get_irq_status(void)
{
    /* 获取HS6220 IRQ PIN状态 */

    return gpio_get_input_level_single(HS6220_IRQ_PORT, HS6220_IRQ_PIN);
}

void hal_spi_init(void)
{
    HS6220_SPI_Init();
    /*初始化SPI接口*/
#if SPI_OUT
    hal_gpio_init();
#endif 
}

void hal_csn_high(void)
{
    /*SPI CSN拉低 */
    HS6220_CSN_HIGH;
    portEXIT_CRITICAL(); //开全局中断
    
#if SPI_OUT
    gpio_set_output_level_single(PA,pin_2,1);
#endif

}

void hal_csn_low(void)
{
    /*SPI CSN拉高 */
    HS6220_CSN_LOW;
    portENTER_CRITICAL();  //关全局中断
#if SPI_OUT
    gpio_set_output_level_single(PA,pin_2,0);
#endif
}

uint8_t hal_rf_spi_wrd(uint8_t txDat)
{
    /*SPI 读写1Byte数据 */
    return spi_4wire_wrd(txDat);
}

/*
   系统时间相关函数
*/
void hal_systime_init(void)
{
    /*系统时钟初始化， 用于协议栈调度*/
}
void hal_delay_ms(int ms)
{
    /*毫秒级延时，精度不用太高，可以偏大，但不能偏小 */
    // mprintf("hal_delay_ms %d\r\n",ms);
    uint32_t now, start = timer0_overtime_rtc_count;
    uint32_t delta, delay_cycles = EXE_MS_TO_SYSTICK(ms);
    do
    {
        now = timer0_overtime_rtc_count;
        delta = ((now - start) > 0xFFFFFFFF / 2) ? start - now : now - start;
    } while (delta < delay_cycles);
}

void hal_delay_us(int us)
{
    /*微秒级延时，精度不用太高，可以偏大，但不能偏小 */
    // mprintf("hal_delay_ms %d\r\n",us);
    uint32_t now, start = timer0_overtime_rtc_count;
    uint32_t delta, delay_cycles = EXE_US_TO_SYSTICK(us);
    do
    {
        now = timer0_overtime_rtc_count;
        delta = ((now - start) > 0xFFFFFFFF / 2) ? start - now : now - start;
    } while (delta < delay_cycles);
}
void bsp_led_set(exe_led_id_t id, exe_led_status_t st)
{
}

uint32_t hal_rng_get_word(void)
{
    return rand();
}

/** @} */
