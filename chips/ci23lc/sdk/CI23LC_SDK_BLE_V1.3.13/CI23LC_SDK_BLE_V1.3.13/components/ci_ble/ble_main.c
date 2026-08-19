#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "sdk_default_config.h"
#include "ci_log.h"
#include "ci_nvdata_manage.h"
#include "ble_communicate.h"
#include "ble_param_config.h"
#include "cias_ble_msg_deal.h"
#include "ci_nvdata_port.h"
#include "ci_spiflash.h"
#include "command_file_reader_v2.h"
#include "ble_main.h"
static uint8_t ble_set_data[BLE_SET_LEN] = {0};
BleInitCfg_t gBleInitCfg = {NULL};   //蓝牙初始化配置
/**
 * @brief 自定义蓝牙服务和特征参数
 * @param uuid: 蓝牙主服务的uuid
 * @param characteris_number: 对外展示的蓝牙特征个数
 * @param characteris: 蓝牙特征
 * {
 * 	.uuid：蓝牙特征uuid，
 * 	.attribute: 特征属性，
 * 	.handle: 设置成功后,芯片返回,本地保存用于数据收发
 * }
 */
ble_service_t ble_service_config =
    {
        .uuid = BLE_UUID_CIAS_SERVICE,
        .characteris_number = 3,
        .characteris[0] = {.uuid = BLE_UUID_CIAS_WRITE, .attribute = BLE_ATTRIBUTE_WRITE},
        .characteris[1] = {.uuid = BLE_UUID_CIAS_NOTIFY, .attribute = BLE_ATTRIBUTE_NOTIFY},
        .characteris[2] = {.uuid = BLE_UUID_CIAS_CMD_NOTIFY, .attribute = BLE_ATTRIBUTE_NOTIFY},
};

/**
 * @brief 默认的蓝牙广播数据,不包含mac和名称
 */
uint8_t adv_ind[BLE_ADV_LEN] = {0};
#if 0
/**
 * @brief 默认的蓝牙广播数据,不包含mac和名称
 */
uint8_t adv_ind[BLE_ADV_LEN] = {
    0x04,0x09,'B','L','E',
    0x02,0x01,0x06,
    0x07,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
};
#endif
#if USE_BLE_MOUDLE
/**
 * @brief 用户层ATT Read回调。
 *

 */
USE_XFI bool app_cb_att_read()
{

    uint8_t model_number = cmd_file_get_model_number(); // 网络数量
    // command_info_sheet = (command_info_v2_sheet_t*)cmd_info_malloc(sizeof(command_info_v2_sheet_t) * model_number);
    extern command_info_v2_sheet_t *command_info_sheet;
    for (uint8_t modle_id = 0; modle_id < model_number; modle_id++)
    {
        // //******获取asr_uuid信息*************
        // char asr_uuid[32] = {'\0'};       //asr文件uuid字符串
        // get_asr_uuid_by_asr_id(modle_id,asr_uuid);
        // (command_info_sheet+modle_id)->uuid = asr_uuid;
        // ci_loginfo(LOG_CMD_INFO, "asr_uuid=%s\t len=%d\r\n",(command_info_sheet+modle_id)->uuid, strlen((command_info_sheet+modle_id)->uuid));

        // //******获取该网络词条、播报等信息*************
        // get_cur_sheet_info(modle_id, command_info_sheet+modle_id);

        // //******保存网络总数信息*************
        // (command_info_sheet+modle_id)->model_number = model_number;

        command_info_v2_sheet_t *tmp_sheet = command_info_sheet + modle_id;
        uint8_t rf_read_data[BLE_MSG_DATA_MAX_SIZE] = {0};

        sprintf(rf_read_data, "asr_id:%d,dnn_id:%d,cur_model:%d,valid:%d,cmd number:%d,asr_uuid:%s", tmp_sheet->asr_id, tmp_sheet->dnn_id,
                tmp_sheet->cur_model_number, tmp_sheet->command_number, tmp_sheet->command_number, tmp_sheet->uuid); // valid 该数据不对?
        ble_send_payload(rf_read_data, strlen(rf_read_data), BLE_UUID_CIAS_CMD_NOTIFY);                              // 第一包数据，包含词条总数
        memset(rf_read_data, 0, BLE_MSG_DATA_MAX_SIZE);

        int read_index = 1;
        extern cmd_node *get_list_node(cmd_node * head, uint32_t offset);
        cmd_node *tmp = get_list_node(tmp_sheet->head, read_index);
        uint8_t rf_read_data_len = 0;

        for (read_index; tmp != NULL;) // 发送多组词条信息的组包数据
        {
            if ((rf_read_data_len + strlen(tmp->command_info_ble.cmd_str) + sizeof(voice_info_ble_t) + 9) > BLE_MSG_DATA_MAX_SIZE) // 超出240字节一包,发送数据
            {
                ble_send_payload(rf_read_data, rf_read_data_len, BLE_UUID_CIAS_CMD_NOTIFY);
                memset(rf_read_data, 0, BLE_MSG_DATA_MAX_SIZE);
                rf_read_data_len = 0;
            }
            else //一组数据(命令词长度:1Byte +命令词:nB +命令词ID:2B +语义ID:4B +置信度:1B +词条类型:1B +特殊词计数:1B \
									+播报类型:1B +播报音ID组数:1B +while(播报组数--){该组播报数:1B +播报音ID:n*2B...})
            {
                // 词条信息
                rf_read_data[rf_read_data_len++] = strlen(tmp->command_info_ble.cmd_str);
                memcpy(&rf_read_data[rf_read_data_len], tmp->command_info_ble.cmd_str, strlen(tmp->command_info_ble.cmd_str));
                rf_read_data_len += strlen(tmp->command_info_ble.cmd_str);

                rf_read_data[rf_read_data_len++] = tmp->command_info_ble.cmd_id / 256;
                rf_read_data[rf_read_data_len++] = tmp->command_info_ble.cmd_id % 256;
                rf_read_data[rf_read_data_len++] = (uint8_t)(tmp->command_info_ble.semantic_id >> 24);
                rf_read_data[rf_read_data_len++] = (uint8_t)(tmp->command_info_ble.semantic_id >> 16);
                rf_read_data[rf_read_data_len++] = (uint8_t)(tmp->command_info_ble.semantic_id >> 8);
                rf_read_data[rf_read_data_len++] = (uint8_t)(tmp->command_info_ble.semantic_id);
                rf_read_data[rf_read_data_len++] = tmp->command_info_ble.score;
                rf_read_data[rf_read_data_len++] = tmp->command_info_ble.wake_up_flag;
                rf_read_data[rf_read_data_len++] = tmp->command_info_ble.special_wait_count;
                // 播报信息
                rf_read_data[rf_read_data_len++] = tmp->command_info_ble.voice_info_ble.select_type;
                rf_read_data[rf_read_data_len++] = tmp->command_info_ble.voice_info_ble.option_number;
                uint8_t voice_id_index = 0;
                for (uint8_t i = 0; i < tmp->command_info_ble.voice_info_ble.option_number; i++)
                {
                    uint8_t combination_number = tmp->command_info_ble.voice_info_ble.combination_number[i];
                    rf_read_data[rf_read_data_len++] = combination_number;
                    for (uint8_t j = 0; j < combination_number; j++)
                    {
                        rf_read_data[rf_read_data_len++] = (uint8_t)(tmp->command_info_ble.voice_info_ble.voice_id_arr[voice_id_index] >> 8);
                        rf_read_data[rf_read_data_len++] = (uint8_t)(tmp->command_info_ble.voice_info_ble.voice_id_arr[voice_id_index++]);
                    }
                }
                read_index++;
            }
            tmp = get_list_node(tmp_sheet->head, read_index);
        }
        ble_send_payload(rf_read_data, rf_read_data_len, BLE_UUID_CIAS_CMD_NOTIFY); // 发送最后一包数据
        memset(rf_read_data, 0, BLE_MSG_DATA_MAX_SIZE);

        sprintf(rf_read_data, "get cmd fnish:%d", tmp_sheet->model_number); // 结束标志信息
        ble_send_payload(rf_read_data, strlen(rf_read_data), BLE_UUID_CIAS_CMD_NOTIFY);
    }
}
#endif

// 蓝牙数据接收消息队列
static QueueHandle_t ble_recever_queue = NULL;

/**
 * @brief 设置蓝牙串口流控
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_uart_control_flow_set()
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_SET_UART_FLOW;
    ble_set_data[2] = 0x01;
    ble_set_data[2] = 0x00;
    if (ble_send_packet(ble_set_data, 1 + BLE_SET_HEAD_LEN, BLE_CMD_SET_UART_FLOW) == false)
    {
        mprintf("ble_uart_control_flow_set error\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 设置蓝牙串口波特率
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_uart_baud_set(uint8_t *baud)
{
    uint8_t len = strlen(baud);
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_SET_UART_BAUD;
    ble_set_data[2] = len;
    memcpy(&ble_set_data[3], baud, len);
    if (ble_send_packet(ble_set_data, len + BLE_SET_HEAD_LEN, BLE_CMD_SET_UART_BAUD) == false)
    {
        mprintf("ble_uart_baud_set error\r\n");
        return false;
    }
    return true;

}

/**
 * @brief 更新蓝牙连接参数
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_conn_updata_set()
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_CONN_UPDATE;
    ble_set_data[2] = BLE_CONN_UPDATA_LEN;
    memcpy(&ble_set_data[3], "\x06\x00\x06\x00\x00\x00\x20\x03", BLE_CONN_UPDATA_LEN);
    if (ble_send_packet(ble_set_data, BLE_CONN_UPDATA_LEN + BLE_SET_HEAD_LEN, BLE_CMD_CONN_UPDATE) == false)
    {
        mprintf("ble_conn_updata_set error\r\n");
        return false;
    }
    return true;

}
/**
 * @brief 查询蓝牙软件版本号
 * @return 成功返回true;失败返回false
 */
USE_XFI bool ble_version_req()
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_VERSION_REQUEST;
    ble_set_data[2] = 0x00;
    if (ble_send_packet(ble_set_data, BLE_SET_HEAD_LEN, BLE_CMD_VERSION_REQUEST) == false)
    {
        mprintf("ble_version_req error\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 软件命令复位蓝牙芯片
 * @return 成功返回true;失败返回false
 */
USE_XFI bool ble_reset_software()
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_RESET_CHIP_REQ;
    ble_set_data[2] = 0;
    if (ble_send_packet(ble_set_data, BLE_SET_HEAD_LEN, BLE_CMD_RESET_CHIP_REQ) == false)
    {
        mprintf("ble_reset_software error\r\n");
        return false;
    }
    ble_port_protocol_hw_init(UART_BaudRate115200);
    return true;
}

/**
 * @brief 软件复位全系统
 *        
 */
USE_XFI void dpmu_software_reset_system_config_ble(void)
{
    ble_reset_software(); 
    dpmu_software_reset_system_config();
}

/**
 * @brief 于请求蓝牙状态
 * @return 成功返回true;失败返回false
 */
USE_XFI bool ble_status_request()
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_STATUS_REQ;
    ble_set_data[2] = 0;
    if (ble_send_packet(ble_set_data, BLE_SET_HEAD_LEN, BLE_EVENT_STATUS_REP) == false)
    {
        mprintf("ble_status_request error...\r\n");
        return false;
    }
    // mprintf("ble_status_request ok\r\n");
    return true;
}
/**
 * @brief 蓝牙协议栈固件串口下载
 * @var file_size :uint16 类型,固件头两个字节
 * @var len :固件参数分包传输的大小
 * @return 固件下载完成返回true;下载失败,立即退出并返回false
 */
USE_XFI bool ble_patch_process(void)
{
    int offset;               // read offset.
    int len;                  // read len.
    uint32_t addr, file_size; // file size.
    offset = PATCH_FILE_LEN_SIZE;
    if(get_userfile_addr(BLE_FIRMWARE_ID, &addr, &file_size))
    {
        ci_logerr(CI_LOG_ERROR, "user_file folder not found ble fimware !\n");
        return false;
    }
    uint8_t ble_patch_data[256] = {0}; 
    post_read_flash(ble_patch_data, addr, PATCH_FILE_LEN_SIZE);
    uint16_t crc_cal = crc16_ccitt(0, ble_patch_data, PATCH_FILE_LEN_SIZE);

    while (offset < file_size)
    {
        post_read_flash(ble_patch_data, addr+offset, PATCH_BLOCK_LEN_SIZE);
        crc_cal = crc16_ccitt(crc_cal, ble_patch_data, PATCH_BLOCK_LEN_SIZE);
        len = ble_patch_data[0];
        offset += PATCH_BLOCK_LEN_SIZE;
        post_read_flash(ble_patch_data, addr+offset, len);
        crc_cal = crc16_ccitt(crc_cal, ble_patch_data, len);
        if (ble_send_packet(ble_patch_data, len, BLE_EVENT_TYPE_PATCH) == false)
        {
            return false;
        }
        offset += len;
        if (offset > file_size)
        {
            return false;
        }
    } // end of while(offset < file_size)
    if (BLE_PATCH_CRC != crc_cal)
    {
        ci_logerr(CI_LOG_ERROR, "ble firmware crc erro! %x\n", crc_cal);
        return false;
    }
    mprintf("ble firmware download ok!\r\n"); 
    return true;
}

/**
 * @brief 设置蓝牙mac地址
 * @param mac: mac数据,6个字节
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_mac_set(uint8_t *mac)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_SET_ADDR;
    ble_set_data[2] = BLE_MAC_LEN;
    memcpy(&ble_set_data[3], mac, BLE_MAC_LEN);
    if (ble_send_packet(ble_set_data, BLE_MAC_LEN + BLE_SET_HEAD_LEN, BLE_CMD_SET_ADDR) == false)
    {
        mprintf("ble_mac_set error\r\n");
        return false;
    }        
    return true;
}

/**
 * @brief 设置蓝牙广播数据
 * @param adv_data: 需发送的广播数据
 * @param adv_len: 广播数据有效长度
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_adv_data_set(uint8_t *adv_data, uint8_t adv_len)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD; // 设置广播数据
    ble_set_data[1] = BLE_CMD_ADV_DATA;
    ble_set_data[2] = adv_len;
    memcpy(&ble_set_data[3], adv_data, adv_len);
    if (ble_send_packet(ble_set_data, adv_len + BLE_SET_HEAD_LEN, BLE_CMD_ADV_DATA) == false)
    {
        mprintf("ble_adv_data_set error\r\n");
        return false;
    } 
    return true;
}

/**
 * @brief 设置蓝牙广播扫描回复数据
 * @param adv_data: 需发送的广播数据
 * @param adv_len: 广播数据有效长度
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_adv_rsp_data_set(uint8_t *adv_rsp_data, uint8_t adv_rsp_len)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD; // 设置广播回复数据
    ble_set_data[1] = BLE_CMD_ADV_RSP_DATA;
    ble_set_data[2] = adv_rsp_len;
    memcpy(&ble_set_data[3], adv_rsp_data, adv_rsp_len);
    if (ble_send_packet(ble_set_data, adv_rsp_len + BLE_SET_HEAD_LEN, BLE_CMD_ADV_RSP_DATA) == false)
    {
        mprintf("ble_adv_rsp_data_set error\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 设置蓝牙广播状态(默认关闭)
 * @param adv_status: 蓝牙广播状态
 * @param adv_channel: 蓝牙广播频道
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_adv_status_set(ble_adv_status_t adv_status, ble_adv_channel_t adv_channel)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_ADV_STATUS;
    ble_set_data[2] = BLE_SET_ADV_CHAN_LEN;
    ble_set_data[3] = adv_status;
    if (adv_channel == ADV_CHAN_ALL)
    {
        ble_set_data[4] = adv_channel;
        ble_set_data[5] = 0;
    }
    else
    {
        ble_set_data[4] = 1;
        ble_set_data[5] = adv_channel;
    }

    // ble_adv_data[5] = ADV_CHAN_NUMBER;
    if (ble_send_packet(ble_set_data, BLE_SET_ADV_CHAN_LEN + BLE_SET_HEAD_LEN, BLE_CMD_ADV_STATUS) == false)
    {
        mprintf("ble_adv_status_set error\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 设置蓝牙广播名称
 * @param adv_name: 需广播名称数据
 * @param len: 广播名称长度
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_adv_name_set(uint8_t *adv_name, uint8_t len)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_SET_NAME;
    ble_set_data[2] = len;
    memcpy(&ble_set_data[3], adv_name, len);
    if (ble_send_packet(ble_set_data, len + BLE_SET_HEAD_LEN, BLE_CMD_SET_NAME) == false)
    {
        mprintf("ble_adv_name_set false\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 设置蓝牙广播扫描参数(默认关闭)
 * @param adv_scan_status: 蓝牙广播扫描状态
 * @param adv_scan_channel: 蓝牙广播扫描频道
 * @param adv_scan_len: 蓝牙扫描到的广播包数据长度
 * 例如,需要设置接收启英遥控器发送的广播包，收到数据为:{02 2A 27 02 25 46 0B AF 43 98 AF 11 07 A5 79 26 FF 00 00 00 FF 00 00 01 03 00 00 00 00 04 09 42 4C 45 00 00 00 00 00 00 00 00}
 * 其中,头部为: cmd:0x02, opcode:0x2a, len:0x27, 2byte head：0x42 0x25, 6byte Mac:0x46 0x0B 0xAF 0x43 0x98 0xAF
 * 有效数据为byte[11]:0x11到结尾,总共31字节
 * 需要将adv_scan_len设置为有效数据+12,即43字节。
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_adv_scan_status_set(ble_adv_scan_status_t adv_scan_status, ble_adv_scan_channel_t adv_scan_channel,
                             uint8_t adv_scan_len)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD; // 设置广播扫描信道
    ble_set_data[1] = BLE_CMD_ADV_SCAN_STATUS;
    ble_set_data[2] = BLE_SET_ADV_SCAN_LEN;
    ble_set_data[3] = adv_scan_status;
    ble_set_data[4] = adv_scan_channel;
    ble_set_data[5] = adv_scan_len;
    if (ble_send_packet(ble_set_data, BLE_SET_ADV_SCAN_LEN + BLE_SET_HEAD_LEN, BLE_CMD_ADV_SCAN_STATUS) == false)
    {
        mprintf("ble_adv_scan_status_set error...\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 设置蓝牙广播扫描效率参数(默认不设置)
 * @param adv_scan_efficiency: 蓝牙扫描效率
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_adv_scan_efficiency_set(ble_adv_scan_efficiency_t adv_scan_efficiency)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_ADV_SCAN_EFFICIENCY;
    ble_set_data[2] = BLE_SET_ADV_SCAN_EFFICIENCY_LEN;
    ble_set_data[3] = adv_scan_efficiency;
    ble_set_data[4] = 0x00;
    ble_set_data[5] = 0x10;
    ble_set_data[6] = 0x00;
    if (ble_send_packet(ble_set_data, BLE_SET_ADV_SCAN_EFFICIENCY_LEN + BLE_SET_HEAD_LEN, BLE_CMD_ADV_SCAN_EFFICIENCY) == false)
    {
        mprintf("ble_adv_scan efficiency set error\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 设置发送功率
 * @param power: 功率等级
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_tx_power_set(ble_tx_power_t power)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_SET_TX_POWER;
    ble_set_data[2] = 1;
    ble_set_data[3] = power;
    if (ble_send_packet(ble_set_data, 1 + BLE_SET_HEAD_LEN, BLE_CMD_SET_TX_POWER) == false)
    {
        mprintf("ble_tx_power_set error\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 删除非系统服务和特征
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_service_delete()
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_DELETE_SERVICE;
    ble_set_data[2] = BLE_UUID_DELETE_LEN;
    if (ble_send_packet(ble_set_data, BLE_UUID_DELETE_LEN + BLE_SET_HEAD_LEN, BLE_CMD_DELETE_SERVICE) == false)
    {
        mprintf("ble_service_delete error...\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 添加服务
 * @param uuid: 用户配置的蓝牙服务uuid
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_service_add(uint16_t uuid)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD; // 添加蓝牙主服务
    ble_set_data[1] = BLE_CMD_ADD_SERVICE;
    ble_set_data[2] = BLE_UUID_ADD_LEN;
    ble_set_data[3] = BLE_UUID_LEN;
    ble_set_data[4] = uuid % 256;
    ble_set_data[5] = uuid / 256;
    if (ble_send_packet(ble_set_data, BLE_UUID_ADD_LEN + BLE_SET_HEAD_LEN, BLE_EVENT_UUID_HANDLE) == false)
    {
        mprintf("ble_service_add error...\r\n");
        return false;
    }
        
    return true;
}

/**
 * @brief 添加特征
 *  @param characteris: 用户配置的蓝牙特征参数，包含特征uuid,特征属性,特征句柄
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_characteris_add(ble_characteris_t *characteris)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_ADD_CHARACTERISTIC;
    ble_set_data[2] = BLE_ADD_HANDLE_LEN;
    ble_set_data[3] = characteris->attribute;
    ble_set_data[4] = BLE_UUID_LEN;
    ble_set_data[5] = characteris->uuid % 256;
    ble_set_data[6] = characteris->uuid / 256;
    if (ble_send_packet(ble_set_data, BLE_ADD_HANDLE_LEN + BLE_SET_HEAD_LEN, BLE_EVENT_UUID_HANDLE) == false)
    {
        mprintf("ble_characteris_add error...\r\n");
        return false;
    }
    characteris->handle = ble_handle;
    return true;
}

/**
 * @brief 设置蓝牙服务和特征
 * @param ble_service_config: 用户配置的蓝牙服务和特征参数
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_service_set(ble_service_t *ble_service_config)
{
    if(!ble_service_add(ble_service_config->uuid))
        return false;
    for (size_t i = 0; i < ble_service_config->characteris_number; i++) // 添加蓝牙特征
    {
        if(!ble_characteris_add(&ble_service_config->characteris[i]))
        {
            return false;
        }
        //mprintf("add character %x ok %x \r\n", ble_service_config->characteris[i].uuid, ble_service_config->characteris[i].handle);
    }
    return true;
}

/**
 * @brief 获取特征UUID对应的handle
 * @param uuid: 必须是蓝牙特征UUID
 * @return 蓝牙特征对应的协议栈handle，用于和手机端进行数据收发
 */
USE_XFI uint8_t get_character_handle(uint16_t uuid)
{
    for (size_t i = 0; i < ble_service_config.characteris_number; i++)
    {
        if (uuid == ble_service_config.characteris[i].uuid)
        {
            // mprintf("get character ok %x ",ble_service_config.characteris[i].handle);
            return ble_service_config.characteris[i].handle;
        }
    }
}

/**
 * @brief 断开蓝牙连接
 * @return 成功返回true;失败返回false
 */
USE_XFI bool ble_disconnect()
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    if (ble_status == BLE_STATUS_DISCONNECT)
        return true;
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_DISCONNECT;
    ble_set_data[2] = 0;
    if (ble_send_packet(ble_set_data, BLE_SET_HEAD_LEN, BLE_CMD_DISCONNECT) == false)
    {
        mprintf("ble_disconnect error\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 设置特定载波频段
 * @param power: 功率等级
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_frequency_band_set(uint8_t data)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = 0x70;
    ble_set_data[2] = 2;
    ble_set_data[3] = 1;
    ble_set_data[4] = data;
	ble_send_packet(ble_set_data, 2 + BLE_SET_HEAD_LEN, 0x70);
    mprintf("ble_frequency_band_set ok\r\n");
    return true;
}

/**
 * @brief 设置频偏
 * @param xtal: 频偏寄存器：0~0x7f
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_xtal_set(uint8_t xtal)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = 0x66;
    ble_set_data[2] = 1;
    ble_set_data[3] = xtal;
    if (ble_send_packet(ble_set_data, 1 + BLE_SET_HEAD_LEN, 0x66) == false)
    {
        mprintf("ble_xtal_set error\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 设置广播参数
 * @param data: adv间隔时间，0.625ms
 * @return 设置成功返回true;失败返回false
 */
USE_XFI bool ble_adv_para_set(uint16_t data)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_ADV_PARA;
    ble_set_data[2] = 2;
    ble_set_data[3] = data;
	ble_set_data[4] = data>>8;
    if (ble_send_packet(ble_set_data, 2 + BLE_SET_HEAD_LEN, BLE_CMD_ADV_PARA) == false)
    {
        mprintf("ble_adv_para_set error\r\n");
        return false;
    }
    mprintf("ble_adv_para_set ok\r\n");
    return true;
}

#if BLE_PAIRING_ENBLE
/**
 * @brief 设置蓝牙配对加密模式
 * @return 成功返回true;失败返回false
 */
USE_XFI bool ble_set_pairing(ble_pairing_mode_t ble_pairing_mode)
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_SET_PAIRING;
    ble_set_data[2] = BLE_SET_PAIRING_LEN;
    ble_set_data[3] = ble_pairing_mode;
    if (ble_send_packet(ble_set_data, BLE_SET_HEAD_LEN + BLE_SET_PAIRING_LEN, BLE_CMD_SET_PAIRING) == false)
    {
        mprintf("ble_set_pairing error...\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 开始蓝牙配对
 * @return 成功返回true;失败返回false
 */
USE_XFI bool ble_start_pairing()
{
    memset(ble_set_data, 0, BLE_SET_LEN);
    if (ble_status != BLE_STATUS_CONNECT)
        return false;
    ble_set_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_data[1] = BLE_CMD_START_PAIRING;
    ble_set_data[2] = 0;
    if (ble_send_packet(ble_set_data, BLE_SET_HEAD_LEN, BLE_CMD_START_PAIRING) == false)
    {
        mprintf("ble_start_pairing error...\r\n");
        return false;
    }
    return true;
}

/**
 * @brief 设置蓝牙配对加密nv数据
 * @return 成功返回true;失败返回false
 */
USE_XFI bool ble_set_nvram(uint8_t *nv_data)
{
    uint8_t ble_set_nvram_data[PAIRING_DATA_LEN + BLE_SET_HEAD_LEN];

    ble_set_nvram_data[0] = BLE_PACK_TPYE_CMD;
    ble_set_nvram_data[1] = BLE_CMD_SET_NVRAM;
    ble_set_nvram_data[2] = PAIRING_DATA_LEN;
    memcpy(&ble_set_nvram_data[3], nv_data, PAIRING_DATA_LEN);
    if (ble_send_packet(ble_set_nvram_data, BLE_SET_HEAD_LEN + PAIRING_DATA_LEN, BLE_CMD_SET_NVRAM) == false)
    {
        mprintf("ble_set_nvram error ...\r\n");
        return false;
    }
    return true;
}
/**
 * @brief 蓝牙配对秘钥初始化，蓝牙和手机app配对成功，下次蓝牙开机需要读取配对秘钥，设置为初始秘钥(5*34字节)
 * @return 成功返回true;失败返回false
 */
USE_XFI void ble_pairing_data_init(void)
{
    uint16_t real_len = 0;
    uint8_t pairing_data[PAIRING_DATA_LEN] = {0};
    if(!cinv_item_read(NVDATA_ID_PAIRING_DATA, PAIRING_DATA_LEN, pairing_data, &real_len))
    {
        return;
    }
    if ((pairing_data[0] == 1) && (pairing_data[34] == 2))
    {
        ble_set_nvram(pairing_data);
    }
    // ble_pairing_status = BLE_PAIRING_STATE_SUCCESS;  //开机检测到配的秘钥，蓝牙模对模式设置为为success
}

/**
 * @brief 蓝牙配对秘钥保存(5*34字节)
 * @return 成功返回true;失败返回false
 */
USE_XFI void ble_pairing_data_write(uint8_t *pairing_data)
{
    cinv_item_init(NVDATA_ID_PAIRING_DATA, PAIRING_DATA_LEN, pairing_data);
    ble_pairing_status = BLE_PAIRING_STATE_SUCCESS;
}

#endif
/**
 * @brief 将接收到的蓝牙消息帧，发送到蓝牙数据接收消息队列
 * @param msg: 需要发送的蓝牙接收消息帧
 */
USE_XFI void ble_send_recv_msg(ble_msg_data_t *msg, BaseType_t *xHigherPriorityTaskWoken)
{
    if (0 != check_curr_trap())
    {
        xQueueSendFromISR(ble_recever_queue, msg, xHigherPriorityTaskWoken);
    }
    else
    {
        xQueueSend(ble_recever_queue, msg, portMAX_DELAY);
    }
}

#if CIAS_BLE_HEART_TIMER_ENABLE
USE_XFI void ble_heart_timer_callback()
{
    if(false == ble_status_request())
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        if(false == ble_status_request())       //请求蓝牙状态,二次确定
        dpmu_software_reset_system_config_ble();
    }
}
#endif

USE_XFI void hexToString(unsigned char *hex, char *string, int len)
{
    for (int i = 0; i < len; i++)
    {
        sprintf(string + i * 2, "%02X", hex[i]);
    }
}
/**
 * @brief 获取蓝牙mac,将flash unique_id(16字节)转换为蓝牙mac id(字节)
 * @param 
 */
USE_XFI void ble_mac_get(uint8_t *mac_id)
{
    uint8_t flash_unique_id[16] = {0};
    uint8_t temp_unique_id[5];
    uint16_t crc_cal;
    post_read_flash_unique_id((uint8_t *)flash_unique_id);
    for (int i = 0; i < 3; i++)
    {
        memset(temp_unique_id, 0, 5);
        for (int j = 0; j < 5; j++)
            temp_unique_id[j] = flash_unique_id[j*3 + i];
        crc_cal = crc16_ccitt(0, temp_unique_id, 5);
        mac_id[i*2] = crc_cal >> 8;
        mac_id[i*2 + 1] = crc_cal & 0xFF;
    }
}
/**
 * @brief 蓝牙主任务，下载蓝牙固件，启动蓝牙广播和扫描广播，等待接收手机和遥控器数据
 */
USE_XFI void ble_main_task(void)
{
    ble_msg_data_t ble_recever_packet;
    bool ble_set_status = true;
    ble_recever_queue = xQueueCreate(BLE_RCV_MSG_QUEUE_SIZE, sizeof(ble_msg_data_t)); // 创建蓝牙数据接收消息队列
    if (ble_recever_queue == NULL)
    {
        mprintf("ble_recever_queue error.\r\n");
        goto ble_error;
    }
    if (!ble_port_mutex_create())
    {
        mprintf("ble_port_mutex_create error.\r\n");
        goto ble_error;
    }
    ble_reset_gpio_init();
 ble_load_start:  
    ble_port_protocol_hw_init(UART_BaudRate115200);
    while (!ble_patch_process())
    {
        mprintf("reload ble firmware..\r\n");
        ble_reset_hardware();
    }
    if(ble_uart_baud_set("921600"))
        ble_port_protocol_hw_init(UART_BaudRate921600);
    else
    {
        ble_reset_software();
        goto ble_load_start;
    }  
    vTaskDelay(pdMS_TO_TICKS(10));
    uint8_t ble_adv_name_len = 0;    //广播名称长度
    uint8_t mac_id[6] = {0};
    ble_mac_get(mac_id);
    uint8_t adv_name[BLE_ADV_NAME_MAX_LEN] = {0};
    if (strlen(BLE_USER_DEFINE_ADV_NAME_CONTENT) > 0)
    {
        #if USE_CI_APPLET_ENABEL
        if(strlen(BLE_USER_DEFINE_ADV_NAME_CONTENT) + (BLE_ADV_NAME_APPEND_FLASH_ID ? 5 : 0) > 18)   //启英物联小程序最大支持18字节
        {
            ci_logerr(CI_LOG_ERROR, "set adv name too long , max 18 bytes!\n");
            return;
        }
        #endif
        #if BLE_ADV_NAME_APPEND_FLASH_ID
        if (strlen(BLE_USER_DEFINE_ADV_NAME_CONTENT) > (BLE_ADV_NAME_MAX_LEN - 5)) // 广播名称最大支持24+5字节
        #else
        if (strlen(BLE_USER_DEFINE_ADV_NAME_CONTENT) > BLE_ADV_NAME_MAX_LEN) // 广播名称最大支持29字节
        #endif
        {
            ci_logerr(CI_LOG_ERROR, "set adv name too long, max 29 bytes!\n");
            ble_adv_name_len = BLE_ADV_NAME_MAX_LEN;
            return;
        }
        else
        {
            ble_adv_name_len = strlen(BLE_USER_DEFINE_ADV_NAME_CONTENT);
        }
        sprintf(adv_name, "%s", BLE_USER_DEFINE_ADV_NAME_CONTENT);
        #if BLE_ADV_NAME_APPEND_FLASH_ID
            ble_adv_name_len = strlen(BLE_USER_DEFINE_ADV_NAME_CONTENT);
            // 默认的蓝牙广播名称
            sprintf(&adv_name[ble_adv_name_len], "_%02X", mac_id[0]);
            sprintf(&adv_name[ble_adv_name_len + 3], "%02X", mac_id[5]);
            ble_adv_name_len += 5;         
        #endif
    }
    else
    {
        ci_logerr(CI_LOG_ERROR, "please set adv name!\n");
        sprintf(adv_name, "%s", "CI_BLE");
        ble_adv_name_len = 6;
        #if BLE_ADV_NAME_APPEND_FLASH_ID
            // 默认的蓝牙广播名称
            sprintf(&adv_name[ble_adv_name_len], "_%02X", mac_id[0]);
            sprintf(&adv_name[ble_adv_name_len + 3], "%02X", mac_id[5]);
            ble_adv_name_len += 5;         
        #endif
    }
    mprintf("adv_name = %s\r\n", adv_name);
    //mprintf("ble_adv_name_len = %d\r\n", ble_adv_name_len);
    if(!ble_xtal_set(0x02))                   //公版蓝牙硬件模块设置频偏
        ble_set_status = false;     
    if(!ble_mac_set(mac_id))
        ble_set_status = false;               // flash ID作为蓝牙mac地址 */
    if(!ble_adv_name_set(adv_name, ble_adv_name_len))
        ble_set_status = false;          // 设置广播名字-local (小程序发现蓝牙设备时，会有两个信息，local name和name,注意区分)             
    if(gBleInitCfg.ble_adv_data_init)                      // 初始化蓝牙广播数据段-NAME
    {
        gBleInitCfg.ble_adv_data_init(adv_ind, adv_name);
    }
    if(!ble_adv_data_set(adv_ind, 13 + ble_adv_name_len))
        ble_set_status = false;      // 设置蓝牙广播数据
    if(!ble_adv_status_set(ADV_ON, ADV_CHAN_2))
        ble_set_status = false;                // 开启蓝牙广播
    if(!ble_tx_power_set(BLE_POWER_3dB))
        ble_set_status = false;                // 设置发送功率                        
    if(!ble_service_delete())
        ble_set_status = false;                                  // 删除非系统服务和特征
    if(!ble_service_set(&ble_service_config))
        ble_set_status = false;                  // 设置蓝牙服务

#if CIAS_BLE_SCAN_ENABLE
    ble_adv_scan_efficiency_set(LOW);                                                   // 设置蓝牙广播扫描效率
    ble_adv_scan_status_set(ADV_SCAN_ON, ADV_SCAN_CHAN_0, CIAS_BLE_SCAN_REMOTE_CONTROL_LEN); // 设置蓝牙广播扫描数据
    
#endif
    dev_state_init();
    if(!ble_status_request())
        ble_set_status = false;
#if BLE_PAIRING_ENBLE
    ble_set_pairing(BLE_PAIRING_MODE);
    ble_pairing_data_init();
#endif
    if(ble_set_status == false)
    {
        ble_reset_software();
        goto ble_load_start;
    }
#if CIAS_BLE_HEART_TIMER_ENABLE
    TimerHandle_t ble_heart_timer = xTimerCreate("ble_heart_timer", pdMS_TO_TICKS(10000), true, (void *)0, ble_heart_timer_callback);
    if(!ble_heart_timer)
    {
        mprintf("ble_heart_timer create error...\r\n");
        goto ble_error;
    }
    xTimerStart(ble_heart_timer, 0);
#endif
    while (1)
    {
        BaseType_t rst = xQueueReceive(ble_recever_queue, &ble_recever_packet, portMAX_DELAY);
        if (rst)
        {
#if CIAS_BLE_HEART_TIMER_ENABLE
            xTimerReset(ble_heart_timer, 10);    //正常接收蓝牙数据时,复位定时器,避免定时查询蓝牙状态时,蓝牙未及时响应导致的误重启问题.
#endif
            switch (ble_recever_packet.opcode)
            {
            case BLE_EVENT_CONN_REP:
                // 收到消息类型是手机建立连接
                mprintf("ble app connect\r\n");
                //ble_conn_updata_set();
#if BLE_PAIRING_ENBLE
                if (ble_pairing_status == BLE_PAIRING_STATE_IDLE)
                {
                    if (ble_pairing_status == BLE_PAIRING_STATE_IDLE)
                    {
                        vTaskDelay(pdMS_TO_TICKS(2000));
                        ble_pairing_status = ble_start_pairing();
                    }
                }
#endif
                if(gBleInitCfg.ble_connected_callback)
                {
                    gBleInitCfg.ble_connected_callback();
                }
                break;
            case BLE_EVENT_DIS_REP:
                // 蓝牙连接断开
                mprintf("ble app disconnect\r\n");
                 if(gBleInitCfg.ble_disconnected_callback)
                {
                    gBleInitCfg.ble_disconnected_callback();
                }
                break;
            case BLE_EVENT_DATA_REP:
                // 收到消息类型是手机发送的蓝牙消息
                if (get_character_handle(BLE_UUID_CIAS_WRITE) == ble_recever_packet.msg_data[0]) // 消息里面的handle字段和定义的特征UUID匹配
                {
                    if (ble_recever_packet.length > 2)
                    {
                        if(gBleInitCfg.ble_recv_data_callback)
                        {
                            gBleInitCfg.ble_recv_data_callback(&ble_recever_packet.msg_data[2], ble_recever_packet.length - 2);
                        }
                    }
                    else // 只有包头和handle，不处理
                    {
                    }
                }
                break;

            case BLE_EVENT_NVRAM_REP:
#if BLE_PAIRING_ENBLE
                // 收到消息类型是配的成功,存储加密秘钥到NV
                ble_pairing_status = BLE_PAIRING_STATE_SUCCESS;
                #if 0
                for (size_t i = 0; i < ble_recever_packet.length; i++)
                {
                    mprintf("%x ", ble_recever_packet.msg_data[i]);
                    if (i % 34 == 33)
                    {
                        mprintf("\r\n");
                    }
                }
                #endif
                ble_pairing_data_write(ble_recever_packet.msg_data);
#endif
                break;
            case BLE_EVENT_ADV_REP:
                #if 0
                for (size_t i = 0; i < ble_recever_packet.length; i++)
                {
                    mprintf("%x ", ble_recever_packet.msg_data[i]);
                }
                mprintf("\r\n"); 
                #endif
#if CIAS_BLE_SCAN_ENABLE
                gBleInitCfg.ble_recv_adv_data_callback(ble_recever_packet.msg_data, ble_recever_packet.length);
#endif
                break;

            default:
                break;
            }
        }
    }

ble_error:
    while(1);

}
