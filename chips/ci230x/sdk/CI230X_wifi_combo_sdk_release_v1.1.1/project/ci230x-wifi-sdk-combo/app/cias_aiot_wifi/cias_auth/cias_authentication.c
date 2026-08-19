/**
 * @file cias_authentication.h
 * @brief 鉴权文件(iot+tvs)
 * @author JasonChan (x@chencc.cc)
 * @date 2021-04-19
 * @copyright Copyright (c) 2021
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define CIAS_LOG_MODULE "[AUTH]"
#include "cias_log.h"

#include "cias_authentication.h"
#include "cias_freertos_common.h"
#include "cias_slave_message_handle.h"
#include "cJSON.h"
#include "cias_common.h"
#include "Config.h"   //and by yjd

static char *sg_iot_product_id = NULL;
static char *sg_iot_device_name = NULL;
static char *sg_iot_device_secret = NULL;

static char *sg_tvs_client_id = NULL;

char tvs_client_id[250] = {"\0"};

/**
 * This function will duplicate a string.
 *
 * @param s the string to be duplicated
 *
 * @return the duplicated string pointer
 */
char *tvs_strdup(const char *s)
{
    int len = strlen(s) + 1;
    char *tmp = (char *)cias_malloc(len);

    if (!tmp)
        return NULL;

    memcpy(tmp, s, len);

    return tmp;
}

/**
 * @brief 从鉴权文件设置鉴权
 * 
 * @param data: 鉴权文件信息
 * @param data_len: 鉴权文件长度
 * @retval -1: error
 * @retval 0: successful
 */
int cias_set_auth_file(uint8_t *data, uint32_t data_len)
{
        cJSON *json = NULL;
        cJSON *product_id = NULL;
        cJSON *device_name = NULL;
        cJSON *device_secret = NULL;
        cJSON *client_id = NULL;

        CIAS_ASSERT_RETURN(data != NULL);
        data[data_len] = '\0';
        json = cJSON_Parse((char *)data);
        /**
         * @brief IOT
         * 
         */
        #if CIAS_IOT_TENCENT_ENABLE  //and by yjd
        product_id = cJSON_GetObjectItem(json, "IOT_PRODUCT_ID");
        CIAS_ASSERT_GOFAIL((product_id != NULL) && (product_id->type == cJSON_String));
        device_name = cJSON_GetObjectItem(json, "IOT_DEVICE_NAME");
        CIAS_ASSERT_GOFAIL((device_name != NULL) && (device_name->type == cJSON_String));
        device_secret = cJSON_GetObjectItem(json, "IOT_DEVICE_SECRET");
        CIAS_ASSERT_GOFAIL((device_secret != NULL) && (device_secret->type == cJSON_String));
    

        cias_auth_set_product_id(product_id->valuestring);
        cias_auth_set_device_name(device_name->valuestring);
        cias_auth_set_device_secret(device_secret->valuestring);
        
        CIAS_LOG_HL("IOT_PRODUCT_ID(%d):%s\r\n", data_len, product_id->valuestring);
        CIAS_LOG_HL("GET_DEVICE_NAME:%s\r\n", device_name->valuestring);
        CIAS_LOG_HL("IOT_DEVICE_SECRET:%s\r\n", device_secret->valuestring); 
        #endif
        #if CIAS_IOT_TVS_ENABLE
        /**
         * @brief TVS
         * 
         */
        client_id = cJSON_GetObjectItem(json, "DEVICE_CLIENT");
        CIAS_ASSERT_GOFAIL((client_id != NULL) && (client_id->type == cJSON_String));
        cias_auth_set_client_id(client_id->valuestring);
        CIAS_LOG_HL("DEVICE_CLIENT:%s\r\n", client_id->valuestring);
        #endif
__fail:
        cJSON_Delete(json);

        return 0;
}
/**
 * @brief 设置iot product id
 * 
 * @param product_id: product id
 * @retval 0: successful
 * @retval -1: error
 */
int cias_auth_set_product_id(char *product_id)
{
        CIAS_ASSERT_RETURN(product_id != NULL);
        if (sg_iot_product_id != NULL)
        {
                cias_free(sg_iot_product_id);
        }
        sg_iot_product_id = tvs_strdup(product_id);

        return 0;
}
/**
 * @brief 设置iot device name
 * 
 * @param device_name: device name
 * @retval 0: successful
 * @retval -1: error
 */
int cias_auth_set_device_name(char *device_name)
{
        CIAS_ASSERT_RETURN(device_name != NULL);
        if (sg_iot_device_name != NULL)
        {
                cias_free(sg_iot_device_name);
        }
        sg_iot_device_name = tvs_strdup(device_name);

        return 0;
}
/**
 * @brief 设置iot device secret
 * 
 * @param device_secret: device secret
 * @retval 0: successful
 * @retval -1: error
 */
int cias_auth_set_device_secret(char *device_secret)
{
        CIAS_ASSERT_RETURN(device_secret != NULL);
        if (sg_iot_device_secret != NULL)
        {
                cias_free(sg_iot_device_secret);
        }
        sg_iot_device_secret = tvs_strdup(device_secret);

        return 0;
}
/**
 * @brief 获取iot product id
 * 
 * @retval NULL: error
 * @retval !NULL: product id
 */
char *cias_auth_get_product_id(void)
{
        CIAS_ASSERT_RETURN_NULL(sg_iot_product_id != NULL);
        return sg_iot_product_id;
}
/**
 * @brief 获取iot device name
 * 
 * @retval NULL: error
 * @retval !NULL: device name
 */
char *cias_auth_get_device_name(void)
{
        CIAS_ASSERT_RETURN_NULL(sg_iot_device_name != NULL);
        return sg_iot_device_name;
}
/**
 * @brief 获取iot device secret
 * 
 * @retval NULL: error
 * @retval !NULL: device secret
 */
char *cias_auth_get_device_secret(void)
{
        CIAS_ASSERT_RETURN_NULL(sg_iot_device_secret != NULL);
        return sg_iot_device_secret;
}
/**
 * @brief 设置云小微 client id
 * 
 * @param client_id: client id
 * @retval 0: successful
 * @retval -1: error
 */
int cias_auth_set_client_id(char *client_id)
{
        CIAS_ASSERT_RETURN(client_id != NULL);
        if (sg_tvs_client_id != NULL)
        {
                cias_free(sg_tvs_client_id);
        }
        sg_tvs_client_id = tvs_strdup(client_id);
        memcpy(tvs_client_id, sg_tvs_client_id, strlen(sg_tvs_client_id)); //add 20210930

        return 0;
}
/**
 * @brief 获取云小微 client id
 * 
 * @retval NULL: error
 * @retval !NULL: client id
 */
char *cias_auth_get_client_id(void)
{
        CIAS_ASSERT_RETURN_NULL(sg_tvs_client_id != NULL);
        return sg_tvs_client_id;
}

