/**
 * @file cias_crc.h
 * @brief crc校验计算
 * @author JasonChan (x@chencc.cc)
 * @date 2021-04-20
 * @copyright Copyright (c) 2021
 */
#ifndef _CIAS_CRC_H_
#define _CIAS_CRC_H_

#ifdef __cplusplus
    extern "C" {
#endif
/**
 * @description: CRC16校验函数
 * @param { crc: 从此基础上再继续计算CRC16
 *          buf: 需要计算的数据buf
 *          len: 需要计算的长度} 
 * @return {crc16计算结果} 
 */
unsigned short crc16_func(unsigned short crc,unsigned char *buf, unsigned int len);
/**
 * @description: CRC8校验函数
 * @param { crc: 从此基础上再继续计算CRC8
 *          buf: 需要计算的数据buf
 *          len: 需要计算的长度} 
 * @return {crc8计算结果} 
 */
unsigned char crc8_func(unsigned char crc, unsigned char *buf, unsigned int len);

#ifdef __cplusplus
}
#endif
#endif
