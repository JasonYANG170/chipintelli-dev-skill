/**
 ******************************************************************************
 * @文件    cias_network_msg_protocol.c
 * @版本    V1.0.0
 * @日期    2019-5-9
 * @概要    接收网络端的数据，并处理
 ******************************************************************************
 * @注意
 *
 * 版权归chipintelli公司所有，未经允许不得使用或修改
 *
 ******************************************************************************
 */
#include <string.h>
#include "ci_log.h"
#include "cias_network_msg_protocol.h"
#include "cias_network_msg_send_task.h"
#include "prompt_player.h"
#include "cias_uart_protocol.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "cias_audio_data_handle.h"
#include "codec_manager.h"
#include "cias_common.h"
#include "cias_voice_plyer_handle.h"
#include "status_share.h"
#include "stream_buffer.h"
#include "cias_voice_upload.h"
#include "cias_aiot_protocol.h"
#define MAX_SEND_NUM 5
#define MAX_ACK_TIME 500 // MS
#define REQ_PLAY_DATA_SIZE (10 * 1024)

pro_wait_acktypedef wait_ackstruct;

extern StreamBufferHandle_t network_msg_rcv_stream_buffer;


bool recv_music_end_sem = false;
bool no_audio_data_flag = false;

int8_t package_data[NETWORK_RECV_BUFF_MAX_SIZE] = {0};
uint8_t gwork_mode = WORKING_APPLICATION_MODE;

static uint32_t response_timer_cnt = 0;
static uint8_t *login_info = NULL;

extern CiasAiotRunParamTypedef gCiasAiotRunParam;
uint32_t get_response_timer_cnt(void)
{
    response_timer_cnt = xTaskGetTickCount() - response_timer_cnt;
    ci_loginfo(LOG_MEDIA, "response_timer %d ms\n", response_timer_cnt);
    return response_timer_cnt;
}

uint32_t set_response_timer_cnt_start(void)
{
    return response_timer_cnt = xTaskGetTickCount();
}

void resp_file_config(void)
{
#if 0
    uint16_t login_info_len = 500;
    if(login_info == NULL)
    {
        login_info = pvPortMalloc(login_info_len);
        if(login_info == NULL)
        {
            ci_loginfo(LOG_MEDIA,"login info malloc error\n");
            return;
        }
    }
    requset_flash_ctl();
    flash_read(FLASH_SPI_PORT,(uint32_t)login_info,PRODUCT_INFO_SPIFLASH_START_ADDR,login_info_len);
    release_flash_ctl();
    cias_send_cmd_and_data(CI_PROFILE_MSG, login_info, login_info_len, DEF_FILL);
    ci_loginfo(LOG_MEDIA,"send login info\n");
#endif
}

static int32_t current_network_state = WIFI_DISCONNECT_STATE;
int32_t wifi_current_state_set(int32_t arg)
{
    if (arg > CLOUD_DISCONNECT_STATE || arg < WIFI_CONNECTED_STATE)
        return -1;

    current_network_state = arg;
    return 0;
}
int32_t wifi_current_state_get(void)
{
    return current_network_state;
}

/**
 * @brief 发送退出唤醒消息
 *
 */
void send_exit_wakeup_msg(char debug)
{
    ci_logdebug(LOG_USER, "send_exit_wakeup_msg = %d\n", debug);
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_EXIT_WAKEUP;
    if (pdPASS != send_msg_to_sys_task(&send_msg, NULL))
    {
        ci_logdebug(LOG_USER, "send_exit_wakeup_msg err\n");
    }
}

/**
 *
 * 判断是否需要回复ACK
 *
 **/
bool cmd_need_ack(uint16_t cmd)
{
    switch (cmd)
    {
    case DL_LOGIN_INFO_REQ:
    case DL_VER_REQ:
    case DL_WRITE_DATA_REQ:
    case DL_OTA_RSP:
    {
        return true;
    }
    default:
    {
        return false;
    }
    }
}

volatile bool start_recv_flag = false; // 向wifi请求数据
void network_recv_data_task(void *parameter)
{
    static int32_t recv_package_length = 0;
    static int32_t read_start_flag = 0;
    static int32_t curr_net_player_cache = 0;
    int32_t package_length = 0;
    int8_t msg_state = NET_MSG_IDE;
    BaseType_t err;
    cias_standard_head_t *data_header;
    uint32_t cache_empty_num = 0;
    int rx_size = 0;
    while (1)
    {
        rx_size = xStreamBufferReceive(network_msg_rcv_stream_buffer, &package_data[package_length], 1, pdMS_TO_TICKS(50));
        if (rx_size == 1)
        {
            if(++package_length >= NETWORK_RECV_BUFF_MAX_SIZE - 1)
            {
                mprintf("rcv err pakcage, package_length = %d\r\n", package_length);
                package_length = 0;
                msg_state = NET_MSG_IDE;
            }
            if (msg_state == NET_MSG_IDE)
            {
                // ci_logdebug(LOG_MEDIA, "msg_state == NET_MSG_IDE(%d)\r\n",package_length);
                msg_state = NET_MSG_HEAD;
            }
            else if (msg_state == NET_MSG_HEAD && package_length > (sizeof(cias_standard_head_t) - 2))
            {
                // for(int i =0 ;i<sizeof(cias_standard_head_t)-1;i++)
                // {
                //     ci_logdebug(LOG_MEDIA, "%x ",package_data[i]);
                // }
                // ci_logdebug(LOG_MEDIA, "\r\nmsg_state == NET_MSG_HEAD\r\n");
                data_header = (cias_standard_head_t *)package_data;
                if (data_header->magic == CIAS_STANDARD_MAGIC)
                {
                    msg_state = NET_MSG_DATE;
                }
                else
                    msg_state = NET_MSG_ERR;
            }
            else if (msg_state == NET_MSG_DATE && package_length > (data_header->len + sizeof(cias_standard_head_t) - 1)) // 等待数据接收完成
            {

                // ci_logdebug(LOG_MEDIA, "msg_state == NET_MSG_DATE\r\n");
                // memset(recv_data, 0, 1050);
                // memcpy(recv_data, package_data, package_length+1);

                data_header = (cias_standard_head_t *)package_data;
                recv_package_length = data_header->len + sizeof(cias_standard_head_t);

                wifi_msg_deal((uint8_t *)package_data, recv_package_length);

                package_length = 0;
                memset(package_data, 0, NETWORK_RECV_BUFF_MAX_SIZE);
                msg_state = NET_MSG_IDE;
            }

            if (msg_state == NET_MSG_ERR)
            {
                ci_logdebug(LOG_USER, "ci_standard_head_t error\r\n");
                package_length = 0;
                memset(package_data, 0, NETWORK_RECV_BUFF_MAX_SIZE);
                msg_state = NET_MSG_IDE;
            }
        }
        else
        {
            if (msg_state != NET_MSG_IDE) // 超时处理？？？
            {
                ci_logdebug(LOG_USER, "recv data timeout error(%d)\r\n", package_length);
                package_length = 0;
                memset(package_data, 0, NETWORK_RECV_BUFF_MAX_SIZE);
                msg_state = NET_MSG_IDE;
            }
        }
       
    }
}

/**
 * @brief WIFI 消息处理
 *
 * @param msg_buf 数据信息
 * @param msg_len 数据长度
 * @return int32_t RETURN_OK 处理成功，RETURN_ERR 数据异常
 */
// extern int32_t aux_aiot_aircondition_callback(uint8_t *msg_buf);
int32_t wifi_msg_deal(uint8_t *msg_buf, int32_t msg_len)
{
    if ((msg_buf == NULL) || (msg_len <= 0))
    {
        return RETURN_ERR;
    }
    aiot_wifi_msg_callback(msg_buf);
#if TUYA_IR_CTRL_DEMO_ENABLE || CI_IR_CTRL_DEMO_ENABLE // 涂鸦红外遥控器demo
    tuya_ir_wifi_msg_callback(msg_buf);
#endif

#if TENCENT_IOT_LIGHT_DEMO_ENABLE || TENCENT_IOT_IR_DEMO_ENABLE
    tencent_iot_network_callback(msg_buf);
#endif

    return RETURN_OK;
}
