#include "cias_system.h"
#include "ln_compiler.h"
#include "ln_kv_api.h"
#include "sysparam_factory_setting.h"
#include "utils/system_parameter.h"
#include "hal/hal_gpio.h"
#include "cias_log.h"
#include "cias_user_config.h"
#define INPUT_PARAM_CHECK(v)                   \
    do                                         \
    {                                          \
        if (v)                                 \
        {                                      \
            return SYSPARAM_ERR_INVALID_PARAM; \
        }                                      \
    } while (0)

__STATIC_INLINE__ int cias_sysparam_store(const char *key, const void *value, size_t value_len)
{
    int ret = ln_kv_set(key, value, value_len);
    if (ret != KV_ERR_NONE)
    {
        return SYSPARAM_ERR_STORE;
    }
    return SYSPARAM_ERR_NONE;
}

__STATIC_INLINE__ int cias_sysparam_load(const char *key, void *value_buf, size_t value_buf_size)
{
    size_t v_len = 0;
    int ret = ln_kv_get(key, value_buf, value_buf_size, &v_len);
    if (ret != KV_ERR_NONE)
    {
        return SYSPARAM_ERR_LOAD;
    }
    return SYSPARAM_ERR_NONE;
}

__STATIC_INLINE__ int sysparam_has_stored(const char *key)
{
    return ln_kv_has_key(key);
}
#if (CIAS_IOT_TENCENT_ENABLE)

//for cias aiot
int iot_tencent_profile_set(mqtt_client_profile_t *profile)
{
    INPUT_PARAM_CHECK(!profile);
    return cias_sysparam_store(KV_IOT_TENCENT_PROFILE, (void *)profile, sizeof(mqtt_client_profile_t));
}
int iot_tencent_profile_get(mqtt_client_profile_t *profile)
{
    INPUT_PARAM_CHECK(!profile);
    return cias_sysparam_load(KV_IOT_TENCENT_PROFILE, (void *)profile, sizeof(mqtt_client_profile_t));
}
#endif
#if TENCENT_AP_NET_WORK_EN// add by hw
int iot_tencent_wifi_set(char *name, char* passwd)
{
    char temp[128] = {0};
    temp[0] = 0x1A;  //存wifi账号和密码是否有效的标志
    sprintf(temp + 1, "%s,%s", name, passwd);
    return cias_sysparam_store(KV_DEV_WIFI_NAME_PASSWD, (void *)temp, strlen(temp));
}
int iot_tencent_wifi_get(char *name, char* passwd)
{
    char temp[128] = {0};
    char str_remove[2] = {0};
    cias_sysparam_load(KV_DEV_WIFI_NAME_PASSWD, (void *)temp, sizeof(temp));
    if(temp[0] == 0x1A)
    {   
        // sscanf(temp+1, "%03s,%03s", name, passwd);
        sscanf(temp+1, "%[^,]%[,]%s",name,str_remove,passwd);
        return SYSPARAM_ERR_NONE;
    }
    return SYSPARAM_ERR_LOAD;
    
}
#endif//TENCENT_AP_NET_WORK_EN

void cias_system_init(void)
{
    //PF3->GPIOB3 设置为输入模式
    hal_gpio_pin_afio_en(GPIOB_BASE, GPIO_PIN_3, HAL_ENABLE);
    gpio_init_t_def gpio_init;
    memset(&gpio_init, 0, sizeof(gpio_init)); //清零结构体
    gpio_init.dir = GPIO_INPUT;               //配置GPIO方向，输入
    gpio_init.pin = GPIO_PIN_3;               //配置GPIO引脚号
    gpio_init.speed = GPIO_HIGH_SPEED;        //设置GPIO速度
    hal_gpio_init(GPIOB_BASE, &gpio_init);    //初始化GPIO
}