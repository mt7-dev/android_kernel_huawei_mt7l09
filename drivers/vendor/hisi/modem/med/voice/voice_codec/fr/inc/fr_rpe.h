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

#ifndef __FR_RPE_H__
#define __FR_RPE_H__

#include "fr_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 *  More prototypes from implementations..
 */

extern void Gsm_RPE_Encoding(
    Word16    * e,        /* -5..-1][0..39][40..44    IN/OUT  */
    Word16    * xmaxc,    /*              OUT */
    Word16    * Mc,       /*              OUT */
    Word16    * xMc,      /* [0..12]          OUT */
    Word16  * pshwDtxXmaxuq);

extern void Gsm_RPE_Decoding(
    Word16        xmaxcr,
    Word16        Mcr,
    Word16        * xMcr,  /* [0..12], 3 bits         IN  */
    Word16        * erp    /* [0..39]         OUT     */
);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* __FR_RPE_H__ */
