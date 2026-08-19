<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI231X%E8%8A%AF%E7%89%87SDK/driver/ci231x_driver/adc/ -->

# 模数转换器(ADC)

---

## 1、ADC概述

* ADC是CI231X系列芯片中实现模数转换功能的模块，主要用于电压值的模拟量转换成数字量，以供CPU进行电压监控或其他处理。

---

## 2、ADC特性

* 4通道，12-bit分辨率，只支持单端输入通道，SAR ADC；
* 采样速率可达1MSPS（一次转换需15cycles，所以若要达到1MSPS的采样速率，必须保证此时ADC的时钟频率为15MHz）；
* 1MSPS时，工作电流450uA；关闭时电流小于1uA；
* 模拟输入范围为VREFL至VREF。
* 中断有两种情况，一种情况是ADC采样值异常触发中断。另外一种情况是ADC每完成一次转换都会触发中断，这种情况下，ADC触发中断的情况太过频繁，会对整个系统造成一定的影响，所以ADC的中断模式只使用采样值异常的情况。
* CI231X系列芯片的ADC含有4个外部通道ADC\_CHANNEL\_2（AIN2）、ADC\_CHANNEL\_3（AIN3）、ADC\_CHANNEL\_4（AIN4）、ADC\_CHANNEL\_5（AIN5）

---

## 3、使用示例

### 3.1、连续转换模式

* 连续采样模式的情况下，ADC启动之后，会一直工作，用户某个时候想读ADC转换之后的结果，就直接调用adc\_get\_result函数。

```
#include "ci130x_system.h"
#include <string.h>
#include "ci130x_core_eclic.h"
#include "ci_log.h"
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_adc.h"

uint32_t adc_current[502] = {0};

void adc_series_mode_test(void)
{
    uint32_t recv_count = 0;
    uint32_t temp;
    uint32_t val;

    scu_set_device_gate(HAL_ADC_BASE,1);     //打开ADC时钟
    dpmu_set_adio_reuse(PC1,ANALOG_MODE);     //配置ADC通道5的引脚为模拟功能，更多请查看ci130x_sch.h的IO复用注释
    dpmu_set_io_pull(PC1,DPMU_IO_PULL_DISABLE);    //关闭引脚内部上下拉
    eclic_irq_enable(ADC_IRQn);       //使能中断
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle);   //指定中断服务函数
    adc_poweron();
    adc_series_mode(ADC_CHANNEL_5);    //初始化成连续模式
    for(;;)
    {
        for(int i = 0;i < 200;i++)
        {
            temp = adc_get_result(ADC_CHANNEL_5);
            temp = temp*33000/4096;
            temp /= 10;
            adc_current[i] = temp;
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        for(int i = 0;i < 200;i++)
        {
            val += adc_current[i];
        }
        val /= 200;
        mprintf("val:%dmv\n",val);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

### 3.2、普通软件触发模式

* 普通软件触发模式测试，需要软件写一次触发才会采集一次数据，再调用adc\_get\_result函数获取电压。

```
#include "ci130x_system.h"
#include <string.h>
#include "ci130x_core_eclic.h"
#include "ci_log.h"
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_adc.h"

uint32_t adc_current[502] = {0};

void adc_signal_mode_test()
{
    uint32_t temp;
    uint32_t val;
    scu_set_device_gate(HAL_ADC_BASE,1);     //打开ADC时钟
    dpmu_set_adio_reuse(PC1,ANALOG_MODE);       //配置ADC通道5的引脚为模拟功能，更多请查看ci130x_sch.h的IO复用注释
    dpmu_set_io_pull(PC1,DPMU_IO_PULL_DISABLE);    //关闭引脚内部上下拉
    eclic_irq_enable(ADC_IRQn);       //使能中断
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle);   //指定中断服务函数
    adc_poweron();             
    adc_signal_mode(ADC_CHANNEL_5);   //初始化成普通软件触发模式

    for(;;)
    {
        for(int i = 0;i < 200;i++)
        {
            adc_soc_soft_ctrl(ENABLE);     //ADC软件触发（软件强制开始转换）
            adc_wait_int(ADC_CHANNEL_5);       //等待中断
            temp = adc_get_result(ADC_CHANNEL_5);    //获取瞬时电压值
            temp = temp*33000/4096;
            temp /= 10;
            adc_current[i] = temp;
            vTaskDelay(pdMS_TO_TICKS(2));
        }
        for(int i = 0;i < 200;i++)
        {
            val += adc_current[i];
        }
        val /= 200;
        mprintf("val:%dmv\n",val);       
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

### 3.3、周期转换模式

* 在周期转换模式下，ADC会根据设置的周期，一个周期转换一次，如果打开ADC中断，转换完成会触发中断。

```
#include "ci130x_system.h"
#include <string.h>
#include "ci130x_core_eclic.h"
#include "ci_log.h"
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_adc.h"

uint32_t adc_current[502] = {0};

void adc_cycle_mode_test(void)
{
    uint32_t temp;
    uint32_t val;
    extern uint8_t adc_cha5_int_flag;      //用于查询异常中断的标志

    scu_set_device_gate(HAL_ADC_BASE,1);     //打开ADC时钟
    dpmu_set_adio_reuse(PC1,ANALOG_MODE);    //配置ADC通道5的引脚为模拟功能，更多请查看ci130x_sch.h的IO复用注释
    dpmu_set_io_pull(PC1,DPMU_IO_PULL_DISABLE);   //关闭引脚内部上下拉

    eclic_irq_enable(ADC_IRQn);     //使能中断
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle);   //指定中断服务函数
    adc_poweron();
    adc_cycle_mode(ADC_CHANNEL_5,1000);     //配置周期 (1000 + 1) * 512

    adc_channel_min_value(ADC_CHANNEL_5,1241);       //最小阈值设置，宽度范围是0 ~ 4095
    adc_channel_min_value_int(ADC_CHANNEL_5,ENABLE);
    adc_channel_max_value(cha,4095);                 //最大阈值设置，宽度范围是0 ~ 4095
    adc_channel_max_value_int(cha,ENABLE);
    for(;;)
    {
        for(int i = 0;i < 200;i++)
        {
            temp = adc_get_result(ADC_CHANNEL_5);
            temp = temp*33000/4096;
            temp /= 10;
            adc_current[i] = temp;
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        if(adc_cha5_int_flag)
        {
            mprintf("电压异常!!\n");     //如果在采集到阈值之外的值，报告异常
            adc_cha5_int_flag = 0;
        }
        for(int i = 0;i < 200;i++)
        {
            val += adc_current[i];
        }
        val /= 200;
        mprintf("val:%dmv\n",val);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

### 3.4、校准模式

* 校准模式下，ADC不会采集管脚上接入的电压，而是测试ADC内部的一个电压基准，校准模式下，ADC的采样值应恒定在(0xfff/2)附近。

```
#include "ci130x_system.h"
#include <string.h>
#include "ci130x_core_eclic.h"
#include "ci_log.h"
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_adc.h"

uint32_t adc_test_count = 10000;
uint32_t adc_current[502] = {0};

void adc_correct_test(void)
{
    uint32_t recv_count = 0;
    uint32_t temp;
    uint32_t val;

    scu_set_device_gate(HAL_ADC_BASE,1);   //打开ADC时钟
    dpmu_set_adio_reuse(PC1,ANALOG_MODE);    //配置ADC通道5的引脚为模拟功能，更多请查看ci130x_sch.h的IO复用注释
    eclic_irq_enable(ADC_IRQn);
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle);   //指定中断服务函数
    adc_poweron();
    adc_caculate_mode();    //打开ADC校验模式
    adc_signal_mode(ADC_CHANNEL_5);     配置单次转换模式
    for(;;)
    {
        for(recv_count=0;recv_count<adc_test_count;recv_count++)
        {
            for(int i = 0;i < 200;i++)
            {
                adc_soc_soft_ctrl(ENABLE);
                adc_wait_int(ADC_CHANNEL_5);
                temp = adc_get_result(ADC_CHANNEL_5);
                temp = temp*33000/4096;
                temp /= 10;
                adc_current[i] = temp;
                vTaskDelay(pdMS_TO_TICKS(1));
            }
            for(int i = 0;i < 200;i++)
            {
                val += adc_current[i];
            }
            val /= 200;
            mprintf("第%d次val:%dmv\n",recv_count,val);
        }
    }
}
```

### 3.5、内部ADC电压检测

* 内部ADC电压检测，采用一个周期检测一次电压，再调用adc\_get\_result函数获取电压。

```
#include "ci130x_system.h"
#include <string.h>
#include "ci130x_core_eclic.h"
#include "ci_log.h"
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_adc.h"

void adc_vol_test()
{

    uint32_t temp;

    scu_set_device_gate(HAL_ADC_BASE,0);
    scu_set_device_reset(HAL_ADC_BASE);
    scu_para_en_disable(HAL_ADC_BASE);
    scu_set_div_parameter(HAL_ADC_BASE,12);
    scu_para_en_enable(HAL_ADC_BASE);
    scu_set_device_reset_release(HAL_ADC_BASE);
    scu_set_device_gate(HAL_ADC_BASE,1);

    dpmu_unlock_cfg_config();//解锁
    dpmu_pmu_div_resistance_en(ENABLE);//分压电阻使能
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO1);//LDO配置更新使能
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO2);
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO3);
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_VDT);
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_TRIM);
    dpmu_lock_cfg_config();
    eclic_irq_enable(ADC_IRQn);       //使能中断
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle);   //指定中断服务函数
    adc_poweron();    

    for(;;)
    {
        adc_cycle_mode(ADC_CHANNEL_1,10);
        for(int i = 0;i < 2000;i++)
        {
            temp = adc_get_result(ADC_CHANNEL_1);
            temp = temp*33000/4096;
            temp /= 10;
            mprintf("%d:%d\n",ADC_CHANNEL_1,temp);
        }
    }
}
```