/*============================================================================*/
/* @file exe_hal.h
 * @brief EXE Hardware Abstract Layer header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_H__
#define __EXE_HAL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "exe_hal_tc.h"
#include "exe_hal_cpu.h"
#include "exe_hal_time.h"
#include "exe_hal_os.h"
#include "exe_hal_cfg.h"
#include "exe_hal_rf.h"
#include "exe_hal_pm.h"

#if defined(__MM32_MINIBOARD )
#include "HAL_gpio.h"
#define HAL_PIN_LED_CONN_PORT           GPIOB
#define HAL_PIN_LED_CONN                GPIO_Pin_3 //PB3
#define HAL_PIN_LED_DANCE_PORT          GPIOB
#define HAL_PIN_LED_DANCE               GPIO_Pin_4 //PB4
#define HAL_PIN_BTN_PORT                GPIOB
#define HAL_PIN_BTN_0                   GPIO_Pin_11
#elif defined(STM32F10X_MD)
  #if defined(__ASM)
  #undef __ASM 
  #endif
#include "stm32f10x_gpio.h"
#define HAL_PIN_LED_CONN_PORT           GPIOB
#define HAL_PIN_LED_CONN                GPIO_Pin_2 
#define HAL_PIN_LED_DANCE_PORT          GPIOB
#define HAL_PIN_LED_DANCE               GPIO_Pin_1 
#define HAL_PIN_BTN_0                   GPIO_Pin_3
#elif defined(HM1001_M0)
#if defined(__ASM)
#undef __ASM
#endif
#include "HM1001_gpio.h"
#define HAL_PIN_LED_CONN_PORT           GPIOB
#define HAL_PIN_LED_CONN                GPIO_PIN11
#define HAL_PIN_LED_DANCE_PORT          GPIOB
#define HAL_PIN_LED_DANCE               GPIO_PIN12
#define HAL_PIN_BTN_0                   GPIO_PIN13
#elif defined(HS6621D)
#ifdef CONFIG_FPGA
#define HAL_PIN_BTN_0                   4
#define HAL_PIN_UART_TX                 5
#define HAL_PIN_LED_CONN_PORT           NULL
#define HAL_PIN_LED_DANCE               14
#define HAL_PIN_LED_DANCE_PORT          NULL
#define HAL_PIN_LED_CONN                15
#define HAL_PIN_LED_0                   16
#define HAL_PIN_LED_1                   17
#define HAL_PIN_LED_2                   18
#define HAL_PIN_LED_3                   18
#else
#define HAL_PIN_BTN_0                   17
#define HAL_PIN_UART_TX                 5
#define HAL_PIN_LED_CONN_PORT           NULL
#define HAL_PIN_LED_DANCE               8
#define HAL_PIN_LED_DANCE_PORT          NULL
#define HAL_PIN_LED_CONN                9
#define HAL_PIN_LED_0                   10
#define HAL_PIN_LED_1                   11
#define HAL_PIN_LED_2                   12
#define HAL_PIN_LED_3                   13
#endif
#endif

/**
 * @brief The LED Id enumeration.
 */
typedef enum {
    EXE_LED_ID_CONN  = 0,
    EXE_LED_ID_DANCE,
    EXE_LED_ID_0     = 0,
    EXE_LED_ID_1,
    EXE_LED_ID_2,
} exe_led_id_t;

/**
 * @brief The LED status enumeration.
 */
typedef enum {
    EXE_LED_ON  = 0,
    EXE_LED_OFF,
    EXE_LED_TOGGLE,

    EXE_LED_LEVEL_LOW      = 0,
    EXE_LED_LEVEL_HIGH     = 1,

    EXE_LED_PULSE_HIGH_1US    = 0x11,
    EXE_LED_PULSE_LOW_1US     = 0x21,
    EXE_LED_PULSE_TOGGLE_1US  = 0x31,
} exe_led_status_t;

/**
 * @brief The Button function bitmap, range [0,15]
 * @ref   The return value of bsp_button_get_status(), the bitmap of buttons.
 */
#define EXE_BTN_PRESSED_LEFT            (1u << 0)
#define EXE_BTN_PRESSED_RIGHT           (1u << 1)
#define EXE_BTN_PRESSED_MIDDLE          (1u << 2)
#define EXE_BTN_PRESSED_PAIR            (1u << 15)


/**
 * @brief Diagnostic indication via GPIO.
 */
#if defined(CONFIG_DEBUG_DIAG) && !defined(HAL_PIN_LED_CONN)
#define HAL_DIAG_TXD_SCAN_RSP()         bsp_led_set(EXE_LED_ID_1, EXE_LED_PULSE_HIGH_1US)
#define HAL_DIAG_RXD()                  //bsp_led_set(EXE_LED_ID_0, EXE_LED_PULSE_HIGH_1US)
#define HAL_DIAG_TXD()                  bsp_led_set(EXE_LED_ID_1, EXE_LED_PULSE_HIGH_1US)

#include <assert.h>
#define HAL_ASSERT(cond)                do { if (!cond) bsp_led_set(EXE_LED_ID_1, EXE_LED_PULSE_TOGGLE_1US); assert(cond); } while (0)

#else
#define HAL_DIAG_TXD_SCAN_RSP()
#define HAL_DIAG_RXD()
#define HAL_DIAG_TXD()
#define HAL_ASSERT(cond)

#endif

/**
 * @brief Statistics buffer for realtime trace.
 */
#if defined(CONFIG_DEBUG_STATS)
#define HAL_STATS_ADD(event_data)       hal_stats_add(event_data)
#else
#define HAL_STATS_ADD(event_data)
#endif

typedef struct {
  ///register address: offset for mmap regs, address for analog regs, *256 for sleep.
  uint16_t off;
  ///register value or us for sleep.
  uint8_t  dat;
  ///3-mmap; 8-analog; 7-sleep
  uint8_t  cmd  :6;
  ///0 means the end of reg table.
  uint8_t  flag :2;
} hal_reg_tbl_t;

/**
 * @brief The direction type of digital GPIO.
 */
#define GPIO_DIR_OUTPUT  0
#define GPIO_DIR_INPUT   1

/**
 * @brief The operation type of NVM (Flash/OTP).
 */
#define PORT_NVM_OP_READ          0x03
#define PORT_NVM_OP_PROGRAM       0x02
#define PORT_NVM_OP_ERASE         0x20


/******************************  数字IO相关函数  ******************************/

/**
 * @brief Digital GPIO module initialization for LEDs and buttons etc.
 */
void hal_gpio_init(void);

/**
 * @brief Set the direction of the specific GPIO.
 *
 * @param [in] port - GPIO port/group.
 *        [in] pin  - Pin number in the GPIO port.
 *        [in] dir  - 0=GPIO_DIR_OUTPUT for output, 
                      1=GPIO_DIR_INPUT for input.
 */
void hal_gpio_set_direction(void* port, int pin, uint8_t dir);

/**
 * @brief Write value to the specific GPIO.
 *
 * @param [in] port  - GPIO port/group.
 *        [in] pin   - Pin number in the GPIO port.
 *        [in] value - The value level to be written.
 */
void hal_gpio_write(void* port, int pin, uint8_t val);

/**
 * @brief Read status of the specific GPIO.
 *
 * @param [in] port  - GPIO port/group.
 *        [in] pin   - Pin number in the GPIO port.
 * @return true for high level.
 *         false for low level.
 */
bool hal_gpio_read(void* port, int pin);

/**
 * @brief Change(Set/Clear/Toggle) level to the specific GPIO.
 *
 * @param [in] port  - GPIO port/group.
 *        [in] pin   - Pin number in the GPIO port.
 */
void hal_gpio_set_bit(void* port, int pin);
void hal_gpio_clr_bit(void* port, int pin);
void hal_gpio_toggle_bit(void* port, int pin);

/**
 * @brief Get status of the pressed buttons on board, for shutter.
 *        获取自拍杆按键状态。
 *
 * @return the bitmap of buttons, @ref EXE_BTN_xxx
 *         1 for pressed, 0 for released.
 * @note The bitmap is consistent with the HID Usages of Consume Control.
 */
uint16_t bsp_button_get_status(void);

/**
 * @brief Get wheel data for mouse.
 * @note  It is called by app_mouse_scan() every ~8ms.
 *
 * @return the wheel data: +clockwise, -counterclockwise
 * @note   The wheel data is consistent with the HID Usages of Consume Control.
 */
int8_t bsp_wheel_get_data(void);

/**
 * @brief Set the LED on board to specific status.
 * @note If this API is implemented, don't define HAL_PIN_LED_XXX above.
 *
 * @param [in] id - LED id.
 * @param [in] st - LED status.
 */
void bsp_led_set(exe_led_id_t id, exe_led_status_t st);

/**
 * @brief Get status of HS6220 transceiver interrupt request line.
 *        获取HS6220 IRQ PIN状态。
 *
 * @return true/high for inactive, false/low for active.
 */
bool hal_get_irq_status(void);

/********************************  SPI相关函数  *******************************/

/**
 * @brief SPI master module initialization for HS6220 transceiver.
 *        初始化SPI主。
 * @note SPI配置为模式0/MSB, 要求有效速率高于2Mbps。
 */
void hal_spi_init(void);

/**
 * @brief The API of access HS6220 transceiver via SPI master.
 *
 * @param [in]     cmd     - Command in one byte to be sent.
 *        [in]     tx_data - Argument/data in tx_len byte to be sent.
 *        [in]     tx_len -  The length to be sent.
 *        [in/out] rx_data - Data in rx_len byte to be received.
 *        [in]     rx_len  - The length to be received.
 */
void hal_spim_transfer(uint8_t cmd,
		       const uint8_t *tx_data, uint8_t tx_len,
		       uint8_t *rx_data, uint8_t rx_len);

/**
 * @brief The internal operations within the SPI transfer.
 * @note Obsoloted.
 */
void hal_ce_low(void);
void hal_ce_high(void);
void hal_csn_high(void);
void hal_csn_low(void);
uint8_t hal_rf_spi_wrd(uint8_t byte);
#if defined(HW_SPI_TRANSFER)
void hal_spim_transfer(uint8_t cmd, const uint8_t *tx_data, uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len);
#endif

/******************************  通用ADC相关函数  *****************************/

/**
 * @brief GPADC module initialization for battery and adkey etc.
 *        初始化系统ADC， 用于采集电池电量。
 */
void hal_adc_init(void);

/**
 * @brief Get the battery voltage.
 *
 * @return the battery in percentage. [0, 100]
 */
uint8_t hal_adc_get_battery(void);


/******************************  随机数相关函数  ******************************/

/**
 * @brief Get a random for BLE SMP protocol.
 *
 * @return a random in 4-byte.
 */
uint32_t hal_rng_get_word(void);


/***************************  非易失性存储相关函数  ***************************/

/**
 * @brief NVM(Flash/OTP) module initialization for config and MAC.
 * @note If the API is executed in flash to erase/program flash,
 *       please implement it in RAM function.
 *
 * @param [in] addr  - The address to be operated(erase/program/read).
 *        [in] len   - The length.
 *        [in] buf   - The pointer to memory buffer for program/read.
 *        [in] op    - Operation type.
 */
void hal_flash_op(uint32_t addr, uint32_t len, uint8_t *buf, uint8_t op);
void hal_pm_wakeup();
#endif /* #ifndef __EXE_HAL_H__ */

