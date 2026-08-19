#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "system_msg_deal.h"
#include "prompt_player.h"
#include "voice_module_uart_protocol.h"
#include "i2c_protocol_module.h"
#include "ci_nvdata_manage.h"
#include "ci_log.h"
#include "ci130x_gpio.h"
#include "voice_print_recognition.h"
#include "device.h"
#include "ci130x_dpmu.h"
#include "user_msg_deal.h"
#include "cias_network_msg_protocol.h"
#include "cias_demo_config.h"
#include "cias_voice_upload.h"
#include "cias_aiot_protocol.h"
// #include "all_cmd_statement.h"

///tag-insert-code-pos-1


#define I2C_SLAVE_ADDR 0x13 /*!< I2C slave address */
#define CHIP_ID 0x1D        // 读出来默认为0x5E
#define TURN_LEFT 0
#define TURN_RIGHT 1
#define CIRCLE_PULSE 8192 // 转一圈所需的脉冲数

#define KEY_CHECK_INTERVAL 5000       // 按键检测间隔
#define KEY_CHECK_COUNT 30            // 按键连续检测次数
#define KEY_CHECK_LEVEL 0             // 按键检测的目标电平
#define KEY_CHECK_SUCCESS_SET_LEVEL 0 // 按键检测成功后，需设置PD4的电平

QueueHandle_t uart_msg_queue = NULL; // 用于传递串口协议消息的队列
// static uart_msg_t uart_send_msg_1 = {0};

// 注！！！这几条放其他文件夹导致一直因ld错误而编译失败，巨坑
static bool uart_send_wakeup_protocol_flag = false;                                                // 串口发送唤醒协议标志位,用于后续接收回复指令
const static uint8_t wakeup_protocol[8] = {0xAA, 0XBB, 0XF0, 0X01, 0X00, 0X01, 0X0A, 0X5F};        // 检验位已算好直接发送
const static uint8_t segmenting_protocol[8] = {0xAA, 0XBB, 0XF0, 0X01, 0X00, 0X02, 0X0B, 0X1F};    // 检验位已算好直接发送
const static uint8_t uart_protocol_responce[8] = {0xBB, 0XAA, 0X00, 0X00, 0X00, 0X00, 0X48, 0X03}; // AD35接收成功回复指令，进入聊天界面后再发不会有回复

extern CiasAiotFuncParamTypedef gCiasAiotFuncParam;
/////////////////////////////////////////////////gpio模块-始///////////////////////////////////////////////
// 用于按键检测，当前指定为PD3，PD4,PD引脚没有中断功能
void gpio_key_init(void)
{
    // PD3 输入
    scu_set_device_gate((unsigned int)PD, ENABLE); // 开启PB时钟
    dpmu_set_io_reuse(PD3, FIRST_FUNCTION);        // 设置引脚功能复用为GPIO
    dpmu_set_adio_reuse(PD3, DIGITAL_MODE);        // 初始化为数字功能
    dpmu_set_io_direction(PD3, 0);                 // 设置引脚功能为输入模式 DPMU_IO_DIRECTION_INPUT
    dpmu_set_io_pull(PD3, 1);                      // 设置关闭上下拉 DPMU_IO_PULL_DISABLE(0), DPMU_IO_PULL_UP(1), DPMU_IO_PULL_DOWN(2)
    gpio_set_input_mode(PD, pin_3);                // GPIO的pin脚配置成输入模式

    // PD4 输入
    scu_set_device_gate((unsigned int)PD, ENABLE); // 开启PB时钟
    dpmu_set_io_reuse(PD4, FIRST_FUNCTION);        // 设置引脚功能复用为GPIO
    dpmu_set_adio_reuse(PD4, DIGITAL_MODE);        // 初始化为数字功能
    dpmu_set_io_direction(PD4, 0);                 // 设置引脚功能为输入模式 DPMU_IO_DIRECTION_INPUT
    dpmu_set_io_pull(PD4, 1);                      // 设置关闭上下拉 DPMU_IO_PULL_DISABLE
    gpio_set_input_mode(PD, pin_4);                // GPIO的pin脚配置成输出模式
}

void gpio_mute_init(void)
{
    scu_set_device_gate(PD, ENABLE);                      // 开启GPIOA时钟
    dpmu_set_io_reuse(PD0, FIRST_FUNCTION);               // 初始化为GPIO功能
    dpmu_set_adio_reuse(PD0, DIGITAL_MODE);               // 初始化为数字功能
    dpmu_set_io_direction(PD0, DPMU_IO_DIRECTION_OUTPUT); // 初始化引脚为输出模式
    gpio_set_output_mode(PD, pin_0);                      // 初始化GPIOA，的pin_0为输出模式
    gpio_set_output_low_level(PD, pin_0);                 // 输出高电平
}

////////////////////////////////////////////gpio模块-终///////////////////////////////////////////////

/////////////////////////////////////////////////IIC电机模块-始///////////////////////////////////////////////
/**
 * 注：api没有传寄存器地址的参数，但是可以把寄存器地址写在buf[0]中，要写的值放在buf[1]中，以此实现读写寄存器的功能
 * @brief 给寄存器写入一个字节数据
 *
 * @param regaddr 寄存器地址
 * @param val 需要写入的值
 */
void i2c_write_reg(uint8_t regaddr, uint8_t val)
{
    uint8_t buf[2] = {0};
    buf[0] = regaddr;
    buf[1] = val;

    i2c_master_only_send(I2C_SLAVE_ADDR, buf, 2);
    //    _delay_10us_240M(5000);
}

/**
 * @brief 读取某个寄存器的值
 *
 * @param regaddr 寄存器地址
 * @return int 读出寄存器的值
 */
uint8_t i2c_read_reg(uint8_t regaddr)
{
    uint8_t buf[2] = {0};
    buf[0] = regaddr;

    i2c_master_send_recv(I2C_SLAVE_ADDR, buf, 1, 1);
    //_delay_10us_240M(5);
    return buf[0];
}

// 读取电机运行状态
bool Read_A_act(void)
{
    uint8_t ret = false;
    ret = i2c_read_reg(0x1c);
    uint8_t temp = ret & 0x01;
    mprintf("ret 0x%02x\n\n", ret);
    if (temp == 0x01) {
        return true;    // 正在执行旋转
    } else {
        return false;
    }
}

void Motor_A_speed(unsigned int speed_A)
{
#if 1 // 通过传入时间参数来设置速度
    int speed = 24973000 / 128 / (8192 / 2) * speed_A;
    mprintf("设置speed_A = [%d]s, speed = %d\n", speed_A, speed);
    speed = speed & 0x3fff;
    i2c_write_reg(0x03, speed >> 6);
    i2c_write_reg(0x02, speed << 2);
#else
    mprintf("设置speed_A = %d\n", speed_A);
    speed_A = speed_A & 0x3fff;
    i2c_write_reg(0x03, speed_A >> 6);
    i2c_write_reg(0x02, speed_A << 2);
#endif
}

void GC61XX_Config(void)
{
    // mprintf("\r\nGC61XX_Config\r\n");
    vTaskDelay(50 / portTICK_PERIOD_MS); // RC复位时等待rest管脚电压稳定

    // init start
    i2c_write_reg(0x17, 0x00); // STB=0 输出端口初始化为高阻态，STM_RS=0 清零计步寄存器，CMD_RS=0 初始化寄存器
    vTaskDelay(1 / portTICK_PERIOD_MS);
    i2c_write_reg(0x17, 0x1f); // STB=1，STM_RS=1，CMD_RS=1   //端口非高阻态，寄存器初始化完成

    // 配置1~4ch的通道为自动模式Autonomous
    i2c_write_reg(0x12, 0x10); // A/B_CTL=0 (Autonomous mode), A/B_ANSEL=1 (UPDW) 选择加减速模式，Motor_SEL=0 选两相四线电机
    i2c_write_reg(0x13, 0x11); // PWM斩波频率设置：chopping=01 fmain/256，chopping=10 fmain/384，chopping=11 fmain/512
    i2c_write_reg(0x14, 0x1f); // CacheM=1 选用1个寄存器, FCLKTRIM[4:0] 设置fmain（用于主逻辑的时钟）。
                               // 当使用内部振荡器做为主时钟时，可以设置这个寄存器来调节主时钟的频率。上电或复位后默认值为10000

    // u-step,output signal from STATE pin
    /*改A/B_Mode输出波形有三种：00=微步（正弦波）, 01=1/2（凸波形）, 10=全步（矩形）*/
    i2c_write_reg(0x00, 0x00); // A_MODE=00=微步模式,A_Sel=000=4细分
    i2c_write_reg(0x09, 0x00); // B_MODE=00=微步模式,A_Sel=000=4细分

    i2c_write_reg(0x01, 0x7f); // AchDOV=111 1111 A通道电流设定
    i2c_write_reg(0x0a, 0x7f); // BchDOV=111 1111 B通道电流设定

    // set the frequency for stepping motor rotation设置步进电机旋转的频率
    i2c_write_reg(0x03, 0x04); // 8 A_Cycel[13:6] 转速的高8位，高位调节对电机的转速起作用且设定的数值越小转速越快，数值越大转速越慢
    i2c_write_reg(0x02, 0x08); // 8 A_Cycel[5:0] 低6位   // UPDW模式 ACycle<5:0>=000010 00==频率系数4
                               // 改变的是整个加减速的长度  000->1, 001->2, 010->4, 011->8, 100->16

    // set the frequency for stepping motor rotation
    i2c_write_reg(0x0c, 0x04); // 8 B_Cycel[13:6] 转速的高8位，高位调节对电机的转速起作用且设定的数值越小转速越快，数值越大转速越慢
    i2c_write_reg(0x0b, 0x08); // 8 B_Cycel[5:0] 低6位   // UPDW模式 BCycle<5:0>=000010 00==频率系数4
                               // 改变的是整个加减速的长度  000->1, 001->2, 010->4, 011->8, 100->16

    // set on of the pre/post-excitation,set time of the pre/post-excitation
    i2c_write_reg(0x05, 0x08); // A_BEXC=0, A_AEXC=1    // 后励磁置1,跑完后自动关电流
    i2c_write_reg(0x0e, 0x08); // B_BEXC=0, B_AEXC=1    // 后励磁置1

    // set A_Start_POS
    i2c_write_reg(0x04, 0x00); // A_Start_POS[1:0]=00
    i2c_write_reg(0x0d, 0x00); // B_Start_POS[1:0]=00

    // set power on of the stepping motor driver通道开启转动必须配置这个
    i2c_write_reg(0x06, 0x02); // A_POS[1:0]=00, A_PS=1 设置步进电机驱动器的电源开关   PS不能用移位赋值
    i2c_write_reg(0x0f, 0x02); // B_POS[1:0]=00, B_PS=1 设置步进电机驱动器的电源开关

    //******直流驱动设置*******/
    //    i2c_write_reg(0x15,0xff);  // 5_ PWM_Duty[6:0]=111 1111 控制5ch的输出占空比
    i2c_write_reg(0x16, 0x03); // 5_ PWM 斩波频率设置，刹车
}

void Motor_A_Run(unsigned int a_dir, unsigned int pulseA)
{
    a_dir = a_dir & 0x01;
    pulseA = pulseA & 0x3fff;
    i2c_write_reg(0x07, pulseA >> 8 | 1 << 7 | a_dir << 6);
    i2c_write_reg(0x08, pulseA);
}

// 传入方向和角度，转动电机
void Motor_A_Run_angle(unsigned int a_dir, int angle)
{
#if ENABLE_MOTOR_DOA
    static int cnt = 0;

    if (angle >= 0 && angle <= 180)
    {
        if (angle - 90 > 0)
        {
            a_dir = TURN_RIGHT;
        }
        else
        {
            a_dir = TURN_LEFT;
        }

        int pulseA = abs(angle - 90) / 360.0 * CIRCLE_PULSE;

        Motor_A_Run(a_dir, pulseA);
        mprintf("A_dir:%d, pulseA:%d, angle:%d\n", a_dir, pulseA, angle);
    }
    else
    {
        mprintf("角度超出范围, angle:%d\n", angle);
    }
#else
    mprintf("Motor_A_Run_angle not enable\n");
#endif
}

void Motor_A_STOP(void)
{
    i2c_write_reg(0x06, 0x03); // 强制停转电机   POS 设为 00, 01, 10, 11 分别停在 1/16, 1-2, 2-2, 1 位置
}

// 硬件iic，电机初始化，如果成功打印出chip_id为0x5E就说明iic通信成功
void iic_motor_init(void)
{
    mprintf("\n\niic_motor_init start\n\n");
    uint8_t chip_id = 0;
    int pulse = 0;

    // iic_software_init();
    GC61XX_Config(); // 初始化GC61XX电机驱动器

    chip_id = i2c_read_reg(CHIP_ID);
    mprintf("\n\niic motor chip_id: 0x%x\n\n", chip_id);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
}
/////////////////////////////////////////////////IIC电机模块-终///////////////////////////////////////////////

/////////////////////////////////////////////////串口协议模块-始///////////////////////////////////////////////
// 自定义串口协议，0x01为唤醒协议，0x02为断句协议
void uart_send_wakeup_protocol(uint8_t protocol_type)
{
    UART_TypeDef *UART_PORT;

#if (UART_PROTOCOL_NUMBER == HAL_UART0_BASE)
    UART_PORT = UART0;
#elif (UART_PROTOCOL_NUMBER == HAL_UART1_BASE)
    UART_PORT = UART1;
#elif (UART_PROTOCOL_NUMBER == HAL_UART2_BASE)
    UART_PORT = UART2;
#endif

    if (protocol_type == 0x01)
    {
        for (int i = 0; i < sizeof(wakeup_protocol); i++)
        {
            UartPollingSenddata(UART_PORT, wakeup_protocol[i]);
        }
    }
    else if (protocol_type == 0x02)
    {
        for (int i = 0; i < sizeof(segmenting_protocol); i++)
        {
            UartPollingSenddata(UART_PORT, segmenting_protocol[i]);
        }
    }
    uart_send_wakeup_protocol_flag = true;
}

// 长按按键PD3 KEY_CHECK_COUNT * KEY_CHECK_INTERVAL 后，设置pd4 KEY_CHECK_SUCCESS_SET_LEVEL
void key_check_task(void *arg)
{
    static int level_3 = -1;
    int check_cnt = 0; // 用于等待按键按下计数器

    gpio_key_init();                       // 初始化GPIO PD3 PD4作按键检测
    vTaskDelay(1000 / portTICK_PERIOD_MS); // 等待1s，确保初始化完成

    while(1) {
        if (KEY_CHECK_LEVEL == gpio_get_input_level_single(PD, pin_3)) {
            mprintf("PD3 level:%d, 充电中。。。\n", KEY_CHECK_LEVEL);
        }

        if (KEY_CHECK_LEVEL == gpio_get_input_level_single(PD, pin_4)) {
            mprintf("PD4 level:%d, 充电完成。。。\n", KEY_CHECK_LEVEL);
        }

        vTaskDelay(KEY_CHECK_INTERVAL / portTICK_PERIOD_MS); // 延时100ms
    }

    // while (1)
    // {
    //     if (KEY_CHECK_LEVEL == gpio_get_input_level_single(PD, pin_3))
    //     {
    //         check_cnt++;
    //     }
    //     else
    //     {
    //         check_cnt = 0; // 清空计数
    //     }

    //     if (check_cnt >= KEY_CHECK_COUNT) // 等待
    //     {
    //         static int level = 0;
    //         static int cnt = 0;
    //         level = (cnt++) % 2;

    //         gpio_set_output_level_single(PD, pin_4, level);  // KEY_CHECK_SUCCESS_SET_LEVEL
    //         mprintf("检测到按键PD3连续[%d]ms为[%d], 设置pd4为[%d]\n", KEY_CHECK_COUNT * KEY_CHECK_INTERVAL, KEY_CHECK_LEVEL, level);
    //         check_cnt = 0; // 清空计数
    //         // 输出之后是否还要做计数清零处理？关机？
    //     }

    //     vTaskDelay(KEY_CHECK_INTERVAL / portTICK_PERIOD_MS); // 延时100ms
    // }

    vTaskDelete(NULL);
}

// 串口通信任务，用于处理串口协议数据，包括发送和接收，用消息队列处理
// void uart_protocol_task(void *arg)
// {
//     uart_msg_t recv_msg = {0};
//     bool wait_for_send_response_flag = false;
//     int cnt = 0; // 用于等待回复的计数器

//     while (1)
//     {
//         if (xQueueReceive(uart_msg_queue, &recv_msg, 100) == pdTRUE)
//         {
//             // mprintf("[%s] recv_msg.type:%d\n", __func__, recv_msg.type);    // debug
//             switch (recv_msg.type)
//             {
//             case UART_SEND_WAKEUP:
//                 uart_send_wakeup_protocol(0x01);
//                 wait_for_send_response_flag = true;
//                 mprintf("[%s] 发送唤醒协议\n", __func__); // debug
//                 break;
//             case UART_SEND_SEGMENT:
//                 uart_send_wakeup_protocol(0x02);
//                 wait_for_send_response_flag = true;
//                 mprintf("[%s] 发送断句协议\n", __func__); // debug
//                 break;
//             case UART_RECV_RESPONSE: // 回复消息应该区分是哪个协议的回复
//                 wait_for_send_response_flag = false;
//                 mprintf("[%s] 收到回复消息\n", __func__); // debug
//                 break;
//             default:
//                 mprintf("[%s] 收到异常消息类型, recv_msg.type:%d\n", __func__, recv_msg.type); // debug
//                 break;
//             }
//         }

//         cnt++;
//         if (cnt >= 5 && wait_for_send_response_flag == true)
//         {
//             mprintf("[%s] 等待回复超时\n", __func__); // debug
//             uart_send_wakeup_protocol(0x01);
//             cnt = 0;
//         }
//     }

//     // vTaskDelay(2000 / portTICK_PERIOD_MS);

//     vTaskDelete(NULL);
// }

/////////////////////////////////////////////////串口协议模块-终///////////////////////////////////////////////

/**
 * @brief 用户初始化
 *
 */
void userapp_initial(void)
{
    #if CPU_RATE_PRINT
    init_timer3_getresource();
    #endif

    #if MSG_COM_USE_UART_EN
    #if (UART_PROTOCOL_VER == 1)
    uart_communicate_init();
    #elif (UART_PROTOCOL_VER == 2)
    vmup_communicate_init();
    #elif (UART_PROTOCOL_VER == 255)
    UARTInterruptConfig((UART_TypeDef *)UART_PROTOCOL_NUMBER, UART_PROTOCOL_BAUDRATE);
    #endif
    #endif

    #if MSG_USE_I2C_EN
    i2c_communicate_init();
    #endif
    #if USE_IR_ENABEL   //红外任务初始化
    ir_task_init(); 
    get_flash_eut_state();   //获取下自动测试状态
    xTaskCreate(DeviceTaskProcess, "DeviceTaskProcess", 300, NULL, 4, NULL);
    #endif

    ///tag-gpio-init
#if ENABLE_MOTOR_DOA
    iic_motor_init();
    gpio_mute_init();
    xTaskCreate(key_check_task, "key_check_task", 128, NULL, 5, NULL);   // 按键检测功能
#endif
}

/**
 * @brief 处理按键消息（目前未实现该demo）
 *
 * @param key_msg 按键消息
 */
void userapp_deal_key_msg(sys_msg_key_data_t  *key_msg)
{
    (void)(key_msg);
}



/**
 * @brief 按语义ID响应asr消息处理
 *
 * @param asr_msg
 * @param cmd_handle
 * @param semantic_id
 * @return uint32_t
 */
uint32_t deal_asr_msg_by_semantic_id(sys_msg_asr_data_t *asr_msg, cmd_handle_t cmd_handle, uint32_t semantic_id)
{
    uint32_t ret = 1;
    if (PRODUCT_GENERAL == get_product_id_from_semantic_id(semantic_id))
    {
        uint8_t vol;
        int select_index = -1;
        switch(get_function_id_from_semantic_id(semantic_id))
        {
        case VOLUME_UP:        //增大音量
            vol = vol_set(vol_get() + 1);
            select_index = (vol == VOLUME_MAX) ? 1:0;
            break;
        case VOLUME_DOWN:      //减小音量
            vol = vol_set(vol_get() - 1);
            select_index = (vol == VOLUME_MIN) ? 1:0;
            break;
        case MAXIMUM_VOLUME:   //最大音量
            vol_set(VOLUME_MAX);
            break;
        case MEDIUM_VOLUME:  //中等音量
            vol_set(VOLUME_MID);
            break;
        case MINIMUM_VOLUME:   //最小音量
            vol_set(VOLUME_MIN);
            break;
        case TURN_ON_VOICE_BROADCAST:    //开启语音播报
            prompt_player_enable(ENABLE);
            break;
        case TURN_OFF_VOICE_BROADCAST:    //关闭语音播报
            prompt_player_enable(DISABLE);
            break;
        default:
            ret = 0;
            break;
        }
        if (ret)
        {
            #if PLAY_OTHER_CMD_EN && !WMAN_PLAY_EN
            #if MULT_INTENT > 1
            prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback,false);
            #else
            prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback,true);
            #endif
            #endif
        }
    }
    else
    {
        ret = 0;
    }
    return ret;
}

#if USE_VPR
bool vpr_delete_tmp_flag = 0;
bool vpr_delete_all_flag = 0;
#endif

/**
 * @brief 按命令词id响应asr消息处理
 *
 * @param asr_msg
 * @param cmd_handle
 * @param cmd_id
 * @return uint32_t
 */
extern CiasAiotRunParamTypedef gCiasAiotRunParam;
uint32_t deal_asr_msg_by_cmd_id(sys_msg_asr_data_t *asr_msg, cmd_handle_t cmd_handle, uint16_t cmd_id)
{
    uint32_t ret = 1;
    int select_index = -1;
    uint8_t vol;
    mprintf("---cmd_id = %d\r\n", cmd_id);
    switch (cmd_id)
    {
    case 103:
        mprintf("start config net....\r\n");
        break;
    case 3: // 增大音量
        mprintf("set vol add..\r\n");
        vol = vol_set(vol_get() + 1);
        select_index = (vol == VOLUME_MAX) ? 1 : 0;
        break;
    case 4: // 减小音量
        mprintf("set vol sub..\r\n");
        vol = vol_set(vol_get() - 1);
        select_index = (vol == VOLUME_MIN) ? 1 : 0;
        break;
    case 5: // 最大音量
        mprintf("set vol max..\r\n");
        vol_set(VOLUME_MAX);
        break;
    case 6: // 最小音量
        mprintf("set vol min..\r\n");
        vol_set(VOLUME_MIN);
        break;
    case 9:
        mprintf("set interaction_multi_round 1\r\n");
        gCiasAiotFuncParam.interaction_multi_round = 1;
        break;
    case 10:
        mprintf("set interaction_multi_round 0\r\n");
        gCiasAiotFuncParam.interaction_multi_round = 0;
        break;
    case 100:   //退出对话 
        mprintf("cmd exit wakeup....\r\n");
        set_state_exit_wakeup();
        gCiasAiotRunParam.request_play_data_flag = false;
        gCiasAiotRunParam.play_cloud_data_flag = false;
        gCiasAiotRunParam.rcv_cloud_play_data_flag = false;
        gCiasAiotRunParam.cloud_play_data_total_len = 0;
        gCiasAiotRunParam.stop_collect_pcm_flag = false;
    break;     
    ///tag-asr-msg-deal-by-cmd-id-end
    default:
        ret = 0;
        break;
    }
#if CIAS_BLE_CONNECT_MODE_ENABLE
    deal_ble_send_msg(cmd_id);
#endif
    if (ret && select_index >= -1)
    {
        #if PLAY_OTHER_CMD_EN && !WMAN_PLAY_EN
        #if MULT_INTENT > 1
        prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback,false);
        #else
        prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback,true);
        #endif
        #endif
    }

    return ret;
}

/**
 * @brief 用户自定义消息处理
 *
 * @param msg
 * @return uint32_t
 */
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
    #if MSG_COM_USE_UART_EN
    /* CI串口协议消息 */
    case SYS_MSG_TYPE_COM:
    {
		#if ((UART_PROTOCOL_VER == 1) || (UART_PROTOCOL_VER == 2))
    	sys_msg_com_data_t *com_rev_data;
        com_rev_data = &(msg->msg_data.com_data);
        userapp_deal_com_msg(com_rev_data);
        #endif
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



