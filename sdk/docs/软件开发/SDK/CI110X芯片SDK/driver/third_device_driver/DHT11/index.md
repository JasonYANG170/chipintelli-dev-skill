<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/driver/third_device_driver/DHT11/ -->

# 温湿度传感器DHT11

## 传感器简介

DHT11数字温度湿度传感器是一款有已校准数字信号输出的温湿度复合传感器，其测量精度：湿度+-5%RH、温度：+-2℃；测量范围：湿度20-90%RH、温度0-50℃。

* Application使用传感器示例代码

```
/* 以下在`#ifdef SENSOR_HUMIDITY`和`#endif`之间的代码段，为温湿度传感器的测
    试代  码，从传感器设备注册、打开、数据接收、中断处理；整个处理流程 */
#ifdef SENSOR_HUMIDITY
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
    sensor_register(SENSOR_TYPE_HUMIDITY,dht11_ops);
    /* open时传入阻塞或非阻塞属性，只作用于读取数据时 */
    sensor_open(SENSOR_TYPE_HUMIDITY,SENSOR_NOBLOCK,NULL);
    for(;;)
    {
        for(int i = 0;i < 10;i++)
        {
            /* 获取传感器数据，这里只读取了湿度 */
            sensor_read(SENSOR_TYPE_HUMIDITY,&sensor_data);
            // TODO:
            mprintf("humidity %f 单位‰RH\n",sensor_data.humidity);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
#endif
```