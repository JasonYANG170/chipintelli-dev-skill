<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/CI230X%E5%86%85%E9%83%A8%E7%AE%A1%E8%84%9A%E6%98%A0%E5%B0%84%E8%A1%A8/ -->

# CI230X系列芯片内部管脚映射表

---

## 1. GPIO映射管脚说明

​ 由于CI230X系列芯片内部分为语音部分和wifi部分，对外的管脚分别为PA，PB， PC，PD，PE，PF；由于语音部分和wifi部分的sdK分为两部分，所以在对GPIO控制时，需要按照下面表格管脚映射来操作具体的内部管脚。硬件管脚描述请参考 ☞[CI230X系列芯片引脚描述](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/CI2305%26CI2306%E8%8A%AF%E7%89%87%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8C/%E5%BC%95%E8%84%9A%E6%8F%8F%E8%BF%B0/)

| WIFI部分内部管脚 | 外部管脚 | QFN56-管脚序号 |
| --- | --- | --- |
| GPIOA0 | PE0 | 8 |
| GPIOA1 | PE1 | 9 |
| GPIOA2 | PE2 | 10 |
| GPIOA3 | PE3 | 11 |
| GPIOA4 | PE4 | 12 |
| GPIOA6 | PE6 | 13 |
| GPIOA7 | PE7 | 14 |
| GPIOA9 | PE9 | 15 |
| GPIOB3 | PF3 | 48 |
| GPIOB4 | PF4 | 49 |
| GPIOB5 | PF5 | 50 |
| GPIOB6 | PF6 | 51 |
| GPIOB7 | PF7 | 52 |
| GPIOB8 | PF8 | 53 |
| GPIOB9 | PF9 | 54 |
| **语音部分内部管脚** | **外部管脚** | **QFN56-管脚序号** |
| u\_iomux/u\_GPIO3\_0\_pad\_PAD | PD0 | 34 |
| u\_iomux/u\_GPIO0\_2\_pad\_PAD | PA2 | 35 |
| u\_iomux/u\_GPIO0\_3\_pad\_PAD | PA3 | 36 |
| u\_iomux/u\_GPIO0\_4\_pad\_PAD | PA4 | 37 |
| u\_iomux/u\_GPIO0\_5\_pad\_PAD | PA5 | 38 |
| u\_iomux/u\_GPIO0\_6\_pad\_PAD | PA6 | 39 |
| u\_iomux/u\_GPIO0\_7\_pad\_PAD | PA7 | 40 |
| u\_iomux/u\_GPIO1\_0\_pad\_PAD | PB0 | 41 |
| u\_iomux/u\_GPIO1\_1\_pad\_PAD | PB1 | 42 |
| u\_iomux/u\_GPIO1\_2\_pad\_PAD | PB2 | 43 |
| u\_iomux/u\_GPIO1\_3\_pad\_PAD | PB3 | 44 |
| u\_iomux/u\_GPIO1\_4\_pad\_PAD | PB4 | 45 |
| u\_iomux/u\_GPIO1\_5\_pad\_PAD | PB5 | 46 |
| u\_iomux/u\_GPIO1\_6\_pad\_PAD | PB6 | 47 |
| u\_iomux/u\_GPIO2\_3\_pad\_PAD | PC3 AIN3 | 17 |
| u\_iomux/u\_GPIO2\_4\_pad\_PAD | PC4 AIN2 | 18 |

## 2. GPIO操作实例

### 2.1 WIFI实例

WIFI的内部管脚GPIOB3对应的外部管脚为PF3，现将PF3管脚(GPIOB3)设置为输入模式

```
    hal_gpio_pin_afio_en(GPIOB_BASE, GPIO_PIN_3, HAL_ENABLE);   /*管脚使能,GPIOB_BASE已定义为硬件地址 0x4000C400 */
    gpio_init_t_def gpio_init;
    memset(&gpio_init, 0, sizeof(gpio_init)); /*清零结构体*/
    gpio_init.dir = GPIO_INPUT;               /*配置GPIO方向，输入*/
    gpio_init.pin = GPIO_PIN_3;               /*配置GPIO引脚号*/
    gpio_init.speed = GPIO_HIGH_SPEED;        /*设置GPIO速度*/
    hal_gpio_init(GPIOB_BASE, &gpio_init);    /*初始化GPIO*/
```

### 2.2 语音实例

使用CI230X系列芯片模块去点亮RGB的蓝色灯光，根据模块原理图可使用PB2管脚(u\_GPIO1\_2\_pad\_PAD)去控制RGB蓝色灯光的亮灭(需外接跳线帽连接CI230X系列芯片与RGB灯)

```
    scu_set_device_gate(PB,ENABLE);              /*开启外设时钟,PB已赋值为 u_GPIO1的硬件地址 0x40021000 */
    dpmu_set_io_reuse(PB2,FIRST_FUNCTION);       /*管脚复用 FIRST_FUNCTION:gpio function*/
    dpmu_set_io_direction(PB2, DPMU_IO_DIRECTION_OUTPUT);   /*配置管脚方向*/
    gpio_set_output_mode(PB, pin_2);             /*管脚配置为输出模式*/
    gpio_set_output_level_single(PB, pin_2, 1);  /*PB2输出高电平*/
```