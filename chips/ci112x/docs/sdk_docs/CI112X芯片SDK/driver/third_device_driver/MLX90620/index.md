<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/driver/third_device_driver/MLX90620/ -->

# 红外矩阵传感器MLX90620

## 传感器简介

MLX90620是全校准16X4像素的IR阵列，集成于工业标准四引脚TO-39封装里。同一封装里包含两个芯片MLX90670（集成信号处理的IR阵列）和24AA02（256x8EEPROM）芯片。MLX90620包含64个IR像素点，每个像素都对应有低噪声斩波放大器和高速ADC。芯片中的PTAT（与绝对温度成正比）传感器用来测量环境温度。IR和PTAT的输出都存在内部的RAM，并可以通过IIC通信获取。

* Application使用传感器示例代码

```
/* 以下在`#ifdef SENSOR_INFRARED_ARRAY`和`#endif`之间的代码段，为红外矩阵传感器的
    测试代码，从传感器设备注册、打开、数据接收、中断处理；整个处理流程 */
#ifdef SENSOR_INFRARED_ARRAY
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
    sensor_register(SENSOR_TYPE_INFRARED_ARRAY,mlx90620_ops);
    /* open时传入阻塞或非阻塞属性，只作用于读取数据时 */
    sensor_open(SENSOR_TYPE_INFRARED_ARRAY,SENSOR_NOBLOCK,NULL);
    for(;;)
    {
        /* 获取传感器数据 */
        sensor_read(SENSOR_TYPE_INFRARED_ARRAY,&sensor_data);
        // TODO:
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif
```