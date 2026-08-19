<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/driver/third_device_driver/YW51/ -->

# 灰尘传感器YW51

## 传感器简介

YW51传感器是PM2.5普三代灰尘传感器，利用微小颗粒物体对光的散射原理，当微小颗粒物体经过检测孔，对光线形成散射时；散射光的一部分通过光轴，经透镜聚集到感光元器件，感光元器件将光信号转换为电信号输出。经过改进取消模拟输出口，并且将内部算法进行微调。串口输出更稳定。

* Application使用传感器示例代码

```
/* 以下在`#ifdef SENSOR_PARTICULATES`和`#endif`之间的代码段，为PM2.5传感器的
    测试代码，从传感器设备注册、打开、数据接收、中断处理；整个处理流程 */
#ifdef SENSOR_PARTICULATES
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
    sensor_register(SENSOR_TYPE_PARTICULATES,yw51_ops);
    /* open时传入阻塞或非阻塞属性，只作用于读取数据时 */
    sensor_open(SENSOR_TYPE_PARTICULATES,SENSOR_NOBLOCK,NULL);
    for(;;)
    {
        for(int i = 0;i < 10;i++)
        {
            /* 获取传感器数据 */
            sensor_read(SENSOR_TYPE_PARTICULATES,&sensor_data);
            // TODO:
            mprintf("PM2.5 %f 单位 xx\n",sensor_data.particulates);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
#endif
```