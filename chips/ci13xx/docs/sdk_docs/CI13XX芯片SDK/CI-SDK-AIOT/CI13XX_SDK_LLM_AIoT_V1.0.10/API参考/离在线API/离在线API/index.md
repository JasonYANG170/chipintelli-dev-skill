<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-AIOT/CI13XX_SDK_LLM_AIoT_V1.0.10/API%E5%8F%82%E8%80%83/%E7%A6%BB%E5%9C%A8%E7%BA%BFAPI/%E7%A6%BB%E5%9C%A8%E7%BA%BFAPI/ -->

# CI13XX系列离在线API说明

---

## 1. 主要任务(Task)说明

| 任务名称 | 说明 |
| --- | --- |
| network\_recv\_data\_task | 接收WiFi串口数据任务 |
| request\_play\_data\_task | 请求WiFi播放数据任务 |
| network\_send\_data\_task | 发送数据到WiFi任务 |
| pcm\_data\_play\_task | pcm数据处理及播放任务 |
| opus\_data\_decode\_task | opus解码任务 |
| speex\_encode\_task | speex编码任务 |
| opus\_encode\_task | opus编码任务 |
| pcm\_debug\_task | 串口采音任务(用于调试) |
| vad\_state\_handle\_task | vad状态检测任务 |
| record\_data\_upload\_task | 录音数据上传任务 |

## 2. 函数详细说明

```
int32_t aiot_wifi_msg_callback(const uint8_t *msg_buf)
名称：AIOT网络数据包处理回调
功能描述：解析网络数据包的指令类型，根据指令类型进行对应处理，指令类型详细见《启英泰伦离在线应用对接说明-V2.2》文档
输入参数：msg_buf网络数据地址
返回值：RETURN_OK 处理成功，RETURN_ERR 数据异常
错误处理：解析到未使用的命令时，回复主端执行失败

void request_play_data_func(void)
名称：请求网络端播放数据函数
功能描述：发送请求REQUEST_ONE_FRAME_SEZIE大小的播放数据指令给网路端，调用cias_send_cmd_and_data发送命令和数据，更新本地唤醒时间来等待网络断数据
输入参数：无
返回值：无
错误处理：接收到网络播放数据后，在请求网络播放数据任务持续调用本接口，若超过30次则停止调用请求数据

void request_play_data_func(void)
名称：请求网络端播放数据函数
功能描述：发送请求REQUEST_ONE_FRAME_SEZIE大小的播放数据指令给网路端，调用cias_send_cmd_and_data发送命令和数据，更新本地唤醒时间来等待网络断数据
输入参数：无
返回值：无
错误处理：接收到网络播放数据后，在请求网络播放数据任务持续调用本接口，若超过30次则停止调用请求数据

void request_play_data_task(void *parameter)
名称：请求网络播放数据任务
功能描述：若网络数据请求标志有效并且网络流数据队列中数据不足播放数据最小长度（4K）时，则调用request_play_data_func函数请求网络数据；若网络数据播放标志有效并且网络流数据队列已播放完，则将尝试请求超时计数加一，当网络播放已结束，请求尝试超时计数大于等于10拍（每拍100ms），或者播放未结束尝试请求超时计数大于等于30拍时，设置bnpu播放结束，复位播放相关参数

输入参数：parameter-未使用
返回值：无
错误处理：无

名称：获取VAD_END静音帧配置
功能描述：根据输入静音参数索引，配置gCiasAiotFuncParam.vad_end_mute_frame的值
输入参数：param_index静音参数索引
返回值：无
错误处理：无

void pcm_data_play_task(void *parameter)
名称：网络PCM数据包播放任务
功能描述：接收pcm数据流队列中的数据大于等于512时，接收数据流，先将数据放入底层空闲队列，拷贝完成后，再将空闲队列数据搬到忙碌队列发送
输入参数：parameter
返回值：无
错误处理：检测播放器状态，若在播放中并且pcm数据流队列中的数据不足512，则停止播放，否则会出现播放卡顿

void vad_state_handle_task(void *p_arg)
名称：VAD状态处理任务
功能描述：状态处理开始后，若开启检测网络状态宏CHECK_NET_WORK_STATE_ENABLE，则获取当前云端是否连接，未连接则延迟5ms进入下一次状态处理；若开启命令自学习宏USE_CWSL，则判断当前是否处于学习状态，处于则延迟5ms进入下一次状态处理；若按键处理在进行、VAD未开始并且未唤醒时则延迟5ms进入下一次状态处理。若未收到开始录音指令时，VAD前端检测到VAD开始，若开启云端响应超时宏时，则判断云端是否正在处理，若在处理则设置VAD前端检测状态为非人声并延迟5ms进入下一次状态处理，若未处理则调用cias_aiot_vad_start_handle进行VAD开始处理；前端VAD检测到VAD结束并且VAD开始标志有效时，则清除VAD开始标志，设置VAD前端检测状态为非人声，若未被语音识别唤醒则调用cias_aiot_vad_end_handle进行VAD结束处理；当检测VAD超时并且VAD开始标志有效并且当前上传音频计数大于等于设置的VAD最大超时时间时，则清除VAD开始标志，设置VAD前端检测状态为非人声，设置超时停止上传数据，调用cias_aiot_vad_end_handle进行VAD结束处理。若进入OTA升级则将本任务释放。
输入参数：p_arg未使用
返回值：无
错误处理：无

void asr_wakeup_on_handle(void)
名称：语音识别唤醒句柄
功能描述：设置bnpu清除VAD状态，设置降噪状态，复位云端播放相关参数，云端超时宏开启则停止云端播放请求定时器，设置bnpu唤醒，回复唤醒成功到网络端，调用本地唤醒用户回调执行函数
输入参数：无
返回值：无
错误处理：CHECK_NET_WORK_STATE_ENABLE宏开启则判断是否与目标云端连接，若未连接则播放2005音频并退出接口

void cias_aiot_vad_start_handle(int cmd)
名称：VAD开始句柄
功能描述：检测pcm压缩缓存内的有效数据是否超过前端回退宏PCM_ALG_ROOLBACK_FRAME_LEN，超过则进行数据回退，设置相关标志，若开启了全双工则取消网络数据请求标志，停止数据请求，串口回复网络端，最后若用户自定义了VAD开始回调函数，则调用回调函数。
输入参数：cmd 串口回复的指令类型
返回值：无
错误处理：若pcm压缩缓存需要回退，但接收到缓存数据不为一帧长度PCM_ALG_FRAME_LEN，则输出错误日志

void cias_aiot_vad_end_handle(int type)
名称：VAD结束句柄
功能描述：设置VAD前端检测状态为非人声，若VAD退出类型type为0则
输入参数：type VAD退出类型
返回值：无
错误处理：若pcm压缩缓

void record_data_upload_task(void *p_arg)
名称：采集数据上传任务
功能描述：若音频压缩失能宏AUDIO_COMPRESS_RECORD_DISABLE有效，pcm压缩缓存内有效数据大于0，则接收流缓存中COMPRESS_UPLOAD_QUEUE_TIEM_SIZE长度的数据，收到数据后置位上传忙碌标志，然后分4次调用voice_data_packet_and_send接口发送串口数据，完成发送后清除上传忙碌标志；
若音频压缩失能宏AUDIO_COMPRESS_RECORD_DISABLE无效并且上传网络端标志有效时，接收pcm_upload_queue上传队列的数据，收到一个队列消息后置位上传忙碌标志，调用voice_data_packet_and_send接口发送串口数据，完成发送后清除上传忙碌标志；
每上传5包数据调用update_awake_time接口更新唤醒超时时间，保持唤醒状态；若进入OTA升级则将本任务释放。
输入参数：p_arg未使用
返回值：无
错误处理：若接收到流缓存数据不为512字节或者发送pcm_upload_queue上传队列超时，则输出错误日志

void speex_encode_task(void)
名称：speex压缩任务
功能描述：当开始压缩标志有效并且pcm压缩缓存内有效数据大于等于压缩所需长度（2*COMPRESS_NEED_PCM_LEN）并且speex压缩忙碌标志有效时，则从流缓存中接收压缩所需长度的数据，收到数据后进行speex压缩，置位speex压缩忙碌标志，压缩完成后清除该标志，当pcm_upload_queue上传队列中帧数（每个队列消息为两帧数据）大于等于50（500ms）加上过滤帧数时使能上传网络端标志；将speex压缩后的数据发送pcm_upload_queue上传队列；若进入OTA升级则将本任务释放。
输入参数：无
返回值：无
错误处理：若接收到流缓存数据不为两帧COMPRESS_NEED_PCM_LEN的长度或者发送pcm_upload_queue上传队列超时，则输出错误日志

bool audio_speex_task_init(void)
名称：speex压缩任务初始化
功能描述：根据sb_wb_mode中的参数配置speex编码模式，初始化speex，创建speex压缩任务。
输入参数：无
返回值：pdTRUE初始化成功，pdFALSE初始化失败

错误处理：分配临时speex缓存、配置speex编码失败时，输出错误日志并返回pdFALSE

void opus_encode_task(void)
名称：opus压缩任务初始化
功能描述：初始化opus编码器，当开始压缩标志有效并且pcm压缩缓存内有效数据大于等于所需压缩长度（2*COMPRESS_NEED_PCM_LEN）时，则从流缓存中接收压缩所需长度的数据，收到数据后进行opus压缩，置位opus压缩忙碌标志，压缩完成后清除该标志，当pcm_upload_queue上传队列中帧数（每个队列消息为四帧数据）大于等于50（500ms）加上过滤帧数时使能上传网络端标志 ；将opus压缩后的数据发送pcm_upload_queue上传队列；若进入OTA升级则将本任务释放。
输入参数：无
返回值：无
错误处理：若接收到流缓存数据不为所需压缩长度（2*COMPRESS_NEED_PCM_LEN）或者发送pcm_upload_queue上传队列超时，则输出错误日志

bool audio_opus_task_init(void)
名称：opus压缩任务初始化
功能描述：创建opus压缩任务。
输入参数：无
返回值：pdTRUE初始化成功，pdFALSE初始化失败
错误处理：创建任务失败后输出日志并返回false

int32_t broadlink_frame_create(uint32_t type, const uint8_t *input, uint32_t len, uint8_t *output, uint32_t maxsize, uint32_t fill_data, uint16_t index)
名称：广播连接帧头创建
功能描述：若开启带协议头上传，则在输出数据开始添加协议头，再将输入数据拷贝到输出数据
输入参数：type 指令类型，input 输入数据地址，len输入数据长度，output输出数据地址，maxsize 网络数据包最大长度，fill_data填充数据，index
返回值：正确返回输出数据长度，错误返回-1
错误处理：输出数据地址若为NULL或者输入数据长度加上标准头长度大于最大maxsize返回-1

int32_t cias_send_cmd(uint32_t cmd, uint32_t fill_data)
名称：cias发送命令函数
功能描述：网络发送消息进行组包，调用broadlink_frame_create接口填充协议头，填充消息数据类型、消息数据长度等，消息类型填充为仅命令，组包好的数据输入send_msg_to_network_task接口
输入参数：cmd 指令类型，fill_data填充数据
返回值：返回组包的指令类型
错误处理：无

int32_t cias_send_cmd_and_data(uint32_t cmd_type, uint8_t *buf, int32_t len, uint32_t fill_data)
名称：cias发送命令和数据函数
功能描述：网络发送消息进行组包，调用broadlink_frame_create接口填充协议头，填充消息类型、消息数据长度等，消息的数据类型填充为命令和数据，组包好的数据输入send_msg_to_network_task接口，调用本接口与voice_data_packet_and_send接口共用5帧缓存
输入参数：cmd 指令类型，fill_data填充数据
返回值：返回组包数据长度
错误处理：无

int voice_data_packet_and_send(uint32_t packet_type,uint8_t *voice_data, int32_t packet_len)
名称：cias发送音频数据函数
功能描述：网络发送消息进行组包，调用broadlink_frame_create接口填充协议头，填充消息类型、消息数据长度等，消息的数据类型填充为音频数据中间包，组包好的数据输入send_msg_to_network_task接口，调用本接口与cias_send_cmd_and_data接口共用5帧缓存
输入参数：cmd 指令类型，fill_data填充数据
返回值：0
错误处理：无

BaseType_t send_msg_to_network_task(sdio_task_msg_t *send_msg, BaseType_t *xHigherPriorityTaskWoken)
名称：发送网络消息
功能描述：根据当前是否处于中断/异常状态，采用不同队列发送接口，将发送消息到network_msg_queue队列
输入参数：send_msg发送的消息，xHigherPriorityTaskWoken上下文切换标志
返回值：返回组包的指令类型
错误处理：若开启带协议头上传直接返回0

void network_send_data_task(void *p_arg)
名称：网络发送数据任务
功能描述：读取network_msg_queue队列，判断收到的消息类型为DMA发送，则判断消息内数据的类型，根据命令和数据或者仅命令，将消息内的对应数据输入到network_send接口，任务延时为5ms
输入参数：p_arg-未使用
返回值：无
错误处理：无
```