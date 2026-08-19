
#ifndef __BAUDRATE_CALIBRATE_H__
#define __BAUDRATE_CALIBRATE_H__


#include "sdk_default_config.h"
#include "ci_system.h"



// 执行一次波特率校准
void baudrate_calibrate(UART_TypeDef *UARTx);

#endif /* UART_BAUDRATE_CALIBRATE */

