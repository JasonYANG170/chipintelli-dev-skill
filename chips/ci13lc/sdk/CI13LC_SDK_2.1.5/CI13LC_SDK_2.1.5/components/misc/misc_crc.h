/**
  ******************************************************************************
  * @file  misc_crc.h
  * @author  wenfeng.wang@chipintelli.com
  * @version V1.0.0
  * @date  2018.10.06
  * @brief 
  ******************************************************************************
  **/ 

#ifndef _MISC_CRC_H_
#define _MISC_CRC_H_

#ifdef __cplusplus
extern "C" {
#endif

uint16_t crc_func(uint16_t crc,uint8_t * buf,uint32_t len);


#ifdef __cplusplus
}
#endif

#endif

