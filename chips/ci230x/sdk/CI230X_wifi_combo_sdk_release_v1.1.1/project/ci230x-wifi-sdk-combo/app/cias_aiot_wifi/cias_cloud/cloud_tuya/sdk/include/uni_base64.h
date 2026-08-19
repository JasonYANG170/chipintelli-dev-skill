/**
 * @file uni_base64.h
 * @brief 
 * @author nzy
 * @version 1.0.0
 * @date 2015-06-09
 */
#ifndef _UNI_BASE64_H
#define _UNI_BASE64_H

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief tuya_base64_encode 
 *
 * @param[in] bin_data
 * @param[out] base64
 * @param[in] bin_length
 *
 * @return 
 */
char * tuya_base64_encode( const unsigned char * bin_data, char * base64, int bin_length );

/**
 * @brief tuya_base64_decode 
 *
 * @param[out] base64
 * @param[in] bin_data
 *
 * @return 
 */
int tuya_base64_decode( const char * base64, unsigned char * bin_data );

#ifdef __cplusplus
}
#endif
#endif

