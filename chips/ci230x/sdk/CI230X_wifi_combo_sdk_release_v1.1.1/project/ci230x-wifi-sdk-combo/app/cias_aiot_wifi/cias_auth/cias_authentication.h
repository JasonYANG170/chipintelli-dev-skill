/**
 * @file cias_authentication.h
 * @brief 鉴权文件(iot+tvs)
 * @author JasonChan (x@chencc.cc)
 * @date 2021-04-19
 * @copyright Copyright (c) 2021
 */

#ifndef _CIAS_AUTHENTICATION__
#define _CIAS_AUTHENTICATION__

/**
 * @brief 从鉴权文件设置鉴权
 * 
 * @param data: 鉴权文件信息
 * @param data_len: 鉴权文件长度
 * @retval -1: error
 * @retval 0: successful
 */
extern int cias_set_auth_file(uint8_t *data, uint32_t data_len);
/**
 * @brief 设置iot product id
 * 
 * @param product_id: product id
 * @retval 0: successful
 * @retval -1: error
 */
extern int cias_auth_set_product_id(char *product_id);
/**
 * @brief 设置iot device name
 * 
 * @param device_name: device name
 * @retval 0: successful
 * @retval -1: error
 */
extern int cias_auth_set_device_name(char *device_name);
/**
 * @brief 设置iot device secret
 * 
 * @param device_secret: device secret
 * @retval 0: successful
 * @retval -1: error
 */
extern int cias_auth_set_device_secret(char *device_secret);
/**
 * @brief 获取iot product id
 * 
 * @retval NULL: error
 * @retval !NULL: product id
 */
extern char *cias_auth_get_product_id(void);
/**
 * @brief 获取iot device name
 * 
 * @retval NULL: error
 * @retval !NULL: device name
 */
extern char *cias_auth_get_device_name(void);
/**
 * @brief 获取iot device secret
 * 
 * @retval NULL: error
 * @retval !NULL: device secret
 */
extern char *cias_auth_get_device_secret(void);
/**
 * @brief 设置云小微 client id
 * 
 * @param client_id: client id
 * @retval 0: successful
 * @retval -1: error
 */
extern int cias_auth_set_client_id(char *client_id);
/**
 * @brief 获取云小微 client id
 * 
 * @retval NULL: error
 * @retval !NULL: client id
 */
extern char *cias_auth_get_client_id(void);

#endif

