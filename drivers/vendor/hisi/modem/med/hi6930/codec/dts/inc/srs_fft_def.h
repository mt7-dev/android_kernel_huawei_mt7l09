/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  Exposes all srs_fft APIs
 *
 *	Author: Zesen Zhuang
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_FFT_DEF_H__
#define __SRS_FFT_DEF_H__

#include "srs_typedefs.h"

/*Data type definition here:*/
typedef enum
{
	SRS_CFFT_32C16,
	SRS_RFFT_32C16,
	SRS_RFFT_16C16,
	SRS_FFT_UNUSED = 0x7FFFFFFF
} SRSFftType;

typedef struct{
	SRSInt16	*dig_rev_tbl;
	void		*twiddle_tbl;
	int			n;
	SRSInt32	*fftws;
	int			fft_type;
}SRSFftCfg;

typedef struct{
	int					FftSize;
	SRSFftType			Type;
}SRSFftCreateData;


typedef struct _SRSFftObj *SRSFftObj;

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

///////////////////////////////////////////////
int	SRS_Fft_GetPlanBufSize(int fftsize);
//int	SRS_Fft_GetLibSize(int fftsize, SRSFftType type);
SRSResult SRS_Fft_CreatePlan(void* pBuf, int fftsize, SRSFftType type);
SRSResult SRS_Fft_CreatePlanWithCfg(SRSFftCfg* pCfg, int fftsize, SRSFftType type);
SRSResult	SRS_Fft_DestroyPlan(int fftsize, SRSFftType type);
SRSBool		SRS_Fft_IsPlanCreated(int fftsize, SRSFftType type);

#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif //__SRS_FFT_DEF_H__
