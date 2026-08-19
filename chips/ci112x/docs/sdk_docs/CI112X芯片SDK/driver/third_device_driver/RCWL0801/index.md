<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/driver/third_device_driver/RCWL0801/ -->

# 激光测距传感器RCWL0801

## 传感器简介

RCWL0801是一款可3.3V-5Ｖ供电，采用优质激光测距传感器VL53L0X的测距模块。模块采用高性能MCU，内部集成VL53L0X的运算滤波算法，可直接串口输出测量距离。

* Application使用传感器示例代码

```
/* 以下在`#ifdef SENSOR_LASER_FINDER`和`#endif`之间的代码段，为激光传感器的
    测试代码，从传感器设备注册、打开、数据接收、中断处理；整个处理流程 */
#ifdef SENSOR_LASER_FINDER
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
    sensor_register(SENSOR_TYPE_LASER_FINDER,rcwl_0801_ops);
    /* open时传入阻塞或非阻塞属性，只作用于读取数据时 */
    sensor_open(SENSOR_TYPE_LASER_FINDER,SENSOR_NOBLOCK,NULL);
    for(;;)
    {
        /* 获取传感器数据 */
        sensor_read(SENSOR_TYPE_LASER_FINDER,&sensor_data);
        // TODO:
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif
```