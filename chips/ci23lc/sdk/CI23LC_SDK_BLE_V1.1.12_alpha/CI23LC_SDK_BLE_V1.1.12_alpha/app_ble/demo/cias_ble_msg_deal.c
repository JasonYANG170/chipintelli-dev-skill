#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ci_log.h"
#include "system_msg_deal.h"
#include "user_config.h"
#include "sdk_default_config.h"
#include "ble_param_config.h"
#include "crc.h"
#include "cias_ble_msg_deal.h"

void *ble_msg_queue = NULL;

/**
 * @brief 初始化adv_ind数组里面的广播数据,同样启英物联小程序连接,用户可以自行修改
 */
void ci_ble_adv_data_init(uint8_t *adv_data, char *adv_name)
{
    int adv_name_len = strlen(adv_name);
    adv_data[0] = adv_name_len + 1;
    adv_data[1] = 0x09;
    sprintf(&adv_data[2], "%s", adv_name);
    adv_data[2 + adv_name_len] = 0x02;
    adv_data[2 + adv_name_len + 1] = 0x01;
    adv_data[2 + adv_name_len + 2] = 0x06;

    adv_data[2 + adv_name_len + 3] = 0x07;   //启英小程序协议
    adv_data[2 + adv_name_len + 4] = 0xFF;

    adv_data[2 + adv_name_len + 5] = DEV_MTU_TYPE;
    adv_data[2 + adv_name_len + 6] = CONFIG_TYPE;
    adv_data[2 + adv_name_len + 7] = DEV_TYPE_ID;
    adv_data[2 + adv_name_len + 8] = DEV_NUMBER_ID;
    unsigned short crc_cal = crc16_ccitt(0, &adv_data[7 + adv_name_len], 4);
    adv_data[2 + adv_name_len + 9] = crc_cal >> 8;
    adv_data[2 + adv_name_len + 10] = crc_cal & 0xFF;

}

void dev_state_init(void)
{
#if (DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)
    aircondition_init();
#elif (DEV_DRIVER_EN_ID == DEV_LIGHT_CONTROL_MAIN_ID)
    rgb_init();
#elif (DEV_DRIVER_EN_ID == DEV_TEA_BAR_MAIN_ID)
    tbm_init();
#elif (DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID)
    fan_init();
#elif (DEV_DRIVER_EN_ID == DEV_HEATTABLE_MAIN_ID)
    heattable_init();
#elif (DEV_DRIVER_EN_ID == DEV_WARMER_MAIN_ID)
    warmer_init();
#elif (DEV_DRIVER_EN_ID == DEV_WATERHEATED_MAIN_ID)
    waterheated_init();
#endif
}

void printf_ble_msg_V1(ble_msg_V1_t ble_msg)
{
    mprintf("ble_msg dev_type %x\r\n", ble_msg.dev_type);
    mprintf("ble_msg dev_number %x\r\n", ble_msg.dev_number);
    mprintf("ble_msg function_type %x\r\n", ble_msg.function_type);
    mprintf("ble_msg data_type %x\r\n", ble_msg.data_type);
    mprintf("ble_msg function_id %x\r\n", ble_msg.function_id);
    //mprintf("ble_msg data_len %x\r\n",ble_msg.data_len);
    mprintf("ble_msg msg data 0: %x\r\n", ble_msg.data[0]);
}
/**
 * @brief 接收并处理蓝牙消息任务，按照蓝牙小程序协议或自定义蓝牙协议，解析设备端收到的手机APP消息(该函数只适用于和启英物联APP交互使用)
 *
 */

void ci_ble_recv_task()
{
    BaseType_t err = pdPASS;
    ble_msg_V1_t recv_ble_msg;
    uint16_t recv_ble_type;
    ble_msg_queue = xQueueCreate(10, sizeof(ble_msg_V1_t));
    while (1)
    {
        /* 阻塞接收系统消息 */
        if (xQueueReceive(ble_msg_queue, &recv_ble_msg, portMAX_DELAY) != pdPASS)
        {
            mprintf("ble_msg_queue rcv error ...\r\n");
        }
        else
        {
            recv_ble_type = recv_ble_msg.dev_type;
            mprintf("ble_recv_msg type = 0x%x\r\n", recv_ble_type);
            switch (recv_ble_type)
            {
#if (DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)
            case AIRCONDITION_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    aircondition_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    aircondition_query(recv_ble_msg);
                }
            }
            break;
#elif (DEV_DRIVER_EN_ID == DEV_LIGHT_CONTROL_MAIN_ID)
            case RGB_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    rgb_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    rgb_query(recv_ble_msg);
                }
            }
            break;
#elif (DEV_DRIVER_EN_ID == DEV_TEA_BAR_MAIN_ID)
            case TEABAR_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    tbm_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    tbm_query(recv_ble_msg);
                }
            }
            break;
#elif (DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID)
            case FAN_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    fan_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    fan_query(recv_ble_msg);
                }
            }
            break;

#elif (DEV_DRIVER_EN_ID == DEV_HEATTABLE_MAIN_ID)
            case HEATTABLE_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    heattable_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    heattable_query(recv_ble_msg);
                }
            }
            break;

#elif (DEV_DRIVER_EN_ID == DEV_WARMER_MAIN_ID)
            case WARMER_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    warmer_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    warmer_query(recv_ble_msg);
                }
            }
            break;

#elif (DEV_DRIVER_EN_ID == DEV_WATERHEATED_MAIN_ID)
            case WATERHEATED_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    waterheated_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    waterheated_query(recv_ble_msg);
                }
            }
            break;
#endif
            default:
                break;
            }
        }
    }
}

/**
 * @brief 组装蓝牙设备发送的消息，按照蓝牙小程序协议或自定义协议，上报本地IOT事件到手机APP端
 *
 */
void deal_ble_send_msg(uint16_t cmd_id)
{
    switch (cmd_id)
    {
    default:
    {

#if (DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)
        aircondition_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_LIGHT_CONTROL_MAIN_ID)
        rgb_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_TEA_BAR_MAIN_ID)
        tbm_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID)
        fan_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_HEATTABLE_MAIN_ID)
        heattable_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_WARMER_MAIN_ID)
        warmer_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_WATERHEATED_MAIN_ID)
        waterheated_report(cmd_id);
#endif
        break;
    }
    }
}
//蓝牙连接成功函数处理
void ci_ble_connected_handle(void)
{
    prompt_play_by_cmd_string("<设备蓝牙已连接>", -1, default_play_done_callback, false);

}
//蓝牙断开函数处理
void ci_ble_disconnect_handle(void)
{
    prompt_play_by_cmd_string("<设备蓝牙已断开>", -1, default_play_done_callback, false);
}
/**
 * @brief 处理蓝牙接收到的手机端消息-只使用于启英物联加密交互，客户使用自己的私有协议
 * @param recv_data 接收到的数据；
 * @param len       接收数据长度；
 */
void ci_ble_recv_data_handle(uint8_t *recv_data, uint8_t len)
{
    cias_crypto_data(recv_data, len);
    uint16_t crc_cal = crc16_ccitt(0, recv_data, len - 2);
    if (recv_data[len - 1] != (crc_cal & 0xff))
    {
        mprintf("crc erro\r\n");
        return;
    }
    ble_cmd_t *recv_ble_cmd = (ble_cmd_t *)recv_data;
    if (recv_ble_cmd->pkg_header[0] == 0xA5 && recv_ble_cmd->pkg_header[1] == 0x5B) // 校验帧头
    {
        if (recv_ble_cmd->ble_cmd == 0x01) // ble连接断开
        {
            //app_cb_disconnected();
            ble_disconnect();
        }
		if (recv_ble_cmd->ble_cmd == 0x02) // ble获取词条
        {
            #if APP_GET_CMD_INFO_ENABEL
            app_cb_att_read();
            #endif
        }
        memset(recv_data, 0, len);
    }
#ifdef CIAS_PROTOCOL_VER
    BaseType_t err = pdPASS;
    ble_msg_V1_t *recv_ble_msg = (ble_msg_V1_t *)recv_data;
    if (recv_ble_msg->pkg_header[0] == 0xA5 && recv_ble_msg->pkg_header[1] == 0x5A) // 校验帧头
    {
        if (recv_ble_msg->protocol_ver == 0x01) // V1.0协议版本
        {
            err = xQueueSend(ble_msg_queue, recv_ble_msg, 0); // 开启ble模式时，将收到数据发送到ble处理消息队列，按照蓝牙小程序协议处理消息
            if (err != pdPASS)
            {
                mprintf("deal_ble_recv_msg send fail:%d,%s\n", __LINE__, __FUNCTION__);
            }
        }
    }
#endif
}

extern TaskHandle_t ble_task_handle;
void app_cb_disconnected_timer_callback()
{
    extern void ble_main_task(void);
    xTaskCreate(ble_main_task,"ble_main_task",480,NULL,4,&ble_task_handle);
}

void app_cb_disconnected()
{
    TimerHandle_t app_cb_disconnected_timer = xTimerCreate("app_cb_disconnected_timer", pdMS_TO_TICKS(1000), 
											false, (void *)0, app_cb_disconnected_timer_callback);
	xTimerStart(app_cb_disconnected_timer, 0);
    mprintf("ble app disconnect\r\n");
    prompt_play_by_cmd_string("<设备蓝牙已断开>", -1, default_play_done_callback,false);
    vTaskDelete(NULL);
}
void uart_send_asr(uint16_t cmd_id)
{
/* #if (UART_PROTOCOL_VER == 1)
    uart_send_AsrResult(cmd_id, 0);
#elif (UART_PROTOCOL_VER == 2)
    vmup_send_asr_result_cmd(cmd_id, 0);
#elif (UART_PROTOCOL_VER == 255)
    usr_send_asr_result(cmd_id);
#endif // 0 */
}
