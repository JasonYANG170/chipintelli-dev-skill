#ifndef __CIAS_WIFI_OTA_CONFIG_H__
#define __CIAS_WIFI_OTA_CONFIG_H__

#include <stdint.h>
#include <stdio.h>
#define WIFI_OTA_SECTOR_SIZE_4KB        (1024 * 4)
#define WIFI_OTA_SPACE_OFFSET           (0x0011A000)
#define WIFI_OTA_TASK_PRIORITY          4

#define WIFI_OTA_ENTRY_TASK_SIZE        1*1024
#define WIFI_OTA_PROGRESS_TASK_SIZE     2*1024
#define WIFI_OTA_GET_TASK_SIZE          3*1024

#define WIFI_OTA_DEBUG                  1
typedef enum
{
    WIFI_OTA_GET_START = 1,
    WIFI_OTA_WRITE_START = 2,
}wifi_ota_status_t;

typedef struct 
{
    /*从云端获取wifi升级数据*/
    int (*cias_get_wifi_image_from_cloud)(unsigned char *, unsigned int, unsigned int len);
    /*上报升级状态到云端回调*/
    uint8_t (*cias_report_ota_status_to_iot)(uint8_t);
}cias_wifi_ota_handle_t;

int cias_wifi_ota_main(void);
#endif//__CIAS_WIFI_OTA_CONFIG_H__