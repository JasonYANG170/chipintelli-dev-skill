/**
 * @file asr_decoder_port.h
 * @brief 
 * @version 0.1
 * @date 2019-06-19
 * 
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 * 
 */

#include "stdint.h"

#ifndef _ASR_DECODER_PORT_H_
#define _ASR_DECODER_PORT_H_


#ifdef __cplusplus
extern "C"
{
#endif    

extern void *decoder_port_malloc(int size);
extern void decoder_port_free(void *pp);
void* malloc_insram_bnpu(int size);
void free_insram_bnpu(void* p);
int check_bufaddr_is_insram(unsigned int addr ,int size);


#ifdef __cplusplus
}
#endif

#endif
