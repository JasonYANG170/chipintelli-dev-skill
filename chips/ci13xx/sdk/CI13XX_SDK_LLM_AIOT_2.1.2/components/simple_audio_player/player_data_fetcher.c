#include "user_config.h"
#if	SIMPLE_AUDIO_PLAYER_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "player_data_fetcher.h"
#include "ci_log.h"
#include "flash_rw_process.h"
#include "status_share.h"
#include "cias_aiot_protocol.h"
#include "cias_network_msg_protocol.h"
extern CiasAiotRunParamTypedef gCiasAiotRunParam;
extern CiasAiotFuncParamTypedef gCiasAiotFuncParam;
typedef struct
{
    src_data_info_t src_data_info;
    StreamBufferHandle_t data_buffer;
    int32_t data_offset;
    int32_t eos;            // End of data stream.
    int32_t request_wait_count;
    TaskHandle_t task_handle;
}data_fetch_task_info_t;
extern SemaphoreHandle_t fetch_sem;
static void task_data_fetch(void *pvParameters);

data_fetch_task_info_t data_fetch_task_info = {0};

void pdf_cloud_pre_init(uint32_t buffer_size)
{
    //云端播放需要先初始化共用流缓存，在播放器中初始化可能会遇到wifi端下发MP3头后，流缓存初始化导致数据丢失
    pdf_deinit();
    if(data_fetch_task_info.data_buffer == NULL)
    data_fetch_task_info.data_buffer = xStreamBufferCreate(buffer_size, 1);
}

void pdf_init(src_data_info_t *src_data_info)
{
    // if(gCiasAiotRunParam.cloud_play_state != CLOUD_PLAY_START)
    pdf_deinit();

    // 将传入的参数赋值给data_fetch_task_info结构体
    data_fetch_task_info.src_data_info = *src_data_info;

    if (src_data_info->data_size <= 0)
    {
        data_fetch_task_info.src_data_info.data_size = PDF_MAX_DATA_SIZE;
    }
    // 创建一个流缓冲区，用于存储数据
    // if(gCiasAiotRunParam.cloud_play_state != CLOUD_PLAY_START)
    if(data_fetch_task_info.data_buffer == NULL)
    data_fetch_task_info.data_buffer = xStreamBufferCreate(src_data_info->buffer_size, 1);
    // 设置数据偏移量
    data_fetch_task_info.data_offset = src_data_info->data_offset;
    data_fetch_task_info.eos = 0;
    if (data_fetch_task_info.src_data_info.src_type == SAP_DATA_SRC_STREAM)
    {
        gCiasAiotRunParam.request_play_data_flag = true;
        gCiasAiotRunParam.cloud_play_state = CLOUD_PLAY_START_DATA_WITING;
    }

    #if NET_AUDIO_PLAY_BY_PCM && AUDIO_COMPRESS_RECORD_DISABLE
    xTaskCreate(task_data_fetch, "data_fetch", 256, src_data_info, 5, &data_fetch_task_info.task_handle);
    #else
    xTaskCreate(task_data_fetch, "data_fetch", 256, src_data_info, 4, &data_fetch_task_info.task_handle);
    #endif
}

void pdf_deinit(void)
{
    while(data_fetch_task_info.task_handle != NULL)
    {
        //强制结束上一个任务
        taskENTER_CRITICAL();
        data_fetch_task_info.src_data_info.data_size = 0;
        data_fetch_task_info.data_offset = 0;
        xStreamBufferReset(data_fetch_task_info.data_buffer);
        taskEXIT_CRITICAL();
        vTaskDelay(1);
    }
}

void play_start_wait_data(void)
{
    uint16_t timeout_cnt =0;
    while((xStreamBufferBytesAvailable(data_fetch_task_info.data_buffer) <= REQUEST_ONE_FRAME_SEZIE) 
    && gCiasAiotRunParam.cloud_play_state == CLOUD_PLAY_START_DATA_WITING
    && timeout_cnt<100)//等待收到2帧数据，前面pdf_fetch_data已经获取了一帧了
    {
        timeout_cnt++;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return;
}

void task_data_fetch(void *pvParameters)
{
    bool ret = false;
    // 设置结束标志位，本地使用有效left_size长度、云端使用gCiasAiotRunParam.request_play_data_flag结束请求任务
    data_fetch_task_info.eos = 0;
    // 计算剩余数据大小
    int left_size = data_fetch_task_info.src_data_info.data_size - data_fetch_task_info.data_offset;
    // 当剩余数据大小大于0且流缓冲区有可用空间时，循环读取数据
    while((left_size > 0) || (xStreamBufferBytesAvailable(data_fetch_task_info.data_buffer) > 0) )
    {
        // 计算流缓冲区可用空间大小
        size_t space_size = xStreamBufferSpacesAvailable(data_fetch_task_info.data_buffer);
        // 如果流缓冲区有可用空间
        if ((space_size > data_fetch_task_info.src_data_info.request_block_size) && ((left_size > 0)))
        {
            if (data_fetch_task_info.src_data_info.src_type == SAP_DATA_SRC_FLASH)
            {
                // 定义一个临时缓冲区
                uint8_t tmp_buffer[256];
                // 计算读取大小
                size_t read_size = (space_size > sizeof(tmp_buffer)) ? sizeof(tmp_buffer) : space_size;
                read_size = (read_size > left_size) ? left_size : read_size;
                // 从flash中读取数据
                post_read_flash(tmp_buffer, data_fetch_task_info.src_data_info.data_addr + data_fetch_task_info.data_offset, read_size);
                xSemaphoreTake(fetch_sem, pdMS_TO_TICKS(1000));
                // 将数据发送到流缓冲区
                xStreamBufferSend(data_fetch_task_info.data_buffer, tmp_buffer, read_size, portMAX_DELAY);
                xSemaphoreGive( fetch_sem );//给出互斥量
                data_fetch_task_info.data_offset += read_size;
            }
            else
            {
                size_t read_size = (space_size >= data_fetch_task_info.src_data_info.request_block_size) ? data_fetch_task_info.src_data_info.request_block_size : space_size;
                // mprintf("read_size = %d,request_play_data_flag = %d,\r\n",read_size,gCiasAiotRunParam.request_play_data_flag);
                if (read_size > 0 && data_fetch_task_info.src_data_info.data_request_callback)
                {
                    if(gCiasAiotRunParam.play_cloud_data_flag && xStreamBufferBytesAvailable(data_fetch_task_info.data_buffer) == 0)
                    {
                        if (gCiasAiotRunParam.play_cloud_end_flag)
                        {
                            if (gCiasAiotRunParam.request_play_try_count >= 5)
                            {
                                ret = true;
                            }
                        }
                        if (gCiasAiotRunParam.request_play_try_count >= 15)
                        {
                            ret = true;
                        }
                        if (ret)
                        {
                            mprintf("play stop sync state to wifi ....\r\n");
                            ret = false;
                            ciss_set(CI_SS_PLAY_STATE, CI_SS_PLAY_STATE_IDLE); // 设置播放结束
                            if (!gCiasAiotFuncParam.upload_play_full_duplex)
                            {
                                ciss_set(CI_SS_VOX_WORK_STATE, 1); // 开启vox vad计算
                            }
                            gCiasAiotRunParam.request_play_try_count = 0;
                            gCiasAiotRunParam.play_cloud_data_flag = false;
                            gCiasAiotRunParam.request_play_data_flag = false;
                            if (gCiasAiotRunParam.play_cloud_end_flag)
                            {
                                gCiasAiotRunParam.cloud_play_state = CLOUD_PLAY_END;
                            }
                            else
                            {
                                gCiasAiotRunParam.cloud_play_state = CLOUD_PLAYING_DATA_WITING_TIMEOUT;
                            }
                            gCiasAiotRunParam.stop_collect_pcm_flag = false;
                            int try_count = 30;
                            while(try_count--)   //等待播放状态同步完成
                            {
                                if(sap_get_state() == SAP_STATE_IDLE)
                                {
                                    break;
                                }
                                else
                                {
                                    mprintf("===wait audio play over\r\n");
                                    vTaskDelay(pdMS_TO_TICKS(10));
                                }
                            }
                            cias_send_cmd(PLAY_TTS_END, DEF_FILL);
                        }
                    }
                    // taskENTER_CRITICAL();
                    if (data_fetch_task_info.request_wait_count == 0)
                    {
                        if(gCiasAiotRunParam.request_play_data_flag && !gCiasAiotRunParam.play_cloud_end_flag)
                        {
                            // 发送数据请求
                            data_fetch_task_info.src_data_info.data_request_callback(read_size);
                        }
                        #if NET_AUDIO_PLAY_BY_PCM 
                        data_fetch_task_info.request_wait_count = 5;
                        #else
                        data_fetch_task_info.request_wait_count = 10;
                        #endif
                        gCiasAiotRunParam.request_play_try_count++;
                    }
                    else
                    {
                        // 如果请求等待次数大于0，则延迟32个tick
                        data_fetch_task_info.request_wait_count--;
                    }
                    // taskEXIT_CRITICAL();
                    if (data_fetch_task_info.request_wait_count != 0)
                    {
                        vTaskDelay(pdMS_TO_TICKS(20));
                    }
                }

                if(!gCiasAiotRunParam.request_play_data_flag) 
                {
                    /*播放结束释放任务*/
                    data_fetch_task_info.src_data_info.data_size = 0;
                    data_fetch_task_info.data_offset = 0;
                }

            }

            // 更新剩余数据大小
            left_size = data_fetch_task_info.src_data_info.data_size - data_fetch_task_info.data_offset;
            // ci_logdebug(LOG_AUDIO_PLAY, "pdf left %d bytes\n", left_size);
        }
        else
        {
            // 如果流缓冲区没有可用空间，则延迟2个tick
            vTaskDelay(2);
        }
    }
    //mprintf("==task_data_fetch vTaskDelete\r\n");
    // 设置结束标志位
    data_fetch_task_info.eos = 1;
    // 删除流缓冲区
    taskENTER_CRITICAL();
    vStreamBufferDelete(data_fetch_task_info.data_buffer);
    // 将流缓冲区指针置为NULL
    data_fetch_task_info.data_buffer = NULL;
    taskEXIT_CRITICAL();
    // 删除任务
    data_fetch_task_info.task_handle = NULL;
    vTaskDelete(NULL);
}

int32_t pdf_push_data(uint8_t *data, uint32_t data_size, TickType_t xTicksToWait)
{
    //这里判断是在初始化流缓存以后type为本地，防止云端数据放入本地播放缓存，导致尾音丢失
    if (data_fetch_task_info.src_data_info.src_type == SAP_DATA_SRC_FLASH)
    return 0;

    xSemaphoreTake(fetch_sem, pdMS_TO_TICKS(1000)); 
    size_t write_bytes = xStreamBufferSend(data_fetch_task_info.data_buffer, data, data_size, xTicksToWait);
    xSemaphoreGive( fetch_sem );//给出互斥量
    // for (int i = 0; i < 10; i++)
    // {
    //        mprintf("zt data_size =%d\r\n",data[i]);
    // }   
    if (write_bytes > 0)
    {
        data_fetch_task_info.data_offset += write_bytes;
        data_fetch_task_info.request_wait_count = 0;
    }
    return write_bytes;
}
// uint16_t pdf_get_vaild_size(void)
// {
//     return xStreamBufferBytesAvailable(data_fetch_task_info.data_buffer);
// }
int32_t pdf_fetch_data(void *buffer, uint32_t buffer_size, TickType_t xTicksToWait )
{
    // 定义变量bytes_read，用于存储读取的字节数
    int32_t bytes_read = 0;
    do 
    {
        //请求任务删除
        if (data_fetch_task_info.eos)
        {
            bytes_read = -1;
            break;
        }
        taskENTER_CRITICAL();
        if (data_fetch_task_info.data_buffer)
        {
            // 从data_buffer中读取数据到buffer中，读取的字节数存储在bytes_read中
            size_t available = xStreamBufferBytesAvailable(data_fetch_task_info.data_buffer);
            if (available >= buffer_size)
            {     
                bytes_read = xStreamBufferReceive(data_fetch_task_info.data_buffer, buffer, buffer_size, 0);
                taskEXIT_CRITICAL();
            }
            else
            {
                xTicksToWait = xTicksToWait > 0 ? xTicksToWait - 1 : 0;
                if (xTicksToWait == 0 && available > 0)
                {
                    bytes_read = xStreamBufferReceive(data_fetch_task_info.data_buffer, buffer, buffer_size, 0);
                    taskEXIT_CRITICAL();
                }
                else
                {
                    taskEXIT_CRITICAL();
                    vTaskDelay(1);
                }
            }
        }
        else
        {
            taskEXIT_CRITICAL();
        }
    }while(bytes_read == 0 && xTicksToWait > 0);

    return bytes_read;
}

int32_t pdf_set_total_data_size(uint32_t data_size)
{
    data_fetch_task_info.src_data_info.data_size = data_size;
    return 0;
}

int32_t pdf_set_eos()
{
    data_fetch_task_info.src_data_info.data_size = data_fetch_task_info.src_data_info.data_offset;
    return 0;
}
#endif
