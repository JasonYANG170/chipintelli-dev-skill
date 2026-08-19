
#include "tuya_timer.h"
#include "tuya_os_adapter_error_code.h"

#include "hal/hal_timer.h"
#include "hal/hal_common.h"
#include "ln882h.h"
#include "utils/debug/log.h"
#include "utils/debug/ln_assert.h"


#define TIMER_DEV_NUM           3


static int timer_dev_init     (tuya_timer_t  *timer, tuya_timer_cfg_t *cfg);
static int timer_dev_start    (tuya_timer_t  *timer, uint32_t us);
static int timer_dev_stop     (tuya_timer_t  *timer);
static int timer_dev_control  (tuya_timer_t  *timer, uint8_t cmd, void *arg);
static int timer_dev_deinit   (tuya_timer_t  *timer);

/**
 * i = 0 --> TUYA_TIMER0
 * i = 1 --> TUYA_TIMER1
 * i = 2 --> TUYA_TIMER2
 * i = 3 --> TUYA_TIMER3 used by wifi
*/
static tuya_timer_t s_timer_dev[TIMER_DEV_NUM] = {0};


static const tuya_timer_ops_t  timer_dev_ops = {
    .init       = timer_dev_init,
    .start      = timer_dev_start,
    .stop       = timer_dev_stop,
    .control    = timer_dev_control,
    .deinit     = timer_dev_deinit,
};


void TIMER0_IRQHandler(void)
{
    tuya_timer_t *timer = &s_timer_dev[0];
    if(hal_tim_get_it_flag(TIMER0_BASE, TIM_IT_FLAG_ACTIVE))
    {
        hal_tim_clr_it_flag(TIMER0_BASE, TIM_IT_FLAG_ACTIVE);
        if(timer != NULL) {
            if (timer->cfg.cb != NULL) {
                timer->cfg.cb(timer->cfg.arg);
            }

            if (timer->cfg.mode == TUYA_TIMER_MODE_ONCE) {
                timer_dev_stop(timer);
            }
        }
    }
}

void TIMER1_IRQHandler(void)
{
    tuya_timer_t *timer = &s_timer_dev[1];
    if(hal_tim_get_it_flag(TIMER1_BASE, TIM_IT_FLAG_ACTIVE))
    {
        hal_tim_clr_it_flag(TIMER1_BASE, TIM_IT_FLAG_ACTIVE);
        if(timer != NULL) {
            if (timer->cfg.cb != NULL) {
                timer->cfg.cb(timer->cfg.arg);
            }

            if (timer->cfg.mode == TUYA_TIMER_MODE_ONCE) {
                timer_dev_stop(timer);
            }
        }
    }
}

void TIMER2_IRQHandler(void)
{
    tuya_timer_t *timer = &s_timer_dev[2];
    if(hal_tim_get_it_flag(TIMER2_BASE, TIM_IT_FLAG_ACTIVE))
    {
        hal_tim_clr_it_flag(TIMER2_BASE, TIM_IT_FLAG_ACTIVE);
        if(timer != NULL) {
            if (timer->cfg.cb != NULL) {
                timer->cfg.cb(timer->cfg.arg);
            }

            if (timer->cfg.mode == TUYA_TIMER_MODE_ONCE) {
                timer_dev_stop(timer);
            }
        }
    }
}

/**
 * used by wifi
*/
// void TIMER3_IRQHandler(void)
// {
//     tuya_timer_t *timer = &s_timer_dev[3];
//     if(hal_tim_get_it_flag(TIMER3_BASE, TIM_IT_FLAG_ACTIVE))
//     {
//         hal_tim_clr_it_flag(TIMER3_BASE, TIM_IT_FLAG_ACTIVE);
//         if(timer != NULL) {
//             if (timer->cfg.cb != NULL) {
//                 timer->cfg.cb(timer->cfg.arg);
//             }

//             if (timer->cfg.mode == TUYA_TIMER_MODE_ONCE) {
//                 timer_dev_stop(timer);
//             }
//         }
//     }
// }

int platform_timer_init(void)
{
    int  i = 0;

    /**
     * i = 0 --> TUYA_TIMER0
     * i = 1 --> TUYA_TIMER1
     * i = 2 --> TUYA_TIMER2
     * i = 3 --> TUYA_TIMER3 used by wifi
    */
    for (i = 0; i < TIMER_DEV_NUM; i++) {
        s_timer_dev[i].ops = (tuya_timer_ops_t *)&timer_dev_ops;
        tuya_driver_register(&s_timer_dev[i].node, TUYA_DRV_TIMER, i);
    }

    return OPRT_OS_ADAPTER_OK;
}

static int timer_dev_init(tuya_timer_t *timer, tuya_timer_cfg_t *cfg)
{
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief timer_dev_start
 *      24bit timer
 *      timeout = (((us + TIMER_PER_CNT_TIME - 1) / TIMER_PER_CNT_TIME) * 1000000 / (40000000 / (tim_div + 1))) us
 *      Minimum time: TIMER_PER_CNT_TIME
 *      Maximum time: (16777215 * TIMER_PER_CNT_TIME) us
 * @param timer
 * @param us
 *      Maximum value: (TIMER_MAX_LOAD_VALUE * TIMER_PER_CNT_TIME + 1 - TIMER_PER_CNT_TIME)
*/
static int timer_dev_start(tuya_timer_t *timer, uint32_t us)
{
#define TIMER_CLOCK          (40)         // 40Mhz
#define TIMER_MAX_LOAD_VALUE (0xFFFFFFUL) // 16777215
#define TIMER_PER_CNT_TIME   (1U)         // uint: us, 1us
#define MAX_DIV              (0xFFU)      // uint8_t type

#if (TIMER_PER_CNT_TIME * TIMER_CLOCK > MAX_DIV)
#error "Maximum exceeded!"
#endif

    uint32_t timer_base = 0;
    uint32_t load_value = (us + TIMER_PER_CNT_TIME - 1) / TIMER_PER_CNT_TIME;
    IRQn_Type irqn      = TIMER0_IRQn;
    tim_init_t_def tim_init;

    LN_ASSERT(timer != NULL);
    if ((load_value > TIMER_MAX_LOAD_VALUE) || (us == 0)) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    switch(timer->node.port)
    {
        case TUYA_TIMER0:
            timer_base = TIMER0_BASE;
            irqn       = TIMER0_IRQn;
            break;
        case TUYA_TIMER1:
            timer_base = TIMER1_BASE;
            irqn       = TIMER1_IRQn;
            break;
        case TUYA_TIMER2:
            timer_base = TIMER2_BASE;
            irqn       = TIMER2_IRQn;
            break;
        default:
            return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    memset(&tim_init, 0, sizeof(tim_init));

    tim_init.tim_load_value = load_value;                             // set timer timeout value.
    tim_init.tim_mode = TIM_USER_DEF_CNT_MODE;                        // please set mode as TIM_USER_DEF_CNT_MODE if using tim_load_value
    tim_init.tim_div  = TIMER_PER_CNT_TIME * TIMER_CLOCK - 1;

    hal_tim_init(timer_base, &tim_init);                              // initialize timer
    hal_tim_en(timer_base, HAL_ENABLE);                               // enable timer
    hal_tim_it_cfg(timer_base, TIM_IT_FLAG_ACTIVE, HAL_ENABLE);       // config timer interrupt
    NVIC_EnableIRQ(irqn);                                             // enable timer interrupt

    return OPRT_OS_ADAPTER_OK;
}

static int timer_dev_stop(tuya_timer_t  *timer)
{
    uint32_t timer_base = 0;
    IRQn_Type irqn      = TIMER0_IRQn;

    switch(timer->node.port)
    {
        case TUYA_TIMER0:
            timer_base = TIMER0_BASE;
            irqn       = TIMER0_IRQn;
            break;
        case TUYA_TIMER1:
            timer_base = TIMER1_BASE;
            irqn       = TIMER1_IRQn;
            break;
        case TUYA_TIMER2:
            timer_base = TIMER2_BASE;
            irqn       = TIMER2_IRQn;
            break;
        default:
            return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    NVIC_DisableIRQ(irqn);                                             // enable timer interrupt
    hal_tim_it_cfg(timer_base, TIM_IT_FLAG_ACTIVE, HAL_DISABLE);       // config timer interrupt
    hal_tim_clr_it_flag(timer_base, TIM_IT_FLAG_ACTIVE);               // clear timer interrupt flags
    hal_tim_en(timer_base, HAL_DISABLE);                               // disable timer

    return OPRT_OS_ADAPTER_OK;
}

static int timer_dev_control(tuya_timer_t  *timer, uint8_t cmd, VOID *arg)
{
    uint32_t timer_base = 0;

    switch(timer->node.port)
    {
        case TUYA_TIMER0:
            timer_base = TIMER0_BASE;
            break;
        case TUYA_TIMER1:
            timer_base = TIMER1_BASE;
            break;
        case TUYA_TIMER2:
            timer_base = TIMER2_BASE;
            break;
        default:
            return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (cmd == TUYA_TIMER_GET_COUNT_CMD) {
        uint32_t timer_value = 0;
        timer_value = hal_tim_get_current_cnt_value(timer_base);
        if (arg != NULL) {
            *(uint32_t *)arg = timer_value;
        }
    } else {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    return OPRT_OS_ADAPTER_OK;
}

static int timer_dev_deinit(tuya_timer_t  *timer)
{
    uint32_t timer_base = 0;
    IRQn_Type irqn      = TIMER0_IRQn;

    switch(timer->node.port)
    {
        case TUYA_TIMER0:
            timer_base = TIMER0_BASE;
            irqn       = TIMER0_IRQn;
            break;
        case TUYA_TIMER1:
            timer_base = TIMER1_BASE;
            irqn       = TIMER1_IRQn;
            break;
        case TUYA_TIMER2:
            timer_base = TIMER2_BASE;
            irqn       = TIMER2_IRQn;
            break;
        default:
            return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    NVIC_DisableIRQ(irqn);                                             // enable timer interrupt
    hal_tim_it_cfg(timer_base, TIM_IT_FLAG_ACTIVE, HAL_DISABLE);       // config timer interrupt
    hal_tim_clr_it_flag(timer_base, TIM_IT_FLAG_ACTIVE);               // clear timer interrupt flags
    hal_tim_en(timer_base, HAL_DISABLE);                               // disable timer

    return OPRT_OS_ADAPTER_OK;
}
