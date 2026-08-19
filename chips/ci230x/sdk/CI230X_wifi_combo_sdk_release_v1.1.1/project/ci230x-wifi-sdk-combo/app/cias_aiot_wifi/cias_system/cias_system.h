#ifndef __CIAS_SYSTEM_H__
#define __CIAS_SYSTEM_H__
#include "cias_user_config.h"
#include "Config.h"
#if CIAS_IOT_TENCENT_ENABLE
#include "cias_qcloud_iot.h"
#endif

#if CIAS_IOT_TENCENT_ENABLE
#define KV_IOT_TENCENT_PROFILE              ((const char *)"18_iot_tencent_profile")    //腾讯鉴权文件
#endif
#define KV_DEV_WIFI_NAME_PASSWD             ((const char *)"19_dev_wifi_name_passwd")   //wifi的账号和密码   


//for cias iot
#if (CIAS_IOT_TENCENT_ENABLE)
int iot_tencent_profile_set(mqtt_client_profile_t *profile);
int iot_tencent_profile_get(mqtt_client_profile_t *profile);
#endif
#if TENCENT_AP_NET_WORK_EN// add by hw 
int iot_tencent_wifi_set(char *name, char* passwd);
int iot_tencent_wifi_get(char *name, char* passwd);
#endif//TENCENT_AP_NET_WORK_EN
#endif   //__CIAS_SYSTEM_H__