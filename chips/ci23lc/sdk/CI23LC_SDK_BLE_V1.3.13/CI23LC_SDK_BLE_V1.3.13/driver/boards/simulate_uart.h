#ifndef __SIMULATE_UART_H__
#define __SIMULATE_UART_H__

#include "ci_core_misc.h"
#include "codec_manager.h"
#include "ci_dpmu.h"
#include "ci_gpio.h"
#include "simple_mp3_player.h"
#include "board.h"
#include "ci_assert.h"
#include "ci13lc_timer.h"

#define SIMULATE_UARTOUT_PWM_PIN_NAME PC4
#define SIMULATE_UARTOUT_GPIO_PIN_BASE PC
#define SIMULATE_UARTOUT_PWM_PIN_NUMBER pin_4
#define SIMULATE_UARTOUT_GPIO_FUNCTION SECOND_FUNCTION

void simulate_for_uart();
void simulate_uart_timer_timeout_deal(void);
void send_uart_code_continue(void);
#endif  //