/*============================================================================*/
/* @file aes_ccm_ble.c
 * @brief AES CCM for BLE implementation described in BLUETOOTH CORE SPECIFICATION Vol 6, Part E
 * @author onmicro
 * @date 2020/02
 */

#include <stdint.h>
#include <string.h>
#include "exe_int_crypto.h"

#define  EXE_NDA_FEAT_SMP 1

#if EXE_NDA_FEAT_SMP || EXE_NDA_FEAT_ENC

/**
 * @brief 使用CBC模式计算明文MIC。
 * @note payload in 27-byte: plainMIC = aes(aes(aes(aes(B0) ^ B1) ^ B2) ^ B3)[3:0] in big endian.
 *  B0,B1: Flags||Nonce||Length, 0x00||0x01||LLID
 *  B2,B3: Payload
 *
 * @param [in] p_nonce     - CCM Nonce.
 *        [in] p_payload   - The payload in ble packet w/o header.
 *        [in] payload_len - The length of payload.
 *        [in] hdr0        - The 1st byte of header in ble packet, NESN,SN,MD bits are masked to 0.
 *        [out] p_mic      - The calculated plainMIC in 4-byte.
 */
void aes_ccm_ble_cbc_mic(uint8_t *p_nonce, uint8_t *p_payload, uint16_t payload_len, uint8_t hdr0, uint8_t *p_mic)
{
  uint16_t r3_plen;
  int r7_bytes;
  uint8_t r6_b1len;
  int idx;
  int r4_plen;
  int stk4_blkidx;
  int stk12_len;
  uint8_t stk16_blk[16];
  uint8_t stk32_tbuf[16];
  uint8_t stk48_blk1[16];
  /* 还没有被处理的payload长度。*/
  r4_plen = payload_len;

  memset(&stk16_blk[0], 0x00, 16);
  memset(&stk32_tbuf[0], 0x00, 16);
  memset(&stk48_blk1[0], 0x00, 16);
  /* 2.2   COUNTER MODE BLOCKS */

  /* Table 2.2:  Block B0 format */
  stk16_blk[0] = 0x49; //Flags
  memcpy(&stk16_blk[1], p_nonce, 13);
  stk16_blk[14] = payload_len >> 8;
  stk16_blk[15] = payload_len & 0xff;

  /* Table 2.3:  Block B1 format */
  *(uint32_t *)&stk48_blk1[0] = ((uint32_t)hdr0 << 16) | (0x0100);

  memset(&stk32_tbuf[0], 0x00, 16);

  /* CBC需要调用2+[PLEN/16]次AES算法，所以stk12_len=roundup(payload_len,16)+16+15 */
  r3_plen = payload_len + 0x10;
  if (payload_len & 0x0f)
  {
    r3_plen &= ~0x0f;
    r3_plen += 0x10;
  } /* end if (payload_len & 0x0f) */

  /* 格式化payload的块索引。*/
  stk4_blkidx = 0;
  /* 遍历需经过aes计算的字节数。*/
  r7_bytes = 0;
  /* B1有意义的长度: 必须是uint8_t */
  r6_b1len = 3;
  /* 需经过aes计算的字节数。*/
  stk12_len = r3_plen + 15;

  while (r7_bytes <= stk12_len) {
    /* 格式化后的Block(blk) 与上次aes计算后结果(tbuf)作XOR操作。 */
    for (idx=0; idx<16; idx++)
    { stk32_tbuf[idx] ^= stk16_blk[idx]; }

    /* aes(tbuf) */
    aes_encrypt_wrapper(NULL, &stk32_tbuf[0], &stk32_tbuf[0]);

    /* 是否对B1要特殊处理? 处理完后b1len=0, 下次就不按B1处理了。*/
    if ((uint8_t)(r6_b1len-1) <= 14)
    {
      /* B1长度<16: 格式化B1至blk，用于下次循环计算. */
      memcpy(&stk16_blk[0], &stk48_blk1[0], r6_b1len);
      memset(&stk16_blk[r6_b1len], 0x00, 16-r6_b1len);
      /* 对格式化payload的块索引。*/
      stk4_blkidx = 0;
    } /* if (r6_b1len-1 <= 14) */
    else
    {
      /* 格式化Blocki时，判断是否是最后一块？ */
      if (r4_plen > 15)
      {
        /* 不是最后一块。*/
        memcpy(&stk16_blk[0], &p_payload[stk4_blkidx*16], 16);
        stk4_blkidx++;
        r4_plen -= 16;
      } /* if (payload_len > 15) */
      else
      {
        /* 最后一块。*/
        memcpy(&stk16_blk[0], &p_payload[stk4_blkidx*16], r4_plen);
        memset(&stk16_blk[r4_plen], 0x00, 16-r4_plen);
      } /* end if (payload_len > 15) */
    } /* end if (r6_b1len-1 <= 14) */

    r7_bytes += 16;
    /* B1已计算完成。*/
    r6_b1len = 0;
    if (stk12_len < r7_bytes) break;
  } /* end while */

  /* 输出4字节计算好的plainMIC. */
  memcpy(p_mic, &stk32_tbuf[0], 4);
}

/**
 * @brief 使用CTR模式进行加密。
 * @note payload in 27-byte: Chiper = (P0^aes(A1)) || (P1^aes(A2)) || (plainMIC^aes(A0))
 *  Ai: Flags||Nonce||Counter
 *
 * @param [in] p_nonce     - CCM Nonce.
 *        [in/out] p_payload   - The plain/cipher payload in ble packet w/o header.
 *        [in] payload_len - The length of payload.
 *        [in] hdr0        - The 1st byte of header in ble packet, NESN,SN,MD bits are masked to 0.
 *        [in/out] p_mic   - The plain/cipher MIC in 4-byte, pointer to the end of payload.
 *        [in] direction   - 0 by slave, 1 by master. Unused, set in Nonce outside.
 */
void aes_ccm_ble_ctr_pdu(uint8_t *p_nonce, uint8_t *p_payload, uint16_t payload_len, uint8_t hdr0, uint8_t *p_mic, uint8_t direction)
{
  int idx, ii;
  int r5_bytes, r4_plen;
  uint16_t r6_pno;
  uint8_t stk12_ctr[16];
  uint8_t stk28_tbuf[16];
  memset(&stk12_ctr[0], 0x0, 16);
  memset(&stk28_tbuf[0], 0x0, 16);

  /* 先使用CTR0生成cipherMIC = plainMIC ^ aes(CTR0). */

  /* Table 2.4:  Block Ai format */
  stk12_ctr[0] = 0x01; //Flags
  memcpy(&stk12_ctr[1], p_nonce, 13);

  aes_encrypt_wrapper(NULL, &stk12_ctr[0], &stk28_tbuf[0]);

  for (idx=0; idx<4; idx++)
  {
    p_mic[idx] ^= stk28_tbuf[idx];
  }

  /* CTR1 */
  stk12_ctr[14] = 0x00;
  stk12_ctr[15] = 0x01;

  /* 按16字节向上对齐的payload长度。*/
  r4_plen = payload_len;
  if (r4_plen & 0x0f)
  {
    r4_plen &= ~0x0f;
    r4_plen += 16;
  }

  /* 对划分payload的每一明文块，用CTRi算出密文块，然后更新CTRi。*/
  /* packetCounter. */
  r6_pno = 1;
  /* 遍历需经过aes计算的字节数。*/
  r5_bytes = 0;
  while (r5_bytes < r4_plen)
  {
    /* tbuf = aes(CTRi) */
    aes_encrypt_wrapper(NULL, &stk12_ctr[0], &stk28_tbuf[0]);

    /* Cipher_i = Plain_i ^ aes(CTRi) */
    for (ii=0; ii<16; ii++)
    {
      p_payload[r5_bytes+ii] ^= stk28_tbuf[ii];

      /* 避免payload最后一块越界。*/
      if (r5_bytes+ii >= payload_len-1) break;
    } /* end for (ii) */

    /* 更新CTRi */
    r6_pno++;
    stk12_ctr[14] = r6_pno>>8;
    stk12_ctr[15] = r6_pno & 0xff;
    r5_bytes += 16;
  } /* end while (r5_bytes < r4_plen) */

  /* 返回值无用。*/
}

#endif /* EXE_NDA */
