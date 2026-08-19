//
// Created by Radioway on 10/19/17.
//

#include <jni.h>
#include <stdlib.h>
#include <string.h>

#define BLE_BROADCAST_CHANNEL 37
#define ADV_HEADER_LEN  13
#define HS6200_PREMBLE_LEN 1
#define HS6200_ADDR_LEN    5
#define HS6200_GUARD_LEN   2
#define HS6200_CRC_LEN     2
#define HS6200_PAYLOAD_LEN 16
#define HS6200_HEADER_LEN  (HS6200_PREMBLE_LEN + HS6200_ADDR_LEN  + HS6200_GUARD_LEN)
#define HS6200_EXTERN_LEN  (HS6200_HEADER_LEN + HS6200_CRC_LEN + HS6200_PAYLOAD_LEN)

unsigned short crc_tabccitt[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};


/*
 * static u16 crc_ccitt_generic( const unsigned char *input_str, u8 num_uint8_ts);
 *
 * The function crc_ccitt_generic() is a generic implementation of the CCITT
 * algorithm for a one-pass calculation of the CRC for a uint8_t string. The
 * function accepts an initial start value for the crc.
 */

unsigned short crc_ccitt_generic(const unsigned char *input_str, unsigned char num_uint8_ts) {

    unsigned short crc;
    const unsigned char *ptr;
    unsigned char a;

    crc = 0xffff;
    ptr = input_str;

    if (ptr != NULL) {
        for (a = 0; a < num_uint8_ts; a++) {
            crc = (crc << 8) ^ crc_tabccitt[((crc >> 8) ^ (unsigned short) *ptr++) & 0x00FF];
        }
    }

    return crc;

}  /* crc_ccitt_generic */

unsigned short get_crc16(const unsigned char *ddata, unsigned char start, unsigned char length,
                         unsigned short crc_init) {

    unsigned short crc = crc_init & 0xFFFF;
    int i;
    for (i = 0; i < length; i++) {
        crc = (crc << 8) ^ crc_tabccitt[(ddata[start + i] ^ (crc >> 8)) & 0x00FF];
    }
    return crc;
}

/**
 * @brief bit_order()
 *
 * change data from msb to lsb
 * For example: the in_data is 0x85, return 0xa1

 *
 * @param[in] in_data
 *
 * @return[out]
 **/
unsigned char bit_order(unsigned char in_data) {
    unsigned char result = 0;
    unsigned char i;
    /* change msb->lsb */
    for (i = 0; i < 8; i++) {
        result += ((in_data >> (8 - 1 - i)) & 0x1) << i;
    }
    return result;
}


static void ll_data_whitening(const unsigned char *in_data, unsigned len, unsigned channel_index,
                              unsigned char *out_data) {
    unsigned char i, j;
    unsigned char seed;

    // Do linear feedback shift register (LFSR)
    seed = 0x01;                           // Position0 = 1;
    seed |= (channel_index & (1 << 5)) >> 4; // Position1;
    seed |= (channel_index & (1 << 4)) >> 2; // Position2;
    seed |= (channel_index & (1 << 3)) << 0; // Position3;
    seed |= (channel_index & (1 << 2)) << 2; // Position4;
    seed |= (channel_index & (1 << 1)) << 4; // Position5;
    seed |= (channel_index & (1 << 0)) << 6; // Position6;

    for (i = 0; i < len; ++i) {
        unsigned char out = 0;

        for (j = 0; j < 8; ++j) {
            unsigned char factor = (seed & (1 << 6)) >> 6;
            unsigned char shift_out;

            out |= (in_data[i] ^ (factor << j)) & (1 << j);

            seed <<= 1;
            shift_out = (seed >> 7) & 0x01;
            seed = (seed & ~(1 << 0)) | shift_out;
            seed = (seed & ~(1 << 4)) | ((seed ^ (shift_out << 4)) & (1 << 4));
        }

        out_data[i] = out;
    }
}

// ˝æ›∞¸º”√‹
void Encryption_Data(unsigned char *pbuf) {
    unsigned char key;

    pbuf[0] = (pbuf[0] ^ pbuf[10]);
    pbuf[1] = (pbuf[1] ^ pbuf[10]);
    key = (pbuf[10] ^ pbuf[1]);
    pbuf[2] = (pbuf[2] ^ key);
    pbuf[3] = (pbuf[3] ^ key);
    pbuf[4] = (pbuf[4] ^ key);
    key = (pbuf[10] ^ pbuf[2]);
    pbuf[5] = (pbuf[5] ^ key);
    pbuf[6] = (pbuf[6] ^ key);
    pbuf[7] = (pbuf[7] ^ key);
    key = (pbuf[10] ^ pbuf[5]);
    pbuf[8] = (pbuf[8] ^ key);
    pbuf[9] = (pbuf[9] ^ key);
    key = (pbuf[8] ^ pbuf[9]);
    pbuf[10] =(pbuf[10] ^ key);

    unsigned char buf[16] = {0};
    memcpy(buf, pbuf, 16);
    pbuf[11] = buf[8];
    pbuf[13] = buf[9];
    pbuf[15] = buf[10];
    pbuf[8] = 76;
    pbuf[9] = buf[11];
    pbuf[10] = buf[12];
    pbuf[12] = buf[13];
    pbuf[14] = buf[14];
}

/**
 *
 * 蓝牙设备发送13个字节广播数据给APP
 *
 * ******可用数据13个字节******
 *
 * inData[0] = 包头 0x57
 * inData[1] = 蓝牙协议版本号
 * inData[2] = 上传指令 0x77
 * inData[3] = 序列号
 * inData[4] = 手机ID
 * inData[5] = 手机ID1
 * inData[6] = 蓝牙设备ID
 * inData[7] = 蓝牙设备ID1
 * inData[8] = 蓝牙设备ID2
 * inData[9] = 蓝牙设备类型
 * inData[10] = 组ID
 * inData[11] = 企业ID
 * inData[12] = 程序版本号
 *
 * ******解密接口******
 *
 * inData  13个字节
 * len     inData数据长度
 * @return inData 13个字节
 *
 **/
void hs6200DecryptionData(unsigned char *inData, unsigned char len) {
   unsigned char key = (inData[10] ^ inData[11]);
   inData[12] = (inData[12] ^ key);
   unsigned char key1 = (inData[12] ^ inData[3]);
   unsigned char key2 = (inData[12] ^ inData[4]);
   unsigned char key3 = (inData[12] ^ inData[7]);
   inData[2] =  (inData[2] ^ inData[12]);
   inData[3] = (inData[3] ^ inData[12]);
   inData[4] = (inData[4] ^ key1);
   inData[5] = (inData[5] ^ key1);
   inData[6] = (inData[6] ^ key1);
   inData[7] = (inData[7] ^ key2);
   inData[8] = (inData[8] ^ key2);
   inData[9] = (inData[9] ^ key2);
   inData[10] = (inData[10] ^ key3);
   inData[11] = (inData[11] ^ key3);
 }

/**
 *
 * APP发送26个字节广播数据给蓝牙设备
 *
 * ******输入可用数据16个字节******
 *
 * in_buf[0] = 命令;
 * in_buf[1] = 序列号
 * in_buf[2] = 手机ID
 * in_buf[3] = 手机ID1
 * in_buf[4] = 组ID
 * in_buf[5] = 组ID1 未使用默认0x00
 * in_buf[6] = 组ID2 未使用默认0x00
 * in_buf[7] = 数据
 * in_buf[8] = 数据1
 * in_buf[9] = 数据2
 * in_buf[10] = 加密密钥
 * in_buf[11] = 数据3
 * in_buf[12] = 数据4
 * in_buf[13] = 数据5
 * in_buf[14] = 数据6
 * in_buf[15] = 设备类型APP默认76
 *
 * ******加密接口******
 *
 * addr    5个字节
 * in_buf  16个字节
 * len     in_buf数据长度
 * @return out_buf 26个字节
 *
 **/
unsigned char gen_hs6200_pkt(unsigned char *addr, unsigned char *in_buf, unsigned char len,
                             unsigned char *out_buf) {
    unsigned char hsrfdata[26] = {0};
    unsigned short crc16;
    unsigned char i;

    // Radioway Encryption
    Encryption_Data(in_buf);
    // preamble
    hsrfdata[0] = ((addr[0] & 0x80) == 0x80) ? 0xAA : 0x55;
    // address
    memcpy(hsrfdata + HS6200_PREMBLE_LEN, addr, 5);
    // guard
    for (i = 0; i < HS6200_GUARD_LEN; i++) {
        hsrfdata[HS6200_PREMBLE_LEN + HS6200_ADDR_LEN + i] = addr[HS6200_ADDR_LEN - 1];
    }
    // payload
    memcpy(hsrfdata + HS6200_HEADER_LEN, in_buf, len);

    crc16 = get_crc16(hsrfdata, 1, 5, 0xFFFF);
    crc16 = get_crc16(hsrfdata, 8, len, crc16);

    hsrfdata[HS6200_HEADER_LEN + len] = (unsigned char) ((crc16 >> 8) & 0xFF);
    hsrfdata[HS6200_HEADER_LEN + len + 1] = (unsigned char) (crc16 & 0xFF);

    unsigned char lib_buf[39];

    for (i = 0; i < HS6200_EXTERN_LEN; i++) {
        lib_buf[i + ADV_HEADER_LEN] = bit_order(hsrfdata[i]);
    }

    ll_data_whitening(lib_buf, ADV_HEADER_LEN + HS6200_EXTERN_LEN, BLE_BROADCAST_CHANNEL, lib_buf);

    memcpy(out_buf, lib_buf + ADV_HEADER_LEN, HS6200_EXTERN_LEN);

    return HS6200_EXTERN_LEN;
}
