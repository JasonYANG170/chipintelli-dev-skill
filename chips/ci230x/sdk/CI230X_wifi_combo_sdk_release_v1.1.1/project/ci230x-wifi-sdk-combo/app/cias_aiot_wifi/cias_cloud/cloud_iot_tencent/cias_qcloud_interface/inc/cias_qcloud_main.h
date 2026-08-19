#ifndef __CI_QCLOUD_MAIN_H__
#define __CI_QCLOUD_MAIN_H__

#include <stdlib.h>
#include <string.h>

int cias_qcloud_iot_main(void);
void ci_iot_tencent_main_task(void *parameter);
uint8_t report_ota_status_to_iot(uint8_t status);
int get_wifi_image_from_cloud(unsigned char *buff, unsigned int offset, unsigned int len);
int get_audio_image_from_cloud(unsigned char *buff, unsigned int offset, unsigned int len);
#endif   //__CI_QCLOUD_MAIN_H__