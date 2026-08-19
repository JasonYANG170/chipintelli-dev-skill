
#include "tuya_adc.h"
#include "tuya_pin.h"
#include "tuya_os_adapter_error_code.h"

#include "hal/hal_adc.h"
#include "hal/hal_gpio.h"
#include "utils/debug/log.h"



/*============================ MACROS ========================================*/
#define ADC_DEV_NUM         2

/*============================ TYPES =========================================*/

/*============================ PROTOTYPES ====================================*/
static int adc_dev_init    (tuya_adc_t *adc, tuya_adc_cfg_t *cfg);
static int adc_dev_convert (tuya_adc_t *adc, uint16_t *result);
static int adc_dev_control (tuya_adc_t *adc, uint8_t cmd, VOID *arg);
static int adc_dev_deinit  (tuya_adc_t *adc);

/*============================ LOCAL VARIABLES ===============================*/

static tuya_adc_t s_adc_dev[ADC_DEV_NUM];

static struct adc_cha_pin
{
    tuya_pin_name_t tuya_pin;
    gpio_pin_t      pin;
    uint32_t        gpio_base;
    adc_ch_t        adc_cha;
    uint8_t         adc_init_sta;
};

struct adc_cha_pin ln_adc_cha_pin[2] = {
    {TUYA_PA0,    GPIO_PIN_0,    GPIOA_BASE,    ADC_CH2,    0},
    {TUYA_PB4,    GPIO_PIN_4,    GPIOB_BASE,    ADC_CH6,    0},
};

static const tuya_adc_ops_t  adc_dev_ops = {
    .init       = adc_dev_init,
    .convert    = adc_dev_convert,
    .control    = adc_dev_control,
    .deinit     = adc_dev_deinit,
};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
int platform_adc_init(void)
{
    int  i = 0;

    for (i = 0; i < ADC_DEV_NUM; i++) {
        s_adc_dev[i].ops = (tuya_adc_ops_t *)&adc_dev_ops;
        tuya_driver_register(&s_adc_dev[i].node, TUYA_DRV_ADC, i);
    }
    return OPRT_OS_ADAPTER_OK;
}

static int adc_dev_init(tuya_adc_t *adc, tuya_adc_cfg_t *cfg)
{
    uint32_t   ln_base = 0;
    gpio_pin_t ln_pin = GPIO_PIN_0;
    uint8_t    ln_result = 0;
    adc_ch_t   adc_ch = ADC_CH0;
    for(int i = 0; i < ADC_DEV_NUM; i ++){
        if(cfg->pin == ln_adc_cha_pin[i].tuya_pin)
        {
            ln_base = ln_adc_cha_pin[i].gpio_base;
            ln_pin  = ln_adc_cha_pin[i].pin;
            adc_ch  = ln_adc_cha_pin[i].adc_cha;

            ln_adc_cha_pin[i].adc_init_sta = 1;

            ln_result = 1;
            break;
        }
    }
    if(ln_result == 0)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    hal_gpio_pin_mode_set(ln_base,ln_pin,GPIO_MODE_ANALOG);

    adc_init_t_def adc_init;
    memset(&adc_init,0,sizeof(adc_init));
    
    adc_init.adc_ch = adc_ch;                               //配置通道
    adc_init.adc_conv_mode = ADC_CONV_MODE_CONTINUE;        //配置ADC为连续转换模式
    adc_init.adc_presc = 80;                                //设置采样周期为1M
    hal_adc_init(ADC_BASE,&adc_init);                       //初始化ADC

    hal_adc_en(ADC_BASE,HAL_ENABLE);
    hal_adc_start_conv(ADC_BASE);

    LOG(LOG_LVL_INFO, "[%s, %d] pin=%d, adc_cha=%d, ln_base:%08x, ln_pin:%08x\r\n",
            __func__, __LINE__, cfg->pin, adc_ch, ln_base, ln_pin);

    return OPRT_OS_ADAPTER_OK;
}


static int adc_dev_convert(tuya_adc_t *adc, uint16_t *result)
{
    uint8_t    ln_result = 0;
    adc_ch_t   adc_ch = ADC_CH0;
    for(int i = 0; i < ADC_DEV_NUM; i ++){
        if(adc->cfg.pin == ln_adc_cha_pin[i].tuya_pin)
        {
            adc_ch  = ln_adc_cha_pin[i].adc_cha;

            if(ln_adc_cha_pin[i].adc_init_sta == 1)
                ln_result = 1;
            break;
        }
    }

    if(ln_result == 0)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    if(ln_adc_cha_pin[0].adc_init_sta == 1 || (ln_adc_cha_pin[1].adc_init_sta == 1)){
        
        uint32_t timeout_cnt = 0;
        while(hal_adc_get_conv_status(ADC_BASE,adc_ch) == 0){
            timeout_cnt++;
            if(timeout_cnt > 100){
                LOG(LOG_LVL_INFO, "[%s, %d] adc cha: %d,read data error.\r\n",__func__, __LINE__,adc_ch);
                return OPRT_OS_ADAPTER_COM_ERROR;
            }
            vTaskDelay(1);
        }
        *(unsigned short*)result = hal_adc_get_data(ADC_BASE,adc_ch);
        return OPRT_OS_ADAPTER_OK;
    } else{
        LOG(LOG_LVL_INFO, "[%s, %d] adc cha: %d,adc not init.\r\n",__func__, __LINE__,adc_ch);
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
}

static int adc_dev_control(tuya_adc_t *adc, uint8_t cmd, VOID *arg)
{
    uint8_t    ln_result = 0;
    adc_ch_t   adc_ch = ADC_CH0;
    for(int i = 0; i < ADC_DEV_NUM; i ++){
        if(adc->cfg.pin == ln_adc_cha_pin[i].tuya_pin)
        {
            adc_ch  = ln_adc_cha_pin[i].adc_cha;
            if(ln_adc_cha_pin[i].adc_init_sta == 1)
                ln_result = 1;
            break;
        }
    }
    if(ln_result == 0)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    *(unsigned short*)arg = 0xFFFF;

    if(cmd == TUYA_ADC_VOLTAGE_CMD){
        uint32_t timeout_cnt = 0;
        while(hal_adc_get_conv_status(ADC_BASE,adc_ch) == 0){
            timeout_cnt++;
            if(timeout_cnt > 100){
                LOG(LOG_LVL_INFO, "[%s, %d] adc cha: %d,read data error.\r\n",__func__, __LINE__,adc_ch);
                return OPRT_OS_ADAPTER_COM_ERROR;
            }
            vTaskDelay(1);
        }
    }else{
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    *(unsigned int*)arg = (unsigned int)(hal_adc_get_data(ADC_BASE,adc_ch) * 3300 * 1.0f / 3669.0f);
    return OPRT_OS_ADAPTER_OK;
}

static int adc_dev_deinit(tuya_adc_t  *adc)
{
    uint32_t   ln_base = 0;
    gpio_pin_t ln_pin = GPIO_PIN_0;
    uint8_t    ln_result = 0;
    for(int i = 0; i < ADC_DEV_NUM; i ++){
        if(adc->cfg.pin == ln_adc_cha_pin[i].tuya_pin)
        {
            ln_base = ln_adc_cha_pin[i].gpio_base;
            ln_pin  = ln_adc_cha_pin[i].pin;

            ln_adc_cha_pin[i].adc_init_sta = 0;
            ln_result = 1;
            break;
        }
    }
    if(ln_result == 0)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    hal_gpio_pin_mode_set(ln_base,ln_pin,GPIO_MODE_DIGITAL);
    return OPRT_OS_ADAPTER_OK;
}
