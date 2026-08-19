#include <stdio.h>
#include <string.h>
#include "update.h"
#include "cias_ota_config.h"
ota_update_flag_t ota_update_flag;
unsigned long write_data_offset = 0;
unsigned char *test_ota_update_buffer;

partition_table_t *partitions_info_in_device = NULL;
partition_table_t *partitions_info_in_file = NULL;
partition_table_t *update_info = NULL;

/*------------------------- typedef --------------------------*/
enum file_type_t
{
	USER_CODE1_BIN = 1,
	USER_CODE2_BIN,
	ASR_BIN,
	DNN_BIN,
	VOICE,
	USER_FILE,
	PARTITION_TABLE,
};

typedef enum
{
	FW_FMT_VER_1 = 1, //固件格式版本1，现用于CI110X SDK和CI130X_SDK
	FW_FMT_VER_2,	  //固件格式版本2，现用于CI110X_SDK_Lite和CI112X_SDK
	FW_FMT_VER_MAX,
} fw_fmt_ver_t;

typedef enum
{
	AGENT_TYPE_MASKROM,
	AGENT_TYPE_UPDATER,
} agent_type_t;

// 传输包原始数据类型，用于传输
typedef struct
{
	unsigned char data1[7];
	unsigned char *data2;
	unsigned char data3[3];
} package_raw_data_t;

// 传输包解析结构类型，用于构造和解包
typedef struct
{
	unsigned short head;
	unsigned short data_length;
	unsigned char msg_type;
	unsigned char cmd;
	unsigned char seq;
	unsigned char *data;
	unsigned short crc;
	unsigned char tail;
} package_property_t;

// 传输包类型
typedef union
{
	package_raw_data_t raw_data;
	package_property_t property;
} package_t;

static const unsigned short crc16tab_ccitt[256] =
	{
		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
		0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
		0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
		0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
		0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
		0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
		0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
		0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
		0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
		0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
		0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
		0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
		0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
		0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
		0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
		0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
		0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
		0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
		0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
		0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
		0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
		0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
		0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
		0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
		0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
		0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
		0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
		0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
		0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
		0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

#define USE_UPDATER_ARRAY	0
#if USE_UPDATER_ARRAY
static unsigned char updater[] = {0};
#endif//USE_UPDATER_ARRAY

int send_package(package_t *package)
{
	get_cias_ota_handle()->cias_ota_communication_send(package->raw_data.data1, 7);//communication_send(package->raw_data.data1, 7);
	get_cias_ota_handle()->cias_ota_communication_send(package->raw_data.data2, package->property.data_length);//communication_send(package->raw_data.data2, package->property.data_length);
	get_cias_ota_handle()->cias_ota_communication_send(package->raw_data.data3, 3);//communication_send(package->raw_data.data3, 3);
	return 0;
}

unsigned short crc_func(unsigned short crc, const unsigned char *buf, int len)
{
	int counter;
	for (counter = 0; counter < len; counter++)
	{
		unsigned char t = *(unsigned char *)buf++;
		crc = (crc << 8) ^ crc16tab_ccitt[((crc >> 8) ^ t) & 0x00FF];
	}
	return crc;
}

package_t *new_package(unsigned char cmd, unsigned char msg_type, unsigned char seq, unsigned char *data, unsigned short data_length)
{
	package_t *package = (package_t *)malloc(sizeof(package_t));
	package->property.head = 0x0FA5;
	package->property.cmd = cmd;
	package->property.msg_type = msg_type;
	package->property.seq = seq;
	package->property.data_length = data_length;
	package->property.data = data;
	package->property.crc = crc_func(0, &package->property.msg_type, 3);
	package->property.crc = crc_func(package->property.crc, package->property.data, package->property.data_length);
	package->property.tail = 0xff;
	return package;
}

void del_package(package_t *p)
{
	free(p);
	// cias_free(p);
}

static int find_head(unsigned char *buffer, int length)
{
	int i;
	for (i = 0; i < (length - 3); i++)
	{
		if (buffer[i] == 0xA5)
		{
			if (buffer[i + 1] == 0x0F)
			{
				unsigned short *p = (unsigned short *)(buffer + i + 2);
				if (*p <= MAX_DATA_LENGTH)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

package_t *recv_package(int timeout_ms)
{
	int time_count = 0;
	package_t *ret = (package_t *)malloc(sizeof(package_t) + 1024);
	if (ret)
	{
		unsigned char *read_buffer = (unsigned char *)ret + sizeof(package_t);
		int read_len = 4;
		int find_head_flag = 1;
		int offset = 0;
		int data_length = 0;
		int readed_len = 0;
		while (1)
		{
			readed_len = get_cias_ota_handle()->cias_ota_communication_recv(read_buffer + offset, read_len);
			if (readed_len)
			{
				#if 0
				LOG(LOG_LVL_INFO, "\r\n");
				for(int i = 0;i < readed_len + offset;i++)
				{
					LOG(LOG_LVL_INFO, "%02x ",read_buffer[i]);
				}
				// LOG(LOG_LVL_INFO, "\r\n");
				#endif

				time_count = 0;
				if (find_head_flag && (offset + readed_len >= 4))
				{
					
					int head_pos = find_head(read_buffer, offset + readed_len);
					if (head_pos >= 0)
					{
						offset = offset + readed_len - head_pos;
						memmove(read_buffer, read_buffer + head_pos, offset);
						find_head_flag = 0;
						// data_length = *(unsigned short*)(read_buffer+2);
						data_length = GET_SWORD(read_buffer + 2);
						read_len = (offset >= (data_length + 10)) ? 0 : (data_length + 10 - offset);
					}
					else
					{
						memcpy(read_buffer, read_buffer + 1, offset + readed_len - 1);
						offset = offset + readed_len - 1;
						readed_len = 0;
						read_len = 1;
					}
				}
				else
				{
					offset += readed_len;
				}

				if (!find_head_flag)
				{
					if (offset >= (data_length + 10))
					{
						ret->property.data_length = *((unsigned short *)(read_buffer + 2));
						// ret->property.crc = *(unsigned short*)(read_buffer+7+ret->property.data_length);
						ret->property.crc = GET_SWORD(read_buffer + 7 + ret->property.data_length);
						ret->property.tail = read_buffer[offset - 1];

						if (ret->property.tail == 0xFF)
						{
							unsigned short crc = crc_func(0, read_buffer + 4, ret->property.data_length + 3);
							if (crc == ret->property.crc)
							{
								ret->property.msg_type = read_buffer[4];
								ret->property.cmd = read_buffer[5];
								ret->property.seq = read_buffer[6];
								ret->property.data = read_buffer + 7;
								return ret;
							}
						}
						memmove(read_buffer, read_buffer + 2, offset - 2);
						find_head_flag = 1;
						read_len = 0;
						readed_len = offset - 2;
						offset = 0;
					}
				}
			}
			else
			{
				get_cias_ota_handle()->cias_ota_task_delay_ms(1);//cias_task_delay_ms(1);
				if (time_count++ > timeout_ms)
				{
					// LOG(LOG_LVL_INFO, "timr_count = %d\r\n", time_count);
					time_count = 0;
					if(ret)
					{
						free(ret);
						// cias_free(ret);
					}
					return NULL;
				}
			}
		}
	}
	return NULL;
}

unsigned short get_partition_list_checksum(partition_table_t *file_config)
{
	int len = sizeof(partition_table_t) - 2;
	unsigned short sum = 0;
	unsigned char UserCode1Status = file_config->user_code1.status;
	unsigned char UserCode2Status = file_config->user_code2.status;
	unsigned char ASRCMDModelStatus = file_config->asr_cmd_model.status;
	unsigned char DNNModelStatus = file_config->dnn_model.status;
	unsigned char VoicePlayingStatus = file_config->voice.status;
	unsigned char UserFileStatus = file_config->user_file.status;

	file_config->user_code1.status = 0xF0;
	file_config->user_code2.status = 0xF0;
	file_config->asr_cmd_model.status = 0;
	file_config->dnn_model.status = 0;
	file_config->voice.status = 0;
	file_config->user_file.status = 0;

	for (int i = 0; i < len; i++)
	{
		sum += ((unsigned char *)file_config)[i];
	}

	file_config->user_code1.status = UserCode1Status;
	file_config->user_code2.status = UserCode2Status;
	file_config->asr_cmd_model.status = ASRCMDModelStatus;
	file_config->dnn_model.status = DNNModelStatus;
	file_config->voice.status = VoicePlayingStatus;
	file_config->user_file.status = UserFileStatus;
	return sum;
}

int cmd_send_partition_info(unsigned long addr_in_flash, int size, unsigned short crc)
{
	unsigned char data[10];
	*(unsigned long *)(data + 0) = addr_in_flash;
	*(unsigned long *)(data + 4) = size;
	*(unsigned short *)(data + 8) = crc;

	package_t *package = new_package(MSG_CMD_UPDATE_BLOCK_INFO, MSG_TYPE_CMD, 1, data, 10);
	send_package(package);
	del_package(package);
	for (;;)
	{
		get_cias_ota_handle()->cias_ota_task_delay_ms(10);//cias_task_delay_ms(10);
		package = recv_package(10);
		if (package == NULL)
		{
			del_package(package);
			return 1;
		}

		if (package->property.cmd != MSG_CMD_UPDATE_BLOCK_INFO && package->property.msg_type != MSG_TYPE_ACK)
		{
			del_package(package);
			continue;
		}

		del_package(package);
		return 0;
	}
}

int cmd_erase_partition(unsigned long addr_in_flash, int size, int erase_block_size)
{
	int time_count = 500;
	unsigned char data[12];
	*(unsigned long *)(data + 0) = addr_in_flash;
	*(unsigned long *)(data + 4) = size;
	*(unsigned long *)(data + 8) = erase_block_size;

	package_t *package = new_package(MSG_CMD_UPDATE_ERA, MSG_TYPE_CMD, 0, data, 12);
	send_package(package);
	del_package(package);
	for (;;)
	{
		package = recv_package(10);
		if (package == NULL)
		{
			time_count--;
			if (time_count > 0)
			{
				del_package(package);
				continue;
			}
			del_package(package);
			return 1;
		}

		if (package->property.cmd != MSG_CMD_UPDATE_ERA && package->property.msg_type != MSG_TYPE_ACK)
		{
			del_package(package);
			continue;
		}

		del_package(package);
		return 0;
	}
}

//对分区表升级
//对分区表升级
int ack_write_partition_data_for_table(int offset_in_file, unsigned char *pdata, int size, unsigned short *crc16)
{
	int time_count = 2000;
	int resend_count = 0;
	package_t *last_pkg = NULL;
	uint8_t p[4096 + 4] = {0};
	while (1)
	{
		package_t *pkg = recv_package(100);
		if (pkg == NULL)
		{
			if (resend_count < 3 && last_pkg)
			{
				resend_count += 1;
				// pkg = last_pkg;
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n resend last package");
				#endif
			}
			else
			{
				time_count--;
				if (time_count > 0)
				{
					continue;
				}
				return 1;
			}
		}
		else
		{
			resend_count = 0;
		}

		if (pkg->property.cmd == MSG_CMD_UPDATE_WRITE)
		{
			int rst;
			if (last_pkg && last_pkg != pkg)
			{
				del_package(last_pkg);
			}
			last_pkg = pkg;

			unsigned long offset = GET_LWORD(pkg->property.data);
			if (offset == 0)
			{
				*crc16 = 0;
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\nWrite progress: 0%%");
				#endif
			}
			unsigned long req_size = GET_LWORD(pkg->property.data + 4);
			req_size = (req_size > MAX_DATA_LENGTH) ? MAX_DATA_LENGTH : req_size;
			memset(p, 0, 4096 + 4);
			if (!p)
			{
				return 2;
			}
			*(unsigned long *)p = offset;
			if (pdata != NULL)
			{
				if (offset >= (unsigned long)size)
					rst = 0;
				else if ((int)(offset + req_size) > size)
					rst = size - offset;
				else
					rst = req_size;
				memcpy(p + 4, pdata + offset, rst);
			}
			else
			{
				#if DEBUG_LOG
				// LOG(LOG_LVL_INFO, "\r\n>>>*pdata = NULL");
				#endif
				if (offset >= (unsigned long)size)
					rst = 0;
				else if ((int)(offset + req_size) > size)
					rst = size - offset;
				else
					rst = req_size;
        		// hal_flash_read(UPDATER_FLASH_START_ADDR + offset, req_size, p + 4);//todo:add flash read func
				get_cias_ota_handle()->cias_ota_flash_read(UPDATER_FLASH_START_ADDR + offset, req_size, p + 4);
			}

			if ((rst) < (long)req_size)
			{
				memset(p + 4 + rst, 0xFF, req_size - rst);
			}

			if (resend_count == 0 || offset == 0)
			{
				*crc16 = crc_func(*crc16, p + 4, req_size);
			}

			package_t *package = new_package(MSG_CMD_UPDATE_WRITE, MSG_TYPE_ACK, 0, p, (unsigned short)req_size + 4);
			send_package(package);
			// free(p);
		}
		else if (pkg->property.cmd == MSG_CMD_PROGRESS)
		{
			pkg->property.data[pkg->property.data_length] = '\0';
			// LOG(LOG_LVL_INFO, "\r\nErase progress: %s", pkg->property.data);
			del_package(pkg);
		}
		else if (pkg->property.cmd == MSG_CMD_UPDATE_BLOCK_WRITE_DONE)
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\nWrite progress: 100%%");
			#endif
			del_package(pkg);
			if (last_pkg && last_pkg != pkg)
			{
				del_package(last_pkg);
			}
			return 0;
		}
		else
		{
			if (last_pkg)
			{
				del_package(last_pkg);
			}
			return 4;
		}
	}
}

//对user_code、dnn、asr等分区写数据升级
//对user_code、dnn、asr等分区写数据升级
//对user_code、dnn、asr等分区写数据升级
int ack_write_partition_data(int offset_in_file, unsigned char *pdata, int size, unsigned short *crc16, unsigned short update_info_crc)
{
	int ota_abc = ((int)OTA_CLOUD_SIZE)/(int)(MAX_DATA_LENGTH);
	int ota_aaa = 1;
	int ota_bbb = 1;
    int resend_count = 0;
	uint8_t p[4096 + 4] = {0};
	unsigned short crc16_cloud_data = 0;
    package_t *last_pkg = NULL;

    while(1)
    {
        package_t *pkg = recv_package(100);
		if (pkg == NULL)
        {
            // if (resend_count < 5 && last_pkg)
            if (resend_count < 30 || last_pkg)
            {
                resend_count += 1;
                pkg = last_pkg;
				// #if DEBUG_LOG
                // LOG(LOG_LVL_INFO,"\r\n resend last package");
				// #endif
			}
            else
            {
				// #if DEBUG_LOG
                // LOG(LOG_LVL_INFO,"\r\nack_write_partition_data-------------return 1");
                // #endif
				return 1;
            }
        }
        else
        {
            resend_count = 0;
        }
		if (pkg->property.cmd == MSG_CMD_UPDATE_WRITE)
        {
			int rst;
            if (last_pkg && last_pkg != pkg)
            {
                del_package(last_pkg);
            }
            last_pkg = pkg;
            write_data_offset = GET_LWORD(pkg->property.data);
			if(ota_bbb)
			{
				ota_update_flag.data_control = ENABLE_CLOUD_GET;//使能云端拉取
				ota_update_flag.offset_in_file = offset_in_file;//存分区地址
    			ota_update_flag.partions_size = size;//存分区大小
				while(ota_update_flag.data_control != ENABLE_PACKAGE_SEND)
				{
					get_cias_ota_handle()->cias_ota_task_delay_ms(5);//cias_task_delay_ms(5);//等待云端读取成功使能
				}
			}
			if(write_data_offset == 0)
            {
				#if DEBUG_LOG
                LOG(LOG_LVL_INFO,"\r\nWrite progress: 0%%");
                #endif
				*crc16 = 0;
            }
            unsigned long req_size = GET_LWORD(pkg->property.data + 4);
            req_size = (req_size > MAX_DATA_LENGTH) ? MAX_DATA_LENGTH : req_size;
			// memset(p, 0, 4096 + 4);
            if (!p)
            {
				// #if DEBUG_LOG
                // LOG(LOG_LVL_INFO,"\r\nack_write_partition_data-------------return 2");
                // #endif
				return 2;
            }
            *(unsigned long *)p = write_data_offset;
            if (pdata != NULL)
            {
                if (write_data_offset >= (unsigned long)size)
                    rst = 0;
                else if ((int)(write_data_offset + req_size) > size)
                    rst = size - write_data_offset;
                else
                    rst = req_size;
                // memcpy(p + 4, pdata, rst);
				// crc16_cloud_data = crc_func(crc16_cloud_data, pdata, rst);
                memcpy(p + 4, pdata + ((int)(ota_aaa - 1) * (int)MAX_DATA_LENGTH), rst);
                crc16_cloud_data = crc_func(crc16_cloud_data, pdata + ((int)(ota_aaa - 1) * (int)MAX_DATA_LENGTH), rst);
				if(ota_aaa < ota_abc)
				{
					ota_aaa++;
					ota_bbb = 0;
				}
				else
				{
					ota_aaa = 1;
					ota_bbb = 1;
				}
				if(rst < req_size)
				{
					if(crc16_cloud_data != update_info_crc)
					{
				#if DEBUG_LOG
						LOG(LOG_LVL_INFO, "\r\n>>>crc16_cloud_data_error!!\r\n");
				#endif
						return 3;
					}
				}
			}
            else
            {
				// #if DEBUG_LOG
                // LOG(LOG_LVL_INFO,"\r\n pdata == NULL");
				// #endif
			}

            if ((rst) < (long)req_size)
            {
                memset(p + 4 + rst, 0xFF, req_size - rst);
            }

            if (resend_count == 0 || write_data_offset == 0)
            {
                *crc16 = crc_func(*crc16, p + 4, req_size);
			}
            package_t *package = new_package(MSG_CMD_UPDATE_WRITE, MSG_TYPE_ACK, 0, p, (unsigned short)req_size+4);
			send_package(package);
            del_package(package);
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO,"\rWrite progress: %.2f%%", (float)write_data_offset/size*100.0);
			#endif
		}
        else if (pkg->property.cmd == MSG_CMD_PROGRESS)
        {
            pkg->property.data[pkg->property.data_length] = '\0';
            // LOG(LOG_LVL_INFO,"\r\n=============0x11================");

            // pkg->property.data[pkg->property.data_length] = '\0';
            // LOG(LOG_LVL_INFO,"\r\nErase progress: %s", pkg->property.data);
            del_package(pkg);
        }
        else if (pkg->property.cmd == MSG_CMD_UPDATE_BLOCK_WRITE_DONE)
        {
			#if DEBUG_LOG
            LOG(LOG_LVL_INFO,"\r\nWrite progress: 100%%");
            #endif
			del_package(pkg);
            if (last_pkg && last_pkg != pkg)
            {
                del_package(last_pkg);
            }
            return 0;
        }
        else
        {
            if (last_pkg)
            {
                del_package(last_pkg);
				// #if DEBUG_LOG
            	// LOG(LOG_LVL_INFO,"\r\nack_write_partition_data-------------return 4");
            	// #endif
				return 4;
            }
			
        }
    }
}

int cmd_verify_partition(unsigned long addr_in_flash, int size, unsigned short crc16)
{
	unsigned char data[10];
	*(unsigned long *)&data[0] = addr_in_flash;
	*(int *)&data[4] = size;
	*(unsigned short *)&data[8] = crc16;

	package_t *package = new_package(MSG_CMD_UPDATE_VERIFY, MSG_TYPE_CMD, 1, data, 10);
	send_package(package);
	del_package(package);
	for (;;)
	{
		get_cias_ota_handle()->cias_ota_task_delay_ms(10);//cias_task_delay_ms(10);
		package = recv_package(10);
		if (package == NULL)
		{
			del_package(package);
			return 1;
		}

		if (package->property.cmd != MSG_CMD_UPDATE_VERIFY && package->property.msg_type != MSG_TYPE_ACK)
		{
			del_package(package);
			continue;
		}

		if (package->property.data[0] == 1)
		{
			// #if DEBUG_LOG
			// LOG(LOG_LVL_INFO, "\r\n>>>INFO: Verify partition 0x%x ok !", (int)addr_in_flash);
			// #endif
			del_package(package);
			return 0;
		}
		else
		{
			// #if DEBUG_LOG
			// LOG(LOG_LVL_INFO, "\r\n>>>INFO: Verify partition 0x%x failed", (int)addr_in_flash);
			// #endif
			del_package(package);
			return 2;
		}
	}
}

int transport_agent(unsigned long agent_runaddr)
{
	int transport_agent_count = 0;
	unsigned short transport_agent_crc16 = 0;
#if USE_UPDATER_ARRAY
	unsigned int updater_size = sizeof(updater);
#else//USE_UPDATER_ARRAY
	unsigned int updater_size = 16280;
#endif//USE_UPDATER_ARRAY
	unsigned long erase_size = (updater_size + MIN_PARTITION_SIZE - 1) / MIN_PARTITION_SIZE * MIN_PARTITION_SIZE;
#if DEBUG_LOG
	LOG(LOG_LVL_INFO, "\r\n>>>>>>transport upgrade agent");
#endif
	/*计算crc16*/
#if USE_UPDATER_ARRAY
	transport_agent_crc16 = crc_func(0, updater, updater_size);

	for (transport_agent_count = 0; transport_agent_count < (int)erase_size - updater_size; transport_agent_count++)
	{
		const unsigned char fill_byte = 0xFF;
		transport_agent_crc16 = crc_func(transport_agent_crc16, &fill_byte, 1);
	}
	LOG(LOG_LVL_INFO, "\r\n>>>%#x, %#x, %#x", agent_runaddr, erase_size, transport_agent_crc16);
#else//USE_UPDATER_ARRAY
	// transport_agent_crc16 = 0xBF07;
	transport_agent_crc16 = 0x0641;
#endif//USE_UPDATER_ARRAY
	get_cias_ota_handle()->cias_ota_task_delay_ms(10);//cias_task_delay_ms(10);
	//发送升级代理信息（起始地址，大小,CRC）
	if (cmd_send_partition_info(agent_runaddr, erase_size, transport_agent_crc16) != 0)
	{
		return 2;
	}

	/*发送升级代理固件（循环等待 Device 请求数据包，然后发送对应数据）*/
#if USE_UPDATER_ARRAY
	if (ack_write_partition_data_for_table(0, updater, updater_size, &transport_agent_crc16) != 0)
#else//USE_UPDATER_ARRAY
	if (ack_write_partition_data_for_table(0, NULL, updater_size, &transport_agent_crc16) != 0)
#endif//USE_UPDATER_ARRAY
	{
		return 3;
	}

	/*发送校验指令*/
	if (cmd_verify_partition(agent_runaddr, erase_size, transport_agent_crc16) != 0)
	{
		return 4;
	}

	return 0;
}

/*------------------------- command manage --------------------------*/
int cmd_check_device_ready()
{
	package_t *package = new_package(MSG_CMD_UPDATE_CHECK_READY, MSG_TYPE_CMD, 1, NULL, 0);
	send_package(package);
	del_package(package);
	for (;;)
	{
		get_cias_ota_handle()->cias_ota_task_delay_ms(10);//cias_task_delay_ms(10);
		package = recv_package(10);
		if (package == NULL)
		{
			return 1;
		}

		if (package->property.cmd != MSG_CMD_UPDATE_CHECK_READY && package->property.msg_type != MSG_TYPE_ACK)
		{
			continue;
		}
		return 0;
	}
}
int cmd_polling_upgrade_request()
{
	/*循环发送进入升级命令，检测设备是否在线*/
	package_t *package = NULL;
	int resend_times = 0;
	for (;;)
	{
		package = new_package(MSG_CMD_UPDATE_REQ, MSG_TYPE_CMD, 0, NULL, 0);
		send_package(package);
		del_package(package);
		get_cias_ota_handle()->cias_ota_task_delay_ms(10);//cias_task_delay_ms(10);
		package = recv_package(10);
		if (package == NULL)
		{
			if (++resend_times > 100)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "package == NULL, resend_times:%d\r\n",resend_times);
				#endif
				return 1;
			}
			continue;
		}
		if (package->property.cmd != MSG_CMD_UPDATE_REQ || package->property.msg_type != MSG_TYPE_ACK)
		{
		#if DEBUG_LOG
			LOG(LOG_LVL_INFO, ">>>UPDATE_FAIL\r\n");
		#endif
			del_package(package);
			return 1;//continue;
		}
		return 0;
	}
}

int cmd_change_baudrate(int baudrate)
{
	package_t *package = new_package(MSG_CMD_CHANGE_BAUDRATE, MSG_TYPE_CMD, 1, (unsigned char *)&baudrate, 4);
	send_package(package);
	del_package(package);

	for (;;)
	{
		get_cias_ota_handle()->cias_ota_task_delay_ms(5);//cias_task_delay_ms(5);
		package = recv_package(10);
		if (package == NULL)
		{
			del_package(package);
			return 1;
		}

		if (package->property.cmd != MSG_CMD_CHANGE_BAUDRATE && package->property.msg_type != MSG_TYPE_ACK)
		{
			del_package(package);
			continue;
		}

		del_package(package);
		return 0;
	}
}

int cmd_set_fw_version(fw_fmt_ver_t ver)
{
	package_t *package = new_package(MSG_CMD_SET_FW_FMT_VER, MSG_TYPE_CMD, 1, (unsigned char *)&ver, 1);
	send_package(package);
	del_package(package);

	for (;;)
	{
		package = recv_package(10);
		if (package == NULL)
		{
			del_package(package);
			return 1;
		}

		if (package->property.cmd != MSG_CMD_SET_FW_FMT_VER && package->property.msg_type != MSG_TYPE_ACK)
		{
			del_package(package);
			continue;
		}

		del_package(package);
		return 0;
	}
}

int cmd_read_partition_table(partition_table_t *partitions_info)
{
	int time_count = 10;
	package_t *package = new_package(MSG_CMD_GET_INFO, MSG_TYPE_CMD, 1, NULL, 0);
	send_package(package);
	del_package(package);
	// cias_task_delay_ms(10);
	for (;;)
	{
		package = recv_package(100);
		if (package == NULL)
		{
			time_count--;
			if (time_count > 0)
			{
				del_package(package);
				continue;
			}
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: recv_package = NULL");
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: recv_package = NULL\r\n");
			#endif
			del_package(package);
			return 1;
		}

		if (package->property.cmd == MSG_CMD_PROGRESS)
		{
			package->property.data[package->property.data_length] = '\0';
			// LOG(LOG_LVL_INFO, "\r\n>>>INFO: Device firmware crc check: %s", (char *)package->property.data);
			del_package(package);
			continue;
		}

		if (package->property.cmd != MSG_CMD_GET_INFO && package->property.msg_type != MSG_TYPE_ACK)
		{
			del_package(package);
			continue;
		}
		memcpy(partitions_info, package->property.data + 8, sizeof(partition_table_t));
#if 0//分区表打印示例
	  // CIAS_LOG_HL("sizeof(info):%d\r\n", cias_ota->file_info);
		LOG(LOG_LVL_INFO,"\r\n本地1----ManufacturerID = 0x%x\r\n", partitions_info->ManufacturerID);
		LOG(LOG_LVL_INFO,"ProductID = 0x%x\r\n", partitions_info->ProductID);
		LOG(LOG_LVL_INFO,"HWName = %s\r\n", partitions_info->HWName);
		LOG(LOG_LVL_INFO,"HWVersion = 0x%0x\r\n", partitions_info->HWVersion);
		LOG(LOG_LVL_INFO,"SWName = %s\r\n", partitions_info->SWName);
		LOG(LOG_LVL_INFO,"SWVersion = 0x%0x\r\n", partitions_info->SWVersion);
		LOG(LOG_LVL_INFO,"BootLoaderVersion = 0x%0x\r\n", partitions_info->BootLoaderVersion);
		LOG(LOG_LVL_INFO,"Reserve = %s\r\n", partitions_info->Reserve);
		LOG(LOG_LVL_INFO,"user_code1.version = 0x%0x\r\n", partitions_info->user_code1.version);
		LOG(LOG_LVL_INFO,"user_code1.address = 0x%0x\r\n", partitions_info->user_code1.address);
		LOG(LOG_LVL_INFO,"user_code1.size = 0x%0x\r\n", partitions_info->user_code1.size);
		LOG(LOG_LVL_INFO,"user_code1.crc = 0x%0x\r\n", partitions_info->user_code1.crc);
		LOG(LOG_LVL_INFO,"user_code1.status = 0x%0x\r\n", partitions_info->user_code1.status);
		LOG(LOG_LVL_INFO,"user_code2.version = 0x%0x\r\n", partitions_info->user_code2.version);
		LOG(LOG_LVL_INFO,"user_code2.address = 0x%0x\r\n", partitions_info->user_code2.address);
		LOG(LOG_LVL_INFO,"user_code2.size = 0x%0x\r\n", partitions_info->user_code2.size);
		LOG(LOG_LVL_INFO,"user_code2.crc = 0x%0x\r\n", partitions_info->user_code2.crc);
		LOG(LOG_LVL_INFO,"user_code2.status = 0x%0x\r\n", partitions_info->user_code2.status);
		LOG(LOG_LVL_INFO,"asr_cmd_model.version = 0x%0x\r\n", partitions_info->asr_cmd_model.version);
		LOG(LOG_LVL_INFO,"asr_cmd_model.address = 0x%0x\r\n", partitions_info->asr_cmd_model.address);
		LOG(LOG_LVL_INFO,"asr_cmd_model.size = 0x%0x\r\n", partitions_info->asr_cmd_model.size);
		LOG(LOG_LVL_INFO,"asr_cmd_model.crc = 0x%0x\r\n", partitions_info->asr_cmd_model.crc);
		LOG(LOG_LVL_INFO,"asr_cmd_model.status = 0x%0x\r\n", partitions_info->asr_cmd_model.status);
		LOG(LOG_LVL_INFO,"dnn_model.version = 0x%0x\r\n", partitions_info->dnn_model.version);
		LOG(LOG_LVL_INFO,"dnn_model.address = 0x%0x\r\n", partitions_info->dnn_model.address);
		LOG(LOG_LVL_INFO,"dnn_model.size = 0x%0x\r\n", partitions_info->dnn_model.size);
		LOG(LOG_LVL_INFO,"dnn_model.crc = 0x%0x\r\n", partitions_info->dnn_model.crc);
		LOG(LOG_LVL_INFO,"dnn_model.status = 0x%0x\r\n", partitions_info->dnn_model.status);
		LOG(LOG_LVL_INFO,"voice.version = 0x%0x\r\n", partitions_info->voice.version);
		LOG(LOG_LVL_INFO,"voice.address = 0x%0x\r\n", partitions_info->voice.address);
		LOG(LOG_LVL_INFO,"voice.size = 0x%0x\r\n", partitions_info->voice.size);
		LOG(LOG_LVL_INFO,"voice.crc = 0x%0x\r\n", partitions_info->voice.crc);
		LOG(LOG_LVL_INFO,"voice.status = 0x%0x\r\n", partitions_info->voice.status);
		LOG(LOG_LVL_INFO,"user_file.version = 0x%0x\r\n", partitions_info->user_file.version);
		LOG(LOG_LVL_INFO,"user_file.address = 0x%0x\r\n", partitions_info->user_file.address);
		LOG(LOG_LVL_INFO,"user_file.size = 0x%0x\r\n", partitions_info->user_file.size);
		LOG(LOG_LVL_INFO,"user_file.crc = 0x%0x\r\n", partitions_info->user_file.crc);
		LOG(LOG_LVL_INFO,"user_file.status = 0x%0x\r\n", partitions_info->user_file.status);
		LOG(LOG_LVL_INFO,"ConsumerDataStartAddr = 0x%0x\r\n", partitions_info->ConsumerDataStartAddr);
		LOG(LOG_LVL_INFO,"ConsumerDataSize = 0x%0x\r\n", partitions_info->ConsumerDataSize);
		LOG(LOG_LVL_INFO,"PartitionTableChecksum = 0x%0x\r\n", partitions_info->PartitionTableChecksum);
#endif
		del_package(package);
		return 0;
	}
}

int compare_partition_info(partition_info_t *p1, partition_info_t *p2)
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

//检查哪些分区需要更新
unsigned int get_partition_update_info(partition_table_t *partitions_info_in_device, partition_table_t *partitions_info_in_file, partition_table_t *update_info)
{
	unsigned int ret = 0;

	// if (partitions_info_in_device->ManufacturerID != partitions_info_in_file->ManufacturerID)
	//{
	//	return PARTITION_INVALID_FILE_MASK;
	// }

	if (partitions_info_in_file->PartitionTableChecksum != get_partition_list_checksum(partitions_info_in_file))
	{
		return PARTITION_INVALID_FILE_MASK;
	}

	/*如果Device分区表被破坏或数据错误，直接整片升级*/
	if (partitions_info_in_device->PartitionTableChecksum != get_partition_list_checksum(partitions_info_in_device))
	{
		*update_info = *partitions_info_in_file;
		update_info->user_code1.status = 0xFC;
		update_info->user_code2.status = 0xFC;
		update_info->asr_cmd_model.status = 0xFF;
		update_info->dnn_model.status = 0xFF;
		update_info->voice.status = 0xFF;
		update_info->user_file.status = 0xFF;
		update_info->PartitionTableChecksum = get_partition_list_checksum(update_info);
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>INFO: device partition be destroyed! update all");
		#endif
		return PARTITION_UPDATE_ALL; /*Device 分区表被破坏，升级所有内容*/
	}

	*update_info = *partitions_info_in_device;
	if (partitions_info_in_device->user_code1.status == 0xFC) /*优先升级无效分区*/
	{
		update_info->user_code1 = partitions_info_in_file->user_code1;
		update_info->user_code2.status = 0xF0;
		update_info->user_code1.status = 0xFC;
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>INFO: update USER1");
		#endif
		ret |= PARTITION_USER1_FLAG_MASK;
	}
	else if (partitions_info_in_device->user_code2.status == 0xFC)
	{
		update_info->user_code2 = partitions_info_in_file->user_code2;
		update_info->user_code1.status = 0xF0;
		update_info->user_code2.status = 0xFC;
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>INFO: update USER2");
		#endif
		ret |= PARTITION_USER2_FLAG_MASK;
	}
	else /*两个 CODE 分区全部有效*/
	{
		/*code 变小优先升级code1， code变大优先升级code2*/
		if (partitions_info_in_device->user_code1.size > partitions_info_in_file->user_code1.size)
		{
			update_info->user_code1 = partitions_info_in_file->user_code1;
			update_info->user_code2.status = 0xF0;
			update_info->user_code1.status = 0xFC;
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>INFO: code 1 and 2 is valid, user become small, update USER1");
			#endif
			ret |= PARTITION_USER1_FLAG_MASK;
		}
		else if (partitions_info_in_device->user_code1.size < partitions_info_in_file->user_code1.size)
		{
			update_info->user_code2 = partitions_info_in_file->user_code2;
			update_info->user_code1.status = 0xF0;
			update_info->user_code2.status = 0xFC;
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>INFO: code 1 and 2 is valid, user become large, update USER2");
			#endif
			ret |= PARTITION_USER2_FLAG_MASK;
		}
		else /*大小相等，根据内容判断是否需要升级*/
		{
			if (compare_partition_info(&partitions_info_in_device->user_code1, &partitions_info_in_file->user_code1) != 0)
			{
				update_info->user_code1 = partitions_info_in_file->user_code1;
				update_info->user_code2.status = 0xF0;
				update_info->user_code1.status = 0xFC;
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n>>>INFO: code 1 and 2 is valid and size no change, user1 content different update USER1");
				#endif
				ret |= PARTITION_USER1_FLAG_MASK;
			}
		}
	}

	if (compare_partition_info(&partitions_info_in_device->asr_cmd_model, &partitions_info_in_file->asr_cmd_model) || partitions_info_in_device->asr_cmd_model.status != 0x00)
	{
		update_info->asr_cmd_model = partitions_info_in_file->asr_cmd_model;
		update_info->asr_cmd_model.status = 0xFF;
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>INFO: update ASR");
		#endif
		ret |= PARTITION_ASR_FLAG_MASK;
	}

	if (compare_partition_info(&partitions_info_in_device->dnn_model, &partitions_info_in_file->dnn_model) || partitions_info_in_device->dnn_model.status != 0x00)
	{
		update_info->dnn_model = partitions_info_in_file->dnn_model;
		update_info->dnn_model.status = 0xFF;
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>INFO: update DNN");
		#endif
		ret |= PARTITION_DNN_FLAG_MASK;
	}

	if (compare_partition_info(&partitions_info_in_device->voice, &partitions_info_in_file->voice) || partitions_info_in_device->voice.status != 0x00)
	{
		update_info->voice = partitions_info_in_file->voice;
		update_info->voice.status = 0xFF;
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>INFO: update VOICE");
		#endif
		ret |= PARTITION_VOICE_FLAG_MASK;
	}

	if (compare_partition_info(&partitions_info_in_device->user_file, &partitions_info_in_file->user_file) || partitions_info_in_device->user_file.status != 0x00)
	{
		update_info->user_file = partitions_info_in_file->user_file;
		update_info->user_file.status = 0xFF;
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>INFO: update USERFILE");
		#endif
		ret |= PARTITION_USERFILE_FLAG_MASK;
	}

	if (ret)
	{
		update_info->PartitionTableChecksum = get_partition_list_checksum(update_info);
	}

	return ret;
}

//分区表升级
//分区表升级
//分区表升级
int update_partition_for_table(unsigned long addr_in_flash, int offset_in_file, unsigned char *pdata, int size)
{
	unsigned short crc16 = 0;
	unsigned long erase_size = (size + MIN_PARTITION_SIZE - 1) / MIN_PARTITION_SIZE * MIN_PARTITION_SIZE;
	// LOG(LOG_LVL_INFO, "\r\n Refresh Flash addr: 0x%x; Erase size: 0x%x ", (int)addr_in_flash, (int)erase_size);
	//发送升级分区信息（起始地址，大小, CRC）
	if (cmd_send_partition_info(addr_in_flash, erase_size, crc16) != 0)
	{
		return 1;
	}

	if (cmd_erase_partition(addr_in_flash, erase_size, ERASE_BLOCK_SIZE) != 0)
	{
		return 2;
	}

	if (ack_write_partition_data_for_table(offset_in_file, pdata, size, &crc16) != 0)
	{
		return 3;
	}

	if (cmd_verify_partition(addr_in_flash, erase_size, crc16) != 0)
	{
		return 4;
	}
	return 0;
}


//code\dnn\asr\voice等分区
//code\dnn\asr\voice等分区
//code\dnn\asr\voice等分区
int update_partition(unsigned long addr_in_flash, int offset_in_file, unsigned char *pdata, int size, unsigned short update_info_crc)
{
	unsigned short crc16 = 0;
	unsigned long erase_size = (size + MIN_PARTITION_SIZE - 1) / MIN_PARTITION_SIZE * MIN_PARTITION_SIZE;
	// LOG(LOG_LVL_INFO, "\r\n Refresh Flash addr: 0x%x; Erase size: 0x%x\r\n", (int)addr_in_flash, (int)erase_size);
	crc16 = update_info->user_code2.crc;
	//发送升级分区信息（起始地址，大小, CRC）
	if (cmd_send_partition_info(addr_in_flash, erase_size, crc16) != 0)
	{
		return 1;
	}
	if (cmd_erase_partition(addr_in_flash, erase_size, ERASE_BLOCK_SIZE) != 0)
	{
		return 2;
	}
	if (ack_write_partition_data(offset_in_file, pdata, size, &crc16, update_info_crc) != 0)
	{
		return 3;
	}
	if (cmd_verify_partition(addr_in_flash, erase_size, crc16) != 0)
	{
		return 4;
	}
	return 0;
}

int cmd_update_reset_device()
{
	package_t *package = new_package(MSG_CMD_SYS_RST, MSG_TYPE_CMD, 0, NULL, 0);
	send_package(package);
	del_package(package);
	package = recv_package(10);
	if (package == NULL)
	{
		return 1;
	}
	unsigned char cmd = package->property.cmd;
	del_package(package);
	if (cmd != MSG_CMD_SYS_RST)
	{
		return 2;
	}
	return 0;
}

extern cias_ota_data_t *cias_ota_data;
int transport_firmware(int fast_baudrate)
{
	int ret = 0;
	unsigned short update_info_crc = 0;
	test_ota_update_buffer = (unsigned char *)malloc(OTA_CLOUD_SIZE);
	if (test_ota_update_buffer == NULL)
	{
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>ERROR:test_ota_update_buffer malloc failed\r\n");
		#endif
	}
	do
	{
		//检查设备是否在线, 发送设置串口高速波特率
		if (cmd_check_device_ready() != 0)
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Device is no answer ! -- updater");
			#endif
			ret = 1;
			break;
		}

		if (cmd_change_baudrate(fast_baudrate))
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: cmd_change_baudrate faild ! -- updater");
			#endif
			ret = 5;
			break;
		}

		/*设置UART1为2m波特率*/
		get_cias_ota_handle()->cias_communication_rate_change(fast_baudrate);//communication_rate_change(fast_baudrate);

		if (cmd_check_device_ready() != 0)
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Device is no answer after change baudrate ! -- updater");
			#endif
			ret = 6;
			break;
		}

		// 设置固件版本为 V1 这里必须设置，不然设备读取分区信息表数据会出错
		if (cmd_set_fw_version(FW_FMT_VER_1) != 0)
		{
			ret = 7;
			break;
		}

		//为分区表申请动态内存，然后读取设备和固件的分区信息表
		partitions_info_in_device = (partition_table_t *)malloc(sizeof(partition_table_t));
		if (!partitions_info_in_device)
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Malloc failed !");
			#endif
			ret = 8;
			break;
		}
		partitions_info_in_file = (partition_table_t *)malloc(sizeof(partition_table_t));
		if (!partitions_info_in_file)
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Malloc failed !");
			#endif
			ret = 8;
			break;
		}
		update_info = (partition_table_t *)malloc(sizeof(partition_table_t));
		if (!update_info)
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Malloc failed !");
			#endif
			ret = 8;
			break;
		}

		*partitions_info_in_file = (cias_ota_data->file_info);//获取升级固件分区表
		if (cmd_read_partition_table(partitions_info_in_device) != 0)//获取当前固件分区表
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Partition in device read failed !");
			#endif
			ret = 9;
			break;
		}

		//从分区表查询需要更新的分区，若有则先更新分区表
		unsigned update_flag = get_partition_update_info(partitions_info_in_device, partitions_info_in_file, update_info);
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>INFO: Update flag: 0x%x", update_flag);
		#endif
		if (update_flag == PARTITION_INVALID_FILE_MASK)
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Invalid upgrade file !");
			#endif
			ret = 10;
			break;
		}
		else if (update_flag) //如果有分区需要更新，先更新分区表
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n\r\n======update partition 1======");
			#endif
			if (update_partition_for_table(PARTITION_TABLE1_START_ADDR, 0, (unsigned char *)update_info, sizeof(partition_table_t)) != 0)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n update partition list1 failed\n");
				#endif
				ret = 11;
				break;
			}
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n\r\n======update partition 2======");
			#endif
			if (update_partition_for_table(PARTITION_TABLE2_START_ADDR, 0, (unsigned char *)update_info, sizeof(partition_table_t)) != 0)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n update partition list2 failed\n");
				#endif
				ret = 12;
				break;
			}
		}
		else
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n>>>INFO: No partitions that need to be upgraded were found !");
			#endif
			ret = 19;
			break;
		}
		//分区升级
		/*USER COED*/
		unsigned char priority_code = 0; //优先升级的code（1或2）
		unsigned long code_offset_addr = 0;
		unsigned long code_size = 0;

		if (update_flag & PARTITION_USER1_FLAG_MASK)
		{
			priority_code = 1;
			code_offset_addr = update_info->user_code1.address;
			code_size = update_info->user_code1.size;
			update_info_crc = update_info->user_code1.crc;
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n======update user code1======");
			#endif
		}
		else if (update_flag & PARTITION_USER2_FLAG_MASK)
		{
			priority_code = 2;
			code_offset_addr = update_info->user_code2.address;
			code_size = update_info->user_code2.size;
			update_info_crc = update_info->user_code2.crc;
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n======update user code2======");
			#endif
		}
		if (priority_code != 0) /*需要升级USER CODE*/
		{
			if (update_partition(code_offset_addr, code_offset_addr, (unsigned char *)test_ota_update_buffer, code_size,update_info_crc) != 0) /*先升级一份usercode*/
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n>>>ERROR:update user code failed");
				#endif
				ret = 13;
				break;
			}
			if (priority_code == 1) /*优先升级的是code1, 后面继续升级code2*/
			{
				update_info->user_code2 = partitions_info_in_file->user_code2;
				update_info->user_code1.status = 0xF0;
				update_info->user_code2.status = 0xFC;
				code_offset_addr = update_info->user_code2.address;
				code_size = update_info->user_code2.size;
			}
			else /*优先升级的是code2, 后面继续升级code1*/
			{
				update_info->user_code1 = partitions_info_in_file->user_code1;
				update_info->user_code2.status = 0xF0;
				update_info->user_code1.status = 0xFC;
				code_offset_addr = update_info->user_code1.address;
				code_size = update_info->user_code1.size;
			}
			update_info->PartitionTableChecksum = get_partition_list_checksum(update_info); /*更改了分区表的内容需要重新计算校验和*/
			#if DEBUG_LOG																			/*升级前先更新分区表*/
			LOG(LOG_LVL_INFO, "\r\n======refresh partition 1 after update user======");
			#endif
			if (update_partition_for_table(PARTITION_TABLE1_START_ADDR, 0, (unsigned char *)update_info, sizeof(partition_table_t)) != 0)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n update partition list1 failed\n");
				#endif
				ret = 11;
				break;
			}
			#if DEBUG_LOG																			/*升级前先更新分区表*/
			LOG(LOG_LVL_INFO, "\r\n======refresh partition 2======");
			#endif
			if (update_partition_for_table(PARTITION_TABLE2_START_ADDR, 0, (unsigned char *)update_info, sizeof(partition_table_t)) != 0)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n update partition list2 failed\n");
				#endif
				ret = 12;
				break;
			}

			/*继续升级第二份code*/
			if (priority_code == 1)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n======update user code2======");
				#endif
			}
			else
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n======update user code1======");
				#endif
			}
			if (update_partition(code_offset_addr, code_offset_addr, (unsigned char *)test_ota_update_buffer, code_size,update_info_crc) != 0)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n======update user code failed");
				#endif
				ret = 14;
				break;
			}
		}
		/*ASR*/
		if (update_flag & PARTITION_ASR_FLAG_MASK)
		{
			update_info_crc = update_info->asr_cmd_model.crc;
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n======update asr======");
			#endif
			if (update_partition(update_info->asr_cmd_model.address, update_info->asr_cmd_model.address, (unsigned char *)test_ota_update_buffer, update_info->asr_cmd_model.size, update_info_crc) != 0)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n======update asr failed");
				#endif
				ret = 15;
				break;
			}
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n");
			#endif
		}

		/*DNN*/
		if (update_flag & PARTITION_DNN_FLAG_MASK)
		{
			update_info_crc = update_info->dnn_model.crc;
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n======update dnn======");
			#endif
			if (update_partition(update_info->dnn_model.address, update_info->dnn_model.address, (unsigned char *)test_ota_update_buffer, update_info->dnn_model.size, update_info_crc) != 0)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n======update dnn failed");
				#endif
				ret = 16;
				break;
			}
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n");
			#endif
		}

		/*VOICE*/
		if (update_flag & PARTITION_VOICE_FLAG_MASK)
		{
			update_info_crc = update_info->voice.crc;
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n======update voice======");
			#endif
			if (update_partition(update_info->voice.address, update_info->voice.address, (unsigned char *)test_ota_update_buffer, update_info->voice.size, update_info_crc) != 0)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n======update voice failed");
				#endif
				ret = 17;
				break;
			}
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n");
			#endif
		}

		/*USERFILE*/
		if (update_flag & PARTITION_USERFILE_FLAG_MASK)
		{
			update_info_crc = update_info->user_file.crc;
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n======update user file======");
			#endif
			if (update_partition(update_info->user_file.address, update_info->user_file.address, (unsigned char *)test_ota_update_buffer, update_info->user_file.size, update_info_crc) != 0)
			{
				#if DEBUG_LOG
				LOG(LOG_LVL_INFO, "\r\n======update user_file failed");
				#endif
				ret = 18;
				break;
			}
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n");
			#endif
		}

		// 5. 分区升级完成，复位设备
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n======reset device");
		#endif
		if (cmd_update_reset_device() != 0)
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n======Device reset failed !");
			#endif
		}
		else
		{
			#if DEBUG_LOG
			LOG(LOG_LVL_INFO, "\r\n======Device reset success !");
			#endif
		}

	} while(0);

	// 释放资源
	#if 0
	if (partitions_info_in_device)
	{
		cias_free(partitions_info_in_device);
	}
	if (partitions_info_in_file)
	{
		cias_free(partitions_info_in_file);
	}
	if (update_info)
	{
		cias_free(update_info);
	}
	if(test_ota_update_buffer)
	{
		cias_free(test_ota_update_buffer);
	}
	#endif
	return ret;
}

int update_ota_main()
{
	int updater_flag = 0;
	int ret = 0;

	get_cias_ota_handle()->cias_audio_device_reboot();//cias_ln_audio_reset();//复位audio端进入bootloader
	get_cias_ota_handle()->cias_ota_task_delay_ms(20);//cias_task_delay_ms(20);
	get_cias_ota_handle()->cias_communication_rate_change(OTA_BOOTLOADER_BAUDRATE);//communication_rate_change(115200);

	//轮询检查设备是否在线
	if (cmd_polling_upgrade_request())
	{
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Device is no answer !");
		#endif
		ret = 1;
		return ret;
	}

	//向设备传输升级代理（updater）
	if (updater_flag = transport_agent(PROGRAM_AGENT_ADDR))
	{
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>ERROR: agent send failed !!!%d", updater_flag);
		#endif
		ret = updater_flag;
		return ret;
	}

	//延时等待Device进入Updater执行
	get_cias_ota_handle()->cias_ota_task_delay_ms(200);//cias_task_delay_ms(200);

	//切换波特率，与Updater通信，传输升级固件
	if ((ret = transport_firmware(OTA_UPDATER_BAUDRATE)) != 0)
	{
		#if DEBUG_LOG
		LOG(LOG_LVL_INFO, "\r\n>>>ERROR: Firmware transport failed !!!");
		#endif
		return ret;
	}

	return ret;
}