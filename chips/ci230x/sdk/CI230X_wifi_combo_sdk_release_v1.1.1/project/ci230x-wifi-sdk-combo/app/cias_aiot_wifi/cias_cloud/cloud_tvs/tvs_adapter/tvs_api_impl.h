#ifndef __TVS_API_IMPL_H__
#define __TVS_API_IMPL_H__

void tvs_api_impl_init();

/**
 * @brief 收到唤醒事件，开始语音对话
 *
 * @param 
 * @return
 */
int tvs_api_impl_speech_wakeup();

/**
 * @brief 前端语音模块写入语音到SDK中进行识别
 *
 * @param audio_data PCM语音数据
 * @param data_size 语音数据的字节数
 * @return
 */
int tvs_api_impl_speech_write_audio(const char* audio_data, int data_size);

/**
 * @brief 收到VAD END事件，结束语音对话
 *
 * @param 
 * @return
 */
void tvs_api_impl_speech_write_end();

/**
 * @brief 初始化，注册回调函数，监听SDK识别事件
 *
 * @param 
 * @return
 */
void tvs_api_impl_speech_init();

#endif