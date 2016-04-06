/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/*
  ************************** WARNING ******************************
   Several pseudo-functions GSM_... provide faster operation than
   their equivalent true function, but may generate non-compatible
   results. It is known that the implementation of GSM_ADD_2 is
   not working correctly on 16-bit machines/compilers (besides
   an error-free compilation!). In the safer case, compilation
   should be carried out using SLOWER_BUT_SAFER defined.
   Another test is using add_test.c, explained elsewhere.
   -- <simao@cpqd.ansp.br> [08.Apr.94] ---
  *****************************************************************
*/

#ifndef __FR_DEC_H__
#define __FR_DEC_H__

#include "fr_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 *  More prototypes from implementations..
 */
extern void FR_DecInit(FR_DECODE_STATE_STRU *pstMedFrDecObj);

extern void Gsm_Decoder(
                FR_DECODE_STATE_STRU                       *pstFrDecState,
                Word16                                     *LARcr,              /* [0..7]       IN  */
                Word16                                     *Ncr,                /* [0..3]       IN  */
                Word16                                     *bcr,                /* [0..3]       IN  */
                Word16                                     *Mcr,                /* [0..3]       IN  */
                Word16                                     *xmaxcr,             /* [0..3]       IN  */
                Word16                                     *xMcr,               /* [0..13*4]        IN  */
                Word16                                     *s,                  /* [0..159]     OUT     */
                FR_TAF_STATUS_ENUM_UINT16                   enTafFlag,
                FR_SID_STATUS_ENUM_UINT16                   enSidFlag,
                FR_BFI_STATUS_ENUM_UINT16                   enBfiFlag);

extern void gsm_decode(FR_DECODE_STATE_STRU * s, Word16 * c, Word16 * target);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* __FR_DEC_H__ */
