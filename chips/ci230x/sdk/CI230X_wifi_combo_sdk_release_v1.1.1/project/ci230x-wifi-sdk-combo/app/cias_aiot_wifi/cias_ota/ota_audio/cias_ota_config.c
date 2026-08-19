#include "cias_ota_config.h"
#if CIAS_IOT_TENCENT_ENABLE   //腾讯云使能
#include "cias_qcloud_main.h"
extern OTAContextData *ota_ctx;
#endif
cias_ota_handle_t cias_ota_handle =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

cias_ota_handle_t *get_cias_ota_handle(void)
{
    return &cias_ota_handle;
}

//初始化网络通信协议
extern void http_ota_olcp_send(unsigned char *buff, unsigned int len);
extern void cias_olcp_ota_call_back(unsigned short recv_count, unsigned short cmd, unsigned char *data, unsigned short len);


cias_ota_data_t *cias_ota_data = NULL;
cias_olcp_t* cias_ota_olcp;
void cias_net_protocol_init(void)
{
    olcp_send_t hal_send = http_ota_olcp_send;

	//初始化通信协议
	cias_ota_olcp = cias_olcp_create(hal_send, cias_olcp_ota_call_back);
#if CIAS_IOT_TENCENT_ENABLE   //腾讯云使能
	cias_ota_data->file_size = ota_ctx->fw_file_size ;
#endif
    CIAS_LOG_HL(OTA_LOG"\r\n>>>file size: %d", cias_ota_data->file_size);

	if((hal_send == NULL) || (cias_ota_olcp == NULL))
	{
  		CIAS_LOG_HL("Exit http ota...");
  		get_cias_ota_handle()->cias_audio_device_reboot();
		get_cias_ota_handle()->cias_ota_task_delay_ms(2);
		get_cias_ota_handle()->cias_wifi_device_reboot();
	}
}

/**
 * @brief ota结构体初始化
 *
 * @param uri: 镜像文件url
 * @retval -1: error
 * @retval 0: success
 */
int cias_net_data_struct_init(void)
{
	if (cias_ota_data == NULL)
		cias_ota_data = (cias_ota_data_t *)malloc(sizeof(cias_ota_data_t));
	if (cias_ota_data == NULL)
		return -1;
	cias_ota_data->updating.buff = malloc(4096 + 4);
	if (cias_ota_data->updating.buff == NULL)
		return -1;
	memset(cias_ota_data->updating.buff, 0x00, (4096 + 4));
	cias_ota_data->session = NULL;
    cias_ota_olcp = NULL;    
    return 0;
}
extern uint8_t hal_flash_read(uint32_t offset, uint32_t length, uint8_t *buffer);
int cias_ota_callback_init(void)
{
    //注册OTA回调函数
#if CIAS_IOT_TENCENT_ENABLE   //腾讯云使能
	get_cias_ota_handle()->cias_get_audio_image = get_audio_image_from_cloud;
	get_cias_ota_handle()->cias_report_ota_status_to_iot = report_ota_status_to_iot;
#endif
    get_cias_ota_handle()->cias_wifi_device_reboot = ln_chip_reboot;
    get_cias_ota_handle()->cias_audio_device_reboot = cias_ln_audio_reset;
    get_cias_ota_handle()->cias_communication_rate_change = communication_rate_change;
    get_cias_ota_handle()->cias_ota_communication_send = communication_send;
    get_cias_ota_handle()->cias_ota_communication_recv = communication_recv;
    get_cias_ota_handle()->cias_ota_task_delay_ms = vTaskDelay;
    get_cias_ota_handle()->cias_ota_flash_read = hal_flash_read;
    get_cias_ota_handle()->cias_net_portocol_init = cias_net_protocol_init;
    get_cias_ota_handle()->cias_net_data_struct_init = cias_net_data_struct_init;

	if(get_cias_ota_handle()->cias_get_audio_image == NULL 
		|| get_cias_ota_handle()->cias_report_ota_status_to_iot == NULL
		|| get_cias_ota_handle()->cias_wifi_device_reboot == NULL
		|| get_cias_ota_handle()->cias_audio_device_reboot == NULL
		|| get_cias_ota_handle()->cias_communication_rate_change == NULL
		|| get_cias_ota_handle()->cias_ota_communication_send == NULL
		|| get_cias_ota_handle()->cias_ota_communication_recv == NULL
		|| get_cias_ota_handle()->cias_ota_task_delay_ms == NULL
		|| get_cias_ota_handle()->cias_ota_flash_read == NULL
		|| get_cias_ota_handle()->cias_net_portocol_init == NULL
		|| get_cias_ota_handle()->cias_net_data_struct_init == NULL)
	{
		CIAS_LOG_ERR("ota module register error");
		return CIAS_FAIL;
	}
	return CIAS_OK;
}

/*===================================================================================*/
/**************************************OTA任务例程*************************************/
/*===================================================================================*/
extern ota_update_flag_t ota_update_flag;
extern unsigned char * test_ota_update_buffer;
extern unsigned long write_data_offset;

/**
 * @brief 获取分区表信息
 *
 * @retval 0: fail 
 * @retval 1: success
 */
static int cias_ota_get_partitions_info(void)
{
	int len = 0;

	len = get_cias_ota_handle()->cias_get_audio_image((unsigned char *)&(cias_ota_data->file_info), PARTITION_TABLE2_START_ADDR, sizeof(partition_table_t));
	if (len != sizeof(partition_table_t))
	{
		CIAS_LOG_ERR("read file_info err");
		return 0;
	}
#if 0//分区表信息打印示例
	CIAS_LOG_HL("云端sizeof(info):%d", cias_ota_data->file_info);
	CIAS_LOG_HL("ManufacturerID = 0x%x", cias_ota_data->file_info.ManufacturerID);
	CIAS_LOG_HL("ProductID = 0x%x", cias_ota_data->file_info.ProductID);
	CIAS_LOG_HL("HWName = %s", cias_ota_data->file_info.HWName);
	CIAS_LOG_HL("HWVersion = 0x%0x", cias_ota_data->file_info.HWVersion);
	CIAS_LOG_HL("SWName = %s", cias_ota_data->file_info.SWName);
	CIAS_LOG_HL("SWVersion = 0x%0x", cias_ota_data->file_info.SWVersion);
	CIAS_LOG_HL("BootLoaderVersion = 0x%0x", cias_ota_data->file_info.BootLoaderVersion);
	CIAS_LOG_HL("Reserve = %s", cias_ota_data->file_info.Reserve);

	CIAS_LOG_HL("user_code1.version = 0x%0x", cias_ota_data->file_info.user_code1.version);
	CIAS_LOG_HL("user_code1.address = 0x%0x", cias_ota_data->file_info.user_code1.address);
	CIAS_LOG_HL("user_code1.size = 0x%0x", cias_ota_data->file_info.user_code1.size);
	CIAS_LOG_HL("user_code1.crc = 0x%0x", cias_ota_data->file_info.user_code1.crc);
	CIAS_LOG_HL("user_code1.status = 0x%0x", cias_ota_data->file_info.user_code1.status);
	CIAS_LOG_HL("user_code2.version = 0x%0x", cias_ota_data->file_info.user_code2.version);
	CIAS_LOG_HL("user_code2.address = 0x%0x", cias_ota_data->file_info.user_code2.address);
	CIAS_LOG_HL("user_code2.size = 0x%0x", cias_ota_data->file_info.user_code2.size);
	CIAS_LOG_HL("user_code2.crc = 0x%0x", cias_ota_data->file_info.user_code2.crc);
	CIAS_LOG_HL("user_code2.status = 0x%0x", cias_ota_data->file_info.user_code2.status);

	CIAS_LOG_HL("asr_cmd_model.version = 0x%0x", cias_ota_data->file_info.asr_cmd_model.version);
	CIAS_LOG_HL("asr_cmd_model.address = 0x%0x", cias_ota_data->file_info.asr_cmd_model.address);
	CIAS_LOG_HL("asr_cmd_model.size = 0x%0x", cias_ota_data->file_info.asr_cmd_model.size);
	CIAS_LOG_HL("asr_cmd_model.crc = 0x%0x", cias_ota_data->file_info.asr_cmd_model.crc);
	CIAS_LOG_HL("asr_cmd_model.status = 0x%0x", cias_ota_data->file_info.asr_cmd_model.status);

	CIAS_LOG_HL("dnn_model.version = 0x%0x", cias_ota_data->file_info.dnn_model.version);
	CIAS_LOG_HL("dnn_model.address = 0x%0x", cias_ota_data->file_info.dnn_model.address);
	CIAS_LOG_HL("dnn_model.size = 0x%0x", cias_ota_data->file_info.dnn_model.size);
	CIAS_LOG_HL("dnn_model.crc = 0x%0x", cias_ota_data->file_info.dnn_model.crc);
	CIAS_LOG_HL("dnn_model.status = 0x%0x", cias_ota_data->file_info.dnn_model.status);

	CIAS_LOG_HL("voice.version = 0x%0x", cias_ota_data->file_info.voice.version);
	CIAS_LOG_HL("voice.address = 0x%0x", cias_ota_data->file_info.voice.address);
	CIAS_LOG_HL("voice.size = 0x%0x", cias_ota_data->file_info.voice.size);
	CIAS_LOG_HL("voice.crc = 0x%0x", cias_ota_data->file_info.voice.crc);
	CIAS_LOG_HL("voice.status = 0x%0x", cias_ota_data->file_info.voice.status);

	CIAS_LOG_HL("user_file.version = 0x%0x", cias_ota_data->file_info.user_file.version);
	CIAS_LOG_HL("user_file.address = 0x%0x", cias_ota_data->file_info.user_file.address);
	CIAS_LOG_HL("user_file.size = 0x%0x", cias_ota_data->file_info.user_file.size);
	CIAS_LOG_HL("user_file.crc = 0x%0x", cias_ota_data->file_info.user_file.crc);
	CIAS_LOG_HL("user_file.status = 0x%0x", cias_ota_data->file_info.user_file.status);

	CIAS_LOG_HL("ConsumerDataStartAddr = 0x%0x", cias_ota_data->file_info.ConsumerDataStartAddr);
	CIAS_LOG_HL("ConsumerDataSize = 0x%0x", cias_ota_data->file_info.ConsumerDataSize);
	CIAS_LOG_HL("PartitionTableChecksum = 0x%0x", cias_ota_data->file_info.PartitionTableChecksum);
	// CIAS_LOG_HL("ota_statue = 0x%0x\r\n", cias_ota_data->file_info.ota_statue);
#endif
	return 1;
}

/**
 * @brief 通过此函数进行从云端拉取OTA升级数据包
 *
 * @retval -1: error
 * @retval 0: successful
 */
static int cias_get_cloud_data_task()
{
	get_cias_ota_handle()->cias_net_portocol_init();//cias_net_protocol_init();   //todo:初始化网络通信协议

	int get_image_len = 0;
	do
	{
		if(ota_update_flag.data_control == ENABLE_CLOUD_GET)
		{
			get_image_len = get_cias_ota_handle()->cias_get_audio_image(test_ota_update_buffer, ota_update_flag.offset_in_file + write_data_offset,\
			((ota_update_flag.partions_size) - write_data_offset >= OTA_CLOUD_SIZE)?OTA_CLOUD_SIZE : ((ota_update_flag.partions_size) - write_data_offset));

			if(get_image_len == -1)
			{
	#if SUBMIT_TEST
				#if 1
				hal_gpio_pin_afio_en(GPIOB_BASE, GPIO_PIN_4, HAL_ENABLE);
   				gpio_init_t_def gpio_init;
   				memset(&gpio_init, 0, sizeof(gpio_init)); //清零结构体
   				gpio_init.dir = GPIO_OUTPUT;               //配置GPIO方向，输入
   				gpio_init.pin = GPIO_PIN_4;               //配置GPIO引脚号
   				gpio_init.speed = GPIO_HIGH_SPEED;        //设置GPIO速度
   				hal_gpio_init(GPIOB_BASE, &gpio_init);    //初始化GPIO
   				hal_gpio_pin_set(GPIOB_BASE, GPIO_PIN_4);  //拉高
   				get_cias_ota_handle()->cias_ota_task_delay_ms(1);
   				hal_gpio_pin_set(GPIOB_BASE, GPIO_PIN_4);
				#else
				/*LN io PE* 输出高电平*/
				gpio_init_t_def gpio_init;
    			memset(&gpio_init,0,sizeof(gpio_init)); 
				gpio_init.dir   = GPIO_OUTPUT;                  //配置GPIO方向，输入或者输出
    			gpio_init.pin   = GPIO_PIN_9;                  //配置GPIO引脚号
    			gpio_init.speed = GPIO_HIGH_SPEED;             //设置GPIO速度
    			gpio_init.mode  = GPIO_MODE_DIGITAL;           //设置引脚为数字引脚
    			gpio_init.pull  = GPIO_PULL_NONE;                //设置引脚为上拉
    			hal_gpio_init(GPIOA_BASE,&gpio_init);
				#endif
	#endif//SUBMIT_TEST
				get_cias_ota_handle()->cias_report_ota_status_to_iot(1);   
				get_cias_ota_handle()->cias_ota_task_delay_ms(2);
				get_cias_ota_handle()->cias_report_ota_status_to_iot(1);
				LOG(LOG_LVL_INFO, "\r\n======report_ota_status:failed===========");
				LOG(LOG_LVL_INFO, "\r\n>>>ERROR:get_audio_image_from_cloud(), reset all!!!");
				get_cias_ota_handle()->cias_ota_task_delay_ms(10);
				get_cias_ota_handle()->cias_audio_device_reboot();//cias_ln_audio_reset();
				get_cias_ota_handle()->cias_ota_task_delay_ms(2);//cias_task_delay_ms(2);
				get_cias_ota_handle()->cias_wifi_device_reboot();//ln_chip_reboot();
			}
			ota_update_flag.data_control = ENABLE_PACKAGE_SEND;
		}
		get_cias_ota_handle()->cias_ota_task_delay_ms(2);//cias_task_delay_ms(2);
	} while(1);

	return 0;
}

static void ci_ota_progress_task(void *params)
{
    int ota_ret = 0;
    if(!(get_cias_ota_handle()->cias_net_data_struct_init()))
    {
		if(!(cias_ota_get_partitions_info()))//获取分区表信息
		{
			#if 1
			get_cias_ota_handle()->cias_report_ota_status_to_iot(1);
			get_cias_ota_handle()->cias_ota_task_delay_ms(2);
			get_cias_ota_handle()->cias_report_ota_status_to_iot(1);
			LOG(LOG_LVL_INFO, "\r\n======report_ota_status:failed===========\r\n");
			#endif

            get_cias_ota_handle()->cias_audio_device_reboot();
			get_cias_ota_handle()->cias_ota_task_delay_ms(2);
			get_cias_ota_handle()->cias_wifi_device_reboot();
        }
        while (1)
		{
			LOG(LOG_LVL_INFO, "\r\n>>>进入OTA升级...\r\n");
			ota_ret = update_ota_main(); 
            if (!ota_ret)
			{
				LOG(LOG_LVL_INFO, "\r\n>>>OTA升级完成!\r\n");
				ota_update_flag.data_control = OTA_FINISHED;
				#if 1
				get_cias_ota_handle()->cias_report_ota_status_to_iot(4);
				get_cias_ota_handle()->cias_ota_task_delay_ms(2);
				get_cias_ota_handle()->cias_report_ota_status_to_iot(4);
				LOG(LOG_LVL_INFO, "\r\n======report_ota_status:success===========\r\n");
				#endif
			}
			else
			{
				LOG(LOG_LVL_INFO, "\r\n>>>Some errors have occurred and the upgrade is over\r\n");
				#if 1
				if(ota_ret != 19)
				{
					get_cias_ota_handle()->cias_report_ota_status_to_iot(1);
					get_cias_ota_handle()->cias_ota_task_delay_ms(2);
					get_cias_ota_handle()->cias_report_ota_status_to_iot(1);
					LOG(LOG_LVL_INFO, "\r\n======report_ota_status:failed===========\r\n");
				}
				#endif
                switch(ota_ret)
                {
                    case AUDIO_NO_ANSWER://设备不在线
                    {
		                LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Device is no answer !");
                        break;
                    }
                    case UPDATER_INFO_SEND_ERROR://发送升级代理程序信息失败
                    {
		                LOG(LOG_LVL_INFO, "\r\n>>>ERROR: updaetr info send fail !");
                        break;
                    }
                    case UPDATER_SEND_FAIL://发送升级代理程序失败
                    {
		                LOG(LOG_LVL_INFO, "\r\n>>>ERROR: updaetr send fail !");
                        break;
                    }
                    case UPDATER_VERIFY_FAIL://升级代理程序校验失败
                    {
		                LOG(LOG_LVL_INFO, "\r\n>>>ERROR: updaetr verify fail !");
                        break;
                    }
                    case AUDIO_BAUD_SET_FAIL://audio波特率设置失败
                    {
			            LOG(LOG_LVL_INFO, "\r\n>>>ERROR: audio baud set failed ! -- updater");
                        break;
                    }
                    case AUDIO_NO_ANSWER_AFTER_CHANGE_BAUD://audio波特率更改后无响应
                    {
            			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: audio is no answer after change baudrate ! -- updater");
                        break;                                            
                    }
                    case SET_AUDIO_FIRMWARE_VER_FAIL://设置固件版本格式失败
                    {
            			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: set audio firmware vesion failed ! -- updater");
                        break;
                    }
                    case MALLOC_FAIL:
                    {
            			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Malloc failed !");
                        break;
                    }
                    case READ_AUDIO_PARTITION_FAIL://读取audio分区表信息失败
                    {
            			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Partition in audio read failed !");
                        break;
                    }
                    case AUDIO_PARTITION_INVALID://读取audio分区表信息无效
                    {
            			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: read audio partition invalid !");
                        break;
                    }
                    case PARTITION_1_UPDATE_FAIL://分区表1更新失败
                    {
				        LOG(LOG_LVL_INFO, "\r\n>>>ERROR: update partition list1 failed");
                        break;
                    }
                    case PARTITION_2_UPDATE_FAIL://分区表2更新失败
                    {
        				LOG(LOG_LVL_INFO, "\r\n>>>ERROR: update partition list2 failed");
                        break;
                    }
                    case USER_CODE_1_UPDATE_FAIL://user code分区更新失败
                    {
        				LOG(LOG_LVL_INFO, "\r\n>>>ERROR: update user code 1 failed");
                        break;
                    }
                    case USER_CODE_2_UPDATE_FAIL://第二份user code分区更新失败
                    {
        				LOG(LOG_LVL_INFO, "\r\n>>>ERROR: update another user code 2 failed");
                        break;
                    }
                    case ASR_UPDATE_FAIL:
                    {
				        LOG(LOG_LVL_INFO, "\r\n>>>ERROR: update asr failed");
                        break;
                    }
                    case DNN_UPDATE_FAIL:
                    {
				        LOG(LOG_LVL_INFO, "\r\n>>>ERROR: update dnn failed");
                        break;
                    }
                    case VOICE_UPDATE_FAIL:
                    {
				        LOG(LOG_LVL_INFO, "\r\n>>>ERROR: update voice failed");
                        break;
                    }
                    case USERFILE_UPDATE_FAIL:
                    {
				        LOG(LOG_LVL_INFO, "\r\n>>>ERROR: update user_file failed");
                        break;
                    }
					case NO_PARTITION_NEED_UPDATE:
					{
				        LOG(LOG_LVL_INFO, "\r\n>>> no partition need update");
						#if 1
						get_cias_ota_handle()->cias_report_ota_status_to_iot(5);
						get_cias_ota_handle()->cias_ota_task_delay_ms(2);
						get_cias_ota_handle()->cias_report_ota_status_to_iot(5);

						LOG(LOG_LVL_INFO, "\r\n======report_ota_status:same version===========\r\n");
						#endif
						break;
					}
                }
            }
			LOG(LOG_LVL_INFO, "\r\n>>>复位audio端和wifi端!\r\n\r\n");
			get_cias_ota_handle()->cias_ota_task_delay_ms(20);
			get_cias_ota_handle()->cias_audio_device_reboot();
			get_cias_ota_handle()->cias_ota_task_delay_ms(2);
			get_cias_ota_handle()->cias_wifi_device_reboot();
		}
	}
    else
    {
		LOG(LOG_LVL_INFO, "\r\n======report_ota_status:failed===========\r\n");
		get_cias_ota_handle()->cias_report_ota_status_to_iot(1);
		get_cias_ota_handle()->cias_ota_task_delay_ms(2);
		get_cias_ota_handle()->cias_report_ota_status_to_iot(1);

        LOG(LOG_LVL_INFO,"\r\n>>>ERROR: cias_ota_struct_init, all reset!\r\n\r\n");
        get_cias_ota_handle()->cias_audio_device_reboot();
        get_cias_ota_handle()->cias_ota_task_delay_ms(2);
        get_cias_ota_handle()->cias_wifi_device_reboot();
    }
}

int cias_get_cloud_data_main(void)
{
	cias_task_t ota_get_cloud_data_handle = {
		.handle = NULL,
	};
	cias_task_create(&ota_get_cloud_data_handle,
					 AUDIO_OTA_GET_FIRMWARE_TASK_NAME,
					 (cias_task_function_t)cias_get_cloud_data_task,
					 (void *)0,
					 AUDIO_OTA_TASK_PRIORITY,
					 (unsigned short)AUDIO_OTA_TASK_SIZE);
}

static void cias_ota_progress_task(void)
{
	cias_task_t ota_main_task_handle = {
		.handle = NULL,
	};
	cias_task_create(&ota_main_task_handle,
					AUDIO_OTA_PROGRESS_TASK_NAME,
					(cias_task_function_t)ci_ota_progress_task,
					(void *)0,
					AUDIO_OTA_TASK_PRIORITY,
					(unsigned short)AUDIO_OTA_TASK_SIZE);
}

extern cias_system_manage_param_t g_cias_system_manage_param;
static void cias_ota_task(void *parameter)
{
	g_cias_system_manage_param.ota_start_flag = 0;
    g_cias_system_manage_param.recv_slave_msg_deal_flag = 0;
    g_cias_system_manage_param.qcloud_main_handle.handle = NULL;
    g_cias_system_manage_param.recv_slave_msg_deal_handle.handle = NULL;
	
	while (1)
    {
        if (g_cias_system_manage_param.ota_start_flag == 1) //升级任务开始，删除多余的任务
        {
            g_cias_system_manage_param.ota_start_flag = 0;
            extern cias_queue_t cias_recv_slave_msg_queue;
            extern cias_queue_t cias_send_slave_msg_queue;
            cias_queue_delete(&cias_send_slave_msg_queue);   //删除队列
            cias_queue_delete(&cias_recv_slave_msg_queue);  //删除队列
            
			if (g_cias_system_manage_param.recv_slave_msg_deal_handle.handle)
                cias_task_delete(&g_cias_system_manage_param.recv_slave_msg_deal_handle);
            if (g_cias_system_manage_param.recv_slave_msg_task_handle.handle)
                cias_task_delete(&g_cias_system_manage_param.recv_slave_msg_task_handle);
            if (g_cias_system_manage_param.send_slave_msg_task_handle.handle)
                 cias_task_delete(&g_cias_system_manage_param.send_slave_msg_task_handle);
            cias_task_delay_ms(100);
            // CIAS_LOG_HL("cias_system_manage_task********************>>>>>Heap left: %d <1> min:%d<<<<<********************\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
            cias_get_cloud_data_main();
            cias_task_delay_ms(2);
            cias_ota_progress_task();
        }
		else
		{
        	cias_task_delay_ms(100);
		}
        // CIAS_LOG_HL("%d-----cias_system_manage_task********************>>>>>Heap left: %d <1> min:%d<<<<<********************, count = %d\r\n",g_cias_system_manage_param.ota_start_flag, xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize(), count);
    }
}
int cias_ota_main(void)
{
    if( cias_ota_callback_init() != CIAS_OK )//注册OTA回调函数
	{
		return CIAS_FAIL;
	}

	cias_task_t ota_main_handle = {
		.handle = NULL,
	};
	cias_task_create(&ota_main_handle,
					AUDIO_OTA_TASK_NAME,
					(cias_task_function_t)cias_ota_task,
					(void *)0,
					AUDIO_OTA_TASK_PRIORITY,
					(unsigned short)1024);
	return CIAS_OK;
}

