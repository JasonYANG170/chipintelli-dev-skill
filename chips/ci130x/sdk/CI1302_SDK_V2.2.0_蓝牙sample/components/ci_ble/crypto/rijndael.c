/* Rijndael (AES) for GnuPG
 * Copyright (C) 2000, 2001, 2002, 2003, 2007,
 *               2008 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 *******************************************************************
 * The code here is based on the optimized implementation taken from
 * http://www.esat.kuleuven.ac.be/~rijmen/rijndael/ on Oct 2, 2000,
 * which carries this notice:
 *------------------------------------------
 * rijndael-alg-fst.c   v2.3   April '2000
 *
 * Optimised ANSI C code
 *
 * authors: v1.0: Antoon Bosselaers
 *          v2.0: Vincent Rijmen
 *          v2.3: Paulo Barreto
 *       2020/02: luwei refine for aes128 encrypt only.
 *
 * This code is placed in the public domain.
 *------------------------------------------
 *
 * The SP800-38a document is available at:
 *   http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for memcmp() */

//#include "types.h"  /* for byte and u32 typedefs */
typedef uint32_t u32;
typedef uint8_t  byte;

#define KC			4//(256/32)
#define ROUNDS		10//14

typedef struct
{
    union
    {
        uint32_t dummy;
        byte keyschedule[ROUNDS+1][4][4];
    } u1;
} RIJNDAEL_context;

#define keySched  u1.keyschedule

/* All the numbers.  */
#include "rijndael-tables.h"

static RIJNDAEL_context ctx_this;

/* Perform the key setup.  */  
static int
do_setkey (const byte *key)
{
    int i,j, r, t, rconpointer = 0;
    union
    {
        uint32_t dummy;
        byte k[KC][4];
    } k;
#define k k.k
    union
    {
        uint32_t dummy;
        byte tk[KC][4];
    } tk;
#define tk tk.tk  

    {
#define W (ctx_this.keySched)
        for (i = 0; i < 16; i++) 
        {
            k[i >> 2][i & 3] = key[i]; 
        }
      
        for (j = KC-1; j >= 0; j--) 
        {
            *((u32*)tk[j]) = *((u32*)k[j]);
        }
        r = 0;
        t = 0;
        /* Copy values into round key array.  */
        for (j = 0; (j < KC) && (r < ROUNDS + 1); )
        {
            for (; (j < KC) && (t < 4); j++, t++)
            {
                *((u32*)W[r][t]) = *((u32*)tk[j]);
            }
            if (t == 4)
            {
                r++;
                t = 0;
            }
        }
      
        while (r < ROUNDS + 1)
        {
            /* While not enough round key material calculated calculate
               new values.  */
            tk[0][0] ^= S[tk[KC-1][1]];
            tk[0][1] ^= S[tk[KC-1][2]];
            tk[0][2] ^= S[tk[KC-1][3]];
            tk[0][3] ^= S[tk[KC-1][0]];
            tk[0][0] ^= rcon[rconpointer++];
          
            if (KC != 8)
            {
                for (j = 1; j < KC; j++) 
                {
                    *((u32*)tk[j]) ^= *((u32*)tk[j-1]);
                }
            } 
            else 
            {
                for (j = 1; j < KC/2; j++)
                {
                    *((u32*)tk[j]) ^= *((u32*)tk[j-1]);
                }
                tk[KC/2][0] ^= S[tk[KC/2 - 1][0]];
                tk[KC/2][1] ^= S[tk[KC/2 - 1][1]];
                tk[KC/2][2] ^= S[tk[KC/2 - 1][2]];
                tk[KC/2][3] ^= S[tk[KC/2 - 1][3]];
                for (j = KC/2 + 1; j < KC; j++)
                {
                    *((u32*)tk[j]) ^= *((u32*)tk[j-1]);
                }
            }
          
            /* Copy values into round key array.  */
            for (j = 0; (j < KC) && (r < ROUNDS + 1); )
            {
                for (; (j < KC) && (t < 4); j++, t++)
                {
                    *((u32*)W[r][t]) = *((u32*)tk[j]);
                }
                if (t == 4)
                {
                    r++;
                    t = 0;
                }
            }
        }		
#undef W    
    }

    return 0;
#undef tk
#undef k
}


void aes128_setkey (const byte *key)
{
    do_setkey (key);
}

/* Encrypt one block.  A and B need to be aligned on a 4 byte
   boundary.  A and B may be the same. */
static void
do_encrypt_aligned (uint32_t *cipher, const uint32_t *plain)
{
#define rk (ctx_this.keySched)
    int r;
    union
    {
        u32  tempu32[4];  /* Force correct alignment. */
        byte temp[4][4];
    } u;

    u.tempu32[0] = plain[0] ^ *((u32*)rk[0][0]);
    u.tempu32[1] = plain[1] ^ *((u32*)rk[0][1]);
    u.tempu32[2] = plain[2] ^ *((u32*)rk[0][2]);
    u.tempu32[3] = plain[3] ^ *((u32*)rk[0][3]);
    cipher[0]               = (*((u32*)T1[u.temp[0][0]])
                          ^ *((u32*)T2[u.temp[1][1]])
                          ^ *((u32*)T3[u.temp[2][2]]) 
                          ^ *((u32*)T4[u.temp[3][3]]));
    cipher[1]               = (*((u32*)T1[u.temp[1][0]])
                          ^ *((u32*)T2[u.temp[2][1]])
                          ^ *((u32*)T3[u.temp[3][2]]) 
                          ^ *((u32*)T4[u.temp[0][3]]));
    cipher[2]               = (*((u32*)T1[u.temp[2][0]])
                          ^ *((u32*)T2[u.temp[3][1]])
                          ^ *((u32*)T3[u.temp[0][2]]) 
                          ^ *((u32*)T4[u.temp[1][3]]));
    cipher[3]               = (*((u32*)T1[u.temp[3][0]])
                          ^ *((u32*)T2[u.temp[0][1]])
                          ^ *((u32*)T3[u.temp[1][2]]) 
                          ^ *((u32*)T4[u.temp[2][3]]));

    for (r = 1; r < ROUNDS-1; r++)
    {
        u.tempu32[0] = cipher[0] ^ *((u32*)rk[r][0]);
        u.tempu32[1] = cipher[1] ^ *((u32*)rk[r][1]);
        u.tempu32[2] = cipher[2] ^ *((u32*)rk[r][2]);
        u.tempu32[3] = cipher[3] ^ *((u32*)rk[r][3]);

        cipher[0] = (*((u32*)T1[u.temp[0][0]])
                          ^ *((u32*)T2[u.temp[1][1]])
                          ^ *((u32*)T3[u.temp[2][2]]) 
                          ^ *((u32*)T4[u.temp[3][3]]));
        cipher[1] = (*((u32*)T1[u.temp[1][0]])
                          ^ *((u32*)T2[u.temp[2][1]])
                          ^ *((u32*)T3[u.temp[3][2]]) 
                          ^ *((u32*)T4[u.temp[0][3]]));
        cipher[2] = (*((u32*)T1[u.temp[2][0]])
                          ^ *((u32*)T2[u.temp[3][1]])
                          ^ *((u32*)T3[u.temp[0][2]]) 
                          ^ *((u32*)T4[u.temp[1][3]]));
        cipher[3] = (*((u32*)T1[u.temp[3][0]])
                          ^ *((u32*)T2[u.temp[0][1]])
                          ^ *((u32*)T3[u.temp[1][2]]) 
                          ^ *((u32*)T4[u.temp[2][3]]));
    }

    /* Last round is special. */
    u.tempu32[0] = cipher[0] ^ *((u32*)rk[ROUNDS-1][0]);
    u.tempu32[1] = cipher[1] ^ *((u32*)rk[ROUNDS-1][1]);
    u.tempu32[2] = cipher[2] ^ *((u32*)rk[ROUNDS-1][2]);
    u.tempu32[3] = cipher[3] ^ *((u32*)rk[ROUNDS-1][3]);
    cipher[ 0] = (T1[u.temp[3][3]][1]<<24) | (T1[u.temp[2][2]][1]<<16) | (T1[u.temp[1][1]][1]<<8) | T1[u.temp[0][0]][1];
    cipher[ 1] = (T1[u.temp[0][3]][1]<<24) | (T1[u.temp[3][2]][1]<<16) | (T1[u.temp[2][1]][1]<<8) | T1[u.temp[1][0]][1];
    cipher[ 2] = (T1[u.temp[1][3]][1]<<24) | (T1[u.temp[0][2]][1]<<16) | (T1[u.temp[3][1]][1]<<8) | T1[u.temp[2][0]][1];
    cipher[ 3] = (T1[u.temp[2][3]][1]<<24) | (T1[u.temp[1][2]][1]<<16) | (T1[u.temp[0][1]][1]<<8) | T1[u.temp[3][0]][1];

    cipher[0] ^= *((u32*)rk[ROUNDS][0]);
    cipher[1] ^= *((u32*)rk[ROUNDS][1]);
    cipher[2] ^= *((u32*)rk[ROUNDS][2]);
    cipher[3] ^= *((u32*)rk[ROUNDS][3]);
#undef rk
}

/**
 * @brief AES128 encrypt function with a single parameter which must be aligned to 4-byte.
 *
 * @param [in/out] p_data - The pointer to plain input and cipher output.
 */
void aes128_encrypt (uint32_t *p_data)
{
    uint32_t cipher[4];
    do_encrypt_aligned (cipher, p_data);
    memcpy(p_data, cipher, 16);
}

int aes_encrypt_wrapper(uint8_t *p_key, uint8_t *p_plain, uint8_t *p_cipher)
{
    do_encrypt_aligned ((uint32_t *)p_cipher, (uint32_t *)p_plain);
    /* remove later. */
    return 0;
}

#if 0
/* Run the self-tests for AES 128.  Returns NULL on success. */
const char*
selftest_basic_128 (void)
{
    unsigned char scratch[16];

    /* The test vectors are from the AES supplied ones; more or less
       randomly taken from ecb_tbl.txt (I=42,81,14) */
    static const unsigned char plaintext_128[16] = 
        {
            0x01,0x4B,0xAF,0x22,0x78,0xA6,0x9D,0x33,
            0x1D,0x51,0x80,0x10,0x36,0x43,0xE9,0x9A
        };
    static const unsigned char key_128[16] =
        {
            0xE8,0xE9,0xEA,0xEB,0xED,0xEE,0xEF,0xF0,
            0xF2,0xF3,0xF4,0xF5,0xF7,0xF8,0xF9,0xFA
        };
    static const unsigned char ciphertext_128[16] =
        {
            0x67,0x43,0xC3,0xD1,0x51,0x9A,0xB4,0xF2,
            0xCD,0x9A,0x78,0xAB,0x09,0xA5,0x11,0xBD
        };

    rijndael_setkey (key_128);
    rijndael_encrypt (scratch, plaintext_128);
    if (memcmp (scratch, ciphertext_128, sizeof (ciphertext_128)))
        fprintf(stderr, "AES-128 test encryption failed.");

    return NULL;
}
#endif
