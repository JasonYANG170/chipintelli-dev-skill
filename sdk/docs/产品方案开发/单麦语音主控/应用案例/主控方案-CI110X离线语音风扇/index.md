<!-- Source: https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E4%B8%BB%E6%8E%A7%E6%96%B9%E6%A1%88-CI110X%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E9%A3%8E%E6%89%87/ -->

请点击保存PDF文档

# 离线语音风扇主控方案

**新用户建议先查看 ☞[新手指南](https://document.chipintelli.com/%E6%96%B0%E6%89%8B%E6%8C%87%E5%8D%97/%E6%96%B0%E6%89%8B%E6%8C%87%E5%8D%97/)，了解开发的基础知识，再查看该文档。**

---

## 1 概述

离线语音在各类产品上的使用越来越多，这为原有的非智能产品增加了能听会说的“耳朵”和“嘴巴”，提升了用户体验。我司的智能语音芯片，既可以用串口和电控板对接的方式快速为产品增加语音功能，也可以作为部分产品的主控芯片，以兼具更小的空间及更低的成本。

该主控方案具备以下特点：

* 单芯片实现：采用我司的芯片直接进行相关的外设的控制，替换原来的MCU；
* IO资源多：每一代的芯片都具备20个以上的IO可以使用，方便控制各类外设；
* 降低成本：采用主控方案相比串口和电控通讯的方案，成本更进一步的降低。

本文以风扇为例，讲解主控方案的使用及注意事项：

![风扇实物图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E9%A3%8E%E6%89%87-1.png)

---

## 2 方案优势

我司离线语音风扇方案相对传统风扇方案，可让用户通过语音直接控制风扇设备，替换传统遥控器或按钮控制的方式，该方案语音部分的特点如下：

* 识别响应快：一般在0.2~0.8s内完成；
* 高识别率：识别率 > 95%；
* 高可靠性：芯片方案已通过双85，EMC测试等，可以满足格力、美的等家电巨头的高质量要求；
* 资料成熟：具有完整的软件、硬件、结构设计方案资料，可以为用户节省开发周期，降低开发难度；
* 便捷定制：可以采用我司语音AI平台进行便捷开发，可快速灵活定制唤醒词和命令词，不需用户提供语料；
* 超远识别：安静情况10米仍可良好识别。

一个语音风扇的控制使用效果图如下：

![语音控制风扇](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E9%A3%8E%E6%89%87-2.png)

---

## 3 应用描述

风扇产品按照使用电源类型可分为交流电风扇、直流电风扇和交直流电风扇三类，其类型可以分为台扇、吊扇、落地扇、无叶风扇等。我司离线语音风扇方案适用于各类风扇，其中尤其在落地扇上面的应用较多。特别注意，不同的风扇运行过程中，因为扇叶大小和叶片数量，风量大小差异，运行产生的风噪大小也有不小的差异，为了达到最好的语音识别效果，在设计风扇整机结构时，应该注意控制噪音小于65dB。

---

## 4 风扇主控方案介绍

​如果您是首次接触语音方案，建议先看产品设计完整流程 ☞[产品方案开发流程](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B/)。如果您是希望快速的完成整套方案，建议使用串口与电控通讯方案进行对接 ☞[串口与电控通讯的纯离线方案](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E8%AF%86%E5%88%AB/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/%E4%B8%B2%E5%8F%A3%E4%B8%8E%E7%94%B5%E6%8E%A7%E9%80%9A%E8%AE%AF%E6%96%B9%E6%A1%88-%E7%BA%AF%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3/)。

本部分内容重点介绍方案设计中的框图，模块及结构的注意事项。

### 4.1 主控方案

#### 4.1.1 方案特点

我司的CI110X系列芯片内置多个IO，可直接取代传统电控风扇中的单片机控制方案，实现单芯片完成语音控制和主控控制，整体成本较低，但该方案的开发需要一定的开发周期。为了方便用户使用，我司已提前做了相应的开发，可以提供完整的语音风扇主控软硬件方案。

#### 4.1.2 方案框图

![设计框图](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E9%A3%8E%E6%89%87-4.png)

整个方案如上图所示，CI110X系列芯片作为主控，可以通过麦克风采集外部语音进行识别，通过功放驱动喇叭播音。如为省成本，还可以将功放和喇叭换为蜂鸣器。CI110X系列芯片还可以接受外部按键及红外信号，兼容传统控制方式。

CI110X系列芯片可支持数码管显示，以及控制外部的驱动电路，实现电机的控制，可以很好的用一个单芯片实现风扇的全部控制功能。

#### 4.1.3 硬件设计参考

风扇电控板的一个硬件参考线路图及IO使用情况见下图，用户可以使用CI110X系列芯片按照相同的IO使用方式设计硬件电路，可以方便的搭建整套风扇方案。

![底板硬件设计](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E9%A3%8E%E6%89%87-5.png)

#### 4.1.4 主控方案SDK包说明

我司为用户基于上述硬件设计，提供了完整的SDK包，用户可以到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com) 上获取相关SDK开发包。

本SDK包有以下功能点：

* 控制方式：支持蓝牙、红外、按键、语音；
* 风扇模式：支持正常风、自然风、睡眠风、智能风；
* 摇头方式：支持左右摇头、上下摇头、3D摇头；
* 定时功能：支持定时关机功能，可按整数定时1 ~ 15小时。

本SDK包中使用到了硬件timer，使用方式如下：

1. 硬件TIMER0：转速电机控制（不归零码NRZ编码，使用一个IO脚）；
2. 硬件TIMER1：红外接收控制；
3. 硬件TIMER2：蓝牙接收控制（注意，蓝牙接收的数据和红外接收模块基本相同）；
4. 硬件TIMER3：蜂鸣器控制和摇头电机控制。

为了方便代码的移植，SDK包中每个外设驱动和风扇应用代码都是单独的文件夹，目录说明如下：

| 目录地址 | 目录说明 | 功能描述 |
| --- | --- | --- |
| ..\components\ir..\components\ir\_remote\_driver\ | 红外驱动目录（有些蓝牙也是单线通讯方式，与红外的驱动基本一致） | 红外接收功能，接收后的数据会队列通知到deal\_userdef\_msg，红外的长度可以根据宏定义配置 #宏定义 #define IR\_MAX\_DATA\_COUNT 120最大红外个数 |
| ..\components\motor\_driver\ | 电机驱动目录 | 当前支持5104的不归零码协议，后续如有其它驱动，我司可以支持 |
| ..\components\ci\_key\ | 使用了2个Touch IC芯片（如品腾PT8041），控制7个按键 | 触摸不同的按键，对应不同的adc数值，故使用两个ADC的输入IO做按键驱动，用户可根据实际产品自行调整 |
| ..\components\digital\_tube\ | 数码管驱动目录 | IO 与LED 进行复用，目前支持的是两个数码管+8个LED |
| ..\components\temperature\_sensor\ | 温度传感器驱动目录 | 使用ADC + 热敏电阻实现，产品可以根据温度进行智能调节风量 |
| ..\components\buzzer\ | 蜂鸣器驱动目录 | 采用定时器默认4Khz进行声音提示，可以更改 #define BUZZER\_HZ 进行配置，单位HZ |
| ..\sample\internal\sample\_1102\src\user\_msg\_deal.c | 风扇应用层代码 | 根据红外、按键、语音识别的消息进行电机的应用控制 |

**针对SDK中一些重要的变量和函数进行说明：**

**风扇状态变量：**

```
typedef struct
{
    unsigned short user_code : 4 ;//用户码固定位 b'1010*
    unsigned short positive : 2 ;//正转b'00 反转b'11*
    unsigned short sync_motor: 2 ;//同步电机1 停止b'00 同步电机停止b'10*
    unsigned short speed_bit: 8 ;//电机工作范围25~255 0表示电机停止*
    unsigned char mode; //0:正常风 1:自然风 2:睡眠风 3:智能风*
    short time; //0~15H*
    unsigned char led; //1开，0关*
    unsigned char head; //0关闭 1:左右 2:上下 3:3D*
    unsigned char gear; //0关闭 1~24*
}motor_status_struct_data;
```

**风扇初始化函数**

路径： ..\sample\internal\sample\_1102\src\user\_msg\_deal.c

```
/*\* @breif 风扇初始化函数**/
void fan_init(void)
{
//风扇状态初始化
  motor_status.user_code = 0xa;
  motor_status.sync_motor = 0x0;
  motor_status.speed_bit = 0x0;
  motor_status.positive = 0x00;
  motor_status.mode = 0;
  motor_status.head = 0;
  motor_status.led = 1;
  motor_status.gear = 0;
  motor_status.time = 0;
  /*风扇软件初始化*/
  extern void  user_fan_soft_init(void);
  user_fan_soft_init();

  //电机初始化
  agreement_5104_app_init();
  //红外初始化
  extern void ir_test_recive(void);
  ir_test_recive();
  //蓝牙初始化
  extern void bt_test_recive(void);
  bt_test_recive();
  //按键初始化
  extern void ci_key_init(void);
  ci_key_init();
  //数码管初始化
  extern int digital_tube_init(void);
  digital_tube_init();
  //蜂鸣器初始化
  buzzer_init();

  //同步电机2初始化
  sync_motor2_hw_init();

  //温度传感器初始化
  temp_sensor_init();
}
```

**风扇应用任务处理**

语音入口和标准SDK一致无改动，蓝牙、语音、按键、红外通过队列sys\_msg\_queue发送消息到UserTaskManageProcess任务。相关控制处理入口入下图所示例：

```
uint32_t deal_userdef_msg(sys_msg_t *msg)
{
    uint32_t ret = 1;
    switch(msg->msg_type)
    {
    /* 按键消息 */
        case SYS_MSG_TYPE_KEY:
        {
            sys_msg_key_data_t *key_rev_data;
            key_rev_data = &msg->msg_data.key_data;
            userapp_deal_key_msg(key_rev_data);
            break;
        }
        /* 红外消息 */
        case SYS_MSG_TYPE_IR:
        {
            sys_msg_ir_data_t *ir_rev_data;
            ir_rev_data = &msg->msg_data.ir_data;
            userapp_deal_ir_msg(ir_rev_data);
            break;
        }
        /* 蓝牙消息*/
        case SYS_MSG_TYPE_BT:
        {
            sys_msg_bt_data_t *bt_rev_data;
            bt_rev_data = &msg->msg_data.bt_data;
            userapp_deal_bt_msg(bt_rev_data);
            break;
        }
        #if MSG_COM_USE_UART_EN
        /* CI串口协议消息 */
        case SYS_MSG_TYPE_COM:
        {
            sys_msg_com_data_t *com_rev_data;
            com_rev_data = &msg->msg_data.com_data;
            userapp_deal_com_msg(com_rev_data);
            break;
        }
        #endif
        /* CI IIC 协议消息 */
        #if MSG_USE_I2C_EN
        case SYS_MSG_TYPE_I2C:
        {
            sys_msg_i2c_data_t *i2c_rev_data;
            i2c_rev_data = &msg->msg_data.i2c_data;
            userapp_deal_i2c_msg(i2c_rev_data);
            break;
        }
        #endif
        default:
        break;
    }
    return ret;
}
```

---

## 5 产品结构相关注意事项

### 5.1 麦克风结构设计

风扇产品在设计结构时，针对麦克风应当注意以下事项：

* 麦克风布局尽量远离风扇的风面及电机；
* 麦克风放在风扇的正面/顶部/底部/的位置避免遮挡，保证拾音最大范围；
* 分贝仪测试最大风速下，麦克风处的稳态噪声需小于60dB；
* 麦克风连线较长可选择带屏蔽的线；
* 麦克风线扎捆远离电机或电控线。

下图是以落地风扇为例的麦克风结构开孔及位置的建议方式，固定麦克风的结构件也需要考虑麦克风线是否方便引出导线和避免干扰。

![风扇结构](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E9%A3%8E%E6%89%87-11.png)

### 5.2 喇叭结构设计

风扇产品在设计结构时，针对喇叭应当注意以下事项：

* 考虑到音质，喇叭功率不能太小，建议选用1W以上喇叭，带音腔的喇叭效果会更好一点，同时注意硬件板上的功放驱动能力要匹配喇叭型号；
* 喇叭放置位置以美观及结构布局方便为主，但注意尽量避免和麦克风放在同一位置；
* 不带音腔喇叭可以在结构上做一个音腔密封后能增大音量输出。

下图是一个喇叭规格及开孔的建议。

![喇叭结构](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E9%A3%8E%E6%89%87-12.png)

### 5.3 其它注意事项

* 建议选择-32dB±3dB，信噪比大于70dB以上，性价比高的全向模拟麦克风；
* 若风扇旋转震动比较大，选择较厚的麦克风胶套减缓震动，可减小对识别效果的影响；
* 某些显控板在底盘上的落地风扇，或类似易接触水或积累灰尘的风扇，需要选择防水防尘的麦克风；
* 特别注意禁止直接点热溶胶用于固定麦克风头；
* 将麦克风完全插入固定孔内部，避免歪斜，拾音孔中心孔对准麦克风的中心；
* 风扇内部接线多容易扯松，插头配插座带卡扣会更牢固；
* 选择符合环保RoHS要求的RTV硅胶，推荐常用的硅胶有703/704/737等，或其它有机材料、单组分室温固化硅胶等；
* 多数硅橡胶灌胶厚度小于3mm以内实温完全固化时间为8 ~ 12小时，灌胶厚度 > 3mm以上完全固化时间更长，可分层多次浇灌，确保完全固化稳定密封；
* 生产工艺可把固定麦克风工序提前，另外特别注意麦克风接线在重力拉扯下，未完全固化前容易导致麦克风扯歪，也需注意将麦克风插线固定好。

![选型及注意](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%96%B9%E6%A1%88%E5%BC%80%E5%8F%91/%E5%8D%95%E9%BA%A6%E8%AF%AD%E9%9F%B3%E4%B8%BB%E6%8E%A7/%E5%BA%94%E7%94%A8%E6%A1%88%E4%BE%8B/img/%E7%A6%BB%E7%BA%BF%E8%AF%AD%E9%9F%B3%E9%A3%8E%E6%89%87-13.png)

---

## 6 相关参考资料列表

| 编号 | 参考资料描述 | 获取方式 |
| --- | --- | --- |
| 1 | 标准离线SDK CI110X\_SDK\_ASR\_Offline | 请到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com) 上获取 |
| 2 | 风扇SDK Demo： 主控方案SDK 串口通讯方案SDK | 请到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com) 上获取 |
| 3 | 测试固件 | 到 ☞[启英泰伦语音AI平台](https://aiplatform.chipintelli.com) 上获取 |
| 4 | 我司芯片和模块的选型指南 | 请点击 ☞[硬件选型指南](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E7%A1%AC%E4%BB%B6%E9%80%89%E5%9E%8B%E6%8C%87%E5%8D%97/) 查看 |
| 5 | 我司芯片硬件设计方法参考 | 请点击 ☞[硬件设计参考](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E7%A1%AC%E4%BB%B6%E8%AE%BE%E8%AE%A1%E5%8F%82%E8%80%83/) 查看 |
| 6 | 麦克风和喇叭器件选型 | 请点击 ☞[外围器件兼容列表](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%A4%96%E5%9B%B4%E5%99%A8%E4%BB%B6%E5%85%BC%E5%AE%B9%E5%88%97%E8%A1%A8/) 查看 |
| 7 | 产品结构设计 | 请点击 ☞[产品结构设计](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/) 查看 |
| 8 | 软件开发方法和标准SDK说明 | 请点击 ☞[软件开发](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E6%A6%82%E8%BF%B0/) 查看各部分内容 |
| 9 | 产品识别性能测试方法 | 请点击 ☞[识别效果测试](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E8%AF%86%E5%88%AB%E6%95%88%E6%9E%9C%E6%B5%8B%E8%AF%95%E6%A0%87%E5%87%86/) 查看 |
| 10 | 生产测试的方法 | 请点击 ☞[生产测试](https://document.chipintelli.com/%E4%BA%A7%E5%93%81%E6%B5%8B%E8%AF%95/%E7%94%9F%E4%BA%A7%E6%B5%8B%E8%AF%95/) 查看 |

用户如遇到使用问题，请到启英泰伦语音AI平台上提交技术工单，或直接联系我司技术人员获取协助。