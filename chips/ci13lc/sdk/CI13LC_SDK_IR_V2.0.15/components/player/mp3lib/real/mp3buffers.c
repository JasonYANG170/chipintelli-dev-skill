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
 * mp3buffers.c - allocation and freeing of internal MP3 decoder buffers
 *
 * All memory allocation for the codec is done in this file, so if you don't want
 *  to use other the default system malloc() and free() for heap management this is
 *  the only file you'll need to change.
 **************************************************************************************/

// J.Sz. 21/04/2006 #include "hlxclib/stdlib.h"		/* for malloc, free */

#include "status_share.h"
#include "mp3coder.h"

#define MP3_USE_MALLOC   1
#if MP3_USE_MALLOC
	#if 0
	#include "FreeRTOS.h"
	#define MP3LIB_MALLOC pvPortMalloc
	#define MP3LIB_FREE   vPortFree
	#else
	#include <stdlib.h>
	#define MP3LIB_MALLOC //malloc
	#define MP3LIB_FREE   //free
	#endif
#else
	MP3DecInfo      s_mp3dec;
	FrameHeader     s_fh;
	SideInfo        s_si;
	ScaleFactorInfo s_sfi;
	HuffmanInfo     s_hi;
	DequantInfo     s_di;
	IMDCTInfo       s_mi;
    SubbandInfo     s_sbi;
#endif

/**************************************************************************************
 * Function:    ClearBuffer
 *
 * Description: fill buffer with 0's
 *
 * Inputs:      pointer to buffer
 *              number of bytes to fill with 0
 *
 * Outputs:     cleared buffer
 *
 * Return:      none
 *
 * Notes:       slow, platform-independent equivalent to memset(buf, 0, nBytes)
 **************************************************************************************/
_XIF_  void ClearBuffer(void *buf, int nBytes)
{
	int i;
	unsigned char *cbuf = (unsigned char *)buf;

	for (i = 0; i < nBytes; i++)
		cbuf[i] = 0;

}

/**************************************************************************************
 * Function:    AllocateBuffers
 *
 * Description: allocate all the memory needed for the MP3 decoder
 *
 * Inputs:      none
 *
 * Outputs:     none
 *
 * Return:      pointer to MP3DecInfo structure (initialized with pointers to all
 *                the internal buffers needed for decoding, all other members of
 *                MP3DecInfo structure set to 0)
 *
 * Notes:       if one or more mallocs fail, function frees any buffers already
 *                allocated before returning
 *
 *              Changed by Kasper Jepsen to support static buffers as well.
 *
 **************************************************************************************/
_XIF_  MP3DecInfo *AllocateBuffers(void)
{
  	MP3DecInfo *mp3DecInfo_pointer;

#if MP3_USE_MALLOC
	FrameHeader *fh;
	SideInfo *si;
	ScaleFactorInfo *sfi;
	HuffmanInfo *hi;
	DequantInfo *di;
	IMDCTInfo *mi;
	SubbandInfo *sbi;

#if 0
	mp3DecInfo_pointer = (MP3DecInfo *)MP3LIB_MALLOC(sizeof(MP3DecInfo));
	if (!mp3DecInfo_pointer)
		return 0;
	ClearBuffer(mp3DecInfo_pointer, sizeof(MP3DecInfo));

	fh =  (FrameHeader *)     MP3LIB_MALLOC(sizeof(FrameHeader));
	si =  (SideInfo *)        MP3LIB_MALLOC(sizeof(SideInfo));
	sfi = (ScaleFactorInfo *) MP3LIB_MALLOC(sizeof(ScaleFactorInfo));
	hi =  (HuffmanInfo *)     MP3LIB_MALLOC(sizeof(HuffmanInfo));
	di =  (DequantInfo *)     MP3LIB_MALLOC(sizeof(DequantInfo));
	mi =  (IMDCTInfo *)       MP3LIB_MALLOC(sizeof(IMDCTInfo));
    sbi = (SubbandInfo *)     MP3LIB_MALLOC(sizeof(SubbandInfo));

	if (!fh || !si || !sfi || !hi || !di || !mi || !sbi) {
		FreeBuffers(mp3DecInfo_pointer);	/* safe to call - only frees memory that was successfully allocated */
		return 0;
	}
#else
	mp3DecInfo_pointer = (MP3DecInfo *)ciss_get(CI_SS_NN_BIG_BUFFER_ADDR);
	ClearBuffer(mp3DecInfo_pointer, sizeof(MP3DecInfo));

	int size = sizeof(MP3DecInfo);
	fh =  (FrameHeader *)((uint32_t)mp3DecInfo_pointer + size);

	size = sizeof(FrameHeader);
	si =  (SideInfo *)((uint32_t)fh + size);

	size = sizeof(SideInfo);
	sfi = (ScaleFactorInfo *)((uint32_t)si + size);

	size = sizeof(ScaleFactorInfo);
	hi =  (HuffmanInfo *)((uint32_t)sfi + size);

	size = sizeof(HuffmanInfo);
	di =  (DequantInfo *)((uint32_t)hi + size);

	size = sizeof(DequantInfo);
	mi =  (IMDCTInfo *)((uint32_t)di + size);

	size = sizeof(IMDCTInfo);
    sbi = (SubbandInfo *)((uint32_t)mi + size);
#endif

	mp3DecInfo_pointer->FrameHeaderPS =     (void *)fh;
	mp3DecInfo_pointer->SideInfoPS =        (void *)si;
	mp3DecInfo_pointer->ScaleFactorInfoPS = (void *)sfi;
	mp3DecInfo_pointer->HuffmanInfoPS =     (void *)hi;
	mp3DecInfo_pointer->DequantInfoPS =     (void *)di;
	mp3DecInfo_pointer->IMDCTInfoPS =       (void *)mi;
	mp3DecInfo_pointer->SubbandInfoPS =     (void *)sbi;

    /* important to do this - DSP primitives assume a bunch of state variables are 0 on first use */
	//Optimized away.. hmm
    ClearBuffer(fh,  sizeof(FrameHeader));
	ClearBuffer(si,  sizeof(SideInfo));
	ClearBuffer(sfi, sizeof(ScaleFactorInfo));
	ClearBuffer(hi,  sizeof(HuffmanInfo));
	ClearBuffer(di,  sizeof(DequantInfo));
	ClearBuffer(mi,  sizeof(IMDCTInfo));
	ClearBuffer(sbi, sizeof(SubbandInfo));
    
#else
	mp3DecInfo_pointer = &s_mp3dec;
	mp3DecInfo_pointer->FrameHeaderPS =     (void *)&s_fh;
	mp3DecInfo_pointer->SideInfoPS =        (void *)&s_si;
	mp3DecInfo_pointer->ScaleFactorInfoPS = (void *)&s_sfi;
	mp3DecInfo_pointer->HuffmanInfoPS =     (void *)&s_hi;
	mp3DecInfo_pointer->DequantInfoPS =     (void *)&s_di;
	mp3DecInfo_pointer->IMDCTInfoPS =       (void *)&s_mi;
	mp3DecInfo_pointer->SubbandInfoPS =     (void *)&s_sbi;
#endif

	return mp3DecInfo_pointer;
}


/**************************************************************************************
 * Function:    FreeBuffers
 *
 * Description: frees all the memory used by the MP3 decoder
 *
 * Inputs:      pointer to initialized MP3DecInfo structure
 *
 * Outputs:     none
 *
 * Return:      none
 *
 * Notes:       safe to call even if some buffers were not allocated (uses SAFE_FREE)
 **************************************************************************************/
_XIF_  void FreeBuffers(MP3DecInfo *mp3DecInfo)
{
#if MP3_USE_MALLOC
    if (!mp3DecInfo)
		return;

	MP3LIB_FREE(mp3DecInfo->FrameHeaderPS);
	MP3LIB_FREE(mp3DecInfo->SideInfoPS);
	MP3LIB_FREE(mp3DecInfo->ScaleFactorInfoPS);
	MP3LIB_FREE(mp3DecInfo->HuffmanInfoPS);
	MP3LIB_FREE(mp3DecInfo->DequantInfoPS);
	MP3LIB_FREE(mp3DecInfo->IMDCTInfoPS);
	MP3LIB_FREE(mp3DecInfo->SubbandInfoPS);

	MP3LIB_FREE(mp3DecInfo);
#endif
}


/**************************************************************************************
 * Function:    ClearBuffers
 *
 * Description: clear all the memory used by the MP3 decoder
 *
 * Inputs:      pointer to initialized MP3DecInfo structure
 *
 * Outputs:     none
 *
 * Return:      none
 *
 * Notes:       modify by QQM
 **************************************************************************************/
_XIF_  void ClearBuffers(MP3DecInfo *mp3DecInfo)
{
	MP3DecInfo *mp3DecInfo_pointer = mp3DecInfo;
	
	if (!mp3DecInfo)
    {
        return;
    }

	void *fh  = mp3DecInfo->FrameHeaderPS;
	void *si  = mp3DecInfo->SideInfoPS;
	void *sfi = mp3DecInfo->ScaleFactorInfoPS; 
	void *hi  = mp3DecInfo->HuffmanInfoPS; 
	void *di  = mp3DecInfo->DequantInfoPS;
	void *mi  = mp3DecInfo->IMDCTInfoPS; 
	void *sbi = mp3DecInfo->SubbandInfoPS;


	ClearBuffer(fh,  sizeof(FrameHeader));
	ClearBuffer(si,  sizeof(SideInfo));
	ClearBuffer(sfi, sizeof(ScaleFactorInfo));
	ClearBuffer(hi,  sizeof(HuffmanInfo));
	ClearBuffer(di,  sizeof(DequantInfo));
	ClearBuffer(mi,  sizeof(IMDCTInfo));
	ClearBuffer(sbi, sizeof(SubbandInfo));
	ClearBuffer(mp3DecInfo_pointer, sizeof(MP3DecInfo));

	mp3DecInfo_pointer->FrameHeaderPS =     (void *)fh;
	mp3DecInfo_pointer->SideInfoPS =        (void *)si;
	mp3DecInfo_pointer->ScaleFactorInfoPS = (void *)sfi;
	mp3DecInfo_pointer->HuffmanInfoPS =     (void *)hi;
	mp3DecInfo_pointer->DequantInfoPS =     (void *)di;
	mp3DecInfo_pointer->IMDCTInfoPS =       (void *)mi;
	mp3DecInfo_pointer->SubbandInfoPS =     (void *)sbi;
}
