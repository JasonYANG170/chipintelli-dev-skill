
/******************红外遥控器协议处理文件****************/

#include "cias_slave_message_handle.h"
#include "dp_process.h"
#include "cias_ir_common.h"
#include "cias_ir_flash.h"
#include "svc_irlib_download_ext.h"
#include "dp_process.h"
#include "cias_log.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "cias_freertos_common.h"
air_conditioner_ir_status_t g_air_conditioner_ir_status;
int g_air_conditioner_timer_count = -1;
int g_air_conditioner_timer_status = pdFALSE;
static TimerHandle_t g_air_conditioner_time_handle = {0};
extern TY_IRLIB_DL_KT_STAT_S g_ty_irlib_dl_kt_info; //记录空调当前设备状态
extern struct remote_info g_remote_info;

static void tuya_air_condition_timer_callback(void)
{
    CIAS_LOG_HL("tuya_air_condition_timer_callback, g_air_conditioner_ir_status.power_status = %d, g_air_conditioner_timer_count = %d", g_air_conditioner_ir_status.power_status, g_air_conditioner_timer_count);
    if (g_air_conditioner_timer_count-- == 0)
    {
        CIAS_LOG_HL("tuya_air_condition timimg over, close ....");
        if (g_air_conditioner_ir_status.power_status == 1) //空调打开状态
        {
            tuya_ir_ctrl_audio_cmd_handle(7); //关闭空调
        }
    }
    //tuya_ir_ctrl_audio_cmd_handle(7); //关闭空调
}
int tuya_air_conditioner_timer_init(int value)
{
    int g_air_conditioner_time_id = 0;
    //定时器60S超时一次
    g_air_conditioner_time_handle = xTimerCreate("air_conditioner_timer", pdMS_TO_TICKS(1000 * value), pdTRUE, &g_air_conditioner_time_id, tuya_air_condition_timer_callback);
    if (g_air_conditioner_time_handle == NULL)
    {
        CIAS_LOG_HL("g_air_conditioner_time_handle timer create error");
        return CIAS_FAIL;
    }
    return CIAS_OK;
}
//空调定时开启函数
static void tuya_air_conditioner_timer_start(int value)
{
    if (g_air_conditioner_timer_status == pdTRUE) //定时器已打开，直接复位定时器
    {
        xTimerReset(g_air_conditioner_time_handle, 0);
        return;
    }
    if (pdPASS != xTimerStart(g_air_conditioner_time_handle, 1000 * value))
    {
        g_air_conditioner_timer_status = pdFALSE;
        CIAS_LOG_HL("g_air_conditioner_time_handle start error");
        return;
    }
    g_air_conditioner_timer_status = pdTRUE;
}
//空调定时结束函数
static void tuya_air_conditioner_timer_stop()
{
    g_air_conditioner_timer_count = -1;
    if (g_air_conditioner_timer_status == pdTRUE)
    {
        if (pdPASS != xTimerStop(g_air_conditioner_time_handle, 0))
        {
            CIAS_LOG_HL("tuya_net_config_out_timer stop error");
            //error
        }
    }
}
void tuya_ir_protocol_init(void)
{

    g_air_conditioner_ir_status.power_status = 1; //默认关闭空调
    g_air_conditioner_ir_status.mode_val = 2;     //自动模式
    g_air_conditioner_ir_status.temp_val = 25;    //默认25度
    g_air_conditioner_ir_status.wind_val = 0;     //自动风速
    tuya_ir_code_head_t tuya_ir_code_head;
    if (tuya_ir_store_head_info_read(IR_AIR_CONDITIONER_ID, &tuya_ir_code_head) == 0)
    {
        g_air_conditioner_ir_status.ir_enable_flag = OPRT_OK;
        memcpy(g_air_conditioner_ir_status.conditioner_ir_devid, tuya_ir_code_head.ir_dev_id, tuya_ir_code_head.ir_dev_id_len);
        CIAS_LOG_HL("read conditioner ir dev id is ok , g_air_conditioner_ir_status.conditioner_ir_devid = %s\r\n", g_air_conditioner_ir_status.conditioner_ir_devid);
    }
    else
    {
        g_air_conditioner_ir_status.ir_enable_flag = OPRT_RECV_ERR;
        CIAS_LOG_HL("read conditioner ir dev id error\r\n");
    }
}
void tuya_ir_ctrl_audio_cmd_handle(int cmd)
{
#if 1
    uint8_t cmd_exec_status = -1; //指令执行状态
    int ir_dev_type = -1;
    int ir_key_value = -1;
    uint8_t m_ir_key[32] = {0};
    switch (cmd)
    {
    case 1:
        CIAS_LOG_HL("小智管家\r\n");
        break;
    case 2:
    case 3:
        CIAS_LOG_HL("开始配网\r\n");
        tuya_iot_wf_gw_unactive();
        return;
    case 4:
        CIAS_LOG_HL("关闭配网\r\n");
        break;
    case 5:
    case 6:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_ir_status.power_status = 1;
        tuya_air_conditioner_timer_stop();
        CIAS_LOG_HL("打开空调\r\n");
        break;
    case 7:
    case 8:
        g_air_conditioner_ir_status.power_status = 0;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        tuya_air_conditioner_timer_stop();
        CIAS_LOG_HL("关闭空调\r\n");
        break;
    case 9:
    case 10:
        g_air_conditioner_ir_status.wind_val = 3;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("空调高速风\r\n");
        break;
    case 11:
    case 12:
        g_air_conditioner_ir_status.wind_val = 2;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("空调中速风\r\n");
        break;
    case 13:
    case 14:
        g_air_conditioner_ir_status.wind_val = 1;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("空调低速风\r\n");
        break;
    case 15:
    case 16:
        g_air_conditioner_ir_status.wind_val = 0;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("自动风速\r\n");
        break;
    case 17:
        g_air_conditioner_ir_status.temp_val = 16;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("十六度\r\n");
        break;
    case 18:
        g_air_conditioner_ir_status.temp_val = 17;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("十七度\r\n");
        break;
    case 19:
        g_air_conditioner_ir_status.temp_val = 18;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("十八度\r\n");
        break;
    case 20:
        g_air_conditioner_ir_status.temp_val = 19;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("十九度\r\n");
        break;
    case 21:
        g_air_conditioner_ir_status.temp_val = 20;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十度\r\n");
        break;
    case 22:
        g_air_conditioner_ir_status.temp_val = 21;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十一度\r\n");
        break;
    case 23:
        g_air_conditioner_ir_status.temp_val = 22;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十二度\r\n");
        break;
    case 24:
        g_air_conditioner_ir_status.temp_val = 23;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十三度\r\n");
        break;
    case 25:
        g_air_conditioner_ir_status.temp_val = 24;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十四度\r\n");
        break;
    case 26:
        g_air_conditioner_ir_status.temp_val = 25;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十五度\r\n");
        break;
    case 27:
        g_air_conditioner_ir_status.temp_val = 26;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十六度\r\n");
        break;
    case 28:
        g_air_conditioner_ir_status.temp_val = 27;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十七度\r\n");
        break;
    case 29:
        g_air_conditioner_ir_status.temp_val = 28;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十八度\r\n");
        break;
    case 30:
        g_air_conditioner_ir_status.temp_val = 29;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("二十九度\r\n");
        break;
    case 31:
        g_air_conditioner_ir_status.temp_val = 30;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("三十度\r\n");
        break;
    case 32:
    case 33:
        g_air_conditioner_ir_status.mode_val = 0;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("制冷模式\r\n");
        break;
    case 34:
    case 35:
        g_air_conditioner_ir_status.mode_val = 1;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("制热模式\r\n");
        break;
    case 36:
    case 37:
        g_air_conditioner_ir_status.mode_val = 1;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("加热模式\r\n");
        break;
    case 38:
    case 39:
        g_air_conditioner_ir_status.mode_val = 3;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("送风模式\r\n");
        break;
    case 40:
    case 41:
        g_air_conditioner_ir_status.mode_val = 4;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("除湿模式\r\n");
        break;
    case 42:
    case 43:
        g_air_conditioner_ir_status.mode_val = 2;
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("自动模式\r\n");
        break;
    case 44:
        if (g_air_conditioner_ir_status.temp_val++ > 30)
        {
            g_air_conditioner_ir_status.temp_val = 30;
        }
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("升高温度\r\n");
        break;
    case 45:
        if (g_air_conditioner_ir_status.temp_val-- < 16)
        {
            g_air_conditioner_ir_status.temp_val = 16;
        }
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("降低温度\r\n");
        break;
    case 46:
    case 47:
        if (g_air_conditioner_ir_status.wind_val++ > 3)
        {
            g_air_conditioner_ir_status.wind_val = 3;
        }
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("增加风速\r\n");
        break;
    case 48:
    case 49:
        if (g_air_conditioner_ir_status.wind_val-- < 0)
        {
            g_air_conditioner_ir_status.wind_val = 1;
        }
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        CIAS_LOG_HL("减小风速\r\n");
        break;
    case 50:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 30;
        CIAS_LOG_HL("定时三十分钟\r\n");
        break;
    case 51:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60;
        CIAS_LOG_HL("定时一小时\r\n");
        break;
    case 52:
    case 53:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 2;
        CIAS_LOG_HL("定时两小时\r\n");
        break;
    case 54:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 3;
        CIAS_LOG_HL("定时三小时\r\n");
        break;
    case 55:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 4;
        CIAS_LOG_HL("定时四小时\r\n");
        break;
    case 56:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 5;
        CIAS_LOG_HL("定时五小时\r\n");
        break;
    case 57:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 6;
        CIAS_LOG_HL("定时六小时\r\n");
        break;
    case 58:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 7;
        CIAS_LOG_HL("定时七小时\r\n");
        break;
    case 59:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 8;
        CIAS_LOG_HL("定时八小时\r\n");
        break;
    case 60:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 9;
        CIAS_LOG_HL("定时九小时\r\n");
        break;
    case 61:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 10;
        CIAS_LOG_HL("定时十小时\r\n");
        break;
    case 62:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 11;
        CIAS_LOG_HL("定时十一小时\r\n");
        break;
    case 63:
        ir_dev_type = IR_AIR_CONDITIONER_ID;
        g_air_conditioner_timer_count = 60 * 12;
        CIAS_LOG_HL("定时十二时\r\n");
        break;
    case 64:
    case 65:
        ir_dev_type = IR_IV_DATA_ID;
        ir_key_value = 1;
        CIAS_LOG_HL("打开电视\r\n");
        break;
    case 66:
    case 67:
        ir_dev_type = IR_IV_DATA_ID;
        ir_key_value = 1;
        CIAS_LOG_HL("关闭电视\r\n");
        break;
    case 68:
        ir_dev_type = IR_SET_TOP_BOX_ID;
        ir_key_value = 1;
        CIAS_LOG_HL("打开机顶盒\r\n");
        break;
    case 69:
        ir_dev_type = IR_SET_TOP_BOX_ID;
        ir_key_value = 1;
        CIAS_LOG_HL("关闭机顶盒\r\n");
        break;
    case 70:
    case 71:
        ir_dev_type = IR_SET_TOP_BOX_ID;
        ir_key_value = 44;
        CIAS_LOG_HL("上一频道\r\n");
        break;
    case 72:
    case 73:
        ir_dev_type = IR_SET_TOP_BOX_ID;
        ir_key_value = 43;
        CIAS_LOG_HL("下一频道\r\n");
        break;
    case 74:
        ir_dev_type = IR_IV_DATA_ID;
        ir_key_value = 50;
        CIAS_LOG_HL("电视音量加\r\n");
        break;
    case 75:
        ir_dev_type = IR_IV_DATA_ID;
        ir_key_value = 51;
        CIAS_LOG_HL("电视音量减\r\n");
        break;
    case 76:
        ir_dev_type = IR_SET_TOP_BOX_ID;
        ir_key_value = 50;
        CIAS_LOG_HL("增加音量\r\n");
        break;
    case 77:
        ir_dev_type = IR_SET_TOP_BOX_ID;
        ir_key_value = 51;
        CIAS_LOG_HL("减小音量\r\n");
        break;
    case 78:
        ir_dev_type = IR_SET_TOP_BOX_ID;
        ir_key_value = 50;
        CIAS_LOG_HL("机顶盒音量加\r\n");
        break;
    case 79:
        ir_dev_type = IR_SET_TOP_BOX_ID;
        ir_key_value = 51;
        CIAS_LOG_HL("机顶盒音量减\r\n");
        break;
    case 80:
        ir_dev_type = IR_IV_DATA_ID;
        ir_key_value = 106;
        CIAS_LOG_HL("电视静音\r\n");
        break;
    case 81:
        ir_dev_type = IR_FAN_ID;
        ir_key_value = 1;
        CIAS_LOG_HL("打开风扇\r\n");
        break;
    case 82:
        ir_dev_type = IR_FAN_ID;
        ir_key_value = 1;
        CIAS_LOG_HL("关闭风扇\r\n");
        break;
    case 83:
        ir_dev_type = IR_FAN_ID;
        ir_key_value = 9362;
        CIAS_LOG_HL("打开摇头\r\n");
        break;
    case 84:
        ir_dev_type = IR_FAN_ID;
        ir_key_value = 9362;
        CIAS_LOG_HL("关闭摇头\r\n");
        break;
    case 85:
        ir_dev_type = IR_FAN_ID;
        ir_key_value = 9367;
        CIAS_LOG_HL("风扇调速\r\n");
        break;
    case 86:
        ir_dev_type = IR_FAN_ID;
        ir_key_value = 23;
        CIAS_LOG_HL("风扇定时\r\n");
        break;
    case 87:
        ir_dev_type = IR_FAN_ID;
        ir_key_value = 9362;
        CIAS_LOG_HL("一挡风\r\n");
        break;
    case 88:
        CIAS_LOG_HL("风扇低速风\r\n");
        break;
    case 89:
        ir_dev_type = IR_FAN_ID;
        ir_key_value = 9362;
        CIAS_LOG_HL("二挡风\r\n");
        break;
    case 90:
        ir_key_value = 9362;
        ir_dev_type = IR_FAN_ID;
        CIAS_LOG_HL("风扇中速风\r\n");
        break;
    case 91:
        ir_key_value = 9362;
        ir_dev_type = IR_FAN_ID;
        CIAS_LOG_HL("三挡风\r\n");
        break;
    case 92:
        ir_key_value = 9362;
        CIAS_LOG_HL("风扇高速风\r\n");
        break;
    case 93:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("冷色灯光\r\n");
        break;
    case 94:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("暖色灯光\r\n");
        break;
    case 95:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("打开夜灯\r\n");
        break;
    case 96:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("打开灯光\r\n");
        break;
    case 97:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("把灯打开\r\n");
        break;
    case 98:
        CIAS_LOG_HL("关闭灯光\r\n");
        break;
    case 99:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("关闭夜灯\r\n");
        break;
    case 100:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("把灯关闭\r\n");
        break;
    case 101:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("夜灯亮一点\r\n");
        break;
    case 102:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("灯光亮一点\r\n");
        break;
    case 103:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("亮一点\r\n");
        break;
    case 104:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("再亮一点\r\n");
        break;
    case 105:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("夜灯暗一点\r\n");
        break;
    case 106:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("灯光暗一点\r\n");
        break;
    case 107:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("暗一点\r\n");
        break;
    case 108:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("再暗一点\r\n");
        break;
    case 109:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("夜光最暗\r\n");
        break;
    case 110:
        ir_dev_type = IR_LIGHT_ID;
        CIAS_LOG_HL("夜光最亮\r\n");
        break;
    case 111:
        CIAS_LOG_HL("学习空调指令");
        cias_ir_partition_erase(IR_AIR_CONDITIONER_ID); //离线端学习空调指令，擦除在线存储分区
        return;
    case 112:
        CIAS_LOG_HL("学习电视指令");
        cias_ir_partition_erase(IR_IV_DATA_ID);
        return;
    case 113:
        CIAS_LOG_HL("学习机顶盒指令");
        cias_ir_partition_erase(IR_SET_TOP_BOX_ID);
        return;
    case 114:
        CIAS_LOG_HL("学习风扇指令");
        cias_ir_partition_erase(IR_FAN_ID);
        return;
    case 999:
        CIAS_LOG_HL("复位按钮按下")
        tuya_air_conditioner_timer_stop();
        tuya_iot_wf_gw_unactive();                           //重新配网
        for (int i = IR_SET_TOP_BOX_ID; i < IR_DEV_MAX; i++) //删除本地码库
        {
            cias_ir_partition_erase(i);
        }
        update_local_ir_code_info();
        return;
    default:
        CIAS_LOG_HL("未识别指令\r\n");
        return;
    }
    switch (ir_dev_type)
    {
    case IR_SET_TOP_BOX_ID:
    case IR_IV_DATA_ID:
    case IR_TV_BOX_ID:
        CIAS_LOG_HL("------ir_key_value = %d", ir_key_value);
        if (tuya_ir_data_find_from_flash(ir_dev_type, NULL, ir_key_value) == 0)
        {
            cmd_exec_status = OPRT_OK;
        }
        else
        {
            CIAS_LOG_HL("send ir cmd fail..\r\n");
        }
        break;
    case IR_DVD_ID:
        CIAS_LOG_HL("DVD设备\r\n");
        break;
    case IR_AIR_CONDITIONER_ID:
        CIAS_LOG_HL("空调设备\r\n");
        if (cmd == 5 || cmd == 6) //打开空调
        {
            sprintf(m_ir_key, "%s", "power_on");
        }
        else if (cmd == 7 || cmd == 8) //关闭空调
        {
            sprintf(m_ir_key, "%s", "power_off");
        }
        else if ((cmd >= 50) && (cmd <= 63))
        {
            CIAS_LOG_HL("空调定时设置, cmd = %d\r\n", cmd);
            tuya_air_conditioner_timer_start(60);              //60S中断一次
            if (g_air_conditioner_ir_status.power_status == 0) //当前空调处于关机状态，直接打开空调
            {
                g_air_conditioner_ir_status.power_status = 1;
                sprintf(m_ir_key, "M%d_T%d_S%d", g_air_conditioner_ir_status.mode_val, g_air_conditioner_ir_status.temp_val, g_air_conditioner_ir_status.wind_val);
            }
            else
            {
                return;
            }
        }
        else
        {
            g_air_conditioner_ir_status.power_status = 1;
            sprintf(m_ir_key, "M%d_T%d_S%d", g_air_conditioner_ir_status.mode_val, g_air_conditioner_ir_status.temp_val, g_air_conditioner_ir_status.wind_val);
        }
        CIAS_LOG_HL("------m_ir_key = %s", m_ir_key);
        if (tuya_ir_data_find_from_flash(ir_dev_type, m_ir_key, 0) == 0)
        {
            cmd_exec_status = OPRT_OK;
        }
        else
        {
            CIAS_LOG_HL("send ir cmd fail..\r\n");  //控制失败
            return;   
        }
        g_ty_irlib_dl_kt_info.mode = g_air_conditioner_ir_status.mode_val;
        g_ty_irlib_dl_kt_info.power = g_air_conditioner_ir_status.power_status;
        g_ty_irlib_dl_kt_info.spd = g_air_conditioner_ir_status.wind_val;
        g_ty_irlib_dl_kt_info.temp = g_air_conditioner_ir_status.temp_val;
        CIAS_LOG_HL("g_ty_irlib_dl_kt_info.spd = %d\r\n", g_ty_irlib_dl_kt_info.spd);
        CIAS_LOG_HL("g_ty_irlib_dl_kt_info.mode = %d\r\n", g_ty_irlib_dl_kt_info.mode);
        if ((g_air_conditioner_ir_status.ir_enable_flag == OPRT_OK) && (cmd_exec_status == OPRT_OK)) //指令执行成功，同步状态到app
        {
            Tuya_svc_irlib_dl_kt_status_rep(g_air_conditioner_ir_status.conditioner_ir_devid, &g_ty_irlib_dl_kt_info);
        }
        break;
    case IR_PROJECTOR_ID:
        CIAS_LOG_HL("投影仪设备\r\n");
        break;
    case IR_AUDIO_ID:
        CIAS_LOG_HL("音响设备\r\n");
        break;
    case IR_FAN_ID:
        CIAS_LOG_HL("风扇设备\r\n");
        CIAS_LOG_HL("------m_ir_key = %s", m_ir_key);
        if (tuya_ir_data_find_from_flash(ir_dev_type, m_ir_key, ir_key_value) == 0)
        {
            cmd_exec_status = OPRT_OK;
        }
        else
        {
            CIAS_LOG_HL("send ir cmd fail..\r\n");  //控制失败
            return;   
        }
        break;
    case IR_CAMERA_ID:
        CIAS_LOG_HL("单反相机设备\r\n");
        break;
    case IR_LIGHT_ID:
        CIAS_LOG_HL("灯光设备\r\n");
        break;
    case IR_FILTER_ID:
        CIAS_LOG_HL("净化器设备\r\n");
        break;
    case IR_GEYSER_ID:
        CIAS_LOG_HL("热水器设备\r\n");
        break;
    default:
        CIAS_LOG_HL("ir_dev_type: val error\r\n");
        break;
    }

    CIAS_LOG_HL("tuya_ir_ctrl_audio_cmd_handle is exec end....\r\n");
#endif
}