#ifndef __CIAS_AIOT_PROTOCOL_H__
#define __CIAS_AIOT_PROTOCOL_H__
#include "stream_buffer.h"
#include "timers.h"
#include "queue.h"
#include "FreeRTOS.h"
#include "semphr.h"
typedef struct
{
    //录音数据
    uint16_t wake_up_pcm_frame_count;      //唤醒pcm帧数统计
    uint16_t speex_compress_frame_count;   //压缩音频帧统计
    uint16_t vad_start_pcm_frame_count;    //上传音频统计
    uint16_t upload_pcm_frame_count;       //上传到云端数据帧计数
    bool     stop_collect_pcm_flag;        //停止采集pcm数据
    bool     is_wake_up_flag;              //唤醒标志
    bool     is_vad_on_flag;               //vad start
    bool     is_vad_force_on_flag;         //强制录音
    volatile bool speex_compress_is_busy;       //speex正在压缩
    bool     pcm_upload_is_busy;          //pcm数据正在上传
    bool     compress_pcm_to_wifi_flag;    //开始压缩数据到WiFi标标志
    bool     upload_pcm_to_wifi_flag;      //开始上传数据到WiFi标标志
    bool     need_send_vad_start_flag;     //需要发送vad start标记;
    bool     local_asr_finish_flag;        //本地识别完成标记
    bool    mp3_play_finish_flag;          //mp3播放完成标记
    StreamBufferHandle_t pcm_compress_stream_buffer;    //pcm压缩数据buf
    StreamBufferHandle_t pcm_play_data_stream_buffer;    //pcm播放数据buffer
    StreamBufferHandle_t pcm_debug_stream_buffer;       //pcm采音数据队列

    SemaphoreHandle_t  pcm_play_data_stream_semaphore;
    QueueHandle_t opus_play_queue;         //opus播放数据队列

   // QueueHandle_t pcm_compress_queue;      //pcm压缩数据队列
    QueueHandle_t pcm_upload_queue;        //pcm压缩后上传数据队列
    uint8_t  cur_local_asr_result;         //当前本地ASR识别结果
    uint8_t  is_first_wake_up;             //
    //播放数据
    bool     request_play_data_flag;       //请求播放数据标志
    bool     rcv_cloud_play_data_flag;     //接收到云端播放数据标志
    bool     play_cloud_data_flag;         //播放云端数据标志
    bool     play_cloud_end_flag;          //结束播放指令
    uint32_t cloud_play_data_total_len;    //云端数据播放总长度
    uint8_t request_play_try_count;        //请求播放数据尝试次数
    uint8_t     request_play_count;        //请求播放数据计数
    bool     wait_play_end_flag;           //等待播放结束标志
    //用户回调函数
    void (*customer_wakeup_on_callback)(void);   //本地唤醒用户回调执行函数
    void (*customer_wakeup_exit_callback)(void); //本地退出唤醒用户回调执行函数
    void (*customer_vad_start_callback)(void);   //本地vad start用户执行回调函数
    void (*customer_vad_end_callback)(void);     //本地vad end用户执行回调函数     
    //云端请求计时定时器
    xTimerHandle cloud_ans_count_timer;
    bool cloud_parse_is_busy_flag;                    //云端解析中
    bool cloud_ans_time_out_flag;                     //云端响应超时
    uint8_t cur_doa_aec_work_state;                   //当前doa和aec工作状态 1-doa 2-aec
    //按键功能
    bool key_is_busy_flag;                            //按键处理中
    //HPOUT 输出开关
    bool is_always_hpout_flag;                        //强制hpout持续输出
}CiasAiotRunParamTypedef;

//功能参数定义
typedef struct 
{
    uint16_t vad_filter_frame;                      //vad start和end之间过滤音频帧数，低于该值则不上传云端
    uint16_t vad_end_mute_param_index;              //vad end产生静音设置参数索引
    uint16_t vad_end_mute_frame;                    //vad end产生静音帧数(10ms一帧)
    uint16_t vad_start_max_timeout;                 //vad start后最大超时时间(S)
    uint16_t wake_up_continue_timeout;              //wake up持续唤醒超时时间(S)
    bool     interaction_multi_round;               //多轮(默认)还是单轮，
    bool     upload_play_full_duplex;               //播放音频同时上传音频
    bool     vad_start_stop_paly;                   //全双工模式下，vad start起来是否打断当前播放
    bool     audio_play_mode;                       //音频播放模式(0-不打断当前播报， 1-打断当前播报)
    bool     upload_audio_by_denoise;               //上传音频是否带降噪
    uint8_t  is_play_exit_wakeup_voice;             /*是否带播报音退出唤醒*/
    bool  is_play_enter_wakeup_voice;               /*是否带播报音进入唤醒*/
#if IIS_CHANNEL_ENG_CALC_EANBLE
    bool     upload_factory_test_real_val_flag;     /*上传生产过程中的事实值*/  
    bool     factory_test_is_busying;               //生产测试中
    bool     micl_eng_db_calc_flag;                 //左mic能量和DB值计算使能
    uint8_t  micl_db_thr_val;                       //左通道db阈值
    uint32_t micl_db;   //micl db值
    uint32_t micl_eng;  //micl 能量值
    bool     micr_eng_db_calc_flag;
    uint8_t  micr_db_thr_val;                       //右通道db阈值
    uint32_t micr_db;   //micr db值
    uint32_t micr_eng;  //micr 能量值
    bool     refl_eng_db_calc_flag;
    uint8_t  refl_db_thr_val;                       //左参考通道db阈值
    uint32_t refl_db;   //左参考通道1 db值
    uint32_t refl_eng;  //左参考通道1 能量值
    bool     refr_eng_db_calc_flag;
    uint8_t  refr_db_thr_val;                       //左参考通道db阈值
    uint32_t refr_db;   //右参考通道1 db值
    uint32_t refr_eng;  //右参考通道1 能量值  
#endif       
}CiasAiotFuncParamTypedef;    

#endif  //__CIAS_AIOT_PROTOCOL_H__