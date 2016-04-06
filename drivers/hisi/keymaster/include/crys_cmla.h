/*******************************************************************  
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents.                                            *
* Use of this Software as part of or with the Discretix CryptoCell *
* or Packet Engine products is governed by the products'           *
* commercial end user license agreement ("EULA").                  *
* It is possible that copies of this Software might be distributed *
* under some type of GNU General Public License ("GPL").           *
* Notwithstanding any such distribution under the terms of GPL,    *
* GPL does not govern the use of this Software as part of or with  *
* the Discretix CryptoCell or Packet Engine products, for which a  *
* EULA is required.                                                *
* If the product's EULA allows any copy or reproduction of this    *
* Software, then such copy or reproduction must include this       *
* Copyright Notice as well as any other notices provided           *
* thereunder.                                                      *
********************************************************************/

 
 
#ifndef CRYS_CMLA_H
#define CRYS_CMLA_H

  /*
   *  Object %CRYS_SMLA.h    : %
   *  State           :  %state%
   *  Creation date   :  Wed Aug 24 2006
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief This module contains the prototypes of routines performing SMLA schemes operations. 
   *         
   *   Implemented according to "CMLA Technical Specification". Version: v1.0-05-12-21,
   *   This material also contains confidential information which may not be disclosed 
   *   to others without the prior written consent of CMLA, LLC. 
   *     
   *  \version CRYS_SMLA.h#1:cincl:6
   *  \author  R.Levin.
   */

#include "DX_VOS_BaseTypes.h"
#include "CRYS_CMLA_error.h"
#include "crys_defs.h"
#include "CRYS_RSA_Types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CRYS_NO_CMLA_SUPPORT    
/************************ Defines ******************************/


/************************ Enums ********************************/


/************************ Typedefs  ****************************/

/* CRYS_CMLA common types definitions */
#define CRYS_CMLA_KEK_SIZE_IN_BYTES       16 
typedef DxUint8_t   CRYS_CMLA_KEK_t[CRYS_CMLA_KEK_SIZE_IN_BYTES];

/* CRYS_CMLA_KDF types definitions */
#define CRYS_CMLA_SEED_SIZE_IN_BYTES     128
#define CRYS_CMLA_KDF_TEMP_SIZE_IN_WORDS  80
typedef DxUint8_t   CRYS_CMLA_SEED_t[CRYS_CMLA_SEED_SIZE_IN_BYTES];
typedef DxUint32_t  CRYS_CMLA_KDF_TEMP_t[CRYS_CMLA_KDF_TEMP_SIZE_IN_WORDS];

/* CRYS_CMLA_WRAP types definitions */
#define CRYS_CMLA_UNWRAPPED_KEY_SIZE_IN_BYTES   32
#define CRYS_CMLA_WRAPPED_KEY_SIZE_IN_BYTES     40
typedef DxUint8_t CRYS_CMLA_UNWRAPPED_KEY_t[CRYS_CMLA_UNWRAPPED_KEY_SIZE_IN_BYTES];
typedef DxUint8_t CRYS_CMLA_WRAPPED_KEY_t[CRYS_CMLA_WRAPPED_KEY_SIZE_IN_BYTES];

/* CRYS_CMLA_RSA types definitions */
#define CRYS_CMLA_RSA_MESSAGE_LEN_BYTES  128
typedef DxUint8_t CRYS_CMLA_RSA_MESSAGE_t[CRYS_CMLA_RSA_MESSAGE_LEN_BYTES];


/************************ Structs  ******************************/

/* The CRYS_CMLA_RSA temp buffers type */

typedef struct CRYS_CMLA_RSA_TempBuff_t
{
   /* Temp buffer for CRYS_RSA primitives */ 
   CRYS_RSAPrimeData_t  RSA_TempBuff;
  
   /* Temp buffer for message, transformed by DDT_Exp function */
   DxUint8_t   DDTmessage[CRYS_CMLA_RSA_MESSAGE_LEN_BYTES];
  
   /* Temp buffer for little endian representation of previous defined message */
   DxUint8_t   DDTmessageLE[CRYS_CMLA_RSA_MESSAGE_LEN_BYTES];              
   
} CRYS_CMLA_RSA_TempBuff_t; 


/************************ Public Variables **********************/


/************************ Public Functions **********************/



/**************************************************************************
 *	              CRYS_CMLA_KDF function                                  *
 **************************************************************************/

CIMPORT_C  CRYSError_t CRYS_CMLA_KDF( CRYS_CMLA_SEED_t      X ,       /*in*/						
                                      CRYS_CMLA_KEK_t       KEK,      /*out*/
                                      CRYS_CMLA_KDF_TEMP_t  TempBuff  /*in*/ );
                                      


/**************************************************************************
 *	              CRYS_CMLA_Wrap function                                 *
 **************************************************************************/

CIMPORT_C  CRYSError_t CRYS_CMLA_Wrap(
                            CRYS_CMLA_KEK_t            KEK,    /*in*/   
                            CRYS_CMLA_UNWRAPPED_KEY_t  Key,    /*in*/
                            CRYS_CMLA_WRAPPED_KEY_t    WrapKey /*out*/ );  


/**************************************************************************
 *	              CRYS_CMLA_Unwrap function                               *
 **************************************************************************/

CIMPORT_C  CRYSError_t CRYS_CMLA_Unwrap(
                            CRYS_CMLA_KEK_t            KEK,      /*in*/   
                            CRYS_CMLA_WRAPPED_KEY_t    WrapKey,  /*in*/
                            CRYS_CMLA_UNWRAPPED_KEY_t  Key       /*out*/ );  
                            

/**************************************************************************
 *	              CRYS_CMLA_RSA_Encrypt function                          *
 **************************************************************************/

CIMPORT_C  CRYSError_t  CRYS_CMLA_RSA_Encrypt(
					                CRYS_RSAUserPubKey_t      *UserPublKey_ptr, /*in*/
					                CRYS_CMLA_RSA_MESSAGE_t    Message,         /*in*/
					                CRYS_CMLA_RSA_MESSAGE_t    EncrMessage,     /*out*/
					                CRYS_CMLA_RSA_TempBuff_t  *TempBuff_ptr     /*in */); 
  
                            
/**************************************************************************
 *	              CRYS_CMLA_RSA_Decrypt function                          *
 **************************************************************************/

CIMPORT_C  CRYSError_t  CRYS_CMLA_RSA_Decrypt(
					                CRYS_RSAUserPrivKey_t     *UserPrivKey_ptr, /*in*/
					                CRYS_CMLA_RSA_MESSAGE_t    EncrMessage,     /*in*/
					                CRYS_CMLA_RSA_MESSAGE_t    Message,         /*out*/
					                CRYS_CMLA_RSA_TempBuff_t  *TempBuff_ptr     /*in */); 
                            
                                      
#endif
#ifdef __cplusplus
}
#endif

#endif
