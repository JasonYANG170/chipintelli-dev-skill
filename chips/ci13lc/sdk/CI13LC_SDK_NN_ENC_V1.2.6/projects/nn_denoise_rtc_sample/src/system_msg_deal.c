/**
 * @file system_msg_deal.c
 * @brief  系统消息处理任务
 * @version V1.0.0
 * @date 2019.01.22
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "system_msg_deal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "message_buffer.h"
#include "ci_log.h"
#include "ci_assert.h"
#include "ci13lc_iwdg.h"
#include "sdk_default_config.h"
#include "ci_iisdma.h"
#include "ci_iis.h"
#include "ci_lowpower.h"
#include "ci_core_misc.h"
#include "voice_module_uart_protocol.h"
#include "voice_module_uart_protocol_g1.h"
#include "prompt_player.h"
#include "product_semantic.h"
#include "ci_nvdata_manage.h"
#include "asr_api.h"
#include "user_msg_deal.h"
#include "system_hook.h"
#include "status_share.h"
#include "codec_manager.h"
#include "asr_process_callback_decoder.h"
#include "simple_mp3_player.h"
#include "board.h"
#include "alg_preprocess.h"
#include "ci13lc_gpio.h"
#include "ci_flash_record_play_handle.h"
#include "debug_time_consuming.h"

#define SYS_MSG_BUF_SIZE 256

/* 系统消息处理任务状态 */
typedef enum
{
    USERSTATE_WAIT_MSG = 0,  /* 等待消息 */
    SYS_STATE_WAKUP_TIMEOUT, /* 消息超时 */
} user_task_state_t;

/* 系统状态结构 */
struct sys_manage_type
{
    MessageBufferHandle_t sys_msg_buf; /* 系统消息队列句柄 */
    MessageBufferHandle_t msg_buf;     /* 消息队列句柄 */
    uint8_t user_msg_state;            /* 系统消息处理任务状态                    */
    sys_wakeup_state_t wakeup_state;   /* 系统状态         0:非唤醒状态 1:唤醒状态 */
    sys_asr_state_t asr_state;         /* asr状态          0:空闲       1:忙碌   */
    uint8_t volset;                    /* 音量设置 */
    uint8_t mute_voice_count;          /* 用于保证语音识别开关配对                 */
    sys_msg_t rcv_msg_buf;             /* 系统任务接收消息的buffer */
} sys_manage_data;

/* 唤醒互斥锁 */
SemaphoreHandle_t WakeupMutex = NULL;

/* 退出唤醒定时器 */
xTimerHandle exit_wakeup_timer = NULL;

/* 系统消息队列 */
static QueueHandle_t sys_msg_queue = NULL;
/* 用于忽略退出唤醒的标志，因为存在收到asr识别结果的同时收到退出唤醒的定时器事件，此时不应退出唤醒 */
static int8_t ignore_exit_wakeup = 0;

/*用于忽略语音识别消息 */
static int8_t ignore_asr_msg = 0;

/**
 * @brief 命令词识别结果播报完成回调函数
 *
 * @param cmd_handle 命令信息句柄
 */
_XIF_ void default_play_done_callback(cmd_handle_t cmd_handle)
{
}

/**
 * @brief Get the wakeup state object
 *
 * @return sys_wakeup_state_t
 */
_XIF_ sys_wakeup_state_t get_wakeup_state(void)
{
    return sys_manage_data.wakeup_state;
}

/**
 * @brief Get the asr state
 *
 * @return sys_asr_state_t asr状态
 */
_XIF_ sys_asr_state_t get_asr_state(void)
{
    return sys_manage_data.asr_state;
}

/**
 * @brief 设置状态为唤醒
 *
 * @param exit_wakup_ms 下次退出唤醒时间，单位ms
 */
_XIF_ void set_state_enter_wakeup(uint32_t exit_wakup_ms)
{
    sys_manage_data.wakeup_state = SYS_STATE_WAKEUP; /*update wakeup state*/
    ciss_set(CI_SS_WAKING_UP_STATE, CI_SS_WAKEUPED);
    ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP, CI_SS_WAKEUPED);
    xTimerStop(exit_wakeup_timer, 0);
    xTimerChangePeriod(exit_wakeup_timer, pdMS_TO_TICKS(exit_wakup_ms), 0); /*or used a new timer*/
    xTimerStart(exit_wakeup_timer, 0);
}

/**
 * @brief 更新唤醒超时时间，保持唤醒状态
 *
 */
_XIF_ void update_awake_time(void)
{
    if (sys_manage_data.wakeup_state == SYS_STATE_WAKEUP)
    {
        set_state_enter_wakeup(EXIT_WAKEUP_TIME);
    }
}

/**
 * @brief 设置状态为退出唤醒
 *
 */
_XIF_ void set_state_exit_wakeup(void)
{
    xTimerStop(exit_wakeup_timer, 0);
    sys_manage_data.wakeup_state = SYS_STATE_UNWAKEUP;
    ciss_set(CI_SS_WAKING_UP_STATE, CI_SS_NO_WAKEUP);
    ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP, CI_SS_NO_WAKEUP);
    ciss_set(CI_SS_CMD_STATE, CI_SS_CMD_IS_NULL);
    ciss_set(CI_SS_CMD_STATE_FOR_SSP, CI_SS_CMD_IS_NULL);
}

#if USE_LOWPOWER_OSC_FREQUENCY
/* 退出降频模式进入osc时钟模式定时器 */
xTimerHandle exit_down_freq_mode_timer = NULL;

/**
 * @brief 退出降频模式进入osc时钟模式定时器
 *
 * @param xTimer 定时器句柄
 */
void exit_down_freq_mode_cb(TimerHandle_t xTimer)
{
    xTimerStop(exit_down_freq_mode_timer, 0);

    xSemaphoreTake(WakeupMutex, portMAX_DELAY);

    /* TODO: 检查逻辑确认这样判断的正确性？？？？ */
    if ((SYS_STATE_UNWAKEUP == get_wakeup_state()) && (POWER_MODE_NORMAL == get_curr_power_mode()))
    {
        if ((0 == asrtop_sys_isbusy()) && (AUDIO_PLAY_STATE_IDLE == get_audio_play_state()))
        {
            // cm_stop_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
            // audio_pre_rslt_stop();
            asrtop_asr_system_pause();
#if (USE_DENOISE_MODULE)
            power_mode_switch(POWER_MODE_DOWN_FREQUENCY);
#else
            power_mode_switch(POWER_MODE_OSC_FREQUENCY);
#endif
            asrtop_asr_system_continue();
            // cm_start_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
            // audio_pre_rslt_start();
        }
        else
        {
            xTimerStart(exit_down_freq_mode_timer, 0);
        }
    }
    xSemaphoreGive(WakeupMutex);
}
#endif

#if USE_LOWPOWER_OSC_FREQUENCY
/**
 * @brief vad start中断回调函数，这个函数在vad start中断中调用，
 *          此时需要将频率提升降频模式以上方可保证识别流程正常进行
 */
void vad_start_irq_cb(void)
{
    /* 如果系统处于晶振频率模式，在vad start时切换到正常模式运行 */
    if (POWER_MODE_OSC_FREQUENCY == get_curr_power_mode())
    {
        // cm_stop_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
        // audio_pre_rslt_stop();
        power_mode_switch(POWER_MODE_NORMAL);
        // cm_start_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
        // audio_pre_rslt_start();
        xTimerStartFromISR(exit_down_freq_mode_timer, 0);
    }
}
#endif

/**
 * @brief 切换唤醒模型，这个函数是sys msg任务调用，其他任务需要切换模型需要发送切换模型消息
 *          通过sys msg任务调用
 */
_XIF_ void change_asr_wakeup_word(void)
{
    xSemaphoreTake(WakeupMutex, portMAX_DELAY);

    /*set wakeup state*/
    set_state_exit_wakeup();

    sys_sleep_hook();

    xSemaphoreGive(WakeupMutex);

#if ADAPTIVE_THRESHOLD
// dynmic_confidence_en_cfg(0);
#endif
#if USE_SEPARATE_WAKEUP_EN
    cmd_info_change_cur_model_group(1);
    ignore_asr_msg++;

    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    sys_msg_cmd_info_data_t *msg_data = (sys_msg_cmd_info_data_t *)send_msg.msg_data;
    msg_data->cmd_info_status = MSG_CMD_INFO_STATUS_ENABLE_PROCESS_ASR;
    send_sys_msg_inner(&send_msg, sizeof(send_msg), NULL);
#endif

#if (ASR_SKIP_FRAME_CONFIG == 1)
    //if (get_cur_lm_states() < CLOSE_SKIP_MAX_MODEL_SIZE)
    {
        asr_dynamic_skip_close();
    }
#endif

#if USE_LOWPOWER_OSC_FREQUENCY
    // 晶振时钟模式，可以产生VAD START，不能识别需要在VAD START中断内切换到正常模式才可以识别
    // cm_stop_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
    // audio_pre_rslt_stop();
    asrtop_asr_system_pause();

#if (USE_DENOISE_MODULE)
    power_mode_switch(POWER_MODE_DOWN_FREQUENCY);
#else
    power_mode_switch(POWER_MODE_OSC_FREQUENCY);
#endif

    asrtop_asr_system_continue();
// cm_start_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
// audio_pre_rslt_start();
#elif USE_LOWPOWER_DOWN_FREQUENCY
    // 降频模式，可以进行正常识别，在进入唤醒模式后切换回正常模式即可
    // cm_stop_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
    // audio_pre_rslt_stop();
    asrtop_asr_system_pause();
    power_mode_switch(POWER_MODE_DOWN_FREQUENCY);
    asrtop_asr_system_continue();
// cm_start_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
// audio_pre_rslt_start();
#endif
}

/**
 * @brief 切换正常模型，这个函数是sys msg任务调用，其他任务需要切换模型需要发送切换模型消息
 *          通过sys msg任务调用
 *
 */
_XIF_ void change_asr_normal_word(void)
{
#if (ASR_SKIP_FRAME_CONFIG == 1)
    asr_dynamic_skip_open();
#endif

#if USE_SEPARATE_WAKEUP_EN
    cmd_info_change_cur_model_group(0);
    ignore_asr_msg++;

    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    sys_msg_cmd_info_data_t *msg_data = (sys_msg_cmd_info_data_t *)send_msg.msg_data;
    msg_data->cmd_info_status = MSG_CMD_INFO_STATUS_ENABLE_PROCESS_ASR;
    send_sys_msg_inner(&send_msg, sizeof(send_msg), NULL);
#endif

#if ADAPTIVE_THRESHOLD
// dynmic_confidence_en_cfg(1);
// dynmic_confidence_config(-20, 10, 1);
#endif
}

/**
 * @brief 进入唤醒模式播放完毕回调函数
 *          相比正常的播放完毕回调函数，除了要解除mute之外需要发送切换模型的消息以切换到正常模型
 *
 * @param cmd_handle
 */
_XIF_ void play_enter_wakeup_done_cb(cmd_handle_t cmd_handle)
{
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    sys_msg_cmd_info_data_t *msg_data = (sys_msg_cmd_info_data_t *)send_msg.msg_data;
    msg_data->cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD;
    send_sys_msg_inner(&send_msg, sizeof(send_msg), NULL);
}

/**
 * @brief 退出唤醒模式播放完毕回调函数
 *          相比正常的播放完毕回调函数，除了要解除mute之外需要发送切换模型的消息以切换到唤醒模型，并设置系统状态
 *
 * @param cmd_handle
 */
_XIF_ void play_exit_wakeup_done_cb(cmd_handle_t cmd_handle)
{
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    sys_msg_cmd_info_data_t *msg_data = (sys_msg_cmd_info_data_t *)send_msg.msg_data;
    msg_data->cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD;
    send_sys_msg_inner(&send_msg, sizeof(send_msg), NULL);

    mprintf("inactivate\n");
}

/**
 * @brief when exit wakup state, need deal some common thing, such as
 *        power mode set, change asr word, wakup state flag set, play wakeup prompt.
 *        asr_busy_check used for immediately exit or wait for current ASR deal done.
 *
 * @param asr_busy_check : 0:no need check asr busy, 1:need check asr busy
 */
_XIF_ void exit_wakeup_deal(uint32_t asr_busy_check)
{
    xSemaphoreTake(WakeupMutex, portMAX_DELAY);

    /*already in unwakeup state, so do nothing*/
    if (SYS_STATE_UNWAKEUP == get_wakeup_state())
    {
        xSemaphoreGive(WakeupMutex);
        return;
    }

    /*now asr busy,so need wait not busy, then call this function again*/
    if ((1 == asr_busy_check) && (SYS_STATE_ASR_BUSY == get_asr_state()))
    {
        sys_manage_data.user_msg_state = SYS_STATE_WAKUP_TIMEOUT;
        xSemaphoreGive(WakeupMutex);
        return;
    }
#if PLAY_EXIT_WAKEUP_EN
    /*play exit wakeup voice*/
    prompt_play_by_cmd_string("<inactivate>", -1, play_exit_wakeup_done_cb, false);
#else
    play_exit_wakeup_done_cb(0);
#endif

    xSemaphoreGive(WakeupMutex);
}

/**
 * @brief when enter wakup state, need deal some common thing, such as
 *        power mode set, change asr word, wakup state flag set, play wakeup prompt
 *
 * @param exit_wakup_ms : wakeup state keep times,
 */
_XIF_ void enter_wakeup_deal(uint32_t exit_wakup_ms, cmd_handle_t cmd_handle)
{
    xSemaphoreTake(WakeupMutex, portMAX_DELAY);

    /*if last state is unwakeup, need change asr word*/
    if (SYS_STATE_UNWAKEUP == get_wakeup_state())
    {
#if (USE_LOWPOWER_OSC_FREQUENCY || USE_LOWPOWER_DOWN_FREQUENCY)
        // cm_stop_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
        // audio_pre_rslt_stop();
        asrtop_asr_system_pause();
        power_mode_switch(POWER_MODE_NORMAL);
        asrtop_asr_system_continue();
// cm_start_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
// audio_pre_rslt_start();
#endif
    }

//if(cmd_handle != INVALID_HANDLE)
#if (COMMAND_INFO_VER == 3)
    if (cmd_info_is_auto_play(cmd_handle))
#endif
    {
        /*if last state is unwakeup, need change asr word*/
        if (SYS_STATE_UNWAKEUP == get_wakeup_state())
        {
#if PLAY_ENTER_WAKEUP_EN
            prompt_play_by_cmd_handle(cmd_handle, -1, play_enter_wakeup_done_cb, true);
#else
            play_enter_wakeup_done_cb(cmd_handle);
#endif
        }
        else
        {
#if PLAY_ENTER_WAKEUP_EN
            prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback, true);
#else
            default_play_done_callback(cmd_handle);
#endif
        }
    }

    /*set wakeup state,and update timer*/
    set_state_enter_wakeup(exit_wakup_ms);
    sys_weakup_hook();

    xSemaphoreGive(WakeupMutex);
}

/**
 * @brief : exit wakup timer callback function, this sample code will wait asr idle when exit wakeup,
 *          if you want immediately eixt wakeup, use exit_wakeup_deal(0)
 *
 * @param xTimer : timer handle
 */
_XIF_ void exit_wakeup_timer_callback(TimerHandle_t xTimer)
{
    (void)xTimer;
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    sys_msg_cmd_info_data_t *msg_data = (sys_msg_cmd_info_data_t *)send_msg.msg_data;
    msg_data->cmd_info_status = MSG_CMD_INFO_STATUS_EXIT_WAKEUP;
    send_sys_msg_inner(&send_msg, sizeof(send_msg), NULL);
}

/**
 * @brief 音量设置函数
 *
 * @param vol 音量值
 */
_XIF_ uint8_t vol_set(char vol)
{
    if (vol <= VOLUME_MAX && vol >= VOLUME_MIN && sys_manage_data.volset != vol)
    {
        sys_manage_data.volset = vol;
        audio_play_set_vol_gain(67 * vol / VOLUME_MAX + 7);
        cinv_item_write(NVDATA_ID_VOLUME, sizeof(sys_manage_data.volset), &sys_manage_data.volset);
    }
    return sys_manage_data.volset;
}

_XIF_ uint8_t vol_get(void)
{
    return sys_manage_data.volset;
}

/**
 * @brief 系统消息任务资源初始化
 *
 */
_XIF_ void sys_msg_task_initial(void)
{
    sys_manage_data.sys_msg_buf = xMessageBufferCreate(SYS_MSG_BUF_SIZE);
    if (!sys_manage_data.sys_msg_buf)
    {
        mprintf("not enough memory:%d,%s\n", __LINE__, __FUNCTION__);
    }

    sys_manage_data.msg_buf = xMessageBufferCreate(SYS_MSG_BUF_SIZE);
    if (!sys_manage_data.msg_buf)
    {
        mprintf("not enough memory:%d,%s\n", __LINE__, __FUNCTION__);
    }

    WakeupMutex = xSemaphoreCreateMutex();
    if (!WakeupMutex)
    {
        mprintf("not enough memory:%d,%s\n", __LINE__, __FUNCTION__);
    }
}

/**
 * @brief A simple code for other component send system message to this module, just wrap freertos queue function
 *
 * @param flag_from_isr : 0 call this function not from isr, other call this from isr
 * @param send_msg : system message
 * @param xHigherPriorityTaskWoken : if call this not from isr set NULL
 * @return BaseType_t
 */
_XIF_ BaseType_t send_msg_to_sys_task(void *msg_data, uint32_t msg_len, BaseType_t *xHigherPriorityTaskWoken)
{
    size_t ret;
    if (msg_len > sizeof(sys_msg_t))
    {
        return pdFALSE;
    }

    if (0 != check_curr_trap())
    {
        eclic_global_interrupt_disable();
        ret = xMessageBufferSendFromISR(sys_manage_data.msg_buf, msg_data, msg_len, xHigherPriorityTaskWoken);
        eclic_global_interrupt_enable();
    }
    else
    {
        vTaskSuspendAll();
        ret = xMessageBufferSend(sys_manage_data.msg_buf, msg_data, msg_len, 0);
        xTaskResumeAll();
    }
    if (ret == msg_len)
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

/**
 * @brief 内部接口，应用程序不要使用
 *
 */
_XIF_ BaseType_t send_sys_msg_inner(void *msg_data, uint32_t msg_len, BaseType_t *xHigherPriorityTaskWoken)
{
    size_t ret;
    if (msg_len > sizeof(sys_msg_t))
    {
        return pdFALSE;
    }

    if (0 != check_curr_trap())
    {
        eclic_global_interrupt_disable();
        ret = xMessageBufferSendFromISR(sys_manage_data.sys_msg_buf, msg_data, msg_len, xHigherPriorityTaskWoken);
        eclic_global_interrupt_enable();
    }
    else
    {
        vTaskSuspendAll();
        ret = xMessageBufferSend(sys_manage_data.sys_msg_buf, msg_data, msg_len, 0);
        xTaskResumeAll();
    }
    if (ret == msg_len)
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

/**
 * @brief 处理一些切换模型请求
 *
 * @param cmd_info_msg
 */
_XIF_ void sys_deal_cmd_info_msg(sys_msg_cmd_info_data_t *cmd_info_msg)
{
    /**
     * 这个函数处理退出唤醒模式和切换模型的请求，这些请求的发起方往往是其他线程，切模型和播放如果是异步的
     * 将导致播放获取的固件信息丢失而产生播放错误，所以播放请求和切模型的流程全部在sys_msg线程中执行将不会
     * 出现这个问题。
     * 关于理解ignore_exit_wakeup时，请切记播放是一个异步的请求，在播放到播放完毕期间是可能产生新的系统状态
     * 更新请求，并切换模型，低功耗的流程。
     */
    if (MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD == cmd_info_msg->cmd_info_status)
    {
        /**
         * 切换到唤醒网络请求，这个请求是在播放完退出唤醒提示音后发送的消息，这里要判断ignore_exit_wakeup变量
         * 是由于在退出唤醒播放的过程中可能出现再次唤醒，这时候再发送切换模型的消息应该将其丢弃，系统状态已经
         * 被新的识别消息重置。切换完毕模型之后将进入低功耗策略，也就是切模型本身是工作于非低功耗下。
         */
        if (ignore_exit_wakeup == 0)
        {
            /*change asr wakeup word*/
            change_asr_wakeup_word();
        }
    }
    else if (MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD == cmd_info_msg->cmd_info_status)
    {
        /* 切换到正常的命令词网络，这个请求是在播放完唤醒词播报后发出的请求，此时系统已经进入到工作模式（非低功耗），
           这样时切换模型是合理的 */
        /*change asr normal word*/
        change_asr_normal_word();
    }
    else if (MSG_CMD_INFO_STATUS_EXIT_WAKEUP == cmd_info_msg->cmd_info_status)
    {
        /**
         * 由软定时器发起的请求，这里需要判断ignore_exit_wakeup变量，因为存在极端情况识别结果消息和该消息同时进入队列
         * 如果识别消息已经处理，则此时系统状态已经更新，这里便不可继续退出唤醒的动作，直接丢弃消息即可，否则可能覆盖了
         * 唤醒状态而未完全执行完毕所有的唤醒动作，产生错误。而如果此消息先处理，则会被识别消息刷新状态，不会产生问题
         */
        if (ignore_exit_wakeup == 0)
        {
            exit_wakeup_deal(1);
        }
    }
    else if (MSG_CMD_INFO_STATUS_ENABLE_EXIT_WAKEUP == cmd_info_msg->cmd_info_status)
    {
        /**
         * 由处理识别消息的位置发送的此消息，收到此消息后便可以继续接受正常的退出唤醒的消息了，因为系统状态更新的全流程
         * 执行完毕了此时再接受退出唤醒消息刷新系统状态之后完整进行退出唤醒流程是允许的。
         */
        if (ignore_exit_wakeup > 0)
        {
            ignore_exit_wakeup--;
        }
        CI_ASSERT(ignore_exit_wakeup >= 0, "ignore_exit_wakeup err\n");
    }
    else if (MSG_CMD_INFO_STATUS_ENABLE_PROCESS_ASR == cmd_info_msg->cmd_info_status)
    {
        if (ignore_asr_msg > 0)
        {
            ignore_asr_msg--;
        }
    }
}

/**
 * @brief 处理asr发送的消息
 *
 * @param asr_msg
 */
_XIF_ void sys_deal_asr_msg(sys_msg_asr_data_t *asr_msg)
{
    cmd_handle_t cmd_handle;

    if (MSG_ASR_STATUS_GOOD_RESULT == asr_msg->asr_status && (ignore_asr_msg == 0))
    {
        int send_cancel_ignore_msg = 0;
        cmd_handle = (cmd_handle_t)asr_msg->asr_cmd_handle;
        if (SYS_STATE_WAKEUP == get_wakeup_state())
        {
            /* 这里已经唤醒了，所以目前已经在队列里的退出唤醒消息无效了，开启这个忽略退出唤醒消息的标志 */
            ignore_exit_wakeup++;
            send_cancel_ignore_msg = 1;
        }

        ciss_set(CI_SS_CMD_SCORE, asr_msg->asr_score);
        if (cmd_info_is_wakeup_word(cmd_handle)) /*wakeup word*/
        {
            /*updata wakeup state*/
            ciss_set(CI_SS_CMD_STATE, CI_SS_CMD_IS_WAKEUP);
            ciss_set(CI_SS_CMD_STATE_FOR_SSP, CI_SS_CMD_IS_WAKEUP);
            enter_wakeup_deal(EXIT_WAKEUP_TIME, cmd_handle); /*updata wakeup state*/
        }
        else if (SYS_STATE_WAKEUP == get_wakeup_state())
        {
            ciss_set(CI_SS_CMD_STATE, CI_SS_CMD_IS_NORMAL);
            ciss_set(CI_SS_CMD_STATE_FOR_SSP, CI_SS_CMD_IS_NORMAL);
            set_state_enter_wakeup(EXIT_WAKEUP_TIME);

            uint32_t semantic_id = cmd_info_get_semantic_id(cmd_handle);
            uint16_t cmd_id = cmd_info_get_command_id(cmd_handle);
            ci_loginfo(LOG_USER, "asr cmd_id:%d,semantic_id:%08x\n", cmd_id, semantic_id);
            //先按用户ID处理，如果用户没有处理，再按语义ID处理
            if (0 == deal_asr_msg_by_cmd_id(asr_msg, cmd_handle, cmd_id))
            {
                if (0 == deal_asr_msg_by_semantic_id(asr_msg, cmd_handle, semantic_id))
                {
#if PLAY_OTHER_CMD_EN
#if (COMMAND_INFO_VER == 3)
                    if (cmd_info_is_auto_play(cmd_handle))
                    {
                        prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback, true);
                    }
#else
                    prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback, true);
#endif
#endif
                }
            }
        }

        if (send_cancel_ignore_msg)
        {
            /* 发送一个取消忽略退出唤醒的消息到队列尾部，处理该消息后就允许退出唤醒了 */
            sys_msg_t send_msg;
            send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
            sys_msg_cmd_info_data_t *msg_data = (sys_msg_cmd_info_data_t *)send_msg.msg_data;
            msg_data->cmd_info_status = MSG_CMD_INFO_STATUS_ENABLE_EXIT_WAKEUP;
            send_sys_msg_inner(&send_msg, sizeof(send_msg), NULL);
        }
        sys_asr_result_hook(cmd_handle, asr_msg->asr_score);
    }
    else if ((MSG_ASR_STATUS_NO_RESULT == asr_msg->asr_status) ||
             (MSG_ASR_STATUS_VAD_END == asr_msg->asr_status))
    {
        sys_manage_data.asr_state = SYS_STATE_ASR_IDLE; /*idle*/
    }
    else if (MSG_ASR_STATUS_VAD_START == asr_msg->asr_status)
    {
        sys_manage_data.asr_state = SYS_STATE_ASR_BUSY; /*busy*/
    }
    else
    {
    }
}

/**
 * @brief system message deal function and user main UI flow. system message include ASR, player, KEY, COM
 *
 * @param p_arg
 */
_XIF_ void UserTaskManageProcess(void *p_arg)
{
    sys_msg_t rev_msg;
    BaseType_t err = pdPASS;

    /* 上电初始化状态 */
    sys_manage_data.wakeup_state = SYS_STATE_UNWAKEUP;
    ciss_set(CI_SS_WAKING_UP_STATE, CI_SS_NO_WAKEUP);
    ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP, CI_SS_NO_WAKEUP);
    sys_manage_data.user_msg_state = USERSTATE_WAIT_MSG;
    sys_manage_data.mute_voice_count = 0;

    /* 退出唤醒timer和led pwm控制timer 初始化 */
    exit_wakeup_timer = xTimerCreate("exit_wakeup", pdMS_TO_TICKS(EXIT_WAKEUP_TIME),
                                     pdFALSE, (void *)0, exit_wakeup_timer_callback);

    if (NULL == exit_wakeup_timer)
    {
        ci_logerr(LOG_USER, "user task create timer error\n");
    }

#if USE_LOWPOWER_OSC_FREQUENCY
    /* osc时钟低功耗模式使用的timer，用于vad start但未唤醒的超时timer */
    exit_down_freq_mode_timer = xTimerCreate("exit_down_freq_mode", pdMS_TO_TICKS(15000),
                                             pdFALSE, (void *)0, exit_down_freq_mode_cb);
#endif

    ciss_set(CI_SS_USER_TASK_START, CI_SS_USER_TASK_START_EN);

    while (1)
    {
        /* 不阻塞接收消息 */
        size_t rcv_size = 0;
        /* 系统消息不为空 */
        if (xMessageBufferIsEmpty(sys_manage_data.sys_msg_buf) != pdTRUE)
        {
            rcv_size = xMessageBufferReceive(sys_manage_data.sys_msg_buf, &rev_msg, sizeof(sys_msg_t), 0);
        }
        /* 应用消息不为空 */
        else if (xMessageBufferIsEmpty(sys_manage_data.msg_buf) != pdTRUE)
        {
            rcv_size = xMessageBufferReceive(sys_manage_data.msg_buf, &rev_msg, sizeof(sys_msg_t), 0);
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(2));
        }

        if (0 < rcv_size)
        {
            /* 根据消息来源来处理对应消息，用户可以自己创建属于自己的系统消息类型 */
            switch (rev_msg.msg_type)
            {
            /* 来自ASR的消息，主要通知识别结果，asr状态 */
            case SYS_MSG_TYPE_ASR:
            {
                sys_msg_asr_data_t *asr_rev_data;
                asr_rev_data = (sys_msg_asr_data_t *)rev_msg.msg_data;
                sys_deal_asr_msg(asr_rev_data);
                break;
            }
            /* 系统控制消息，主要通知处理识别模型切换以及低功耗模式切换的请求 */
            case SYS_MSG_TYPE_CMD_INFO:
            {
                sys_msg_cmd_info_data_t *cmd_info_rev_data;
                cmd_info_rev_data = (sys_msg_cmd_info_data_t *)rev_msg.msg_data;
                sys_deal_cmd_info_msg(cmd_info_rev_data);
                break;
            }
            /* 音频采集任务消息，目前处理音频采集开启完成，在这里播放欢迎词 */
            case SYS_MSG_TYPE_AUDIO_IN_STARTED:
            {
                uint8_t volume;
                uint16_t real_len;

                /* 从nvdata里读取播放音量 */
                if (CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_VOLUME, sizeof(volume), &volume, &real_len))
                {
                    /* nvdata内无播放音量则配置为初始默认音量并写入nv */
                    volume = VOLUME_DEFAULT;
                    cinv_item_init(NVDATA_ID_VOLUME, sizeof(volume), &volume);
                }
                /* 音量设置 */
                vol_set(volume);

#if (EXCEPTION_RST_SKIP_BOOT_PROMPT)
                if (RETURN_OK != scu_get_system_reset_state())
                {
                    /* 本次开机属于异常复位，不需要播放欢迎词直接进入切换非唤醒模式流程 */
                    sys_msg_t send_msg;
                    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
                    sys_msg_cmd_info_data_t *msg_data = (sys_msg_cmd_info_data_t *)send_msg.msg_data;
                    msg_data->cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD;
                    send_sys_msg_inner(&send_msg, sizeof(send_msg), NULL);
                }
                else
#endif
                {
#if PLAY_WELCOME_EN
                    vTaskDelay(pdMS_TO_TICKS(300)); // 等待功放开启

                    /* mute语音输入并播放欢迎词，播放完毕后调用play_exit_wakeup_done_cb回调以开启语音输入并进入切换非唤醒模式流程 */
                    prompt_play_by_cmd_string("<welcome>", -1, play_exit_wakeup_done_cb, true);
#else
                    play_exit_wakeup_done_cb(INVALID_HANDLE);
#endif
                }
                userapp_initial();

/* 通过串口协议发送模块上电通知 */
#if (EXCEPTION_RST_SKIP_BOOT_PROMPT)
                if (RETURN_OK == scu_get_system_reset_state())
#endif
                {
#if MSG_COM_USE_UART_EN
#if UART_BAUDRATE_CALIBRATE
                    send_baudrate_sync_req();
#endif
#endif
                    sys_power_on_hook();
                }
                break;
            }
            default:
                deal_userdef_msg(&rev_msg);
                break;
            }

            switch (sys_manage_data.user_msg_state)
            {
            case SYS_STATE_WAKUP_TIMEOUT:
            {
                if (SYS_STATE_ASR_IDLE == get_asr_state())
                {
                    exit_wakeup_deal(0);
                    sys_manage_data.user_msg_state = USERSTATE_WAIT_MSG;
                }
                break;
            }
            default:
                break;
            }
        }
        else
        {
            // TODO:
            /*timeout for feed watchdog*/
        }
    }
}

//通话降噪相关函数
ci_rtc_state_t rtc_state;

_XIF_ void set_rtc_mode(ci_rtc_mode_t mode)
{
    rtc_state.mode = mode;
}

//通道数据选择
_XIF_ short *iis_audio_choose(audio_channel_model channel_model, ci_wrapfft_audio *wrapfft_audio_t, short *iis_dst_data)
{
    short *dst_data;
    switch (channel_model)
    {
    case MICL: //mic左通道
    {
        dst_data = wrapfft_audio_t->mic[0];
        break;
    }
    case MICR: //mic右通道
    {
        dst_data = wrapfft_audio_t->mic[1];
        break;
    }
    case REFL: //ref左通道
    {
        dst_data = wrapfft_audio_t->ref[0];
        break;
    }
    case REFR: //ref右通道
    {
        dst_data = wrapfft_audio_t->ref[1];
        break;
    }
    case DST1: //数据处理后的通道
    {
        if (iis_dst_data == NULL)
            dst_data = wrapfft_audio_t->dst[0];
        else
            dst_data = iis_dst_data;
        break;
    }
    case DST2: //数据处理后的通道，算法处理存在输出双通道的情况
    {
        dst_data = wrapfft_audio_t->dst[1];
        break;
    }
    default: //默认配置mic左通道
    {
        dst_data = wrapfft_audio_t->mic[0];
    }
    }
    if (NULL != dst_data)
    {
        //32k采样率数据通过采音板输出需要抽取转成16k数据
        if (32000 == wrapfft_audio_t->module_config->sample_frequency)
        {
            //32k数据进过istft处理后会降采样成16k采样率数据跳过抽取操作
            if (DST1 != channel_model && DST2 != channel_model)
            {
                int frame_len = wrapfft_audio_t->iis_input_frame_len / 2;
                for (int i = 0; i < frame_len; i++)
                {
                    dst_data[i] = dst_data[2 * i];
                }
                //mprintf("DST1:dst_data=%p\n",dst_data);
            }
        }
    }
    else
    {
        ci_logdebug(LOG_SSP_MODULE, "iis_result_out_error:\n");
    }

    return dst_data;
}

//ifft数据处理队列
typedef struct
{
    uint32_t eq_config_addr;
    uint32_t drc_config_addr;
    ci_wrapfft_audio wrapfft_audio_st;
    uint32_t iis_out_audio_addr;
    short dst_data[256];
} ifft_data_msg_t;

npu_eq_param_t *g_eq_param = NULL;
npu_drc_param_t *g_drc_param = NULL;
QueueHandle_t ifft_data_queue = NULL;
ifft_data_msg_t send_ifft_data_msg;
extern eq_param_t init_eq_drc_val;
_XIF_ int ifft_data_handle_task_rtc_init(void)
{
    ifft_data_queue = xQueueCreate(16, sizeof(ifft_data_msg_t));
    if (!ifft_data_queue)
    {
        mprintf("ifft_data_queue create error !\r\n", __LINE__, __FUNCTION__);
        return -1;
    }
    mprintf("ifft_data_queue create success ..\r\n");
    return 0;
}
/**
* @brief   将EQ和DRC前端算法移到host端来执行，传递过来的参数结构体都是在NPU端初始化
* 
* @param eq_config_addr       EQ参数结构体地址             
* @param drc_config_addr      DRC参数结构体地址             
* @param wrapfft_audio_addr   前端算法处理语音数据结构体地址             
* @param iis_out_audio_addr   IIS参数结构体地址             
* @return int     -1:失败; 0:成功.
*/
_XIF_ int ifft_data_to_eq_drc_handle(uint32_t eq_config_addr, uint32_t drc_config_addr, ci_wrapfft_audio *wrapfft_audio_st, short *dst_data, uint32_t iis_out_audio_addr)
{
    int ret = 0;
    g_eq_param = (npu_eq_param_t *)eq_config_addr;
    g_drc_param = (npu_drc_param_t *)drc_config_addr;
    //EQ和DRC处理
#if USE_EQ_MODULE
    ret = ci_eq_deal((void *)eq_config_addr, dst_data, dst_data);
#endif
#if USE_DRC_MODULE
    ret = ci_drc_deal((void *)drc_config_addr, dst_data, dst_data);
#endif
    iis_out_audio_config_t *iis_out_audio = (iis_out_audio_config_t *)iis_out_audio_addr;
    int frame_len = wrapfft_audio_st->iis_input_frame_len;
    if (32000 == wrapfft_audio_st->module_config->sample_frequency)
    {
        frame_len /= 2;
    }

    if (iis_out_audio->iis_out_enable)
    {
        short *left_data, *right_data;
        audio_channel_model left_channel_model = iis_out_audio->iis_left_channel;
        audio_channel_model right_channel_model = iis_out_audio->iis_right_channel;

#if SYS_BOOT_AUDIO_DATA_PREPROCESS
        static int frame_count = 0;
        if (frame_count < 3)
        {
            left_data = iis_audio_choose(MICL, wrapfft_audio_st);
            right_data = iis_audio_choose(MICR, wrapfft_audio_st);
            frame_count++;
        }
        else
        {
#if USE_EQ_MODULE | USE_DRC_MODULE
            left_data = iis_audio_choose(left_channel_model, wrapfft_audio_st, dst_data);
#else
            left_data = iis_audio_choose(left_channel_model, wrapfft_audio_st, NULL);

#endif
            right_data = iis_audio_choose(right_channel_model, wrapfft_audio_st, NULL);
        }
#else
#if USE_EQ_MODULE | USE_DRC_MODULE
        left_data = iis_audio_choose(left_channel_model, wrapfft_audio_st, dst_data);
#else
        left_data = iis_audio_choose(left_channel_model, wrapfft_audio_st, NULL);

#endif
        right_data = iis_audio_choose(right_channel_model, wrapfft_audio_st, NULL);
#endif

        audio_pre_rslt_write_data(right_data, left_data);
    }

    if ((iis_out_audio->ssp_dst_cover_micl_enble) && (NULL != dst_data))
    {

        MASK_ROM_LIB_FUNC->newlibcfunc.memcpy_p(wrapfft_audio_st->mic[0], dst_data, sizeof(int16_t) * frame_len);
    }
    return ret;
}

ifft_data_msg_t rcv_ifft_data_msg;
_XIF_ void ifft_data_handle_task_rtc(void *p)
{
    BaseType_t ret = 0;

    while (1)
    {
        //阻塞接收数据
        ret = xQueueReceive(ifft_data_queue, &rcv_ifft_data_msg, portMAX_DELAY);
        if (ret == pdPASS)
        {
            ifft_data_to_eq_drc_handle(rcv_ifft_data_msg.eq_config_addr, rcv_ifft_data_msg.drc_config_addr, &(rcv_ifft_data_msg.wrapfft_audio_st), rcv_ifft_data_msg.dst_data, rcv_ifft_data_msg.iis_out_audio_addr);
        }
        else
        {
            mprintf("ifft_data_queue rcv error....\r\n");
        }
    }
}

//获取模型
_XIF_ void get_denoise_model_addr(void)
{
    uint32_t addr;
    uint32_t size;
    get_dnn_addr_by_id(CI_AI_DENOISE_MODEL_ID, &addr, &size);
    ciss_set(CI_SS_AI_DENOISE_MODEL_ADDR, addr);
}

int rtc_voice_buffer_read(uint32_t* data);
void rtc_voice_buffer_write(int16_t* addr);

//通话降噪任务处理
extern ci_wrapfft_audio *p_wrapfft_audio;
extern void *drc;
extern void *eq;
extern void *agc;
extern void *iis_out_audio;

static int is_rtc_deal_task_init_done = false;

_XIF_ void audio_in_manage_inner_task_rtc(void *p)
{
    int ret = 0;
    // cm_set_codec_alc(HOST_MIC_RECORD_CODEC_ID, CM_CHA_LEFT,DISABLE);
    // cm_set_codec_adc_gain(HOST_MIC_RECORD_CODEC_ID, CM_CHA_LEFT, 6);
    // cm_set_codec_adc_gain(HOST_MIC_RECORD_CODEC_ID, CM_CHA_LEFT,22);
    cm_start_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
    ciss_set(CI_SS_MIC_VOICE_STATUE, CI_SS_MIC_VOICE_NORMAL);
    audio_pre_rslt_out_play_card_init();
    cm_set_codec_dac_gain(PLAY_CODEC_ID, CM_CHA_TWO_CHA, RTC_HPOUT_DEFAULT_VOL);

    is_rtc_deal_task_init_done = true;
    for (;;)
    {
        if (rtc_state.mode == CI_RTC_MODE_UP)
        {
            //上行模式，MIC收音，处理之后发送出去
            vTaskDelay(pdMS_TO_TICKS(0));
            uint32_t data_addr = 0, data_size;
            // if (0 == cm_read_codec(HOST_MIC_RECORD_CODEC_ID, &data_addr, &data_size, 0))
            // {
            //     rtc_voice_buffer_write((int16_t*)data_addr);
            // }
            int ret = rtc_voice_buffer_read(&data_addr);
            if(ret)
            {
                // timer0_end_count_only_print_time_us();
                // init_timer0();
                // timer0_start_count();
                #if 0
                mprintf("data_size = %d\r\n", data_size);
                short *ptr = (short *)data_addr;
                for(int i = 0 ;i < data_size; i++)
                {
                    mprintf("%d ", ptr[i]);
                }
                #endif
                if (!data_addr)
                {
                    mprintf("iisdma int too slow\n");
                    continue;
                }
                p_wrapfft_audio->mic[0] = (int16_t *)data_addr;
                static uint32_t frame_count = 0;
				#if 0
                if(frame_count < 3)
                {   
                    audio_pre_rslt_write_data((int16_t*)p_wrapfft_audio->mic[0], (int16_t*)p_wrapfft_audio->mic[0]);
                    frame_count++;
                }
                else
                {
                    ret = ci_ssp_processing();
                    audio_pre_rslt_write_data((int16_t*)p_wrapfft_audio->mic[0],(int16_t*)(int16_t*)p_wrapfft_audio->dst[0]);
                }
				#else
				ret = ci_ssp_processing();
                iwdg_feed(IWDG);
                if(rtc_play_status != PLAY_END)  //播放中，不进行录音操作
                {
                    continue;
                }
                audio_pre_rslt_write_data((int16_t*)p_wrapfft_audio->mic[0],(int16_t*)(int16_t*)p_wrapfft_audio->dst[0]);
                #endif

                //gpio_set_output_level_single(PC, pin_4, 0);           // 输出高电平

               
            }
        }
    }
}



// #define RTC_VOICE_BUFFER_FRM AUDIO_IN_BUFFER_NUM
#define RTC_VOICE_BUFFER_FRM 4
static int16_t rtc_voice_tmp_buffer[RTC_VOICE_BUFFER_FRM][AUDIO_CAP_POINT_NUM_PER_FRM];
static uint32_t rtc_voice_cnt_r = 0;
static uint32_t rtc_voice_cnt_w = 0;


int rtc_voice_buffer_read(uint32_t* data)
{
    if(rtc_voice_cnt_w > rtc_voice_cnt_r)
    {
        //mprintf("R W %d  R %d\n", rtc_voice_cnt_w, rtc_voice_cnt_r);
        vTaskSuspendAll();
        int r_ptr = rtc_voice_cnt_r % RTC_VOICE_BUFFER_FRM;
        *data = (uint32_t)&rtc_voice_tmp_buffer[r_ptr][0];
        rtc_voice_cnt_r++;
        xTaskResumeAll();
        return 1;
    }
    else{
        return 0;
    }
}


void rtc_voice_buffer_write(int16_t* addr)
{
    //mprintf("W W %d  R %d\n", rtc_voice_cnt_w, rtc_voice_cnt_r);
    int tmp_counrt = rtc_voice_cnt_w - rtc_voice_cnt_r;
    if(tmp_counrt >= (RTC_VOICE_BUFFER_FRM/2)+1)
    {
        rtc_voice_cnt_r = rtc_voice_cnt_w;
        mprintf("w full\n");
    }

    vTaskSuspendAll();
    int w_ptr = rtc_voice_cnt_w % RTC_VOICE_BUFFER_FRM;
    memcpy((void*)&rtc_voice_tmp_buffer[w_ptr][0], (void*)addr, AUDIO_CAP_POINT_NUM_PER_FRM * sizeof(int16_t));
    rtc_voice_cnt_w++;
    xTaskResumeAll();
    // mprintf("dst_db2 = %d,w_ptr = %d\n", (int)smooth_db_long, tmp_counrt);
    
}



void voice_recode_task(void* p)
{
    // vTaskDelay(pdMS_TO_TICKS(2000));
    while(!is_rtc_deal_task_init_done)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        mprintf("wait rtc deal init done\n");
    }
    mprintf("RTC_VOICE_BUFFER_FRM = %d\n",RTC_VOICE_BUFFER_FRM);
    while(1)
    {
        uint32_t data_addr = 0, data_size;
        if (0 == cm_read_codec(HOST_MIC_RECORD_CODEC_ID, &data_addr, &data_size, 0))
        {
            rtc_voice_buffer_write((int16_t*)data_addr);
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }

}

bool get_rtc_flash_record_play_enable()
{
    return USE_FLASH_RECORD_PLAY_ENABLE;
}


// fft获取输入信号的高通滤波器的截止频率
int get_rtc_fft_parame()
{
    int hpf = HPF_FILTER_CUT_OFF_FREQ/31.25 + 1;

    return hpf;
}

// ifft获取出信号的低通滤波器的截止频率
int get_rtc_ifft_parame()
{
    int lpf = USE_OUTPUT_BANDWIDTH/31.25 + 1;

    return lpf;
}