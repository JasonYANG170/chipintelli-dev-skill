
#include "tuya_pwm.h"
#include "tuya_pin.h"
#include "tuya_os_adapter_error_code.h"

#include "hal/hal_adv_timer.h"
#include "hal/hal_gpio.h"
#include "utils/debug/log.h"


#define PWM_DEV_NUM             6

struct pwm_channel_pin
{
    tuya_pin_name_t pin_name;
    uint32_t        gpio_base;
    gpio_pin_t      gpio_pin;
    uint32_t        pwm_cha_base;
    afio_function_t pwm_cha;
    uint16_t        pwm_freq;
    uint8_t         polarity;
    uint8_t         pol_sta;
};

struct pwm_pol_t
{
    uint8_t         pol_sta;
    uint32_t        pwm_cha_base;
    afio_function_t pwm_cha;
    uint8_t         pwm_com_cha1;
    uint8_t         pwm_com_cha2;
}pwm_pol;


struct pwm_channel_pin ln_pwn_cha_pin[PWM_DEV_NUM] = 
{
    {TUYA_PA11,GPIOA_BASE,GPIO_PIN_11, ADV_TIMER_0_BASE,ADV_TIMER_PWM0,0,0,0},
    {TUYA_PA10,GPIOA_BASE,GPIO_PIN_10, ADV_TIMER_1_BASE,ADV_TIMER_PWM2,0,0,0},
    {TUYA_PA7, GPIOA_BASE,GPIO_PIN_7,  ADV_TIMER_2_BASE,ADV_TIMER_PWM4,0,0,0},
    {TUYA_PA5, GPIOA_BASE,GPIO_PIN_5,  ADV_TIMER_3_BASE,ADV_TIMER_PWM6,0,0,0},
    {TUYA_PB3, GPIOB_BASE,GPIO_PIN_3,  ADV_TIMER_4_BASE,ADV_TIMER_PWM8,0,0,0},
    {TUYA_PA12,GPIOA_BASE,GPIO_PIN_12, ADV_TIMER_5_BASE,ADV_TIMER_PWM10,0,0,0},
};

static int pwm_dev_init(tuya_pwm_t     *pwm, tuya_pwm_cfg_t *cfg);
static int pwm_dev_deinit(tuya_pwm_t   *pwm);
static int pwm_dev_start(tuya_pwm_t    *pwm);
static int pwm_dev_stop(tuya_pwm_t     *pwm);
static int pwm_dev_control(tuya_pwm_t  *pwm, uint8_t cmd, void *arg);

static tuya_pwm_t s_pwm_dev[PWM_DEV_NUM];
static const tuya_pwm_ops_t  s_pwm_dev_ops = {
    .init       = pwm_dev_init,
    .start      = pwm_dev_start,
    .stop       = pwm_dev_stop,
    .control    = pwm_dev_control,
    .deinit     = pwm_dev_deinit
};

int platform_pwm_init(void)
{
    int  i = 0;

    for (i = 0; i < PWM_DEV_NUM; i++) {
        s_pwm_dev[i].ops = (tuya_pwm_ops_t *)&s_pwm_dev_ops;
        tuya_driver_register(&s_pwm_dev[i].node, TUYA_DRV_PWM, i);
    }

    return OPRT_OS_ADAPTER_OK;
}


#define LN_GPIOA_NUM (16) // [0, 15]
#define LN_GPIOB_NUM (16) // [0, 15]

#define LN_GET_GPIO_BASE(port)   ((TUYA_PIN_PORT(port) != 0) ? GPIOB_BASE : GPIOA_BASE)
#define LN_GET_PIN_NUM(port)     ((gpio_pin_t)(1U << (port & 0x000F)))

// true -> in range; false -> not in range
#define LN_PIN_IS_IN_RANGE(port) (!(TUYA_PIN_PORT(port) > 1 || TUYA_PIN_NUM(port) > 15))
#define LN_PIN_PARAM_CHECK(port, error_no)      \
    do {                                        \
        if (!LN_PIN_IS_IN_RANGE(port)) {        \
            return error_no;                    \
        }                                       \
    }                                           \
    while(0)


static int pwm_dev_init(tuya_pwm_t *pwm, tuya_pwm_cfg_t *cfg)
{
    uint8_t     pwm_num = 0;
    uint32_t    pwm_base = 0;
    uint8_t     ln_result = 0;
    float       div_clk = 0;

    for(int i = 0; i < PWM_DEV_NUM; i++){
        if(ln_pwn_cha_pin[i].pin_name == cfg->pin){
            pwm_num = i;
            ln_result = 1;
        }
    }
    if(ln_result == 0)
        return OPRT_OS_ADAPTER_INVALID_PARM;
    
    if(ln_pwn_cha_pin[pwm_num].polarity != TUYA_PWM_NEGATIVE){
        pwm_base = ln_pwn_cha_pin[pwm_num].pwm_cha_base;
        hal_gpio_pin_afio_select(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,ln_pwn_cha_pin[pwm_num].pwm_cha);
        hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_ENABLE);
    }else{
        if(pwm_pol.pol_sta == 0){
            pwm_base = ln_pwn_cha_pin[pwm_num].pwm_cha_base;
            pwm_pol.pol_sta = 1;
            pwm_pol.pwm_cha_base = ln_pwn_cha_pin[pwm_num].pwm_cha_base;
            pwm_pol.pwm_cha      = ln_pwn_cha_pin[pwm_num].pwm_cha;
            pwm_pol.pwm_com_cha1 = pwm_num;
            hal_gpio_pin_afio_select(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,ln_pwn_cha_pin[pwm_num].pwm_cha);
            hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_ENABLE);
        }else if(pwm_pol.pol_sta == 1){
            pwm_pol.pol_sta = 2;
            pwm_pol.pwm_com_cha2 = pwm_num;
            pwm_base = pwm_pol.pwm_cha_base;
            hal_gpio_pin_afio_select(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,pwm_pol.pwm_cha + 1);
            hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_ENABLE);
        }
        else{
            return OPRT_OS_ADAPTER_INVALID_PARM;
        }
    }
    
    /* PWM参数初始化 */
    adv_tim_init_t_def adv_tim_init;
    memset(&adv_tim_init,0,sizeof(adv_tim_init));

    if(cfg->period_ns > 1000000)
        adv_tim_init.adv_tim_clk_div = 40 - 1;
    else
        adv_tim_init.adv_tim_clk_div = 0;

    div_clk = (25 * ((adv_tim_init.adv_tim_clk_div + 1) * 1.0f));
                                               
    adv_tim_init.adv_tim_load_value  = (uint32_t)(cfg->period_ns *1.0f / div_clk - 1);           
    adv_tim_init.adv_tim_cmp_a_value = (uint32_t)(adv_tim_init.adv_tim_load_value * cfg->percent); 
    adv_tim_init.adv_tim_cha_inv_en = 1;

    if(pwm_pol.pol_sta == 2){
        adv_tim_init.adv_tim_dead_en = 1;
        adv_tim_init.adv_tim_dead_gap_value = 1;
    }

    if(adv_tim_init.adv_tim_cmp_a_value == 0){
        gpio_init_t_def gpio_init;
        memset(&gpio_init,0,sizeof(gpio_init));       
        gpio_init.dir   = GPIO_OUTPUT;              
        gpio_init.pin   = ln_pwn_cha_pin[pwm_num].gpio_pin;               
        gpio_init.speed = GPIO_HIGH_SPEED;           
        gpio_init.mode  = GPIO_MODE_DIGITAL;           
        gpio_init.pull  = GPIO_PULL_UP;            
        hal_gpio_init(ln_pwn_cha_pin[pwm_num].gpio_base,&gpio_init);        
        if(pwm_pol.pol_sta == 2 && pwm_pol.pwm_com_cha2 == pwm_num){ 
            hal_gpio_pin_set(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin);
        }else{
            hal_gpio_pin_reset(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin);
        }
        hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_DISABLE);
    }else if(adv_tim_init.adv_tim_cmp_a_value == adv_tim_init.adv_tim_load_value){
        gpio_init_t_def gpio_init;
        memset(&gpio_init,0,sizeof(gpio_init));       
        gpio_init.dir   = GPIO_OUTPUT;              
        gpio_init.pin   = ln_pwn_cha_pin[pwm_num].gpio_pin;               
        gpio_init.speed = GPIO_HIGH_SPEED;           
        gpio_init.mode  = GPIO_MODE_DIGITAL;           
        gpio_init.pull  = GPIO_PULL_UP;            
        hal_gpio_init(ln_pwn_cha_pin[pwm_num].gpio_base,&gpio_init);         
        if(pwm_pol.pol_sta == 2 && pwm_pol.pwm_com_cha2 == pwm_num){ 
            hal_gpio_pin_reset(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin);
        }else{
            hal_gpio_pin_set(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin);
        }
        hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_DISABLE);
    }
    adv_tim_init.adv_tim_cnt_mode = ADV_TIMER_CNT_MODE_INC; 
    hal_adv_tim_init(pwm_base,&adv_tim_init);  

    LOG(LOG_LVL_INFO, "[%s, %d] pwm%d init\r\n",__func__, __LINE__, pwm_num);

    return OPRT_OS_ADAPTER_OK;
}

static int pwm_dev_deinit(tuya_pwm_t *pwm)
{
    uint8_t     pwm_num = 0;
    uint8_t     ln_result = 0;

    for(int i = 0; i < PWM_DEV_NUM; i++){
        if(ln_pwn_cha_pin[i].pin_name == pwm->cfg.pin){
            pwm_num = i;
            ln_result = 1;
        }
    }
    if(ln_result == 0)
        return OPRT_OS_ADAPTER_INVALID_PARM;
    
    if(ln_pwn_cha_pin[pwm_num].polarity == TUYA_PWM_NEGATIVE && pwm_pol.pol_sta >= 1){
        pwm_pol.pol_sta = 0;
        for(int i = 0; i < PWM_DEV_NUM; i++){
            ln_pwn_cha_pin[i].polarity = TUYA_PWM_POSITIVE;
        }
        hal_adv_tim_a_en(pwm_pol.pwm_cha_base,HAL_DISABLE);  
        hal_adv_tim_b_en(pwm_pol.pwm_cha_base,HAL_DISABLE);  
        hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_DISABLE);
    }else{
        hal_adv_tim_a_en(ln_pwn_cha_pin[pwm_num].pwm_cha_base,HAL_DISABLE);  
        hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_DISABLE);
    }
    return OPRT_OS_ADAPTER_OK;
}

static int pwm_dev_start(tuya_pwm_t *pwm)
{
    uint8_t     pwm_num = 0;
    uint8_t     ln_result = 0;

    for(int i = 0; i < PWM_DEV_NUM; i++){
        if(ln_pwn_cha_pin[i].pin_name == pwm->cfg.pin){
            pwm_num = i;
            ln_result = 1;
        }
    }
    if(ln_result == 0)
        return OPRT_OS_ADAPTER_INVALID_PARM;
    
    if(ln_pwn_cha_pin[pwm_num].polarity == TUYA_PWM_NEGATIVE && pwm_pol.pol_sta == 2){
        hal_adv_tim_a_en(pwm_pol.pwm_cha_base,HAL_ENABLE);  
        hal_adv_tim_b_en(pwm_pol.pwm_cha_base,HAL_ENABLE);  
    }else{
        hal_adv_tim_a_en(ln_pwn_cha_pin[pwm_num].pwm_cha_base,HAL_ENABLE);  
    }
    return OPRT_OS_ADAPTER_OK;
}

static int pwm_dev_stop(tuya_pwm_t *pwm)
{
    uint8_t     pwm_num = 0;
    uint8_t     ln_result = 0;

    for(int i = 0; i < PWM_DEV_NUM; i++){
        if(ln_pwn_cha_pin[i].pin_name == pwm->cfg.pin){
            pwm_num = i;
            ln_result = 1;
        }
    }
    if(ln_result == 0)
        return OPRT_OS_ADAPTER_INVALID_PARM;
    
    if(ln_pwn_cha_pin[pwm_num].polarity == TUYA_PWM_NEGATIVE && pwm_pol.pol_sta == 2){
        hal_adv_tim_a_en(pwm_pol.pwm_cha_base,HAL_DISABLE);  
        hal_adv_tim_b_en(pwm_pol.pwm_cha_base,HAL_DISABLE);  
    }else{
        hal_adv_tim_a_en(ln_pwn_cha_pin[pwm_num].pwm_cha_base,HAL_DISABLE);  
    }
    return OPRT_OS_ADAPTER_OK;
}

struct pwm_update_t
{
    struct
    {
        volatile uint8_t  update_flag;
        volatile uint32_t pwm_base;
        volatile uint16_t com_val;
    }param[6];
} pwm_update;

int pwm_dev_set_dead_time(uint8_t percent)
{
    uint16_t pwm_load_val = 0;
    uint16_t pwm_cha_val = 0;
    uint16_t dead_time = 0;
    if(pwm_pol.pol_sta != 2)
        return OPRT_OS_ADAPTER_INVALID_PARM;
    
    pwm_load_val = hal_adv_tim_get_load_value(pwm_pol.pwm_cha_base);
    pwm_cha_val = hal_adv_tim_get_comp_a(pwm_pol.pwm_cha_base);

    dead_time = (uint16_t)((1.0f * percent / 100) * pwm_load_val);

    hal_adv_tim_set_dead_gap(pwm_pol.pwm_cha_base,dead_time);

    return OPRT_OS_ADAPTER_OK;
}

static int pwm_dev_control(tuya_pwm_t *pwm, uint8_t cmd, void *arg)
{
    uint8_t     pwm_num   = 0;
    uint8_t     ln_result = 0;
    uint32_t    pwm_base  = 0;

    float       div_clk = 0;
    uint8_t     div_val = 0;
    uint16_t    pwm_cha_val_buffer = 0;

    for(int i = 0; i < PWM_DEV_NUM; i++){
        if(ln_pwn_cha_pin[i].pin_name == pwm->cfg.pin){
            pwm_num = i;
            ln_result = 1;
        }
    }
    if(ln_result == 0)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    if(ln_pwn_cha_pin[pwm_num].polarity == TUYA_PWM_NEGATIVE && pwm_pol.pol_sta == 2){
        pwm_base = pwm_pol.pwm_cha_base;
    }else{
        pwm_base = ln_pwn_cha_pin[pwm_num].pwm_cha_base ;
    }


    if(pwm->cfg.period_ns > 1000000)
        div_val = 40 - 1;
    else
        div_val = 0;

    div_clk = (25 * ((div_val + 1) * 1.0f));

    pwm_cha_val_buffer = (uint16_t)(pwm->cfg.percent * (pwm->cfg.period_ns * 1.0f / div_clk - 1));

    if((pwm_cha_val_buffer == 0) || (pwm_cha_val_buffer  ==  ((uint16_t)(pwm->cfg.period_ns * 1.0f / div_clk - 1)))){
        if(cmd == TUYA_PWM_PULSE_SET_CMD)
            cmd = TUYA_PWM_SET_CMD;
    }
    switch (cmd) {
        case TUYA_PWM_SET_CMD:
        case TUYA_PWM_PERIOD_SET_CMD:
            if((uint16_t)(pwm->cfg.percent * (pwm->cfg.period_ns * 1.0f / div_clk - 1)) == 0){
                gpio_init_t_def gpio_init;
                memset(&gpio_init,0,sizeof(gpio_init));       
                gpio_init.dir   = GPIO_OUTPUT;              
                gpio_init.pin   = ln_pwn_cha_pin[pwm_num].gpio_pin;               
                gpio_init.speed = GPIO_HIGH_SPEED;           
                gpio_init.mode  = GPIO_MODE_DIGITAL;           
                gpio_init.pull  = GPIO_PULL_UP;            
                hal_gpio_init(ln_pwn_cha_pin[pwm_num].gpio_base,&gpio_init);     
                if(pwm_pol.pol_sta == 2 && pwm_pol.pwm_com_cha2 == pwm_num){ 
                    hal_gpio_pin_set(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin);
                }else{
                    hal_gpio_pin_reset(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin);
                }    
                hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_DISABLE);
            }else if ((uint32_t)(pwm->cfg.percent * (pwm->cfg.period_ns * 1.0f / div_clk - 1)) == ((uint32_t)(pwm->cfg.period_ns * 1.0f / div_clk - 1))){
                gpio_init_t_def gpio_init;
                memset(&gpio_init,0,sizeof(gpio_init));       
                gpio_init.dir   = GPIO_OUTPUT;              
                gpio_init.pin   = ln_pwn_cha_pin[pwm_num].gpio_pin;               
                gpio_init.speed = GPIO_HIGH_SPEED;           
                gpio_init.mode  = GPIO_MODE_DIGITAL;           
                gpio_init.pull  = GPIO_PULL_UP;            
                hal_gpio_init(ln_pwn_cha_pin[pwm_num].gpio_base,&gpio_init);    
                if(pwm_pol.pol_sta == 2 && pwm_pol.pwm_com_cha2 == pwm_num){ 
                    hal_gpio_pin_reset(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin);
                }else{
                    hal_gpio_pin_set(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin);
                }  
                hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_DISABLE);
            }
            else{
                hal_adv_tim_a_en(pwm_base,HAL_DISABLE); 
                hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin,HAL_ENABLE);
            }
            hal_adv_tim_a_en(pwm_base,HAL_DISABLE); 
            hal_adv_tim_set_clock_div(pwm_base,div_val);
            hal_adv_tim_set_load_value(pwm_base,(uint32_t)(pwm->cfg.period_ns * 1.0f / div_clk - 1));
            hal_adv_tim_set_comp_a(pwm_base,pwm_cha_val_buffer);  
            hal_adv_tim_a_en(pwm_base,HAL_ENABLE);     
            break;
        case TUYA_PWM_PULSE_SET_CMD:
            
            hal_gpio_pin_afio_en(ln_pwn_cha_pin[pwm_num].gpio_base,ln_pwn_cha_pin[pwm_num].gpio_pin, HAL_ENABLE);
            //LOG(LOG_LVL_INFO, "[%s, %d] pwm%d duty = %d\r\n",__func__, __LINE__, pwm_num,pwm_cha_val_buffer);

            NVIC_DisableIRQ(ADV_TIMER_IRQn);
            pwm_update.param[pwm_num].pwm_base    = pwm_base;
            pwm_update.param[pwm_num].com_val     = pwm_cha_val_buffer;
            pwm_update.param[pwm_num].update_flag = 1;
            hal_adv_tim_clr_it_flag(pwm_base, ADV_TIMER_IT_FLAG_LOAD);
            hal_adv_tim_it_cfg(pwm_base, ADV_TIMER_IT_FLAG_LOAD, HAL_ENABLE);
            NVIC_EnableIRQ(ADV_TIMER_IRQn);
            break;
            
        case TUYA_PWM_POLARIITY_SET_CMD:      
            ln_pwn_cha_pin[pwm_num].polarity = pwm->cfg.polarity;
            break;

        default : 
            return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    return OPRT_OS_ADAPTER_OK;
}

static uint8_t int_cnt = 0;
void ADV_TIMER_IRQHandler()
{
    for(int_cnt = 0; int_cnt < 6; int_cnt++)
    {
        if(pwm_update.param[int_cnt].update_flag == 1)
        {
            if(hal_adv_tim_get_it_flag(pwm_update.param[int_cnt].pwm_base,ADV_TIMER_IT_FLAG_LOAD) == HAL_SET)
            {
                hal_adv_tim_it_cfg(pwm_update.param[int_cnt].pwm_base,ADV_TIMER_IT_FLAG_LOAD,HAL_DISABLE);
                hal_adv_tim_set_comp_a(pwm_update.param[int_cnt].pwm_base,pwm_update.param[int_cnt].com_val);  
                pwm_update.param[int_cnt].update_flag = 0;
            }
        }
    }
}
