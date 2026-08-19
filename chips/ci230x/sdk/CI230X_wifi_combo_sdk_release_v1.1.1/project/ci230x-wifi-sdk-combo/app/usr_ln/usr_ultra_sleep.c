#include "hal/hal_ext.h"
#include "hal/hal_gpio.h"
#include "utils/power_mgmt/ln_pm.h"
#include "tuya_iot_config.h"
#include "usr_ultra_sleep.h"

#ifdef ULTRA_LOW_POWER_SLEEP

void EXT_IRQHandler()
{
    if(hal_ext_get_it_flag(EXTIO_INT_SENSE_IT_FLAG) == HAL_SET)
    {
        hal_ext_clr_it_flag(EXTIO_INT_SENSE_IT_FLAG);
    }
}

static void extio_wakeup_init(void)
{
    hal_gpio_pin_pull_set(EXTIO_WAKEUP_IO_PORT, EXTIO_WAKEUP_IO_PIN, EXTIO_WAKEUP_IO_PULL);   
    
    hal_ext_init(EXTIO_INT_SENSE, EXTIO_INT_TRIG_TYPE, HAL_ENABLE);   
    
    NVIC_SetPriority(EXT_IRQn, EXTIO_INT_PRIO);                               
    NVIC_EnableIRQ(EXT_IRQn);  
}

/*
 * 仅仅给用户提供一个使用ultra sleep的方法，用户可以自行决定何时进入ultra sleep
 * 进入ultra sleep之前，用户需要配置好唤醒源，目前支持的唤醒源为RTC和EXT_INT_IO
 * 具体EXT_INT_IO可详见hal_ext.h里涉及的IO
 * ultra sleep唤醒后，将复位重启系统
 * 如需演示ultra sleep功能，打开此宏ULTRA_LOW_POWER_SLEEP即可
*/
void ultra_lowpower_enter(void)
{
    //用户所用的IO结合外部电路重新配置，使功耗最低
    //io_sleep_config();
#if (TY_RTC == 1)
    tuya_rtc_deinit();
#endif
    extio_wakeup_init();
    ln_pm_sleep_frozen();
}

#endif