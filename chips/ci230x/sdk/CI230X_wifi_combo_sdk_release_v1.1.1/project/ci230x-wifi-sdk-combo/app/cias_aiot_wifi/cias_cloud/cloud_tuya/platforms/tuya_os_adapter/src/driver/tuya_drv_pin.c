 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/


/*============================ INCLUDES ======================================*/
#include "tuya_pin.h"
#include "tuya_os_adapter.h"
#include "tuya_os_adapter_error_code.h"

#include "hal/hal_gpio.h"
#include "utils/debug/log.h"

#include "osal/osal.h"

#define LN_PIN_DEBUG (1)

#define LN_GET_GPIO_BASE(port)  pin_get_base(port)
#define LN_GET_PIN_NUM(port)    pin_get_num(port)

#define PIN_AMOUNT_MAX 26

// true -> in range; false -> not in range
#define LN_PIN_IS_IN_RANGE(port, pin)   (((port == GPIOA_BASE) || (port == GPIOB_BASE)) && \
                                        ((pin <= GPIO_PIN_15) && (pin != 0)))
#define LN_PIN_PARAM_CHECK(port, pin, error_no)      \
    do {                                             \
        if (!LN_PIN_IS_IN_RANGE(port, pin)) {        \
            return error_no;                         \
        }                                            \
    } while(0)

typedef struct {
    tuya_pin_name_t    pin;
    tuya_pin_irq_cb    cb;
    uint8_t            used_flag;
    void              *args;
} gpio_irq_info_t;

/*============================ PROTOTYPES ====================================*/
static int pin_dev_init(tuya_pin_name_t pin, tuya_pin_mode_t mode);
static int pin_dev_write(tuya_pin_name_t pin, tuya_pin_level_t level);
static int pin_dev_read(tuya_pin_name_t pin);
static int pin_dev_toggle(tuya_pin_name_t pin);
static int pin_dev_control(tuya_pin_name_t pin, uint8_t cmd, void *arg);

/*============================ LOCAL VARIABLES ===============================*/
static gpio_irq_info_t g_gpio_irq_info[PIN_AMOUNT_MAX];
static uint8_t         gpio_init_num  = 0;

struct tuya_ln_gpio_conv {
    tuya_pin_name_t tuya_pin;
    uint32_t base;
    gpio_pin_t ln_pin;
};

static const struct tuya_ln_gpio_conv io_map[] = {
    {1,     GPIOA_BASE,     GPIO_PIN_0},
    {2,     GPIOA_BASE,     GPIO_PIN_1},
    {5,     GPIOA_BASE,     GPIO_PIN_4},
    {6,     GPIOA_BASE,     GPIO_PIN_5},
    {7,     GPIOA_BASE,     GPIO_PIN_6},
    {8,     GPIOA_BASE,     GPIO_PIN_7},
    {10,    GPIOA_BASE,     GPIO_PIN_10},
    {11,    GPIOA_BASE,     GPIO_PIN_11},
    {12,    GPIOA_BASE,     GPIO_PIN_12},
    {13,    GPIOB_BASE,     GPIO_PIN_3},
    {14,    GPIOB_BASE,     GPIO_PIN_4},
    {15,    GPIOB_BASE,     GPIO_PIN_5},
    {16,    GPIOB_BASE,     GPIO_PIN_6},
    {17,    GPIOB_BASE,     GPIO_PIN_7},
    {19,    GPIOB_BASE,     GPIO_PIN_9}
};

static uint32_t pin_get_base(tuya_pin_name_t pin)
{
    for (int i = 0; i < sizeof(io_map) / sizeof(io_map[0]); i++) {
        if (pin == io_map[i].tuya_pin)
            return io_map[i].base;
    }
    return 0;
}

static gpio_pin_t pin_get_num(tuya_pin_name_t pin)
{
    for (int i = 0; i < sizeof(io_map) / sizeof(io_map[0]); i++) {
        if (pin == io_map[i].tuya_pin)
            return io_map[i].ln_pin;
    }
    return 0;
}

static int pin_dev_init(tuya_pin_name_t pin, tuya_pin_mode_t mode)
{
    uint32_t    ln_base;
    gpio_pin_t  ln_pin;
    gpio_init_t_def ln_gpio_init;
    memset(&ln_gpio_init, 0, sizeof(ln_gpio_init));

    ln_base = LN_GET_GPIO_BASE(pin);
    ln_pin =  LN_GET_PIN_NUM(pin);

    LN_PIN_PARAM_CHECK(ln_base, ln_pin, OPRT_OS_ADAPTER_INVALID_PARM);

#if LN_PIN_DEBUG
    LOG(LOG_LVL_INFO, "[%s, %d] pin=%d, mode=%d, ln_base:%08x, ln_pin:%08x\r\n",
            __func__, __LINE__, pin, mode, ln_base, ln_pin);
#endif // LN_PIN_DEBUG

    ln_gpio_init.pin   = ln_pin;
    ln_gpio_init.speed = GPIO_HIGH_SPEED;

    //! set pin direction
    switch (mode & TUYA_PIN_INOUT_MASK)  {
        case TUYA_PIN_IN:
        case TUYA_PIN_IN_FL:
        case TUYA_PIN_IN_IRQ:
            ln_gpio_init.dir = GPIO_INPUT;
            break;
        case TUYA_PIN_OUT_PP:
        case TUYA_PIN_OUT_OD:
            ln_gpio_init.dir = GPIO_OUTPUT;
            break;
        default:
            return OPRT_OS_ADAPTER_NOT_SUPPORTED;
    }

    //! set pin mdoe
    switch (mode & TUYA_PIN_MODE_MASK) {
        case TUYA_PIN_PULL_UP:
            ln_gpio_init.pull  = GPIO_PULL_UP;
            break;
        case TUYA_PIN_PULL_DOWN:
            ln_gpio_init.pull  = GPIO_PULL_DOWN;
            break;
        case TUYA_PIN_PULL_NONE:
            ln_gpio_init.pull  = GPIO_PULL_NONE;
            break;
    }

    hal_gpio_init(ln_base, &ln_gpio_init);

    //! set pin init level
    switch (mode & TUYA_PIN_INIT_MASK) {
        case TUYA_PIN_INIT_LOW:
            hal_gpio_pin_reset(ln_base, ln_pin);
            break;

        case TUYA_PIN_INIT_HIGH:
            hal_gpio_pin_set(ln_base, ln_pin);
            break;
    }

    return OPRT_OS_ADAPTER_OK;
}

static int pin_dev_write(tuya_pin_name_t pin, tuya_pin_level_t level)
{
    uint32_t    ln_base;
    gpio_pin_t  ln_pin;

    ln_base = LN_GET_GPIO_BASE(pin);
    ln_pin =  LN_GET_PIN_NUM(pin);

    LN_PIN_PARAM_CHECK(ln_base, ln_pin, OPRT_OS_ADAPTER_INVALID_PARM);

#if LN_PIN_DEBUG
    LOG(LOG_LVL_INFO, "[%s, %d] pin=%d, level=%d, ln_base:%08x, ln_pin:%08x\r\n",
            __func__, __LINE__, pin, level, ln_base, ln_pin);
#endif // LN_PIN_DEBUG

    if (TUYA_PIN_LOW == level)
    {
        hal_gpio_pin_reset(ln_base, ln_pin);
    }
    else
    {
        hal_gpio_pin_set(ln_base, ln_pin);
    }

    return OPRT_OS_ADAPTER_OK;
}

static int pin_dev_read(tuya_pin_name_t pin)
{
    uint32_t    ln_base;
    gpio_pin_t  ln_pin;

    ln_base = LN_GET_GPIO_BASE(pin);
    ln_pin =  LN_GET_PIN_NUM(pin);

    LN_PIN_PARAM_CHECK(ln_base, ln_pin, OPRT_OS_ADAPTER_INVALID_PARM);

#if LN_PIN_DEBUG
    LOG(LOG_LVL_INFO, "[%s, %d] pin=%d, ln_base:%08x, ln_pin:%08x\r\n",
            __func__, __LINE__, pin, ln_base, ln_pin);
#endif // LN_PIN_DEBUG


    return (HAL_RESET == hal_gpio_pin_read(ln_base, ln_pin) ? 0 : 1);
}

static int pin_dev_toggle(tuya_pin_name_t pin)
{
    uint32_t    ln_base;
    gpio_pin_t  ln_pin;

    ln_base = LN_GET_GPIO_BASE(pin);
    ln_pin =  LN_GET_PIN_NUM(pin);

    LN_PIN_PARAM_CHECK(ln_base, ln_pin, OPRT_OS_ADAPTER_INVALID_PARM);

    hal_gpio_pin_toggle(ln_base, ln_pin);
    return OPRT_OS_ADAPTER_OK;
}



static int pin_dev_irq_config(tuya_pin_name_t pin, tuya_pin_irq_t *irq)
{
    uint32_t    ln_base;
    gpio_pin_t  ln_pin;
    gpio_int_type_t  type;

    ln_base = LN_GET_GPIO_BASE(pin);
    ln_pin =  LN_GET_PIN_NUM(pin);

    LN_PIN_PARAM_CHECK(ln_base, ln_pin, OPRT_OS_ADAPTER_INVALID_PARM);

    if (irq == NULL)
    {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (TUYA_PIN_IN_IRQ != (irq->mode & TUYA_PIN_INOUT_MASK)) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    switch (irq->mode & TUYA_PIN_IRQ_MASK) {
    case TUYA_PIN_IRQ_RISE:
        type = GPIO_INT_RISING;
        break;
    case TUYA_PIN_IRQ_FALL:
        type = GPIO_INT_FALLING;
        break;
    case TUYA_PIN_IRQ_RISE_FALL:
        type = GPIO_INT_RISING_FALLING;
        break;
    default:
        return OPRT_OS_ADAPTER_NOT_SUPPORTED;
    }

    for (int i = 0; i < gpio_init_num; i++)
    {
        //Because there is no deinit, the number of pins(gpio_init_num) must be initialized
        if(g_gpio_irq_info[i].pin == pin /*&& g_gpio_irq_info[i].used_flag == 1*/){
            g_gpio_irq_info[i].cb   = irq->cb;
            g_gpio_irq_info[i].args = irq->arg;
            hal_gpio_pin_it_cfg(ln_base, ln_pin, type);
            hal_gpio_pin_it_en(ln_base, ln_pin, HAL_ENABLE);

            //LOG(LOG_LVL_INFO, "repeat int %d %x %x  num = %d\r\n",pin,  irq->cb,irq->arg,gpio_init_num);

            if (ln_base == GPIOB_BASE)
                NVIC_EnableIRQ(GPIOB_IRQn);
            else if (ln_base == GPIOA_BASE)
                NVIC_EnableIRQ(GPIOA_IRQn);
            return OPRT_OS_ADAPTER_OK;
        }
    }

    for(int i = 0; i < PIN_AMOUNT_MAX; i++){
        if(g_gpio_irq_info[i].used_flag == 0){
            g_gpio_irq_info[i].used_flag = 1;
            gpio_init_num ++;

            //LOG(LOG_LVL_INFO, "init %d %x %x  num = %d\r\n",pin, irq->cb,irq->arg,gpio_init_num);

            g_gpio_irq_info[i].pin  = pin;
            g_gpio_irq_info[i].cb   = irq->cb;
            g_gpio_irq_info[i].args = irq->arg;
            hal_gpio_pin_it_cfg(ln_base, ln_pin, type);
            hal_gpio_pin_it_en(ln_base, ln_pin, HAL_ENABLE);
            if (ln_base == GPIOB_BASE)
                NVIC_EnableIRQ(GPIOB_IRQn);
            else if (ln_base == GPIOA_BASE)
                NVIC_EnableIRQ(GPIOA_IRQn);
            return OPRT_OS_ADAPTER_OK;
        }

    }
    return OPRT_OS_ADAPTER_COM_ERROR;
}

static int pin_dev_control(tuya_pin_name_t pin, uint8_t cmd, void *arg)
{
    int result = 0;
    uint32_t    ln_base;
    gpio_pin_t  ln_pin;

    ln_base = LN_GET_GPIO_BASE(pin);
    ln_pin =  LN_GET_PIN_NUM(pin);

    LN_PIN_PARAM_CHECK(ln_base, ln_pin, OPRT_OS_ADAPTER_INVALID_PARM);

    switch (cmd) {
    case TUYA_DRV_SET_ISR_CMD:
        //LOG(LOG_LVL_INFO, "[%s, %d]TUYA_DRV_SET_ISR_CMD\r\n", __func__, __LINE__);
        result = pin_dev_irq_config(pin, (tuya_pin_irq_t *)arg);
        break;

    case TUYA_DRV_SET_INT_CMD:
        //LOG(LOG_LVL_INFO, "[%s, %d]TUYA_DRV_SET_INT_CMD\r\n", __func__, __LINE__);
        hal_gpio_pin_it_en(ln_base, ln_pin, HAL_ENABLE);
        break;

    case TUYA_DRV_CLR_INT_CMD:
        //LOG(LOG_LVL_INFO, "[%s, %d]TUYA_DRV_CLR_INT_CMD\r\n", __func__, __LINE__);
        hal_gpio_pin_it_en(ln_base, ln_pin, HAL_DISABLE);
        break;
    default:
        break;
    }
    return result;
}

static void GPIO_IRQHandler(void)
{
    uint32_t    ln_base;
    gpio_pin_t  ln_pin;

    for (int i = 0; i < gpio_init_num; i++)
    {
        ln_base = LN_GET_GPIO_BASE(g_gpio_irq_info[i].pin);
        ln_pin =  LN_GET_PIN_NUM(g_gpio_irq_info[i].pin);

        LN_PIN_PARAM_CHECK(ln_base, ln_pin, OPRT_OS_ADAPTER_INVALID_PARM);

        // 获取 GPIO 中断状态寄存器的值，以判断是哪一个 GPIO 发生了中断
        if (HAL_SET == hal_gpio_pin_get_it_flag(ln_base, ln_pin))
        {
            hal_gpio_pin_clr_it_flag(ln_base, ln_pin);
            if(g_gpio_irq_info[i].cb){
                g_gpio_irq_info[i].cb(g_gpio_irq_info[i].args);
            }
        }
    }
}

void GPIOA_IRQHandler(void)
{
    GPIO_IRQHandler();
}

void GPIOB_IRQHandler(void)
{
    GPIO_IRQHandler();
}

//! ro data
static const  tuya_pin_ops_t s_pin_dev_ops = {
    .init     = pin_dev_init,
    .write    = pin_dev_write,
    .read     = pin_dev_read,
    .toggle   = pin_dev_toggle,
    .control  = pin_dev_control,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
int platform_pin_init(void)
{
    return tuya_pin_register(&s_pin_dev_ops);
}
