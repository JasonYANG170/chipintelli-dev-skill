#include <string.h>
#include "simple_mp3_player.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "romlib_api.h"
#include "codec_manager.h"
#include "ci_log_config.h"
#include "mp3dec.h"
#include "ci_log.h"
#include "board.h"
#include "flash_rw_process.h"


#define SMP_SOURCE_DATA_BUF_SIZE        1024
#define SMP_MSG_QUEUE_LENGHT            20
#define SMP_PCM_BUF_COUNT               4


typedef enum {
    SMP_STATE_START,
    SMP_STATE_PLAY,
    SMP_STATE_STOP,
    SMP_STATE_IDLE,
}smp_status_t;

typedef struct {
    smp_msg_id_t msg_id;
    uint32_t data_addr;
    SMP_PLAY_END_CALLBACK play_end_callback;
}smp_msg_t;

#pragma push
#pragma pack(1)
typedef struct
{
    char ID3[3];               //"ID3"
    char ver;                  //3
    char revision;             //0
    char flag;                 //0
    uint32_t total_frame_size; //标签帧大小
    char frame_ID[4];          //"PRIV"
    uint32_t frame_size;       //PRIV大小
    uint16_t frame_flag;       //0
    char CI[2];                //"CI"
    uint32_t file_size;        //文件大小
    uint32_t pcm_size;         //PCM大小
}ci_mp3_header_t;
#pragma pop

typedef struct {
    QueueHandle_t msg_queue;
    smp_status_t state;
    uint32_t data_addr;
    SMP_PLAY_END_CALLBACK play_end_callback;
    HMP3Decoder decoder_handle;
    uint32_t sample_rate;
    uint32_t output_samples;
    uint32_t pcm_buffer_size;
    short * pcm_buffer;
    uint32_t source_file_bytes_left;
    uint32_t source_buf_bytes_left;
    uint32_t decode_frame_count;
    uint32_t source_frame_size;
    uint32_t frame_pcm_size;
    uint32_t total_pcm_size;
    uint8_t source_data_buf[SMP_SOURCE_DATA_BUF_SIZE];
    uint8_t channels;
}smp_info_t;

smp_info_t smp_info = {
    .msg_queue = NULL,
    .state = SMP_STATE_IDLE,
    .pcm_buffer = NULL,
    .source_file_bytes_left = 0,
    .source_buf_bytes_left = 0,
};


static void task_simple_mp3_player(void *pvParameters);
static void smp_send_msg(smp_msg_t * msg, BaseType_t *xHigherPriorityTaskWoken);

/**
 * @brief Initialize the simple mp3 player module.
 * 
 * @return int 1: successed; not 1: failed.
 */
int smp_init(void)
{
    return (int)xTaskCreate(task_simple_mp3_player, "simple-mp3-player", 360, 0, 4, NULL);
}

/**
 * @brief Start playing.
 * 
 * @param data_addr uint32_t It's used to specifiy the address of the mp3 data.
 * @param play_end_callback A pointer to a function that will be called when play to end.
 * @return int 1: successed; not 1: failed.
t */
int smp_play(uint32_t data_addr, void* play_end_callback)
{
    smp_msg_t msg;
    msg.msg_id = SMP_MSG_START_PLAY;
    msg.data_addr = data_addr;
    msg.play_end_callback = play_end_callback;
    smp_send_msg(&msg, NULL);
}

// 停止接口
void smp_stop()
{
    smp_msg_t msg;
    msg.msg_id = SMP_MSG_STOP_PLAY;
    while(smp_info.decode_frame_count < 8 && smp_info.state != SMP_STATE_IDLE)
    {
        vTaskDelay(1);
    }
    smp_send_msg(&msg, NULL);
}


/**
 * @brief 调节播放音量
 * 
 * @param gain 音量(0--100)
 */
void audio_play_set_vol_gain(int32_t gain)
{
    cm_set_codec_dac_gain(PLAY_CODEC_ID, 0, gain);
}

/**
 * @brief pa、da控制，可选择是否控制功放
 * 
 * @param cmd pa使能或失能
 * 
 */
void audio_play_hw_pa_da_ctl(FunctionalState cmd,bool is_control_pa)
{
    //PA的操作需要在DAC关闭的情况下进行，不然会影响采音
    if(ENABLE == cmd)
    {
        icodec_start(CODEC_OUTPUT);
        if(is_control_pa)
        {
            power_amplifier_on();
        }
    }
    else
    {
        if(is_control_pa)
        {
            power_amplifier_off();
        }
        icodec_stop(CODEC_OUTPUT);
    }
}

static void smp_send_msg(smp_msg_t * msg, BaseType_t *xHigherPriorityTaskWoken)
{
    if(0 != check_curr_trap())
    {
        xQueueSendFromISR(smp_info.msg_queue, msg, xHigherPriorityTaskWoken);
    }
    else
    {
        xQueueSend(smp_info.msg_queue, msg, portMAX_DELAY);
    }
}

/**
 * @brief Inner start playing function.
 * 
 * @param msg A pointer to a smp_msg_t structure used to input data address and callback function.
 * @return int 0:successed, not 0:failed.
 */
static int smp_start_play_inner(smp_msg_t *msg)
{
    int ret = 0;
    short *tmp_buf;

    smp_info.data_addr = msg->data_addr;
    smp_info.play_end_callback = msg->play_end_callback;
    smp_info.decoder_handle = MP3InitDecoder();

    do 
    {
        // malloc a buffer to recevie output data from decoder.
        tmp_buf = pvPortMalloc(2048);
        if (!tmp_buf)
        {
            ci_logerr(CI_LOG_ERROR,"not enough memory\n");
            ret = -1;
            break;
        }
        post_read_flash(smp_info.source_data_buf, smp_info.data_addr, SMP_SOURCE_DATA_BUF_SIZE);
        smp_info.data_addr += SMP_SOURCE_DATA_BUF_SIZE;
        smp_info.source_buf_bytes_left = SMP_SOURCE_DATA_BUF_SIZE;
        smp_info.source_file_bytes_left = ((ci_mp3_header_t*)smp_info.source_data_buf)->file_size - SMP_SOURCE_DATA_BUF_SIZE;
        smp_info.total_pcm_size = ((ci_mp3_header_t*)smp_info.source_data_buf)->pcm_size*sizeof(short);

        int32_t mp3_sync_offset;
        mp3_sync_offset = MP3FindSyncWord(smp_info.source_data_buf, SMP_SOURCE_DATA_BUF_SIZE);   // Find the head flag of the frist frame.
        if (mp3_sync_offset == -1)
        {
            ret = -2;
            break;
        }
        int bytes_left = SMP_SOURCE_DATA_BUF_SIZE - mp3_sync_offset;
        // Decode a frame to get some information about the audio, such as sample rate, channels, output samples per frame.
        uint8_t * in_data_ptr = &smp_info.source_data_buf[mp3_sync_offset];
        int32_t err = MP3Decode(smp_info.decoder_handle, &in_data_ptr, &bytes_left, tmp_buf, 0);  // Decode a frame. 
        MP3FrameInfo mp3FrameInfo;
        MP3GetLastFrameInfo(smp_info.decoder_handle, &mp3FrameInfo);       // Get information about the frame that just decoded.
        if(ERR_MP3_NONE != err )
        {   
            ci_logwarn(LOG_AUDIO_PLAY,"mp3_decorde err %d,bad frame!\n",err);
            ret = -3;
            break;
        }
        smp_info.source_buf_bytes_left = bytes_left;
        smp_info.output_samples = mp3FrameInfo.outputSamps;
        smp_info.decode_frame_count = 1;
        smp_info.source_frame_size = mp3FrameInfo.bitrate*mp3FrameInfo.outputSamps/mp3FrameInfo.samprate/8;
        smp_info.frame_pcm_size = smp_info.output_samples*sizeof(short);

        cm_pcm_buffer_info_t pcm_buffer_info;
        pcm_buffer_info.play_buffer_info.block_num = 2;
        pcm_buffer_info.play_buffer_info.buffer_num = SMP_PCM_BUF_COUNT;
        pcm_buffer_info.play_buffer_info.buffer_size = smp_info.frame_pcm_size;
        pcm_buffer_info.play_buffer_info.block_size = pcm_buffer_info.play_buffer_info.buffer_size/pcm_buffer_info.play_buffer_info.block_num;
        int pcm_buffer_total_size = pcm_buffer_info.play_buffer_info.buffer_size*pcm_buffer_info.play_buffer_info.buffer_num;
        if(smp_info.pcm_buffer_size < pcm_buffer_total_size)
        {
            if (smp_info.pcm_buffer != NULL)
            {
                vPortFree(smp_info.pcm_buffer);
                smp_info.pcm_buffer = NULL;
            }
        }
        
        if (smp_info.pcm_buffer == NULL)
        {
            smp_info.pcm_buffer = pvPortMalloc(pcm_buffer_total_size);
            if (!smp_info.pcm_buffer)
            {
                ci_logerr(CI_LOG_ERROR,"not enough memory\n");
                ret = -4;
                break;
            }
            smp_info.pcm_buffer_size = pcm_buffer_total_size;
        }
        pcm_buffer_info.play_buffer_info.pcm_buffer = smp_info.pcm_buffer;
        cm_config_pcm_buffer(PLAY_CODEC_ID, CODEC_OUTPUT, &pcm_buffer_info);
        uint32_t ret_buf;
        //cm_get_pcm_buffer(PLAY_CODEC_ID,&ret_buf,portMAX_DELAY);
        //memcpy((void*)ret_buf, tmp_buf, mp3FrameInfo.outputSamps*sizeof(short)*mp3FrameInfo.nChans);
        //cm_write_codec(PLAY_CODEC_ID, (void*)ret_buf,portMAX_DELAY);
        static uint32_t pre_sample_rate = 0;
        static uint32_t pre_nChans = 0;
        if((smp_info.sample_rate != mp3FrameInfo.samprate) || (smp_info.channels != mp3FrameInfo.nChans))
        {
            smp_info.sample_rate = mp3FrameInfo.samprate;
            smp_info.channels = mp3FrameInfo.nChans;

            cm_sound_info_t sound_info;
            sound_info.sample_rate = mp3FrameInfo.samprate;
            sound_info.sample_depth = IIS_DW_16BIT;
            sound_info.channel_flag = (mp3FrameInfo.nChans == 2) ? 3:1;
            cm_config_codec(PLAY_CODEC_ID, CODEC_OUTPUT, &sound_info);
        }
    }while(0);
    if (tmp_buf)
    {
        vPortFree(tmp_buf);
    }
    ci_loginfo(LOG_AUDIO_PLAY, "Play start\r\n");
    return ret;
}

static void wait_codec_play_finish()
{
    while(cm_get_codec_empty_buffer_number(PLAY_CODEC_ID, CODEC_OUTPUT) < SMP_PCM_BUF_COUNT)
    {
        vTaskDelay(1);
    }
}

/**
 * @brief Decode one frame.
 * 
 * @return int  0: Decode failed; 
 *              1: Decode successed,and the PCM buffer of codec manager is full, you can do some delay before the next call to this function to free up CPU for other tasks; 
 *              2: Decode successed,and the PCM buffer of codec manager is not full, you'd better call this function again immediately.
 *              3: End of file, there's no source data any more, end of play.
 */
static int smp_decode_one_frame(void)
{
    int ret = 0;
    uint32_t pcm_buf;
    uint32_t data_offset = SMP_SOURCE_DATA_BUF_SIZE - smp_info.source_buf_bytes_left;
    int bytes_left = smp_info.source_buf_bytes_left;

    if (smp_info.source_frame_size > smp_info.source_buf_bytes_left)
    {
        if (smp_info.source_file_bytes_left == 0)
        {
            wait_codec_play_finish();
            ret = 3;
            return ret;
        }
        else
        {
            memcpy(smp_info.source_data_buf, &smp_info.source_data_buf[data_offset], bytes_left);
            uint32_t read_size = data_offset < smp_info.source_file_bytes_left ? data_offset : smp_info.source_file_bytes_left;
            post_read_flash(smp_info.source_data_buf+bytes_left, smp_info.data_addr, data_offset);
            smp_info.data_addr += read_size;
            smp_info.source_file_bytes_left -= read_size;
            smp_info.source_buf_bytes_left = SMP_SOURCE_DATA_BUF_SIZE;
            bytes_left = SMP_SOURCE_DATA_BUF_SIZE;
            data_offset = 0;
        }
    }
    int32_t mp3_sync_offset;
    mp3_sync_offset = MP3FindSyncWord(&smp_info.source_data_buf[data_offset], bytes_left);   // Find the head flag of the frist frame.
    if (mp3_sync_offset == -1)
    {
        smp_info.source_buf_bytes_left = 2;
        ret = 2;
        return ret;
    }
    uint8_t * in_data_ptr = &smp_info.source_data_buf[data_offset];
    if(smp_info.decode_frame_count > 1)
    {
        cm_get_pcm_buffer(PLAY_CODEC_ID, &pcm_buf, portMAX_DELAY);
    }
    else
    {
        pcm_buf = (uint32_t)((void*)pvPortMalloc(smp_info.frame_pcm_size));
        if(pcm_buf == NULL)
        {
            mprintf("not enough memory\n");
        }
    }
    int32_t err = MP3Decode(smp_info.decoder_handle, &in_data_ptr, &bytes_left, (void*)pcm_buf, 0);  // Decode a frame. 
    if (ERR_MP3_NONE == err)
    {
        if(smp_info.decode_frame_count > 1)
        {
            cm_write_codec(PLAY_CODEC_ID, (void*)pcm_buf,portMAX_DELAY);
        }
        else
        {
            vPortFree((void*)pcm_buf);
        }
        
        int bytes_used = smp_info.source_buf_bytes_left - bytes_left;
        smp_info.source_buf_bytes_left = bytes_left;
        smp_info.decode_frame_count += 1;
        if (smp_info.decode_frame_count == 3)
        {
            cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
            cm_set_codec_mute(PLAY_CODEC_ID, CODEC_OUTPUT, 3, DISABLE);
        }
        else if (smp_info.total_pcm_size <= (smp_info.decode_frame_count)*smp_info.frame_pcm_size)
        {
            wait_codec_play_finish();
            ret = 3;
            return ret;
        }
        ret = 1;
    }
    else
    {
        cm_release_pcm_buffer(PLAY_CODEC_ID, CODEC_OUTPUT, (void*)pcm_buf);
        wait_codec_play_finish();
        ci_logwarn(LOG_AUDIO_PLAY,"mp3_decorde err %d,bad frame!\n",err);
    }
    return ret;
}

static int smp_stop_play_inner(int32_t play_cb_state)
{
    int ret = 0;

    cm_set_codec_mute(PLAY_CODEC_ID, CODEC_OUTPUT, 3, ENABLE);
    cm_stop_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
    MP3FreeDecoder(smp_info.decoder_handle);
    smp_info.decoder_handle = NULL;
    ci_loginfo(LOG_AUDIO_PLAY, "Play end\r\n");
    if (smp_info.play_end_callback)
    {
        smp_info.play_end_callback(play_cb_state);
    }
    return ret;
}

/**
 * @brief Task function of player
 * 
 * @param pvParameters Task parameter.
 */
static void task_simple_mp3_player(void *pvParameters)
{
    TickType_t wait_time = portMAX_DELAY;
    smp_info.msg_queue = xQueueCreate(SMP_MSG_QUEUE_LENGHT, sizeof(smp_msg_t));
    while(1)
    {
        smp_msg_t msg;
        BaseType_t rst = xQueueReceive(smp_info.msg_queue, &msg, wait_time);
        if (rst == pdTRUE)      // If received a message from the message queue.
        {
            // Received a message.
            if (smp_info.state != SMP_STATE_IDLE) 
            {
                // stop last play.
                smp_info.state = SMP_STATE_STOP;
                smp_stop_play_inner(-1);
                smp_info.state = SMP_STATE_IDLE;
            }
            wait_time = 0;
            if (msg.msg_id == SMP_MSG_START_PLAY) 
            {
                // start current play request.
                smp_info.state = SMP_STATE_START;
                if (smp_start_play_inner(&msg) != 0)
                {
                    smp_info.state = SMP_STATE_IDLE;
                    wait_time = portMAX_DELAY;
                }
                smp_info.state = SMP_STATE_PLAY;
            }
        }
        else
        {
            // No message
            if (smp_info.state == SMP_STATE_PLAY)  
            {
                // decode one frame
                int rst = smp_decode_one_frame();
                if (rst == 3 || rst == 0)
                {
                    smp_info.state = SMP_STATE_STOP;
                    smp_stop_play_inner(0);
                    smp_info.state = SMP_STATE_IDLE;
                    wait_time = portMAX_DELAY;
                }
                else
                {
                    wait_time = rst ? 0:2;
                }
            }
            else if (smp_info.state == SMP_STATE_IDLE)
            {
                wait_time = portMAX_DELAY;
            }
            else
            {
                wait_time = 0;
            }
        }
    }
}




