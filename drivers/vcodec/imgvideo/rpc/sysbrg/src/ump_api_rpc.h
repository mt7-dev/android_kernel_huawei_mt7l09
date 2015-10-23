/*!
 *****************************************************************************
 *
 * @file	   ump_api_rpc.h
 *
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) Imagination Technologies Ltd.
 * 
 * The contents of this file are subject to the MIT license as set out below.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * Alternatively, the contents of this file may be used under the terms of the 
 * GNU General Public License Version 2 ("GPL")in which case the provisions of
 * GPL are applicable instead of those above. 
 * 
 * If you wish to allow use of your version of this file only under the terms 
 * of GPL, and not to allow others to use your version of this file under the 
 * terms of the MIT license, indicate your decision by deleting the provisions 
 * above and replace them with the notice and other provisions required by GPL 
 * as set out in the file called �GPLHEADER� included in this distribution. If 
 * you do not delete the provisions above, a recipient may use your version of 
 * this file under the terms of either the MIT license or GPL.
 * 
 * This License is also included in this distribution in the file called 
 * "MIT_COPYING".
 *
 *****************************************************************************/

#ifndef __UMP_RPC_H__
#define __UMP_RPC_H__

#include "img_defs.h"
#include "sysbrg_api.h"
#include "ump_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	UMP_GetConfigData1_ID,
	UMP_ReadBlock1_ID,
	UMP_FlushPipe1_ID,
	UMP_PreemptRead1_ID,

} UMP_eFuncId;

typedef struct
{
	UMP_eFuncId	eFuncId;
    union
	{
	
		struct
		{
			 IMG_UINT32 ui32ConnId;
			 UMP_sConfigData __user * psConfigData;
	
		} sUMP_GetConfigData1Cmd;
	
		struct
		{
			 IMG_UINT32 ui32ConnId;
			 IMG_UINT32 ui32BufferSize;
			 IMG_VOID __user * pvData;
			 IMG_UINT32 __user * pui32DataRead;
			 IMG_BOOL __user * pbDataLost;
	
		} sUMP_ReadBlock1Cmd;
	
		struct
		{
			 IMG_UINT32 ui32ConnId;
	
		} sUMP_FlushPipe1Cmd;
	
		struct
		{
			 IMG_UINT32 ui32ConnId;
	
		} sUMP_PreemptRead1Cmd;
	
	} sCmd;
} UMP_sCmdMsg;

typedef struct
{
    union
	{
	
		struct
		{
			IMG_RESULT		xUMP_GetConfigData1Resp;
		} sUMP_GetConfigData1Resp;
	
		struct
		{
			IMG_RESULT		xUMP_ReadBlock1Resp;
		} sUMP_ReadBlock1Resp;
	
		struct
		{
			IMG_RESULT		xUMP_FlushPipe1Resp;
		} sUMP_FlushPipe1Resp;
	
		struct
		{
			IMG_RESULT		xUMP_PreemptRead1Resp;
		} sUMP_PreemptRead1Resp;
	
	} sResp;
} UMP_sRespMsg;



extern IMG_VOID UMP_dispatch(SYSBRG_sPacket __user *psPacket);

#ifdef __cplusplus
}
#endif

#endif
