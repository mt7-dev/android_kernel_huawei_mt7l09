/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/*$Header: /home/kbs/jutta/src/gsm/gsm-1.0/inc/RCS/private.h,v 1.2 1993/01/29 18:25:27 jutta Exp $*/

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

#ifndef __FR_LONG_TERM_H__
#define __FR_LONG_TERM_H__

#include "fr_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 *  More prototypes from implementations..
 */

extern void Gsm_Long_Term_Predictor(     /* 4x for 160 samples */
                Word8    fast,
                Word16    * d,    /* [0..39]   residual signal    IN  */
                Word16    * dp,   /* [-120..-1] d'        IN  */
                Word16    * e,    /* [0..40]          OUT */
                Word16    * dpp,  /* [0..40]          OUT */
                Word16    * Nc,   /* correlation lag      OUT */
                Word16    * bc    /* gain factor          OUT */);

extern void Gsm_Long_Term_Synthesis_Filtering(
                Word16    * pshwNrp,
                Word16    Ncr,
                Word16    bcr,
                Word16    * erp,      /* [0..39]        IN    */
                Word16    * drp);    /* [-120..-1] IN, [0..40] OUT   */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* __FR_LONG_TERM_H__ */
