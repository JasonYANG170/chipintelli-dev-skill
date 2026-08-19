#include "simulate_uart.h"
#if SIMULATE_UART_ENABLE
uint32_t simulate_send_len = 0;
char simulate_bit_buf[(SIMULATE_UART_MAX_SIZE - 1)*10 + 9] = {0};
uint32_t simulate_index = 0;
/**
 * @brief timer中断处理函数
 *
 */
void simulate_uart_timer_timeout_deal(void)
{
    timer_clear_irq(TIMER0);
    send_uart_code_continue();
}

/**
 * @brief ir硬件初始化
 *
 * @retval RETURN_OK 初始化成功
 * @retval RETURN_ERR 初始化失败
 */
int32_t simulate_uart_hw_init(void)
{
    // pwm_init_t pwm_38k_init;
    timer_init_t timer0_init;
    timer_init_t timer2_init;
    timer_init_t timer1_init;

    // 配置TIMER0的中断
    __eclic_irq_set_vector(TIMER0_IRQn, (int)simulate_uart_timer_timeout_deal);
    eclic_irq_enable(TIMER0_IRQn);
    scu_set_device_gate(TIMER0, ENABLE);
    timer0_init.mode = timer_count_mode_single; /*one shot mode*/
    timer0_init.div = timer_clk_div_0;          /*1us = 50/2 period*/
    timer0_init.width = timer_iqr_width_f;
    timer0_init.count = (get_apb_clk() / 1000000) * 104;
    timer_init(TIMER0, timer0_init);

    return RETURN_OK;
}

/**
 * @brief 管脚输出低电平
 *
 */
void simulate_uart_pwm_out_pad_enable(void)
{
    // mprintf("hihg...\r\n");
    gpio_set_output_level_single(SIMULATE_UARTOUT_GPIO_PIN_BASE, SIMULATE_UARTOUT_PWM_PIN_NUMBER, 1);
}
/**
 * @brief 管脚输出低电平
 *
 */
void simulate_uart_pwm_out_pad_disable(void)
{
    // mprintf("low...\r\n");
    gpio_set_output_level_single(SIMULATE_UARTOUT_GPIO_PIN_BASE, SIMULATE_UARTOUT_PWM_PIN_NUMBER, 0);
}
/**
 * @brief 数据发送底半部
 *
 */
void send_uart_code_continue(void)
{
    bool send_end_flag = false;
    timer_stop(TIMER0);
    timer_start(TIMER0);

    if (simulate_index < simulate_send_len + 1)
    {
        if (simulate_bit_buf[simulate_index] == 1)
        {
            // 拉高
            simulate_uart_pwm_out_pad_enable();
        }
        else if (simulate_bit_buf[simulate_index] == 0)
        {
            // 拉低
            simulate_uart_pwm_out_pad_disable();
        }
        simulate_index++;
    }
    else
    {
        timer_stop(TIMER0);
        simulate_index = 0;
        return;
    }
}

// 模拟串口波特率固定为9600
void simulate_for_uart()
{
    //mprintf("初始化模拟串口\n");
    simulate_uart_hw_init();
    scu_set_device_gate(PC, ENABLE);
    dpmu_set_io_direction(PC4, DPMU_IO_DIRECTION_OUTPUT);
    dpmu_set_io_reuse(PC4, SECOND_FUNCTION); /*gpio function*/
    gpio_set_output_mode(PC, pin_4);
    eclic_irq_enable(PC_IRQn);
}
int32_t simulate_uart_send(char *buf, uint8_t lenght)
{
  //  mprintf("simulate_uart_send is called...\r\n");
    // 转换成1/0的数据
    uint8_t send_s = 0;
    for (uint8_t i = 0; i < lenght; i++) // 数据
    {
        simulate_bit_buf[i * 10] = 0; // 起始位为0
        for (uint8_t j = 0; j < 8; j++)
        {
            simulate_bit_buf[i * 10 + j + 1] = (buf[i] >> j) & 0x01;
        }
        simulate_bit_buf[i * 10 + 9] = 1; // 停止位为1
        send_s++;
    }
    simulate_send_len = (send_s - 1) * 10 + 9;
    mprintf("simulate uart send len = %d\n", simulate_send_len);
    send_uart_code_continue();
}
#endif