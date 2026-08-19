<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/driver/third_device_driver/APDS9960/ -->

# 手势光感传感器APDS9960

## 传感器简介

APDS-9960传感器具有先进的手势检测、接近检测、数字环境光感（ALS）和色感（RGBC）。

* Application使用传感器示例代码

```
/* 以下在`#ifdef SENSOR_GESTURE`和`#endif`之间的代码段，为光感手势传感器的
    测试代码，从传感器设备注册、打开、数据接收、中断处理；整个处理流程 */
#ifdef SENSOR_GESTURE
/**
 * @brief 中断回调函数
 *
 */
void APDS_9960_callback(void)
{
    gesture_manage();/* 手势处理 */
}

/**
 * @brief 传感器测试任务
 *
 * @param p 任务参数
 */
void sensor_task(void *p)
{
    /* 注册传感器时，要明确传感器类型、API结构体 */
    sensor_register(SENSOR_TYPE_GESTURE,apds9960_ops);
    /* open时传入阻塞或非阻塞属性，只作用于读取数据时 */
    sensor_open(SENSOR_TYPE_GESTURE,SENSOR_NOBLOCK,APDS_9960_callback);
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif
```