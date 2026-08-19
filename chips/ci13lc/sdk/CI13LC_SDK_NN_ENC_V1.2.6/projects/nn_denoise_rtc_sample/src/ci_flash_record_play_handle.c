
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ci_flash_record_play_handle.h"
#include "ci_flash_data_info.h"
#include "ci_uart_audio_data_handle.h"
#include "stream_buffer.h"
#include "ci_nvdata_manage.h"
#include "ci13lc_dpmu.h"
#include "voice_module_uart_protocol.h"
#include "ci13lc_gpio.h"
#include "ci13lc_codec.h"
#define RTC_RECORD_LOG_ON   0
vox_info_t vox_info;
test_info_t test_info;
// 按键参数配置
#define SINGLE_PRESS_VALID_MIN_TIME_MS 100               //短按最小触发时间-消抖 防毛刺
#define LONG_PRESS_VALID_MIN_TIME_MS  1300               //长按最小触发时间
#define LONG_PRESS_VALID_MAX_TIME_MS  10000              //长按最大触发时间-超过该时间，长按无效(异常处理)
#define DOUBLE_CLICK_TIME_MS 500                         //双击有效的间隔时间
#define UART_SPEEX_FRAME_LEN 43                          //speex编码后每帧长度-----不可修改
#define WRITE_FLASH_SPEEX_SIZE 512                       //读/写Flash一次的字节数(大于43字节的2N次方长度:64 128 256 512....)
#define WRITE_FLASH_SPEEX_NUM (WRITE_FLASH_SPEEX_SIZE / UART_SPEEX_FRAME_LEN)   //读/写Flash一次包含speex压缩包个数-----不可修改
#define WRITE_FLASH_SPEEX_ENCODE_STREAN_SIZE WRITE_FLASH_SPEEX_NUM * UART_SPEEX_FRAME_LEN   //读/写Flash一次包含speex编码数据字节数-----不可修改

uint8_t new_play_index = 0;                              //记录录音后需要回放的最新音频
uint8_t rtc_flash_record_left = 0;                       //录音降噪后
uint8_t rtc_flash_record_key_func_status = 1;            //录音功能按键功能状态，如果是下拉，默认配置为0
uint8_t rtc_audio_out_switch_key_func_status = 1;        //音频输出模式切换按键功能状态，如果是下拉，默认配置为0
StreamBufferHandle_t gFlashEncodeStreamBuffer = NULL;    //Flash接收音频speex编码数据队列
rtc_record_status_t rtc_record_status = RECORD_END;
rtc_play_status_t rtc_play_status = PLAY_END;
QueueHandle_t rtc_protocol_msg_recv_queue = NULL;        //电控协议消息接收队列
uint32_t flash_available_offset;
extern bool speex_request_play_flag;                           //解码任务数据请求标志
extern QueueHandle_t gSpeexdecodeDataQueue;             //解码队列
extern StreamBufferHandle_t gSpeexPlayStreamBuffer;      //hpout播放流

static rtc_flash_record_play_info_t rtc_record_info = {0};
static rtc_flash_record_play_info_t rtc_play_info = {0};
static uint32_t rtc_flash_record_play_info_addr = 0;  //录音播放信息的flash存放地址

//打印当前录音信息
_XIF_ void print_rtc_flash_record_play_info()
{
    //mprintf("录音信息：write_index %d, play_index %d, write_addr %x, crc %x\r\n", rtc_record_info.write_index, rtc_record_info.play_index, rtc_record_info.write_addr, rtc_record_info.crc);
    for (int i = 0; i < RECORD_INFO_MAX; i++)
    {
        mprintf("  %d:%d, [%x, %x];", i, rtc_record_info.info[i].available, rtc_record_info.info[i].start_addr, rtc_record_info.info[i].end_addr);
    }
    //mprintf("\r\n");
}

//更新录音信息到flash
uint32_t head_cover_addr = 0;
uint32_t head_cover_end_addr = 0;
uint32_t record;
_XIF_ static void updata_rtc_record_info()
{
    uint16_t crc = crc16_ccitt(0, &rtc_record_info, sizeof(rtc_flash_record_play_info_t)-2);
    rtc_record_info.crc = crc;
    post_write_flash(&rtc_record_info, head_cover_addr, sizeof(rtc_flash_record_play_info_t));
    head_cover_addr += sizeof(rtc_flash_record_play_info_t);
    #if RTC_RECORD_LOG_ON
    mprintf("record_info updata %x\r\n", head_cover_addr);
    #endif
    if(head_cover_addr >= head_cover_end_addr-sizeof(rtc_flash_record_play_info_t))
    {
        #if RTC_RECORD_LOG_ON
        mprintf(">>>>>>>updata record info end........\r\n");
        #endif
        head_cover_addr = RECORD_HEAD_INFO_ADDR;
        record = test_info.w_full;
        post_erase_flash(head_cover_addr, ERASE_4K*RECORD_HEAD_INFO_PAGE);
        test_info.w_full = record;
        post_write_flash(&rtc_record_info, head_cover_addr, sizeof(rtc_flash_record_play_info_t));
        head_cover_addr += sizeof(rtc_flash_record_play_info_t);
        #if RTC_RECORD_LOG_ON
        mprintf("success %x\r\n", head_cover_addr);
        #endif
    }
    print_rtc_flash_record_play_info();
}

_XIF_ reset_rtc_record_index(uint8_t index)
{
    rtc_record_info.info[index].available = false;
    rtc_record_info.info[index].start_addr = 0;
    rtc_record_info.info[index].end_addr = 0;
    updata_rtc_record_info();
}

_XIF_ void check_rtc_record_data()
{
    for(int i = 0; i < RECORD_INFO_MAX; i++)
    {
        if(rtc_record_info.info[i].available == true)
        {
            if (rtc_record_info.info[i].start_addr > rtc_record_info.info[i].end_addr)
            {
                reset_rtc_record_index(i);
            }
        }
    }
}

_XIF_ void rtc_record_data_init(uint32_t addr)
{
    rtc_record_info.play_index = 0;
    rtc_record_info.write_index = 0;
    rtc_record_info.write_addr = addr;
    for(int i = 0; i < RECORD_INFO_MAX; i++)
    {
        rtc_record_info.info[i].available = false;
        rtc_record_info.info[i].start_addr = 0;
        rtc_record_info.info[i].end_addr = 0;
    }
}


//每存够4k判断一次是否需要把前一个index存储的录音信息覆盖掉
_XIF_ static void updata_rtc_record_index()
{
    #if RTC_RECORD_LOG_ON
    mprintf("updata_rtc_record_index %d %x:", rtc_record_info.write_index, rtc_record_info.write_addr);
    #endif
    uint8_t next_index;
    if((rtc_record_info.write_index + 1) == RECORD_INFO_MAX)
    {
        for(int i = 0; i < rtc_record_info.write_index; i++)
        {
            if(rtc_record_info.info[i].available == true)
            {
                #if RTC_RECORD_LOG_ON
                mprintf("[back %d %x]", i, rtc_record_info.info[i].start_addr);
                #endif
                if(rtc_record_info.write_addr == rtc_record_info.info[i].start_addr)
                {
                    reset_rtc_record_index(i);
                }
                return;
            }
            
        }
    }
    else
    {
        next_index = rtc_record_info.write_index + 1;
        for(int i = next_index; i < RECORD_INFO_MAX; i++)
        {
            if(rtc_record_info.info[i].available == true)
            {
                #if RTC_RECORD_LOG_ON
                mprintf("[fornt %d %x]", i, rtc_record_info.info[i].start_addr);
                #endif
                if(rtc_record_info.write_addr == rtc_record_info.info[i].start_addr)
                {
                    reset_rtc_record_index(i);
                }
                return;
            }
        }
        for(int i = 0; i < rtc_record_info.write_index; i++)
        {
            if(rtc_record_info.info[i].available == true)
            {
                #if RTC_RECORD_LOG_ON
                mprintf("[back %d %x]", i, rtc_record_info.info[i].start_addr);
                #endif
                if(rtc_record_info.write_addr == rtc_record_info.info[i].start_addr)
                {
                    reset_rtc_record_index(i);
                }
                return;
            }
        }
    }
    #if RTC_RECORD_LOG_ON
    mprintf("\r\n");
    #endif
}

static bool record_play_force_stop = false;
static bool pause_record_play = false;
/* 开始录音 */ 
static uint32_t record_write_addr = 0;
_XIF_ void rtc_record_start(void)
{
    if (rtc_record_status == RECORD_END)
    {
        rtc_play_stop();
        if(rtc_record_info.write_addr % ERASE_4K != 0)
            rtc_record_info.write_addr = rtc_record_info.write_addr - (rtc_record_info.write_addr % ERASE_4K) + ERASE_4K;   //开始地址,4k对齐
        #if RTC_RECORD_LOG_ON
        mprintf("开始录音,id: %d, 起始地址: %x\r\n", rtc_record_info.write_index, rtc_record_info.write_addr);
        #endif
        record_write_addr = rtc_record_info.write_addr;
        if(rtc_record_info.write_addr == RECORD_HEAD_INFO_ADDR)
        {
            #if RTC_RECORD_LOG_ON
            mprintf("上次已经到了flash结尾,自动从新开始\r\n");
            #endif
            rtc_record_info.write_addr = flash_available_offset;
        }
        rtc_record_info.info[rtc_record_info.write_index].available = true; 
        rtc_record_info.info[rtc_record_info.write_index].start_addr = rtc_record_info.write_addr;
        post_erase_flash(rtc_record_info.write_addr, ERASE_4K);    //开始录音4k擦除
        rtc_record_status = RECORD_START;
        updata_rtc_record_index();
    }
    else
    {
        #if RTC_RECORD_LOG_ON
        mprintf("已开始录音,请不要重复点击\r\n");
        #endif
    }
}

_XIF_ void stop_record(void)
{
    xStreamBufferReset(gFlashEncodeStreamBuffer);   //清除录音剩余不足一包数据，防止合并进下个录音
    #if RTC_RECORD_LOG_ON
    mprintf("结束录音 %d...%x\r\n", rtc_record_info.write_index, rtc_record_info.write_addr);
    #endif
    if (record_write_addr == rtc_record_info.write_addr)
    {
        #if RTC_RECORD_LOG_ON
        mprintf("录音信息为空\r\n");
        #endif
        return;
    }
    new_play_index = rtc_record_info.write_index;
    rtc_record_info.info[rtc_record_info.write_index].end_addr = rtc_record_info.write_addr;
    rtc_record_info.write_index ++;
    if(rtc_record_info.write_index == RECORD_INFO_MAX)
        rtc_record_info.write_index = 0;
    #if RTC_RECORD_LOG_ON
    mprintf("结束地址 %x", rtc_record_info.write_addr);
    #endif
    updata_rtc_record_info();
    uint8_t temp_play_index = rtc_play_info.play_index;
    memcpy(&rtc_play_info, &rtc_record_info, sizeof(rtc_flash_record_play_info_t));
    rtc_play_info.play_index = temp_play_index;
}

_XIF_ void rtc_record_end(void)
{
    if (rtc_record_status != RECORD_END)
    {
        rtc_record_status = RECORD_END;
        stop_record();
    }
    else
    {
        #if RTC_RECORD_LOG_ON
        mprintf("没有开始录音,请不要重复点击\r\n");
        #endif
    }
}

//暂停录音
_XIF_ void rtc_record_pause()
{
    #if RTC_RECORD_LOG_ON
    mprintf("暂停录音  ");
    #endif
    if (rtc_record_status == RECORD_START)
    {
        #if RTC_RECORD_LOG_ON
        mprintf("生效\r\n");
        #endif
        rtc_record_status = RECORD_PAUSE;
    }
    else
    {
        #if RTC_RECORD_LOG_ON
        mprintf("无效\r\n");
        #endif
    }
}

_XIF_ void rtc_record_resume()
{
    #if RTC_RECORD_LOG_ON
    mprintf("恢复录音  ");
    #endif
    if (rtc_record_status == RECORD_PAUSE)
    {
        #if RTC_RECORD_LOG_ON
        mprintf("生效\r\n");
        #endif
        rtc_record_status = RECORD_START;
    }
    else
    {
        #if RTC_RECORD_LOG_ON
        mprintf("无效\r\n");
        #endif
    }
}

_XIF_ void rtc_flash_record_task()
{
    int32_t stream_aviable_len = 0;
    int8_t speex_encode_buf[WRITE_FLASH_SPEEX_SIZE] = {0};
    gFlashEncodeStreamBuffer = xStreamBufferCreate(WRITE_FLASH_SPEEX_ENCODE_STREAN_SIZE * 2, WRITE_FLASH_SPEEX_ENCODE_STREAN_SIZE);
    if (gFlashEncodeStreamBuffer == NULL)
    {
        mprintf("create error...-%s %d\r\n",__FUNCTION__, __LINE__);
        // 处理错误情况
    }
    while(1)
    {
        stream_aviable_len = xStreamBufferBytesAvailable(gFlashEncodeStreamBuffer);
        if (stream_aviable_len >= WRITE_FLASH_SPEEX_ENCODE_STREAN_SIZE)
        {                 
            memset(speex_encode_buf, 0, sizeof(speex_encode_buf));
            int rx_size = xStreamBufferReceive(gFlashEncodeStreamBuffer, speex_encode_buf, WRITE_FLASH_SPEEX_ENCODE_STREAN_SIZE, portMAX_DELAY);
            if (rx_size != WRITE_FLASH_SPEEX_ENCODE_STREAN_SIZE)
            {
                mprintf("gFlashEncodeStreamBuffer rcv error...-%s %d\r\n",__FUNCTION__, __LINE__);
            }
            else
            { 
                #if RTC_RECORD_LOG_ON
                mprintf("flash_write_addr=%x\r\n",rtc_record_info.write_addr);
                #endif
                if (rtc_record_status != RECORD_START)
                {
                   continue;
                }
                
                if(RETURN_ERR == post_write_flash(speex_encode_buf, rtc_record_info.write_addr, WRITE_FLASH_SPEEX_SIZE))
                    mprintf("gFlashEncodeStreamBuffer flash写入失败-%s %d\r\n",__FUNCTION__, __LINE__); 
                rtc_record_info.write_addr += WRITE_FLASH_SPEEX_SIZE;  //指向下包数据写入地址
                if(rtc_record_info.write_addr % ERASE_4K == 0)
                {
                    if(rtc_record_info.write_addr == RECORD_HEAD_INFO_ADDR)
                    {
                        #if RTC_RECORD_LOG_ON
                        mprintf("录音到了flash结尾,自动结束录音...\r\n");
                        #endif
                        rtc_record_end();
                        rtc_record_info.write_addr = flash_available_offset;
                        //updata_rtc_record_info();
                        rtc_record_start();
                    }
                    else
                    {
                        updata_rtc_record_index();
                        if(RETURN_OK != post_erase_flash(rtc_record_info.write_addr, ERASE_4K))
                            mprintf("4k flash erase fail...-%s %d\r\n",__FUNCTION__, __LINE__); 
                        if ((rtc_record_info.write_addr - rtc_record_info.info[rtc_record_info.write_index].start_addr) % RECORD_BACKUP_LEN == 0)
                        {
                            #if RTC_RECORD_LOG_ON
                            mprintf("强制保存一次\r\n");
                            #endif
                            rtc_record_info.info[rtc_record_info.write_index].end_addr = rtc_record_info.write_addr;
                            updata_rtc_record_info();
                        }
                        
                    }
                }
            }
            //mprintf("FLASH_START_RECORD success...\r\n");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/*----------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------*/
bool rtc_play_local_flag = false;
bool rtc_play_local_type_speex = false;
uint32_t temp_play_loacl_pcm_counter = 0;
uint32_t record_play_start_addr, record_play_end_addr;
_XIF_ void rtc_play_stop(void)
{
    #if RTC_RECORD_LOG_ON
    mprintf("停止播放 ");
    #endif
    if (rtc_play_status == PLAY_START)
    {
        rtc_play_status = PLAY_STOP;
        while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(5));
            if (rtc_play_status == PLAY_END)
            {
                break;
            }
        }
    }
    else if (rtc_play_status == PLAY_STOP)
    {
        while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(5));
            if (rtc_play_status == PLAY_END)
            {
                break;
            }
        }
    }
    else
    {
        #if RTC_RECORD_LOG_ON
        mprintf("无效\r\n");
        #endif
        return;
    }
    #if RTC_RECORD_LOG_ON
    mprintf("成功\r\n");
    #endif
}

_XIF_ void start_play(uint32_t start, uint32_t end)
{
    if (start >= end)
    {
        #if RTC_RECORD_LOG_ON
        mprintf("start_play error %x : %x\r\n", start, end);
        #endif
        return;
    }
    
    if (rtc_play_status == PLAY_START)
    {
        rtc_play_status = PLAY_STOP;
        #if RTC_RECORD_LOG_ON
        mprintf("结束上一首播放\r\n");
        #endif
        while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(5));
            if (rtc_play_status == PLAY_END)
            {
                break;
            }
        }
    }
    else if (rtc_play_status == PLAY_STOP)
    {
        #if RTC_RECORD_LOG_ON
        mprintf("等待上一首播放结束\r\n");
        #endif
        while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(5));
            if (rtc_play_status == PLAY_END)
            {
                break;
            }
        }
    }
    rtc_play_status = PLAY_START;
    record_play_start_addr = start;
    record_play_end_addr = end;
}

/* 播放下一录音 */ 
_XIF_ void rtc_play_next(void)
{
    bool get_play_index_success = false;
    rtc_play_info.play_index ++;
    if(rtc_play_info.play_index==RECORD_INFO_MAX)
    {
        rtc_play_info.play_index = 0;
        if(rtc_play_info.info[0].available == true)
        {
            get_play_index_success = true;
        }
        else
            rtc_play_info.play_index ++;
    }
    if(!get_play_index_success)
    {
        for(int i = rtc_play_info.play_index; i < RECORD_INFO_MAX; i++)
        {
            if(rtc_play_info.info[i].available == true)
            {
                rtc_play_info.play_index = i;
                get_play_index_success = true;
                break;
            }
        }
    }
    if(!get_play_index_success)
    {
        for(int i = 0; i < rtc_play_info.play_index; i++)
        {
            if(rtc_play_info.info[i].available == true)
            {
                rtc_play_info.play_index = i;
                get_play_index_success = true;
                break;
            }
        }
    }
    if(!get_play_index_success)
    {
        rtc_play_info.play_index = 0;
        #if RTC_RECORD_LOG_ON
        mprintf("当前没有录音可供播放...\r\n");
        #endif
    }
    else
    {
        #if RTC_RECORD_LOG_ON
        mprintf("开始播放录音:%d\r\n", rtc_play_info.play_index);
        #endif
        rtc_play_local_flag = false;
        rtc_play_local_type_speex = false;
        start_play(rtc_play_info.info[rtc_play_info.play_index].start_addr, 
            rtc_play_info.info[rtc_play_info.play_index].end_addr);
    }
}

/* 播放上一录音 */ 
_XIF_ void rtc_play_previous(void)
{
    bool get_play_index_success = false;
    if(rtc_play_info.play_index==0)
    {
        rtc_play_info.play_index = RECORD_INFO_MAX-1;
        if(rtc_play_info.info[rtc_play_info.play_index].available == true)
        {
            get_play_index_success = true;
        }
        else
            rtc_play_info.play_index --;
    }
    else
        rtc_play_info.play_index --;
    if(!get_play_index_success)
    {
        for(int i = rtc_play_info.play_index; i >= 0; i--)
        {
            if(rtc_play_info.info[i].available == true)
            {
                rtc_play_info.play_index = i;
                get_play_index_success = true;
                break;
            }
        }
    }
    if(!get_play_index_success)
    {
        for(int i = RECORD_INFO_MAX; i > rtc_play_info.play_index; i--)
        {
            if(rtc_play_info.info[i].available == true)
            {
                rtc_play_info.play_index = i;
                get_play_index_success = true;
                break;
            }
        }
    }
    if(!get_play_index_success)
    {
        rtc_play_info.play_index = 0;
        #if RTC_RECORD_LOG_ON
        mprintf("当前没有录音可供播放...\r\n");
        #endif
    }
    else
    {
        #if RTC_RECORD_LOG_ON
        mprintf("开始播放录音:%d\r\n", rtc_play_info.play_index);
        #endif
        rtc_play_local_flag = false;
        rtc_play_local_type_speex = false;
        start_play(rtc_play_info.info[rtc_play_info.play_index].start_addr, 
            rtc_play_info.info[rtc_play_info.play_index].end_addr);
    }
}
void find_new_play_index()
{
    if(rtc_play_info.write_index == 0)
    {
        for(int i = 1; i < RECORD_INFO_MAX; i++)
        {
            if(rtc_play_info.info[i].available == true)
            {
                new_play_index = i;
            }
        }
    }
    else
    {
        for(int i = 0; i < rtc_play_info.write_index; i++)
        {
            if(rtc_play_info.info[i].available == true)
            {
                new_play_index = i;
            }
        }
    }
    #if RTC_RECORD_LOG_ON
    mprintf("最新录音id: %d", new_play_index);
    #endif
}


/* 播放xin录音 */ 
_XIF_ void rtc_play_new(void)
{
    if(rtc_play_info.info[new_play_index].available == true)
    {
        rtc_play_info.play_index = new_play_index;
        #if RTC_RECORD_LOG_ON
        mprintf("开始播放录音:%d\r\n", rtc_play_info.play_index);
        #endif
        rtc_play_local_flag = false;
        rtc_play_local_type_speex = false;
        start_play(rtc_play_info.info[rtc_play_info.play_index].start_addr, 
            rtc_play_info.info[rtc_play_info.play_index].end_addr);
    }
    else
    {
        #if RTC_RECORD_LOG_ON
        mprintf("未找到录音\r\n");
        #endif
    }
}

//播放内置speex压缩后的提示音
_XIF_ void rtc_play_local_voice_by_id(uint16_t id)
{
    uint16_t local_play_id;
    uint32_t flash_addr_start;
    uint32_t flash_addr_end;
    partition_table_t * partition_table = get_partition_table();
    uint32_t offset = partition_table->voice_offset;
    uint16_t toatl_local_play_number = 0;
    post_read_flash((char *)&toatl_local_play_number, offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    for(int i = 0; i < toatl_local_play_number; i++)
    {
        post_read_flash((char *)&local_play_id, offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        if(local_play_id == id)
        {
            post_read_flash((char *)&flash_addr_start, offset, sizeof(uint32_t));
            flash_addr_start += partition_table->voice_offset;
            offset += sizeof(uint32_t);
            post_read_flash(&flash_addr_end, offset, sizeof(uint32_t));
            flash_addr_end += flash_addr_start;
            flash_addr_start += 12;
            break;
        }
        else
            offset += 2*sizeof(uint32_t);
    }
    #if RTC_RECORD_LOG_ON
    mprintf("开始播放提示音:%d",local_play_id);
    #endif
    rtc_play_local_flag = true;
    if((id == 128) || (id == 141)||(id == 196))
    {
        //power_amplifier_off();
        rtc_play_local_type_speex = false;
        temp_play_loacl_pcm_counter = 0;
    } 
    else
    {
        //power_amplifier_on();
        rtc_play_local_type_speex = true;
    }
        
    start_play(flash_addr_start, flash_addr_end);
}

_XIF_ void rtc_flash_play_task()
{
    int8_t speex_encode_buf[WRITE_FLASH_SPEEX_SIZE] = {0};
    int8_t speex_tmp[UART_SPEEX_FRAME_LEN] = {0};
    uint8_t record_play_index;
    
    uint8_t time_out_counter = 0;
    while(1)
    {
        if(rtc_play_status != PLAY_START)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        speex_request_play_flag = true;
        if(rtc_play_local_flag == true)
        {
            #if RTC_RECORD_LOG_ON
            mprintf("提示音地址: %x, %x\r\n",record_play_start_addr, record_play_end_addr);
            #endif
            if (!rtc_play_local_type_speex)
            {
                record_play_start_addr += WRITE_FLASH_SPEEX_SIZE;
                record_play_end_addr -= WRITE_FLASH_SPEEX_SIZE;
            }
            
            while (record_play_start_addr < record_play_end_addr)  //一直拿去数据
            {
                if(rtc_play_local_type_speex)
                {
                    while (1)
                    {
                        if(speex_request_play_flag)
                        {  
                            if(RETURN_ERR == post_read_flash(speex_tmp, record_play_start_addr, UART_SPEEX_FRAME_LEN))
                                mprintf("flash读取失败...-%s %d\r\n",__FUNCTION__, __LINE__);
                            //mprintf(">");
                            if (xQueueSend(gSpeexdecodeDataQueue, speex_tmp, 10) == pdFALSE)
                                mprintf("----gSpeexdecodeDataQueue send err------\n");
                            vTaskDelay(pdMS_TO_TICKS(10));     //防丢数据
                            break;
                        }
                        else
                        {
                            vTaskDelay(pdMS_TO_TICKS(5));     //防丢数据
                        }
                    }
                    record_play_start_addr += UART_SPEEX_FRAME_LEN;     //指向下包数据读取地址
                }
                else
                {
                    temp_play_loacl_pcm_counter++;
                    if(RETURN_ERR == post_read_flash(speex_encode_buf, record_play_start_addr, WRITE_FLASH_SPEEX_SIZE))
                    {
                        mprintf("flash读取失败...-%s %d\r\n",__FUNCTION__, __LINE__);
                        continue;
                    }
                    //if (temp_play_loacl_pcm_counter > 8)
                    {
                        xStreamBufferSend(gSpeexPlayStreamBuffer, speex_encode_buf, WRITE_FLASH_SPEEX_SIZE, 1000); 
                        vTaskDelay(pdMS_TO_TICKS(5));     //防丢数据
                    }  
                    record_play_start_addr += WRITE_FLASH_SPEEX_SIZE;     //指向下包数据读取地址
                }
            }
        }
        else
        {
            #if RTC_RECORD_LOG_ON
            mprintf("录音地址: %x, %x\r\n",record_play_start_addr, record_play_end_addr);
            #endif
            record_play_start_addr += WRITE_FLASH_SPEEX_SIZE;
            record_play_end_addr -= WRITE_FLASH_SPEEX_SIZE*2;
            while (record_play_start_addr <= record_play_end_addr)  //一直拿去数据
            {
                if(RETURN_ERR == post_read_flash(speex_encode_buf, record_play_start_addr, WRITE_FLASH_SPEEX_SIZE))
                    mprintf("flash读取失败...-%s %d\r\n",__FUNCTION__, __LINE__);
                for(size_t j = 0; j < WRITE_FLASH_SPEEX_NUM; j++)
                {
                    while (1)
                    {
                        if(speex_request_play_flag)
                        {
                            memcpy(&speex_tmp, &speex_encode_buf[UART_SPEEX_FRAME_LEN * j], UART_SPEEX_FRAME_LEN);
                            if(speex_tmp[0] == 0x2a)    //有效数据,编码后有效音频数据长度为42字节
                            {
                                if (xQueueSend(gSpeexdecodeDataQueue, speex_tmp, 10) == pdFALSE)
                                    mprintf("----gSpeexdecodeDataQueue send err------\n");
                                vTaskDelay(pdMS_TO_TICKS(15));     //防丢数据
                            }
                            break;
                        }
                        else
                        {
                            vTaskDelay(pdMS_TO_TICKS(5));     //防丢数据
                        }
                        if(rtc_play_status == PLAY_STOP)
                        {
                            break;
                        }
                    }
                    if(rtc_play_status == PLAY_STOP)
                    {
                        break;
                    }
                }
                #if RTC_RECORD_LOG_ON
                mprintf("r_t=%x\r\n",record_play_start_addr);
                #endif
                record_play_start_addr += WRITE_FLASH_SPEEX_SIZE;     //指向下包数据读取地址
                if(rtc_play_status == PLAY_STOP)
                {
                    //xStreamBufferReset(gSpeexPlayStreamBuffer); 
                    break;
                }
            }
        }
        memset(speex_encode_buf, 0, sizeof(speex_encode_buf));
        while (1)
        { 
            if(rtc_play_local_flag == true)
            {
                if(xStreamBufferBytesAvailable(gSpeexPlayStreamBuffer) <= ORIGIN_PCM_FRAME_LEN)  //16k单通道录音，数据小于128，则播放完成
                {
                    //xStreamBufferSend(gSpeexPlayStreamBuffer, speex_encode_buf, 512, 1000); 
                    vTaskDelay(pdMS_TO_TICKS(15));     //延时等待播放结束
                    xStreamBufferReset(gSpeexPlayStreamBuffer);     //清除不足一帧未播放的音频数据
                    //xStreamBufferSend(gSpeexPlayStreamBuffer, speex_encode_buf, 512, 1000); 
                    //vTaskDelay(pdMS_TO_TICKS(15));
                    //xStreamBufferReset(gSpeexPlayStreamBuffer);  
                    #if RTC_RECORD_LOG_ON
                    mprintf("播放结束\r\n");
                    #endif
                    break;
                }

            }
            else 
            { 
                if(xStreamBufferBytesAvailable(gSpeexPlayStreamBuffer) <= ORIGIN_PCM_FRAME_LEN / 4)  //8k单通道录音，数据小于128，则播放完成
                {
                    xStreamBufferSend(gSpeexPlayStreamBuffer, speex_encode_buf, 512, 1000); 
                    vTaskDelay(pdMS_TO_TICKS(15)); 
                    xStreamBufferReset(gSpeexPlayStreamBuffer);     //清除不足一帧未播放的音频数据
                    xStreamBufferSend(gSpeexPlayStreamBuffer, speex_encode_buf, 512, 1000); 
                    #if RTC_RECORD_LOG_ON
                    mprintf("播放结束\r\n");
                    #endif
                    break;
                }
            }
            vTaskDelay(pdMS_TO_TICKS(5));     //延时等待播放结束
        }
        rtc_play_status = PLAY_END;
    }
}


/*----------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------*/

_XIF_ void vox_deal(void)
{
    extern short ci_agc_get_vox_val();
    static bool vox_flag = 0;
    if(vox_flag != ci_agc_get_vox_val())
    {
        vox_flag = !vox_flag;
        if(vox_flag == 0)
        {
            #if RTC_RECORD_LOG_ON
            mprintf("vox 关... \r\n");
            #endif
            #if MSG_COM_USE_UART_EN
            vmup_send_mcu_ack(0x11);
            #endif
            #if RTC_FLASH_VOX_FUNC_GPIO_DEFAULT
            gpio_set_output_level_single(RTC_FLASH_VOX_FUNC_GPIO_BASE, RTC_FLASH_VOX_FUNC_GPIO_PIN, 1); 
            #endif
        }
        else
        {
            #if RTC_RECORD_LOG_ON
            mprintf("vox 开..\r\n");
            #endif
            #if MSG_COM_USE_UART_EN
            vmup_send_mcu_ack(0x10);
            #endif
            #if RTC_FLASH_VOX_FUNC_GPIO_DEFAULT
            gpio_set_output_level_single(RTC_FLASH_VOX_FUNC_GPIO_BASE, RTC_FLASH_VOX_FUNC_GPIO_PIN, 0); 
            #endif
        }
    }
}

//录音数据读写flash
_XIF_ void rtc_msg_task(void)
{
    #if CUSTOMER_PROTOCOL_CLK_GPIO_DEFAULT  //引脚电平协议
    uint8_t rtc_protocol_com_msg_data = 0;
    rtc_protocol_msg_recv_queue = xQueueCreate(5, sizeof(uint8_t));
    #else
    #if MSG_COM_USE_UART_EN     //串口协议
    com_msg_data_t rtc_protocol_com_msg_data = {0};
    rtc_protocol_msg_recv_queue = xQueueCreate(5, sizeof(com_msg_data_t));
    #endif
    #endif
    if (rtc_protocol_msg_recv_queue == NULL)
    {
        mprintf("rtc_protocol_msg_recv_queue create fail\r\n");
    }

    #if VOX_1_AGC_0 || VOX_1_AGC_1      //vox检测到人声 输出高电平
    #if RTC_FLASH_VOX_FUNC_GPIO_DEFAULT
    scu_set_device_gate(RTC_FLASH_VOX_FUNC_GPIO_BASE, ENABLE);        // 开启PB时钟
    dpmu_set_io_reuse(RTC_FLASH_VOX_FUNC_GPIO, FIRST_FUNCTION);               // 设置引脚功能复用为GPIO
    dpmu_set_io_direction(RTC_FLASH_VOX_FUNC_GPIO, DPMU_IO_DIRECTION_OUTPUT); // 设置引脚功能为输出模式
    gpio_set_output_mode(RTC_FLASH_VOX_FUNC_GPIO_BASE, RTC_FLASH_VOX_FUNC_GPIO_PIN);                      // GPIO的pin脚配置成输出模式 
    gpio_set_output_level_single(RTC_FLASH_VOX_FUNC_GPIO_BASE, RTC_FLASH_VOX_FUNC_GPIO_PIN, 1); 
    #endif
    //init_test_info();
    init_vox_info();
    WebRtcAgc_vox_param_set(vox_info.th*10000 + 5000, vox_info.wake, vox_info.delay);
    #if RTC_RECORD_LOG_ON
    mprintf("WebRtcAgc_vox_param_set %d , %d\r\n", vox_info.th, vox_info.delay);
    #endif
    #endif
    uint32_t counter = 0;
    uint8_t cmd_id;
    while (1)
    {
        #if MSG_COM_USE_UART_EN || CUSTOMER_PROTOCOL_CLK_GPIO_DEFAULT
        if(pdPASS == xQueueReceive(rtc_protocol_msg_recv_queue, &rtc_protocol_com_msg_data, 0))
        {
            mprintf("rtc_protocol_msg_recv_queue msg_cmd %d %d\r\n", rtc_protocol_com_msg_data.msg_cmd, rtc_protocol_com_msg_data.msg_data[1]);
            cmd_id = rtc_protocol_com_msg_data.msg_data[1];
            
            switch (rtc_protocol_com_msg_data.msg_cmd)
            {
                case 1:
                    rtc_play_local_voice_by_id(cmd_id);
                    break;
                case 2:
                    switch (cmd_id)
                    {
                        case RTC_CMD_RECORD_START:     
                            rtc_record_start();
                            break;
                        case RTC_CMD_RECORD_END:     
                            rtc_record_end();
                            break;
                        case RTC_CMD_PLAY_NEXT:     
                            rtc_play_next();
                            break;
                        case RTC_CMD_PLAY_PREVIOUS: 
                            rtc_play_previous();
                            break;
                        case RTC_CMD_PLAY_NEW:     
                            rtc_play_new();
                            break;
                        case RTC_CMD_PLAY_STOP:     
                            rtc_play_stop();
                            break;
                        case RTC_CMD_RECORD_PAUSE:     
                            rtc_record_pause();
                            break;
                        case RTC_CMD_RECORD_RESUME:     
                            rtc_record_resume();
                            break;
                        case RTC_CMD_RECORD_MIC:     
                            rtc_flash_record_left = 0;
                            break;
                        case RTC_CMD_RECORD_DST:     
                            rtc_flash_record_left = 1;
                            break;
                        default:
                            break;
                    }
                    
                    break;
                case 3: //vox设置接口
                    write_vox_info(cmd_id);
                    break;
                case 4: //开关降噪设置
                    switch (cmd_id)
                    {
                        case 1:
                            mprintf("开降噪\r\n");
                            scu_iis_codec_ad2da_loop(0);
                        break;
                        case 2:
                            mprintf("关降噪\r\n");
                            scu_iis_codec_ad2da_loop(1);
                        break;
                        case 3:
                            mprintf("上行输入\r\n");
                        break;
                        case 4:
                            mprintf("下行输入\r\n");
                        break;
                        default:
                            break;
                    }
                break;
            }
        }
        #endif
        #if VOX_1_AGC_0 || VOX_1_AGC_1
        vox_deal();
        #endif
        vTaskDelay(pdMS_TO_TICKS(10));

    }
}


_XIF_ void rtc_record_task_init(void)
{
    //获取可用flash起始地址及大小
    
    partition_table_t * partition_table = get_partition_table();
/*     if (get_partition_table_crc(partition_table) !=partition_table->patitiontablechecksum)
    {
        CI_ASSERT(partition_table->patitiontablechecksum, "partition_table crc error\n");
    } */
    if ((partition_table->user_file_offset + partition_table->user_file_size) > RECORD_FLASH_START_ADDR)
    {
        CI_ASSERT(partition_table->user_file_offset + partition_table->user_file_size, "Firmware end addr is overflow 0x80000, Please check the Packaging tools\n");
    }
    
    flash_available_offset = RECORD_FLASH_START_ADDR;
    uint32_t tmp = partition_table->nv_data_offset - ERASE_4K*RECORD_HEAD_INFO_PAGE;
    CI_ASSERT(tmp > RECORD_HEAD_INFO_ADDR, "\n");
    head_cover_end_addr = RECORD_HEAD_INFO_ADDR + ERASE_4K*RECORD_HEAD_INFO_PAGE;
    //nv data记录该设备是否第一次使用
    if (CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_RTC_FLASH_EFUSE_INFO, sizeof(int8_t), NULL, NULL))
    {
        cinv_item_ret_t cinv_item_ret = cinv_item_init(NVDATA_ID_RTC_FLASH_EFUSE_INFO, sizeof(record_info_t),NULL);
        mprintf("RTC_FLASH_EFUSE_INFO cinv_item_read failed \r\n");
        vTaskDelay(pdMS_TO_TICKS(AUDIO_IN_BUFFER_NUM * 16));    //等待rtc_voice_tmp_buffer播报缓冲
        post_erase_flash(RECORD_HEAD_INFO_ADDR, ERASE_4K*RECORD_HEAD_INFO_PAGE);
        rtc_record_data_init(flash_available_offset);
        head_cover_addr = RECORD_HEAD_INFO_ADDR;
        updata_rtc_record_info(); 
        print_rtc_flash_record_play_info();
    }else
    {
        mprintf("RTC_FLASH_EFUSE_INFO cinv_item_read success \r\n");
        uint32_t total_record_number = ERASE_4K*RECORD_HEAD_INFO_PAGE/sizeof(rtc_flash_record_play_info_t);
        #if RTC_RECORD_LOG_ON
        mprintf("start find recnet record number %d\r\n", total_record_number);
        #endif
        for(int i = 0; i < total_record_number; i++)
        {
            post_read_flash(&rtc_record_info, RECORD_HEAD_INFO_ADDR+i*sizeof(rtc_flash_record_play_info_t), sizeof(rtc_flash_record_play_info_t));
            if(rtc_record_info.write_addr == 0xffffffff) //没有信息了
            {
                post_read_flash(&rtc_record_info, RECORD_HEAD_INFO_ADDR+(i-1)*sizeof(rtc_flash_record_play_info_t), sizeof(rtc_flash_record_play_info_t));
                head_cover_addr = RECORD_HEAD_INFO_ADDR + i*sizeof(rtc_flash_record_play_info_t);
                #if RTC_RECORD_LOG_ON
                mprintf("find recnet record number %d  %x\r\n", i, head_cover_addr);
                #endif
                break;
            }
            uint16_t crc = crc16_ccitt(0, &rtc_record_info, sizeof(rtc_flash_record_play_info_t)-2);
            if(rtc_record_info.crc != crc)
            {
                #if RTC_RECORD_LOG_ON
                mprintf("crc error ,find recnet record number %d  %x\r\n", i, head_cover_addr);
                #endif
                post_erase_flash(RECORD_HEAD_INFO_ADDR, ERASE_4K*RECORD_HEAD_INFO_PAGE);
                if(i == 0)
                {
                    rtc_record_data_init(flash_available_offset);
                }
                else
                    post_read_flash(&rtc_record_info, RECORD_HEAD_INFO_ADDR+(i-1)*sizeof(rtc_flash_record_play_info_t), sizeof(rtc_flash_record_play_info_t));
                head_cover_addr = RECORD_HEAD_INFO_ADDR;
                updata_rtc_record_info(); 
                break;
            }
            else
            {
                //mprintf("crc ok %d\r\n", i);
                //print_rtc_flash_record_play_info();
            }
        }
        check_rtc_record_data();
        //print_rtc_flash_record_play_info();
    }
    memcpy(&rtc_play_info, &rtc_record_info, sizeof(rtc_flash_record_play_info_t));
    find_new_play_index();
    
    //读写flash与检测按键任务
    xTaskCreate(rtc_msg_task,"rtc_msg_task", 512 , NULL, 4, NULL);
    xTaskCreate(rtc_flash_record_task,"rtc_flash_record_task",  512 + (WRITE_FLASH_SPEEX_NUM*UART_SPEEX_FRAME_LEN) / 4 , NULL, 4, NULL);
    xTaskCreate(rtc_flash_play_task,"rtc_flash_play_task",  512, NULL, 4, NULL);
    #if TEST_RECORD
    xTaskCreate(rtc_test_task,"rtc_test_task", 256, NULL, 4, NULL);
    #endif
}

#define NVDATA_ID_VOX_INFO 0x90000001 
cinv_item_ret_t init_vox_info()
{
	uint16_t real_len = 0;
	cinv_item_ret_t ret;
 	ret = cinv_item_read(NVDATA_ID_VOX_INFO, sizeof(vox_info_t), &vox_info, &real_len);
    if(CINV_OPER_SUCCESS != ret)
    {
        vox_info.th = 0;
		vox_info.wake = 3;
		vox_info.delay = 1;
        ret = cinv_item_init(NVDATA_ID_VOX_INFO, sizeof(vox_info_t), &vox_info);
    }
	return ret;
}

cinv_item_ret_t write_vox_info(uint8_t data)
{
    switch(data)
    {
        case 0xE0 ... 0xEF: //设置vox启动延迟
            vox_info.wake = data - 0xE0;
            break;

        case 0xF0 ... 0xF8: //设置vox阈值
            vox_info.th = 0xF8 - data;
            break;

        case 0xF9 ... 0xFD: //设置休眠延迟
            vox_info.delay = (data - 0xF9)/2 + 1;
            break;

        default:
            return CINV_OPER_FAILED;
    }
    WebRtcAgc_vox_param_set(vox_info.th*10000 + 5000, vox_info.wake, vox_info.delay);
    mprintf("WebRtcAgc_vox_param_set %d , %d  %d\r\n", vox_info.th, vox_info.wake, vox_info.delay);
	return cinv_item_write(NVDATA_ID_VOX_INFO, sizeof(vox_info_t), &vox_info);
}

#define NVDATA_ID_TEST_INFO 0x90000002 

cinv_item_ret_t init_test_info()
{
	uint16_t real_len = 0;
	cinv_item_ret_t ret;
 	ret = cinv_item_read(NVDATA_ID_TEST_INFO, sizeof(test_info_t), &test_info, &real_len);
    if(CINV_OPER_SUCCESS != ret)
    {
        test_info.w_full = 0;
		test_info.reset = 0;
        ret = cinv_item_init(NVDATA_ID_TEST_INFO, sizeof(test_info_t), &test_info);
    }
    else
    {
        test_info.reset++;
        cinv_item_write(NVDATA_ID_TEST_INFO, sizeof(test_info_t), &test_info);
    }
    mprintf("test_info w_full %d,  reset %d.......\r\n", test_info.w_full, test_info.reset);
	return ret;
}

cinv_item_ret_t write_test_info()
{
    uint16_t real_len = 0;
    cinv_item_write(NVDATA_ID_TEST_INFO, sizeof(test_info_t), &test_info);
    cinv_item_read(NVDATA_ID_TEST_INFO, sizeof(test_info_t), &test_info, &real_len);
    mprintf("write_test_info w_full %d,  reset %d.......\r\n", test_info.w_full, test_info.reset);
	return CINV_OPER_SUCCESS;
}



_XIF_ void rtc_test_task(void)
{
    vTaskDelay(pdMS_TO_TICKS(5000));
    com_msg_data_t send_packet;
    send_packet.msg_type = 0;
    send_packet.msg_cmd = 2;
    while(1)
    {
        //rtc_record_start();
        send_packet.msg_data[1] = 1;
        if (xQueueSend(rtc_protocol_msg_recv_queue, &send_packet, 0) == pdFALSE)
            mprintf("----rtc_protocol_msg_recv_queue send err------\n");
        vTaskDelay(pdMS_TO_TICKS(25000));
        send_packet.msg_data[1] = 2;
        if (xQueueSend(rtc_protocol_msg_recv_queue, &send_packet, 0) == pdFALSE)
            mprintf("----rtc_protocol_msg_recv_queue send err------\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
        //write_test_info();

    }
}