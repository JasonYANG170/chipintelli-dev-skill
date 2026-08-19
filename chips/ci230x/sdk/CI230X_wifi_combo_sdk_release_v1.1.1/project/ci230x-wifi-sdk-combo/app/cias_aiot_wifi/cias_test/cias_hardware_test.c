#include "cias_hardware_test.h"
#include "hal/hal_gpio.h"
void cias_ln_hardware_init(void)
{
   CIAS_LOG_HL("cias_ln_hardware_init is exec....\r\n");
   //PF3->GPIOB3 设置为输入模式
   CIAS_LOG_HL("set PF3 as input mode....\r\n");
   hal_gpio_pin_afio_en(GPIOB_BASE, GPIO_PIN_3, HAL_ENABLE);

   gpio_init_t_def gpio_init;
   memset(&gpio_init, 0, sizeof(gpio_init)); //清零结构体
   gpio_init.dir = GPIO_INPUT;               //配置GPIO方向，输入
   gpio_init.pin = GPIO_PIN_3;               //配置GPIO引脚号
   gpio_init.speed = GPIO_HIGH_SPEED;        //设置GPIO速度
   hal_gpio_init(GPIOB_BASE, &gpio_init);    //初始化GPIO
}
