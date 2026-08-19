/**
 * @file ci_1308x.h
 * @brief  芯片型号相关信息文件
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024  Chipintelli Technology Co., Ltd.
 *
 */

#ifndef _CI_1308X_H_
#define _CI_1308X_H_

#include "ci13lc.h"


typedef enum
{
/***-----PAD-----****************----analgo--****----1st-----****---2nd-----****----3rd----****----4th----****----5th----****----6th----****----7th----****/
    SPI0_CS_PAD          = 3,   /*               SPI0_CS         ---            ---            ---            ---            ---            ---           */
    SPI0_D1_PAD          = 4,   /*               SPI0_D1         ---            ---            ---            ---            ---            ---           */
    SPI0_D2_PAD          = 5,   /*               SPI0_D2         ---            ---            ---            ---            ---            ---           */
    PB5                  = 17,  /*               PB_5            UART0_TX       IIC0_SDA       PWM1           PWMP           ---            INTER_CLKOUT  */
    PB6                  = 18,  /*               PB_6            UART0_RX       IIC0_SCL       PWM2           PWMN           ---            ---           */
    PC0                  = 20,  /*               PC_0            ---            ---            PWM0           ---            ---            ---           */
    SPI0_D0_PAD          = 23,  /*               SPI0_D0         ---            ---            ---            ---            ---            ---           */
    SPI0_CLK_PAD         = 24,  /*               SPI0_CLK        ---            ---            ---            ---            ---            ---           */
    SPI0_D3_PAD          = 25,  /*               SPI0_D3         ---            ---            ---            ---            ---            ---           */
    PC1                  = 26,  /*               ---             PC_1           UART2_TX       PWM3           ---            ---            ---           */
}PinPad_Name;



#endif


