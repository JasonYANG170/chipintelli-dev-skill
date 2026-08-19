#ifndef __CI_QCLOUD_OTA_H__
#define __CI_QCLOUD_OTA_H__


#include <stdbool.h>
#include <stdint.h>

#define FW_RUNNING_VERSION "1.0.0"
#define KEY_VER "version"
#define KEY_SIZE "downloaded_size"

#define FW_VERSION_MAX_LEN 32
#define FW_MD5SUM_MAX_LEN 64
#define FW_FILE_PATH_MAX_LEN 128
//#define OTA_BUF_LEN 5000
#define OTA_BUF_LEN 4096 + 1
#define FW_INFO_FILE_DATA_LEN 128
#define SG_DATA_REPORT_BUFFERSIZE 260   //max 260


typedef struct OTAContextData
{
  void *ota_handle;
  void *mqtt_client;
  char fw_file_path[FW_FILE_PATH_MAX_LEN];
  char fw_info_file_path[FW_FILE_PATH_MAX_LEN];
  char md5sum[FW_MD5SUM_MAX_LEN];
  // remote_version means version for the FW in the cloud and to be downloaded
  char remote_version[FW_VERSION_MAX_LEN];
  uint32_t fw_file_size;
  uint32_t fetched_size;

  // for resuming download
  /* local_version means downloading but not running */
  char local_version[FW_VERSION_MAX_LEN];
  int downloaded_size;

  // to make sure report is acked
  bool report_pub_ack;
  int report_packet_id;

} OTAContextData;



#endif    //__CI_QCLOUD_OTA_H__