#ifndef _GPIO_PRINTF_H_
#define _GPIO_PRINTF_H_

#include "ci_gpio.h"

extern void asic_init(void);
extern void asic_info(unsigned int pr_info);

extern void gpio_print_core1_init(void);
extern void gpio_print_core2_init(void);
extern void gpio_print_core1(unsigned int info);
extern void gpio_print_core2(unsigned int info);
extern void gpio_print_init(void);
extern void gpio_print(unsigned int info);
extern void gpio_print_init_s(gpio_base_t Px);
extern void gpio_print_s(gpio_base_t Px, unsigned int info);

#endif
