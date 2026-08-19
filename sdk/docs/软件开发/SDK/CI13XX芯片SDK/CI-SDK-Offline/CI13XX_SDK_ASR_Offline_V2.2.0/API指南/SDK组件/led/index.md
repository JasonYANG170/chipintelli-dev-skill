<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/API%E6%8C%87%E5%8D%97/SDK%E7%BB%84%E4%BB%B6/led/ -->

# 灯控(LED)

---

## 1. 概述

组件包中的led为灯控组件，提供了以下几种常见场景灯控的代码。

* 彩色灯：RGB灯或者RGBW灯，使用PWM控制，使用HSV色域空间，色彩丰富，亮度效果好，可以分别调整颜色和亮度，相互无影响；
* 小夜灯：使用PWM控制，亮度可调；
* 眨眼灯：使用普通IO和软件timer控制，一个用于玩具内的参考，通过1个简单的IO实现唤醒应答效果。

---

## 2. 使用说明

| 函数接口 | 说明 |
| --- | --- |
| color\_light\_init | 初始化灯控组件 |
| color\_light\_control | 设置颜色(HSV all) |
| color\_light\_set\_level | 设置颜色(hue and saturation) |
| color\_light\_set\_color | 设置亮度(value(lightness)) |
| night\_light\_init | 小夜灯初始化 |
| night\_light\_set\_brightness | 小夜灯设置亮度 |
| blink\_light\_init | 眨眼灯初始化 |
| blink\_light\_on | 眨眼灯开启一次 |

使用示例：

```
#define POWER_ON_COLOR 360,1.0,1.0 //179,0.32,0.3

/*彩色灯示例*/
void light_init(void)
{
    /* 初始化灯控组件 */
    color_light_init();
    /* 设置颜色 */
    color_light_control(POWER_ON_COLOR);
    color_light_set_level(0.0);

    ci_loginfo(LOG_USER,"color light ok\n");

    for(int i=0;i<100;i++)
    {
        color_light_set_level((float)i/100.0);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
```

```
/*小夜灯示例*/
void light_sample(void)
{
    /* 初始化 */
    night_light_init();
    /* 打开灯，并设置亮度80% */
    night_light_set_brightness(1，80);
    /* 关闭灯 */
    night_light_set_brightness(0，80);
}
```

```
/*闪烁灯示例*/
void light_sample(void)
{
    /* 初始化 */
    blink_light_init();
    /* 闪烁一次 */
    blink_light_on();
}
```