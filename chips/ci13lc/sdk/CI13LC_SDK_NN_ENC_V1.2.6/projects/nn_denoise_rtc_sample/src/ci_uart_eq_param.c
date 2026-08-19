/***************************************************************************************************************/

#include "ci_uart_eq_param.h"
#include "sdk_default_config.h"

#if EQ_TEST_ENABLE

#include <stdarg.h>
#include <stdio.h>
#include "ci_log.h"
#include <stdbool.h>
#include <stdlib.h>
#include "romlib_api.h"
#include "ci13lc_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "portmacro.h"
#include "ci_nvdata_manage.h"
#include "user_config.h"

static QueueHandle_t eq_msg_recv_queue = NULL;
static int8_t eq_port = 0;

eq_param_t init_eq_drc_val = {0};
extern npu_eq_param_t *g_eq_param;
extern npu_drc_param_t *g_drc_param;

static unsigned int CRC_table[256] =
{
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50A5,0x60C6,0x70E7,
    0x8108,0x9129,0xA14A,0xB16B,0xC18C,0xD1AD,0xE1CE,0xF1EF,
    0x1231,0x0210,0x3273,0x2252,0x52B5,0x4294,0x72F7,0x62D6,
    0x9339,0x8318,0xB37B,0xA35A,0xD3BD,0xC39C,0xF3FF,0xE3DE,
    0x2462,0x3443,0x0420,0x1401,0x64E6,0x74C7,0x44A4,0x5485,
    0xA56A,0xB54B,0x8528,0x9509,0xE5EE,0xF5CF,0xC5AC,0xD58D,
    0x3653,0x2672,0x1611,0x0630,0x76D7,0x66F6,0x5695,0x46B4,
    0xB75B,0xA77A,0x9719,0x8738,0xF7DF,0xE7FE,0xD79D,0xC7BC,
    0x48C4,0x58E5,0x6886,0x78A7,0x0840,0x1861,0x2802,0x3823,
    0xC9CC,0xD9ED,0xE98E,0xF9AF,0x8948,0x9969,0xA90A,0xB92B,
    0x5AF5,0x4AD4,0x7AB7,0x6A96,0x1A71,0x0A50,0x3A33,0x2A12,
    0xDBFD,0xCBDC,0xFBBF,0xEB9E,0x9B79,0x8B58,0xBB3B,0xAB1A,
    0x6CA6,0x7C87,0x4CE4,0x5CC5,0x2C22,0x3C03,0x0C60,0x1C41,
    0xEDAE,0xFD8F,0xCDEC,0xDDCD,0xAD2A,0xBD0B,0x8D68,0x9D49,
    0x7E97,0x6EB6,0x5ED5,0x4EF4,0x3E13,0x2E32,0x1E51,0x0E70,
    0xFF9F,0xEFBE,0xDFDD,0xCFFC,0xBF1B,0xAF3A,0x9F59,0x8F78,
    0x9188,0x81A9,0xB1CA,0xA1EB,0xD10C,0xC12D,0xF14E,0xE16F,
    0x1080,0x00A1,0x30C2,0x20E3,0x5004,0x4025,0x7046,0x6067,
    0x83B9,0x9398,0xA3FB,0xB3DA,0xC33D,0xD31C,0xE37F,0xF35E,
    0x02B1,0x1290,0x22F3,0x32D2,0x4235,0x5214,0x6277,0x7256,
    0xB5EA,0xA5CB,0x95A8,0x8589,0xF56E,0xE54F,0xD52C,0xC50D,
    0x34E2,0x24C3,0x14A0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xA7DB,0xB7FA,0x8799,0x97B8,0xE75F,0xF77E,0xC71D,0xD73C,
    0x26D3,0x36F2,0x0691,0x16B0,0x6657,0x7676,0x4615,0x5634,
    0xD94C,0xC96D,0xF90E,0xE92F,0x99C8,0x89E9,0xB98A,0xA9AB,
    0x5844,0x4865,0x7806,0x6827,0x18C0,0x08E1,0x3882,0x28A3,
    0xCB7D,0xDB5C,0xEB3F,0xFB1E,0x8BF9,0x9BD8,0xABBB,0xBB9A,
    0x4A75,0x5A54,0x6A37,0x7A16,0x0AF1,0x1AD0,0x2AB3,0x3A92,
    0xFD2E,0xED0F,0xDD6C,0xCD4D,0xBDAA,0xAD8B,0x9DE8,0x8DC9,
    0x7C26,0x6C07,0x5C64,0x4C45,0x3CA2,0x2C83,0x1CE0,0x0CC1,
    0xEF1F,0xFF3E,0xCF5D,0xDF7C,0xAF9B,0xBFBA,0x8FD9,0x9FF8,
    0x6E17,0x7E36,0x4E55,0x5E74,0x2E93,0x3EB2,0x0ED1,0x1EF0,
};
// 计算 16 位 CRC 值，CRC-16 或 CRC-CCITT
_XIF_ void CalculateCrc16(unsigned char *aData, unsigned char aSize)
{
    unsigned char i;
    unsigned short nAccum = 0;
    for (i = 0; i < aSize; i++)
        nAccum = (nAccum << 8) ^ (unsigned short)(CRC_table[(nAccum >> 8) ^ *aData++]);
    *aData++ = nAccum / 256;
    *aData++ = nAccum % 256;
}
_XIF_ unsigned short CRC16(unsigned char *aData, unsigned char aSize)
{
    unsigned char i;
    unsigned short nAccum = 0;

    for (i = 0; i < aSize; i++)
        nAccum = (nAccum << 8) ^ (unsigned short)CRC_table[(nAccum >> 8) ^ *aData++];
    return nAccum;
}

#if RTC_EQ_UART_PORT == HAL_UART0_BASE
void UART0_IRQHandler(void)
#elif RTC_EQ_UART_PORT == HAL_UART1_BASE
void UART1_IRQHandler(void)
#elif RTC_EQ_UART_PORT == HAL_UART2_BASE
void UART2_IRQHandler(void)
#else
#error "please choose UART\n"
#endif
{
    UART_TypeDef *uart = (UART_TypeDef *)RTC_EQ_UART_PORT;
    /*接受数据*/
    if ((uart->UARTMIS & (1UL << UART_RXInt)) || (uart->UARTMIS & (1UL << UART_RXTimeoutInt)))
    {
        while (!(((UART_TypeDef *)RTC_EQ_UART_PORT)->UARTFlag & (0x1 << 6))) // read fifo have data
        {
            eq_port = (int8_t)UartPollingReceiveData(uart);
            if (eq_msg_recv_queue != NULL)
                xQueueSendFromISR(eq_msg_recv_queue, &eq_port, pdFALSE);
        }
        UART_IntClear(uart, UART_RXInt);
    }
    (uart)->UARTICR = 0xFFF;
}

// eq参数配置任务初始化
_XIF_ int uart_eq_param_cfg_init(void)
{
    UARTInterruptConfig((UART_TypeDef *)RTC_EQ_UART_PORT, UART_BaudRate921600);
    eq_msg_recv_queue = xQueueCreate(EQ_DATA_MAX_LEN, sizeof(int8_t));
    if (eq_msg_recv_queue == NULL)
    {
        mprintf("eq_msg_recv_queue create fail\r\n");
    }
    mprintf("eq_msg_recv_queue create success\r\n");

    xTaskCreate(uart_eq_param_cfg_task, "uart_eq_param_cfg_task", EQ_TASK_MAX_LEN, NULL, 4, NULL);
    return 0;
}

_XIF_ int init_eq_drc_param_from_nv(void)
{
    eq_config_t eq_gain_str;
    drc_config_t drc_config_str;
    mprintf("init_eq_drc_val.param_enable_flag = %d\r\n", init_eq_drc_val.param_enable_flag);
    if (init_eq_drc_val.param_enable_flag == EQ_DRC_NV_ENABEL_FLAG)
    {
        mprintf("g_eq_param init from nv...\r\n");
        mprintf("g_drc_param init from nv...\r\n");
#if USE_EQ_MODULE
        for (int i = 0; i < EQ_CONFIG_MAX_CH; i++)
        {
            eq_gain_str.gain[i] = ((float)init_eq_drc_val.uart_eq_config.gain[i]) / 1000.0f;
            eq_gain_str.bw[i] = ((float)init_eq_drc_val.uart_eq_config.bw[i]);
            eq_gain_str.fc[i] = ((float)init_eq_drc_val.uart_eq_config.fc[i]);
            eq_gain_str.type[i] = 0;
            eq_gain_str.eq_gain_from_uart = true;
        }
        //memcpy(&g_eq_param->eq_param, &eq_gain_str, sizeof(eq_gain_str));
        set_eq_gain_str(&eq_gain_str);
#endif
#if USE_DRC_MODULE
        drc_config_str.attack_time = (float)init_eq_drc_val.uart_drc_config.attack_time / 1000.0f;
        drc_config_str.release_time = (float)init_eq_drc_val.uart_drc_config.release_time / 1000.0f;
        drc_config_str.ratio = (float)init_eq_drc_val.uart_drc_config.ratio / 1000.0f;
        drc_config_str.knee = (float)init_eq_drc_val.uart_drc_config.knee / 1000.0f;
        drc_config_str.thr_comp = (float)init_eq_drc_val.uart_drc_config.thr_comp / 1000.0f;
        drc_config_str.thr_exp = (float)init_eq_drc_val.uart_drc_config.thr_exp / 1000.0f;
        drc_config_str.thr_gate = (float)init_eq_drc_val.uart_drc_config.thr_gate / 1000.0f;
        drc_config_str.drc_para_from_uart = true;
        //memcpy(&g_drc_param->drc_param, &drc_config_str, sizeof(drc_config_str)); 
        set_drc_config_str(&drc_config_str);
#endif
    }

}

_XIF_ void uart_eq_param_cfg_task(void *ptr)
{
    uint16_t data_len = 0;
    uint16_t package_length;
    int16_t real_len;
    int8_t package_data[EQ_DATA_MAX_LEN] = {0};
    eq_config_t eq_gain_str;
    drc_config_t drc_config_str;
    eq_gain_str.eq_gain_from_uart = false;
    drc_config_str.drc_para_from_uart = false;
    memset(&init_eq_drc_val, 0, sizeof(eq_param_t));
    // 数据nv
    if (CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_EQ_PARAM, sizeof(init_eq_drc_val), &init_eq_drc_val, &real_len))
    {
        cinv_item_init(NVDATA_ID_EQ_PARAM, sizeof(init_eq_drc_val), &init_eq_drc_val);
        mprintf("cinv_item_read failed %d,%d\r\n", sizeof(init_eq_drc_val), real_len);
    }
    else
    {
        mprintf("cinv_item_read success %d,%d\r\n", sizeof(init_eq_drc_val), real_len);
        mprintf("init_eq_drc_val.param_enable_flag = %x\r\n", init_eq_drc_val.param_enable_flag);
        mprintf("gain0 \t\t(%d.%d) \r\n", init_eq_drc_val.uart_eq_config.gain[0] / 1000, abs(init_eq_drc_val.uart_eq_config.gain[0] % 1000));
        mprintf("gain1 \t\t(%d.%d) \r\n", init_eq_drc_val.uart_eq_config.gain[1] / 1000, abs(init_eq_drc_val.uart_eq_config.gain[1] % 1000));
        mprintf("gain2 \t\t(%d.%d) \r\n", init_eq_drc_val.uart_eq_config.gain[2] / 1000, abs(init_eq_drc_val.uart_eq_config.gain[2] % 1000));
        mprintf("gain3 \t\t(%d.%d) \r\n", init_eq_drc_val.uart_eq_config.gain[3] / 1000, abs(init_eq_drc_val.uart_eq_config.gain[3] % 1000));
        mprintf("gain4 \t\t(%d.%d) \r\n", init_eq_drc_val.uart_eq_config.gain[4] / 1000, abs(init_eq_drc_val.uart_eq_config.gain[4] % 1000));
        mprintf("gain5 \t\t(%d.%d) \r\n", init_eq_drc_val.uart_eq_config.gain[5] / 1000, abs(init_eq_drc_val.uart_eq_config.gain[5] % 1000));
        mprintf("gain6 \t\t(%d.%d) \r\n", init_eq_drc_val.uart_eq_config.gain[6] / 1000, abs(init_eq_drc_val.uart_eq_config.gain[6] % 1000));
        mprintf("gain7 \t\t(%d.%d) \r\n", init_eq_drc_val.uart_eq_config.gain[7] / 1000, abs(init_eq_drc_val.uart_eq_config.gain[7] % 1000));
        mprintf("gain8 \t\t(%d.%d) \r\n", init_eq_drc_val.uart_eq_config.gain[8] / 1000, abs(init_eq_drc_val.uart_eq_config.gain[8] % 1000));
        mprintf("bw0 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[0]);
        mprintf("bw1 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[1]);
        mprintf("bw2 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[2]);
        mprintf("bw3 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[3]);
        mprintf("bw4 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[4]);
        mprintf("bw5 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[5]);
        mprintf("bw6 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[6]);
        mprintf("bw7 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[7]);
        mprintf("bw8 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[8]);
        mprintf("fc0 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[0]);
        mprintf("fc1 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[1]);
        mprintf("fc2 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[2]);
        mprintf("fc3 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[3]);
        mprintf("fc4 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[4]);
        mprintf("fc5 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[5]);
        mprintf("fc6 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[6]);
        mprintf("fc7 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[7]);
        mprintf("fc8 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[8]);
        mprintf("attack_time \t(%d) \r\n", init_eq_drc_val.uart_drc_config.attack_time);
        mprintf("release_time \t(%d) \r\n", init_eq_drc_val.uart_drc_config.release_time);
        mprintf("ratio \t\t(%d.%d) \r\n", init_eq_drc_val.uart_drc_config.ratio / 1000, abs(init_eq_drc_val.uart_drc_config.ratio % 1000));
        mprintf("knee \t\t(%d.%d) \r\n", init_eq_drc_val.uart_drc_config.knee / 1000, abs(init_eq_drc_val.uart_drc_config.knee % 1000));
        mprintf("thr_comp \t(%d.%d) \r\n", init_eq_drc_val.uart_drc_config.thr_comp / 1000, abs(init_eq_drc_val.uart_drc_config.thr_comp % 1000));
        mprintf("thr_exp \t(%d.%d) \r\n", init_eq_drc_val.uart_drc_config.thr_exp / 1000, abs(init_eq_drc_val.uart_drc_config.thr_exp % 1000));
        mprintf("thr_gate \t(%d.%d) \r\n", init_eq_drc_val.uart_drc_config.thr_gate / 1000, abs(init_eq_drc_val.uart_drc_config.thr_gate % 1000));
#if SYS_BOOT_READ_EQ_DRC_FROM_NV
        init_eq_drc_param_from_nv();
#endif
    }
    mprintf("start recv uart data ....\r\n");
    while (1)
    {
        while (pdPASS == xQueueReceive(eq_msg_recv_queue, &package_data[package_length++], portMAX_DELAY))
        {
            if (package_length < EQ_DATA_MAX_LEN)
            {
                if (package_length > 3)
                {
                    data_len = (uint8_t)package_data[2];     //数据长度解析完成
                    mprintf("data_len = %d\n\r",data_len);
                    if (data_len <= 0)
                        goto error_data;
                }
                if (package_length > data_len + 4)
                {
                    mprintf("recv success !!! \r\n");
                    break;                          //数据接收完成
                }
            }
            else
            {
                package_length = 0;
                mprintf("error:eq data too long !!! \r\n");
            }
        }
        // 进行数据校验
        uint16_t crc_val = CRC16(package_data, package_length - 2);
        uint8_t data_head1 = package_data[0];
        uint8_t data_head2 = package_data[1];
        if (data_head1 == 0x5a && data_head2 == 0xa5)                   //数据校验通过 存入nv
        {
            mprintf("\r\nrecv uart len:%d;uart crc:%x;eq data len:%d;recv data :\r\n", package_length, crc_val, data_len);
            for (int16_t i = 0; i < package_length; i++)
            {
                mprintf("%02X ", (package_data[i] & 0x00FF));
            }
            mprintf("\r\n");

            memset(&init_eq_drc_val, 0, sizeof(eq_param_t));
            memcpy(&init_eq_drc_val, package_data + 3, data_len);
            init_eq_drc_val.param_enable_flag = EQ_DRC_NV_ENABEL_FLAG;   //使能NV存储
            cinv_item_ret_t ret = cinv_item_write(NVDATA_ID_EQ_PARAM, sizeof(init_eq_drc_val), &init_eq_drc_val);
            if (CINV_OPER_SUCCESS != ret)
            {
                mprintf("cinv_item_write %d\r\n", ret);
                mprintf("write error \n");
                while (1);
            }
            mprintf("cinv_item_write success \r\n");
            for (int i = 0; i < EQ_CONFIG_MAX_CH; i++)
            {
                eq_gain_str.gain[i] = ((float)init_eq_drc_val.uart_eq_config.gain[i]) / 1000.0f;
                eq_gain_str.bw[i] = ((float)init_eq_drc_val.uart_eq_config.bw[i]);
                eq_gain_str.fc[i] = ((float)init_eq_drc_val.uart_eq_config.fc[i]);
                eq_gain_str.type[i] = 0;
            }
            eq_gain_str.eq_gain_from_uart = true;
            set_eq_gain_str(&eq_gain_str);
            drc_config_str.attack_time = (float)init_eq_drc_val.uart_drc_config.attack_time;
            drc_config_str.release_time = (float)init_eq_drc_val.uart_drc_config.release_time;
            drc_config_str.ratio = (float)init_eq_drc_val.uart_drc_config.ratio / 1000.0f;
            drc_config_str.knee = (float)init_eq_drc_val.uart_drc_config.knee / 1000.0f;
            drc_config_str.thr_comp = (float)init_eq_drc_val.uart_drc_config.thr_comp / 1000.0f;
            drc_config_str.thr_exp = (float)init_eq_drc_val.uart_drc_config.thr_exp / 1000.0f;
            drc_config_str.thr_gate = (float)init_eq_drc_val.uart_drc_config.thr_gate / 1000.0f;
            drc_config_str.drc_para_from_uart = true;
            set_drc_config_str(&drc_config_str);
#if 1
            mprintf("gain0 \t\t(%d.%d) \r\n",init_eq_drc_val.uart_eq_config.gain[0]/1000,abs(init_eq_drc_val.uart_eq_config.gain[0]%1000));
            mprintf("gain1 \t\t(%d.%d) \r\n",init_eq_drc_val.uart_eq_config.gain[1]/1000,abs(init_eq_drc_val.uart_eq_config.gain[1]%1000));
            mprintf("gain2 \t\t(%d.%d) \r\n",init_eq_drc_val.uart_eq_config.gain[2]/1000,abs(init_eq_drc_val.uart_eq_config.gain[2]%1000));
            mprintf("gain3 \t\t(%d.%d) \r\n",init_eq_drc_val.uart_eq_config.gain[3]/1000,abs(init_eq_drc_val.uart_eq_config.gain[3]%1000));
            mprintf("gain4 \t\t(%d.%d) \r\n",init_eq_drc_val.uart_eq_config.gain[4]/1000,abs(init_eq_drc_val.uart_eq_config.gain[4]%1000));
            mprintf("gain5 \t\t(%d.%d) \r\n",init_eq_drc_val.uart_eq_config.gain[5]/1000,abs(init_eq_drc_val.uart_eq_config.gain[5]%1000));
            mprintf("gain6 \t\t(%d.%d) \r\n",init_eq_drc_val.uart_eq_config.gain[6]/1000,abs(init_eq_drc_val.uart_eq_config.gain[6]%1000));
            mprintf("gain7 \t\t(%d.%d) \r\n",init_eq_drc_val.uart_eq_config.gain[7]/1000,abs(init_eq_drc_val.uart_eq_config.gain[7]%1000));
            mprintf("gain8 \t\t(%d.%d) \r\n",init_eq_drc_val.uart_eq_config.gain[8]/1000,abs(init_eq_drc_val.uart_eq_config.gain[8]%1000));
            mprintf("bw0 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[0]);
            mprintf("bw1 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[1]);
            mprintf("bw2 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[2]);
            mprintf("bw3 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[3]);
            mprintf("bw4 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[4]);
            mprintf("bw5 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[5]);
            mprintf("bw6 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[6]);
            mprintf("bw7 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[7]);
            mprintf("bw8 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.bw[8]);
            mprintf("fc0 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[0]);
            mprintf("fc1 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[1]);
            mprintf("fc2 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[2]);
            mprintf("fc3 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[3]);
            mprintf("fc4 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[4]);
            mprintf("fc5 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[5]);
            mprintf("fc6 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[6]);
            mprintf("fc7 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[7]);
            mprintf("fc8 \t\t(%d) \r\n", init_eq_drc_val.uart_eq_config.fc[8]);
            mprintf("attack_time \t(%d) \r\n",init_eq_drc_val.uart_drc_config.attack_time);
            mprintf("release_time \t(%d) \r\n",init_eq_drc_val.uart_drc_config.release_time);
            mprintf("ratio \t\t(%d.%d) \r\n",init_eq_drc_val.uart_drc_config.ratio/1000,abs(init_eq_drc_val.uart_drc_config.ratio%1000));
            mprintf("knee \t\t(%d.%d) \r\n",init_eq_drc_val.uart_drc_config.knee/1000,abs(init_eq_drc_val.uart_drc_config.knee%1000));
            mprintf("thr_comp \t(%d.%d) \r\n",init_eq_drc_val.uart_drc_config.thr_comp/1000,abs(init_eq_drc_val.uart_drc_config.thr_comp%1000));
            mprintf("thr_exp\t(%d.%d) \r\n",init_eq_drc_val.uart_drc_config.thr_exp/1000,abs(init_eq_drc_val.uart_drc_config.thr_exp%1000));
            mprintf("thr_gate \t(%d.%d) \r\n",init_eq_drc_val.uart_drc_config.thr_gate/1000,abs(init_eq_drc_val.uart_drc_config.thr_gate%1000));

#endif
        }
error_data:
        package_length = 0;
        memset(package_data, 0, EQ_DATA_MAX_LEN);
        memset(&init_eq_drc_val, 0, sizeof(init_eq_drc_val));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

#endif // EQ_TEST_ENABLE
/***************************************************************************************************************/
