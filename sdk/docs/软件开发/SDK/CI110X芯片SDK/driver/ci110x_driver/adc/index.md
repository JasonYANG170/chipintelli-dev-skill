<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/driver/ci110x_driver/adc/ -->

# 模数转换器(ADC)

---

## ADC概述

ADC是CI110X系列芯片中实现模数转换功能的模块，主要用于电压值的模拟量转换成数字量，以供CPU进行电压监控或其他处理。

---

## ADC特性

* 4通道，12-bit分辨率，只支持单端输入通道，SAR ADC；
* 采样速率可达1MSPS（一次转换需15cycles，所以若要达到1MSPS的采样速率，必须保证此时ADC的时钟频率为15MHz）；
* 1MSPS时，工作电流450uA；关闭时电流小于1uA；
* 模拟输入范围为VREFL至VREF。

---

## 使用示例

### 连续转换模式

连续采样模式的情况下，ADC启动之后，会一直工作，用户某个时候想读ADC转换之后的结果，就直接调用adc\_get\_vol\_value函数。

```
void adc_continue_mode_test(void)
{
    adc_init_continue_mode(ADC_CHANNEL_3);
    for(;;)
    {
        float vol_val = 0.0;
        adc_get_vol_value(ADC_CHANNEL_3,&vol_val);
        mprintf("vol_val = %0.3f\n",vol_val);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

### 周期转换模式

在周期转换模式下，ADC会根据设置的周期，一个周期转换一次，如果打开ADC中断，转换完成会触发中断。

```
static uint8_t sg_adc_trans_done = 0;

void adc_trans_done(void)
{
    sg_adc_trans_done = 1;
}  

void adc_cycle_mode_test(void)
{
    //200ms转换一次
    adc_init_cycle_mode(ADC_CHANNEL_3,200,adc_trans_done);
    for(;;)
    {
        while(!sg_adc_trans_done);
        sg_adc_trans_done = 0;
        float vol_val = 0.0;
        adc_get_vol_value(ADC_CHANNEL_3,&vol_val);
        mprintf("vol_val = %0.3f\n",vol_val);
    }
}
```

### 中断模式

中断模式下，有两种情况，一种情况是ADC采样值异常触发中断，另外一种情况是ADC每完成一次转换都会触发中断，
这种情况下，ADC触发中断的情况太过频繁，会对整个系统造成一定的影响，所以ADC的中断模式只使用采样值异常的情况。

```
static uint8_t sg_adc_trans_done = 0;

void adc_trans_done(void)
{
    sg_adc_trans_done = 1;
}  

void adc_int_mode_test(void)
{
    adc_int_mode_init_t adc_str;
    memset(&adc_str,0,sizeof(adc_int_mode_init_t));
    adc_str.cha = ADC_CHANNEL_3;
    adc_str.upper_limit = 3.295;
    adc_str.lower_limit = 0.0;
    //只有超过了设置的上下限才会触发中断
    adc_init_int_mode(&adc_str,adc_trans_done);
    for(;;)
    {
        while(!sg_adc_trans_done);
        sg_adc_trans_done = 0;
        float vol_val = 0.0;
        adc_get_vol_value(ADC_CHANNEL_3,&vol_val);
        mprintf("vol_val = %0.3f\n",vol_val);
    }
}
```

### 校准模式

校准模式下，ADC不会采集管脚上接入的电压，而是测试ADC内部的一个电压基准，校准模式下，ADC的采样值应恒定在(0xfff/2)附近。

```
void adc_caculate_mode_test(void)
{
    adc_init_caculate_mode(ADC_CHANNEL_3);
    for(;;)
    {
        float vol_val = 0.0;
        adc_get_vol_value(ADC_CHANNEL_3,&vol_val);
        mprintf("vol_val = %0.3f\n",vol_val);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```