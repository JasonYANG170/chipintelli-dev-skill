<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-NN-ENC/CI13XX_SDK_NN_ENC_V2.1.8/API%E5%8F%82%E8%80%83/%E5%A4%96%E8%AE%BEAPI/adc/ -->

# 模数转换器(ADC)

---

## 1、简介

* SARADC（Analog-to-Digital Converter）是将连续变化的模拟信号（如电压、电流、温度、声音等）转换为离散数字信号（二进制代码）的模块，以下简称ADC。

---

## 2、特性

* ADC分辨率为12bit；
* 支持单次采样和连续采样模式；
* 采样速率可达1MSPS(Mega Samples Per Second)，若要达到1MSPS的采样速率，必须保证此时ADC的时钟频率不小于15MHz；
* 采样速率为1MSPS时，工作电流450uA；关闭时电流小于1uA；
* 模拟电源电压输入范围为1.8V~3.63V；
* 中断产生有两种情况，一种情况是ADC采样值异常触发中断；另外一种情况是ADC每完成一次转换都会触发中断；
* 支持6通道单端输入，CI13XX的ADC含有4个外部通道ADC\_CHANNEL\_2(AIN2)/
  ADC\_CHANNEL\_3(AIN3)/ADC\_CHANNEL\_4(AIN4)/ADC\_CHANNEL\_5(AIN5)，通道ADC\_CHANNEL\_0(AIN0)/ADC\_CHANNEL\_1(AIN1) 供芯片内部使用；

---

## 3、使用示例

* CI13XX的引脚和SARADC通道对应关系为：

| 引脚名 | 对应的ADC通道 |
| --- | --- |
| PC1 | ADC\_CHANNEL\_5（AIN5） |
| PC2 | ADC\_CHANNEL\_4（AIN4） |
| PC3 | ADC\_CHANNEL\_3（AIN3） |
| PC4 | ADC\_CHANNEL\_2（AIN2） |

### 3.1、连续转换模式

* 连续采样模式的情况下，ADC启动之后，会一直工作，用户某个时候想读ADC转换之后的结果，就直接调用adc\_get\_result函数。下面为ADC连续转换模式的使用示例代码：

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

    /*ADC通道5的引脚初始化*/
    dpmu_set_adio_reuse(PC1,ANALOG_MODE);       //配置ADC通道5的引脚为模拟功能，更多请查看ci130x_sch.h的IO复用注释
    dpmu_set_io_pull(PC1,DPMU_IO_PULL_DISABLE); //关闭引脚内部上下拉

    /*ADC控制器时钟、中断配置*/
    scu_set_device_gate(HAL_ADC_BASE,ENABLE);   //打开ADC时钟
    eclic_irq_enable(ADC_IRQn);                 //使能中断
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle); //指定中断服务函数

    /*ADC上电、初始化成连续转换模式*/
    adc_poweron();                              //ADC上电
    adc_series_mode(ADC_CHANNEL_5);             //初始化成连续转换模式

    /*连续采集电压值，这里举例按200次算一次平均值，用户可按需调整次数*/
    for(;;)
    {
        /*每隔1 ms 调用adc_get_result读1次电压值*/
        for(int i = 0;i < 200;i++)
        {
            temp = adc_get_result(ADC_CHANNEL_5); //获取电压值
            temp = temp*33000/4096;
            temp /= 10;
            adc_current[i] = temp;
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        /*计算200次电压的平均值*/
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

* 普通软件触发模式测试，需要软件写一次触发才会采集一次数据，再调用adc\_get\_result函数获取电压。下面为ADC普通软件触发模式的使用示例代码：

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

    /*ADC通道4的引脚初始化*/
    dpmu_set_adio_reuse(PC2,ANALOG_MODE);        //配置ADC通道4的引脚为模拟功能，更多请查看ci130x_sch.h的IO复用注释
    dpmu_set_io_pull(PC2,DPMU_IO_PULL_DISABLE);  //关闭引脚内部上下拉

    /*ADC控制器时钟、中断配置*/
    scu_set_device_gate(HAL_ADC_BASE,ENABLE);         //打开ADC时钟
    eclic_irq_enable(ADC_IRQn);                  //使能中断
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle);   //指定中断服务函数

    /*ADC上电、初始化成普通软件触发模式*/
    adc_poweron();                               //ADC上电
    adc_signal_mode(ADC_CHANNEL_4);              //初始化成普通软件触发模式

    /*连续采集电压值，这里举例按200次算一次平均值，用户可按需调整次数*/
    for(;;)
    {
        /*每隔2 ms 调用1次adc_soc_soft_ctrl软件触发，中断有效后调用adc_get_result读1次电压值*/
        for(int i = 0;i < 200;i++)
        {
            adc_soc_soft_ctrl(ENABLE);            //ADC软件触发（软件强制开始转换）
            adc_wait_int(ADC_CHANNEL_4);          //等待中断
            temp = adc_get_result(ADC_CHANNEL_4); //获取电压值
            temp = temp*33000/4096;
            temp /= 10;
            adc_current[i] = temp;
            vTaskDelay(pdMS_TO_TICKS(2));
        }

        /*计算200次电压的平均值*/
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

* 在周期转换模式下，ADC会根据设置的周期，一个周期转换一次，如果打开ADC中断，转换完成会触发中断。下面为ADC周期转换模式的使用示例代码：

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
    extern uint8_t adc_cha3_int_flag;      //用于查询异常中断的标志

    /*ADC通道3的引脚初始化*/
    dpmu_set_adio_reuse(PC3,ANALOG_MODE);         //配置ADC通道3的引脚为模拟功能，更多请查看ci130x_sch.h的IO复用注释
    dpmu_set_io_pull(PC3,DPMU_IO_PULL_DISABLE);   //关闭引脚内部上下拉

    /*ADC控制器时钟、中断配置*/
    scu_set_device_gate(HAL_ADC_BASE,ENABLE);     //打开ADC时钟
    eclic_irq_enable(ADC_IRQn);                   //使能中断
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle);   //指定中断服务函数

    /*ADC上电、初始化成周期转换模式*/
    adc_poweron();                                   //ADC上电
    adc_cycle_mode(ADC_CHANNEL_3,1000);              //配置周期 (1000 + 1) * 512
    adc_channel_min_value(ADC_CHANNEL_3,1241);       //最小阈值设置，宽度范围是0 ~ 4095
    adc_channel_min_value_int(ADC_CHANNEL_3,ENABLE); //ADC通道采样结果低于阀值下限中断使能
    adc_channel_max_value(cha,4095);                 //最大阈值设置，宽度范围是0 ~ 4095
    adc_channel_max_value_int(cha,ENABLE);           //ADC通道采样结果超过阀值上限中断使能

    /*连续采集电压值，这里举例按200次算一次平均值，用户可按需调整次数*/
    for(;;)
    {
        /*每隔1 ms 调用adc_get_result读1次电压值*/
        for(int i = 0;i < 200;i++)
        {
            temp = adc_get_result(ADC_CHANNEL_3); //获取电压值
            temp = temp*33000/4096;
            temp /= 10;
            adc_current[i] = temp;
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        /*如果在采集到阈值之外的值，报告异常*/
        if(adc_cha3_int_flag)
        {
            mprintf("电压异常!!\n");     
            adc_cha3_int_flag = 0;
        }

        /*计算200次电压的平均值*/
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

* 校准模式下，ADC不会采集管脚上接入的电压，而是测试ADC内部的一个电压基准，校准模式下，ADC的采样值应恒定在(0xfff/2)附近。下面为ADC校准模式的使用示例代码：

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

    /*ADC通道2的引脚初始化*/
    dpmu_set_adio_reuse(PC4,ANALOG_MODE);      //配置ADC通道2的引脚为模拟功能，更多请查看ci130x_sch.h的IO复用注释

    /*ADC控制器时钟、中断配置*/
    scu_set_device_gate(HAL_ADC_BASE,ENABLE);  //打开ADC时钟
    eclic_irq_enable(ADC_IRQn);                //使能中断
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle);   //指定中断服务函数

    /*ADC上电、初始化成周期转换模式*/
    adc_poweron();                      //ADC上电
    adc_caculate_mode();                //打开ADC校准模式
    adc_signal_mode(ADC_CHANNEL_2);     //配置单次转换模式

    /*连续采集电压值，这里举例按200次算一次平均值，用户可按需调整次数*/
    for(;;)
    {
        for(recv_count=0;recv_count<adc_test_count;recv_count++)
        {
            /*每隔1 ms 调用1次adc_soc_soft_ctrl软件触发，中断有效后调用adc_get_result读1次电压值*/
            for(int i = 0;i < 200;i++)
            {
                adc_soc_soft_ctrl(ENABLE);            //ADC软件触发（软件强制开始转换）
                adc_wait_int(ADC_CHANNEL_2);          //等待中断
                temp = adc_get_result(ADC_CHANNEL_2); //获取电压值
                temp = temp*33000/4096;
                temp /= 10;
                adc_current[i] = temp;
                vTaskDelay(pdMS_TO_TICKS(1));
            }

            /*计算200次电压的平均值*/
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

* 内部ADC电压检测，采用一个周期检测一次电压，再调用adc\_get\_result函数获取电压。下面为ADC内部ADC电压检测的使用示例代码：

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

    /*ADC的时钟分频、时钟开关、复位配置*/
    scu_set_device_gate(HAL_ADC_BASE,DISABLE);
    scu_set_device_reset(HAL_ADC_BASE);
    scu_para_en_disable(HAL_ADC_BASE);
    scu_set_div_parameter(HAL_ADC_BASE,12);
    scu_para_en_enable(HAL_ADC_BASE);
    scu_set_device_reset_release(HAL_ADC_BASE);
    scu_set_device_gate(HAL_ADC_BASE,ENABLE);

    /*内部ADC电压检测初始化*/
    dpmu_unlock_cfg_config();                       //解锁
    dpmu_pmu_div_resistance_en(ENABLE);             //分压电阻使能
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO1); //LDO1配置更新使能
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO2); //LDO2配置更新使能
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO3); //LDO3配置更新使能
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_VDT);  //VDT配置更新使能
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_TRIM); //TRIM配置更新使能
    dpmu_lock_cfg_config();

    /*ADC中断、上电配置*/
    eclic_irq_enable(ADC_IRQn);                     //使能中断
    __eclic_irq_set_vector(ADC_IRQn, (int32_t)ADC_irqhandle);   //指定中断服务函数
    adc_poweron();                                  //ADC上电
    adc_cycle_mode(ADC_CHANNEL_1,10);               //配置周期 (10 + 1) * 512

    for(;;)
    {
        for(int i = 0;i < 2000;i++)
        {
            temp = adc_get_result(ADC_CHANNEL_1);  //获取电压值
            temp = temp*33000/4096;
            temp /= 10;
            mprintf("%d:%d\n",ADC_CHANNEL_1,temp);
        }
    }
}
```

## 4、API 参考