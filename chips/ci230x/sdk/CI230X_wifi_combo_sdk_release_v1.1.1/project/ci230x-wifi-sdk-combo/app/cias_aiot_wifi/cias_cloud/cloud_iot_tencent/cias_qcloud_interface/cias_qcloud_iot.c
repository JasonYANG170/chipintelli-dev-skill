/*
 * @FileName:: 
 * @Author: hongchuan.wu
 * @Date: 2022-03-19 09:25:37
 * @LastEditTime: 2022-03-19 16:08:51
 * @Description: 
 */
#include <stdlib.h>
#include <string.h>
#include "cias_qcloud_iot.h"
#include "cias_user_config.h"
#include "cias_log.h"
#include "cias_freertos_debug.h"
//static void * cias_qcloud_main_handler;

static bool qcloud_mqtt_device_init_flag = false;
static mqtt_client_profile_t *qcloud_mqtt_device_info_handle = NULL;

char * ci_mqtt_get_product_id(void)
{
    if(qcloud_mqtt_device_info_handle)
    {
        return qcloud_mqtt_device_info_handle->qcloud_iot_product_id;
    }
    return NULL;
}

char * ci_mqtt_get_device_name(void)
{
    if(qcloud_mqtt_device_info_handle)
    {
        return qcloud_mqtt_device_info_handle->qcloud_iot_device_name;
    }
    return NULL;
}

char * ci_mqtt_get_device_secret(void)
{
    if(qcloud_mqtt_device_info_handle)
    {
        return qcloud_mqtt_device_info_handle->qcloud_iot_device_secret;
    }
    return NULL;
}



static int ci_mqtt_client_info_init(mqtt_client_profile_t *profile_info)
{
    if(profile_info == NULL)
    {
        return -1;
    }
    if(qcloud_mqtt_device_init_flag == false)
    {
        qcloud_mqtt_device_info_handle = (mqtt_client_profile_t*)malloc(sizeof(mqtt_client_profile_t));
        if(qcloud_mqtt_device_info_handle == NULL)
        {
            return -1;
        }

        memset((char *)qcloud_mqtt_device_info_handle,'\0',sizeof(mqtt_client_profile_t));
        memcpy((char *)qcloud_mqtt_device_info_handle->qcloud_iot_product_id,profile_info->qcloud_iot_product_id,MAX_SIZE_OF_PRODUCT_ID+1);
        memcpy((char *)qcloud_mqtt_device_info_handle->qcloud_iot_device_name,profile_info->qcloud_iot_device_name,MAX_SIZE_OF_DEVICE_NAME+1);
        memcpy((char *)qcloud_mqtt_device_info_handle->qcloud_iot_device_secret,profile_info->qcloud_iot_device_secret,MAX_SIZE_OF_DEVICE_SECRET+1);
        CIAS_PRINT_DEBUG("init product_id:%s",ci_mqtt_get_product_id());
        CIAS_PRINT_DEBUG("init device_name:%s",ci_mqtt_get_device_name());
        CIAS_PRINT_DEBUG("init device_secret:%s",ci_mqtt_get_device_secret());
        qcloud_mqtt_device_init_flag = true;
        return 0;
    }
    return -1;
}

int cias_mqtt_client_init(mqtt_client_profile_t *profile_info)
{
    if(ci_mqtt_client_info_init(profile_info) < 0)
    {
        return CIAS_FAIL;
    }
    return CIAS_OK;
}