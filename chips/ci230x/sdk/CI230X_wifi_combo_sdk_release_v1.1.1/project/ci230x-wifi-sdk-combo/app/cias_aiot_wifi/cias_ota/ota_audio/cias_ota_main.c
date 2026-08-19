
#include "cias_crc.h"
#include "cias_log.h"
#include "cias_olcp.h"
#include "cias_freertos_common.h"
#include "cias_common.h"
#include "cias_http_download.h"
#include "cias_ota_http_port.h"
#include "cias_ota_main.h"
#include "cias_slave_message_handle.h"
#include "cias_flash_update.h"
#include "cias_user_config.h"
#include "cias_qcloud_ota.h"
#include "update.h"
#include "cias_ota_config.h"

#if CIAS_IOT_TENCENT_ENABLE
#include "qcloud_iot_export_ota.h"
#endif


extern update_result_param_info_st gupdate_result_param_info;
cias_ota_t *cias_ota = NULL;
//---------------htt下载相关---------------//

/**
 * @brief 关闭ota会话
 *
 * @param session: 会话句柄指针
 * @retval -1: error
 * @retval 0: successful
 */
int cias_ota_close_session(void **session)
{
	int rc = 0;
	if (*session != NULL)
	{
		rc = cias_http_session_close((cias_http_t *)(*session));
		*session = NULL;
	}
	return rc;
}
/**
 * @brief 创建ota链接会话
 *
 * @param session: 会话句柄指针
 * @param url: 会话url
 * @retval -1: error
 * @retval 0: successful
 */
int cias_ota_create_session(void **session, char *url)
{
	(*session) = cias_http_session_create(url);
	CIAS_ASSERT_RETURN((*session) != NULL);
	return 0;
}
/**
 * @brief ota链接文件读取
 *
 * @param session: 会话句柄指针
 * @param buff: 缓存buffer
 * @param offset: 文件偏移
 * @param len: 读取长度
 * @retval -1: error
 * @retval 其他: 实际下载文件块大小
 */
int cias_ota_file_read(void **session, unsigned char *buff, unsigned int offset, unsigned int len)
{
	int rc;

	rc = cias_http_download_file((cias_http_t *)(*session), buff, offset, len);

	return rc;
}
/**
 * @brief 获取镜像文件大小
 *
 * @param session: 会话句柄指针
 * @retval int: 实际文件大小
 */
int cias_ota_file_size(void **session)
{
	return ((cias_http_t *)(*session))->resp->header.Content_Range;
}
//----------------------------------------//
/**
 * @brief 获取ota的olcp句柄
 *
 * @retval NULL: error
 * @retval !NULL: olcp句柄
 */
cias_olcp_t *cias_ota_get_olcp_handle(void)
{
	if (cias_ota != NULL)
		return cias_ota->ota_olcp;

	return NULL;
}
/**
 * @brief 获取ota句柄
 *
 * @retval ***: ota句柄
 */
cias_ota_t *cias_ota_get_ota_handle(void)
{
	return cias_ota;
}
/**
 * @brief 打印进度条
 *
 * @param name: 进度条名字
 * @param cur_size: 当前进度
 * @param total_size: 总进度大小
 */
static void cias_ota_print_progress(char *name, int cur_size, int total_size)
{
	int i;
	char progress_sign[21];
	unsigned char tmp_per, per;

	tmp_per = cur_size * 100 / total_size;
	per = tmp_per / 5;

	for (i = 0; i < 20; i++)
	{
		if (i < per)
			progress_sign[i] = '=';
		else if (per == i)
			progress_sign[i] = '>';
		else
			progress_sign[i] = ' ';
	}
	progress_sign[i] = '\0';

	if (per >= 20)
	{
		CIAS_LOG_AGENT("\r%s:[%s]100%%\n", name, progress_sign);
	}
	else
		CIAS_LOG_AGENT("\r%s:[%s]%02d%%", name, progress_sign, tmp_per);
}
/**
 * @brief ota获取一包数据
 *
 * @param offset: 数据偏移，不是整个文件偏移，而是当前更新分区偏移
 * @param size: 数据大小
 * @retval -1: error
 * @retval 0: successful
 */
int cias_ota_get_one_packet(int offset, int size)
{
	CIAS_LOG_HL("cias_ota_get_one_packet:offset = %x, size = %d\r\n", offset, size);
	int tmp_len;
	unsigned char err_info;
	updating_partition_t *updating = &(cias_ota->updating);

	if (size > OTA_PER_FRAME_LEN)
	{
		CIAS_LOG_ERR("[CI110X]req size is wrong");
		return -1;
	}
	if (updating->offset != 0 && updating->offset + updating->len != offset)
	{
		CIAS_LOG_ERR("[CI110X]req offset is wrong");
		return -1;
	}
	if (updating->addr + offset + size > cias_ota->file_size) //超过文件大小(在请求最后一包的时候会出现这种状况)
	{
		int remain_size;
		err_info = 0;
		remain_size = cias_ota->file_size - (updating->addr + offset);
		// tmp_len = cias_ota_file_read(&(cias_ota->session), (updating->buff+4), updating->addr+offset, remain_size);
		tmp_len = get_cias_ota_handle()->cias_get_audio_image((updating->buff + 4), updating->addr + offset, remain_size);
		memset((updating->buff + 4 + remain_size), 0xff, size - remain_size);
		tmp_len += (size - remain_size);
	}
	else if (updating->addr == PARTITION_TABLE1_START_ADDR ||
			 updating->addr == PARTITION_TABLE2_START_ADDR) //如果更新分区表
	{
		err_info = 1;
		memcpy((updating->buff + 4), (unsigned char *)cias_ota->update_info, sizeof(partitions_info_t));
		tmp_len = sizeof(partitions_info_t);
		if ((size - sizeof(partitions_info_t)) > 0) //补全
		{
			memset((updating->buff + 4 + sizeof(partitions_info_t)), 0xff, size - sizeof(partitions_info_t));
			tmp_len += (size - sizeof(partitions_info_t));
		}
	}
	else
	{
		err_info = 2;
		tmp_len = get_cias_ota_handle()->cias_get_audio_image((updating->buff + 4), updating->addr + offset, size);
		// cias_ota_file_read(&(cias_ota->session), (updating->buff+4), updating->addr+offset, size);
	}
	CIAS_LOG_HL("tmp_len = %d, size = %d\r\n", tmp_len, size);
	if (tmp_len != size)
	{
		CIAS_LOG_ERR("[packet] err:[%d][0x%x][0x%x]", err_info, tmp_len, size);
		return -1;
	}

	updating->len = tmp_len;
	updating->offset = offset;

	return 0;
}
/**
 * @brief 计算当前正在更新的分区crc16校验
 *
 * @param offset: 当前偏移
 * @param size: 数据长度
 * @param data: 原始数据
 */
void cias_ota_file_crc16(int offset, int size, unsigned char *data)
{
	updating_partition_t *updating = &(cias_ota->updating);

	if (offset + size <= updating->file_size)
		updating->file_crc16 = crc16_func(updating->file_crc16, data, size);
	else
	{
		updating->crc16 = crc16_func(updating->file_crc16, data, size);
		updating->file_crc16 = crc16_func(updating->file_crc16, data, updating->file_size - offset);
	}
}
/**
 * @brief ota协议回调函数，当协议解析完成一个完整包调用此函数
 *
 * @param recv_count: 接收计数
 * @param cmd: 命令字段
 * @param data: 数据段
 * @param len: 数据长度
 */

void cias_olcp_ota_call_back(unsigned short recv_count, unsigned short cmd, unsigned char *data, unsigned short len)
{
	updating_partition_t *updating = &(cias_ota->updating);
	CIAS_LOG_HL("recv_count = %d, cmd = 0x%x, len = %d\r\n", recv_count, cmd, len);
	for (int i = 0; i < len; i++)
	{
		CIAS_LOG_HL("0x%02x ", data[i]);
	}
	CIAS_LOG_HL("\r\n");
	switch (cmd)
	{
	case OTA_NEED_FLASH_DATA_OLCP:
	{
		CIAS_LOG_HL("OTA_NEED_FLASH_DATA_OLCP is called....\r\n");
		updating->timeout = 0xff; //停止超时计数(防止因为网络不佳导致超时触发，在下载数据的时候不进行超时计数)
		updating->need_offset = *(int *)(data + 0);
		updating->need_len = *(int *)(data + 4);
		CIAS_LOG_HL("updating->need_offset = 0x%x,  updating->need_len = %d\r\n", updating->need_offset, updating->need_len);
		if (cias_ota_get_one_packet(updating->need_offset, updating->need_len) != 0)
		{
			CIAS_LOG_HL("cias_ota_get_one_packet error...\r\n");
			updating->http_err = 1;
			break;
		}

		*(int *)(updating->buff + 0) = updating->offset;
		cias_olcp_send_cmd_data_check_crc8(cias_ota->ota_olcp, OTA_WRITE_FLASH_DATA_OLCP, updating->buff, updating->len + 4); //包括4byte偏移
		cias_task_delay_ms(100);																							  //添加延时，解决audio端err!rinsts_reg = 0x00000100中断错误
		cias_ota_file_crc16(updating->offset, updating->len, (updating->buff + 4));
		cias_ota_print_progress(updating->name, updating->offset + updating->len, updating->size);
		gupdate_result_param_info.current_update_partition_offset += 0x1000;
		update_result_param_info_write();

		break;
	}
	case OTA_EARSE_PROGRESS_OLCP:
	{
		cias_ota_print_progress(OTA_ERASE_NAME, *(int *)(data + 0), *(int *)(data + 4));
		break;
	}
	case OTA_SET_PARTITION_TABLE_OLCP:
	{
		CIAS_LOG_HL("cmd OTA_SET_PARTITION_TABLE_OLCP is call.....\r\n");
		if (len == sizeof(partitions_info_t))
		{
			memcpy(&(cias_ota->device_info), data, len);
			cias_ota->is_get_device_info = 1;
#if 0 // and by yjd
                CIAS_LOG_HL("ManufacturerID = 0x%x\r\n", cias_ota->device_info.ManufacturerID);
                CIAS_LOG_HL("ProductID = 0x%x\r\n", cias_ota->device_info.ProductID);
                CIAS_LOG_HL("HWName = %s\r\n", cias_ota->device_info.HWVersion);
                CIAS_LOG_HL("HWVersion = 0x%0x\r\n", cias_ota->device_info.HWVersion);
                CIAS_LOG_HL("SWName = %s\r\n", cias_ota->device_info.SWName);
                CIAS_LOG_HL("SWVersion = %0x\r\n", cias_ota->device_info.SWVersion);
                CIAS_LOG_HL("BootLoaderVersion = 0x%0x\r\n", cias_ota->device_info.BootLoaderVersion);
                CIAS_LOG_HL("user_code1.version = 0x%0x\r\n", cias_ota->device_info.user_code1.version);
                CIAS_LOG_HL("user_code1.address = 0x%0x\r\n", cias_ota->device_info.user_code1.address);
                CIAS_LOG_HL("user_code1.size = 0x%0x\r\n", cias_ota->device_info.user_code1.size);
                CIAS_LOG_HL("user_code1.crc = 0x%0x\r\n", cias_ota->device_info.user_code1.crc);
                CIAS_LOG_HL("user_code2.status = 0x%0x\r\n", cias_ota->device_info.user_code1.status);
                CIAS_LOG_HL("user_code2.version = 0x%0x\r\n", cias_ota->device_info.user_code2.version);
                CIAS_LOG_HL("user_code2.address = 0x%0x\r\n", cias_ota->device_info.user_code2.address);
                CIAS_LOG_HL("user_code2.size = 0x%0x\r\n", cias_ota->device_info.user_code2.size);
                CIAS_LOG_HL("user_code2.crc = 0x%0x\r\n", cias_ota->device_info.user_code2.crc);
                CIAS_LOG_HL("user_code2.status = 0x%0x\r\n", cias_ota->device_info.user_code2.status);
                
                CIAS_LOG_HL("asr_cmd_model.version = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.version);
                CIAS_LOG_HL("asr_cmd_model.address = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.address);
                CIAS_LOG_HL("asr_cmd_model.size = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.size);
                CIAS_LOG_HL("asr_cmd_model.crc = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.crc);
                CIAS_LOG_HL("asr_cmd_model.status = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.status);

                CIAS_LOG_HL("dnn_model.version = 0x%0x\r\n", cias_ota->device_info.dnn_model.version);
                CIAS_LOG_HL("dnn_model.address = 0x%0x\r\n", cias_ota->device_info.dnn_model.address);
                CIAS_LOG_HL("dnn_model.size = 0x%0x\r\n", cias_ota->device_info.dnn_model.size);
                CIAS_LOG_HL("dnn_model.crc = 0x%0x\r\n", cias_ota->device_info.dnn_model.crc);
                CIAS_LOG_HL("dnn_model.status = 0x%0x\r\n", cias_ota->device_info.dnn_model.status);

                CIAS_LOG_HL("voice.version = 0x%0x\r\n", cias_ota->device_info.voice.version);
                CIAS_LOG_HL("voice.address = 0x%0x\r\n", cias_ota->device_info.voice.address);
                CIAS_LOG_HL("voice.size = 0x%0x\r\n", cias_ota->device_info.voice.size);
                CIAS_LOG_HL("voice.crc = 0x%0x\r\n", cias_ota->device_info.voice.crc);
                CIAS_LOG_HL("voice.status = 0x%0x\r\n", cias_ota->device_info.voice.status);

                CIAS_LOG_HL("user_file.version = 0x%0x\r\n", cias_ota->device_info.user_file.version);
                CIAS_LOG_HL("user_file.address = 0x%0x\r\n", cias_ota->device_info.user_file.address);
                CIAS_LOG_HL("user_file.size = 0x%0x\r\n", cias_ota->device_info.user_file.size);
                CIAS_LOG_HL("user_file.crc = 0x%0x\r\n", cias_ota->device_info.user_file.crc);
                CIAS_LOG_HL("user_file.status = 0x%0x\r\n", cias_ota->device_info.user_file.status);

                CIAS_LOG_HL("ConsumerDataStartAddr = 0x%0x\r\n", cias_ota->device_info.ConsumerDataStartAddr);
                CIAS_LOG_HL("ConsumerDataSize = 0x%0x\r\n", cias_ota->device_info.ConsumerDataSize);
                CIAS_LOG_HL("PartitionTableChecksum = 0x%0x\r\n", cias_ota->device_info.PartitionTableChecksum);
                CIAS_LOG_HL("ota_statue = 0x%0x\r\n", cias_ota->device_info.ota_statue);
#endif
		}
		else
		{
			CIAS_LOG_ERR("recv CI110X partition info is wrong");
		}

		break;
	}
	case OTA_UPDATE_BLOCK_DONE_OLCP:
	{
		CIAS_LOG_HL("OTA_UPDATE_BLOCK_DONE_OLCP is update over\r\n");
		updating->is_update_done = 1;
		break;
	}
	case OTA_VERIFY_PARTITION_OLCP:
	{

		updating->recv_crc16 = *(unsigned short *)(data + 0);
		updating->is_recv_crc16 = 1;
		CIAS_LOG_HL("OTA_VERIFY_PARTITION_OLCP is exec, updating->recv_crc16 = 0x%x\r\n", updating->recv_crc16);
		break;
	}
	default:
	{
		CIAS_LOG_ERR("unknow cmd: %04x", cmd);
		break;
	}
	}

	updating->timeout = 0; //清空超时计数
}
/**
 * @brief ota结构体初始化
 *
 * @param uri: 镜像文件url
 * @retval -1: error
 * @retval 0: successful
 */
int cias_ota_struct_init(void)
{
	if (cias_ota == NULL)
		cias_ota = (cias_ota_t *)malloc(sizeof(cias_ota_t));
	if (cias_ota == NULL)
		return -1;
	cias_ota->updating.buff = malloc(OTA_BUFF_LEN);
	if (cias_ota->updating.buff == NULL)
		return -1;
	memset(cias_ota->updating.buff, 0x00, OTA_BUFF_LEN);
	cias_ota->session = NULL;
	cias_ota->ota_olcp = NULL;
	return 0;
}
/**
 * @brief 计算全部分区校验和
 *
 * @param file_config: 分区表
 * @retval unsigned short: 检验和
 */
unsigned short cias_ota_get_partition_list_checksum(partitions_info_t *file_config)
{
	int len = sizeof(partitions_info_t) - 3;
	unsigned short sum = 0;
	unsigned char user_code1_complt_status = file_config->user_code1.status;
	unsigned char user_code2_complt_status = file_config->user_code2.status;
	unsigned char ars_cmd_model_complt_status = file_config->asr_cmd_model.status;
	unsigned char dnn_model_complt_status = file_config->dnn_model.status;
	unsigned char voice_playing_complt_status = file_config->voice.status;
	unsigned char user_file_complt_status = file_config->user_file.status;

	file_config->user_code1.status = USERCODE_AREA_STA_OK; // 0xf0
	file_config->user_code2.status = USERCODE_AREA_STA_OK; // 0xf0
	file_config->asr_cmd_model.status = OTHER_AREA_OK;	   // 0x0
	file_config->dnn_model.status = OTHER_AREA_OK;
	file_config->voice.status = OTHER_AREA_OK;
	file_config->user_file.status = OTHER_AREA_OK;

	for (int i = 0; i < len; i++)
	{
		sum += ((unsigned char *)file_config)[i];
		// CIAS_LOG_HL("file_config[%d] = 0x%x", i, ((unsigned char*)file_config)[i]);
	}

	file_config->user_code1.status = user_code1_complt_status;
	file_config->user_code2.status = user_code2_complt_status;
	file_config->asr_cmd_model.status = ars_cmd_model_complt_status;
	file_config->dnn_model.status = dnn_model_complt_status;
	file_config->voice.status = voice_playing_complt_status;
	file_config->user_file.status = user_file_complt_status;
	return sum;
}

/**
 * @brief 比较两个分区信息
 *
 * @param p1: 分区1
 * @param p2: 分区2
 * @retval 1: 分区信息不相同
 * @retval 0: 分区信息相同
 */
int _ota_compare_songle_partition_info(partition_t *p1, partition_t *p2)
{
	if (p1->version != p2->version ||
		p1->size != p2->size ||
		p1->address != p2->address ||
		p1->crc != p2->crc)
	{
		return 1;
	}
	return 0;
}
/**
 * @brief 计算更新哪一个代码分区
 *
 * @retval unsigned short: 需要更新分区信息
 */
unsigned short _ota_compare_user_code_partition(void)
{
	unsigned short ret = 0;
	partitions_info_t *p_device = NULL;
	partitions_info_t *p_file = NULL;

	if (cias_ota == NULL)
	{
		return 0;
	}
	p_device = &(cias_ota->device_info);
	p_file = &(cias_ota->file_info);

	if (p_device->user_code1.status == USERCODE_AREA_STA_UPDATE) // code1上次更新未完成，继续更新code1
	{
		ret |= USER1_NEED_UPDATE_MASK;
	}
	else if (p_device->user_code2.status == USERCODE_AREA_STA_UPDATE)
	{
		ret |= USER2_NEED_UPDATE_MASK;
	}
	else if (p_device->user_code1.status == USERCODE_AREA_STA_OK) // code1为有效区(一般都是code1为有效区域，除非由于特殊原因 更新code1将code2置为有效区)
	{
		/*比较设备第一块和固件第一块是否相同*/
		if (_ota_compare_songle_partition_info(&(p_device->user_code1), &(p_file->user_code1)))
		{
			if ((p_file->user_code1.size < p_device->user_code1.size) && (p_device->user_code2.status == USERCODE_AREA_STA_OLD)) //特殊原因
			{
				// code1 变小，为了保证备份区的保险功能，改为升级code1
				ret |= USER1_NEED_UPDATE_MASK;
			}
			else
			{ // code1 保持不变，更新code2
				ret |= USER2_NEED_UPDATE_MASK;
			}
		}
	}
	else if (p_device->user_code2.status == USERCODE_AREA_STA_OK) //特殊原因造成code2为有效区 并且上次更新code1失败 所以继续更新code1
	{
		ret |= USER1_NEED_UPDATE_MASK;
	}

	return ret;
}

/**
 * @brief 计算更新哪一个分区
 *
 * @retval -1: error
 * @retval 其他: 需要更新分区信息
 */
int __ota_compare_all_partitions(void)
{
	unsigned short check_sum = 0, ret = 0;
	partitions_info_t *p_update = &(cias_ota->device_info);
	partitions_info_t *p_device = &(cias_ota->device_info);
	partitions_info_t *p_file = &(cias_ota->file_info);

	cias_ota->update_info = p_update;

#if 1 // and by yjd
	CIAS_LOG_HL("1----ManufacturerID = 0x%x\r\n", cias_ota->file_info.ManufacturerID);
	CIAS_LOG_HL("ProductID = 0x%x\r\n", cias_ota->file_info.ProductID);
	CIAS_LOG_HL("HWName = %s\r\n", cias_ota->file_info.HWVersion);
	CIAS_LOG_HL("HWVersion = 0x%0x\r\n", cias_ota->file_info.HWVersion);
	CIAS_LOG_HL("SWName = %s\r\n", cias_ota->file_info.SWName);
	CIAS_LOG_HL("SWVersion = %0x\r\n", cias_ota->file_info.SWVersion);
	CIAS_LOG_HL("BootLoaderVersion = 0x%0x\r\n", cias_ota->file_info.BootLoaderVersion);
	CIAS_LOG_HL("user_code1.version = 0x%0x\r\n", cias_ota->file_info.user_code1.version);
	CIAS_LOG_HL("user_code1.address = 0x%0x\r\n", cias_ota->file_info.user_code1.address);
	CIAS_LOG_HL("user_code1.size = 0x%0x\r\n", cias_ota->file_info.user_code1.size);
	CIAS_LOG_HL("user_code1.crc = 0x%0x\r\n", cias_ota->file_info.user_code1.crc);
	CIAS_LOG_HL("user_code2.status = 0x%0x\r\n", cias_ota->file_info.user_code1.status);
	CIAS_LOG_HL("user_code2.version = 0x%0x\r\n", cias_ota->file_info.user_code2.version);
	CIAS_LOG_HL("user_code2.address = 0x%0x\r\n", cias_ota->file_info.user_code2.address);
	CIAS_LOG_HL("user_code2.size = 0x%0x\r\n", cias_ota->file_info.user_code2.size);
	CIAS_LOG_HL("user_code2.crc = 0x%0x\r\n", cias_ota->file_info.user_code2.crc);
	CIAS_LOG_HL("user_code2.status = 0x%0x\r\n", cias_ota->file_info.user_code2.status);

	CIAS_LOG_HL("asr_cmd_model.version = 0x%0x\r\n", cias_ota->file_info.asr_cmd_model.version);
	CIAS_LOG_HL("asr_cmd_model.address = 0x%0x\r\n", cias_ota->file_info.asr_cmd_model.address);
	CIAS_LOG_HL("asr_cmd_model.size = 0x%0x\r\n", cias_ota->file_info.asr_cmd_model.size);
	CIAS_LOG_HL("asr_cmd_model.crc = 0x%0x\r\n", cias_ota->file_info.asr_cmd_model.crc);
	CIAS_LOG_HL("asr_cmd_model.status = 0x%0x\r\n", cias_ota->file_info.asr_cmd_model.status);

	CIAS_LOG_HL("dnn_model.version = 0x%0x\r\n", cias_ota->file_info.dnn_model.version);
	CIAS_LOG_HL("dnn_model.address = 0x%0x\r\n", cias_ota->file_info.dnn_model.address);
	CIAS_LOG_HL("dnn_model.size = 0x%0x\r\n", cias_ota->file_info.dnn_model.size);
	CIAS_LOG_HL("dnn_model.crc = 0x%0x\r\n", cias_ota->file_info.dnn_model.crc);
	CIAS_LOG_HL("dnn_model.status = 0x%0x\r\n", cias_ota->file_info.dnn_model.status);

	CIAS_LOG_HL("voice.version = 0x%0x\r\n", cias_ota->file_info.voice.version);
	CIAS_LOG_HL("voice.address = 0x%0x\r\n", cias_ota->file_info.voice.address);
	CIAS_LOG_HL("voice.size = 0x%0x\r\n", cias_ota->file_info.voice.size);
	CIAS_LOG_HL("voice.crc = 0x%0x\r\n", cias_ota->file_info.voice.crc);
	CIAS_LOG_HL("voice.status = 0x%0x\r\n", cias_ota->file_info.voice.status);

	CIAS_LOG_HL("user_file.version = 0x%0x\r\n", cias_ota->file_info.user_file.version);
	CIAS_LOG_HL("user_file.address = 0x%0x\r\n", cias_ota->file_info.user_file.address);
	CIAS_LOG_HL("user_file.size = 0x%0x\r\n", cias_ota->file_info.user_file.size);
	CIAS_LOG_HL("user_file.crc = 0x%0x\r\n", cias_ota->file_info.user_file.crc);
	CIAS_LOG_HL("user_file.status = 0x%0x\r\n", cias_ota->file_info.user_file.status);

	CIAS_LOG_HL("ConsumerDataStartAddr = 0x%0x\r\n", cias_ota->file_info.ConsumerDataStartAddr);
	CIAS_LOG_HL("ConsumerDataSize = 0x%0x\r\n", cias_ota->file_info.ConsumerDataSize);
	CIAS_LOG_HL("PartitionTableChecksum = 0x%0x\r\n", cias_ota->file_info.PartitionTableChecksum);
	// CIAS_LOG_HL("ota_statue = 0x%0x\r\n", cias_ota->file_info.ota_statue);
#endif
	//-------如果cias_ota初始化错误或者文件分区表校验不通过， 则不进行更新-------//
	if (cias_ota == NULL)
	{
		return -1;
	}
	check_sum = cias_ota_get_partition_list_checksum(p_file);
	CIAS_LOG_HL("check_sum = 0x%x\r\n", check_sum);
	CIAS_LOG_HL("p_file->PartitionTableChecksum = 0x%x\r\n", p_file->PartitionTableChecksum);
	if (check_sum != p_file->PartitionTableChecksum)
	{
		CIAS_LOG_ERR("file info check err"); //文件分区表校验不通过，不进行更新
		return -1;
	}
#if 0 // remove by yjd
    //-------无论是部分更新还是全部更新，都先比较code1，code2分区表信息决定先更新哪一个code分区，防止OTA升级擦除覆盖有效分区--------//
    ret = _ota_compare_user_code_partition();  //计算更新哪一个代码分区
    ret &= (USER1_NEED_UPDATE_MASK|USER2_NEED_UPDATE_MASK);
    if (ret & USER1_NEED_UPDATE_MASK) //需要更新code1
    {
        CIAS_LOG_HL("nedd update user code1....\r\n");
#if 0 // remove by yjd
        p_update->user_code1 = p_file->user_code1;
        p_update->user_code1.status = USERCODE_AREA_STA_UPDATE;
        p_update->user_code2.status = USERCODE_AREA_STA_OK;
#endif  
    }
   
    else if(ret & USER2_NEED_UPDATE_MASK) //需要更新code2
    {
        CIAS_LOG_HL("nedd update user code2....\r\n");
        p_update->user_code2 = p_file->user_code2;
        p_update->user_code2.status = USERCODE_AREA_STA_UPDATE;
        p_update->user_code1.status = USERCODE_AREA_STA_OK;
    }
#endif
	if (cias_ota->file_info.user_code1.version != cias_ota->device_info.user_code1.version) //直接比较版本号
	{
		CIAS_LOG_HL("nedd update user code2....\r\n");
		ret |= USER2_NEED_UPDATE_MASK;
		p_update->user_code2 = p_file->user_code2;
		p_update->user_code2.status = USERCODE_AREA_STA_UPDATE;
		p_update->user_code1.status = USERCODE_AREA_STA_OK;
	}
#if UPDATE_ALL_PARTITION //跳过比较，直接更新全部分区
	goto __update_all_partition;
#endif

	//-------ManufacturerID或者设备校验和有问题，则全部更新--------//
	if (p_device->ManufacturerID != p_file->ManufacturerID)
	{
		CIAS_LOG_ERR("ManufacturerID err"); // ManufacturerID不一致更新全部分区
		goto __update_all_partition;
	}
#if 1 // and by yjd
	CIAS_LOG_HL("2----ManufacturerID = 0x%x\r\n", cias_ota->device_info.ManufacturerID);
	CIAS_LOG_HL("ProductID = 0x%x\r\n", cias_ota->device_info.ProductID);
	CIAS_LOG_HL("HWName = %s\r\n", cias_ota->device_info.HWVersion);
	CIAS_LOG_HL("HWVersion = 0x%0x\r\n", cias_ota->device_info.HWVersion);
	CIAS_LOG_HL("SWName = %s\r\n", cias_ota->device_info.SWName);
	CIAS_LOG_HL("SWVersion = %0x\r\n", cias_ota->device_info.SWVersion);
	CIAS_LOG_HL("BootLoaderVersion = 0x%0x\r\n", cias_ota->device_info.BootLoaderVersion);
	CIAS_LOG_HL("user_code1.version = 0x%0x\r\n", cias_ota->device_info.user_code1.version);
	CIAS_LOG_HL("user_code1.address = 0x%0x\r\n", cias_ota->device_info.user_code1.address);
	CIAS_LOG_HL("user_code1.size = 0x%0x\r\n", cias_ota->device_info.user_code1.size);
	CIAS_LOG_HL("user_code1.crc = 0x%0x\r\n", cias_ota->device_info.user_code1.crc);
	CIAS_LOG_HL("user_code2.status = 0x%0x\r\n", cias_ota->device_info.user_code1.status);
	CIAS_LOG_HL("user_code2.version = 0x%0x\r\n", cias_ota->device_info.user_code2.version);
	CIAS_LOG_HL("user_code2.address = 0x%0x\r\n", cias_ota->device_info.user_code2.address);
	CIAS_LOG_HL("user_code2.size = 0x%0x\r\n", cias_ota->device_info.user_code2.size);
	CIAS_LOG_HL("user_code2.crc = 0x%0x\r\n", cias_ota->device_info.user_code2.crc);
	CIAS_LOG_HL("user_code2.status = 0x%0x\r\n", cias_ota->device_info.user_code2.status);

	CIAS_LOG_HL("asr_cmd_model.version = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.version);
	CIAS_LOG_HL("asr_cmd_model.address = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.address);
	CIAS_LOG_HL("asr_cmd_model.size = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.size);
	CIAS_LOG_HL("asr_cmd_model.crc = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.crc);
	CIAS_LOG_HL("asr_cmd_model.status = 0x%0x\r\n", cias_ota->device_info.asr_cmd_model.status);

	CIAS_LOG_HL("dnn_model.version = 0x%0x\r\n", cias_ota->device_info.dnn_model.version);
	CIAS_LOG_HL("dnn_model.address = 0x%0x\r\n", cias_ota->device_info.dnn_model.address);
	CIAS_LOG_HL("dnn_model.size = 0x%0x\r\n", cias_ota->device_info.dnn_model.size);
	CIAS_LOG_HL("dnn_model.crc = 0x%0x\r\n", cias_ota->device_info.dnn_model.crc);
	CIAS_LOG_HL("dnn_model.status = 0x%0x\r\n", cias_ota->device_info.dnn_model.status);

	CIAS_LOG_HL("voice.version = 0x%0x\r\n", cias_ota->device_info.voice.version);
	CIAS_LOG_HL("voice.address = 0x%0x\r\n", cias_ota->device_info.voice.address);
	CIAS_LOG_HL("voice.size = 0x%0x\r\n", cias_ota->device_info.voice.size);
	CIAS_LOG_HL("voice.crc = 0x%0x\r\n", cias_ota->device_info.voice.crc);
	CIAS_LOG_HL("voice.status = 0x%0x\r\n", cias_ota->device_info.voice.status);

	CIAS_LOG_HL("user_file.version = 0x%0x\r\n", cias_ota->device_info.user_file.version);
	CIAS_LOG_HL("user_file.address = 0x%0x\r\n", cias_ota->device_info.user_file.address);
	CIAS_LOG_HL("user_file.size = 0x%0x\r\n", cias_ota->device_info.user_file.size);
	CIAS_LOG_HL("user_file.crc = 0x%0x\r\n", cias_ota->device_info.user_file.crc);
	CIAS_LOG_HL("user_file.status = 0x%0x\r\n", cias_ota->device_info.user_file.status);

	CIAS_LOG_HL("ConsumerDataStartAddr = 0x%0x\r\n", cias_ota->device_info.ConsumerDataStartAddr);
	CIAS_LOG_HL("ConsumerDataSize = 0x%0x\r\n", cias_ota->device_info.ConsumerDataSize);
	CIAS_LOG_HL("PartitionTableChecksum = 0x%0x\r\n", cias_ota->device_info.PartitionTableChecksum);
	// CIAS_LOG_HL("ota_statue = 0x%0x\r\n", cias_ota->device_info.ota_statue);
#endif
	/*check_sum = cias_ota_get_partition_list_checksum(p_device);
	  CIAS_LOG_HL("--check_sum = 0x%x\r\n", check_sum);
	  CIAS_LOG_HL("--p_device->PartitionTableChecksum = 0x%x\r\n", p_device->PartitionTableChecksum);
	  if (check_sum != p_device->PartitionTableChecksum)   //remove by yjd-由于直接用工具烧写各分区信息对不上，所以导致每次都会全部更新
	  {
		  CIAS_LOG_ERR(OTA_LOG"device info check err"); //设备分区表校验不通过，更新设备全部分区
		  goto __update_all_partition;
	  }*/

	//-------比较分区信息--------//
	if (_ota_compare_songle_partition_info(&(p_device->asr_cmd_model), &(p_file->asr_cmd_model)) || p_device->asr_cmd_model.status != 0x00)
	{
		p_update->asr_cmd_model = p_file->asr_cmd_model;
		p_update->asr_cmd_model.status = OTHER_AREA_UPDATE;
		ret |= ASR_NEED_UPDATE_MASK;
	}

	if (_ota_compare_songle_partition_info(&(p_device->dnn_model), &(p_file->dnn_model)) || p_device->dnn_model.status != 0x00)
	{
		p_update->dnn_model = p_file->dnn_model;
		p_update->dnn_model.status = OTHER_AREA_UPDATE;
		ret |= DNN_NEED_UPDATE_MASK;
	}

	if (_ota_compare_songle_partition_info(&(p_device->voice), &(p_file->voice)) || p_device->voice.status != 0x00)
	{
		p_update->voice = p_file->voice;
		p_update->voice.status = OTHER_AREA_UPDATE;
		ret |= VOICE_NEED_UPDATE_MASK;
	}

	if (_ota_compare_songle_partition_info(&(p_device->user_file), &(p_file->user_file)) || p_device->user_file.status != 0x00)
	{
		p_update->user_file = p_file->user_file;
		p_update->user_file.status = OTHER_AREA_UPDATE;
		ret |= USERFILE_NEED_UPDATE_MASK;
	}
	goto __ret;

__update_all_partition: //全部更新
	// code
	if (ret == 0) //如果没有需要更新的code区域, 则强制先更新code2, 再更新code1
	{
		ret |= USER2_NEED_UPDATE_MASK;
		p_update->user_code2 = p_file->user_code2;
		p_update->user_code2.status = USERCODE_AREA_STA_UPDATE;
		p_update->user_code1.status = USERCODE_AREA_STA_OK;
	}
	// asr
	p_update->asr_cmd_model = p_file->asr_cmd_model;
	p_update->asr_cmd_model.status = OTHER_AREA_UPDATE;
	ret |= ASR_NEED_UPDATE_MASK;
	// dnn
	p_update->dnn_model = p_file->dnn_model;
	p_update->dnn_model.status = OTHER_AREA_UPDATE;
	ret |= DNN_NEED_UPDATE_MASK;
	// voice
	p_update->voice = p_file->voice;
	p_update->voice.status = OTHER_AREA_UPDATE;
	ret |= VOICE_NEED_UPDATE_MASK;
	// user file
	p_update->user_file = p_file->user_file;
	p_update->user_file.status = OTHER_AREA_UPDATE;
	ret |= USERFILE_NEED_UPDATE_MASK;

__ret:
	CIAS_LOG_DEBUG("Update:");
	CIAS_LOG_DEBUG("    -" OTA_CODE1_NAME " addr:0x%x, crc:0x%x", p_update->user_code1.address, p_update->user_code1.crc);
	CIAS_LOG_DEBUG("    -" OTA_CODE2_NAME " addr:0x%x, crc:0x%x", p_update->user_code2.address, p_update->user_code2.crc);
	CIAS_LOG_DEBUG("    -" OTA_ASR_NAME " addr:0x%x, crc:0x%x", p_update->asr_cmd_model.address, p_update->asr_cmd_model.crc);
	CIAS_LOG_DEBUG("    -" OTA_DNN_NAME " addr:0x%x, crc:0x%x", p_update->dnn_model.address, p_update->dnn_model.crc);
	CIAS_LOG_DEBUG("    -" OTA_VOICE_NAME " addr:0x%x, crc:0x%x", p_update->voice.address, p_update->voice.crc);
	CIAS_LOG_DEBUG("    -" OTA_USER_FILE_NAME " addr:0x%x, crc:0x%x", p_update->user_file.address, p_update->user_file.crc);

	return ret;
}
/**
 * @brief 给CI110X 发送升级分区信息
 *
 * @param addr: 需要升级分区的起始地址
 * @param size: 需要升级分区的大小
 * @retval -1: error
 * @retval 0: successful
 */
int cias_ota_olcp_send_erase_partition_info(unsigned int addr, unsigned int size)
{
	unsigned char tmp_data[8];

	*(unsigned int *)(tmp_data + 0) = addr;
	*(unsigned int *)(tmp_data + 4) = size;
	cias_ota->updating.addr = addr;
	cias_ota->updating.size = size;
	return cias_olcp_send_cmd_data_check_crc8_ack(cias_ota->ota_olcp, OTA_NEED_ERASE_PARTITION_OLCP, tmp_data, 8);
}
/**
 * @brief   等待升级的分区升级完成。
 *          如果超过30s没有收到CI110X回应，则认为超时退出更新。
 *          如果超过2s没有收到CI110X回应，则发送最后要的那一帧数据
 *
 * @retval -1: error
 * @retval 0: successful
 */
int cias_ota_is_write_partition_done(void)
{
	updating_partition_t *updating = &(cias_ota->updating);
	//等待刚才发送的升级的分区升级完成
	CIAS_LOG_HL("updating->is_update_done set to 0\r\n");
	updating->is_update_done = 0;
	updating->timeout = 0;

	//如果超过30s没有收到CI110X回应，则认为超时退出更新
	//如果超过2s没有收到CI110X回应，则发送最后要的那一帧数据
	while (updating->is_update_done == 0)
	{
		CIAS_LOG_HL("updating->is_update_done = %d\r\n", updating->is_update_done);
		cias_task_delay_ms(500);
		if (updating->timeout == 0xff) //下载文件的时候不进行超时计数
		{
			continue;
		}
		updating->timeout++;

		if ((updating->timeout != 0) && ((updating->timeout % 4) == 0) && (updating->offset != 0)) //间隔2s发送最后要的那一帧数据
		{
			if (updating->http_err == 1) //如果是http下载导致OTA暂停则尝试http下载
			{
				CIAS_LOG_HL("-----updating->need_offset = 0x%x, updating->need_len = %d\r\n", updating->need_offset, updating->need_len);
				if (cias_ota_get_one_packet(updating->need_offset, updating->need_len) == 0)
				{
					*(int *)(updating->buff + 0) = updating->offset;
					cias_olcp_send_cmd_data_check_crc8(cias_ota->ota_olcp, OTA_WRITE_FLASH_DATA_OLCP, updating->buff, updating->len + 4); //包括4byte偏移
					cias_ota_file_crc16(updating->offset, updating->len, (updating->buff + 4));
				}
			}
			else
			{
				CIAS_LOG_ERR("[write]send last frame[0x%x]", *(int *)(cias_ota->updating.buff + 0));
				cias_olcp_send_cmd_data_check_crc8(cias_ota->ota_olcp, OTA_WRITE_FLASH_DATA_OLCP, cias_ota->updating.buff, cias_ota->updating.len + 4); //包括4byte偏移
			}
		}
		if (updating->timeout >= 60) //超过30s认为超时
		{
			CIAS_LOG_ERR("[write]time out!!!");
			return -1;
		}
	}
	return 0;
}
/**
 * @brief 更新完成后一个分区后校验该分区，比较传入的crc16与CI110X应答的crc16是否相等
 *
 * @param crc16: 该分区计算的crc16校验值
 * @retval -1: error
 * @retval 0: successful
 */
int cias_ota_verify_partition(unsigned short crc16)
{
	int timeout = (cias_ota->updating.size / 100000) + 2; // timeout和当前更新的文件大小成正比
	unsigned char tmp_data[6];
	unsigned short partitions_crc16 = 0;
	updating_partition_t *updating = &(cias_ota->updating);
	partitions_info_t *p_update = cias_ota->update_info;

	*(unsigned int *)(tmp_data + 0) = updating->addr;
	*(unsigned short *)(tmp_data + 4) = crc16;
	updating->is_recv_crc16 = 0;
	CIAS_LOG_HL("cias_ota_verify_partition is exec...timeout = %d\r\n", timeout);
	cias_olcp_send_cmd_data_check_crc8_ack(cias_ota->ota_olcp, OTA_VERIFY_PARTITION_OLCP, tmp_data, 6);
	CIAS_LOG_HL("cias_ota_verify_partition is end...\r\n");
	if (timeout < 6) //最小延时3S
		timeout = 6;
	if (timeout > 25)
		timeout = 25;
	do
	{
		cias_task_delay_ms(500);
	} while (timeout-- && (updating->is_recv_crc16 == 0));

	if (timeout <= 0)
		return -1;
	CIAS_LOG_HL("updating->recv_crc16 = 0x%x, crc16 = 0x%x\r\n", updating->recv_crc16, crc16);
	// CIAS_LOG_HL("[verify]%x", crc16);
	if (updating->recv_crc16 != crc16)
	{
		CIAS_LOG_ERR("[verify]device:0x%x--0x%x", crc16, updating->recv_crc16);
		return -1;
	}

	if (updating->addr == p_update->user_code1.address && updating->file_crc16 != p_update->user_code1.crc)
		partitions_crc16 = p_update->user_code1.crc;
	else if (updating->addr == p_update->user_code2.address && updating->file_crc16 != p_update->user_code2.crc)
		partitions_crc16 = p_update->user_code2.crc;
	else if (updating->addr == p_update->asr_cmd_model.address && updating->file_crc16 != p_update->asr_cmd_model.crc)
		partitions_crc16 = p_update->asr_cmd_model.crc;
	else if (updating->addr == p_update->dnn_model.address && updating->file_crc16 != p_update->dnn_model.crc)
		partitions_crc16 = p_update->dnn_model.crc;
	else if (updating->addr == p_update->voice.address && updating->file_crc16 != p_update->voice.crc)
		partitions_crc16 = p_update->voice.crc;
	else if (updating->addr == p_update->user_file.address && updating->file_crc16 != p_update->user_file.crc)
		partitions_crc16 = p_update->user_file.crc;
	CIAS_LOG_HL("partitions_crc16 = 0x%x\r\n", partitions_crc16);
	if (partitions_crc16 != 0)
	{
		CIAS_LOG_ERR("[verify]file:0x%x--0x%x", updating->file_crc16, partitions_crc16);
		return -1;
	}

	return 0;
}
/**
 * @brief ota更新单个分区
 *
 * @param name: 分区名称
 * @param addr: 分区起始地址
 * @param size: 分区大小
 * @retval -1: error
 * @retval 0: successful
 */
int cias_ota_update_songle_partition(char *name, unsigned int addr, unsigned int size)
{
	unsigned int erase_size;
	updating_partition_t *updating = &(cias_ota->updating);
	//由于flash一次擦除4K，所以4K对齐
	erase_size = (size + OTA_MIN_PARTITION_SIZE - 1) / OTA_MIN_PARTITION_SIZE;
	erase_size *= OTA_MIN_PARTITION_SIZE;

	//清除crc16，校验时候用
	updating->crc16 = 0;
	updating->file_crc16 = 0;
	updating->file_size = size;
	updating->name = name;
	updating->offset = 0;
	updating->http_err = 0;
	CIAS_LOG_HL("cias_ota_olcp_send_erase_partition_info is start ....\r\n");
	CIAS_LOG_HL("addr = 0x%x, erase_size = 0x%x, updating->file_size = 0x%x\r\n", addr, erase_size, updating->file_size);

#if 0
	//发送升级分区信息
	if (cias_ota_olcp_send_erase_partition_info(addr, erase_size) != 0)
	{
		CIAS_LOG_ERR("[partition]erase info err");
		return -1;
	}
	CIAS_LOG_HL("cias_ota_olcp_send_erase_partition_info is end ....\r\n");
#endif
	//擦除和更新分区
	if (cias_ota_is_write_partition_done() != 0)
	{
		CIAS_LOG_ERR("[partition]write partition err");

		return -1;
	}
	CIAS_LOG_HL("cias_ota_is_write_partition_done is end ....\r\n");
#if 0
	//校验分区
	if (cias_ota_verify_partition(cias_ota->updating.crc16) != 0)
	{
		CIAS_LOG_ERR("[partition]verify partition err");
		return -1;
	}
	CIAS_LOG_HL("cias_ota_verify_partition is end ....\r\n");
#endif
	return 0;
}
/**
 * @brief 跟新分区表
 *
 * @retval -1: error
 * @retval -1: successful
 */
static int _ota_upgrade_partition_table(void)
{
	int rc;

	CIAS_ASSERT_RETURN(cias_ota != NULL);
	//更新分区表前需重新计算校验和
	cias_ota->update_info->PartitionTableChecksum = cias_ota_get_partition_list_checksum(cias_ota->update_info);
	CIAS_LOG_HL("cias_ota->update_info->PartitionTableChecksum = 0x%x\r\n", cias_ota->update_info->PartitionTableChecksum);
	/*rc = cias_ota_update_songle_partition(OTA_TABLE1_NAME, PARTITION_TABLE1_START_ADDR, sizeof(partitions_info_t));   //防止在写入主分区表的时候异常，audio侧在写入备份分区表时自动更新主分区表
	  CIAS_ASSERT_RETURN(rc==0);*/
	rc = cias_ota_update_songle_partition(OTA_TABLE2_NAME, PARTITION_TABLE2_START_ADDR, sizeof(partitions_info_t));
	CIAS_ASSERT_RETURN(rc == 0);

	return 0;
}
/**
 * @brief ota跟新需要更新的分区
 *
 * @param need_update: 需要跟新的分区信息
 * @retval -1: error
 * @retval 0: successful
 */
int cias_ota_upgrade_need_update_partition(int need_update)
{
	int rc;
	partitions_info_t *p_update = cias_ota->update_info;
	unsigned int waddress;
	unsigned int wsize;
	
	#if 0
	//更新OTA状态
	// cias_ota->update_info->ota_statue = OTA_UPDATING;
	CIAS_LOG_HL("_ota_upgrade_partition_table is exec .....\r\n");
	//先更新分区表
	rc = _ota_upgrade_partition_table();
	CIAS_ASSERT_RETURN(rc == 0);
	CIAS_LOG_HL("_ota_upgrade_partition_table is end .....\r\n");
	update_result_param_info_read();
	#endif
	if (gupdate_result_param_info.current_update_partition_offset >= 0x1000)
		gupdate_result_param_info.current_update_partition_offset -= 0x1000; //防止上个次没写入异常，后退4KBytes
#if 0
    if (need_update & USER1_NEED_UPDATE_MASK   )     //需要升级user code 1  特殊情况
    {
        CIAS_LOG_HL("USER1_NEED_UPDATE_MASK is update....\r\n");
         update_result_param_info_read();
        if((gupdate_result_param_info.current_update_partition_status == 0x0f) &&  (gupdate_result_param_info.current_update_partition_index == AUDIO_PARTITION_PRIMARY_CODE))  //升级失败，断点续传
        {
          CIAS_LOG_HL("set user_code1.addriss to current_update_partition_offset:---0x%x \r\n", gupdate_result_param_info.current_update_partition_offset);
          waddress = p_update->user_code1.address + gupdate_result_param_info.current_update_partition_offset;
          wsize = p_update->user_code1.size - gupdate_result_param_info.current_update_partition_offset;
        }
        else
        {
          waddress = p_update->user_code1.address;
          wsize = p_update->user_code1.size;
        }
        gupdate_result_param_info.current_update_partition_index == AUDIO_PARTITION_PRIMARY_CODE;
        gupdate_result_param_info.current_update_partition_status = 0xf;   //修改为失败状态，等待升级完成，修改为成功
        update_result_param_info_write();
        CIAS_LOG_HL("update p_update->user_code1.address = 0x%x, p_update->user_code1.size = 0x%x\r\n", p_update->user_code1.address, p_update->user_code1.size);   //remove by yjd
        CIAS_LOG_HL("waddress = 0x%x, wsize = 0x%x\r\n", waddress, wsize);
        rc = cias_ota_update_songle_partition(OTA_CODE1_NAME, waddress, wsize);
        CIAS_ASSERT_RETURN(rc==0);
        /*CIAS_LOG_HL("USER1_NEED_UPDATE_MASK - 1\r\n");
        p_update->user_code2 = cias_ota->file_info.user_code2;
        p_update->user_code2.status = USERCODE_AREA_STA_UPDATE;
        p_update->user_code1.status = USERCODE_AREA_STA_OK;
        rc = _ota_upgrade_partition_table();
        CIAS_LOG_HL("USER1_NEED_UPDATE_MASK - 2\r\n");
        CIAS_ASSERT_RETURN(rc==0);
        rc = cias_ota_update_songle_partition(OTA_CODE2_NAME, p_update->user_code2.address, p_update->user_code2.size);
        CIAS_ASSERT_RETURN(rc==0);*/
        gupdate_result_param_info.current_update_partition_status = 0xe;   //升级成功，修改升级状态为成功
        update_result_param_info_write();
    }
#endif

	if (need_update & ASR_NEED_UPDATE_MASK) //需要升级ASR
	{
		CIAS_LOG_HL("ASR_NEED_UPDATE_MASK is update....\r\n");
		if ((gupdate_result_param_info.current_update_partition_status == 0x0f) && (gupdate_result_param_info.current_update_partition_index == AUDIO_PARTITION_ASR)) //升级失败，断点续传
		{
			CIAS_LOG_HL("set asr_cmd_model.addriss to current_update_partition_offset:---0x%x \r\n", gupdate_result_param_info.current_update_partition_offset);
			waddress = p_update->asr_cmd_model.address + gupdate_result_param_info.current_update_partition_offset;
			wsize = p_update->asr_cmd_model.size - gupdate_result_param_info.current_update_partition_offset;
		}
		else
		{
			waddress = p_update->asr_cmd_model.address;
			wsize = p_update->asr_cmd_model.size;
			gupdate_result_param_info.current_update_partition_offset = 0x0;
		}
		CIAS_LOG_HL("ASR_NEED_UPDATE_MASK - 1\r\n");
		gupdate_result_param_info.current_update_partition_index = AUDIO_PARTITION_ASR;
		gupdate_result_param_info.current_update_partition_status = 0xf; //修改为失败状态，等待升级完成，修改为成功
		update_result_param_info_write();
		rc = cias_ota_update_songle_partition(OTA_ASR_NAME, waddress, wsize);
		CIAS_ASSERT_RETURN(rc == 0);
		CIAS_LOG_HL("ASR_NEED_UPDATE_MASK - 2\r\n");
		gupdate_result_param_info.current_update_partition_offset = 0x0;
		gupdate_result_param_info.current_update_partition_status = 0xe; //升级成功，修改升级状态为成功
		update_result_param_info_write();
	}
	if (need_update & VOICE_NEED_UPDATE_MASK) //需要升级voice
	{
		CIAS_LOG_HL("VOICE_NEED_UPDATE_MASK is update....\r\n");
		if ((gupdate_result_param_info.current_update_partition_status == 0x0f) && (gupdate_result_param_info.current_update_partition_index == AUDIO_PARTITION_VOICE)) //升级失败，断点续传
		{
			CIAS_LOG_HL("set voice.addriss to current_update_partition_offset:---0x%x \r\n", gupdate_result_param_info.current_update_partition_offset);
			waddress = p_update->voice.address + gupdate_result_param_info.current_update_partition_offset;
			wsize = p_update->voice.size - gupdate_result_param_info.current_update_partition_offset;
		}
		else
		{
			waddress = p_update->voice.address;
			wsize = p_update->voice.size;
			gupdate_result_param_info.current_update_partition_offset = 0x0;
		}
		gupdate_result_param_info.current_update_partition_index = AUDIO_PARTITION_VOICE;
		gupdate_result_param_info.current_update_partition_status = 0xf; //修改为失败状态，等待升级完成，修改为成功
		CIAS_LOG_HL("waddress = 0x%x, wsize = 0x%x\r\n", waddress, wsize);
		update_result_param_info_write();
		rc = cias_ota_update_songle_partition(OTA_VOICE_NAME, waddress, wsize);
		CIAS_ASSERT_RETURN(rc == 0);
		gupdate_result_param_info.current_update_partition_offset = 0x0;
		gupdate_result_param_info.current_update_partition_status = 0xe; //升级成功，修改升级状态为成功
		update_result_param_info_write();
	}
	if (need_update & DNN_NEED_UPDATE_MASK) //需要升级DNN-由于文件过大，放到最后来升级
	{
		CIAS_LOG_HL("DNN_NEED_UPDATE_MASK is update....\r\n");
		if ((gupdate_result_param_info.current_update_partition_status == 0x0f) && (gupdate_result_param_info.current_update_partition_index == AUDIO_PARTITION_DNN)) //升级失败，断点续传
		{
			CIAS_LOG_HL("----set dnn_model.addriss to current_update_partition_offset:---0x%x \r\n", gupdate_result_param_info.current_update_partition_offset);
			waddress = p_update->dnn_model.address + gupdate_result_param_info.current_update_partition_offset;
			wsize = p_update->dnn_model.size - gupdate_result_param_info.current_update_partition_offset;
			// updating_partition_t *updating = &(cias_ota->updating);
			// updating->need_offset = gupdate_result_param_info.current_update_partition_offset;
		}
		else
		{
			waddress = p_update->dnn_model.address;
			wsize = p_update->dnn_model.size;
			gupdate_result_param_info.current_update_partition_offset = 0x0;
		}
		gupdate_result_param_info.current_update_partition_index = AUDIO_PARTITION_DNN;
		gupdate_result_param_info.current_update_partition_status = 0xf; //修改为失败状态，等待升级完成，修改为成功
		CIAS_LOG_HL("waddress = 0x%x, wsize = 0x%x\r\n", waddress, wsize);
		update_result_param_info_write();
		rc = cias_ota_update_songle_partition(OTA_DNN_NAME, waddress, wsize);
		CIAS_ASSERT_RETURN(rc == 0);
		gupdate_result_param_info.current_update_partition_offset = 0x0;
		gupdate_result_param_info.current_update_partition_status = 0xe; //升级成功，修改升级状态为成功
		update_result_param_info_write();
	}
	if (need_update & USERFILE_NEED_UPDATE_MASK) //需要升级user file
	{
		CIAS_LOG_HL("USERFILE_NEED_UPDATE_MASK is update....\r\n");
		update_result_param_info_read();
		if ((gupdate_result_param_info.current_update_partition_status == 0x0f) && (gupdate_result_param_info.current_update_partition_index == AUDIO_PARTITION_USERFILE)) //升级失败，断点续传
		{
			CIAS_LOG_HL("set user_file.addriss to current_update_partition_offset:---0x%x \r\n");
			waddress = p_update->user_file.address + gupdate_result_param_info.current_update_partition_offset;
			wsize = p_update->user_file.size - gupdate_result_param_info.current_update_partition_offset;
		}
		else
		{
			waddress = p_update->user_file.address;
			wsize = p_update->user_file.size;
			gupdate_result_param_info.current_update_partition_offset = 0x0;
		}
		gupdate_result_param_info.current_update_partition_index = AUDIO_PARTITION_USERFILE;
		gupdate_result_param_info.current_update_partition_status = 0xf; //修改为失败状态，等待升级完成，修改为成功
		update_result_param_info_write();
		rc = cias_ota_update_songle_partition(OTA_USER_FILE_NAME, waddress, wsize);
		CIAS_ASSERT_RETURN(rc == 0);
		gupdate_result_param_info.current_update_partition_offset = 0x0;
		gupdate_result_param_info.current_update_partition_status = 0xe; //升级成功，修改升级状态为成功
		update_result_param_info_write();
	}
	if (need_update & USER2_NEED_UPDATE_MASK) //需要升级user code 2-由于文件过大，放到最后更新
	{
		CIAS_LOG_HL("USER2_NEED_UPDATE_MASK is update....\r\n");
		if ((gupdate_result_param_info.current_update_partition_status == 0x0f) && (gupdate_result_param_info.current_update_partition_index == AUDIO_PARTITION_BACKUP_CODE)) //升级失败，断点续传
		{
			CIAS_LOG_HL("set user_code2.addriss to current_update_partition_offset:---0x%x \r\n", gupdate_result_param_info.current_update_partition_offset);
			waddress = p_update->user_code2.address + gupdate_result_param_info.current_update_partition_offset;
			wsize = p_update->user_code2.size - gupdate_result_param_info.current_update_partition_offset;
		}
		else
		{
			waddress = p_update->user_code2.address;
			wsize = p_update->user_code2.size;
			gupdate_result_param_info.current_update_partition_offset = 0x0;
		}
		gupdate_result_param_info.current_update_partition_index = AUDIO_PARTITION_BACKUP_CODE;
		gupdate_result_param_info.current_update_partition_status = 0xf; //修改为失败状态，等待升级完成，修改为成功
		// update_result_param_info_write();
		CIAS_LOG_HL("update p_update->user_code2.address = 0x%x, p_update->user_code2.size = 0x%x\r\n", p_update->user_code2.address, p_update->user_code2.size);
		CIAS_LOG_HL("waddress = 0x%x, p_update->user_code1.size = 0x%x\r\n", waddress, wsize);
		rc = cias_ota_update_songle_partition(OTA_CODE2_NAME, waddress, wsize);
		CIAS_ASSERT_RETURN(rc == 0);
		/*CIAS_LOG_HL("USER2_NEED_UPDATE_MASK - 1\r\n");
			p_update->user_code1 = cias_ota->file_info.user_code1;
			p_update->user_code1.status = USERCODE_AREA_STA_UPDATE;
			p_update->user_code2.status = USERCODE_AREA_STA_OK;
			rc = _ota_upgrade_partition_table();
			CIAS_ASSERT_RETURN(rc==0);
			/*CIAS_LOG_HL("USER1_NEED_UPDATE_MASK - 2\r\n");
			rc = cias_ota_update_songle_partition(OTA_CODE1_NAME, p_update->user_code1.address, p_update->user_code1.size);
			CIAS_ASSERT_RETURN(rc==0);*/
		gupdate_result_param_info.current_update_partition_offset = 0x0;
		gupdate_result_param_info.current_update_partition_status = 0xe; //升级成功，修改升级状态为成功
		// update_result_param_info_write();
	}

	#if 0
	//更新OTA状态
	// cias_ota->update_info->ota_statue = OTA_UPDATE_SUCCESS;
	//完全更新code1和code2后，以code1为启动区，code2为备份区
	p_update->user_code1.status = USERCODE_AREA_STA_OK;
	p_update->user_code2.status = USERCODE_AREA_STA_OLD;
	p_update->asr_cmd_model.status = OTHER_AREA_OK;
	p_update->dnn_model.status = OTHER_AREA_OK;
	p_update->voice.status = OTHER_AREA_OK;
	p_update->user_file.status = OTHER_AREA_OK;
	rc = _ota_upgrade_partition_table();
	CIAS_ASSERT_RETURN(rc == 0);
	#endif

	return 0;
}
/**
 * @brief 摧毁ota模块，释放内存，断开http链接
 *
 */
void cias_http_ota_destory(void)
{
	if (cias_ota != NULL)
	{
		cias_ota_close_session(&(cias_ota->session));
		cias_olcp_destory(cias_ota->ota_olcp);
		cias_free(cias_ota->updating.buff);
		cias_free(cias_ota);
	}
}

/**
 * @brief 打印需要跟新的分区
 *
 * @param need_update_partition: 需要跟新分区信息
 */
void _printf_need_update_partition(int need_update_partition)
{
	CIAS_LOG_HL("Need update partition:");
	if (need_update_partition == 0)
	{
		CIAS_LOG_HL("    -NULL");
	}
	if (USER1_NEED_UPDATE_MASK & need_update_partition)
	{
		CIAS_LOG_HL("    -" OTA_CODE1_NAME);
	}
	if (USER2_NEED_UPDATE_MASK & need_update_partition)
	{
		CIAS_LOG_HL("    -" OTA_CODE2_NAME);
	}
	if (ASR_NEED_UPDATE_MASK & need_update_partition)
	{
		CIAS_LOG_HL("    -" OTA_ASR_NAME);
	}
	if (DNN_NEED_UPDATE_MASK & need_update_partition)
	{
		CIAS_LOG_HL("    -" OTA_DNN_NAME);
	}
	if (VOICE_NEED_UPDATE_MASK & need_update_partition)
	{
		CIAS_LOG_HL("    -" OTA_VOICE_NAME);
	}
	if (USERFILE_NEED_UPDATE_MASK & need_update_partition)
	{
		CIAS_LOG_HL("    -" OTA_USER_FILE_NAME);
	}
	CIAS_LOG_INFO(" ");
}

void play_update_successful(void)
{
	cias_message_send_interface(UPDTAE_SOFT_SUCESS, DEF_FILL, 0, NULL);
}
void play_update_fail(void)
{
	cias_message_send_interface(UPDTAE_SOFT_SUCESS, DEF_FILL, 0, NULL);
}
