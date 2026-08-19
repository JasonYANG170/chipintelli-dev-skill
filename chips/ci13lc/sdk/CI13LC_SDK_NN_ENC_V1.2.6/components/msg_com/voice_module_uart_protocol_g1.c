#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "voice_module_uart_protocol.h"

#include "system_msg_deal.h"
#include "command_info.h"
#include "ci_spiflash.h"
#include "prompt_player.h"
#include "ci_nvdata_manage.h"
#include "voice_module_uart_protocol_g1.h"
//#include "color_light_control.h"
#include "baudrate_calibrate.h"
#include "ci_log.h"
#include "ci13lc_dpmu.h"
#include "board.h"

#if (UART_PROTOCOL_VER == 1)

#define TIMEOUT_ONE_PACKET_INTERVAL (1000) /*ms, in this code, it should be bigger than portTICK_PERIOD_MS */

/***************extern variable*************/
extern SemaphoreHandle_t WakeupMutex;

uint8_t voice_onoff = 1;

static uint32_t baud_sync_req;

/***************extern funct*************/
static void play_done_callback(cmd_handle_t cmd_handle)
{
}

void uart_communicate_send(char *buffer, int lenth)
{
    char tc_sum;
    UART_TypeDef *UART_PORT;

    if (buffer == NULL)
    {
        mprintf("err com send buf\r\n");
        return;
    }
    if ((lenth <= 4) || (lenth > sizeof(com_msg_data_t)))
    {
        mprintf("err com send lenth %d \r\n", lenth);
        return;
    }
    else
    {
        lenth -= 2; //last 2 byte not send
    }

    tc_sum = 0;

#if (UART_PROTOCOL_NUMBER == HAL_UART0_BASE)
    UART_PORT = UART0;
#elif (UART_PROTOCOL_NUMBER == HAL_UART1_BASE)
    UART_PORT = UART1;
#elif (UART_PROTOCOL_NUMBER == HAL_UART2_BASE)
    UART_PORT = UART2;
#endif
    while (lenth--)
    {
        tc_sum += *buffer;
        UartPollingSenddata(UART_PORT, *buffer++);
    }
    UartPollingSenddata(UART_PORT, tc_sum); //send sum
    UartPollingSenddata(UART_PORT, UART_END);
}

int send_packet_msg(com_msg_data_t *msg)
{
    sys_msg_t send_msg;
    BaseType_t xResult;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    memcpy((uint8_t *)(&send_msg.msg_data), msg, sizeof(com_msg_data_t));
    send_msg.msg_type = SYS_MSG_TYPE_COM;

    xResult = send_msg_to_sys_task(&send_msg, sizeof(send_msg), &xHigherPriorityTaskWoken);

    if ((xResult != pdFAIL) && (pdTRUE == xHigherPriorityTaskWoken))
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return xHigherPriorityTaskWoken;
}

TickType_t last_time;
static bool timeout_one_packet(void)
{
    TickType_t now_time;
    TickType_t timeout;

    now_time = xTaskGetTickCountFromISR();

    timeout = (now_time - last_time); /*uint type, so overflow just used - */

    last_time = now_time;

    if (timeout > TIMEOUT_ONE_PACKET_INTERVAL / portTICK_PERIOD_MS) /*also as timeout = timeout*portTICK_PERIOD_MS;*/
    {
        return true;
    }
    else
    {
        return false;
    }
}

#define UARTDATA_VALID_TIMEOUT 0xFFFF
void uart_int_rx_handle(unsigned char rxdata)
{
    static BaseType_t xHigherPriorityTaskWoken;
    static unsigned char rx_buf[sizeof(com_msg_data_t)];
    static int tc_state = 0;
    static int tc_cnt = 0;

    if (true == timeout_one_packet())
    {
        tc_state = SOP_STATE1;
        tc_cnt = 0;
    }

    switch (tc_state)
    {
    case SOP_STATE1:
        if (UART_HEADER0 == rxdata)
        {
            rx_buf[tc_cnt++] = rxdata;
            tc_state = SOP_STATE2;
        }
        else
        {
#if UART_BAUDRATE_CALIBRATE
            baudrate_calibrate((UART_TypeDef *)UART_PROTOCOL_NUMBER);
#endif
        }
        break;
    case SOP_STATE2:
        if (UART_HEADER1 == rxdata)
        {
            rx_buf[tc_cnt++] = rxdata;
            tc_state = DATA_STATE;
        }
        else
        {
#if UART_BAUDRATE_CALIBRATE
            baudrate_calibrate((UART_TypeDef *)UART_PROTOCOL_NUMBER);
#endif
            tc_state = SOP_STATE1;
            tc_cnt = 0;
        }
        break;
    case DATA_STATE:
        if (tc_cnt < sizeof(com_msg_data_t))
        {
            rx_buf[tc_cnt] = rxdata;

            if (tc_cnt == (sizeof(com_msg_data_t) - 1))
            {
                if (rx_buf[tc_cnt] == UART_END)
                {
                    com_msg_data_t send_msg;
                    memcpy(&send_msg, rx_buf, sizeof(com_msg_data_t));
                    if (send_msg.data0 == 0x01)
                    {
                        convert_left_and_right(false);
                        mprintf("uart-开降噪\n");
                    }
                    else if (send_msg.data0 == 0x02)
                    {
                        convert_left_and_right(true);
                        mprintf("uart-关降噪\n");
                    }
                    if (send_msg.data0 == 0xAA) //返回固件打包版本
                    {
                        mprintf("uart-查询固件打包版本\n");
                        partition_table_t partition_table_l;
                        uint32_t config_start_addr = 0;
#if CI_OTA_ENABLE
                        config_start_addr = 0x6000;
#else
                        config_start_addr = 0x2000;
#endif
                        post_read_flash((char *)&partition_table_l, config_start_addr, sizeof(partition_table_t));
                        mprintf("soft_ware_name = %s\r\n", partition_table_l.soft_ware_name);
                        mprintf("soft_ware_version = %d\r\n", partition_table_l.soft_ware_version);
                        rx_buf[5] = ((partition_table_l.soft_ware_version >> 16) & 0xff);
                        rx_buf[6] = ((partition_table_l.soft_ware_version >> 8) & 0xff);
                        rx_buf[7] = (partition_table_l.soft_ware_version & 0xff);
                    }
                    else if (send_msg.data0 == 0xAB) //查询sdk版本
                    {
                        mprintf("uart-查询sdk版本\n");
                        rx_buf[5] = SDK_VERSION;
                        rx_buf[6] = SDK_SUBVERSION;
                        rx_buf[7] = SDK_REVISION;
                    }
                    for (int i = 0; i < sizeof(sys_msg_com_data_t); i++)
                    {
                        UartPollingSenddata((UART_TypeDef *)UART_PROTOCOL_NUMBER, rx_buf[i]);
                    }
                    if (send_msg.data0 == 0x1E)
                    {
                        mprintf("reset sw...\r\n");
                        _delay_10us_240M(20000); // 延时200ms
                        dpmu_software_reset_system_config();
                    }
                    send_packet_msg(&send_msg);
                }
                else
                {
                    mprintf("get com data err_end \r\n");
                }
                memset(rx_buf, 0, sizeof(com_msg_data_t));
#if UART_BAUDRATE_CALIBRATE
                baudrate_calibrate((UART_TypeDef *)UART_PROTOCOL_NUMBER);
#endif
                tc_state = SOP_STATE1;
                tc_cnt = 0;
            }
            else
            {
                tc_cnt++;
            }
        }
        else
        {
            //CheckSCUWakeup();
            mprintf("com2 too many data,reset recive\r\n");
#if UART_BAUDRATE_CALIBRATE
            baudrate_calibrate((UART_TypeDef *)UART_PROTOCOL_NUMBER);
#endif
            tc_state = SOP_STATE1;
            tc_cnt = 0;
        }
        break;
    }
}

static void uart_communicate_irq_handler(void)
{
    /*发送数据*/
    if (((UART_TypeDef *)UART_PROTOCOL_NUMBER)->UARTMIS & (1UL << UART_TXInt))
    {
        UART_IntClear((UART_TypeDef *)UART_PROTOCOL_NUMBER, UART_TXInt);
    }
    /*接受数据*/
    if (((UART_TypeDef *)UART_PROTOCOL_NUMBER)->UARTMIS & (1UL << UART_RXInt))
    {
        //here FIFO DATA must be read out
        uart_int_rx_handle(UART_RXDATA((UART_TypeDef *)UART_PROTOCOL_NUMBER));
        UART_IntClear((UART_TypeDef *)UART_PROTOCOL_NUMBER, UART_RXInt);
    }

    UART_IntClear((UART_TypeDef *)UART_PROTOCOL_NUMBER, UART_AllInt);
}

#if UART_BAUDRATE_CALIBRATE
void send_baudrate_sync_req(void)
{
    // sys_msg_t send_msg;
    // memset((void *)(&send_msg.msg_data.com_data.header0),0,sizeof(send_msg.msg_data.com_data));
    // send_msg.msg_type = SYS_MSG_TYPE_COM;
    com_msg_data_t g1_com_data;
    memset(&g1_com_data, 0, sizeof(com_msg_data_t));
    // g1_com_data = (com_msg_data_t*)&send_msg.msg_data.com_data.header0;
    g1_com_data.header0 = UART_HEADER0;
    g1_com_data.header1 = UART_HEADER1;
    g1_com_data.end = UART_END;
    g1_com_data.id = 0;
    g1_com_data.cmd = CI_TX_ASR;
    g1_com_data.data0 = (char)baud_sync_req;
    g1_com_data.data1 = baud_sync_req >> 8; //0;//score_conver;

    uart_communicate_send((char *)&g1_com_data, sizeof(com_msg_data_t));
    //send_msg_to_sys_task( &send_msg, sizeof(send_msg), NULL);
}
#endif

int uart_communicate_init(void)
{
#if HAL_UART0_BASE == UART_PROTOCOL_NUMBER
    __eclic_irq_set_vector(UART0_IRQn, (int32_t)uart_communicate_irq_handler);
#elif (HAL_UART1_BASE == UART_PROTOCOL_NUMBER)
    __eclic_irq_set_vector(UART1_IRQn, (int32_t)uart_communicate_irq_handler);
#else
    __eclic_irq_set_vector(UART2_IRQn, (int32_t)uart_communicate_irq_handler);
#endif
    UARTInterruptConfig((UART_TypeDef *)UART_PROTOCOL_NUMBER, UART_PROTOCOL_BAUDRATE);

#if UART_BAUDRATE_CALIBRATE
    cmd_handle_t cmd_handle = cmd_info_find_command_by_string("<baud_sync_req>");
    baud_sync_req = cmd_info_get_command_id(cmd_handle);
    ci_loginfo(CI_LOG_INFO, "baud_sync_req:%08x\n", baud_sync_req);
#endif

    return RETURN_OK;
}

void uart_send_AsrResult(unsigned int index, float score)
{
    // sys_msg_t send_msg;
    // memset((void *)(&send_msg.msg_data.com_data.header0),0,sizeof(send_msg.msg_data.com_data));
    // send_msg.msg_type = SYS_MSG_TYPE_COM;
    com_msg_data_t g1_com_data;
    memset(&g1_com_data, 0, sizeof(com_msg_data_t));
    // g1_com_data = (com_msg_data_t*)&send_msg.msg_data.com_data.header0;
    g1_com_data.header0 = UART_HEADER0;
    g1_com_data.header1 = UART_HEADER1;
    g1_com_data.end = UART_END;
    g1_com_data.id = 0;
    g1_com_data.cmd = CI_TX_ASR;
    g1_com_data.data0 = (char)index;
    g1_com_data.data1 = index >> 8; //0;//score_conver;

    // send_msg_to_sys_task( &send_msg, sizeof(send_msg), NULL);
    uart_communicate_send((char *)&g1_com_data, sizeof(com_msg_data_t));
}

void uart_send_exit_wakeup(void)
{
    // sys_msg_t send_msg;
    // memset((void *)(&send_msg.msg_data.com_data.header0),0,sizeof(send_msg.msg_data.com_data));
    // send_msg.msg_type = SYS_MSG_TYPE_COM;
    com_msg_data_t g1_com_data;
    memset(&g1_com_data, 0, sizeof(com_msg_data_t));
    // g1_com_data = (com_msg_data_t*)&send_msg.msg_data.com_data.header0;
    g1_com_data.header0 = UART_HEADER0;
    g1_com_data.header1 = UART_HEADER1;
    g1_com_data.end = UART_END;
    g1_com_data.id = 0;
    g1_com_data.cmd = CI_TX_UNWAKEN;
    g1_com_data.data0 = 0x01;
    g1_com_data.data1 = 0; //0;//score_conver;
    // send_msg_to_sys_task( &send_msg, sizeof(send_msg), NULL);
    uart_communicate_send((char *)&g1_com_data, sizeof(com_msg_data_t));
}

void userapp_deal_com_msg(sys_msg_t *sys_msg)
{
    int tmp_data;
    char tc_Chksum;

    com_msg_data_t *g1_com_data;
    g1_com_data = (com_msg_data_t *)sys_msg->msg_data;

    if ((CI_TX_START < g1_com_data->cmd) && (CI_TX_END > g1_com_data->cmd))
    {
        switch (g1_com_data->cmd)
        {
        case CI_TX_ASR:
        case CI_TX_UNWAKEN:
        case CI_TX_VER:
        case CI_TX_REV:
            //uart_communicate_send((char*)g1_com_data,sizeof(com_msg_data_t));
            break;
        }
    }
    else
    {
#if 0 //二代对应修改
        if(gs_uar2_user.echo)//回显打开
        {
            uart_communicate_send((char*)g1_com_data,sizeof(com_msg_data_t));
        }
        if(gs_uar2_user.chk_enable)//check sum
        {
            tc_Chksum = g1_com_data->header0+g1_com_data->header1+g1_com_data->id + g1_com_data->cmd + g1_com_data->data0+g1_com_data->data1;
            if(g1_com_data->chksum != tc_Chksum)
            {
                mprintf("get chk=%x,cal chk=%x,err!\r\n",g1_com_data->chksum,tc_Chksum);
                return;
            }
        }
#endif
        if ((CI_RX_START < g1_com_data->cmd) && (CI_RX_END > g1_com_data->cmd))
        {
            switch (g1_com_data->cmd)
            {
            case CI_RX_CHK_EN: //checksum enable
#if 0                          //二代对应修改
                    gs_uar2_user.chk_enable = g1_com_data->data0;
#endif
                mprintf("rx cmd = %d,data=%d\r\n", g1_com_data->data0, g1_com_data->data1);
                break;
            case CI_RX_ECHO_EN: //echo enable
#if 0                           //二代对应修改
                    gs_uar2_user.echo = g1_com_data->data0;
#endif
                mprintf("rx cmd = %d,data=%d\r\n", g1_com_data->data0, g1_com_data->data1);
                break;
            case CI_RX_PLAY_INDEX: //play wav index (com_data->data0)
                tmp_data = g1_com_data->data0;
                prompt_play_by_cmd_id(tmp_data, -1, play_done_callback, false);
                update_awake_time();
                break;

            case CI_RX_RESET: //play wav index (com_data->data0)
                if (g1_com_data->data0 == 1)
                {
                    mprintf("CI_RX_RESET\r\n");
                    //Scu_SoftwareRst_System();
                    UartPollingSenddone((UART_TypeDef *)CONFIG_CI_LOG_UART);
                    dpmu_software_reset_system_config();
                }
                break;
            case CI_RX_UNWAKE_CMD:
                if (g1_com_data->data0 == 1)
                {
                    if (SYS_STATE_WAKEUP == get_wakeup_state())
                    {
                        xSemaphoreTake(WakeupMutex, portMAX_DELAY);
                        sys_msg_t send_msg;
                        send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
                        sys_msg_cmd_info_data_t *msg_data = (sys_msg_cmd_info_data_t *)send_msg.msg_data;
                        msg_data->cmd_info_status = MSG_CMD_INFO_STATUS_EXIT_WAKEUP;
                        send_sys_msg_inner(&send_msg, sizeof(send_msg), NULL);
                        xSemaphoreGive(WakeupMutex);
                    }
                    mprintf("unwake....\n");
                }
                break;
            case CI_RX_WAKEN:
                if (g1_com_data->data0 == 1)
                {
                    if (SYS_STATE_UNWAKEUP == get_wakeup_state())
                    {
                        // xSemaphoreTake(WakeupMutex, portMAX_DELAY);
                        // /*if last state is unwakeup, need change asr word*/
                        // audio_cap_stop(AUDIO_CAP_INNER_CODEC);
                        // audio_pre_rslt_stop();
                        // power_mode_switch(POWER_MODE_NORMAL);
                        // audio_cap_start(AUDIO_CAP_INNER_CODEC);
                        // audio_pre_rslt_start();
                        // sys_msg_t send_msg;
                        // send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
                        // send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD;
                        // send_msg_to_sys_task(&send_msg, sizeof(send_msg), NULL);
                        // set_state_enter_wakeup(EXIT_WAKEUP_TIME);
                        // xSemaphoreGive(WakeupMutex);
                        enter_wakeup_deal(EXIT_WAKEUP_TIME, INVALID_HANDLE);
                    }
                    mprintf("wakeup....\n");
                }
                else
                {
                    mprintf("com2 wake cmd error\n");
                }
                break;
            case CI_RX_MUTE:
                if (g1_com_data->data0 == 1)
                {
                    prompt_player_enable(ENABLE);
                    prompt_play_by_semantic_id(0x1941B02, -1, play_done_callback, true);
                    update_awake_time();
                }
                else if (g1_com_data->data0 == 0)
                {
                    // 此命令词的播报还需要，但后面的流程已关闭，所以在此播报。
                    prompt_play_by_semantic_id(0x1941B45, -1, play_done_callback, true);
                    prompt_player_enable(DISABLE);
                    update_awake_time();
                }
                else
                {
                    mprintf("mute cmd data error\n");
                }
                break;
            case CI_RX_VOLSET: //play wav index (com_data->data0)
                if (g1_com_data->data0 <= 0x07)
                {
                    vol_set((g1_com_data->data0 >= VOLUME_MAX) ? g1_com_data->data0 : (g1_com_data->data0 + 1));
                }
                else
                {
                    mprintf("volset set %d,too large fail set volset\r\n", g1_com_data->data0);
                }
                break;
            case CI_RX_GETVER:
                if (g1_com_data->data0 == 1)
                {
                    // sys_msg_t send_msg;
                    // memset((void *)(&send_msg.msg_data.com_data.header0),0,sizeof(send_msg.msg_data.com_data));
                    // send_msg.msg_type = SYS_MSG_TYPE_COM;
                    com_msg_data_t g1_com_data;
                    memset(&g1_com_data, 0, sizeof(com_msg_data_t));
                    // g1_com_data = (com_msg_data_t*)&send_msg.msg_data.com_data.header0;
                    g1_com_data.header0 = UART_HEADER0;
                    g1_com_data.header1 = UART_HEADER1;
                    g1_com_data.id = 0;
                    g1_com_data.cmd = CI_TX_VER;
                    g1_com_data.data0 = USER_VERSION_MAIN_NO;
                    g1_com_data.data1 = USER_VERSION_SUB_NO; //score_conver;
                    g1_com_data.end = UART_END;

                    //send_msg_to_sys_task( &send_msg, sizeof(send_msg), NULL);
                    uart_communicate_send((char *)&g1_com_data, sizeof(com_msg_data_t));
                }
                break;
            case CI_RX_REV:
            {
#if 0 //二代对应修改
                    tmp_data = ReservedFunc((char *)g1_com_data);
#endif
                // sys_msg_t send_msg;
                // memset((void *)(&send_msg.msg_data.com_data.header0),0,sizeof(send_msg.msg_data.com_data));
                // send_msg.msg_type = SYS_MSG_TYPE_COM;
                com_msg_data_t g1_com_data;
                memset(&g1_com_data, 0, sizeof(com_msg_data_t));
                // g1_com_data = (com_msg_data_t*)&send_msg.msg_data.com_data.header0;
                g1_com_data.header0 = UART_HEADER0;
                g1_com_data.header1 = UART_HEADER1;
                g1_com_data.id = (tmp_data >> 16) & 0xff;
                g1_com_data.cmd = CI_TX_REV; //(tmp_data>>16);
                g1_com_data.data0 = (tmp_data >> 8) & 0xff;
                g1_com_data.data1 = (tmp_data & 0xff); //score_conver;
                g1_com_data.end = UART_END;
                //send_msg_to_sys_task( &send_msg, sizeof(send_msg), NULL);
                uart_communicate_send((char *)&g1_com_data, sizeof(com_msg_data_t));
                break;
            }
#if 0 //二代对应修改
#if MULTI_GROUP_ENABLE
                case CI_RX_CHANGE_GROUP:
                    asr_set_awake(false);
                    stop_current_play();
                    if (com_data->data0 == 0)
                    {   
                        switch_language_req(0,0,0,SUPPORT_CHINESE_LANGUAGE);
                        nvdata_save.uLanguageId = SUPPORT_CHINESE_LANGUAGE;
                        nvdata_write((uint8_t* )&nvdata_save,sizeof(nvdata_save));
                    }
                    else if (com_data->data0 == 1)
                    {
                        switch_language_req(1,1,1,SUPPORT_ENGLISH_LANGUAGE);
                        nvdata_save.uLanguageId = SUPPORT_ENGLISH_LANGUAGE;
                        nvdata_write((uint8_t* )&nvdata_save,sizeof(nvdata_save));
                    }
                break;
#endif
#endif
            }
        }
        else
        {
            mprintf("com2 may recived an undefined cmd\r\n", g1_com_data->cmd);
        }
    }
}
#endif
