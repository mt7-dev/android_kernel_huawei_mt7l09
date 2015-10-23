/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  Exposes all srs_trmediahd APIs
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_TRMEDIAHD_API_H__
#define __SRS_TRMEDIAHD_API_H__

#include "srs_trmediahd_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

//Creates TruMedia HD Object:
int			SRS_TrMediaHD_GetObjSize(const SRSTrMediaHDCreateData *pAttr);
SRSResult	SRS_TrMediaHD_CreateObj(SRSTrMediaHDObj *pObj, void *pBuf, SRS_Allocator alloc,void* pContext, const SRSTrMediaHDCreateData *pAttr);

//Set TruMedia HD Control values:
void		SRS_TrMediaHD_SetControlDefaults(SRSTrMediaHDObj tmhdObj);

//Initialize TruMedia HD Object:
SRSResult		SRS_TrMediaHD_InitObj32k(SRSTrMediaHDObj tmhdObj, const SRSTrMediaHDFilterConfig *filterCfg);
SRSResult		SRS_TrMediaHD_InitObj44k(SRSTrMediaHDObj tmhdObj, const SRSTrMediaHDFilterConfig *filterCfg);
SRSResult		SRS_TrMediaHD_InitObj48k(SRSTrMediaHDObj tmhdObj, const SRSTrMediaHDFilterConfig *filterCfg);

//Stereo in, stereo out processing:
SRSResult SRS_TrMediaHD_Process(SRSTrMediaHDObj tmhdObj, SRSStereoCh *audioIO, int blockFrames, void *workSpace);


//"Set" functions:
SRSResult SRS_TrMediaHD_SetControls(SRSTrMediaHDObj tmhdObj, const SRSTrMediaHDControls *pCtrls); //Set all controls of TruMedia HD

SRSResult SRS_TrMediaHD_SetCshpControls(SRSTrMediaHDObj tmhdObj, const SRSTrMediaHDCshpControls *pCtrls); //Set all controls for CSHP
SRSResult SRS_TrMediaHD_SetWowhdxControls(SRSTrMediaHDObj tmhdObj, const SRSTrMediaHDWowhdxControls *pCtrls); //Set all controls for WOW HDX
SRSResult SRS_TrMediaHD_SetHdLmtControls(SRSTrMediaHDObj tmhdObj, const SRSTrMediaHDHdLmtControls *pCtrls); //Set all controls for Hard Limiter

SRSResult	SRS_TrMediaHD_SetCshpTBHDCustomSpeakerFilterCoefs(SRSTrMediaHDObj tmhdObj, const SRSInt32 *coefs); //Set custom TBHD coefficients for CSHP
SRSResult	SRS_TrMediaHD_SetWowhdxTBHDCustomSpeakerFilterCoefs(SRSTrMediaHDObj tmhdObj, const SRSInt32 *coefs); //Set custom TBHD coefficients for WOW HDX
SRSResult	SRS_TrMediaHD_SetGEQBandGain(SRSTrMediaHDObj tmhdObj, int bandIndex, SRSInt16 gain); //bandIndex: 0~9, gain: -15dB~+15dB, linear scale

//Utility 'set' functions for fast changing a single scalar control:
void	  SRS_TrMediaHD_SetEnable(SRSTrMediaHDObj tmhdObj, SRSBool enable); //enables/disables TruMedia HD processing

SRSResult SRS_TrMediaHD_SetInputGain(SRSTrMediaHDObj tmhdObj, SRSInt16 gain); //set the input Gain, I16.SRS_TRMEDIAHD_GAIN_IWL (I16.1)
SRSResult SRS_TrMediaHD_SetOutputGain(SRSTrMediaHDObj tmhdObj, SRSInt16 gain); //set the output Gain, I16.SRS_TRMEDIAHD_GAIN_IWL (I16.1)
SRSResult SRS_TrMediaHD_SetBypassGain(SRSTrMediaHDObj tmhdObj, SRSInt16 gain); //set the bypass Gain, I16.SRS_TRMEDIAHD_GAIN_IWL (I16.1)

SRSResult SRS_TrMediaHD_SetHeadrmGain(SRSTrMediaHDObj tmhdObj, SRSInt16 gain); //set the Headroom Gain, I16.SRS_TRMEDIAHD_GAIN_IWL (I16.1)
SRSResult SRS_TrMediaHD_SetMainTech(SRSTrMediaHDObj tmhdObj, SRSTrMediaHDMainTech tech); //set the main tech to use: CSHP/WOWHDX/NONE
void	  SRS_TrMediaHD_SetGEQEnable(SRSTrMediaHDObj tmhdObj, SRSBool enable); //enables/disables GEQ
void	  SRS_TrMediaHD_SetHPFEnable(SRSTrMediaHDObj tmhdObj, SRSBool enable); //enables/disables HPF
void	  SRS_TrMediaHD_SetTruEQEnable(SRSTrMediaHDObj tmhdObj, SRSBool enable); //enables/disables TruEQ
void	  SRS_TrMediaHD_SetHdLmtEnable(SRSTrMediaHDObj tmhdObj, SRSBool enable); //enables/disables HardLimiter

//"Get" functions:
void	SRS_TrMediaHD_GetControls(SRSTrMediaHDObj tmhdObj, SRSTrMediaHDControls *pCtrls); //Get all controls of TruMedia HD

void	SRS_TrMediaHD_GetCshpControls(SRSTrMediaHDObj tmhdObj, SRSTrMediaHDCshpControls *pCtrls); //Get all controls of CSHP
void	SRS_TrMediaHD_GetWowhdxControls(SRSTrMediaHDObj tmhdObj, SRSTrMediaHDWowhdxControls *pCtrls); //Get all controls of WOW HDX
void	SRS_TrMediaHD_GetHdLmtControls(SRSTrMediaHDObj tmhdObj, SRSTrMediaHDHdLmtControls *pCtrls); //Get all controls of HardLimiter

SRSInt16 SRS_TrMediaHD_GetGEQBandGain(SRSTrMediaHDObj tmhdObj, int bandIndex);

//Utility 'get' functions for fast querying a single scalar control:
SRSBool		SRS_TrMediaHD_GetEnable(SRSTrMediaHDObj tmhdObj);
SRSInt16	SRS_TrMediaHD_GetInputGain(SRSTrMediaHDObj tmhdObj);
SRSInt16	SRS_TrMediaHD_GetOutputGain(SRSTrMediaHDObj tmhdObj);
SRSInt16	SRS_TrMediaHD_GetBypassGain(SRSTrMediaHDObj tmhdObj);
SRSInt16	SRS_TrMediaHD_GetHeadrmGain(SRSTrMediaHDObj tmhdObj);
SRSTrMediaHDMainTech	SRS_TrMediaHD_GetMainTech(SRSTrMediaHDObj tmhdObj); 
SRSBool		SRS_TrMediaHD_GetGEQEnable(SRSTrMediaHDObj tmhdObj); 
SRSBool		SRS_TrMediaHD_GetHPFEnable(SRSTrMediaHDObj tmhdObj); 
SRSBool		SRS_TrMediaHD_GetTruEQEnable(SRSTrMediaHDObj tmhdObj); 
SRSBool		SRS_TrMediaHD_GetHdLmtEnable(SRSTrMediaHDObj tmhdObj); 


//Version and lib infor query:
unsigned char	SRS_TrMediaHD_GetTechVersion(SRSVersion which);
unsigned char	SRS_TrMediaHD_GetLibVersion(SRSVersion which);

const SRSLibInfo*  SRS_TrMediaHD_GetLibsInfo();

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__SRS_TRMEDIAHD_API_H__*/
