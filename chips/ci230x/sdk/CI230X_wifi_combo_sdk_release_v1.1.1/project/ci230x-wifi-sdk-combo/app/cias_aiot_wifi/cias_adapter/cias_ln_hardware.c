#include "cias_ln_hardware.h"
#include "base_addr_ln882h.h"

//wifi 复位audio接口-GPIOA10拉低复位
void cias_ln_audio_reset(void)
{
   hal_gpio_pin_afio_en(GPIOA_BASE, GPIO_PIN_10, HAL_ENABLE);
   gpio_init_t_def gpio_init;
   memset(&gpio_init, 0, sizeof(gpio_init)); //清零结构体
   gpio_init.dir = GPIO_OUTPUT;               //配置GPIO方向，输入
   gpio_init.pin = GPIO_PIN_10;               //配置GPIO引脚号
   gpio_init.speed = GPIO_HIGH_SPEED;        //设置GPIO速度
   hal_gpio_init(GPIOA_BASE, &gpio_init);    //初始化GPIO
   cias_task_delay_ms(10);
   //modify by houwang
   hal_gpio_pin_reset(GPIOA_BASE, GPIO_PIN_10);//拉高
   cias_task_delay_ms(10);
   hal_gpio_pin_set(GPIOA_BASE, GPIO_PIN_10);//拉低
   
}