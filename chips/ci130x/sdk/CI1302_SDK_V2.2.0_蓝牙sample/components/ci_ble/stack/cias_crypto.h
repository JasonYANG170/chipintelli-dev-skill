#ifndef __CIAS_CRYPTO_H__
#define __CIAS_CRYPTO_H__

#include "stdint.h"
//蓝牙收发数据的加密函数，pack_data为待加/解密数据，len为数据总长度
void cias_crypto_data(uint8_t* pack_data, uint8_t len); 

#endif