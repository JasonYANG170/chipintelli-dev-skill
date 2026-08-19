#include "ci_key.h"
#include "ci_log.h"
#include "ci13lc_gpio.h"
#include "ci13lc_dpmu.h"
#include "system_msg_deal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/**
 * @brief 初始化按键，初始化按键中断，主要初始化ADC
 *
 */
_XIF_ void ci_key_io_init(void)
{
#if RTC_UPLINK_SWITCH_GPIO_DEFAULT
    scu_set_device_gate(RTC_UPLINK_SWITCH_GPIO_BASE, ENABLE);
    dpmu_set_io_pull(RTC_UPLINK_SWITCH_GPIO, DPMU_IO_PULL_UP); // 默认拉高
    dpmu_set_io_reuse(RTC_UPLINK_SWITCH_GPIO, FIRST_FUNCTION);
    gpio_set_input_mode(RTC_UPLINK_SWITCH_GPIO_BASE, RTC_UPLINK_SWITCH_GPIO_PIN);
    gpio_irq_trigger_config(RTC_UPLINK_SWITCH_GPIO_BASE, RTC_UPLINK_SWITCH_GPIO_PIN, both_edges_trigger);
    gpio_clear_irq_single(RTC_UPLINK_SWITCH_GPIO_BASE, RTC_UPLINK_SWITCH_GPIO_PIN);
    eclic_irq_enable(RTC_UPLINK_SWITCH_GPIO_IRQ);
#endif
#if RTC_DENOISE_FUNC_GPIO_DEFAULT
    scu_set_device_gate(RTC_DENOISE_FUNC_GPIO_BASE, ENABLE);
    dpmu_set_io_pull(RTC_DENOISE_FUNC_GPIO, DPMU_IO_PULL_UP);
    dpmu_set_io_reuse(RTC_DENOISE_FUNC_GPIO, FIRST_FUNCTION);
    gpio_set_input_mode(RTC_DENOISE_FUNC_GPIO_BASE, RTC_DENOISE_FUNC_GPIO_PIN);
    gpio_irq_trigger_config(RTC_DENOISE_FUNC_GPIO_BASE, RTC_DENOISE_FUNC_GPIO_PIN, both_edges_trigger);
    gpio_clear_irq_single(RTC_DENOISE_FUNC_GPIO_BASE, RTC_DENOISE_FUNC_GPIO_PIN);
    eclic_irq_enable(RTC_DENOISE_FUNC_GPIO_IRQ);
#endif

#if RTC_USER_KEY_FUNC_GPIO_DEFAULT
    scu_set_device_gate(RTC_USER_KEY_FUNC_GPIO_BASE, ENABLE);
    dpmu_set_io_pull(RTC_USER_KEY_FUNC_GPIO, DPMU_IO_PULL_UP);
    dpmu_set_io_reuse(RTC_USER_KEY_FUNC_GPIO, FIRST_FUNCTION);
    gpio_set_input_mode(RTC_USER_KEY_FUNC_GPIO_BASE, RTC_USER_KEY_FUNC_GPIO_PIN);
    gpio_irq_trigger_config(RTC_USER_KEY_FUNC_GPIO_BASE, RTC_USER_KEY_FUNC_GPIO_PIN, both_edges_trigger);
    gpio_clear_irq_single(RTC_USER_KEY_FUNC_GPIO_BASE, RTC_USER_KEY_FUNC_GPIO_PIN);
    eclic_irq_enable(RTC_USER_KEY_FUNC_GPIO_IRQ);
#endif

#if RTC_FLASH_MUTY_KEY_FUNC_GPIO_DEFAULT
    scu_set_device_gate(RTC_FLASH_MUTY_KEY_FUNC_GPIO_BASE, ENABLE);
    dpmu_set_io_pull(RTC_FLASH_MUTY_KEY_FUNC_GPIO, DPMU_IO_PULL_UP);
    dpmu_set_io_reuse(RTC_FLASH_MUTY_KEY_FUNC_GPIO, FIRST_FUNCTION);
    gpio_set_input_mode(RTC_FLASH_MUTY_KEY_FUNC_GPIO_BASE, RTC_FLASH_MUTY_KEY_FUNC_GPIO_PIN);
    gpio_irq_trigger_config(RTC_FLASH_MUTY_KEY_FUNC_GPIO_BASE, RTC_FLASH_MUTY_KEY_FUNC_GPIO_PIN, both_edges_trigger);
    gpio_clear_irq_single(RTC_FLASH_MUTY_KEY_FUNC_GPIO_BASE, RTC_FLASH_MUTY_KEY_FUNC_GPIO_PIN);
    eclic_irq_enable(RTC_FLASH_MUTY_KEY_FUNC_GPIO_IRQ);
#endif
}

/**
 * 测试上电时间
 *
 */
_XIF_ void system_start_time_test(void)
{
#if 1
    scu_set_device_gate((unsigned int)PC, ENABLE);        // 开启PB时钟
    dpmu_set_io_reuse(PC4, SECOND_FUNCTION);               // 设置引脚功能复用为GPIO
    dpmu_set_io_direction(PC4, DPMU_IO_DIRECTION_OUTPUT); // 设置引脚功能为输出模式
    dpmu_set_io_pull(PC4, DPMU_IO_PULL_UP);             // 设置关闭上下拉
    gpio_set_output_mode(PC, pin_4);                      // GPIO的pin脚配置成输出模式
    gpio_set_output_level_single(PC, pin_4, 1);           // 输出高电平
    // gpio_set_output_level_single(PB,pin_4,0);          //输出低电平
    mprintf("GPIO-TEST:%s   %d\n", __func__, __LINE__);
#endif
}

/**
 * @brief a sample code key infomation printf
 *
 * @param key_msg
 */
_XIF_ void key_info_show(sys_msg_key_data_t *key_msg)
{
    mprintf("HEMY-%s %d\n", __FUNCTION__, __LINE__);
    if (key_msg->key_index != KEY_NULL)
    {
        ci_loginfo(LOG_USER, "key_value is 0x%x ", key_msg->key_index);
        if (MSG_KEY_STATUS_PRESS == key_msg->key_status)
        {
            ci_loginfo(LOG_USER, "status : press down\n");
        }
        else if (MSG_KEY_STATUS_PRESS_LONG == key_msg->key_status)
        {
            ci_loginfo(LOG_USER, "status : long press\n");
        }
        else if (MSG_KEY_STATUS_RELEASE == key_msg->key_status)
        {
            ci_loginfo(LOG_USER, "status : release\n");
        }
        if (key_msg->key_status == MSG_KEY_STATUS_RELEASE)
        {
            ci_loginfo(LOG_USER, "按键时间为：%d ms\n", key_msg->key_time_ms);
        }
        /*user code*/
    }
}

void rtc_key_handle(gpio_base_t base, int pins)
{
    // mprintf("rtc_key_handle = 0x%x, %d\n", base, gpio_port_index);
    static uint8_t rtc_uplink_switch_status = 1; // 如果是下拉，默认配置为0
    static uint8_t rtc_denoise_func_status = 1;
    static uint8_t rtc_user_key_func_status = 1;

    if (base == RTC_UPLINK_SWITCH_GPIO_BASE)
    {
        if ((1 << pins) == RTC_UPLINK_SWITCH_GPIO_PIN)
        {
#if RTC_UPLINK_SWITCH_GPIO_DEFAULT
            int status = gpio_get_input_level_single(RTC_UPLINK_SWITCH_GPIO_BASE, RTC_UPLINK_SWITCH_GPIO_PIN);
            if (rtc_uplink_switch_status != status)
            {
                if (0 == status) // 获取IO状态
                {
                    // INFO:输入为低电平
                    mprintf("左声道:  %s %d\n", __FUNCTION__, __LINE__);
                    set_src_from_right_mic(false);
                }
                else
                {
                    // INFO:输入为高电平
                    mprintf("右声道: %s %d\n", __FUNCTION__, __LINE__);
                    set_src_from_right_mic(true);
                }
                rtc_uplink_switch_status = status;
            }
#endif
        }
    }
    if (base == RTC_DENOISE_FUNC_GPIO_BASE)
    {
        if ((1 << pins) == RTC_DENOISE_FUNC_GPIO_PIN)
        {
#if RTC_DENOISE_FUNC_GPIO_DEFAULT
            int status = gpio_get_input_level_single(RTC_DENOISE_FUNC_GPIO_BASE, RTC_DENOISE_FUNC_GPIO_PIN);
            if (status != rtc_denoise_func_status)
            {
                if (0 == status) // 获取IO状态
                {
                    // INFO:输入为低电平
                    mprintf("降噪关:  %s %d\n", __FUNCTION__, __LINE__);
                    convert_left_and_right(true);
                }
                else
                {
                    // INFO:输入为高电平
                    mprintf("降噪开: %s %d\n", __FUNCTION__, __LINE__);
                    convert_left_and_right(false);
                }
                rtc_denoise_func_status = status;
            }
           
#endif
        }
    }
    if (base == RTC_USER_KEY_FUNC_GPIO_BASE)
    {
        if ((1 << pins) == RTC_USER_KEY_FUNC_GPIO_PIN)
        {
#if RTC_USER_KEY_FUNC_GPIO_DEFAULT
            int status = gpio_get_input_level_single(RTC_USER_KEY_FUNC_GPIO_BASE, RTC_USER_KEY_FUNC_GPIO_PIN);
            if (status != rtc_user_key_func_status)
            {
                if (0 == status) // 获取IO状态
                {
                    // INFO:输入为低电平
                    mprintf("-按键按下...:  %s %d\n", __FUNCTION__, __LINE__);
                }
                else
                {
                    // INFO:输入为高电平
                    mprintf("按键松开: %s %d\n", __FUNCTION__, __LINE__);
                }
                rtc_user_key_func_status = status;
            }
#endif
        }
    }
    if (base == RTC_FLASH_MUTY_KEY_FUNC_GPIO_BASE)
    {
        if ((1 << pins) == RTC_FLASH_MUTY_KEY_FUNC_GPIO_PIN)
        {
#if RTC_FLASH_MUTY_KEY_FUNC_GPIO_DEFAULT
            extern uint8_t rtc_flash_record_key_func_status;
            int status = gpio_get_input_level_single(RTC_FLASH_MUTY_KEY_FUNC_GPIO_BASE, RTC_FLASH_MUTY_KEY_FUNC_GPIO_PIN);
            if (status != rtc_flash_record_key_func_status)
            {
                if (0 == status) // 获取IO状态
                {
                    // INFO:输入为低电平
                    // mprintf("按键按下:  %s %d\n", __FUNCTION__, __LINE__);
                }
                else
                {
                    // INFO:输入为高电平
                    // mprintf("按键松开: %s %d\n", __FUNCTION__, __LINE__);
                }
                rtc_flash_record_key_func_status = status;
            }
#endif
        }
    }
}
void ci_key_init(void)
{
    /*io 按键初始化*/
    ci_key_io_init();
}
