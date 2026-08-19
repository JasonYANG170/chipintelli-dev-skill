<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/components/key/ -->

# 按键(KEY)

---

## 1. 概述

* 实现了ADC按键，GPIO按键，触摸（外挂adc接口的touch芯片）按键的三种参考；
* 统一的按键消息接口，方便应用部分代码和底层具体按键隔离；
* 参考代码内具体使用的ADC，GPIO资源方便快速修改适应不同板子。

---

## 2. 使用说明

| 函数接口 | 说明 |
| --- | --- |
| ci\_key\_init | 初始化按键组件 |
| key\_info\_show | 打印按键的各个消息 |
| ci\_key\_gpio\_init | gpio 按键初始化，初始化IO和软件定时器，如需修改使用不同的IO，修改gpio\_key\_list即可 |
| ci\_key\_gpio\_isr\_handle | gpio 按键中断处理函数，如果使用GPIO按键功能，需要添加到gpio中断内 |
| ci\_key\_adc\_init | ADC按键初始化，初始化ADC硬件，如需修改按键，修改adc\_key\_info，ADC\_KEY\_CHANNEL，ADC\_KEY\_PAD |
| ci\_adc\_key\_isr\_handle | ADC 按键中断处理函数，如果使用ADC按键，需要将该函数放到adc中断处理函数里面，处理去抖，按键按下，长按，短按 |
| ci\_key\_touch\_init | touch 按键初始化，使用ADC和GPIO中断，修改ADC\_KEY\_CHANNEL，TOUCH\_ISR\_PAD相关宏定义即可 |
| ci\_key\_touch\_gpio\_isr\_handle | touch 按键GPIO中断处理函数，需要添加到GPIO中断内 |
| ci\_key\_touch\_adc\_isr\_handle | touch 按键ADC中断处理函数，需要添加到ADC中断内 |

### 2.1. 使用示例

sample工程里UserTaskManageProcess任务里面可以接受按键消息，用户只需要在void userapp\_deal\_key\_msg(sys\_msg\_key\_data\_t \*key\_msg)函数里面对按键进行处理。

下面以GPIO按键为例：

```
void ci_key_init(void)
{
    /*gpio 按键初始化*/
    ci_key_gpio_init();
}

ci_key_init();//初始化按键，只调用一次

//用户处理按键消息的函数
void userapp_deal_key_msg(sys_msg_key_data_t  *key_msg)
{
    if(key_msg->key_index != KEY_NULL)
    {
        ci_loginfo(LOG_USER,"key_value is 0x%x ",key_msg->key_index);

        //按键被按下
        if(MSG_KEY_STATUS_PRESS == key_msg->key_status)
        {
            ci_loginfo(LOG_USER,"status : press down\n");
        }

        //按键处于长按状态
        else if(MSG_KEY_STATUS_PRESS_LONG == key_msg->key_status)
        {
            ci_loginfo(LOG_USER,"status : long press\n");
        }

        //按键被释放
        else if(MSG_KEY_STATUS_RELEASE == key_msg->key_status)
        {
            ci_loginfo(LOG_USER,"status : release\n");
        }

        //按键被按下的时间
        if(key_msg->key_status == MSG_KEY_STATUS_RELEASE)
        {
            ci_loginfo(LOG_USER,"按键时间为：%d ms\n",key_msg->key_time_ms);
        }
        /*user code*/                
    }
}

//GPIO中断处理函数，放到实际的GPIO处理函数

//正确使用
void GPIO3_IRQHandler(void)
{
    ci_key_gpio_isr_handle();//GPIO按键处理函数

    if(gpio_get_irq_mask_status_single(GPIO3,gpio_pin_0))
    {
        gpio_clear_irq_single(GPIO3,gpio_pin_0);
        ci_loginfo(LOG_GPIO_DRIVER,"GPIO%d Pin%d IRQ\n", 3, 0);
    }

}

//错误使用
void GPIO3_IRQHandler(void)
{
    if(gpio_get_irq_mask_status_single(GPIO3,gpio_pin_0))
    {
        gpio_clear_irq_single(GPIO3,gpio_pin_0);
        ci_loginfo(LOG_GPIO_DRIVER,"GPIO%d Pin%d IRQ\n", 3, 0);
        ci_key_gpio_isr_handle();//GPIO按键处理函数
    }

}
```

提示

按键的定义需要结合板子来定，具体修改点可参考代码内文件开始的宏定义，然后需要将ISR处理函数添加到中断处理内。