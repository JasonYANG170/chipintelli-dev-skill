/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0/RPSL 1.0 
 *  
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved. 
 *      
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Public Source License 
 * Version 1.0 (the "RPSL") available at 
 * http://www.helixcommunity.org/content/rpsl unless you have licensed 
 * the file under the RealNetworks Community Source License Version 1.0 
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl, 
 * in which case the RCSL will apply. You may also obtain the license terms 
 * directly from RealNetworks.  You may not use this file except in 
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks 
 * applicable to this file, the RCSL.  Please see the applicable RPSL or 
 * RCSL for the rights, obligations and limitations governing use of the 
 * contents of the file.  
 *  
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 *  
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 *    http://www.helixcommunity.org/content/tck 
 * 
 * Contributor(s): 
 *  
 * ***** END LICENSE BLOCK ***** */ 

/**************************************************************************************
 * Fixed-point MP3 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2003
 *
 * mp3assembly.h - assembly language functions and prototypes for supported platforms
 *
 * - inline rountines with access to 64-bit multiply results 
 * - x86 (_WIN32) and ARM (ARM_ADS, _WIN32_WCE) versions included
 * - some inline functions are mix of asm and C for speed
 * - some functions are in native asm files, so only the prototype is given here
 *
 * MULSHIFT32(x, y)    signed multiply of two 32-bit integers (x and y), returns top 32 bits of 64-bit result
 * FASTABS(x)          branchless absolute value of signed integer x
 * CLZ(x)              count leading zeros in x
 * MADD64(sum, x, y)   (Windows only) sum [64-bit] += x [32-bit] * y [32-bit]
 * SHL64(sum, x, y)    (Windows only) 64-bit left shift using __int64
 * SAR64(sum, x, y)    (Windows only) 64-bit right shift using __int64
 */

#ifndef _ASSEMBLY_H
#define _ASSEMBLY_H

#include "mp3dec.h"

/* toolchain:           ARM gcc
 * target architecture: ARM v7-m 
 */
#if defined(__GNUC__) && (defined(ARM) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__))

typedef long long Word64;

typedef union _U64 
{
    Word64 w64;
    struct 
    {
        unsigned int lo32;
        signed int   hi32;
    }r;
} U64;

#define MULSHIFT32	xmp3_MULSHIFT32
extern  int MULSHIFT32(int x, int y);

#define FASTABS	xmp3_FASTABS
extern  int FASTABS(int x);

static inline Word64 MADD64(Word64 sum64, int x, int y)
{
    U64 u;
    u.w64 = sum64;
    __asm__ volatile ("smlal %0,%1,%2,%3" : "+&r" (u.r.lo32), "+&r" (u.r.hi32) : "r" (x), "r" (y) : "cc");
    return u.w64;
}

static inline long long SAR64(long long x, int n)
{
	long long ret = x;
	ret = ret>>n;
	return ret;
}

#define CLZ(x)       __CLZ(x)

#define CLIPTO30(x)  __SSAT(x,30)


/* toolchain:           iar iccarm
 * target architecture: ARM v7-m 
 */
#elif defined(__ICCARM__)

typedef long long Word64;

#define MULSHIFT32	xmp3_MULSHIFT32
extern  int MULSHIFT32(int x, int y);

#define FASTABS	xmp3_FASTABS
extern  int FASTABS(int x);

static inline long long MADD64(long long sum, int x, int y)
{
	return (sum +(long long)x * y);
}

static inline long long SAR64(long long x, int n)
{
	long long ret = x;
	ret = ret>>n;
	return ret;
}

#define CLZ(x)       __CLZ(x)

#define CLIPTO30(x)  __SSAT(x,30)

#else

typedef long long Word64;

_XIF_ static inline int MULSHIFT32(int x, int y)
{
	int z;
	z = (Word64)x * (Word64)y >> 32;
	return z;
}

_XIF_ static inline  int FASTABS(int x)
{
	int sign;
	sign = x >> (sizeof(int) * 8 -1);
	x ^= sign;
	x -= sign;
	return x;
}

_XIF_ static inline long long MADD64(long long sum, int x, int y)
{
	return (sum +(long long)x * y);
}

_XIF_ static inline long long SAR64(long long x, int n)
{
	long long ret = x;
	ret = ret>>n;
	return ret;
}

_XIF_ static inline int CLZ(int x)
{
	int numZeros;

	if(!x)
		return 32;

	numZeros = 1;
	if(!((unsigned int)x >> 16)) { numZeros += 16; x <<= 16;}
	if(!((unsigned int)x >> 24)) { numZeros += 8; x <<= 8;}
	if(!((unsigned int)x >> 28)) { numZeros += 4; x <<= 4;}
	if(!((unsigned int)x >> 30)) { numZeros += 2; x <<= 2;}

	numZeros -= ((unsigned int)x >> 31);

	return numZeros;
}

_XIF_ static inline int CLIPTO30(int x)
{
	int sign;

	sign = x>>31;
	if(sign != (x >> 30))
		x = sign ^ ((1 << 30)-1);

	return x;
}

#endif	/* platforms */

#endif /* _ASSEMBLY_H */
