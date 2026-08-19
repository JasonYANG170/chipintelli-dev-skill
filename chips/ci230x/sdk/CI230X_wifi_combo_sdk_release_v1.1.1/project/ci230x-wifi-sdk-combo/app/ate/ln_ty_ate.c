/**
 * @file   ln_ty_ate.c
 * @author LightningSemi WLAN Team
 * Copyright (C) 2021-2021 LightningSemi Technology Co., Ltd. All rights reserved.
 * 
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-16     MurphyZhao   the first version
 */

#include <ctype.h>                  // isprint
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "ln_ty_ate.h"
#include "utils/debug/ln_assert.h"
#include "utils/system_parameter.h"
#include "ln_utils.h"
#include "osal/osal.h"
#include "utils/debug/log.h"
#include "serial/serial.h"
#include "wifi.h"

#include "hal/hal_gpio.h"
#include "ln_nvds.h"

#include "proj_config.h"

#ifdef CFG_UART_BAUDRATE_CONSOLE
  #define ATE_CLI_BAUDRATE       CFG_UART_BAUDRATE_CONSOLE
#else
  #define ATE_CLI_BAUDRATE       (115200)
#endif

#define LN_TY_ATE_CLI_THR_PRI        OS_PRIORITY_NORMAL
#define LN_TY_ATE_CLI_THR_STACK_SIZE (1024)

#define LN_TY_ATE_CLI_CMD_BUFF_SIZE  (512)
#define LN_TY_ATE_CLI_LOG_BUFF_SIZE  (128)
#define LN_TY_ATE_CLI_CMD_END_MARK   ('\n')
#define ATE_OK_AND_SAVED_OK_STR      ("SAVE_FLASH_OK\r\n")
#define ATE_OK_AND_SAVED_FAIL_STR    ("SAVE_FLASH_FAIL\r\n")
#define ATE_CMD_FOUND                (0)
#define ATE_CMD_NOT_FOUND            (-1)
#define ATE_CMD_PRINT(fmt, ...)      ln_ty_ate_printf(fmt, ##__VA_ARGS__)

typedef struct cmd_tbl_s
{
    char *name;        /* Command Name */
    int maxargs;       /* maximum number of arguments */
    int (*cmd)(struct cmd_tbl_s *, int, const char *);
    char *usage;       /* Usage message	(short) */
} cmd_tbl_t;

static Serial_t sg_ate_cli_serial = { 0 };
static OS_Semaphore_t sg_ate_cli_rx_sem = { 0 };
static uint8_t *sg_ate_cli_cmd_buff_p = NULL;
static uint8_t *sg_ate_cli_log_buff_p = NULL;

static bool is_ate_complete_cmd(char * cmd_str);

static int do_get_sta_mac(cmd_tbl_t *cmd, int argc, const char *line)
{
    char *str = NULL;
    uint8_t mac[6];
    // LOG(LOG_LVL_INFO, "raw data:\r\n%s\r\n", line);
    // LOG(LOG_LVL_INFO, "[%s:%d] %s \thelp:\r\n%s\r\n",
    //         __func__, __LINE__, cmd->name, cmd->usage);

    if (SYSPARAM_ERR_NONE != sysparam_sta_mac_get(mac)) {
        ATE_CMD_PRINT("+mac FAIL\r\n");
    } else {
        ATE_CMD_PRINT("+mac OK mac:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    return 0;
}

/**
 * SET_STA_MAC_ADDR=<addr>
*/
static int do_set_sta_mac(cmd_tbl_t *cmd, int argc, const char *line)
{
    char *str = NULL;
    uint8_t mac[6];

    // LOG(LOG_LVL_INFO, "raw data:\r\n%s\r\n", line);
    // LOG(LOG_LVL_INFO, "[%s:%d] %s \thelp:\r\n%s\r\n",
    //         __func__, __LINE__, cmd->name, cmd->usage);

    str = (char *)(line + strlen(cmd->name) + 1);
    if (strlen(str) <= 17) {
        ATE_CMD_PRINT("+mac FAIL\r\n");
        return -1;
    }

    str[17] = '\0';
    ln_mac_str2hex((const uint8_t *)str, mac);

    if (SYSPARAM_ERR_NONE != sysparam_sta_mac_update((const uint8_t *)mac)) {
        ATE_CMD_PRINT("+mac FAIL\r\n");
    } else {
        ATE_CMD_PRINT("+mac OK\r\n");
    }
    return 0;
}

static int do_get_ap_mac(cmd_tbl_t *cmd, int argc, const char *line)
{
    char *str = NULL;
    uint8_t mac[6];

    // LOG(LOG_LVL_INFO, "raw data:\r\n%s\r\n", line);
    // LOG(LOG_LVL_INFO, "[%s:%d] %s \thelp:\r\n%s\r\n",
    //         __func__, __LINE__, cmd->name, cmd->usage);

    if (SYSPARAM_ERR_NONE != sysparam_softap_mac_get(mac)) {
        ATE_CMD_PRINT("+mac FAIL\r\n");
    } else {
        ATE_CMD_PRINT("+mac OK mac:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    return 0;
}

static int do_set_ap_mac(cmd_tbl_t *cmd, int argc, const char *line)
{
    char *str = NULL;
    uint8_t mac[6];

    // LOG(LOG_LVL_INFO, "raw data:\r\n%s\r\n", line);
    // LOG(LOG_LVL_INFO, "[%s:%d] %s \thelp:\r\n%s\r\n",
    //         __func__, __LINE__, cmd->name, cmd->usage);

    str = (char *)(line + strlen(cmd->name) + 1);
    if (strlen(str) <= 17) {
        ATE_CMD_PRINT("+mac FAIL\r\n");
        return -1;
    }

    str[17] = '\0';
    ln_mac_str2hex((const uint8_t *)str, mac);

    if (SYSPARAM_ERR_NONE != sysparam_softap_mac_update((const uint8_t *)mac)) {
        ATE_CMD_PRINT("+mac FAIL\r\n");
    } else {
        ATE_CMD_PRINT("+mac OK\r\n");
    }
    return 0;
}

static int do_ate_ok(cmd_tbl_t *cmd, int argc, const char *line)
{
    LOG(LOG_LVL_INFO, "raw data:\r\n%s\r\n", line);
    LOG(LOG_LVL_INFO, "[%s:%d] %s \thelp:\r\n%s\r\n",
            __func__, __LINE__, cmd->name, cmd->usage);

    is_ate_complete_cmd(line);
    return 0;
}

static const cmd_tbl_t sg_cmd_list[] = {
    {"GET_STA_MAC_ADDR", 1, do_get_sta_mac, "eg: GET_STA_MAC_ADDR\r\n"},
    {"SET_STA_MAC_ADDR", 2, do_set_sta_mac, "eg: SET_STA_MAC_ADDR=<addr>\r\n"},
    {"GET_AP_MAC_ADDR",  1, do_get_ap_mac,  "eg: GET_AP_MAC_ADDR\r\n"},
    {"SET_AP_MAC_ADDR",  2, do_set_ap_mac,  "eg: SET_AP_MAC_ADDR=<addr>\r\n"},
    {"ATE_OK",           2, do_ate_ok,      "eg: ATE_OK=<status>\r\n0:fail;1:succ\r\n"},
};

size_t ln_ty_ate_vprintf(const char *format, va_list args);

static const cmd_tbl_t *find_cmd (const char *cmd)
{
    const cmd_tbl_t *cmdtp = NULL;
    int i;

    for (i = 0; i < (sizeof(sg_cmd_list) / sizeof(sg_cmd_list[0])); i++) {
        cmdtp = &sg_cmd_list[i];
        if (strncasecmp(cmd, cmdtp->name, strlen(cmdtp->name)) == 0) {
            return cmdtp;   /* full match */
        }
    }

    return NULL;
}

#include "hal/hal_adc.h"
#include "drv_adc_measure.h"

static void wifi_temp_calibrate_timer_cb(void *arg)
{
    uint16_t adc_val = 0;
    int16_t curr_adc = 0;
    LN_UNUSED(arg);

    adc_val = drv_adc_read(ADC_CH0);
    wifi_do_temp_cal_period(adc_val);

    curr_adc = (adc_val & 0xFFF);
    static volatile uint8_t cnt = 0;
    cnt++;
    if ((cnt % 60) == 0) {
        LOG(LOG_LVL_INFO, "adc raw: %4d, temp_IC: %4d\r\n",
                curr_adc, (int16_t)(25 + (curr_adc - 750) / 2.54f));
    }
}
static OS_Timer_t s_wifi_temp_cal_timer = {0};

static int ln_ty_ate_wifi_init(void)
{
#define NOT_CONNECTABLE_SSID ("NOT_CONNECTABLE_SSID_0XDEADBEEF")
    wifi_sta_connect_t conn           = {0};
    wifi_scan_cfg_t scan_cfg          = {0};
    uint8_t mac_addr[MAC_ADDRESS_LEN] = {0};

    // rf preprocess,img cal
    wifi_rf_preprocess();    // time consuming 80ms
    wifi_rf_image_cal();     // time consuming 156ms

    // init wifi
    wifi_init();
    wifi_manager_init();     // Creat wifi manager task

    // Start wifi temperature calibrate
    {
        int8_t cap_comp = 0;

        if (NVDS_ERR_OK == ln_nvds_get_xtal_comp_val((uint8_t *)&cap_comp)) {
            if ((uint8_t)cap_comp == 0xFF) {
                cap_comp = 0;
            }
        }

        drv_adc_init();
        wifi_temp_cal_init(drv_adc_read(ADC_CH0), cap_comp);

        if (OS_OK != OS_TimerCreate(&s_wifi_temp_cal_timer, OS_TIMER_PERIODIC,
                wifi_temp_calibrate_timer_cb, NULL, 1000)) {
            LOG(LOG_LVL_ERROR, "create wifi temp calibrate timer failed.\r\n");
        } else {
            OS_TimerStart(&s_wifi_temp_cal_timer);
        }
    }

    sysparam_sta_mac_get(mac_addr);
    // netdev_set_mac_addr(NETIF_IDX_STA, mac_addr);
    // netdev_set_active(NETIF_IDX_STA);
    wifi_sta_start(mac_addr, WIFI_NO_POWERSAVE);

    conn.ssid  = (char *)NOT_CONNECTABLE_SSID;
    conn.pwd   = "";
    conn.bssid = NULL;
    conn.psk_value = NULL;

    if (SYSPARAM_ERR_NONE != sysparam_sta_scan_cfg_get(&scan_cfg))
    {
        LOG(LOG_LVL_ERROR, "[%s:%d]\r\n", __func__, __LINE__);
        return -1;
    }

    if (wifi_sta_connect(&conn, &scan_cfg) != 0)
    {
        LOG(LOG_LVL_ERROR, "[%s:%d]\r\n", __func__, __LINE__);
        return -1;
    }

    return 0;
}

static void ln_ty_ate_cli_rx_cb(void)
{
    OS_SemaphoreRelease(&sg_ate_cli_rx_sem);
}

size_t ln_ty_ate_vprintf(const char *format, va_list args)
{
    uint32_t len = 0;
    int      ret = 0;

    if (sg_ate_cli_log_buff_p != NULL) {
        len = vsnprintf(sg_ate_cli_log_buff_p, LN_TY_ATE_CLI_LOG_BUFF_SIZE, format, args);
        ret = serial_write(&sg_ate_cli_serial, (const void *)sg_ate_cli_log_buff_p, len);
    }

    return ret;
}

void ln_ty_ate_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ln_ty_ate_vprintf(format, args);
    va_end(args);
}

/**
 * SER_PORT_UART1 -> A2:TX; A3:RX
*/
static int ln_ty_ate_uart_init(void)
{
    Serial_t *fd = &sg_ate_cli_serial;

    if (OS_OK != OS_SemaphoreCreate(&sg_ate_cli_rx_sem, 0, 512)) {
        return -1;
    }

    if (NULL == (sg_ate_cli_cmd_buff_p = OS_Malloc(LN_TY_ATE_CLI_CMD_BUFF_SIZE))) {
        goto __exit;
    }

    if (NULL == (sg_ate_cli_log_buff_p = OS_Malloc(LN_TY_ATE_CLI_LOG_BUFF_SIZE))) {
        goto __exit;
    }

    serial_init(fd, SER_PORT_UART1, ATE_CLI_BAUDRATE, ln_ty_ate_cli_rx_cb);
    return 0;

__exit:
    if (sg_ate_cli_cmd_buff_p != NULL) {
        OS_Free(sg_ate_cli_cmd_buff_p);
    }

    if (sg_ate_cli_log_buff_p != NULL) {
        OS_Free(sg_ate_cli_log_buff_p);
    }
    return -1;
}

static int ate_cmd_parser(const char *line)
{
    int ret = ATE_CMD_FOUND;
    cmd_tbl_t *cmdtp;

    /* Look up command in command table */
    if ((cmdtp = (cmd_tbl_t *)find_cmd(line)) == 0) {
        // LOG(LOG_LVL_ERROR, "Unknown command: %s\r\n", line);
        ret = ATE_CMD_NOT_FOUND;
    } else if (cmdtp->cmd) {
        (cmdtp->cmd)(cmdtp, 0, line);
    }
    return ret;
}

static bool is_ate_complete_cmd(char * cmd_str)
{
    extern int ate_get_xtalcap_txpower_offset (int8_t *xtal_cap, int8_t *tx_power);
    int8_t xtal_cap = 0, tx_power = 0;

    char *str = NULL;
    if ((str = strstr(cmd_str, "ATE_OK")) > 0) {
        ate_get_xtalcap_txpower_offset(&xtal_cap, &tx_power);

        /* 1. save to nvds(flash) */
        //1.1 save XTAL_CAP
        ln_nvds_set_xtal_comp_val(xtal_cap);

        //1.2 save TX_POWER
        ln_nvds_set_tx_power_comp(tx_power);

        //1.3 save ATE result
        //('S'=ate_successful, 'F'=ate_failed)
        if (strstr(str, "=1") > 0) {
            ln_nvds_set_ate_result('S');
        } else {
            ln_nvds_set_ate_result('F');
        }

        /* 2.Response to instrument: SAVE_FLASH_OK\r\n */
        ln_ty_ate_printf("SAVE_FLASH_OK\r\n");

        return true;
    }

    return false;
}

static void ate_cli_thr_entry(void *arg)
{
    int ret = 0;
    Serial_t *port = &sg_ate_cli_serial;
    int len = 0;
    uint8_t ch;
    uint16_t counter = 0;

    LN_UNUSED(arg);

    while(1)
    {
        if (0 != OS_SemaphoreWait(&sg_ate_cli_rx_sem, OS_WAIT_FOREVER)) continue;

        while (!fifo_isempty(&port->rxfifo))
        {
            len = serial_read(port, &ch, 1);
            if((len > 0) && 
                (isprint(ch) || (ch == LN_TY_ATE_CLI_CMD_END_MARK) ||
                (ch == '\r')))
            {
                // LOG(LOG_LVL_INFO, "[%c]\r\n", ch);
                sg_ate_cli_cmd_buff_p[counter++] = ch;
                if (counter >= LN_TY_ATE_CLI_CMD_BUFF_SIZE) {
                    counter = 0;
                    continue;
                }

                if (ch == LN_TY_ATE_CLI_CMD_END_MARK) {
                    sg_ate_cli_cmd_buff_p[counter] = '\0';

                    if (ATE_CMD_NOT_FOUND == ate_cmd_parser(sg_ate_cli_cmd_buff_p))
                    {
                        // if (!is_ate_complete_cmd(sg_ate_cli_cmd_buff_p)) 
                        {
                            // rm CRLF
                            if (sg_ate_cli_cmd_buff_p[counter-1] == '\n') {
                                sg_ate_cli_cmd_buff_p[counter-1] = '\0';
                                counter--;
                            }

                            if (sg_ate_cli_cmd_buff_p[counter-1] == '\r') {
                                sg_ate_cli_cmd_buff_p[counter-1] = '\0';
                                counter--;
                            }

                            if(0 != wifi_private_command(sg_ate_cli_cmd_buff_p)) {
                                LOG(LOG_LVL_ERROR, "send pvt cmd failed!\r\n");
                            }
                        }
                    }
                    counter = 0;
                    memset(sg_ate_cli_cmd_buff_p, 0x0, LN_TY_ATE_CLI_CMD_BUFF_SIZE);
                }
            }
        }
    }

__exit:
    OS_ThreadDelete(NULL);
}

bool ln_ty_ate_mode_check(void)
{
    if (HAL_RESET == hal_gpio_pin_input_read(GPIOA_BASE, GPIO_PIN_9)) {
        LOG(LOG_LVL_INFO, "GPIOA_9 low level, will enter ATE mode!\r\n");
        return true;
    }

    LOG(LOG_LVL_INFO, "GPIOA_9 high level, will enter Normal mode!\r\n");
    return false;
}

/**
 * @brief ln_ty_ate_main
 * @return -1: Failed to enter ate mode;
 *          0: Successfully entered ate mode.
*/
#include "utils/power_mgmt/ln_pm.h"
int ln_ty_ate_main(ate_mode_check_pfn cb)
{
    int ret = 0;
    OS_Thread_t ate_cli_thr = { 0 };
    LN_ASSERT(cb);

    if (false == cb()) {
        return -1;
    }

    ln_pm_always_clk_disable_select(CLK_G_I2S | CLK_G_WS2811 |
                                    CLK_G_SDIO | CLK_G_AES);
    ln_pm_always_clk_disable_select(CLK_G_GPIOA | CLK_G_GPIOB |
                                        CLK_G_SPI0 | CLK_G_SPI1 | CLK_G_I2C0 |
                                        CLK_G_WDT |
                                        CLK_G_TIM_REG | CLK_G_TIM1 | CLK_G_TIM2 | 
                                        CLK_G_TIM4 | CLK_G_DMA |
                                        CLK_G_TRNG);

    if (0 != ln_ty_ate_wifi_init()) {
        return -1;
    }

    if (0 != (ret = ln_ty_ate_uart_init())) {
        return -1;
    }

    if (OS_OK != OS_ThreadCreate(&ate_cli_thr, "cli",
            ate_cli_thr_entry, NULL,
            LN_TY_ATE_CLI_THR_PRI, LN_TY_ATE_CLI_THR_STACK_SIZE)) {
        return -1;
    }

    wlib_pvtcmd_output_cb_set(ln_ty_ate_vprintf);
    LOG(LOG_LVL_INFO, "Enter ATE mode!\r\n");

    return 0;
}

int ln_ty_ate_uart_test_use_start(void)
{
    int ret = 0;
    OS_Thread_t ate_cli_thr = { 0 };

    if (0 != (ret = ln_ty_ate_uart_init())) {
        return -1;
    }

    if (OS_OK != OS_ThreadCreate(&ate_cli_thr, "cli",
            ate_cli_thr_entry, NULL,
            LN_TY_ATE_CLI_THR_PRI, LN_TY_ATE_CLI_THR_STACK_SIZE)) {
        return -1;
    }

    wlib_pvtcmd_output_cb_set(ln_ty_ate_vprintf);

    return 0;
}
