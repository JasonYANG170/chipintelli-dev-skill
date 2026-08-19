#include "data_template_client.h"
#include "cias_qcloud_main.h"
#include "qcloud_iot_export.h"
#include "cias_qcloud_ota.h"
#include "cias_common.h"
#include "cias_workmode.h"
#include "cias_slave_message_handle.h"
#include "cias_qcloud_iot.h"
#include "cias_user_config.h"
#include "cias_freertos_debug.h"
#include "cias_log.h"
#include "cias_qcloud_dev.h"
#include "cias_qcloud_user.h"
#include "qcloud_iot_export_log.h"
#include "cias_freertos_task.h"
#include "cias_flash_update.h"
#include "cias_wifi_port.h"
#include "cias_system_manage.h"
#include "update.h"
#include "Config.h"
#include "ota_client.h"
#include "proj_config.h"
#include "ota_err.h"
#if CIAS_AIOT_WIFI_OTA_ENABLE
#include "cias_wifi_http_ota.h"
#include "reboot_trace.h"
#endif
#include "cias_demo_config.h"
#include <utils/system_parameter.h>
#include "ln_kv_key_def.h"
#include "cias_ota_config.h"

#define FW_VERSION_MAX_LEN 32
#define FW_MD5SUM_MAX_LEN 64
#define FW_FILE_PATH_MAX_LEN 128
//#define OTA_BUF_LEN 5000
#define OTA_BUF_LEN 4096 + 1
#define FW_INFO_FILE_DATA_LEN 128
#define SG_DATA_REPORT_BUFFERSIZE 260 // max 260

DeviceInfo sg_dev_info;
bool sg_control_msg_arrived = false;
bool sg_ota_msg_arrived = false;
static char sg_data_report_buffer[SG_DATA_REPORT_BUFFERSIZE];
Qcloud_IoT_Template *g_iot_template_client = NULL;
void *ota_client = NULL;
void *h_ota = NULL;
OTAContextData *ota_ctx;
#if CIAS_AIOT_AUDIO_OTA_ENABLE
extern update_result_param_info_st gupdate_result_param_info; // and by yjd
#endif//CIAS_AIOT_AUDIO_OTA_ENABLE
extern sDataPoint sg_data_template[TOTAL_PROPERTY_COUNT];
extern cias_system_manage_param_t g_cias_system_manage_param;
static void event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg)
{
	uintptr_t packet_id = (uintptr_t)msg->msg;

	switch (msg->event_type)
	{
	case MQTT_EVENT_UNDEF:
		CIAS_LOG_INFO("[MQTT]undefined event occur.");
		break;

	case MQTT_EVENT_DISCONNECT:
		CIAS_LOG_INFO("[MQTT]MQTT disconnect.");
		break;

	case MQTT_EVENT_RECONNECT:
		CIAS_LOG_INFO("[MQTT]MQTT reconnect.");
		break;

	case MQTT_EVENT_SUBCRIBE_SUCCESS:
		CIAS_LOG_INFO("[MQTT]subscribe success, packet-id=%u", packet_id);
		break;

	case MQTT_EVENT_SUBCRIBE_TIMEOUT:
		CIAS_LOG_INFO("[MQTT]subscribe wait ack timeout, packet-id=%u", packet_id);
		break;

	case MQTT_EVENT_SUBCRIBE_NACK:
		CIAS_LOG_INFO("[MQTT]subscribe nack, packet-id=%u", packet_id);
		break;

	case MQTT_EVENT_PUBLISH_SUCCESS:
		CIAS_LOG_INFO("[MQTT]publish success, packet-id=%u", (unsigned int)packet_id);
		break;

	case MQTT_EVENT_PUBLISH_TIMEOUT:
		CIAS_LOG_INFO("[MQTT]publish timeout, packet-id=%u", (unsigned int)packet_id);
		break;

	case MQTT_EVENT_PUBLISH_NACK:
		CIAS_LOG_INFO("[MQTT]publish nack, packet-id=%u", (unsigned int)packet_id);
		break;
	default:
		CIAS_LOG_INFO("[MQTT]Should NOT arrive here.");
		break;
	}
}
// Setup MQTT construct parameters
static int _setup_connect_init_params(TemplateInitParams *init_params, void *ota_ctx)
{
	int ret;

	ret = HAL_GetDevInfo((void *)&sg_dev_info); // remove by yjd
	if (QCLOUD_RET_SUCCESS != ret)
	{
		return ret;
	}

	init_params->device_name = sg_dev_info.device_name;
	init_params->product_id = sg_dev_info.product_id;
#ifdef AUTH_MODE_CERT
	/* TLS with certs*/
	char certs_dir[PATH_MAX + 1] = "certs";
	char current_path[PATH_MAX + 1];
	char *cwd = getcwd(current_path, sizeof(current_path));
	if (cwd == NULL)
	{
		CIAS_LOG_ERR("getcwd return NULL");
		return QCLOUD_ERR_FAILURE;
	}
	sprintf(sg_cert_file, "%s/%s/%s", current_path, certs_dir, sg_dev_info.dev_cert_file_name);
	sprintf(sg_key_file, "%s/%s/%s", current_path, certs_dir, sg_dev_info.dev_key_file_name);

	init_params->cert_file = sg_cert_file;
	init_params->key_file = sg_key_file;
#else
	init_params->device_secret = sg_dev_info.device_secret;
#endif

	init_params->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	init_params->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;
	init_params->auto_connect_enable = 1;
	init_params->event_handle.h_fp = event_handler;
	init_params->event_handle.context = ota_ctx;
	return QCLOUD_RET_SUCCESS;
}

static void on_report_reply_callback(void *pclient, Method method, ReplyAck reply_ack, const char *pjson_document, void *puser_data)
{

	CIAS_LOG_INFO("recv report reply response, reply ack: %d", reply_ack);
	// if (replyAck == 0)
	// {
	//   gsystem_work_mode_info.send_elc_count++;
	// }
}

/*find propery need report*/
static int find_wait_report_property(DeviceProperty *preport_data_list[])
{
	int i, j;

	for (i = 0, j = 0; i < TOTAL_PROPERTY_COUNT; i++)
	{
		if (eCHANGED == sg_data_template[i].data_property.state)
		{
			preport_data_list[j++] = &(sg_data_template[i].data_property);
			sg_data_template[i].data_property.state = eNOCHANGE;
		}
	}

	return j;
}

// demo for up-stream
// add changed properties to preport_data_list, then the changed properties would be reported
// you should add your own logic for how to get the changed properties
int deal_up_stream_user_logic(DeviceProperty *preport_data_list[], int *pcount)
{
	/*find propery need report*/
	*pcount = find_wait_report_property(preport_data_list);

	return (*pcount > 0) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

// publish MQTT msg
static int _publish_token_msg(void *mqtt_client, DeviceInfo *dev_info, char *token_str)
{
	char *procuct_id = ci_mqtt_get_product_id();
	char *device_name = ci_mqtt_get_device_name();

	char topic_name[128] = {0};
	int size = HAL_Snprintf(topic_name, sizeof(topic_name), "$thing/up/service/%s/%s", procuct_id, device_name);
	if (size < 0 || size > sizeof(topic_name) - 1)
	{
		CIAS_LOG_ERR("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
		return QCLOUD_ERR_FAILURE;
	}
	PublishParams pub_params = DEFAULT_PUB_PARAMS;
	pub_params.qos = QOS1;

	char topic_content[256] = {0};
	size = HAL_Snprintf(topic_content, sizeof(topic_content),
						"{\"method\":\"app_bind_token\",\"clientToken\":\"%s-%u\",\"params\": {\"token\":\"%s\"}}",
						device_name, HAL_GetTimeMs(), token_str);
	if (size < 0 || size > sizeof(topic_content) - 1)
	{
		CIAS_LOG_ERR("payload content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_content));
		return QCLOUD_ERR_MALLOC;
	}

	pub_params.payload = topic_content;
	pub_params.payload_len = strlen(topic_content);

	return IOT_MQTT_Publish(mqtt_client, topic_name, &pub_params);
}
/* get local firmware offset for resuming download from break point */
static int _update_fw_downloaded_size(OTAContextData *ota_ctx)
{
	int local_size = 0;
	/*local_size = _get_local_fw_info(ota_ctx->fw_info_file_path, ota_ctx->local_version);  //remove by yjd
	if (local_size == 0)
	{
	  ota_ctx->downloaded_size = 0;
	  return 0;
	}*/

	if ((0 != strcmp(ota_ctx->local_version, ota_ctx->remote_version)) ||
		(ota_ctx->downloaded_size > ota_ctx->fw_file_size))
	{
		ota_ctx->downloaded_size = 0;
		return 0;
	}

	ota_ctx->downloaded_size = local_size;
	CIAS_LOG_INFO("calc MD5 for resuming download from offset: %d", ota_ctx->downloaded_size);
	return local_size;
}

int mqtt_send_token(char *sg_token_str)
{
	int ret = 0;
	int wait_cnt = 3;
	Qcloud_IoT_Template *ptemplate = (Qcloud_IoT_Template *)g_iot_template_client;

publish_token:
	ret = _publish_token_msg(ptemplate->mqtt, NULL, sg_token_str);
	if (ret < 0 && (wait_cnt-- > 0))
	{
		CIAS_LOG_ERR("client publish token failed: %d", ret);
		if (IOT_MQTT_IsConnected(ptemplate->mqtt))
		{
			IOT_MQTT_Yield(ptemplate->mqtt, 500);
		}
		else
		{
			CIAS_LOG_ERR("MQTT lost connection! Wait and retry!");
			IOT_MQTT_Yield(ptemplate->mqtt, 2000);
		}
		goto publish_token;
	}

	return ret;
}
DeviceInfo *IOT_MQTT_GetDeviceInfo(void *pclient)
{
	POINTER_SANITY_CHECK(pclient, NULL);
	Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pclient;
	return &mqtt_client->device_info;
}

//上报升级状态到云端
uint8_t report_ota_status_to_iot(uint8_t status)
{
	DeviceInfo *device_info = IOT_MQTT_GetDeviceInfo(ota_ctx->mqtt_client);
	CIAS_LOG_HL("device_info->client_id = %s, ota_ctx->remote_version = %s\r\n", device_info->client_id, ota_ctx->remote_version);
	switch (status)
	{
		case 1://add by hw 
			IOT_OTA_ReportUpgradeFail(ota_ctx->ota_handle, ota_ctx->remote_version);
			break;
		case IOT_OTAR_UPGRADE_SUCCESS: //更新成功
			CIAS_LOG_HL("firmware update ok ....\r\n");
			IOT_OTA_ReportUpgradeSuccess(ota_ctx->ota_handle, ota_ctx->remote_version);
			break;
		case IOT_OTAR_NO_NEED_UPGRADE: //重复版本，不需要更新
			CIAS_LOG_HL("no need upgrade ....\r\n");
			IOT_OTA_ReportNoNeedUpgrade(ota_ctx->ota_handle, ota_ctx->remote_version);
			break;
		case IOT_OTAR_UPGRADE_FAIL: //更新失败
			CIAS_LOG_HL("firmware update error ....\r\n");
			IOT_OTA_ReportUpgradeFail(ota_ctx->ota_handle, ota_ctx->remote_version);
			break;
		default:
			break;
	}
}

//从腾讯云平台获取wifi 
#include "cias_wifi_ota_config.h"
int get_wifi_image_from_cloud(unsigned char *buff, unsigned int offset, unsigned int len)
{
	int rc = -1;
	int down_len = 0;
	int try_fail_count = 0;

wifi_img_down_load:
	rc = IOT_OTA_StartDownload(h_ota, offset, offset + len);
	if (QCLOUD_RET_SUCCESS != rc)
	{
		CIAS_LOG_HL("\r\n>>>download start err,rc:%d\r\n", rc);
		return -1;
	}
	down_len = IOT_OTA_FetchYield(h_ota, buff, WIFI_OTA_SECTOR_SIZE_4KB + 1, 3); //每次最多只能下载OTA_BUF_LEN-1个字节
	if (down_len < 0)
	{
		CIAS_LOG_HL("\r\n>>>download fail down_len=%d\r\n", down_len);
		return -1;
	}
	if ((offset + len) > ota_ctx->fw_file_size) //由于4K对齐，导致取到文件末尾还不比len小，这时候填充满4K
	{
		return len;
	}
	if (down_len < len)
	{
		// CIAS_LOG_HL("1-download fail down_len=%d\r\n", down_len);
		if (try_fail_count++ > 100)
		{
			return -1;
		}
		cias_task_delay_ms(50);
		goto wifi_img_down_load;
	}
	return len;
}

//从云端获取audio端音频数据
int get_audio_image_from_cloud(unsigned char *buff, unsigned int offset, unsigned int len)
{
	int rc = -1;
	int down_len = 0;
	int try_fail_count = 0;

img_down_load:
	rc = IOT_OTA_StartDownload(h_ota, offset, offset + len); //获取分区表信息
	if (QCLOUD_RET_SUCCESS != rc)
	{
		CIAS_LOG_HL("OTA download start err,rc:%d", rc);
		return -1;
	}
	down_len = IOT_OTA_FetchYield(h_ota, buff, OTA_CLOUD_SIZE + 1, 3); //每次最多只能下载OTA_BUF_LEN-1个字节
	if (down_len < 0)
	{
		CIAS_LOG_HL("3-download fail down_len=%d\r\n", down_len);
		return -1;
	}
	if ((offset + len) > ota_ctx->fw_file_size) //由于4K对齐，导致取到文件末尾还不比len小，这时候填充满4K
	{
		return len;
	}
	if (down_len < len)
	{
		// CIAS_LOG_HL("1-download fail down_len=%d\r\n", down_len);
		if (try_fail_count++ > 100)
		{
			return -1;
		}
		cias_task_delay_ms(50);
		goto img_down_load;
	}
	return len;
}

#if TENCENT_AP_NET_WORK_EN//add by hw
extern int wifi_softAP_connect_flag;
extern int ip_addr_get_status;
#endif//TENCENT_AP_NET_WORK_EN
volatile int ota_task_flag = 0;//modify by houwang
void ci_iot_tencent_main_task(void *parameter)
{
	int rc;
	reply_para_st reply_para;
	DeviceProperty *preport_data_list[TOTAL_PROPERTY_COUNT];
	int report_cont;
	int construct_failed_count = 0;
	CIAS_LOG_HL("ci_iot_tencent_main_task is exec ... \r\n");
	ota_ctx = (OTAContextData *)HAL_Malloc(sizeof(OTAContextData));
	if (ota_ctx == NULL)
	{
		CIAS_LOG_ERR("ota_ctx malloc failed\r\n");
	}
	memset(ota_ctx, 0, sizeof(OTAContextData));

	_init_data_template();

#if TENCENT_AP_NET_WORK_EN//add by hw
	while(wifi_softAP_connect_flag || (!ip_addr_get_status))
	{
		#if AP_NET_DEBUG
		CIAS_LOG_HL("wifi_softAP_connect_flag:%d, ip_addr_get_status:%d, wait... \r\n",wifi_softAP_connect_flag,ip_addr_get_status);
		#endif
		cias_task_delay_ms(1000);
	}
#endif//TENCENT_AP_NET_WORK_EN

	while (1)
	{
		cias_task_delay_ms(1000);
		if (cias_get_wifi_sta_connect_state() == NETDEV_LINK_UP) //连上路由
		{
			cias_task_delay_ms(5000);//add bu houwang
			CIAS_LOG_HL("qcloud try connect cloud\n");
#if 0
			//char *ota_url = "https://ota-1255858890.cos.ap-guangzhou.myqcloud.com/100015781777_ZDFVDMWJWB_vb_001?sign=q-sign-algorithm%3Dsha1%26q-ak%3DAKIDdO8ldrUa0Uts4H5Gzx6FZ9nfedjpiCd7%26q-sign-time%3D1654591836%3B1654678236%26q-key-time%3D1654591836%3B1654678236%26q-header-list%3D%26q-url-param-list%3D%26q-signature%3D942d378d15ee05aa8820888d26587498ae1a8410";
		    char *ota_url = "https://ota-1255858890.cos.ap-guangzhou.myqcloud.com/100015781777_ZDFVDMWJWB_vb_002?sign=q-sign-algorithm%3Dsha1%26q-ak%3DAKIDdO8ldrUa0Uts4H5Gzx6FZ9nfedjpiCd7%26q-sign-time%3D1654653957%3B1654740357%26q-key-time%3D1654653957%3B1654740357%26q-header-list%3D%26q-url-param-list%3D%26q-signature%3D9cc2589223904c7dc3a95efb495bd867e1280c0c";
			wifi_http_ota_start(ota_url);
			while(1)
			{
				cias_task_delay_ms(2000);
			}
#endif
#if 0
			//char *ota_url = "https://ota-1255858890.cos.ap-guangzhou.myqcloud.com/100015781777_ZDFVDMWJWB_vb_001?sign=q-sign-algorithm%3Dsha1%26q-ak%3DAKIDdO8ldrUa0Uts4H5Gzx6FZ9nfedjpiCd7%26q-sign-time%3D1654591836%3B1654678236%26q-key-time%3D1654591836%3B1654678236%26q-header-list%3D%26q-url-param-list%3D%26q-signature%3D942d378d15ee05aa8820888d26587498ae1a8410";
		    // char *ota_url = "http://192.168.0.198:8080/wifi-ota-flashimage02.bin";
			unsigned char *ota_url = "http://192.168.3.139:8080/flashimage-ota-xz-v0.1-2022072501.bin";
			CIAS_LOG_HL("\r\n============delay 6s=================\r\n");
			cias_task_delay_ms(6000);//add by houwang
			CIAS_LOG_HL("\r\n============wifi ota test start=================\r\n");
			wifi_http_ota_start(ota_url);
			while(1)
			{
				// CIAS_LOG_HL("\r\n=================delay 2s=================\r\n");
				cias_task_delay_ms(2000);
			}
#endif
			// init connection
			TemplateInitParams init_params = DEFAULT_TEMPLATE_INIT_PARAMS;
			rc = _setup_connect_init_params(&init_params, ota_ctx);
			if (rc != QCLOUD_RET_SUCCESS)
			{
				CIAS_LOG_ERR("init params err,  rc=%d", rc);
				continue;
			}

			CIAS_LOG_HL("ci_iot_tencent_main_task:********************>>>>>Heap left: %d <1> min:%d<<<<<********************\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
			g_iot_template_client = (Qcloud_IoT_Template *)IOT_Template_Construct(&init_params, NULL);
			if (g_iot_template_client == NULL)
			{
				CIAS_LOG_ERR("Cloud Device Construct Failed");
				if(construct_failed_count++ > 5)
					cias_system_reboot();
				continue;
			}
			construct_failed_count = 0;
#if CIAS_AIOT_AUDIO_OTA_ENABLE || CIAS_AIOT_WIFI_OTA_ENABLE
			// init OTA handle
			CIAS_LOG_HL("IOT_OTA_Init:(%s)(%s)*******************\r\n", sg_dev_info.product_id, sg_dev_info.device_name);
			h_ota = IOT_OTA_Init(sg_dev_info.product_id, sg_dev_info.device_name, g_iot_template_client->mqtt);
			if (NULL == h_ota)
			{
				CIAS_LOG_ERR("--------------initialize OTA failed----------------");
				cias_system_reboot();
				//continue;
			}

			ota_ctx->ota_handle = h_ota;
			ota_ctx->mqtt_client = g_iot_template_client->mqtt;
			Qcloud_IoT_Client *m_mqtt = (Qcloud_IoT_Client *)ota_ctx->mqtt_client;
			memcpy(&m_mqtt->device_info, &sg_dev_info, sizeof(DeviceInfo));
#endif
#if TENCENT_AP_NET_WORK_EN//add by hw			
			if (cias_get_qcloud_iot_network_token() != NULL)
            {
				#if AP_NET_DEBUG
                #endif
				rc = mqtt_send_token((char *)cias_get_qcloud_iot_network_token());
                if (rc == QCLOUD_RET_SUCCESS)
                {
                    CIAS_LOG_DEBUG("AP: mqtt send token success\n");
                }
                else
                {
                    CIAS_LOG_ERR("AP: mqtt send token fail, err: %d", rc);
                }
				#if AP_NET_DEBUG
				CIAS_LOG_HL("token-------------------------2,%s\r\n",cias_get_qcloud_iot_network_token());
				#endif
				cias_free_qcloud_iot_network_token();
				#if AP_NET_DEBUG
				CIAS_LOG_HL("token-------------------------3,%s\r\n",cias_get_qcloud_iot_network_token());
				#endif
			}
#endif//TENCENT_AP_NET_WORK_EN			
			// get the property changed during offline
			rc = IOT_Template_GetStatus_sync(g_iot_template_client, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
			if (rc != QCLOUD_RET_SUCCESS)
			{
				CIAS_LOG_ERR("Get data status fail, err: %d", rc);
			}
			else
			{
				CIAS_LOG_DEBUG("Get data status success");
			}
			//_ota_callback_local(ota_ctx->ota_handle); // and by yjd		
			while (IOT_Template_IsConnected(g_iot_template_client) || rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT || rc == QCLOUD_RET_MQTT_RECONNECTED || QCLOUD_RET_SUCCESS == rc)
			{
				rc = IOT_Template_Yield(g_iot_template_client, 200);
				if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT)
				{
					CIAS_LOG_WARN("MQTT attempting reconnect...");
					cias_task_delay_ms(1000);
					continue;
				}
				else if (rc != QCLOUD_RET_SUCCESS)
				{
					CIAS_LOG_ERR("Exit loop caused of errCode: %d", rc);
				}

				/* handle control msg from server */
				if (sg_control_msg_arrived)
				{
					/* control msg should reply, otherwise server treat device didn't receive and retain the msg which would be get by  get status*/
					memset((char *)&reply_para, 0, sizeof(reply_para_st));
					reply_para.code = eDEAL_SUCCESS;
					reply_para.timeout_ms = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
					reply_para.status_msg[0] = '\0'; // add extra info to reply_para.status_msg when error occured

					rc = IOT_Template_ControlReply(g_iot_template_client, sg_data_report_buffer, SG_DATA_REPORT_BUFFERSIZE, &reply_para);
					if (rc == QCLOUD_RET_SUCCESS)
					{
						CIAS_LOG_DEBUG("Contol msg reply success");
						sg_control_msg_arrived = false;
					}
					else
					{
						CIAS_LOG_ERR("Contol msg reply failed, err: %d", rc);
					}
					CIAS_LOG_HL("deal_down_stream is exec .....\r\n");
					deal_down_stream();
				}
#if CIAS_AIOT_AUDIO_OTA_ENABLE || CIAS_AIOT_WIFI_OTA_ENABLE
				// recv the upgrade cmd
				if (IOT_OTA_IsFetching(h_ota) && (ota_task_flag != FIRMWARE_UPDATE_ING))
				{
					IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &ota_ctx->fw_file_size, 4);
					IOT_OTA_Ioctl(h_ota, IOT_OTAG_VERSION, ota_ctx->remote_version, FW_VERSION_MAX_LEN);
					IOT_OTA_Ioctl(h_ota, IOT_OTAG_FETCHED_SIZE, &ota_ctx->fetched_size, 4);
					IOT_OTA_Ioctl(h_ota, IOT_OTAG_MD5SUM, ota_ctx->md5sum, 33);
					IOT_OTA_Ioctl(h_ota, IOT_OTAG_CHECK_FIRMWARE, ota_ctx->remote_version, FW_VERSION_MAX_LEN);
					DeviceInfo *device_info = IOT_MQTT_GetDeviceInfo(ota_ctx->mqtt_client);
					CIAS_LOG_HL("device_info->client_id = %s, ota_ctx->remote_version = %s\r\n", device_info->client_id, ota_ctx->remote_version);
					CIAS_LOG_HL("ota_ctx->fw_file_size = %d, ota_ctx->fetched_size = %d\r\n", ota_ctx->fw_file_size, ota_ctx->fetched_size);
					CIAS_LOG_HL("ota_ctx->md5sum = %s\r\n", ota_ctx->md5sum);
					CIAS_LOG_HL("((OTA_Struct_t *)h_ota)->purl = %s\r\n", ((OTA_Struct_t *)h_ota)->purl);
					HAL_Snprintf(ota_ctx->fw_file_path, FW_FILE_PATH_MAX_LEN, "./FW_%s_%s.bin",
								 STRING_PTR_PRINT_SANITY_CHECK(device_info->client_id),
								 STRING_PTR_PRINT_SANITY_CHECK(ota_ctx->remote_version));
					HAL_Snprintf(ota_ctx->fw_info_file_path, FW_FILE_PATH_MAX_LEN, "./FW_%s.json",
								 STRING_PTR_PRINT_SANITY_CHECK(device_info->client_id));
					CIAS_LOG_HL("ota_ctx->fw_file_path = %s, ota_ctx->fw_info_file_path = %s\r\n", ota_ctx->fw_file_path, ota_ctx->fw_info_file_path);
					/* check if pre-downloading finished or not */
					/* if local FW downloaded size (ota_ctx->downloaded_size) is not zero, it will do resuming download */
					_update_fw_downloaded_size(ota_ctx);
					/*set offset and start http connect*/
					CIAS_LOG_HL("ota_ctx->downloaded_size = %d", ota_ctx->downloaded_size);
					CIAS_LOG_HL("ota_ctx->fw_file_size = %d", ota_ctx->fw_file_size);
					// update_result_param_info_read(); //连上网检测上次更新状态 current_update_partition_status:0x0f-升级失败，0x0e-升级成功，0x0c-升级中， 0x0d-还未升级
					switch (ota_ctx->remote_version[1])
					{
					case 'a':
						#if CIAS_AIOT_AUDIO_OTA_ENABLE
						if ((gupdate_result_param_info.audio_version[0] == ota_ctx->remote_version[3]) && (gupdate_result_param_info.audio_version[1] == ota_ctx->remote_version[5]) && (gupdate_result_param_info.audio_version[2] == ota_ctx->remote_version[7])) //版本号相同，不需要更新
						{
							report_ota_status_to_iot(IOT_OTAR_NO_NEED_UPGRADE);
							CIAS_LOG_HL("current audio firmware is newest\n");
							break;
						}
						gupdate_result_param_info.update_type = AUDIO_IMG_UPDATE;
						ota_task_flag = FIRMWARE_UPDATE_ING;
						// update_result_param_info_write();
						CIAS_LOG_INFO("ota task start, destroy mqtt task.....\r\n"); //退出任务
						g_cias_system_manage_param.ota_start_flag = 1;
						while (1)
							; //等待任务结束
						#else
						// ota_task_flag = FIRMWARE_UPDATE_ING;
						report_ota_status_to_iot(1);
						#endif//CIAS_AIOT_AUDIO_OTA_ENABLE
						break;
					case 'b':
						//云端生成版本号固定格式为vb_x.x
						if ((FLASH_IMAGE_VER_MAJOR == ota_ctx->remote_version[3]) && (FLASH_IMAGE_VER_MINOR == ota_ctx->remote_version[5])) //版本号相同，不需要更新
						{
							report_ota_status_to_iot(IOT_OTAR_NO_NEED_UPGRADE);
							CIAS_LOG_HL("current wifi firmware is newest\n");
							break;
						}
						#if CIAS_AIOT_WIFI_OTA_ENABLE
						#if 1//modify by houwang
						ota_task_flag = FIRMWARE_UPDATE_ING;
						cias_task_delay_ms(10);
						if(wifi_http_ota_start((((OTA_Struct_t *)h_ota)->purl)) == OTA_ERR_NONE)
						{
			            	g_cias_system_manage_param.wifi_ota_start_flag = 1;//使能wifi ota启动标志
							for(;;);
						}
						#else
						ota_task_flag = FIRMWARE_UPDATE_ING;
						if(wifi_http_ota_start((((OTA_Struct_t *)h_ota)->purl)) == OTA_ERR_NONE)
						{
							int wifi_image_update_wait = 100;  //超时100S
							while(wifi_image_update_wait--)
							{
								CIAS_LOG_HL("wifi image update: wifi_image_update_wait = %d\r\n", wifi_image_update_wait);
								cias_task_delay_ms(1000);
							}
						}
						#endif//modify by houwang
						report_ota_status_to_iot(IOT_OTAR_UPGRADE_FAIL);
						cias_system_reboot();
						#else//CIAS_AIOT_WIFI_OTA_ENABLE
						report_ota_status_to_iot(1);
						#endif//CIAS_AIOT_WIFI_OTA_ENABLE
						break;
					case 'c':
						break;
					default:
						CIAS_LOG_HL("firmware info error-----------------\r\n");
						break;
					}
				}
#endif
				deal_up_stream();
				/*report msg to server*/
				/*report the lastest properties's status*/
				if (QCLOUD_RET_SUCCESS == deal_up_stream_user_logic(preport_data_list, &report_cont))
				{
					rc = IOT_Template_JSON_ConstructReportArray(g_iot_template_client, sg_data_report_buffer, SG_DATA_REPORT_BUFFERSIZE, report_cont, preport_data_list);
					CIAS_LOG_DEBUG("report:%s", sg_data_report_buffer);
					if (rc == QCLOUD_RET_SUCCESS)
					{
						rc = IOT_Template_Report(g_iot_template_client, sg_data_report_buffer, SG_DATA_REPORT_BUFFERSIZE,
												 on_report_reply_callback, NULL, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
						if (rc == QCLOUD_RET_SUCCESS)
						{
							CIAS_LOG_INFO("data template reporte success");
						}
						else
						{
							CIAS_LOG_ERR("data template reporte failed, err: %d", rc);
						}
					}
					else
					{
						CIAS_LOG_ERR("construct reporte data failed, err: %d", rc);
					}
				}

				if (cias_get_wifi_sta_connect_state() == NETDEV_LINK_DOWN)
				{
					CIAS_LOG_ERR("sta disconnect. break...\n");
					break;
				}
				cias_task_delay_ms(50);
			}
		}
		else
		{
			// CIAS_LOG_INFO("wifi disconnect\n");
			if (g_iot_template_client != NULL)
			{
				CIAS_LOG_INFO("wifi disconnect----\n");
				rc = IOT_Template_Destroy(g_iot_template_client);
				if (rc != QCLOUD_RET_SUCCESS)
				{
					CIAS_LOG_ERR("mqtt_client destroy err\n");
				}
				else
				{
					CIAS_LOG_HL("mqtt_client destroy successful");
				}
				g_iot_template_client = NULL;
			}
		}
	}
}

int cias_qcloud_iot_main(void)
{
	int ret;
	mqtt_client_profile_t profile_info;
	mqtt_client_profile_t profile_info_b;
	strcpy(profile_info.qcloud_iot_product_id, QCLOUD_IOT_PRODUCT_ID);
	strcpy(profile_info.qcloud_iot_device_name, QCLOUD_IOT_DEV_NAME);
	strcpy(profile_info.qcloud_iot_device_secret, QCLOUD_IOT_DEV_SECRET);


	iot_tencent_profile_set(&profile_info);
	iot_tencent_profile_get(&profile_info_b);

	if (cias_mqtt_client_init(&profile_info) != CIAS_OK)
	{
		CIAS_PRINT_DEBUG("cias_mqtt_client_init error \r\n");
		return CIAS_FAIL;
	}

	if( cias_slave_message_handle_init() != CIAS_OK)
	{
		return CIAS_FAIL;
	}
	cias_task_create(&g_cias_system_manage_param.qcloud_main_handle,
					 QCLOUD_IOT_TASK_NAME,
					 (cias_task_function_t)ci_iot_tencent_main_task,
					 (void *)0,
					 QCLOUD_IOT_TASK_PRIORITY,
					 (unsigned short)QCLOUD_IOT_TASK_SIZE);
	return CIAS_OK;
}