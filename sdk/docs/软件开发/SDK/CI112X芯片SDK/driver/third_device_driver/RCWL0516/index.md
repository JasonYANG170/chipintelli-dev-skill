<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/driver/third_device_driver/RCWL0516/ -->

# 微波雷达传感器RCWL0516

## 传感器简介

RCWL0516传感器是一款采用多普勒雷达技术、专门检测物体移动的微波感应模块。该模块具有灵敏度高、感应距离远、可靠性强、感应角度大、供电范围广等特点。广泛应用于各种人体感应照明和防盗报警等场合。

* Application使用传感器示例代码

```
/* 以下在`#ifdef SENSOR_MICROWAVE`和`#endif`之间的代码段，为雷达微波传感器的
    测试代码，从传感器设备注册、打开、数据接收、中断处理；整个处理流程 */
#ifdef SENSOR_MICROWAVE
/**
 * @brief 中断回调函数
 *
 */
void RCWL_0516_callback(void)
{
    mprintf("雷达微波触发\n");
}

/**
 * @brief 传感器测试任务
 *
 * @param p 任务参数
 */
void sensor_task(void *p)
{
    /* 注册传感器时，要明确传感器类型、API结构体 */
    sensor_register(SENSOR_TYPE_MICROWAVE,rcwl0516_ops);
    /* open时传入阻塞或非阻塞属性，只作用于读取数据时 */
    sensor_open(SENSOR_TYPE_MICROWAVE,SENSOR_NOBLOCK,RCWL_0516_callback);
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif
```