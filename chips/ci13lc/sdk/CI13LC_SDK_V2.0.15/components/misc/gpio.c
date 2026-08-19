#include "gpio.h"
#include "ci_gpio.h"
#include "ci_scu.h"

extern uint32_t get_core_id();


#define GPIO_TC_BASE    0x40020000
#define GPIO_INFO_BASE  0x40021000
#define GPIO_ASIC_BASE  0x40031000



void asic_init(void)
{
    scu_set_device_gate(HAL_PA_BASE,ENABLE);
    /*init gpio output 0*/
    *(volatile unsigned int *)(GPIO_TC_BASE + 0x420) = 0x0;
    *(volatile unsigned int *)(GPIO_TC_BASE + 0x600) = 0x0;
    *(volatile unsigned int *)(GPIO_TC_BASE + 0x400) = 0xff;
    *(volatile unsigned int *)(GPIO_TC_BASE + (0xff << 2)) = 0;
}

void asic_info(unsigned int pr_info)
{
    *(volatile unsigned int *)(GPIO_TC_BASE + (0xff << 2)) = 0;
    *(volatile unsigned int *)(GPIO_TC_BASE + (0xff << 2)) = pr_info;
}

void asic_else_init(void)
{
    scu_set_device_gate(HAL_PB_BASE,ENABLE);
    /*init gpio output 0*/
    *(volatile unsigned int *)(GPIO_INFO_BASE + 0x420) = 0x0;
    *(volatile unsigned int *)(GPIO_INFO_BASE + 0x600) = 0x0;
    *(volatile unsigned int *)(GPIO_INFO_BASE + 0x400) = 0xff;
    *(volatile unsigned int *)(GPIO_INFO_BASE + (0xff << 2)) = 0;
}

void asic_else_info(unsigned int pr_info)
{
    *(volatile unsigned int *)(GPIO_INFO_BASE + (0xff << 2)) = 0;
    *(volatile unsigned int *)(GPIO_INFO_BASE + (0xff << 2)) = pr_info;
}


void asic_gpio2_init(void)
{
    scu_set_device_gate(HAL_PC_BASE,ENABLE);
    dpmu_set_io_reuse(PC1,SECOND_FUNCTION);
    dpmu_set_io_reuse(PC2,SECOND_FUNCTION);
    dpmu_set_io_reuse(PC3,SECOND_FUNCTION);
    dpmu_set_io_reuse(PC4,SECOND_FUNCTION);
    /*init gpio output 0*/
    *(volatile unsigned int *)(GPIO_ASIC_BASE + 0x420) = 0x0;
    *(volatile unsigned int *)(GPIO_ASIC_BASE + 0x600) = 0x0;
    *(volatile unsigned int *)(GPIO_ASIC_BASE + 0x400) = 0xff;
    *(volatile unsigned int *)(GPIO_ASIC_BASE + (0xff << 2)) = 0;
}

void asic_gpio2_info(unsigned int pr_info)
{
    *(volatile unsigned int *)(GPIO_ASIC_BASE + (0xff << 2)) = 0;
    *(volatile unsigned int *)(GPIO_ASIC_BASE + (0xff << 2)) = pr_info;
}


void gpio_print_core1_init(void)
{
    asic_init();
}

void gpio_print_core2_init(void)
{
    asic_else_init();
}

void gpio_print_core1(unsigned int info)
{
    asic_info(info);
}

void gpio_print_core2(unsigned int info)
{
    asic_else_info(info);
}

void gpio_print_init(void)
{
    if (get_core_id() == 0)
    {
        gpio_print_core1_init();
    }
    else if (get_core_id() == 1)
    {
        gpio_print_core2_init();
    }
}

void gpio_print_init_s(gpio_base_t Px)
{
    *(volatile unsigned int *)(Px + 0x420) = 0x0;
    *(volatile unsigned int *)(Px + 0x600) = 0x0;
    *(volatile unsigned int *)(Px + 0x400) = 0xff;
    *(volatile unsigned int *)(Px + (0xff << 2)) = 0;
}

void gpio_print(unsigned int info)
{
    if (get_core_id() == 0)
    {
        gpio_print_core1(info);
    }
    else if (get_core_id() == 1)
    {
        gpio_print_core2(info);
    }
}

void gpio_print_s(gpio_base_t Px, unsigned int info)
{
    *(volatile unsigned int *)(Px + (0xff << 2)) = 0;
    *(volatile unsigned int *)(Px + (0xff << 2)) = info;
}

