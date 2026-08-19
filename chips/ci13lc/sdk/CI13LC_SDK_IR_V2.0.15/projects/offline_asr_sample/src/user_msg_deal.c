#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "system_msg_deal.h"
#include "prompt_player.h"
#include "voice_module_uart_protocol.h"
#include "voice_module_uart_protocol_g1.h"
#include "i2c_protocol_module.h"
#include "ci_nvdata_manage.h"
#include "ci_log.h"
#include "user_msg_deal.h"
#include "common_api.h"
#include "ci_gpio.h"
#include "ci13lc_dpmu.h"
// #include "all_cmd_statement.h"

///tag-insert-code-pos-1
xTimerHandle xTimerDevice = NULL;
QueueHandle_t device_queue = NULL;

/**
 * @brief 用户初始化
 *
 */
#if IR_BOARN_LEVEL
xTimerHandle xTimerLedFlash = NULL;
static unsigned int uiLedFlashCnt = 0;
#endif

void user_led_light_on(void)
{
    #if IR_BOARN_LEVEL
    //低电平有效
    gpio_set_output_level_single(PB, pin_6,1);
    #endif
}

void user_led_light_off(void)
{
    #if IR_BOARN_LEVEL
     gpio_set_output_level_single(PB, pin_6,0);
    #endif
}

#if IR_BOARN_LEVEL
void LedFlashTimerCallback(TimerHandle_t xTimer)
{
    if (0 == uiLedFlashCnt)
    {
        xTimerStop(xTimerLedFlash,0);
        if (SYS_STATE_WAKEUP == get_wakeup_state())
        {
            user_led_light_on();
        }
        else
        {
            user_led_light_off();
        }
        goto out;
    }

    if (uiLedFlashCnt%2 == 1)
    {
        user_led_light_on();
    }
    else
    {
        user_led_light_off();
    }

    uiLedFlashCnt--;

out:
    return;
}
#endif

void user_led_light_init(void)
{
    #if IR_BOARN_LEVEL
    scu_set_device_gate(PB,ENABLE);
    dpmu_set_io_direction(PB6,DPMU_IO_DIRECTION_OUTPUT);
    dpmu_set_io_reuse(PB6,FIRST_FUNCTION);/*gpio function*/
    gpio_set_output_mode(PB, pin_6);
    gpio_set_output_level_single(PB, pin_6,0);

    xTimerLedFlash = xTimerCreate("xTimerLedFlash", (pdMS_TO_TICKS(100)), pdTRUE, (void *)0, LedFlashTimerCallback);
    if(!xTimerLedFlash)
    {
        mprintf("xTimerLedFlash fail!\n");
    }
    #endif
}


void user_led_light_flash(void)
{
     #if IR_BOARN_LEVEL
     uiLedFlashCnt = 6;
     xTimerStart(xTimerLedFlash,0);
     #endif
}
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
//红外需要创建任务队列和timer
    device_queue = xQueueCreate(10, sizeof(device_msg));
    if(!device_queue)
    {
        mprintf("device_queue fail\n");
    }
 //创建timer心跳，用于给设备做定时任务。心跳周期是DEVICE_TIME，不需要很精确的任务

    ContinueSendKey_list_init();
    xTimerDevice = xTimerCreate("xTimerDevice", (pdMS_TO_TICKS(DEVICE_TIME)), pdTRUE, (void *)0, DeviceTimerCallback);
    if(!xTimerDevice)
    {
        mprintf("xTimerMain fail!\n");
    }
    user_led_light_init();
    night_light_init();
    ///tag-gpio-init
    ///tag-gpio-init
}

/**
 * @brief 处理按键消息（目前未实现该demo）
 *
 * @param key_msg 按键消息
 */
void userapp_deal_key_msg(sys_msg_t *msg)
{
    sys_msg_key_data_t *key_msg = (sys_msg_key_data_t*)msg->msg_data;
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
            #if PLAY_OTHER_CMD_EN
            prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback,true);
            #endif
        }
    }
    else
    {
        ret = 0;
    }
    return ret;
}


/**
 * @brief 按命令词id响应asr消息处理
 *
 * @param asr_msg
 * @param cmd_handle
 * @param cmd_id
 * @return uint32_t
 */
uint32_t deal_asr_msg_by_cmd_id(sys_msg_asr_data_t *asr_msg, cmd_handle_t cmd_handle, uint16_t cmd_id)
{
    uint32_t ret = 1;
    int select_index = -1;
    switch(cmd_id)
    {
        ///tag-asr-msg-deal-by-cmd-id-start
        case 2://“打开空调”
        {
            break;
        }
        case 3://“关闭空调”
        {
            break;
        }
        case 14://“除湿模式”
        {
            break;
        }
        case 15://"关闭除湿"
        {
            break;
        }
        case 22://"关闭睡眠模式"
        {
            break;
        }
        ///tag-asr-msg-deal-by-cmd-id-end
        default:
            ret = 0;
            break;
    }

    if (ret && select_index >= -1)
    {
        #if PLAY_OTHER_CMD_EN
        prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback,true);
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
        userapp_deal_key_msg(msg);
        break;
    }
    #if MSG_COM_USE_UART_EN
    /* CI串口协议消息 */
    case SYS_MSG_TYPE_COM:
    {
		#if ((UART_PROTOCOL_VER == 1) || (UART_PROTOCOL_VER == 2))
        userapp_deal_com_msg(msg);
        #endif
        break;
    }
    #endif
    /* CI IIC 协议消息 */
    #if MSG_USE_I2C_EN
    case SYS_MSG_TYPE_I2C:
    {
        userapp_deal_i2c_msg(msg);
        break;
    }
    #endif
    default:
        break;
    }
    return ret;
}

