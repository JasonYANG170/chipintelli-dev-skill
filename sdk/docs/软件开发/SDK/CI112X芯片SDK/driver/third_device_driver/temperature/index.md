<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/driver/third_device_driver/temperature/ -->

# 热敏电阻温度传感器

## 传感器简介

热敏电阻传感器，其阻值随着温度的变化而变化，我们通过ADC采集热敏电阻上的电压值，再通过计算得到当前温度信息。

* Application使用传感器示例代码

```
/* 以下在`#ifdef SENSOR_TEMPERATURE`和`#endif`之间的代码段，为温度传感器的测
    试代  码，从传感器设备注册、打开、数据接收、中断处理；整个处理流程 */
#ifdef SENSOR_TEMPERATURE
/**
 * @brief 传感器测试任务
 *
 * @param p 任务参数
 */
void sensor_task(void *p)
{
    /* 用于读取和保存获取到的传感器数据 */
    sensor_data_t sensor_data;
    /* 注册传感器时，要明确传感器类型、API结构体 */
    sensor_register(SENSOR_TYPE_TEMPERATURE,temperature_ops);
    /* open时传入阻塞或非阻塞属性，只作用于读取数据时 */
    sensor_open(SENSOR_TYPE_TEMPERATURE,SENSOR_NOBLOCK,NULL);
    for(;;)
    {
        for(int i = 0;i < 10;i++)
        {
            /* 获取传感器数据 */
            sensor_read(SENSOR_TYPE_TEMPERATURE,&sensor_data);
            // TODO:
            mprintf("temperature %d 单位0.1℃\n",sensor_data.temperature);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
#endif
```