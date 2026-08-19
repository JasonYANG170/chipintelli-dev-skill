#include "FreeRTOS.h" 
#include "task.h"
#include "timers.h"
#include "cias_ble_msg_deal.h"
#include "system_msg_deal.h"
#include "ble_param_config.h"
#include "sdk_default_config.h"
#include "crc.h"
#include "cias_fan_msg_deal.h"
#include "ci_log.h"
#include "status_share.h"
uint32_t fan_timer_counter = 0;//定时器秒数
xTimerHandle fan_timer = NULL; //定时器
static fan_dev_t fan_dev;//设备功能状态


static fan_timer_callback()
{
    if (fan_timer_counter)
    {
        fan_timer_counter --;
        mprintf("定时剩余：%d S\r\n",fan_timer_counter);
    }
    else
    {
        xTimerStop(fan_timer,0);
        fan_dev.timing = FUN_TURN_OFF; 
        mprintf("定时时间到\r\n");
        fan_report(TURN_OFF);
    }
}


void set_fan_timing(uint8_t timing)
{
    if ((timing >= DEV_TIMING_MIN)&&(timing <= DEV_TIMING_MAX))
    {
        prompt_play_by_cmd_id((timing - DEV_TIMING_MIN) + TIMMING_1H, -1, default_play_done_callback, true);
        uart_send_asr((timing - DEV_TIMING_MIN) + TIMMING_1H);
        fan_dev.timing = timing; 
        
        fan_timer_counter = (fan_dev.timing-0xA0)*3600;
        xTimerStop(fan_timer, 0);
        xTimerStart(fan_timer, 0);
    }
}


void fan_init()
{
    fan_dev.power = FUN_TURN_OFF;
    fan_dev.speed = DEV_SPEED_MIN;
    fan_dev.shake = FUN_TURN_OFF;
    fan_dev.mode = FAN_MODE_NORMAL;
    fan_dev.anion = FUN_TURN_OFF;
    fan_dev.timing = FUN_TURN_OFF;
    fan_dev.asr_status = FUN_TURN_ON;   //默认开识别
    fan_timer = xTimerCreate("fan_timer", pdMS_TO_TICKS(1000),pdTRUE, (void *)0, fan_timer_callback);
}

uint8_t fan_report(uint16_t cmd_id)
{
    bool send_flag = true;
    uint8_t send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t len;

    send_data[0] = 0xA5;
    send_data[1] = 0x5A;
    send_data[2] = 0x01;
    send_data[3] = 0x01;
    send_data[4] = FAN_DEV;
    send_data[5] = DEV_NUMBER_ID;
    send_data[6] = 0x21;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    switch (cmd_id)
    {
        case TURN_OFF:
        {
            send_data[7] = FAN_POWER;
            send_data[10] = FUN_TURN_OFF;//数据
            fan_dev.power = send_data[10];
            fan_dev.speed = DEV_SPEED_MIN;
            fan_dev.shake = FUN_TURN_OFF;
            fan_dev.mode = FAN_MODE_NORMAL;
            fan_dev.anion = FUN_TURN_OFF;
            fan_dev.timing = FUN_TURN_OFF;
            xTimerStop(fan_timer, 0);
            break;
        }
        case TURN_ON:
        {
            send_data[7] = FAN_POWER;
            send_data[10] = FUN_TURN_ON;//数据
            fan_dev.power = send_data[10];
            break;
        }
        case SPEED_ONE ... SPEED_SIX://一档风~六档风
        {
            send_data[7] =  FAN_SPEED;
            send_data[10] = (cmd_id - SPEED_ONE) + DEV_SPEED_MIN;//数据
            fan_dev.speed = send_data[10];
            break;
        }
        case SPEED_RAISE://风速增大
        {
            send_data[7] = FAN_SPEED;
            if (fan_dev.speed < DEV_SPEED_MAX)
            {
                fan_dev.speed ++;
            }
            send_data[10] = fan_dev.speed;//数据
            break;
        }
        case SPEED_REDUCE://风速减小
        {
            send_data[7] = FAN_SPEED;
            if (fan_dev.speed > DEV_SPEED_MIN)
            {
                fan_dev.speed --;
            }
            send_data[10] = fan_dev.speed;//数据
            break;
        }
        case SPEED_MAX://风速最大
        {
            send_data[7] = FAN_SPEED;           
            send_data[10] = DEV_SPEED_MAX;//数据
            fan_dev.speed = send_data[10];
            break;
        }
        case SPEED_MIN://风速最小
        {
            send_data[7] = FAN_SPEED;                
            send_data[10] = DEV_SPEED_MIN;//数据
            fan_dev.speed = send_data[10];
            break;
        }
        case SHAKE_OFF://关闭摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = FUN_TURN_OFF;//数据
            fan_dev.shake = send_data[10];
            break;
        }
        case SHAKE_ON://打开摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = FUN_TURN_ON;//数据
            fan_dev.shake = send_data[10];
            break;
        }
        case SHAKE_LR_OFF://关闭左右摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = SHAKE_LR_OFF_DATA;//数据
            fan_dev.shake = send_data[10];
            break;
        }
        case SHAKE_LR_ON://打开左右摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = SHAKE_LR_ON_DATA;//数据
            fan_dev.shake = send_data[10];
            break;
        }
        case SHAKE_HD_OFF://关闭上下摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = SHAKE_HD_OFF_DATA;//数据
            fan_dev.shake = send_data[10];
            break;
        }
        case SHAKE_HD_ON://打开上下摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = SHAKE_HD_ON_DATA;//数据
            fan_dev.shake = send_data[10];
            break;
        }
        case NORMAL_ON://打开正常风
        {
            send_data[7] = FAN_MODE;
            send_data[10] = FAN_MODE_NORMAL;//数据
            fan_dev.mode = send_data[10];
            break;
        }
        case SLEEP_ON://打开睡眠
        {
            send_data[7] = FAN_MODE;
            send_data[10] = FAN_MODE_SLEEP;//数据
            fan_dev.mode = send_data[10];
            break;
        }
        case NATURAL_ON://打开自然风
        {
            send_data[7] = FAN_MODE;
            send_data[10] = FAN_MODE_NATURAL;//数据
            fan_dev.mode = send_data[10];
            break;
        }
        case ANION_ON://打开负离子
        {
            send_data[7] = FAN_ANION;
            send_data[10] = FUN_TURN_ON;//数据
            fan_dev.anion = send_data[10];
            break;
        }
        case ANION_OFF://关闭负离子
        {
            send_data[7] = FAN_ANION;
            send_data[10] = FUN_TURN_OFF;//数据
            fan_dev.anion = send_data[10];
            break;
        }
        case TIMING_OFF://关闭定时
        {
            send_data[7] = FAN_TIMING;
            send_data[10] = FUN_TURN_OFF;//数据
            fan_dev.timing = send_data[10];
            xTimerStop(fan_timer, 0);
            break;
        }
        case TIMMING_1H ... TIMMING_12H://定时X小时
        {
            send_data[7] = FAN_TIMING;
            send_data[10] = (cmd_id - TIMMING_1H) + DEV_TIMING_MIN;//数据
            fan_dev.timing = send_data[10];
            fan_timer_counter = (fan_dev.timing - 0xA0)*3600;
            xTimerStop(fan_timer, 0);
            xTimerStart(fan_timer, 0);
            break;
        }
        default:
        {
            send_flag = false;
            break;
        }
    }
    if (send_flag)
    {
        uint16_t crc_cal = crc16_ccitt(0, send_data, 11);
        mprintf("crc = 0x%x\r\n",crc_cal);
        //send_data[send_data[7]+8] = crc_cal >> 8;
        send_data[11] = crc_cal>>8;
        send_data[12] = crc_cal&0xFF;

        cias_crypto_data(send_data, 13);
        ble_send_payload(send_data, 13, BLE_UUID_CIAS_NOTIFY);
        
    }
    return 0;
}

void fan_callback(ble_msg_V1_t msg)
{
    int8_t ret = -1;
    switch (msg.function_id)
    {
        case FAN_POWER:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关风扇\r\n");       
                prompt_play_by_cmd_id(TURN_OFF, -1, default_play_done_callback,true);      
                uart_send_asr(TURN_OFF);  
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("开风扇\r\n");
                prompt_play_by_cmd_id(TURN_ON, -1, default_play_done_callback,true);  
                uart_send_asr(TURN_ON);     
            }
            fan_dev.power =  msg.data[0];
            fan_dev.speed = DEV_SPEED_MIN;
            fan_dev.shake = FUN_TURN_OFF;
            fan_dev.mode = FAN_MODE_NORMAL;
            fan_dev.anion = FUN_TURN_OFF;
            fan_dev.timing = FUN_TURN_OFF;
            xTimerStop(fan_timer, 0);
            break;
        }
        case FAN_SPEED:
        {
            prompt_play_by_cmd_id(SPEED_ONE+msg.data[0]-1, -1, default_play_done_callback,true);  
            uart_send_asr(SPEED_ONE + msg.data[0] -1); 
            fan_dev.speed = msg.data[0];
            break;
        }
        case FAN_SHAKE:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(SHAKE_OFF);        
            }
            if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_ON, -1, default_play_done_callback,true);   
                uart_send_asr(SHAKE_ON);      
            }
            else if (msg.data[0] == SHAKE_LR_OFF_DATA)
            {
                mprintf("关闭左右摇头\r\n");       
                prompt_play_by_cmd_id(SHAKE_LR_OFF, -1, default_play_done_callback,true);  
                uart_send_asr(SHAKE_LR_OFF);          
            }
            else if (msg.data[0] == SHAKE_LR_ON_DATA)
            {
                mprintf("打开左右摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_LR_ON, -1, default_play_done_callback,true); 
                uart_send_asr(SHAKE_LR_ON);        
            }
            else if (msg.data[0] == SHAKE_HD_OFF_DATA)
            {
                mprintf("关闭上下摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_HD_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(SHAKE_HD_OFF);        
            }
            else if (msg.data[0] == SHAKE_HD_ON_DATA)
            {
                mprintf("打开上下摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_HD_ON, -1, default_play_done_callback,true);   
                uart_send_asr(SHAKE_HD_ON);      
            }
            fan_dev.shake = msg.data[0];
            break;
        }
        case FAN_MODE:
        {
            if (msg.data[0] == FAN_MODE_NORMAL)
            {
                mprintf("打开正常风\r\n");       
                prompt_play_by_cmd_id(NORMAL_ON, -1, default_play_done_callback,true); 
                uart_send_asr(NORMAL_ON);             
            }
            else if (msg.data[0] == FAN_MODE_SLEEP)
            {
                mprintf("打开睡眠风\r\n");       
                prompt_play_by_cmd_id(SLEEP_ON, -1, default_play_done_callback,true); 
                uart_send_asr(SLEEP_ON);   
            }
            else if (msg.data[0] == FAN_MODE_NATURAL)
            {
                mprintf("打开自然风\r\n");       
                prompt_play_by_cmd_id(NATURAL_ON, -1, default_play_done_callback,true); 
                uart_send_asr(NATURAL_ON);   
            }
            fan_dev.mode = msg.data[0];
            break;
        }
        case FAN_ANION:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭负离子\r\n");
                prompt_play_by_cmd_id(ANION_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(ANION_OFF);        
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开负离子\r\n");       
                prompt_play_by_cmd_id(ANION_ON, -1, default_play_done_callback,true); 
                uart_send_asr(ANION_ON);           
            }
            fan_dev.anion = msg.data[0];
            break;
        }
        case FAN_TIMING:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭定时\r\n");
                prompt_play_by_cmd_id(TIMING_OFF, -1, default_play_done_callback,true);   
                uart_send_asr(TIMING_OFF);   
                fan_dev.timing = FUN_TURN_OFF;   
                xTimerStop(fan_timer,0);
            }
            else
            {
                set_fan_timing(msg.data[0]);              
            }
            break;
        }
        case FAN_SPEAKER:
        {
            uint8_t vol;
            int select_index = -1;
            if (msg.data[0] == VOICE_UP_DATA)
            {
                mprintf("音量增大\r\n");
                vol = vol_set(vol_get() + 1);
                select_index = (vol == VOLUME_MAX) ? 1:0;
                prompt_play_by_cmd_id(VOICE_UP, select_index, default_play_done_callback,true);   
                
            }
            else if (msg.data[0] == VOICE_DOWN_DATA)
            {
                mprintf("音量减小\r\n");
                vol = vol_set(vol_get() - 1);
                select_index = (vol == VOLUME_MIN) ? 1:0;
                prompt_play_by_cmd_id(VOICE_DOWN, select_index, default_play_done_callback,true);   
                
            }
            else if (msg.data[0] == VOICE_MAX_DATA)
            {
                mprintf("音量最大\r\n");
                vol_set(VOLUME_MAX);
                prompt_play_by_cmd_id(VOICE_MAX, -1, default_play_done_callback,true);   
                
            }
            else if (msg.data[0] == VOICE_MIN_DATA)
            {
                mprintf("音量最小\r\n");
                vol_set(VOLUME_MIN);
                prompt_play_by_cmd_id(VOICE_MIN, -1, default_play_done_callback,true);   
                
            }
            break;
        }
        case FAN_ASR:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭语音识别\r\n");
                prompt_play_by_cmd_string("<已关闭语音识别>", -1, default_play_done_callback,true);
                if(fan_dev.asr_status == FUN_TURN_ON)
                {
                    fan_dev.asr_status = FUN_TURN_OFF;
                    pause_asr();
                }
                
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开语音识别\r\n");       
                prompt_play_by_cmd_string("<已打开语音识别>", -1, default_play_done_callback,true);
                if(fan_dev.asr_status == FUN_TURN_OFF)
                {
                    fan_dev.asr_status = FUN_TURN_ON;
                    resume_asr();
                }
            }
            break;
        }
        default:
        {
            mprintf("other cmd\r\n");
            break;
        }
    }

}

void fan_query(ble_msg_V1_t msg)
{
    if (msg.function_id != 0x00)
    {
        return;
    }
    mprintf("查询设备全属性指令");
    fan_query_all();
}


/*
    启英设备全属性上报小程序
    协议格式：帧头(2Byte)+协议版本(1Byte)+功能状态(nByte)+CRC16校验(2Byte)
    *帧头(2Byte): 0xA55A
    *协议版本(1Byte): 0x03
    *功能状态(nByte): 可填入任意个功能，但功能位置顺序保持固定
    {
        电源(1Byte): 0x01关; 0x02开;
        风速(1Byte): 0x01~0xFF,1档风~255档风;
        风向(1Byte): 0x01,关摇头; 0x02,开摇头; 0x11,左右摇头关; 0x12,左右摇头开; 0x21,上下摇头关; 0x22,上下摇头开; 0x31,左右上下摇头关; 0x32,左右上下摇头开;
        模式(1Byte): 0x01智能风; 0x02宝宝风; 0x03正常风; 0x04睡眠风; 0x05自然风; 0x06暴风模式;
        预留(1Byte): 0x00;
        预留(1Byte): 0x00;
        负离子(1Byte): 0x01关; 0x02开;
        定时(1Byte): 0x01,关; 0xA1~0xAC,定时一小时~定时十二小时; 
        播报音量(1Byte): 0x00;
        语音识别(1Byte): 0x01关; 0x02开;
        定时设置(2Byte): 0x0000;
        风速设置(1Byte): 0x0000;
        风模式设置(1Byte): 0x00;
        驱蚊(1Byte): 0x01关; 0x02开;
        灯光(1Byte): 0x01关; 0x02开;
        暖灯(1Byte): 0x01关; 0x02开;
        加湿(1Byte): 0x01关; 0x02开;
        制冷(1Byte): 0x01关; 0x02开;
        温度显示(1Byte): 0x01关; 0x02开;
        屏显(1Byte): 0x01关; 0x02开;
    }
    *CRC16校验(2Byte): crc16_ccitt函数获得
*/
void fan_query_all()
{
    uint8_t send_data[BLE_MSG_DATA_MAX_SIZE] = {0};
    uint8_t len = 0;

    send_data[len++] = 0xA5;
    send_data[len++] = 0x5A;
    send_data[len++] = 0x03;
    send_data[len++] = fan_dev.power;
    send_data[len++] = fan_dev.speed;
    send_data[len++] = fan_dev.shake;
    send_data[len++] = fan_dev.mode;
    send_data[len++] = 0x00;
    send_data[len++] = 0x00;
    send_data[len++] = fan_dev.anion;
    send_data[len++] = fan_dev.timing;
    send_data[len++] = 0x00;
    send_data[len++] = fan_dev.asr_status;

    uint16_t crc_cal = crc16_ccitt(0, send_data, len);
    // mprintf("crc = 0x%x\r\n",crc_cal);
    send_data[len++] = crc_cal>>8;
    send_data[len++] = crc_cal&0xFF;
    cias_crypto_data(send_data, len); 
    ble_send_payload(send_data, len, BLE_UUID_CIAS_NOTIFY);
}
