

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "sdk_default_config.h"
#include "ci13lc.h"
#include "ci13lc_gpio.h"
#include "ci13lc_dpmu.h"
#include "ble_communicate.h"
#include "ci_log.h"
#include "user_config.h"

#define BLE_MODE_BQB_TEST 0
//蓝牙连接状态
ble_status_t ble_status = BLE_STATUS_DISCONNECT;
//蓝牙配对状态
ble_pairing_state_t ble_pairing_status = BLE_PAIRING_STATE_IDLE;

//蓝牙设置指令回复的操作码
volatile uint8_t ble_recv_cmd = 0xff;
//添加蓝牙服务成功返回的handle
volatile uint8_t ble_handle = 0xff;

//处理蓝牙设置指令回复的信号量
static SemaphoreHandle_t ble_recv_ack_mutex = NULL;
//处理蓝牙发送数据的信号量
static SemaphoreHandle_t ble_send_packet_mutex = NULL;
//接收蓝牙回复数据帧
static ble_msg_data_t ble_recever_packet;

/**
 * @brief   蓝牙串口通信超时函数，蓝牙一帧通讯超时，串口接收状态机回到初始状态
 */
static bool ble_port_timeout_one_packet(void)
{
    static TickType_t ble_last_time;
    TickType_t now_time;
    TickType_t timeout;
    
    now_time = xTaskGetTickCountFromISR();
    timeout = (now_time - ble_last_time);/*uint type, so overflow just used - */
    ble_last_time = now_time;

    if(timeout > TIMEOUT_ONE_PACKET_INTERVAL/portTICK_PERIOD_MS) /*also as timeout = timeout*portTICK_PERIOD_MS;*/
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief 蓝牙串口通信中断接收处理函数,收到不同的字符,状态机进入不同状态,直到收完一帧蓝牙数据,回到最初状态
 * 例如: MCU发送设置蓝牙广播名字指令（16进制:01 34 0B 02 01 06 07 FF 00 00 03 07 25 B4）后,
 * 串口中断函数收到的回复消息依次为为:(16进制:02 06 02 34 00);
 * 接收机状态为:消息头部HEAD0:0x02; 消息操作码HEAD1:0x06; 消息有效载荷LENGTH:0x02; 
 * 数据区第1字节0x34,对应原始的设置指令的操作码（消息第二字节）;数据区第2字节00,表示设置参数成功
 */
void ble_receive_packet(uint8_t receive_char)
{
    static uint8_t rev_state = BLE_REV_STATE_PACK_TYPE;
    static uint16_t data_rev_count = 0;
    if(true == ble_port_timeout_one_packet())
    {
        rev_state = BLE_REV_STATE_PACK_TYPE;
    }
    //mprintf(" 0x%02x,",receive_char);
    switch(rev_state)
    {
        case BLE_REV_STATE_PACK_TYPE: 
           // mprintf("_HEAD0 :0x%02x ",receive_char);
            if((BLE_PACK_TPYE_ENENT == receive_char)||(BLE_PACK_TPYE_PATCH == receive_char))
            {
                rev_state = BLE_REV_STATE_OPCODE;
                ble_recever_packet.type = receive_char;
                data_rev_count = 0;
            }
            else
            {
                #if UART_BAUDRATE_CALIBRATE
                baudrate_calibrate((UART_TypeDef *)BLE_PROTOCOL_NUMBER);
                #endif
                rev_state = BLE_REV_STATE_PACK_TYPE;
            }
            break;

        case BLE_REV_STATE_OPCODE:
           // mprintf("_HEAD1 :0x%02x \r\n",receive_char);
            ble_recever_packet.opcode = receive_char;
            rev_state = BLE_REV_STATE_LENGTH;
            break;
            
        case BLE_REV_STATE_LENGTH:
            //mprintf("lenth :0x%02x \r\n",receive_char);
            ble_recever_packet.length = receive_char;
            if (ble_recever_packet.length == 0)
            {
                switch (ble_recever_packet.opcode)
                {
                case BLE_EVENT_STACK_OK:
                    //蓝牙协议栈启动成功
                    mprintf("ble stack start ok\r\n");
                    break;

                case BLE_EVENT_CONN_REP:
                    //蓝牙连接成功
                    ble_status = BLE_STATUS_CONNECT;
                    ble_send_recv_msg(&ble_recever_packet, NULL);
                    break;
                case BLE_EVENT_DIS_REP:
                    //蓝牙连接断开
                    ble_status = BLE_STATUS_DISCONNECT;
                    ble_send_recv_msg(&ble_recever_packet, NULL);
#if BLE_PAIRING_ENBLE
                    if (ble_pairing_status == BLE_PAIRING_STATE_START)
                    {
                        ble_pairing_status = BLE_PAIRING_STATE_IDLE;
                    }
#endif
                    break;
                }
                rev_state = BLE_REV_STATE_PACK_TYPE;
            }
            else
                rev_state = BLE_REV_STATE_DATA;
            break;
         
        case BLE_REV_STATE_DATA:
            //mprintf("%d:0x%02x ",data_rev_count, receive_char);
            ble_recever_packet.msg_data[data_rev_count++] = receive_char;
            if(data_rev_count == ble_recever_packet.length)
            {
                switch (ble_recever_packet.opcode)
                {
                case BLE_EVENT_CMD_RES:
                    //收到蓝牙设置命令返回
                    if (ble_recever_packet.msg_data[data_rev_count-1] == BLE_CMD_SUCCESS)
                    {
                        //设置命令成功
                       // mprintf("cmd len:%d  type:0x%02x \r\n",ble_recever_packet.length, ble_recever_packet.msg_data[0]);
                        ble_recv_cmd = ble_recever_packet.msg_data[0];
                        xSemaphoreGiveFromISR(ble_recv_ack_mutex, false);
                    }
                    else if (ble_recever_packet.msg_data[0] == BLE_CMD_VERSION_REQUEST) //查询版本，结尾字符不是0
                    {
                        //查询固件版本成功
                        ble_recv_cmd = ble_recever_packet.msg_data[0];;
                        xSemaphoreGiveFromISR(ble_recv_ack_mutex, false);
                    }
                    break;    
                
                case BLE_EVENT_STATUS_REP:
                    //收到蓝牙状态查询回复事件
                    ble_recv_cmd = BLE_EVENT_STATUS_REP;
                    ble_status = ble_recever_packet.msg_data[0];
                    xSemaphoreGiveFromISR(ble_recv_ack_mutex, false);
                    break;

                case BLE_EVENT_PATCH_ACK:
                    //收到固件升级包ack
                    if ((ble_recever_packet.msg_data[0] == BLE_EVENT_TYPE_PATCH)&&(ble_recever_packet.msg_data[data_rev_count-1] == BLE_CMD_SUCCESS))
                    {
                        ble_recv_cmd = BLE_EVENT_TYPE_PATCH;
                        xSemaphoreGiveFromISR(ble_recv_ack_mutex, false);
                    }
                    break;
                    
                case BLE_EVENT_DATA_REP:
                case BLE_EVENT_NVRAM_REP:
                    //收到手机蓝牙数据或nv存储数据
                    ble_send_recv_msg(&ble_recever_packet, NULL);
                    break;

                case BLE_EVENT_PAIRING_STATE:
                    //收到配对状态
                    if((ble_recever_packet.msg_data[0] == 0x80)&&(ble_recever_packet.msg_data[1] == 0x00))
                    {
                        ble_pairing_status = BLE_PAIRING_STATE_SUCCESS;
                        mprintf("ble pairing success\r\n");
                    }
                    else if ((ble_recever_packet.msg_data[0] == 0x80)&&(ble_recever_packet.msg_data[1] == 0x01))
                    {
                        ble_pairing_status = BLE_PAIRING_STATE_FAIL;
                        mprintf("ble pairing fail\r\n");
                    }
                    break;

                case BLE_EVENT_ENCRYPTION_STATE:
                    //收到加密状态
                    if(ble_recever_packet.msg_data[0] == 0x01)
                    {
                        mprintf("ble encryption success\r\n");
                    }
                    else if (ble_recever_packet.msg_data[0] == 0)
                    {
                        mprintf("ble encryption fail\r\n");
                    }
                    break;

                case BLE_EVENT_ADV_REP:
                    //收到广播扫描数据
                    ble_send_recv_msg(&ble_recever_packet, NULL);
                    break;

                case BLE_EVENT_UUID_HANDLE:
                    //收到添加蓝牙服务和特征成功
                    if(ble_recever_packet.msg_data[data_rev_count-1] == BLE_CMD_SUCCESS)
                    {
                        //保存返回的蓝牙设置指令操作码
                        ble_recv_cmd = ble_recever_packet.opcode;
                        //添加蓝牙服务和特征成功,还需要保存当前返回的handle,用于后续和手机进行数据收发
                        ble_handle = ble_recever_packet.msg_data[data_rev_count-2];
                        xSemaphoreGiveFromISR(ble_recv_ack_mutex, false);
                    }
                    break;
                
                default:
                    mprintf("other event %x %d\r\n", ble_recever_packet.type, ble_recever_packet.msg_data[0]);
                    break;
                }
                rev_state = BLE_REV_STATE_PACK_TYPE;
                data_rev_count = 0;
            }
            break;
        default:
            rev_state = BLE_REV_STATE_PACK_TYPE;
            break;
    }
}


/**
 * @brief 注册的蓝牙串口通信中断回调函数,将收到的字符发送到消息处理函数
 */
static void ble_protocol_irq_handler(void)
{
    /*发送数据*/
    if (((UART_TypeDef*)BLE_PROTOCOL_NUMBER)->UARTMIS & (1UL << UART_TXInt))
    {
        UART_IntClear((UART_TypeDef*)BLE_PROTOCOL_NUMBER,UART_TXInt);
    }
    /*接受数据*/
    if (((UART_TypeDef*)BLE_PROTOCOL_NUMBER)->UARTMIS & (1UL << UART_RXInt))
    {
        //here FIFO DATA must be read out
        #if BLE_MODE_BQB_TEST
            UartPollingSenddata((UART_TypeDef *)BLE_MCU_NUMBER, UART_RXDATA((UART_TypeDef*)BLE_PROTOCOL_NUMBER)); 
        #else
            ble_receive_packet(UART_RXDATA((UART_TypeDef*)BLE_PROTOCOL_NUMBER));
        #endif 
        
        UART_IntClear((UART_TypeDef*)BLE_PROTOCOL_NUMBER,UART_RXInt);
    }

    UART_IntClear((UART_TypeDef*)BLE_PROTOCOL_NUMBER,UART_AllInt);
}

/**
 * @brief 蓝牙通信串口初始化
 */
USE_XFI int ble_port_protocol_hw_init(UART_BaudRate baud)
{
    __eclic_irq_set_vector(BLE_PROTOCOL_IRQ_NUMBER, (int32_t)ble_protocol_irq_handler);
    UARTInterruptConfig((UART_TypeDef *)BLE_PROTOCOL_NUMBER, baud);
    return 1;
}


#if BLE_MODE_BQB_TEST
/**
 * @brief MCU桥接PC和蓝牙，mcu串口接收处于桥接模式时,收到PC端发送数据直接透传到蓝牙端
 */
USE_XFI static void ble_mcu_irq_handler(void)
{
    /*发送数据*/
    if (((UART_TypeDef*)BLE_MCU_NUMBER)->UARTMIS & (1UL << UART_TXInt))
    {
        UART_IntClear((UART_TypeDef*)BLE_MCU_NUMBER,UART_TXInt);
    }
    /*接受数据*/
    if (((UART_TypeDef*)BLE_MCU_NUMBER)->UARTMIS & (1UL << UART_RXInt))
    {
        uint8_t rev_data =  UART_RXDATA((UART_TypeDef*)BLE_MCU_NUMBER);
        mprintf("%x ",rev_data);
        UartPollingSenddata((UART_TypeDef *)BLE_PROTOCOL_NUMBER, rev_data);  
        UART_IntClear((UART_TypeDef*)BLE_MCU_NUMBER,UART_RXInt);
    }

    UART_IntClear((UART_TypeDef*)BLE_MCU_NUMBER,UART_AllInt);
}


/**
 * @brief MCU桥接PC和蓝牙，进行BQB认证时,mcu和pc端通信串口初始化
 */
USE_XFI int ble_port_trans_hw_init(void)
{
    __eclic_irq_set_vector(BLE_MCU_IRQ_NUMBER, (int32_t)ble_mcu_irq_handler);
    UARTInterruptConfig((UART_TypeDef *)BLE_MCU_NUMBER, BLE_DEFAULT_BAUDRATE);
    return 1;
}
#endif

/**  
 * @brief 硬件复位蓝牙模块,拉低mcu连接蓝牙的reset脚超过1ms复位蓝牙模块
 * 注意：蓝牙复位后会丢失所有配置，需从新下载协议栈固件并配置工作状态
 */
USE_XFI void ble_reset_hardware(void)
{
    gpio_set_output_level_single(BLE_RESET_GPIO_PORT, BLE_RESET_GPIO_PIN, 1);            //输出高电平
    vTaskDelay(pdMS_TO_TICKS(100)); 
    gpio_set_output_level_single(BLE_RESET_GPIO_PORT, BLE_RESET_GPIO_PIN, 0);            //输出低电平
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_output_level_single(BLE_RESET_GPIO_PORT, BLE_RESET_GPIO_PIN, 1);            //输出高电平
    vTaskDelay(pdMS_TO_TICKS(100)); 
}

USE_XFI void ble_reset_gpio_init(void)
{
    scu_set_device_gate(BLE_RESET_GPIO_PORT, ENABLE);

    dpmu_set_io_reuse(BLE_RESET_PIN, BLE_RESET_PIN_REUSE);
    dpmu_set_io_direction(BLE_RESET_PIN, DPMU_IO_DIRECTION_OUTPUT);
    dpmu_set_io_pull(BLE_RESET_PIN, DPMU_IO_PULL_UP);
    gpio_set_output_mode(BLE_RESET_GPIO_PORT, BLE_RESET_GPIO_PIN);

    gpio_set_output_level_single(BLE_RESET_GPIO_PORT, BLE_RESET_GPIO_PIN, 1);            //输出高电平
    vTaskDelay(pdMS_TO_TICKS(100)); 
}

/**
 * @brief 创建蓝牙设置指令回复的信号量和蓝牙发送数据的信号量
 */
USE_XFI bool ble_port_mutex_create(void)
{
    if (NULL == ble_send_packet_mutex)
    {
        ble_send_packet_mutex = xSemaphoreCreateMutex();
    }
    if (NULL == ble_send_packet_mutex) 
    {
        mprintf("%s, error\n",__func__);
        return false;
    }

    if (NULL == ble_recv_ack_mutex)
    {
        ble_recv_ack_mutex = xSemaphoreCreateBinary();
    }
    if (NULL == ble_recv_ack_mutex) 
    {
        mprintf("%s, error\n",__func__);
        return false;
    }
    mprintf("%s\n",__func__);
    return true;
}

/**
 * @brief 获取蓝牙发送数据的信号量
 */
USE_XFI static void ble_send_packet_mutex_take(void)
{
    if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState()) 
    {
        if (xSemaphoreTake(ble_send_packet_mutex, portMAX_DELAY) == false)
        {
           // mprintf("%s, error\n",__func__);
        }
        //mprintf("%s\n",__func__);
    }
}

/**
 * @brief 释放蓝牙发送数据的信号量
 */
USE_XFI static void ble_send_packet_mutex_give(void)
{
    if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState()) 
    {
        if (xSemaphoreGive(ble_send_packet_mutex) == false) 
        {
            //mprintf("%s, error\n",__func__);
        }
        //mprintf("%s\n",__func__);
    }
}

/**
 * @brief 获取蓝牙设置指令回复的信号量
 * @return 成功返回true;超时500ms未收到返回的操作码,返回false
 */
USE_XFI static bool ble_recv_ack_mutex_take(void)
{
    if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState()) 
    {
        if (xSemaphoreTakeFromISR(ble_recv_ack_mutex, pdMS_TO_TICKS(500)) == false)
        {
           // mprintf("%s, error\n",__func__);
            return false;
        }
        else 
            return true;
    }
}


/**
 * @brief 发送蓝牙设置命令,并判断设置是否成功
 * @param buf: 需发送的设置参数命令
 * @param len: 命令长度
 * @param req_cmd: 命令对应的操作码
 * @return 设置成功返回true;失败返回false
 * 
 * 例如:设置蓝牙广播名称,需发送数据（16进制:01 34 0B 02 01 06 07 FF 00 00 03 07 25 B4）
 * buf为原始数据,len为总长度14,req_cmd为操作码0x34
 * 蓝牙设置成功后回复(16进制:02 06 02 34 00)
 */
USE_XFI bool ble_send_packet(uint8_t *buf, uint16_t len, uint8_t req_cmd)
{
    ble_send_packet_mutex_take(); //获取蓝牙发送数据信号量
    /*header and data*/    
    for(uint16_t i = 0; i < len; i++)
    {
        UartPollingSenddata((UART_TypeDef *)BLE_PROTOCOL_NUMBER, buf[i]);
    }
    vTaskDelay(pdMS_TO_TICKS(15));
    ble_send_packet_mutex_give(); //释放蓝牙发送数据信号量
    
    if (ble_recv_ack_mutex_take() == false) //等待蓝牙设置指令成功回复
        return false;
    if (ble_recv_cmd != req_cmd)  //判断回复的指令操作码是否一致
        return false;
        
    return true;
}

/**
 * @brief 发送蓝牙数据到手机,并判断是否发送成功
 * @param buf: 需发送的数据
 * @param len: 数据长度
 * @param uuid: 发送数据对应的特征值uuid
 * @return 发送成功返回true;发送返回false
 * 
 * 例如:需发送数据有效数据为（16进制:31 32 33 34）
 * buf为原始有效数据,len为总长度4,uuid初始定义的writet特征对应的uuid(0x0XAE3B)
 * 函数里面填写的发送的串口实际数据为(16进制: 01 09 06 05 00 31 32 33 34)
 * 数据依次为:消息头部 0x01; 消息操作码 0x09; 消息有效载荷len+2: 0x06; 
 * 发送消息的handle低字节 0x05(通过uuid获取); 发送消息所需的handle高字节 0x00;
 * 发送的数据区(16进制: 31 32 33 34)
 */
USE_XFI bool ble_send_payload(uint8_t *buf, uint16_t len, uint16_t uuid)
{
    if (ble_status != BLE_STATUS_CONNECT)
        return false;
    
    ble_send_packet_mutex_take();            //获取蓝牙发送数据信号量
    UartPollingSenddata((UART_TypeDef *)BLE_PROTOCOL_NUMBER, BLE_PACK_TPYE_CMD);  //填写蓝牙发送数据头
    UartPollingSenddata((UART_TypeDef *)BLE_PROTOCOL_NUMBER, BLE_CMD_SEND_DATA);  
    UartPollingSenddata((UART_TypeDef *)BLE_PROTOCOL_NUMBER, len+2); 
    UartPollingSenddata((UART_TypeDef *)BLE_PROTOCOL_NUMBER, get_character_handle(uuid)); //获取特征值uuid对应的handle
    UartPollingSenddata((UART_TypeDef *)BLE_PROTOCOL_NUMBER, 0); 
    for(uint16_t i = 0; i < len; i++)
    {
        UartPollingSenddata((UART_TypeDef *)BLE_PROTOCOL_NUMBER, buf[i]); //填写蓝牙发送有效数据
    }
    if(len <= BLE_MSG_DATA_MAX_SIZE/4)
    {
        vTaskDelay(pdMS_TO_TICKS(BLE_SEND_PAYLOAD_DELAY_20));
    }
    else if(len >= BLE_MSG_DATA_MAX_SIZE/4 && len <= BLE_MSG_DATA_MAX_SIZE/2)
    {
        vTaskDelay(pdMS_TO_TICKS(BLE_SEND_PAYLOAD_DELAY_50));
    }
    else
    {
        vTaskDelay(pdMS_TO_TICKS(BLE_SEND_PAYLOAD_DELAY_100));
    }
    ble_send_packet_mutex_give();           //释放蓝牙发送数据信号量
    if (ble_recv_ack_mutex_take() == false) //等待蓝牙指令成功回复
        return false;
    if (ble_recv_cmd == BLE_CMD_SEND_DATA) //判断回复的指令操作码是否是发送数据的操作码
    {
        //mprintf("ble send ok\r\n");
        return true;
    }
    else
    {
        mprintf("ble send erro\r\n");
        return false;
    }
}


