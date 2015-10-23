/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  Basic arithmetic functions header file
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id: //srstech/srs_common/std_fxp/include/srs_math_api.h#9 $
 *  $Author: zesenz $
 *  $Date: 2011/02/16 $
********************************************************************************/

#ifndef __SRS_MATH_API_H__
#define __SRS_MATH_API_H__

#include "srs_typedefs.h"

//32x16=high 32-bit MSB:
//Note: acc and in32 cannot be the same variable
#define SRS_FXP32X16(acc, in32, in16) do{															\
									acc = (SRSInt32)(SRSUint16)(in32) * (SRSInt32)(in16);		\
									acc += 0x8000;													\
									acc >>= 16;														\
									acc += ((in32)>>16) * (SRSInt32)(in16);						\
								  }while(0)


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

SRSInt32  SRS_CountLeadingZeroes(SRSUint32 xIn);
SRSUint32 SRS_FxpSqrt(SRSUint32 xIn);
SRSUint32 SRS_FxpPow2_32(SRSUint32 x);
SRSUint32 SRS_FxpPow2_16(SRSUint16 x);
SRSInt32  SRS_FxpLog2(SRSUint32 x);
SRSInt32 SRS_CalRecipro(SRSInt32 d, int *iwl);

void	SRS_ApplyGain(SRSInt32 *audioIO, int blockSamples, SRSInt16 gain, int gainIwl);	//performs X*gain
void	SRS_ApplyGainWithAnticlip16(SRSInt32 *audioIO, int blockSamples, SRSInt16 gain);	//performs X*gain, then applies anticlip, using 16-bit anticlip table
void	SRS_ApplyGainWithAnticlip32(SRSInt32 *audioIO, int blockSamples, SRSInt16 gain);	//performs X*gain, then applies anticlip, using 32-bit anticlip table

void	SRS_MixAndScale(SRSInt32 *audioIO, SRSInt32 *y, int blockSamples, SRSInt16 gain, int gainIwl); //performs (AudioIO+Y)*gain

void	SRS_LeftShift8Bits(SRSInt32 *audio, int blockSamples);
void	SRS_RightShift8Bits(SRSInt32 *audio, int blockSamples);

void	SRS_PowXY(SRSInt32 *out, int *oiwl, SRSUint32 x, int xiwl, SRSInt32 y, int yiwl);

SRSInt32	SRS_Div(int oiwl, SRSInt32 x, int xiwl, SRSInt32 y, int yiwl);


#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif /*__SRS_MATH_API_H__*/

