#if EXE_NDA_SW_AES128_OPT

/*============================================================================*/
/* @file aes128_enc.c
 * @brief The optimized implemented AES128 algorithm, encryption only.
 * @author rijndael
 */

#include <stdint.h>
#include <string.h>

/* rijndael AES key. */
static uint32_t aes128_key[16/4];

static const uint8_t aes128_S[0x100] = {
   0x63, 0x7c, 0x77, 0x7b,
   0xf2, 0x6b, 0x6f, 0xc5,
   0x30, 0x01, 0x67, 0x2b,
   0xfe, 0xd7, 0xab, 0x76,
   0xca, 0x82, 0xc9, 0x7d,
   0xfa, 0x59, 0x47, 0xf0,
   0xad, 0xd4, 0xa2, 0xaf,
   0x9c, 0xa4, 0x72, 0xc0,
   0xb7, 0xfd, 0x93, 0x26,
   0x36, 0x3f, 0xf7, 0xcc,
   0x34, 0xa5, 0xe5, 0xf1,
   0x71, 0xd8, 0x31, 0x15,
   0x04, 0xc7, 0x23, 0xc3,
   0x18, 0x96, 0x05, 0x9a,
   0x07, 0x12, 0x80, 0xe2,
   0xeb, 0x27, 0xb2, 0x75,
   0x09, 0x83, 0x2c, 0x1a,
   0x1b, 0x6e, 0x5a, 0xa0,
   0x52, 0x3b, 0xd6, 0xb3,
   0x29, 0xe3, 0x2f, 0x84,
   0x53, 0xd1, 0x00, 0xed,
   0x20, 0xfc, 0xb1, 0x5b,
   0x6a, 0xcb, 0xbe, 0x39,
   0x4a, 0x4c, 0x58, 0xcf,
   0xd0, 0xef, 0xaa, 0xfb,
   0x43, 0x4d, 0x33, 0x85,
   0x45, 0xf9, 0x02, 0x7f,
   0x50, 0x3c, 0x9f, 0xa8,
   0x51, 0xa3, 0x40, 0x8f,
   0x92, 0x9d, 0x38, 0xf5,
   0xbc, 0xb6, 0xda, 0x21,
   0x10, 0xff, 0xf3, 0xd2,
   0xcd, 0x0c, 0x13, 0xec,
   0x5f, 0x97, 0x44, 0x17,
   0xc4, 0xa7, 0x7e, 0x3d,
   0x64, 0x5d, 0x19, 0x73,
   0x60, 0x81, 0x4f, 0xdc,
   0x22, 0x2a, 0x90, 0x88,
   0x46, 0xee, 0xb8, 0x14,
   0xde, 0x5e, 0x0b, 0xdb,
   0xe0, 0x32, 0x3a, 0x0a,
   0x49, 0x06, 0x24, 0x5c,
   0xc2, 0xd3, 0xac, 0x62,
   0x91, 0x95, 0xe4, 0x79,
   0xe7, 0xc8, 0x37, 0x6d,
   0x8d, 0xd5, 0x4e, 0xa9,
   0x6c, 0x56, 0xf4, 0xea,
   0x65, 0x7a, 0xae, 0x08,
   0xba, 0x78, 0x25, 0x2e,
   0x1c, 0xa6, 0xb4, 0xc6,
   0xe8, 0xdd, 0x74, 0x1f,
   0x4b, 0xbd, 0x8b, 0x8a,
   0x70, 0x3e, 0xb5, 0x66,
   0x48, 0x03, 0xf6, 0x0e,
   0x61, 0x35, 0x57, 0xb9,
   0x86, 0xc1, 0x1d, 0x9e,
   0xe1, 0xf8, 0x98, 0x11,
   0x69, 0xd9, 0x8e, 0x94,
   0x9b, 0x1e, 0x87, 0xe9,
   0xce, 0x55, 0x28, 0xdf,
   0x8c, 0xa1, 0x89, 0x0d,
   0xbf, 0xe6, 0x42, 0x68,
   0x41, 0x99, 0x2d, 0x0f,
   0xb0, 0x54, 0xbb, 0x16,
};

/* AES-128 in 10 rounds. */
static const uint8_t Rcon[10] = {
  0x01, 0x02, 0x04, 0x08,
  0x10, 0x20, 0x40, 0x80,
  0x1b, 0x36,
};


/* If msb is set xor with 0x1b otherwise just *2 */
// xtime is a macro that finds the product of {02} and the argument to
// xtime modulo {0x1B}
static uint8_t xtime(uint8_t x)
{
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}
/**
 * msb is set:     (x<<1) ^ 0x1B;
 * msb is not set: (x<<1)
 */
static uint8_t AES_xtime(uint8_t x)
{
  if (x & (1<<7))
  {
    return ((x<<1) ^ 0x1B);
  } /* */
  else
  {
    return (x << 1);
  } /* end if */
}

/**
 * @brief rijndaelAES算法，设置内部密钥。
 * @note API of AES128 encryption.
 *
 * @param [in] p_key - The pointer to 128-bit key in big endian.
 */
void aes128_setkey(uint8_t *p_key)
{
  memcpy(&aes128_key[0], p_key, 16);
}

/**
 * @brief rijndaelAES算法，加密计算的优化实现。
 * 2.2.1  Security function e
 * @note API of AES128 encryption.
 *
 * @param [in/out] p_data - The pointer to 128-bit plain/cipher text in big endian.
 */
void aes128_encrypt(uint32_t *p_wdata)
{
  int r3_ii, r4_idx;
  uint32_t r1_tmp;
  uint32_t r3_wdata;
  uint8_t r3_idx_s;
  uint32_t stk0_wbuf[6];
  int stk12_round;
  uint32_t *stk24_wptr;
  uint32_t stk28_wdata;
  uint32_t stk32_wbuf[4];
  uint32_t stk48_wkey[4];
  uint32_t r7_tmp;

  /* 优化复制key 16-byte。*/
  stk48_wkey[0] = aes128_key[0];
  stk48_wkey[1] = aes128_key[1];
  stk48_wkey[2] = aes128_key[2];
  stk48_wkey[3] = aes128_key[3];

  /* 优化xor data[] ^= key[] 16字节。*/
  p_wdata[0] ^= stk48_wkey[0];
  p_wdata[1] ^= stk48_wkey[1];
  p_wdata[2] ^= stk48_wkey[2];
  p_wdata[3] ^= stk48_wkey[3];
  {
  }

  stk12_round = 0;
  /* enumerate stk12_round in [0,10): coverage Rcon[stk12_round] */
  do {
    /* initial assign in 1st subblock in inner loop */
    stk24_wptr = &stk32_wbuf[0];

    /* enumerate r4_idx in [0,3) to assign stk32_wbuf[r4_idx] */
    for (r4_idx=0; r4_idx<4; r4_idx++)
    {
      r3_wdata = p_wdata[(0x03 & r4_idx)];
      r3_idx_s = (r3_wdata >> 0) & 0xff;
      stk0_wbuf[0] = aes128_S[r3_idx_s];

      /* 在inner loop最后inc，下面索引p_wdata[]时，就是r4_idx+1,+2,+3 */
      r3_wdata = p_wdata[0x03 & (r4_idx+1)];
      r3_idx_s = (r3_wdata >> 8) & 0xff;
      stk0_wbuf[1] = aes128_S[r3_idx_s];

      r3_wdata = p_wdata[0x03 & (r4_idx+2)];
      r3_idx_s = (r3_wdata >> 16) & 0xff;
      stk0_wbuf[2] = aes128_S[r3_idx_s];

      r3_wdata = p_wdata[0x03 & (r4_idx+3)];
      r3_idx_s = r3_wdata >> 24;
      r7_tmp = aes128_S[r3_idx_s];
      stk0_wbuf[3] = r7_tmp;
      /* assert(r3_idx_s < 0x100) */
      /* assert((r5_mask & (r4_idx+2)) < 4) */

      /* Last round is special. */
      if (stk12_round != 9)
      {
        uint32_t r0_tmp = stk0_wbuf[1] ^ stk0_wbuf[0];
        stk0_wbuf[5] = stk0_wbuf[2] ^ r7_tmp;
        stk0_wbuf[4] = stk0_wbuf[5] ^ r0_tmp;

        r0_tmp = AES_xtime(r0_tmp);
        stk28_wdata = stk0_wbuf[4] ^ stk0_wbuf[0] ^ r0_tmp;

        r0_tmp = AES_xtime(stk0_wbuf[2] ^ stk0_wbuf[1]);
        stk0_wbuf[1] = stk0_wbuf[4] ^ stk0_wbuf[1] ^ r0_tmp;

        r0_tmp = AES_xtime(stk0_wbuf[5]);
        stk0_wbuf[2] = stk0_wbuf[4] ^ stk0_wbuf[2] ^ r0_tmp;

        r0_tmp = AES_xtime(stk0_wbuf[0] ^ r7_tmp);
        r7_tmp = stk0_wbuf[4] ^ r7_tmp ^ r0_tmp;
        stk0_wbuf[3] = r7_tmp;
        stk0_wbuf[0] = stk28_wdata;
      } /* end if (stk12_round != 9) */

      /* *stk24_wptr++ = (stk0_wbuf[3]<<24) | (stk0_wbuf[2]<<16) | (stk0_wbuf[1]<<8) | (stk0_wbuf[0]<<0); */
      r7_tmp <<= 24;
      r7_tmp |= stk0_wbuf[2] << 16;
      r7_tmp |= stk0_wbuf[0];
      r7_tmp |= stk0_wbuf[1] << 8;
      /* stk24_wptr pointer to stk32_wbuf[4] */
      *stk24_wptr++ = r7_tmp;
    } /* end for (r4_idx) */

    for (r3_ii=0; r3_ii<4; r3_ii++)
    {
      r1_tmp = stk48_wkey[(r3_ii+3) & 0x03];
      /* r5_mask fsm: 0x03 -> 0xff -> 0xfe -> 0xfd -> 0xfe -> 0xff */
      if ((r3_ii & 0x03) != 0)
      {
        uint32_t r2_mask = 0x20;
        uint32_t r7_tmp = 0 << 8;
        /* enumerate r2_mask in 32,24,16,8, to generate r7_tmp in 4-byte. */
        do {
          r7_tmp <<= 8;
          r3_wdata = r1_tmp >> (r2_mask & 0x1f);
          stk0_wbuf[0] = r3_wdata;

          r7_tmp |= aes128_S[r3_wdata & 0xff];
          r2_mask -= 8;
        } while (r2_mask != 0);

        r1_tmp = Rcon[stk12_round] ^ r7_tmp;
      } /* end if ((r3_ii & 0x03) != 0) */

      stk48_wkey[r3_ii & 0x03] ^= r1_tmp;
    } /* end for (r3_ii) */

    /* generate cipher. */
    for (r3_ii=0; r3_ii<4; r3_ii++)
    {
      p_wdata[r3_ii] = stk48_wkey[r3_ii] ^ stk32_wbuf[r3_ii];
    } /* end for (r3_ii) */

    stk12_round++;
  } while (stk12_round != 10);

    /* 1st subblock in outer loop. */

}

/**
 * @brief 调用AES-128加密函数。
 * @note 所有的输入，输出都是128-bit, big endian; key使用以前的设置。
 *
 * @param [in] p_key     - The pointer to key, unused.
 *        [in] p_plain   - The pointer to plain text.
 *        [out] p_cipher - The pointer to cipher text.
 */
int aes_encrypt_wrapper(uint8_t *p_key, uint8_t *p_plain, uint8_t *p_cipher)
{
  memcpy(p_cipher, p_plain, 16);
  aes128_encrypt(p_cipher);
  /* remove later. */
  return 0;
}

#endif /* EXE_NDA */
