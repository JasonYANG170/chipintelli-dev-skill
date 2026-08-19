/**
 * @file ci_2316x.h
 * @brief  芯片型号相关信息文件
 * @version 0.1
 * @date 2024-08-05
 *
 * @copyright Copyright (c) 2024  Chipintelli Technology Co., Ltd.
 *
 */

#ifndef _CI_2316X_H_
#define _CI_2316X_H_

#include "ci13lc.h"


typedef enum
{
/***-----PAD-----****************----analgo--****----1st-----****---2nd-----****----3rd----****----4th----****----5th----****----6th----****----7th----****/
    PA0                  = 0,   /*  OSC_IN       PA_0            PWM2           ---            ---            ---            ---            ---           */
    SPI0_CS_PAD          = 3,   /*               SPI0_CS         ---            ---            ---            ---            ---            ---           */
    SPI0_D1_PAD          = 4,   /*               SPI0_D1         ---            ---            ---            ---            ---            ---           */
    SPI0_D2_PAD          = 5,   /*               SPI0_D2         ---            ---            ---            ---            ---            ---           */
    PA2                  = 6,   /*               PA_2            IIS0_SDI       IIC0_SDA       UART1_TX       PWM0           PWMP           ---           */
    PA3                  = 7,   /*               PA_3            IIS0_LRCLK     IIC0_SCL       UART1_RX       PWM1           PWMN           ---           */
    PA4                  = 8,   /*               PA_4            IIS0_SDO       ---            ---            PWM2           PWMP           ---           */
    PA6                  = 10,  /*               PA_6            IIS0_MCLK      ---            UART2_RX       PWM0           INTER_CLKOUT   ---           */
    PA7                  = 11,  /*               PA_7            PWM0           UART1_TX       EXT_INT0       ---            ---            ---           */
    PB0                  = 12,  /*               PB_0            PWM1           UART1_RX       EXT_INT1       ---            ---            ---           */
    PB5                  = 17,  /*               PB_5            UART0_TX       IIC0_SDA       PWM1           PWMP           ---            INTER_CLKOUT  */
    PB6                  = 18,  /*               PB_6            UART0_RX       IIC0_SCL       PWM2           PWMN           ---            ---           */
    SPI0_D0_PAD          = 23,  /*               SPI0_D0         ---            ---            ---            ---            ---            ---           */
    SPI0_CLK_PAD         = 24,  /*               SPI0_CLK        ---            ---            ---            ---            ---            ---           */
    SPI0_D3_PAD          = 25,  /*               SPI0_D3         ---            ---            ---            ---            ---            ---           */
    PC4                  = 29,  /*               ---             PC_4           IIC0_SCL       PWM0           ---            ---            ---           */
    PC5                  = 30,  /*               PC_5            ---            ---            ---            ---            ---            ---           */
}PinPad_Name;



#endif

