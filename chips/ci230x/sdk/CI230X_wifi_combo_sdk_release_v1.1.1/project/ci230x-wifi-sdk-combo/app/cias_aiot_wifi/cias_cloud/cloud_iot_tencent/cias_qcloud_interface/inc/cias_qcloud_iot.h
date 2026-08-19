#ifndef __CI_QCLOUD_IOT_H__
#define __CI_QCLOUD_IOT_H__

#include "qcloud_iot_export.h"

#define TOTAL_PROPERTY_COUNT        8
#define DEVICE_INDEX 3

#if (DEVICE_INDEX == 0)
#define CI_PRODUCT_ID      "QPDJ7ERDET"
#define CI_DEVICE_NAME     "device01"
#define CI_DEVICE_SECRET   "D079qJz2eOhwTCzQXhlpqQ=="
#elif (DEVICE_INDEX == 1)
#define CI_PRODUCT_ID      "R91HWMIKIU"
#define CI_DEVICE_NAME     "00001"
#define CI_DEVICE_SECRET   "Fd+8aIF1/Vcr1VkHoufArA=="
#elif (DEVICE_INDEX == 2)
#define CI_PRODUCT_ID      "QPDJ7ERDET"
#define CI_DEVICE_NAME     "device02"
#define CI_DEVICE_SECRET   "j72mc+0/FkcD8cfKxyig5Q=="
#elif (DEVICE_INDEX == 3)
#define CI_PRODUCT_ID      "TV5DGH8IHX"
#define CI_DEVICE_NAME     "TANMI_M1_18E1DE010009"
#define CI_DEVICE_SECRET   "kaouWVXpx1AaNoq/khsnhw=="
#endif

#define CI_DEVICE_MAC      "\x18\xee\xdd\xff\xff\x0f"   //
typedef struct mqtt_client_profile
{
    char qcloud_iot_product_id[MAX_SIZE_OF_PRODUCT_ID+1];
    char qcloud_iot_device_name[MAX_SIZE_OF_DEVICE_NAME+1];
    char qcloud_iot_device_secret[MAX_SIZE_OF_DEVICE_SECRET+1];
    //char qcloud_iot_dev_mac[13];
}mqtt_client_profile_t;


char * ci_mqtt_get_product_id(void);
char * ci_mqtt_get_device_name(void);
char * ci_mqtt_get_device_secret(void);

int cias_mqtt_client_init(mqtt_client_profile_t *profile_info);

//static bool qcloud_iot_device_connect_cloud_status = false; // device_connect_cloud_state

#endif   //__CI_QCLOUD_IOT_H__
