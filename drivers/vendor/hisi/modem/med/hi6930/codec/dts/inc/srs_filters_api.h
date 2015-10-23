/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  The header file exposes all filter implementation APIs
 *
 *	Author: Oscar Huang
 *
 *$Id: //srstech/srs_common/std_fxp/include/srs_filters_api.h#6 $
 *$Author: oscarh $
 *$Date: 2010/08/12 $
********************************************************************************/

#ifndef __SRS_FILTERS_H__
#define __SRS_FILTERS_H__

#include "srs_typedefs.h"

//Data type definition here:


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

//API declaration here:

void	SRS_Iir2ndTdf2_c32s64(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt64 *state);
void	SRS_Iir2ndTdf2_c32s32(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir2ndTdf2_c16(SRSInt32 *inout, int nsize, const SRSInt16 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir1stTdf2_c32s64(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt64 *state);
void	SRS_Iir1stTdf2_c32s32(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir1stTdf2_c16(SRSInt32 *inout, int nsize, const SRSInt16 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir2ndDf1_c32(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir2ndDf1_c16(SRSInt32 *inout, int nsize, const SRSInt16 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir1stDf1_c32(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir1stDf1_c16(SRSInt32 *inout, int nsize, const SRSInt16 *coef, int coefIwl, SRSInt32 *state);
void	SRS_PhaseShift90(SRSStereoCh *input, SRSStereoCh *output, SRSInt32 *state, int blockSamples);

void	SRS_Iir2ndTdf2Cascaded_c32s64(SRSInt32 *inout, int blockSamples, const SRSInt32 *coef, SRSInt64 *state, int nSos);
void	SRS_Iir2ndTdf2Cascaded_c32s32(SRSInt32 *inout, int blockSamples, const SRSInt32 *coef, SRSInt32 *state, int nSos);
void	SRS_Iir2ndTdf2Cascaded_c16(SRSInt32 *inout, int blockSamples, const SRSInt16 *coef, SRSInt32 *state, int nSos);

void	SRS_FirFiltering(SRSInt32 *xIn, int blockSamples, const SRSInt32 *coefs, int taps, SRSInt32 *yOut, SRSInt32 *state);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__SRS_FILTERS_H__*/


